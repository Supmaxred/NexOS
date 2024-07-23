#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "multiboot.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define INT16LOW(v) (((v) >> 8) & 0xFF)
#define INT16HIGH(v) ((v) & 0xFF)

//pit.c
void pit_setfreq(uint32_t freq);
void pit_init(void);
//cmos.c
time_t rtc_update();

extern multiboot_info_t* mb;
extern uint32_t ke_ticks;
extern uint32_t ke_clock;
extern uint32_t ke_systime;

extern uint32_t kernel_start;
extern uint32_t kernel_end;

void sleep(uint32_t ms);