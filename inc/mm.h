#include <stdint.h>

typedef struct
{
    void* next;
    uint32_t bitmap_count;
    uint32_t* first_bitmap;
    uint32_t chunks_count;
    void* first_chunk;
} bitmap_list_t;