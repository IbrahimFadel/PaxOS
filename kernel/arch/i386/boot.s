%include "multiboot.s"

KERNEL_STACK_SIZE equ 16384 ; 1024 x 16 = 16KiB. multiple of 16 for alignment
KERNEL_VADDR equ 0xC0000000
VGA_BUF_PHY_ADDR equ 0x000B8000

; -- Paging --
; Page Directory (PD) and Page Table (PT) both have 1024 4 byte entries
; Each PD entry points to a PT
; Each PT entry points to a 4 KiB physical page frame
; Each PT entry also has flag bits
;
; 1024 PD entries x 1024 PT entries x 4 KiB  per frame = 4Gb address space
PAGE_SIZE equ 4096
ENTRY_SIZE equ 4
NUM_ENTRIES equ 1024

; ENTRY FLAGS
PAGE_PRESENT equ 1
PAGE_RW equ (1 << 1)

CR0_WRITE_PROTECT equ (1 << 16)
CR0_PAGING equ (1 << 31)

section .bootstrap_stack alloc write nobits
align 16
stack_bottom:
	resb KERNEL_STACK_SIZE
stack_top:

section .bss alloc write nobits
align PAGE_SIZE
boot_page_directory:
	resb PAGE_SIZE
boot_page_table1:
	resb PAGE_SIZE

section .multiboot.text, alloc
global _start
extern _kernel_start, _kernel_end ; defined in linker.ld
_start:
	mov edi, boot_page_table1 - KERNEL_VADDR ; phys address of boot_page_table1.
	mov esi, 0 ; map address 0x0 first
	mov ecx, NUM_ENTRIES - 1 ; map all but one pages. the last one will be for VGA text buffer
.map_addr:
	; make sure we only map the kernel
	cmp esi, _kernel_start
	jl .map_inc
	cmp esi, _kernel_end - KERNEL_VADDR
	jge .map_finish

	; map physical address
	mov edx, esi
	or edx, PAGE_PRESENT | PAGE_RW
	mov [edi], edx
.map_inc:
	add esi, PAGE_SIZE
	add edi, ENTRY_SIZE
	loop .map_addr
.map_finish:
	; map VGA buffer to last table
	; TODO: don't hardcode `VGA_BUF_PHY_ADDR`, we can get this info from multiboot at runtime
	mov long [boot_page_table1 - KERNEL_VADDR + (NUM_ENTRIES - 1) * ENTRY_SIZE], VGA_BUF_PHY_ADDR | (PAGE_PRESENT | PAGE_RW)

	; paging does not take effect immediately, so map to 0x0 temporarily
	; map PT to 0x0
	mov long [boot_page_directory - KERNEL_VADDR + 0], boot_page_table1 - KERNEL_VADDR + (PAGE_PRESENT | PAGE_RW)
	; map PT to `KERNEL_VADDR`
	mov long [boot_page_directory - KERNEL_VADDR + 768 * ENTRY_SIZE], boot_page_table1 - KERNEL_VADDR + (PAGE_PRESENT | PAGE_RW)

	; set control registers for paging
	mov ecx, boot_page_directory - KERNEL_VADDR
	mov cr3, ecx

	mov ecx, cr0
	or ecx, CR0_PAGING | CR0_WRITE_PROTECT
	mov cr0, ecx

	lea ecx, higher_half
	jmp ecx

section .text
extern kmain
higher_half:
	; unmap 0x0 as it is now unnecessary. 
	mov long [boot_page_directory + 0], 0
	; flush TLB after unmapping identity mapping
	mov ecx, cr3
	mov cr3, ecx

	; prepare for kernel entry
	mov esp, stack_top
	push ebx ; physical address of mbi
	push eax ; multiboot magic number 0x36d76289
	call kmain
	cli
.ruhroh:
	hlt
	jmp .ruhroh
