ENTRY(_start)

_KERNEL_VADDR = 0xC0000000;
PROVIDE(_kernel_va = _KERNEL_VADDR);

SECTIONS {
    /* kernel loaded into _KERNEL_VADDR + 1MB VA */
	/* this gets mapped to 1MB PA */
    . = 0x00100000;

    PROVIDE(_kernel_start = .);

    .multiboot2.data ALIGN(4) : {
        *(.multiboot2.data)
	}

	.multiboot2.text : {
        *(.multiboot2.text)
	}

	. += _KERNEL_VADDR;

	.text ALIGN (4K) : AT (ADDR (.text) - _KERNEL_VADDR) {
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _KERNEL_VADDR) {
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _KERNEL_VADDR) {
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - _KERNEL_VADDR) {
		*(COMMON)
		*(.bss)
		/* KEEP(*(.bootstrap_stack)) */
	}

	.bootstrap_stack (NOLOAD) : ALIGN(16) {
		KEEP(*(.bootstrap_stack))
	}

    PROVIDE(_kernel_end = .);
}
