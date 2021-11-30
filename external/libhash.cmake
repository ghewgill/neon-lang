if (NOT EXISTS external/hash-library)
    execute_process(
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/extract.py ${CMAKE_CURRENT_SOURCE_DIR}/hash-library.zip hash-library
        WORKING_DIRECTORY external
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        file(GLOB cpps ${CMAKE_CURRENT_BINARY_DIR}/hash-library/*.cpp)
        foreach (fn ${cpps})
            execute_process(
                COMMAND perl -n -i -e "print unless /<endian.h>/" ${fn}
                RESULT_VARIABLE retcode
            )
            if (NOT "${retcode}" STREQUAL "0")
                message(FATAL_ERROR "Fatal error extracting archive")
            endif ()
        endforeach ()
    endif ()
endif ()
add_library(hash-library
    hash-library/crc32.cpp
    hash-library/md5.cpp
    hash-library/sha1.cpp
    hash-library/sha256.cpp
    hash-library/sha3.cpp
)
target_include_directories(hash-library
    PUBLIC hash-library
)
