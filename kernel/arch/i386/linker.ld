#include "mmap.h"

ENTRY(_start)

SECTIONS {
    /* kernel loaded into KERNEL_VA + 1MB VA */
	/* this gets mapped to 1MB PA */
    . = KERNEL_OFFSET_START;
	PROVIDE(_kernel_start = .);

    .multiboot2.data ALIGN(4) : {
        *(.multiboot2.data)
	}

	.multiboot2.text : {
        *(.multiboot2.text)
	}

	. += KERNEL_VA;

	.text ALIGN (4K) : AT (ADDR (.text) - KERNEL_VA) {
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - KERNEL_VA) {
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - KERNEL_VA) {
		*(.data)
		. = ALIGN(TSS_ALIGNMENT);
		PROVIDE(ld_tss_base = .);
		*(.tss)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - KERNEL_VA) {
		*(COMMON)
		*(.bss)
	}

	.bootstrap_stack (NOLOAD) : ALIGN(STACK_ALIGNMENT) {
		KEEP(*(.bootstrap_stack))
	}

    PROVIDE(ld_kernel_end = .);
}
