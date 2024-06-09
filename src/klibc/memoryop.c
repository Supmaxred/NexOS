#include <stdint.h>

void memset(void* dst, uint8_t val, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        *(uint8_t*)(dst++) = val;
    }
}