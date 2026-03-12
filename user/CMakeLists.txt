cmake_minimum_required(VERSION 3.22.4)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

set(USER_LINKER_FILE_UNPROCESSED ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
set(USER_LINKER_FILE ${CMAKE_BINARY_DIR}/user/linker.ld)

add_custom_command(
    OUTPUT
        ${USER_LINKER_FILE}
    COMMAND
        ${CMAKE_C_COMPILER}
            -E -P -x c
            ${USER_LINKER_FILE_UNPROCESSED}
            -o ${USER_LINKER_FILE}
    DEPENDS ${USER_LINKER_FILE_UNPROCESSED}
    COMMENT "Preprocessing user linker script"
)

add_custom_target(user_linker_script_preprocess
    DEPENDS ${USER_LINKER_FILE}
)

set(LIBC_NAME libc)
set(LIBC_COMPILE_DEFINITIONS )
add_subdirectory(../libc libc)

set(MODULE_TARGETS
  hello_world_write
)

set(MODULES_STAGING_DIR ${CMAKE_BINARY_DIR}/modules_staging)
set(MODULE_FILES "")

foreach(target ${MODULE_TARGETS})
  add_subdirectory(${target})
  list(APPEND MODULE_FILES $<TARGET_FILE:${target}>)
endforeach()

file(RELATIVE_PATH INITRD_RELATIVE_PATH
  ${MODULES_STAGING_DIR}
  ${CMAKE_SOURCE_DIR}/iso/boot/initrd.img
)

add_custom_target(modules
  COMMAND ${CMAKE_COMMAND} -E chdir ${MODULES_STAGING_DIR}
          tar -cf ${INITRD_RELATIVE_PATH} .
  DEPENDS ${MODULE_TARGETS}
  COMMENT "Packing modules into initrd.img"
)
