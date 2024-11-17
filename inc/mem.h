#pragma once
#include <stdint.h>
#include "linker.h"

#define BITMAP_STACK_SIZE 64
#define PAGE_SIZE 0x1000
#define PAGE_ENTRIES 1024
#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

typedef struct
{
    void* next;
    uint32_t last_search;
    uint32_t* first_bitmap;
    uint32_t blocks_count;
    void* first_block;
} mmap_entry_t;

void pmm_init(void);
void pmm_initblock(struct multiboot_mmap_entry* block);
void* malloc(uint32_t count, uint32_t align_up);
void mfree(void* addr, uint32_t count);