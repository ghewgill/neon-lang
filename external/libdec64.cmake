if (NOT EXISTS external/DEC64-alt)
    execute_process(
        COMMAND python3 ../scripts/extract.py DEC64-alt.zip .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
    execute_process(
        COMMAND perl -p -i -e "s/(\\(.*LL\\))/(long long)$1/" external/DEC64-alt/dec64.h
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
add_library(dec64
    DEC64-alt/dec64.c
    DEC64-alt/dec64_math.c
    DEC64-alt/dec64_string.c
)
target_include_directories(dec64
    INTERFACE DEC64-alt
)
