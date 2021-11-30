if (NOT EXISTS external/sqlite-amalgamation-3320300)
    execute_process(
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/extract.py ${CMAKE_CURRENT_SOURCE_DIR}/sqlite-amalgamation-3320300.zip .
        WORKING_DIRECTORY external
        RESULT_VARIABLE retcode
    )
    if (NOT "${retcode}" STREQUAL "0")
        message(FATAL_ERROR "Fatal error extracting archive")
    endif ()
endif ()
add_library(sqlite3
    sqlite-amalgamation-3320300/sqlite3.c
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(sqlite3 pthread)
endif ()
target_include_directories(sqlite3
    INTERFACE sqlite-amalgamation-3320300
)
