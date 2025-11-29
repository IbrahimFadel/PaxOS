cmake_minimum_required(VERSION 3.22.4)

set(C_SRCS
    boot.s
    cpu.c
    tty/tty.c
)

add_library(i386 OBJECT ${C_SRCS})

target_include_directories(i386 PRIVATE
    ${KERNEL_DIR}/lib
    ${KERNEL_DIR}/include
)

target_link_libraries(i386 PRIVATE
    klibc
)
