if (NOT EXISTS sqlite-amalgamation-3080803)
    execute_process(
        COMMAND python ../scripts/extract.py sqlite-amalgamation-3080803.zip .
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
add_library(sqlite3
    sqlite-amalgamation-3080803/sqlite3.c
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(sqlite3 pthread)
endif ()
target_include_directories(sqlite3
    PUBLIC sqlite-amalgamation-3080803
)
