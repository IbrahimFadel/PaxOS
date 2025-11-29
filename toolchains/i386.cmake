set(CMAKE_SYSTEM_NAME GNU)

set(CMAKE_C_COMPILER i386-elf-gcc)
set(CMAKE_CXX_COMPILER i386-elf-g++)

set(CMAKE_C_FLAGS "-ffreestanding -nostdlib -nostartfiles -m32 -std=gnu99")
set(CMAKE_C_LINK_FLAGS "-nostdlib -nostartfiles -m32")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(LINKER_FILE ${CMAKE_SOURCE_DIR}/kernel/arch/i386/linker.ld)
