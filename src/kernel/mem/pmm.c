#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ke.h"
#include "mem.h"
#include "log.h"

#define UINT32BITS (32)

#define BSIZE sizeof(uint32_t)

#define calcsize(blocks, bitmaps) ((blocks) * BLOCK_SIZE + (bitmaps) * sizeof(uint32_t))

#define test_bit(val, bit) ((val & bit) != 0)
#define set_bit(val, bit) val |= bit
#define clear_bit(val, bit) val &= ~bit
#define next_bit(bit) bit <<= 1;
#define calc_pageaddr(bitmapi, bit) (((bitmapi) * UINT32BITS + bit) * BLOCK_SIZE)
#define test_bitno(num, bitn) ((num & (1 << bitn)))

mmap_entry_t bitmap_stack[BITMAP_STACK_SIZE];
uint32_t bitmap_stack_top = 0;

mmap_entry_t* firstblock = NULL;
struct multiboot_mmap_entry* main_block = NULL;

static inline uint32_t div_ceil(uint32_t num, uint32_t den) {
    return (num + den - 1) / den;
}

static inline uint32_t align_up(uint32_t address, uint32_t align) {
    return div_ceil(address, align) * align;
}

static inline uint32_t align_down(uint32_t address, uint32_t align) {
    return (address / align) * align;
}

static inline mmap_entry_t* alloc_bitmap_list()
{
	if(bitmap_stack_top >= BITMAP_STACK_SIZE)
		return NULL;
	
	return &bitmap_stack[bitmap_stack_top++];
}

// Binary search
static inline void calc_blocks_and_bitmaps(uint32_t size, uint32_t* num_blocks, uint32_t* num_bitmaps)
{
    uint32_t max_blocks = size / BLOCK_SIZE;
    uint32_t min_blocks = 0;
    uint32_t blocks = 0;
    uint32_t bitmaps = 0;

    for (int i = 31; i >= 0; --i)
    {
        uint32_t candidate_blocks = blocks | ((uint32_t)1 << i);

        if (candidate_blocks <= max_blocks)
        {
            bitmaps = div_ceil(candidate_blocks, UINT32BITS);

            if (calcsize(candidate_blocks, bitmaps) <= size)
                blocks = candidate_blocks;
        }
    }

    *num_blocks = blocks;
    *num_bitmaps = div_ceil(blocks, UINT32BITS);
}


static inline void block_setstart(struct multiboot_mmap_entry* block, uint32_t new)
{
    int32_t old = new - block->addr;
    block->addr = new;
    block->len = (old > 0) ? (block->len - (uint32_t)old) : ((block->len - (uint32_t)(-old)));
}

static inline void block_addstart(struct multiboot_mmap_entry* block, uint32_t inc)
{
	block_setstart(block, block->addr + inc);
}

static inline void block_setend(struct multiboot_mmap_entry* block, uint32_t new)
{
    block->len = new - block->addr;
}

static inline uint32_t block_getend(struct multiboot_mmap_entry* block)
{
    return block->addr + block->len;
}

static inline void block_cut(struct multiboot_mmap_entry* block, uint32_t cut_start, uint32_t cut_end)
{
    if(block->type == MULTIBOOT_MEMORY_RESERVED)
        return;
    
    if(block->addr_high != 0)
        return;

    if(cut_start > cut_end)
    {
        //exchange
        uint32_t temp = cut_start;
        cut_start = cut_end;
        cut_end = temp;
    }

    //if the block does not intersect the cutting range, do nothing
    if (block->addr >= cut_end || block_getend(block) <= cut_start)
        return;

    //if the block is entirely within the cutting range, mark it as reserved
    if ((block->addr >= cut_start) && (block_getend(block) <= cut_end)) {
        block->type = MULTIBOOT_MEMORY_RESERVED;
        return;
    }

    //if the cutting range overlaps the beginning of the block
    if (block->addr < cut_start && block_getend(block) > cut_start) {
        block_setend(block, cut_start);
    }

    //if the cutting range overlaps the end of the block
    if (block_getend(block) > cut_end && block->addr < cut_end) {
        block_setstart(block, cut_end);
    }

    if(block->len == 0)
        block->type = MULTIBOOT_MEMORY_RESERVED;
}

