.intel_syntax noprefix

.set KERNEL_STACK_SIZE, 16384 # 1024 x 16 = 16KiB. multiple of 16 for alignment
.set KERNEL_VADDR, 0xC0000000
.set VGA_BUF_PHY_ADDR, 0x000B8000 # TODO: i dont want to hardcode this, multiboot should provide it

.set PAGE_SIZE, 4096
.set ENTRY_SIZE, 4
.set NUM_ENTRIES, 1024

# ENTRY FLAGS
.set PAGE_PRESENT, 1
.set PAGE_RW, (1 << 1)

.section .bootstrap_stack, "aw", @nobits
.align 16
.globl stack_top
stack_bottom:
	.skip KERNEL_STACK_SIZE
stack_top:

.section .bss, "aw", @nobits
.globl boot_pd

.align PAGE_SIZE
boot_pd:
	.skip ENTRY_SIZE * NUM_ENTRIES
kernel_page_table:
	.skip ENTRY_SIZE * NUM_ENTRIES

.section .multiboot.text, "a"
.globl _start
.extern _kernel_start, _kernel_end # defined in linker.ld
_start:
	mov esi, 0
	mov edi, offset kernel_page_table - KERNEL_VADDR
	mov ecx, NUM_ENTRIES - 1 # map all but last entry
.map_loop:
	# put all kernel code into `kernel_page_table`
	cmp esi, offset _kernel_start
	jl .map_inc
	cmp esi, offset _kernel_end - KERNEL_VADDR
	jge .map_finish

	# mark address as present and store in page table
	mov edx, esi
	or edx, PAGE_PRESENT | PAGE_RW
	mov [edi], edx
.map_inc:
	add esi, PAGE_SIZE  # go to next page that needs to be mapped
	add edi, ENTRY_SIZE # go to next entry in `kernel_page_table`
	loop .map_loop
.map_finish:
	# map VGA to last entry
	mov dword ptr [kernel_page_table - KERNEL_VADDR + (NUM_ENTRIES - 1) * ENTRY_SIZE], VGA_BUF_PHY_ADDR | PAGE_PRESENT | PAGE_RW

	# once we enable paging, in order to fetch the next instruction we need this identity map
	# after we have jumped to the higher half and everything is setup we can undo the mapping
	# identity map PD[0] to kernel PT
	mov dword ptr [boot_pd - KERNEL_VADDR], offset kernel_page_table - KERNEL_VADDR + (PAGE_PRESENT | PAGE_RW)

	# map PD[768] to kernel PT
	# 768 = KERNEL_VADDR / PAGE_SIZE / NUM_ENTRIES
	mov dword ptr [boot_pd - KERNEL_VADDR + (768 * ENTRY_SIZE)], offset kernel_page_table - KERNEL_VADDR + (PAGE_PRESENT | PAGE_RW)

	# set Page Directory Base Register
	mov ecx, offset boot_pd - KERNEL_VADDR
	mov cr3, ecx

	# enable paging and write protect 
	mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx

	lea ecx, higher_half
	jmp ecx

.section .text
.extern kmain
higher_half:
	# undo identity mapping
	mov dword ptr [boot_pd], 0

	# flush TLB after unmapping identity mapping
	# tbh im not 100% sure if i did the `invlpg` instr correctlu, the mov is safer... because i'm an idiot
	invlpg [boot_pd]
	# mov ecx, cr3
	# mov cr3, ecx

	mov esp, offset stack_top
	push ebx # physical address of mbi
	push eax # multiboot magic number 0x36d76289
	call kmain
	cli
.ruhroh:
	hlt
	jmp .ruhroh
