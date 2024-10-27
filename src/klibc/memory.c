#include <stdint.h>

void* memset(void* dest, int val, uint32_t len)
{
    uint8_t* ptr = dest;
    uint16_t vval = (uint8_t)val | ((uint8_t)val << 8);
    uint32_t vvval = vval | (vval << 16);
    
    while (len >= 4)
    {
        *(uint32_t*)ptr = vvval;
        ptr += 4;
        len -= 4;
    }
    
    if (len >= 2)
    {
        *(uint16_t*)ptr = vval;
        ptr += 2;
        len -= 2;
    }
    
    if (len > 0)
    {
        *ptr = (uint8_t)val;
    }

    return dest;
}

void* memmove(void* dest, const void* src, uint32_t len)
{
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    if (d < s)
    {
        while (len >= 4)
        {
            *(uint32_t*)d = *(const uint32_t*)s;
            d += 4;
            s += 4;
            len -= 4;
        }
        
        if (len >= 2)
        {
            *(uint16_t*)d = *(const uint16_t*)s;
            d += 2;
            s += 2;
            len -= 2;
        }
        
        if (len > 0)
        {
            *d = *s;
        }
    }
    else
    {
        d += len;
        s += len;
        
        while (len >= 4)
        {
            d -= 4;
            s -= 4;
            *(uint32_t*)d = *(const uint32_t*)s;
            len -= 4;
        }

        if (len >= 2)
        {
            d -= 2;
            s -= 2;
            *(uint16_t*)d = *(const uint16_t*)s;
            len -= 2;
        }

        if (len > 0)
        {
            d--;
            s--;
            *d = *s;
        }
    }

    return dest;
}