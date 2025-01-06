project('libc', 'c', version: '0.1', license: 'GPL-2.0')

project_root = meson.project_source_root()

arch = target_machine.cpu()
arch_dir = 'arch/' + arch

assembler = find_program('i386-elf-as')
assembler_flags = ['-msyntax=intel', '@INPUT@', '-o', '@OUTPUT@']

asm_gen = generator(assembler,
    output: '@BASENAME@.o',
    arguments: assembler_flags
)

# arch_asm_objs = asm_gen.process([arch_dir + '/gdt.s', arch_dir + '/idt.s'])
arch_asm_objs = []

sources = [
	arch_dir + '/io.c',

	'stdio/printf.c',
	'stdio/putchar.c',
	'stdio/puts.c',
	'stdio/sprintf.c',

	'stdlib/abort.c',
	'stdlib/itoa.c',
	'stdlib/panic.c',

	'string/memcmp.c',
	'string/memcpy.c',
	'string/memmove.c',
	'string/memset.c',
	'string/strcpy.c',
	'string/strlen.c',
	'string/strrev.c',
]

incdir = include_directories('include')

libc = static_library('c',
		sources,
		install: true, install_dir: project_root + '/../../../sysroot/usr/lib',
		include_directories: incdir,
		c_args: [
			'--sysroot=' + project_root + '/../../../sysroot',
			'-isystem=/usr/include',
			'-std=gnu11',
			'-O2',
			'-g',
			'-ffreestanding',
			'-Wall',
			'-Wextra',
			# '-fstack-protector-all',
			'-D__is_pax_os_kernel',
			'-D__is_libk'
		],
    objects: [arch_asm_objs])

libc_dep = declare_dependency(include_directories : incdir,
  link_with : libc)

install_subdir('include', install_dir : project_root + '/../../../sysroot/usr/', install_tag: 'headers')
