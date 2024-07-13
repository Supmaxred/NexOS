#include <stdint.h>

void* memset(void* dest, int val, uint32_t len)
{
    unsigned char* ptr = dest;
    unsigned char vval = val;
    while (len-- > 0)
        *ptr++ = vval;
    return dest;
}

void* memmove (void *dest, const void *src, uint32_t len)
{
    char *d = dest;
    const char *s = src;
    
    if (d < s)
    {
        while (len--)
            *d++ = *s++;
    }
    else
    {
        const char* lasts = s + (len-1);
        char* lastd = d + (len-1);
        while (len--)
            *lastd-- = *lasts--;
    }

    return dest;
}