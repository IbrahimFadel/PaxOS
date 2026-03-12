cmake_minimum_required(VERSION 3.22.4)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

set(USER_LINKER_FILE_UNPROCESSED ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
set(USER_LINKER_FILE ${CMAKE_BINARY_DIR}/user/linker.ld)

set(C_SRCS
	src/crt0.s
	src/main.c
)

add_custom_command(
    OUTPUT
        ${USER_LINKER_FILE}
    COMMAND
        ${CMAKE_C_COMPILER}
            -E -P -x c
            ${USER_LINKER_FILE_UNPROCESSED}
            -o ${USER_LINKER_FILE}
    DEPENDS ${USER_LINKER_FILE_UNPROCESSED}
    COMMENT "Preprocessing kernel linker script"
)

add_custom_target(user_linker_script_preprocess
    DEPENDS ${USER_LINKER_FILE}
)

add_executable(user_main)
target_sources(user_main PRIVATE ${C_SRCS})
target_include_directories(user_main PRIVATE )
target_link_libraries(user_main PRIVATE libc)
target_link_options(user_main PRIVATE -T ${USER_LINKER_FILE})
add_dependencies(user_main user_linker_script_preprocess sysroot_headers)

set(LIBC_NAME libc)
set(LIBC_COMPILE_DEFINITIONS )
add_subdirectory(../libc libc)
