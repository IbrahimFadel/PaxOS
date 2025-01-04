MAGIC equ 0xe85250d6
ARCH equ  0x0 ; 32-bit (protected) mode of i386
CHECKSUM equ 0x100000000 - (MAGIC + ARCH + (header_end - header_start))

; Tag structure from multiboot2 spec
;         +-------------------+
; u16     | type              |
; u16     | flags             |
; u32     | size              |
;         +-------------------+

%macro TAG 3
	dw %1
	dw %2
	dd %3
%endmacro

section .multiboot.data alloc write
align 4
header_start:
	dd MAGIC
	dd ARCH
	dd header_end - header_start
	dd CHECKSUM
	; Follow up with tags
	TAG 0x0, 0x0, 0x8 ; terminate tags
header_end:
