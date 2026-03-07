cmake_minimum_required(VERSION 3.22.4)

set(C_SRCS
    logging.c
)

add_library(logging STATIC ${C_SRCS})

target_include_directories(logging PRIVATE )
target_link_libraries(logging PRIVATE klibc)
