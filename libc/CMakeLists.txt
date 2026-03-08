cmake_minimum_required(VERSION 3.22.4)

add_library(klibc STATIC
    arch/i386/io.c

    stdio/printf.c
    stdio/putchar.c
    stdio/puts.c
    stdio/snprintf.c
    stdio/sprintf.c
    stdio/vsnprintf.c

    stdlib/abort.c
    stdlib/itoa.c
    stdlib/panic.c
    stdlib/utoa.c

    string/memcmp.c
    string/memcpy.c
    string/memmove.c
    string/memset.c
    string/strcpy.c
    string/strlen.c
    string/strncpy.c
    string/strnlen.c
    string/strrev.c
)
target_include_directories(klibc PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
add_dependencies(klibc sysroot_headers)
target_compile_definitions(klibc PRIVATE ${KCONFIG_COMPILE_DEFS})
