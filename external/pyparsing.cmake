if (NOT EXISTS external/pyparsing-2.4.5)
    execute_process(
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/extract.py ${CMAKE_CURRENT_SOURCE_DIR}/pyparsing-2.4.5.tar.gz .
        WORKING_DIRECTORY external
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
