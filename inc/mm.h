#include <stdint.h>

typedef struct
{
    void* next;
    uint32_t search_next_startblock;
    uint32_t* first_bitmap;
    uint32_t blocks_count;
    void* first_block;
} bitmap_list_t;

void mm_init(void);
void mm_initblock(struct multiboot_mmap_entry* block);
void* malloc(uint32_t count, uint32_t align_up);
void mfree(void* addr, uint32_t count);