#include "mmap.h"

.intel_syntax noprefix

.section .bootstrap_stack, "aw", @nobits
.align 16
.globl stack_top
stack_bottom:
	.skip STACK_SIZE
stack_top:

.section .bss, "aw", @nobits
.globl boot_page_dir

.align PAGE_SIZE
boot_page_dir:
	.skip PTE_SIZE * PAGE_TABLE_SIZE
kernel_page_table:
	.skip PTE_SIZE * PAGE_TABLE_SIZE

.section .multiboot2.text, "a"
.globl _start
.extern _kernel_start, _kernel_end
_start:
	mov esi, 0
	mov edi, offset kernel_page_table - KERNEL_VA
	mov ecx, PAGE_TABLE_SIZE - 1 # map all but last entry
.map_loop:
	# put all kernel code into `kernel_page_table`
	cmp esi, offset KERNEL_START
	jl .map_inc
	cmp esi, offset _kernel_end - KERNEL_VA
	jge .map_finish

	# mark address as present and store in page table
	mov edx, esi
	or edx, PAGE_PRESENT | PAGE_RW
	mov [edi], edx
.map_inc:
	add esi, PAGE_SIZE  # go to next page that needs to be mapped
	add edi, PTE_SIZE # go to next entry in `kernel_page_table`
	loop .map_loop
.map_finish:
	# map VGA to last entry
	mov dword ptr [kernel_page_table - KERNEL_VA + (PAGE_TABLE_SIZE - 1) * PTE_SIZE], VGA_BUF_PHY_ADDR | PAGE_PRESENT | PAGE_RW
	# identity map PD[0] to kernel PT, that way we can fetch the next instruction after enabling paging
	mov dword ptr [boot_page_dir - KERNEL_VA], offset kernel_page_table - KERNEL_VA + (PAGE_PRESENT | PAGE_RW)
	# map PD[768] to kernel PT. 768 = KERNEL_VA / PAGE_SIZE / PAGE_TABLE_SIZE
	mov dword ptr [boot_page_dir - KERNEL_VA + (768 * PTE_SIZE)], offset kernel_page_table - KERNEL_VA + (PAGE_PRESENT | PAGE_RW)
	# map PD[1023] to PD[0]
	mov dword ptr [boot_page_dir - KERNEL_VA + (PAGE_TABLE_SIZE - 1) * PTE_SIZE], offset boot_page_dir - KERNEL_VA + (PAGE_PRESENT | PAGE_RW)

	# set Page Directory Base Register
	mov ecx, offset boot_page_dir - KERNEL_VA
	mov cr3, ecx

	# enable paging and write protect 
	mov ecx, cr0
	or ecx, 0x80000001
	mov cr0, ecx

	lea ecx, higher_half
	jmp ecx

.section .text
.extern kmain
higher_half:
	# undo identity mapping
	mov dword ptr [boot_page_dir], 0

	# flush TLB after unmapping identity mapping
	# tbh im not 100% sure if i did the `invlpg` instr correctlu, the mov is safer... because i'm an idiot
	# invlpg [boot_page_dir]
	mov ecx, cr3
	mov cr3, ecx

	mov esp, offset stack_top
	push ebx # physical address of mbi
	push eax # multiboot2 magic number 0x36d76289
	call kmain
	cli
.ruhroh:
	hlt
	jmp .ruhroh
