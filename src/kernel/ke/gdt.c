#include <string.h>
#include "x86.h"

gdt_entry_t gdt_table[6];
gdtr_t gdtr;

void seg_init()
{
    gdtr.offset = (uintptr_t)&gdt_table;
    gdtr.limit = sizeof(gdt_table) - 1;

    memset(&gdt_table[0], 0, sizeof(gdt_table[0]));
    setgdtgate(&gdt_table[1], 0x0, 0xFFFFF, DPL_KRNL, 0, IS_CS);
    setgdtgate(&gdt_table[2], 0x0, 0xFFFFF, DPL_KRNL, 0, IS_DS);
    setgdtgate(&gdt_table[3], 0x0, 0xFFFFF, DPL_USER, 0, IS_CS);
    setgdtgate(&gdt_table[3], 0x0, 0xFFFFF, DPL_USER, 0, IS_DS);

    gdt_flush(&gdtr);
}