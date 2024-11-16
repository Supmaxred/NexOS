#pragma once
#include <multiboot.h>
#include <stdint.h>

static inline void block_setstart(struct multiboot_mmap_entry* block, uint32_t newa)
{
    int32_t old = newa - block->addr;
    block->addr = newa;
    block->len = (old > 0) ? (block->len - (uint32_t)old) : ((block->len - (uint32_t)(-old)));
}

static inline void block_addstart(struct multiboot_mmap_entry* block, uint32_t inc)
{
	block_setstart(block, block->addr + inc);
}

static inline void block_setend(struct multiboot_mmap_entry* block, uint32_t newa)
{
    block->len = newa - block->addr;
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

static inline void* block_fsalloc(struct multiboot_mmap_entry* block, uint32_t size)
{
    block_addstart(block, size);
    return (void*)(block->addr - size);
}

static inline void* block_fealloc(struct multiboot_mmap_entry* block, uint32_t size)
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
