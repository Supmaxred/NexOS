#include <stdio.h>

#include "video.h"
#include "serialport.h"
#include "x86.h"
#include "kshell.h"
#include "pckbd.h"
#include "ke.h"
#include "mm.h"

multiboot_info_t* mb;

uint32_t ke_ticks = 0;
uint64_t ke_uptimens = 0;
uint32_t ke_systime = 0;

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

    seg_init();
    idt_init();
    init_serial();
    fb_init();
    pckbd_init();
    pit_init();
    pit_setfreq(50);

    ke_systime = rtc_update();

    //printf("kernel_start: %x, kernel_end: %x\n", &kernel_start, &kernel_end);
    //uint64_t result = 0x1234567890ABCDEF + 1;
    //printf("%x %x", *((uint32_t*)(&result) + 1), *((uint32_t*)(&result)));

    mm_init();

    //mm_init();
    //void* oi = malloc(1, 1);
    //void* ye = malloc(4, 1);
    //void* yee = malloc(1, 1);
    //mfree(oi, 1);
    //mfree(yee, 1);
    //mfree(ye, 4);

    //for (size_t i = 0; i < 0xffffFFFF; i++)
    //{
    //    void* addr = malloc();
    //    if((i % 0xffff) == 0)
    //        printf("%x\n", addr);
    //}

//    for (size_t i = 0; i < 0xffffFFFF; i++)
//    {
//        void* addr = malloc(4);
//
//        if((i % 0xff) == 0) {
//            printf("alloc addr: %x\n", addr);
//        }
//    }

    //pg_init();
    kshell_main();
}