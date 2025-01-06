.intel_syntax noprefix

.section .init
.globl _init
_init:
	push ebp
	mov ebp, esp
.section .fini
.globl _fini
_fini:
	push ebp
	mov ebp, esp
