cmake_minimum_required(VERSION 3.22.4)
project(pax C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(KERNEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kernel)

add_subdirectory(libc)
add_subdirectory(kernel)
