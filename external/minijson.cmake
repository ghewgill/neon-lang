if (NOT EXISTS external/minijson_writer-master)
    execute_process(
        COMMAND python ../scripts/extract.py minijson_writer-master.zip .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