static inline void* block_alloc(struct multiboot_mmap_entry* block, uint32_t size)
{
    block_setend(block, block_getend(block) - size);
    return (void*)block_getend(block);
}

static inline uint32_t is_block_under1mb(struct multiboot_mmap_entry* block)
{
    return block_getend(block) < 0x100000;
}

static inline uint32_t is_partofblock_under1mb(struct multiboot_mmap_entry* block)
{
    return block_getend(block) >= 0x100000 && block->addr < 0x100000;
}

static inline void block_adjust(struct multiboot_mmap_entry* block)
{
    block_cut(block, 0, 0x100000);
    block_cut(block, (uint32_t)&vkernel_start, (uint32_t)&vkernel_end);

    //Align addres of memory block to page size
    uint32_t alignedaddr = align_up(block->addr, PAGE_SIZE);

    if(alignedaddr >= block->addr + block->len)
    {
        LOGDBG("Cut so much that no memory there is left\n");
        block->type = MULTIBOOT_MEMORY_RESERVED;
        return;
    }

    block_setstart(block, alignedaddr);
}

static inline mmap_entry_t* pmm_initmemblock(struct multiboot_mmap_entry* block)
{
    block_adjust(block);
    
    uint32_t blocks = block->len / BLOCK_SIZE;
    uint32_t bitmaps = div_ceil(blocks, UINT32BITS);

    if(block->type == MULTIBOOT_MEMORY_RESERVED)
        return NULL;
    
    mmap_entry_t* entry = alloc_bitmap_list();
    entry->blocks_count = blocks;
    entry->first_block = (void*)((uint32_t)block->addr);
    entry->first_bitmap = (uint32_t*)(block_alloc(main_block, bitmaps * sizeof(uint32_t)));
    entry->last_search = 0;
    entry->next = NULL;

    if(firstblock == NULL)
        firstblock = entry;

    return entry;
}

static inline mmap_entry_t* pmm_initmainblock()
{
    struct multiboot_mmap_entry* block = main_block;

    block_adjust(block);
    
    if(block->type == MULTIBOOT_MEMORY_RESERVED)
        return NULL;

    uint32_t blocks = 0;
    uint32_t bitmaps = 0;

    calc_blocks_and_bitmaps(block->len, &blocks, &bitmaps);
    
    mmap_entry_t* entry = alloc_bitmap_list();
    entry->blocks_count = blocks;
    entry->first_block = (void*)(align_up((uint32_t)block->addr + bitmaps * sizeof(uint32_t), BLOCK_SIZE));
    entry->first_bitmap = (uint32_t*)((uint32_t)block->addr);
    entry->last_search = 0;
    entry->next = NULL;

    if(firstblock == NULL)
        firstblock = entry;

    return entry;
}

static inline struct multiboot_mmap_entry* biggest_block_under_4gb()
{
    size_t ib = 0;
    size_t lb = 0;

    for (size_t i = 0; i < mb->mmap_length; i += sizeof(struct multiboot_mmap_entry))
    {
        struct multiboot_mmap_entry* me = (struct multiboot_mmap_entry*)(mb->mmap_addr + i);

        if(me->type == MULTIBOOT_MEMORY_AVAILABLE && me->addr_high == 0 && me->len > lb)
        {
            ib = i;
            lb = me->len;
        }
    }

    return lb ? (struct multiboot_mmap_entry*)(mb->mmap_addr + ib) : NULL;
}

