#include <stdio.h>

#include "video.h"
#include "serialport.h"
#include "x86.h"
#include "kshell.h"
#include "pckbd.h"
#include "ke.h"
#include "mem.h"
#include "log.h"

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

void panic(char* str)
{
    LOG("\nKernel panic: %s", str);

    cli();
    loop: hlt(); goto loop;
}

void kernel_main(multiboot_info_t* _multiboot)
{
    mb = _multiboot;

    init_serial();
    fb_init();
    gdt_init();
    idt_init();
    pckbd_init();
    pit_init();
    pit_setfreq(36);
    //panic("lox");

    ke_systime = rtc_update();

    //pmm_init();

    //pg_init();
    kshell_main();
}