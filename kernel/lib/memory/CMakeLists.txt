cmake_minimum_required(VERSION 3.22.4)

set(C_SRCS
    paging.c
)

add_library(memory ${C_SRCS})

target_include_directories(memory PRIVATE
    ${KERNEL_DIR}/arch
)

target_link_libraries(memory PRIVATE
)
