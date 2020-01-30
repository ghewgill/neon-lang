project(gonex)

if (NOT NEON_TEST_DIR)
    message(FATAL_ERROR "no test dir")
endif ()

function(add_tests TESTS TAG RUNNER EXCLUDEFILE)
    set(EXCLUDE "")
    if (EXCLUDEFILE)
        file(STRINGS "${EXCLUDEFILE}" EXCLUDE_WITH_COMMENTS)
        foreach (EXCL ${EXCLUDE_WITH_COMMENTS})
            string(REGEX REPLACE "[ ]*#.*" "" E "${EXCL}")
            list(APPEND EXCLUDE ${E})
        endforeach()
    endif (EXCLUDEFILE)
    foreach (TEST ${TESTS})
        get_filename_component(T "${TEST}" NAME)
        if (TAG)
            if (NOT ";${EXCLUDE};" MATCHES ";${T};")
                add_test(
                    NAME "${TAG}:${T}"
                    COMMAND python3 ${NEON_TEST_DIR}/run_test.py --runner "python3 ${RUNNER}" ${TEST}
                )
            endif()
        else()
            add_test(
                NAME "${T}"
                COMMAND python3 ${NEON_TEST_DIR}/run_test.py ${TEST}
            )
        endif()
    endforeach()
endfunction(add_tests)

find_program(GO go)
if (GO)
    message("go found: ${GO}")

    add_custom_command(
        OUTPUT gonex
        COMMAND ${GO} build
        DEPENDS gonex.go
    )

    add_custom_target(gonex_target ALL
        DEPENDS gonex
    )

    file(GLOB TESTS "${NEON_TEST_DIR}/*.neon")
    add_tests("${TESTS}" "gonex" "run_test.py" "exclude.txt")

else (GO)
    message("go not found")
endif (GO)
