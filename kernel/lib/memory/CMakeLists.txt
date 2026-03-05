cmake_minimum_required(VERSION 3.22.4)

set(C_SRCS
    paging.c
)

add_library(memory STATIC ${C_SRCS})
target_include_directories(memory PRIVATE )
target_link_libraries(memory PRIVATE )
