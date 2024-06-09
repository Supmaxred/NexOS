#include <stdio.h>

#include "global.h"
#include "video.h"
#include "serialport.h"
#include "i386/int.h"
#include "kshell.h"
#include "pckbd.h"
#include "pit.h"
#include "cpu.h"

multiboot_info_t* multiboot;
uint32_t kernelcs;
uint32_t kernelds;
uint32_t kernel_ticks = 0;

void sleep(uint32_t ms)
{
    uint32_t curticks = kernel_ticks;
    uint32_t tarticks = curticks + ms;

    while(kernel_ticks < tarticks)
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
    pit_init(1000);

    kshell_main();
}