void pmm_init()
{
    LOGDBG("vkernel_start:%x, vkernel_end:%x\n", &vkernel_start, &vkernel_end);


    LOG_STEP_START("Initialising PMM");

    if(!test_bitno(mb->flags, 6))
        panic("No memory map");

    main_block = biggest_block_under_4gb();
    pmm_initmainblock();

    
    
    mmap_entry_t* newblock = NULL;
    for (size_t i = 0; i < mb->mmap_length; i += sizeof(struct multiboot_mmap_entry))
    {
        struct multiboot_mmap_entry* me = (struct multiboot_mmap_entry*)(mb->mmap_addr + i);

        if(me->type == MULTIBOOT_MEMORY_AVAILABLE && me != main_block && me->len_high == 0)
        {
            mmap_entry_t* tempblock = pmm_initmemblock(me);

            if(!tempblock)
                continue;

            mmap_entry_t* oldblock = newblock;

            if(oldblock)
                oldblock->next = newblock;
        }

        LOGDBG("addr = %x%x, len = %x%x, size = %x, type = %i\n", (uint32_t)me->addr_high, (uint32_t)me->addr, (uint32_t)me->len_high, (uint32_t)me->len, me->size, me->type);
        LOGDBG("MB:%u\n", *((uint64_t*)&me->len) / 1048576);
    }

    LOG_STEP_END();
}

void* frame_alloc(uint32_t count, uint32_t align_up)
{
    if(count == 0 || count > 32)
        return NULL;
    
    if(align_up == 0)
        align_up = 1;

    mmap_entry_t* curblock = firstblock;
    while(1)
    {
        const uint32_t repeatcount = div_ceil(curblock->blocks_count, UINT32BITS);
        char isnotset_snsb = 1;
        for(uint32_t i = curblock->last_search; i < repeatcount; i++)
        {
            uint32_t* bitmap = &(curblock->first_bitmap)[i];

            if(*bitmap == 0xffffFFFF)
                continue;

            uint32_t bit = 1;
            uint32_t streak = 0;
            uint32_t firstj = 0;
            for(uint32_t j = 0; j < UINT32BITS; j++)
            {
                if(!streak) {
                    if(32 - j < count) {
                        break;
                    }

                    if(j % align_up != 0) {
                        continue;
                    }
                }

                if(!test_bit(*bitmap, bit))
                {
                    if(streak == 0) {
                        firstj = j;
                    }

                    streak++;

                    if(isnotset_snsb) {
                        curblock->last_search = i;
                        isnotset_snsb = 0;
                    }

                    if(streak == count)
                    {
                        j -= streak - 1;

                        bit = 1 << j;
                        for (uint32_t k = 0; k < streak; k++)
                        {
                            set_bit(*bitmap, bit);
                            next_bit(bit);
                        }
                        return (void*)((uintptr_t)(curblock->first_block) + calc_pageaddr(i, firstj));
                    }
                }
                else
                {
                    streak = 0;
                }
                next_bit(bit);
            }
        }

        if(curblock->next == NULL) {
            return NULL;
        }
        
        curblock = curblock->next;
    }
    return NULL;
}

void frame_free(void* addr, uint32_t count)
{
    uint32_t addrint = (uint32_t)addr;

    if(count == 0 || count > 32)
        return;

    mmap_entry_t* current_block = firstblock;
    while(1)
    {
        if(addrint >= (uint32_t)current_block->first_block && addrint <= (uint32_t)current_block->first_block + current_block->blocks_count)
        {
            addrint -= (uint32_t)current_block->first_block;
            addrint /= BLOCK_SIZE;
            uint32_t bitpos = addrint % 31;
            addrint -= bitpos;
            uint32_t bitmap_index = addrint / 32;
            uint32_t* bitmap = &(current_block->first_bitmap)[bitmap_index];
            uint32_t bitmask = 1 << bitpos;

            if(32 - bitpos < count) {
                return;
            }

            for (uint32_t i = 0; i < count; i++)
            {
                clear_bit(*bitmap, bitmask);
                next_bit(bitmask);
            }
            return;
        }

        if(current_block->next == NULL) {
            break;
        }
        
        current_block = current_block->next;
    }
    return;
}
