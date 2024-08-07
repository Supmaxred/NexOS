#include "x86.h"
#include "ke.h"
#include "serialport.h"
#include "video.h"

#define PITPORT_CH0 0x40
#define PITPORT_CH1 0x41
#define PITPORT_CH2 0x42
#define PITPORT_CMD 0x43

//Channel
#define PITCMD_CH0  (0 << 6)
#define PITCMD_CH1  (1 << 6)
#define PITCMD_CH2  (2 << 6)
#define PITCMD_RB   (3 << 6) //Read Back

//Access mode
#define PITCMD_LATCHCOUNT  (0 << 4)
#define PITCMD_LOBYTE       (1 << 4)
#define PITCMD_HIBYTE       (2 << 4)

//Operating mode
#define PITCMD_MODE0 (0 << 1) //Interrupt on terminal count
#define PITCMD_MODE1 (1 << 1) //Hardware re-triggerable one-shot
#define PITCMD_MODE2 (2 << 1) //Rate generator
#define PITCMD_MODE3 (3 << 1) //Square wave generator
#define PITCMD_MODE4 (4 << 1) //Software triggered strobe
#define PITCMD_MODE5 (5 << 1) //Hardware triggered strobe

//Mode
#define PITCMD_BINARYMODE  (0)
#define PITCMD_BCDMODE     (1)

#define PIT_BASEFREQUENCY 1193182
#define PIT_DELAYNS 838 //1 000 000 000 / PIT_BASEFREQUENCY

#define S2NS 1000000000 //1 000 000 000

uint32_t pit_freq = 1;
uint32_t pit_delayns;

static uint32_t counterns = 0;

void pit_irqhandler(irqctx_t* ctx)
{
    ke_ticks++;

    ke_uptimens += pit_delayns;

    counterns += pit_delayns;
    //printf("%u\n", counterns);
    if(counterns >= S2NS)
    {
        counterns -= S2NS;
        ke_systime++;
        fb_togglecursor();
    }
}

void pit_t0countdown_set(uint32_t val)
{
    
}

void pit_setfreq(uint32_t hz)
{
    uint16_t divisor = PIT_BASEFREQUENCY / hz;

    outb(PITPORT_CMD, PITCMD_BINARYMODE | PITCMD_MODE3 | (PITCMD_LOBYTE | PITCMD_HIBYTE));

    outb(PITPORT_CH0, INT16LOW(divisor));
    outb(PITPORT_CH0, INT16HIGH(divisor));

    pit_freq = hz;
    pit_delayns = PIT_DELAYNS * divisor;
    printf("%u\n", pit_delayns);
}

void pit_init()
{
    counterns = 0;

    idt_setirqhandler(0, pit_irqhandler, 1);
}