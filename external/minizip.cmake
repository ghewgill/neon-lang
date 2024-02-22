if (NOT EXISTS external/minizip11)
    execute_process(
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/extract.py ${CMAKE_CURRENT_SOURCE_DIR}/unzip11.zip minizip11
        WORKING_DIRECTORY external
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
if (WIN32)
    set(minizip_platform
        minizip11/iowin32.c
    )
endif ()
add_library(minizip
    minizip11/ioapi.c
    minizip11/unzip.c
    minizip11/zip.c
    ${minizip_platform}
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "NetBSD")
    set_source_files_properties(minizip11/ioapi.c PROPERTIES COMPILE_DEFINITIONS "fopen64=fopen;fseeko64=fseeko;ftello64=ftello")
endif ()
target_include_directories(minizip
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/minizip11
)
target_link_libraries(minizip ${ZLIB_TARGET})
