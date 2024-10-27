#include <stdint.h>
#include <stddef.h>
#include <time.h>

#include "multiboot.h"

#define INT16LOW(v) (((v) >> 8) & 0xFF)
#define INT16HIGH(v) ((v) & 0xFF)

//pit.c
void pit_setfreq(uint32_t freq);
void pit_init(void);
//cmos.c
time_t rtc_update();

extern multiboot_info_t* mb;
extern uint32_t ke_ticks;
extern uint64_t ke_uptimens;
extern uint32_t ke_systime;

extern uint32_t vkernel_start;
extern uint32_t vkernel_end;

void sleepms(uint32_t ms);

void panic(char* str);