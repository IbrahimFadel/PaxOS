cmake_minimum_required(VERSION 3.22.4)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

add_subdirectory(${ARCH_DIR})

set(C_SRCS
	src/interrupts/isr.c
	src/interrupts/idt.c
	src/interrupts/interrupts.s

	src/ipc/spinlock.c

	src/mem/bootstrap.c
	src/mem/kmalloc.c
	src/mem/pmm.c
	src/mem/vmm.c

	src/multiboot2/multiboot2.c

	src/gdt.c
	src/kernel.c
	src/pic.c
	src/proc.c
	src/tss.c

	$<TARGET_OBJECTS:i386>
)

add_executable(pax_kernel)
target_sources(pax_kernel PRIVATE ${C_SRCS})
target_include_directories(pax_kernel PRIVATE include arch lib)
target_link_libraries(pax_kernel PRIVATE klibc)
target_link_options(pax_kernel PRIVATE -T ${KERNEL_LINKER_FILE})
add_dependencies(pax_kernel kernel_linker_script_preprocess sysroot_headers)

if("KCONFIG_ENABLE_LOGGING" IN_LIST KCONFIG_COMPILE_DEFS)
	message(STATUS "adding `logging` library")
	add_subdirectory(lib/logging)
	target_link_libraries(pax_kernel PRIVATE logging)
endif()

add_custom_target(iso
	COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/iso/boot/grub
	COMMAND cp ${CMAKE_SOURCE_DIR}/grub.cfg ${CMAKE_SOURCE_DIR}/iso/boot/grub/
	COMMAND cp $<TARGET_FILE:pax_kernel> ${CMAKE_SOURCE_DIR}/iso/boot/
	COMMAND grub-mkrescue -o ${CMAKE_SOURCE_DIR}/build/pax_os.iso ${CMAKE_SOURCE_DIR}/iso
	DEPENDS pax_kernel
	BYPRODUCTS ${CMAKE_SOURCE_DIR}/iso/boot
	COMMENT "Generating bootable ISO"
)

set(SERIAL_PORT 1234)

add_custom_target(qemu
	DEPENDS iso
	USES_TERMINAL
	COMMAND qemu-system-i386
			-cdrom ${CMAKE_SOURCE_DIR}/build/pax_os.iso 
			-serial pty
			-nographic
)

add_custom_target(qemu-gui
	DEPENDS iso
	USES_TERMINAL
	COMMAND qemu-system-i386
			-cdrom ${CMAKE_SOURCE_DIR}/build/pax_os.iso 
			-serial pty
)

add_custom_target(debug
	DEPENDS iso
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	USES_TERMINAL
	COMMAND bochs
)

set_property(
	TARGET iso
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_SOURCE_DIR}/iso/boot/
)
