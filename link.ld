OUTPUT_ARCH(i386);
OUTPUT_FORMAT(elf32-i386);
ENTRY(start)

/* Thanks to https://github.com/RMuskovets/Skeles/blob/master/kernel/arch/i386/linker.ld*/
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 0x100000;
	kernel_start = .;

	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	/*.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}*/

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
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