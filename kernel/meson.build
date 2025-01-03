project('PaxOS', 'c', subproject_dir: 'lib', version: '0.1', license: 'GPL-2.0')

find_crtbegin_result = run_command('i386-elf-gcc', ['$CFLAGS', '-print-file-name=crtbegin.o'])
find_crtend_result = run_command('i386-elf-gcc', ['$CFLAGS', '-print-file-name=crtend.o'])
crtbegin_obj = find_crtbegin_result.stdout().strip()
crtend_obj = find_crtend_result.stdout().strip()

project_root = meson.current_source_dir()
arch_dir = 'arch/i386'

nasm = find_program('nasm')
asm_gen = generator(nasm,
    output: '@BASENAME@.o',
    arguments: ['-felf32', '@INPUT@', '-o', '@OUTPUT@'])

bootstrapobjs = asm_gen.process([arch_dir + '/boot.s'])
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
	'arch/i386/tty.c',
]

incdir = include_directories('include')

libc_proj = subproject('libc')
libc_dep = libc_proj.get_variable('libc_dep')

uacpi = subproject('uacpi')
uacpi_sources = uacpi.get_variable('sources')
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
			'--sysroot=' + project_root + '/../sysroot',
			'-isystem=/usr/include',
			'-std=gnu11',
			'-O2',
			'-g',
			'-ffreestanding',
			'-Wall',
			'-Wextra',
			'-fstack-protector-all',
			'-D__is_pax_os_kernel'
		],
    link_args: [
			'--sysroot=' + project_root + '/../sysroot',
			'-T', project_root + '/arch/i386/linker.ld',
			'-O2',
			'-lgcc',
			'-lc',
			'-nostdlib'
		],
    link_depends: 'arch/i386/linker.ld',
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

install_subdir('include', install_dir : project_root + '/../sysroot/usr/')
