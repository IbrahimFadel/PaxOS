.section .text
.global outb, outw, outl
.type outb, @function
.type outw, @function
.type outl, @function

# __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
outb:
	movb %dl, %al  # Set byte to be outputted
	movw %si, %dx  # Set port
	out %ax, %dx
	ret
outw:
	movw %di, %ax  # Set word to be outputted
	movw %si, %dx  # Set port
	out %ax, %dx
	ret
outl:
	movl %edi, %eax  # Set int to be outputted
	movw %si, %dx  # Set port
	out %eax, %dx
	ret

.global inw
.type outb, @function
inw:
	mov %si, %dx # Set port
	in %dx, %ax
	ret
