ENTRY(_start)

_KERNEL_VADDR = 0xC0000000;

SECTIONS
{
	/* kernel loaded into 3GB + 1MB in virtual address space */
	/* this gets mapped to 1MB in physical address space */
	. = 0x00100000;

	PROVIDE(_kernel_start = .);

	.multiboot.data : {
			*(.multiboot.data)
	}

	.multiboot.text : {
			*(.multiboot.text)
	}

	. += _KERNEL_VADDR;

	.text ALIGN (4K) : AT (ADDR (.text) - _KERNEL_VADDR)
	{
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _KERNEL_VADDR)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _KERNEL_VADDR)
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - _KERNEL_VADDR)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}

	PROVIDE(_kernel_end = .);
}
