#include <stdint.h>
#include "mm.h"
#include "ke.h"

#define BLOCK_SIZE 1024 // 1 KB
#define PAGE_SIZE 4096 // 4 KB
#define UINT32BITS (8 * sizeof(uint32_t))

#define BSIZE sizeof(uint32_t)

#define calcsize(chunks, bitmaps) ((chunks) * BLOCK_SIZE + (bitmaps) * sizeof(uint32_t) + sizeof(bitmap_list_t))

#define test_bit(val, bit) ((val & bit) != 0)
#define set_bit(val, bit) val |= bit
#define next_bit(bit) bit <<= 1;
#define calc_pageaddr(bitmapi, bit) (((bitmapi << 3) * sizeof(uint32_t) + bit) << 12)

bitmap_list_t* firstblock = NULL;

uint32_t* pagebm;
void* pagedata;

uint32_t align_up(uint32_t address, uint32_t align)
{
    if (address % align == 0)
        return address;

    return (address + align - 1) & ~(align - 1);
}

void calc_chunks_and_bitmaps(uint32_t size, uint32_t* num_chunks, uint32_t* num_bitmaps)
{
    uint32_t chunks = size / BLOCK_SIZE;
    uint32_t bitmaps = chunks / UINT32BITS + 1;

    while (1)
    {
        if (calcsize(chunks, bitmaps) <= size)
            break;

        chunks--;
        bitmaps = chunks / UINT32BITS + 1;
    }

    *num_chunks = chunks;
    *num_bitmaps = bitmaps;
}

static void block_setstart(struct multiboot_mmap_entry* block, uint32_t new)
{
    uint32_t old = block->addr;
    block->addr = new;
    block->len = block->len + (new - old);
}

static void block_setend(struct multiboot_mmap_entry* block, uint32_t new)
{
    block->len = new - block->addr;
}

void** mm_initmemblock(struct multiboot_mmap_entry* block)
{
    if(block->addr < 0x100000)
    {
        block->type = MULTIBOOT_MEMORY_RESERVED;
        return NULL;
    }

    uint32_t blockend = block->addr + block->len;

    char b = block->addr > &kernel_start;
    char t = blockend < &kernel_end;

    if(b && t || t)
    {
        block_setstart(block, &kernel_end);
    }
    else if(b)
    {
        block_setend(block, &kernel_start);
    }

    uint32_t alignedaddr = align_up(block->addr, PAGE_SIZE);

    if(alignedaddr >= block->addr + block->len)
    {
        //🙂
        block->type = MULTIBOOT_MEMORY_RESERVED;
        return NULL;
    }

    block_setstart(block, alignedaddr);

    uint32_t chunks = 0;
    uint32_t bitmaps = 0;

    calc_chunks_and_bitmaps(block->len, &chunks, &bitmaps);

    bitmap_list_t* entry = block->addr + chunks * BLOCK_SIZE + bitmaps * sizeof(uint32_t);
    entry->bitmap_count = bitmaps;
    entry->chunks_count = chunks;
    entry->first_chunk = block->addr;
    entry->first_bitmap = block->addr + chunks * BLOCK_SIZE;
    entry->next = NULL;

    if(firstblock == NULL)
        firstblock = entry;

    return &entry->next;
}

void* malloc()
{
    bitmap_list_t* curlist = firstblock;
    while(1)
    {
        for(uint32_t i = 0; i < curlist->bitmap_count; i++)
        {
            uint32_t* bitmap = &(curlist->first_bitmap)[i];

            if(*bitmap == 0xffffFFFF)
                continue;

            uint32_t bit = 1;
            for( uint32_t j = 0; j < UINT32BITS; j++)
            {
                if(!test_bit(*bitmap, bit))
                {
                    set_bit(*bitmap, bit);
                    return (uintptr_t)pagedata + calc_pageaddr(i, j);
                }
                next_bit(bit);
            }
        }

        if(curlist->next == NULL)
            return NULL;
        
        curlist = curlist->next;
    }
    return NULL;
}

void mm_init()
{
    if(testbit(mb->flags, 6))
    {
        void** nextvar = NULL;
        for (size_t i = 0; i < mb->mmap_length; i += sizeof(struct multiboot_mmap_entry))
        {
            struct multiboot_mmap_entry* me = (struct multiboot_mmap_entry*)(mb->mmap_addr + i);

            if(nextvar != NULL)
                *nextvar = me;
            
            nextvar = mm_initmemblock(me);
        }
    }
    else
    {
        //🙂
        return;
    }
}