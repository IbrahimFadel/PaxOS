.set MAGIC, 0xe85250d6
.set MODE,  0x0
.set TYPE,  0x0
.set FLAGS, 0x0
.set SIZE,  0x8

.section .multiboot_header
header_start:
	.4byte MAGIC
	.4byte MODE
	.4byte header_end - header_start
	.4byte 0x100000000 - (MAGIC + 0 + (header_end - header_start))
	.2byte TYPE
	.2byte FLAGS
	.4byte SIZE
header_end:

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp
	call kmain
	cli
1:	hlt
	jmp 1b

.size _start, . - _start
