#include "i386/int.h"
#include "cpu.h"
#include "global.h"
#include "serialport.h"

uint32_t pit_delay = 1;
uint32_t pit_freq = 1;

static uint32_t counter = 0;

void pit_irqhandler(irqctx_t ctx)
{
    ke_ticks++;
    ke_clock += pit_delay;

    if(++counter >= pit_freq)
    {
        counter -= pit_freq;
        ke_systime++;
    }
}

void pit_init(uint32_t freq)
{
    counter = 0;

    idt_setirqhandler(0, pit_irqhandler, 1);

    // The value sent to the PIT is the value to divide its input clock
    // (1193180 Hz) by, to get the required frequency.
    // Important to note is that the divisor must be small enough to fit into 16-bits.
    uint16_t divisor = (uint16_t) (1193180 / freq);

    // The divisor must be sent 8 bits at a time
    uint8_t low = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);

    // Send the command byte
    outb(0x43, 0x36);

    // Send the frequency divisor
    outb(0x40, low);
    outb(0x40, high);

    pit_freq = freq;
    pit_delay = 1000 / freq;
}