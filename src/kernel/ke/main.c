#include <stdio.h>

#include "video.h"
#include "serialport.h"
#include "x86.h"
#include "kshell.h"
#include "pckbd.h"
#include "ke.h"

multiboot_info_t* mb;

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

int testbit(int num, int bit) {
    return (num & (1 << bit)) != 0;
}

int test;

void testfun()
{

}

void kernel_main(multiboot_info_t* _multiboot)
{
    mb = _multiboot;

    seg_init();
    idt_init();
    init_serial();
    fb_init();
    pckbd_init();
    pit_init();
    pit_setfreq(35);

    ke_systime = rtc_update();

    if(testbit(mb->flags, 0))
    {
        printf("mem_lower = %x, mem_upper = %x\n", mb->mem_lower * 1024, mb->mem_upper * 1024);
    }

    if(testbit(mb->flags, 6))
    {
        for (size_t i = 0; i < mb->mmap_length; i += sizeof(struct multiboot_mmap_entry))
        {
            struct multiboot_mmap_entry *me = (struct multiboot_mmap_entry*)(mb->mmap_addr + i);
            printf("addr = %x, len = %x, size = %x, type = %i\n", (uint32_t)me->addr, (uint32_t)me->len, me->size, me->type);
        }
        printf("test = %x, testfun = %x\n", &test, testfun);
    }
    //pg_init();
    kshell_main();
}