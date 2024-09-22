#include <stdint.h>
#include "x86.h"
#include "ke.h"

#define PAGE_SIZE 0x1000
#define PAGE_ENTRIES 1024

#define PTSIZE 1024 * 3
#define BSIZE sizeof(uint32_t)


uint32_t pagedir[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint32_t pagetable[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint32_t pagetablefb[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

void pg_init()
{
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        pagetable[i] = (i << 12) | 3;
    }

    uint32_t fbaddr = (mb->framebuffer_addr >> 12) & 0xfff;
    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        pagetablefb[i] = ((i + fbaddr) << 12) | 3;
    }

    for (int i = 0; i < PAGE_ENTRIES; i++)
    {
        pagedir[i] = 0;
    }

    pagedir[0] = ((uint32_t)pagetable) | 3;
    pagedir[mb->framebuffer_addr >> 22] = ((uint32_t)pagetablefb) | 3;

    lcr3((uint32_t*)&pagedir);
    lcr0(gcr0() | 0x80000000);
}
