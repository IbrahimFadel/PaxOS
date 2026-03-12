cmake_minimum_required(VERSION 3.22.4)





add_executable(hello_world_write
    ../crt0.s
    main.c
)

target_link_libraries(hello_world_write PRIVATE libc)
target_link_options(hello_world_write PRIVATE -T ${USER_LINKER_FILE})
add_dependencies(hello_world_write user_linker_script_preprocess sysroot_headers)

add_custom_command(TARGET hello_world_write POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MODULES_STAGING_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hello_world_write> ${MODULES_STAGING_DIR}/
    COMMENT "Staging hello_world_write"
)
