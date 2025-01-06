project('PaxOS', 'c', subproject_dir: 'lib', version: '0.1', license: 'GPL-2.0')

find_crtbegin_result = run_command('i386-elf-gcc', ['$CFLAGS', '-print-file-name=crtbegin.o'])
find_crtend_result = run_command('i386-elf-gcc', ['$CFLAGS', '-print-file-name=crtend.o'])
crtbegin_obj = find_crtbegin_result.stdout().strip()
crtend_obj = find_crtend_result.stdout().strip()

project_root = meson.current_source_dir()

arch = target_machine.cpu()
arch_dir = 'arch/' + arch

assembler = find_program('i386-elf-as')
assembler_flags = ['-msyntax=intel', '@INPUT@', '-o', '@OUTPUT@']

asm_gen = generator(assembler,
    output: '@BASENAME@.o',
    arguments: assembler_flags
)

bootstrapobjs = asm_gen.process([arch_dir + '/multiboot.s', arch_dir + '/boot.s'])
crtiobj = asm_gen.process([arch_dir + '/crti.s'])
crtnobj = asm_gen.process([arch_dir + '/crtn.s'])
arch_asm_objs = asm_gen.process([arch_dir + '/gdt.s', arch_dir + '/idt.s'])

linker_file = files(arch_dir + '/linker.ld')

sources = [
	'pax/kernel.c',
	'pax/acpi.c',
	'pax/interrupt.c',
	'pax/protect.c',
	'pax/stack_prot.c',
	'pax/boot_info.c',
	'pax/memory.c',
	'arch/i386/tty.c',
]

incdir = include_directories('include')

libc_proj = subproject('libc')
libc_dep = libc_proj.get_variable('libc_dep')

# uacpi = subproject('uacpi')
# uacpi_sources = uacpi.get_variable('sources')
# sources += uacpi_sources
# uacpi_includes = uacpi.get_variable('includes')

kernel_elf = executable('pax_os.elf',
		bootstrapobjs,
		crtiobj,
		sources,
		crtnobj,
		install: true, install_dir: project_root + '/../sysroot/boot',
		include_directories: [incdir],
		c_args: [
			'-D__is_pax_os_kernel',
			'--sysroot=' + project_root + '/../sysroot',
			'-isystem=/usr/include',
			'-std=gnu11',
			'-O2',
			'-g',
			'-ffreestanding',
			'-Wall',
			'-Wextra',
			# '-fstack-protector-all',
			# '-Wstack-protector',
			# '--param', 'ssp-buffer-size=4',
			# '-fstack-clash-protection',
			# '-D_FORTIFY_SOURCE=2',
			# '--specs=nosys.specs'
		],
    link_args: [
			'--sysroot=' + project_root + '/../sysroot',
			'-T', project_root + '/' + arch_dir + '/linker.ld',
			'-O2',
			'-lgcc',
			'-lc',
			'-nostdlib',
			# '--specs=nosys.specs'
		],
    link_depends: arch_dir + '/linker.ld',
    objects: [crtbegin_obj, crtend_obj, arch_asm_objs],
		dependencies: [libc_dep]
)

prog_python = import('python').find_installation('python3')
grub_cfg = files(project_root + '/../grub.cfg')
iso_gen = custom_target('build-iso',
		input: [grub_cfg, kernel_elf],
    output: ['pax_os.iso'],
		install: true,
		install_dir: project_root + '/../',
    command: [prog_python, project_root + '/build-iso.py', '@INPUT@'],
    build_by_default: true,
    depends: [kernel_elf],
)

# install_libc_gen = custom_target('install-libc',
# 		output: 'none',
#     # command: [prog_python, project_root + '/install-libc.py'],
#     # command: ['cd lib/libc && meson install -C build-i386'],
#     command: ['install-libc.sh'],
#     build_by_default: true,
#     # depends: [libc_proj],
# )

install_subdir('include', install_dir: project_root + '/../sysroot/usr/', install_tag: 'headers')
