#include <stdint.h>
#include "x86.h"
#include "ke.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

uint32_t pagedir[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint32_t pagetable[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint32_t pagetablefb[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

void mm_init()
{
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        pagetable[i] = (i << 12) | 3;
    }

    uint32_t fbaddr = (multiboot->framebuffer_addr >> 12) & 0xfff;
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        pagetablefb[i] = ((i + fbaddr) << 12) | 3;
    }

    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        pagedir[i] = 0;
    }

    pagedir[0] = ((uint32_t)pagetable) | 3;
    pagedir[multiboot->framebuffer_addr >> 22] = ((uint32_t)pagetablefb) | 3;

    lcr3(&pagedir);
    lcr0(gcr0() | 0x80000000);
}

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PTSIZE 1024 * 3
#define PGSIZE 0x1000
#define BSIZE sizeof(uint32_t)


uint32_t* pagebm;
void* pagedata;

void pageinit(uint32_t* meta, void* data)
{
    pagebm = meta;
    pagedata = data;
}    

uint32_t firstfree = 0;

void* palloc()
{
    uint32_t j = firstfree % (8 * BSIZE);
    for(uint32_t i = firstfree / (8 * BSIZE); i < PTSIZE / BSIZE; i++)
    {
        uint32_t* bitmap = &pagebm[i];
        
        if(*bitmap == 0xffffFFFF)
            goto end;
        
        uint32_t cursor = 1;
        for( ; j < 8 * BSIZE; j++)
        {
            if(!(*bitmap & cursor))
            {
                printf("finded!");
                *bitmap |= cursor;
                return (uintptr_t)pagedata + (i * 8 * BSIZE + j) * PGSIZE;
            }
            cursor <<= 1;
        }
        end:
        j = 0;
    }
    return NULL;
}