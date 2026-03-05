#include "mmap.h"

ENTRY(_start)

_KERNEL_VA = KERNEL_VA;

SECTIONS {
    /* kernel loaded into _KERNEL_VA + 1MB VA */
	/* this gets mapped to 1MB PA */
    . = KERNEL_OFFSET_START;
	PROVIDE(_kernel_start = .);

    .multiboot2.data ALIGN(4) : {
        *(.multiboot2.data)
	}

	.multiboot2.text : {
        *(.multiboot2.text)
	}

	. += _KERNEL_VA;

	.text ALIGN (4K) : AT (ADDR (.text) - _KERNEL_VA) {
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _KERNEL_VA) {
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _KERNEL_VA) {
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - _KERNEL_VA) {
		*(COMMON)
		*(.bss)
		/* KEEP(*(.bootstrap_stack)) */
	}

	.bootstrap_stack (NOLOAD) : ALIGN(STACK_ALIGNMENT) {
		KEEP(*(.bootstrap_stack))
	}

    PROVIDE(_kernel_end = .);
}
