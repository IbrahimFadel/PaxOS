.intel_syntax noprefix

.section .text

.globl gdt_flush
gdt_flush:
	mov eax, [esp+4]
	lgdt [eax]
	mov eax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush # set CS
.flush:
	ret

# SEG_SIZE equ 8
# TSS_IDX equ 5

# 0x2B = 0x28 + 3

.globl tss_flush
tss_flush:
	mov ax, 0x2B
	ltr ax
	ret
