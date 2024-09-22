#include <stdint.h>

#define BITMAP_STACK_SIZE 64
#define BLOCK_SIZE 4096
#define PAGE_SIZE 4096

typedef struct
{
    void* next;
    uint32_t search_next_startblock;
    uint32_t* first_bitmap;
    uint32_t blocks_count;
    void* first_block;
} bitmap_list_t;

void pmm_init(void);
void pmm_initblock(struct multiboot_mmap_entry* block);
void* malloc(uint32_t count, uint32_t align_up);
void mfree(void* addr, uint32_t count);