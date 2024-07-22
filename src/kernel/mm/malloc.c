#include <stdint.h>
#include "mm.h"
#include "ke.h"

#define BLOCK_SIZE 1024 // 1 KB
#define UINT32BITS (8 * sizeof(uint32_t))

#define PTSIZE 1024 * 3
#define BSIZE sizeof(uint32_t)

#define calcsize(chunks, bitmaps) ((chunks) * BLOCK_SIZE + (bitmaps) * sizeof(uint32_t))

#define test_bit(val, bit) ((val & bit) != 0)
#define set_bit(val, bit) val |= bit
#define next_bit(bit) bit <<= 1;
#define calc_pageaddr(bitmapi, bit) (((bitmapi << 3) * BSIZE + bit) << 12)

uint32_t* pagebm;
void* pagedata;

void calc_chunks_and_bitmaps(uint32_t size, uint32_t* num_chunks, uint32_t* num_bitmaps) {
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

void* malloc()
{
    uint32_t j = 0;
    for(uint32_t i = 0; i < PTSIZE / sizeof(uint32_t); i++)
    {
        uint32_t* bitmap = &pagebm[i];
        
        if(*bitmap == 0xffffFFFF)
            continue;
        
        uint32_t bit = 1;
        for( j = 0; j < UINT32BITS; j++)
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