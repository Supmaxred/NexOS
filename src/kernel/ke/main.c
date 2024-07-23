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
    if(ms < 0)
    {
        cli();
        
        while(1)
            hlt();
    }

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
    pit_setfreq(35);

    ke_systime = rtc_update();

    if(testbit(mb->flags, 0))
    {
        printf("mem_lower = %x, mem_upper = %x\n", mb->mem_lower * 1024, mb->mem_upper * 1024);
    }

    if(testbit(mb->flags, 5))
    {
        multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mb->u.elf_sec);

        printf ("multiboot_elf_sec: num = %i, size = 0x%x, addr = 0x%x, shndx = 0x%x\n",
            (unsigned) multiboot_elf_sec->num, (unsigned) multiboot_elf_sec->size,
            (unsigned) multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
    }

    if(testbit(mb->flags, 6))
    {
        for (size_t i = 0; i < mb->mmap_length; i += sizeof(struct multiboot_mmap_entry))
        {
            struct multiboot_mmap_entry *me = (struct multiboot_mmap_entry*)(mb->mmap_addr + i);
            printf("addr = %x, len = %x, size = %x, type = %i\n", (uint32_t)me->addr, (uint32_t)me->len, me->size, me->type);
        }
        printf("test = %x, testfun = %x\n", &test, testfun);
        printf("start = %x, end = %x\n", &kernel_start, &kernel_end);
    }
    //pg_init();
    kshell_main();
}