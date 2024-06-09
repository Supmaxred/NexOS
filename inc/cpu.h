#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" :: "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory"); 
    return ret;
}

static inline uint8_t halt()
{
    __asm__ volatile ("hlt");
}

static inline void io_wait(void)
{
    outb(0x80, 0);
}