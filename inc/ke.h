#include <multiboot.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define INT16LOW(v) (((v) >> 8) & 0xFF)
#define INT16HIGH(v) ((v) & 0xFF)

void pit_init(uint32_t freq);
time_t rtc_update();

extern multiboot_info_t* multiboot;
extern uint32_t kernelcs;
extern uint32_t ke_ticks;
extern uint32_t ke_clock;
extern uint32_t ke_systime;

void sleep(uint32_t ms);