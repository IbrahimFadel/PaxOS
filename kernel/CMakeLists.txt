cmake_minimum_required(VERSION 3.22.4)

set(CMAKE_C_COMPILER i386-elf-gcc)
set(CMAKE_CXX_COMPILER i386-elf-g++)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(KERNEL_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(arch/i386)

add_subdirectory(lib/memory)
add_subdirectory(lib/multiboot2)
add_subdirectory(lib/tty)

set(C_SRCS
	kernel.c

	$<TARGET_OBJECTS:i386>
	$<TARGET_OBJECTS:multiboot2>
)

add_executable(pax_kernel ${C_SRCS})

target_include_directories(pax_kernel PRIVATE include arch lib)
target_link_libraries(pax_kernel PRIVATE klibc memory)
# target_sources(pax_kernel PRIVATE
# 	$<TARGET_OBJECTS:i386>
# 	$<TARGET_OBJECTS:multiboot2>
# )
# target_link_libraries(pax_kernel PUBLIC multiboot2)

set(CMAKE_C_FLAG "")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -nostdlib -m32 -std=gnu99 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "-O3 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2 -g")

set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T ${LINKER_FILE} -nostdlib -lgcc")

add_custom_target(iso
	COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/iso/boot/grub
	COMMAND cp ${CMAKE_SOURCE_DIR}/iso/grub.cfg ${CMAKE_SOURCE_DIR}/iso/boot/grub/
	COMMAND cp $<TARGET_FILE:pax_kernel> ${CMAKE_SOURCE_DIR}/iso/boot/
	COMMAND grub-mkrescue -o ${CMAKE_SOURCE_DIR}/build/pax_os.iso ${CMAKE_SOURCE_DIR}/iso
	DEPENDS pax_kernel
	BYPRODUCTS ${CMAKE_SOURCE_DIR}/iso/boot
	COMMENT "Generating bootable ISO"
)

add_custom_target(qemu
	DEPENDS iso
	COMMAND qemu-system-i386 -cdrom ${CMAKE_SOURCE_DIR}/build/pax_os.iso
)

set_property(
	TARGET iso
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_SOURCE_DIR}/iso/boot/
)
