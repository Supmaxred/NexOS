OUTPUT_ARCH(i386);
OUTPUT_FORMAT(elf32-i386);
ENTRY(start)

K_OFFSET = 0x00000000;

SECTIONS
{
	. = 0x100000;

	vkernel_start = . + K_OFFSET;
    pkernel_start = .;

	.setup ALIGN(0x1000) :
	{
		*(.setup)
	}

	. += K_OFFSET;

    .text ALIGN (0x1000) : AT(ADDR(.text)-K_OFFSET)
    {
		*(.multiboot)
        *(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata)-K_OFFSET)
    {
        *(.rodata*)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data)-K_OFFSET)
    {
        *(.data)
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss)-K_OFFSET)
    {
        *(COMMON)
        *(.bss)
    }

	vkernel_end = .;
	pkernel_end = . - K_OFFSET;

	/DISCARD/ :
	{
		*(.interp)
	}
}