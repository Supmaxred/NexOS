#include <stdint.h>

typedef struct
{
    void* next;
    uint32_t bitmap_count;
    uint32_t* first_bitmap;
    uint32_t blocks_count;
    void* first_block;
} bitmap_list_t;

void mm_init(void);
void mm_initblock(struct multiboot_mmap_entry* block);
void* malloc();