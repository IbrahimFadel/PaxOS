.intel_syntax noprefix

.set MAGIC, 0xe85250d6
.set ARCH, 0x0 # 32-bit (protected) mode of i386
.set CHECKSUM, 0x100000000 - (MAGIC + ARCH + (header_end - header_start))

# Tag structure from multiboot2 spec
#         +-------------------+
# u16     | type              |
# u16     | flags             |
# u32     | size              |
#         +-------------------+

.macro TAG, type, flags, size
	.word \type
	.word \flags
	.long \size
.endm

.section .multiboot.data, "aw"
.align 4
header_start:
	.long MAGIC
	.long ARCH
	.long header_end - header_start
	.long CHECKSUM
	# Follow up with tags
	TAG 0x0, 0x0, 0x8 # terminate tags
header_end:
