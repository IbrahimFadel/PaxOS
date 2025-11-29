cmake_minimum_required(VERSION 3.22.4)

set(C_SRCS
    multiboot2.c
)

# this must be an OBJECT library so that the
# header doesn't get discarded at link time
add_library(multiboot2 OBJECT ${C_SRCS})

target_include_directories(multiboot2 PRIVATE
)

target_link_libraries(multiboot2 PRIVATE
)
