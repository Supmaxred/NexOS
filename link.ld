OUTPUT_ARCH(i386);
OUTPUT_FORMAT(elf32-i386);
ENTRY(start)

SECTIONS
{
	. = 0x100000;
	kernel_start = .;

	.text : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/*.rodata : ALIGN(4K)
	{
		*(.rodata)
	}*/

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.bss : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	kernel_end = .;

	/DISCARD/ :
	{
		*(.interp)
	}
}