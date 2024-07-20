#include <stdio.h>

#include "video.h"
#include "serialport.h"
#include "x86.h"
#include "kshell.h"
#include "pckbd.h"
#include "ke.h"

multiboot_info_t* multiboot;

uint32_t ke_ticks = 0;
uint32_t ke_clock = 0;
uint32_t ke_systime = 0;

void sleep(uint32_t ms)
{
    uint32_t curtime = ke_clock;
    uint32_t tartime = curtime + ms;

    while(ke_clock < tartime)
        hlt();
}

void kernel_main(multiboot_info_t* _multiboot)
{
    multiboot = _multiboot;

    seg_init();
    idt_init();
    init_serial();
    fb_init();
    pckbd_init();
    //pit_init();
    //pit_setfreq(35);

    ke_systime = rtc_update();
    pg_init();
    sp_printf("Now in paging!:)\n");
    printf("Hi");
    sp_printf("Now im praying!:(\n");
    //kshell_main();
}