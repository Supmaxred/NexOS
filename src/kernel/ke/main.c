#include <stdio.h>

#include "video.h"
#include "serialport.h"
#include "x86.h"
#include "kshell.h"
#include "pckbd.h"
#include "ke.h"
#include "mem.h"

multiboot_info_t* mb;

uint32_t ke_ticks = 0;
uint64_t ke_uptimens = 0;
uint32_t ke_systime = 0;

void sleepns(uint32_t ns)
{
    uint64_t tartime = ke_uptimens + ns;

    while(ke_uptimens < tartime)
        hlt();
}

void sleepms(uint32_t ms)
{
    uint64_t ns = (uint64_t)ms * 1000000;

    uint64_t tartime = ke_uptimens + ns;

    while(ke_uptimens < tartime)
        hlt();
}

int testbit(int num, int bit) {
    return (num & (1 << bit)) != 0;
}

extern uint32_t kernel_start;
extern uint32_t kernel_end;

void kernel_main(multiboot_info_t* _multiboot)
{
    mb = _multiboot;

    fb_init();
    gdt_init();
    idt_init();
    init_serial();
    pckbd_init();
    pit_init();
    pit_setfreq(36);

    ke_systime = rtc_update();

    pmm_init();

    //pg_init();
    kshell_main();
}