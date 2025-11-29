cmake_minimum_required(VERSION 3.22.4)

file(GLOB C_SRCS **/**/*.c **/*.c *.c)

add_library(klibc ${C_SRCS})
target_include_directories(klibc PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
