cmake_minimum_required(VERSION 3.22.4)
project(pax C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# parse kconfig
include(cmake/ParseTOML.cmake)
toml_to_compile_definitions("kconfig.toml" KCONFIG_COMPILE_DEFS)
set_property(
    DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/kconfig.toml"
)

set(KERNEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/kernel)

add_custom_command(
    OUTPUT
        ${KERNEL_LINKER_FILE}
    COMMAND
        ${CMAKE_C_COMPILER}
            -E -P -x c
            # -I${COMMON_DIR}
            ${KERNEL_LINKER_FILE_UNPROCESSED}
            -o ${KERNEL_LINKER_FILE}
    DEPENDS ${KERNEL_LINKER_FILE_UNPROCESSED}
    COMMENT "Preprocessing kernel linker script"
)

add_custom_target(kernel_linker_script_preprocess
    DEPENDS ${KERNEL_LINKER_FILE}
)


file(GLOB_RECURSE KERNEL_HEADERS ${CMAKE_SOURCE_DIR}/kernel/include/*.h)
file(GLOB_RECURSE LIBC_HEADERS   ${CMAKE_SOURCE_DIR}/libc/include/*.h)

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/headers.stamp 
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SYSROOT}/usr/include/sys
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SYSROOT}/usr/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/kernel/include ${SYSROOT}/usr/include/pax
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/libc/include       ${SYSROOT}/usr/include
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/headers.stamp
    DEPENDS ${KERNEL_HEADERS} ${LIBC_HEADERS}
    COMMENT "Installing headers to sysroot"
)
add_custom_target(sysroot_headers DEPENDS ${CMAKE_BINARY_DIR}/headers.stamp)

add_compile_definitions(__is_libk)
add_subdirectory(libc)
add_subdirectory(kernel)
