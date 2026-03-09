#include "mmap_config.h"

ENTRY(_start)

SECTIONS {
    . = KERNEL_OFFSET_START;
	PROVIDE(_kernel_start = .);

    .multiboot2.data ALIGN(4) : {
        *(.multiboot2.data)
	}

	.multiboot2.text : {
        *(.multiboot2.text)
	}

	 /* User code lives at a well-known user VA,
       but physically sits right after the multiboot sections.
       AT() sets the LMA (load/physical address) independently of the VMA. */
    _user_text_phys = ALIGN(LOADADDR(.multiboot2.text) 
                            + SIZEOF(.multiboot2.text), 4K);

    .user_text 0x00400000 : AT(_user_text_phys) {
        _user_text_start = .;         /* VMA = 0x00400000 */
        *(.user_text)
        . = ALIGN(4K);
        _user_text_end = .;
    }

	. = KERNEL_OFFSET_START + SIZEOF(.multiboot2.data) 
                             + SIZEOF(.multiboot2.text)
                             + SIZEOF(.user_text);
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
