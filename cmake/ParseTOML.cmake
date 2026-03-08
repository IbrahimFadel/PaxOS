# cmake/ParseTOML.cmake
# ---------------------------------------------------------------------------
# Lightweight TOML parser for CMake.
# Supports:
#   [sections]          — ignored (all keys are processed regardless of section)
#   key = true/false    — boolean flags
#   key = 123           — integer definitions
#   key = "string"      — string definitions
#   # comments          — skipped
# ---------------------------------------------------------------------------

function(toml_to_compile_definitions TOML_FILE OUT_VAR)
    set(DEFS)
    set(KCONFIG_PREFIX KCONFIG_)

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${TOML_FILE}")
        message(FATAL_ERROR "[ParseTOML] Config file not found: ${CMAKE_SOURCE_DIR}/${TOML_FILE}")
    endif()

    message(STATUS "[ParseTOML] Reading ${TOML_FILE}")
    file(STRINGS "${CMAKE_SOURCE_DIR}/${TOML_FILE}" TOML_LINES)

    foreach(LINE IN LISTS TOML_LINES)
        # Strip inline comments and leading/trailing whitespace
        string(REGEX REPLACE "#.*$" "" LINE "${LINE}")
        string(STRIP "${LINE}" LINE)

        # Skip empty lines and section headers
        if(LINE STREQUAL "" OR LINE MATCHES "^\\[")
            continue()
        endif()

        # Match: key = value
        if(LINE MATCHES "^([A-Za-z_][A-Za-z0-9_]*)[ \t]*=[ \t]*(.+)$")
            set(KEY   "${CMAKE_MATCH_1}")
            set(VALUE "${CMAKE_MATCH_2}")
            string(STRIP "${VALUE}" VALUE)

            # --- Boolean true → -DKEY (bare define, enables #ifdef) ---
            if(VALUE STREQUAL "true")
                list(APPEND DEFS ${KCONFIG_PREFIX}${KEY})

            # --- Boolean false → skip (no define added) ---
            elseif(VALUE STREQUAL "false")

            # --- Quoted string → -DKEY=\"value\" ---
            elseif(VALUE MATCHES "^\"(.*)\"$")
                set(STR_VAL "${CMAKE_MATCH_1}")
                list(APPEND DEFS ${KCONFIG_PREFIX}${KEY}=${KCONFIG_PREFIX}"${STR_VAL}")

            # --- Integer → -DKEY=N ---
            elseif(VALUE MATCHES "^-?[0-9]+$")
                list(APPEND DEFS ${KCONFIG_PREFIX}${KEY}=${KCONFIG_PREFIX}${VALUE})

            else()
                message(WARNING "[ParseTOML]   Skipping unrecognised value for ${KEY}: ${VALUE}")
            endif()
        endif()
    endforeach()
    
    message("Definitions: ${DEFS}")

    set(${OUT_VAR} "${DEFS}" PARENT_SCOPE)
endfunction()
