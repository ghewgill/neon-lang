if (NOT EXISTS external/minijson_writer-master)
    execute_process(
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/extract.py ${CMAKE_CURRENT_SOURCE_DIR}/minijson_writer-master.zip .
        WORKING_DIRECTORY external
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
