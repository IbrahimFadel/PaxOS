# CMake Toolchain File for i386 Target
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=i386-toolchain.cmake ..

# ─── System ───────────────────────────────────────────────────────────────────
set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

# ─── Compiler ─────────────────────────────────────────────────────────────────
# Prefer an explicit i386-linux-gnu cross-compiler if available;
# fall back to the host compiler forced to 32-bit via -m32.

find_program(CROSS_C_COMPILER   i386-elf-gcc)
find_program(CROSS_CXX_COMPILER i386-elf-g++)

if(CROSS_C_COMPILER)
    set(CMAKE_C_COMPILER   ${CROSS_C_COMPILER})
    set(CMAKE_CXX_COMPILER ${CROSS_CXX_COMPILER})
else()
    # Native compiler in 32-bit mode
    set(CMAKE_C_COMPILER   gcc)
    set(CMAKE_CXX_COMPILER g++)

    # Force 32-bit output
    add_compile_options(-m32)
    add_link_options(-m32)
endif()

# ─── Assembler / Archiver ─────────────────────────────────────────────────────
find_program(CROSS_ASM i386-elf-as)
if(CROSS_ASM)
    set(CMAKE_ASM_COMPILER ${CROSS_ASM})
endif()

find_program(CROSS_AR i386-elf-ar)
if(CROSS_AR)
    set(CMAKE_AR ${CROSS_AR})
endif()

find_program(CROSS_RANLIB i386-elf-ranlib)
if(CROSS_RANLIB)
    set(CMAKE_RANLIB ${CROSS_RANLIB})
endif()

find_program(CROSS_STRIP i386-elf-strip)
if(CROSS_STRIP)
    set(CMAKE_STRIP ${CROSS_STRIP})
endif()

# ─── Sysroot (optional) ───────────────────────────────────────────────────────
# Set CMAKE_SYSROOT if you have a dedicated i386 sysroot, e.g.:
#   set(CMAKE_SYSROOT /opt/sysroots/i386-linux-gnu)

# ─── Search Paths ─────────────────────────────────────────────────────────────
# Prevent CMake from accidentally resolving host (64-bit) libraries.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  # use host tools
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)   # target libs only
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)   # target headers only
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)   # target packages only

# ─── Compiler Flags ───────────────────────────────────────────────────────────
# Core 32-bit flag (always set, even with cross-compiler, for clarity)
set(ARCH_FLAGS "-m32 -march=i386 -mtune=i386")

set(CMAKE_C_FLAGS_INIT             "${ARCH_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT           "${ARCH_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "-m32")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-m32")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-m32")

set(ARCH_DIR ${CMAKE_SOURCE_DIR}/kernel/arch/i386)

set(CMAKE_C_FLAGS "-ffreestanding -nostdlib -nostartfiles -m32 -std=gnu99")
set(CMAKE_C_LINK_FLAGS "-nostdlib -nostartfiles -m32")

set(KERNEL_LINKER_FILE_UNPROCESSED  ${ARCH_DIR}/linker.ld)
set(KERNEL_LINKER_FILE ${CMAKE_BINARY_DIR}/kernel/linker.ld)

add_custom_command(
    OUTPUT
        ${KERNEL_LINKER_FILE}
    COMMAND
        ${CMAKE_C_COMPILER}
            -E -P -x c
            # -I${COMMON_DIR}
            ${KERNEL_LINKER_FILE_UNPROCESSED}
            -o ${KERNEL_LINKER_FILE}
    DEPENDS ${KERNEL_LINKER_FILE_UNPROCESSED}
    COMMENT "Preprocessing kernel linker script"
)

add_custom_target(kernel_linker_script_preprocess
    DEPENDS ${KERNEL_LINKER_FILE}
)
