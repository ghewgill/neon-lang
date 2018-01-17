if (NOT EXISTS utfcpp-master)
    execute_process(
        COMMAND python ../scripts/extract.py utfcpp-master.zip .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
