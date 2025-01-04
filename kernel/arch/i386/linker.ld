ENTRY(_start)

SECTIONS
{
	/* kernel loaded into 3GB + 1MB in virtual address space */
	/* this gets mapped to 1MB in physical address space */
	. = 0x00100000;

	_kernel_start = .;

	.multiboot.data : {
			*(.multiboot.data)
	}

	.multiboot.text : {
			*(.multiboot.text)
	}

	. += 0xC0000000;

	/* page align sections */

	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	_kernel_end = .;
}
