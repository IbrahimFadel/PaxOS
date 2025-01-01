MAGIC equ 0xe85250d6
MODE equ  0x0
TYPE equ 0x0
FLAGS equ 0x0
SIZE equ 0x8

section .multiboot_header
header_start:
	dd MAGIC
	dd MODE
	dd header_end - header_start
	dd 0x100000000 - (MAGIC + 0 + (header_end - header_start))
	dw TYPE
	dw FLAGS
	dd SIZE
header_end:

; Stack must be 16 byte aligned
section .bss
align 16
stack_bottom:
	resb 16384 ; 16 KiB
stack_top:

extern kmain
section .text
global _start:function (_start.end - _start)

_start:
	mov esp, stack_top
	call kmain
	cli
.hang:
	hlt
	jmp .hang
.end: