cmake_minimum_required(VERSION 3.22.4)

set(C_SRCS
    tty.c
)

add_library(tty ${C_SRCS})

target_include_directories(tty PRIVATE
)

target_link_libraries(tty PRIVATE
)
