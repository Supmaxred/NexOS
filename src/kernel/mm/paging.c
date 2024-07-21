#include <stdint.h>
#include "x86.h"
#include "ke.h"

#define PAGE_SIZE 0x1000
#define PAGE_ENTRIES 1024

#define PTSIZE 1024 * 3
#define BSIZE sizeof(uint32_t)

#define test_bit(val, bit) ((val & bit) != 0)
#define set_bit(val, bit) val |= bit
#define next_bit(bit) bit <<= 1;
#define calc_pageaddr(bitmapi, bit) (((bitmapi << 3) * BSIZE + bit) << 12)

uint32_t pagedir[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint32_t pagetable[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
uint32_t pagetablefb[PAGE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

void mm_init()
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

    lcr3(&pagedir);
    lcr0(gcr0() | 0x80000000);
}


uint32_t* pagebm;
void* pagedata;

void* palloc()
{
    uint32_t j = 0;
    for(uint32_t i = 0; i < PTSIZE / BSIZE; i++)
    {
        uint32_t* bitmap = &pagebm[i];
        
        if(*bitmap == 0xffffFFFF)
            continue;
        
        uint32_t bit = 1;
        for( j = 0; j < 8 * BSIZE; j++)
        {
            if(!test_bit(*bitmap, bit))
            {
                set_bit(*bitmap, bit);
                return (uintptr_t)pagedata + calc_pageaddr(i, j);
            }
            next_bit(bit);
        }
    }
    return NULL;
}