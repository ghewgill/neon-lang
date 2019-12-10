if (NOT EXISTS external/pyparsing-2.0.3)
    execute_process(
        COMMAND python3 ../scripts/extract.py pyparsing-2.0.3.tar.gz .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
