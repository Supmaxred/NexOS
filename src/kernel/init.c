#include <stdio.h>

#include "global.h"
#include "video.h"
#include "serialport.h"
#include "i386/int.h"
#include "kshell.h"
#include "pckbd.h"
#include "pit.h"
#include "cpu.h"
#include "hal.h"

multiboot_info_t* multiboot;

uint32_t kernelcs;
uint32_t kernelds;

uint32_t ke_ticks = 0;
uint32_t ke_clock = 0;
uint32_t ke_systime = 0;

void sleep(uint32_t ms)
{
    uint32_t curtime = ke_clock;
    uint32_t tartime = curtime + ms;

    while(ke_clock < tartime)
        halt();
}

void kernel_main(uint32_t ds, uint32_t cs, multiboot_info_t* _multiboot)
{
    multiboot = _multiboot;
    kernelcs = cs;
    kernelds = ds;
    dbg_printf("cs:%x\n", cs);
    dbg_printf("ds:%x\n\n", ds);

    idt_init();
    init_serial();
    fb_init();
    pckbd_init();
    pit_init(50);

    ke_systime = rtc_update();

    kshell_main();
}