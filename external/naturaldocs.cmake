if (NOT EXISTS external/NaturalDocs/NaturalDocs)
    execute_process(
        COMMAND python3 ../scripts/extract.py NaturalDocs-1.52.zip NaturalDocs
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
