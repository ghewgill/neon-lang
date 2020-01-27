if (ZLIB_FOUND)
    set(ZLIB_TARGET "ZLIB::ZLIB")
else ()
    if (NOT EXISTS external/zlib-1.2.8)
        execute_process(
            COMMAND python3 ../scripts/extract.py zlib-1.2.8.tar.gz .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE retcode
        )
        if (NOT "${retcode}" STREQUAL "0")
            message(FATAL_ERROR "Fatal error extracting archive")
        endif ()
    endif ()
    add_library(ZLIB
        zlib-1.2.8/adler32.c
        zlib-1.2.8/compress.c
        zlib-1.2.8/crc32.c
        zlib-1.2.8/deflate.c
        zlib-1.2.8/inffast.c
        zlib-1.2.8/inflate.c
        zlib-1.2.8/inftrees.c
        zlib-1.2.8/trees.c
        zlib-1.2.8/uncompr.c
        zlib-1.2.8/zutil.c
    )
    target_include_directories(ZLIB
        INTERFACE zlib-1.2.8
    )
    set(ZLIB_TARGET "ZLIB")
endif ()
set(ZLIB_TARGET "${ZLIB_TARGET}" PARENT_SCOPE)
