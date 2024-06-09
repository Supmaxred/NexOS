#include "i386/int.h"
#include <stdint.h>
#include <ctype.h>
#include "kbd_keymap.h"
#include "cpu.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define BUFSIZE 100

typedef struct _buffer
{
    char buffer[BUFSIZE];
    uint32_t start;
    uint32_t end;
}  buffer_t;

uint8_t is_disabled;
uint8_t is_shifted = 0;
buffer_t buf;

int pckbd_getch()
{
    if(buf.start == buf.end)
        return -1;
    
    char ret = buf.buffer[buf.start];
    buf.start = (buf.start + 1) % BUFSIZE;
    return ret;
}

void pckbd_flush()
{
    int c;
    while((c = pckbd_getch()) != -1);
}

void pckbd_putch(char c)
{
    if(buf.end + 1 == buf.start)
        pckbd_flush();
    
    buf.buffer[buf.end++] = c;
    buf.end = buf.end % BUFSIZE;
}

void pckbd_irqhandler(irqctx_t ctx)
{
    if(is_disabled)
        return;
    
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
        char scancode = inb(KEYBOARD_DATA_PORT);
        uint8_t key = scancode & ~-128;
        char character = keymap[key];

        if(is_shifted)
            character = toupper(keymap_up[key]);

        if(key == 0x2A)
            is_shifted = scancode >= 0;

        if(character == 0)
            return;
        
        if(scancode >= 0) 
            pckbd_putch(character);
	}
}

void pckbd_init()
{
    is_disabled = 1;
    idt_setirqhandler(1, pckbd_irqhandler, 1);
}

void pckbd_enable()
{
    is_disabled = 0;
}

void pckbd_disable()
{
    is_disabled = 0;
}