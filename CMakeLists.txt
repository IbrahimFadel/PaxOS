cmake_minimum_required(VERSION 3.22.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchains/i386.cmake)
project(pax C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(libc)
add_subdirectory(kernel)
