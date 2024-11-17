#include <multiboot.h>
#include "x86.h"
#include "math.h"
#include "memblock.h"
#include "compiler.h"
#include "mem.h"
#include "ke.h"
#include "log.h"
#include "string.h"

void build_pgdir(multiboot_info_t* _multiboot)
{
    LOGDBG("HI!!!")
;
    multiboot_info_t* _multiboot_rm = (multiboot_info_t*)((uintptr_t)_multiboot + VIRTUAL_OFFSET);
    struct multiboot_mmap_entry* main_block = NULL;
    uint32_t max_len = 0;
    
    for (size_t i = 0; i < _multiboot_rm->mmap_length; i += sizeof(struct multiboot_mmap_entry))
    {
        struct multiboot_mmap_entry* me = (struct multiboot_mmap_entry*)((uintptr_t)(_multiboot_rm->mmap_addr + i) + VIRTUAL_OFFSET);
        if (me->type == MULTIBOOT_MEMORY_AVAILABLE && me->len > max_len && !is_block_under1mb(me)) {
            max_len = me->len;
            main_block = me;
        }
    }

    if (!main_block) {
        panic("Largest block not found(WTF)");
        return;
    }

    block_cut(main_block, 0, 0x100000);
    block_cut(main_block, (uint32_t)&vkernel_start, (uint32_t)&vkernel_end);

    //Align address of memory block to page size
    uint32_t alignedaddr = align_up(main_block->addr, PAGE_SIZE);

    if(alignedaddr >= main_block->addr + main_block->len)
    {
        LOGDBG("Cut so much that no memory there is left\n");
        main_block->type = MULTIBOOT_MEMORY_RESERVED;
        return;
    }

    block_setstart(main_block, alignedaddr);

    uint32_t num_pages = div_ceil((uint32_t)&pkernel_end, PAGE_SIZE);
    uint32_t num_page_tables = div_ceil(num_pages, PAGE_ENTRIES);

    uint32_t* pgdir = (uint32_t*)((uintptr_t)block_fsalloc(main_block, PAGE_SIZE) + VIRTUAL_OFFSET);
    memset(pgdir, 0, PAGE_SIZE);

    for (uint32_t i = 0; i < num_page_tables; i++) {
        uint32_t* pagetable = (uint32_t*)((uintptr_t)block_fsalloc(main_block, PAGE_SIZE) + VIRTUAL_OFFSET);
        memset(pagetable, 0, PAGE_SIZE);

        uint32_t pgdir_entry = ((uint32_t)pagetable - VIRTUAL_OFFSET) | PAGE_PRESENT | PAGE_WRITE;

        for (uint32_t j = 0; j < PAGE_ENTRIES; j++) {
            uint32_t page_num = i * PAGE_ENTRIES + j;

            if(page_num >= num_pages)
                break;

            uint32_t phys_addr = page_num * PAGE_SIZE;
            uint32_t virt_addr = VIRTUAL_OFFSET + phys_addr;
            
            pagetable[j] = phys_addr | PAGE_PRESENT | PAGE_WRITE;
            if (j == 0) {
                pgdir[GET_PDE_INDEX(virt_addr)] = pgdir_entry;
                pgdir[GET_PDE_INDEX(phys_addr)] = pgdir_entry;
            }
        }
    }
    
    lcr3((uint32_t*)((uintptr_t)pgdir - VIRTUAL_OFFSET));
    lcr0(gcr0() | CR0_PG | CR0_WP | CR0_NE | CR0_MP);
}