set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

set(CNEX_RTL
    lib/binary.c
    lib/io.c
    lib/math.c
    lib/random.c
    lib/runtime.c
    lib/sys.c
    lib/textio.c
)

add_executable(cnex
    array.c
    bytecode.c
    cell.c
    cnex.c
    dictionary.c
    framestack.c
    global.c
    module.c
    nstring.c
    number.c
    object.c
    stack.c
    support.c
    util.c
    ${CNEX_RTL}
)

target_include_directories(cnex
    PRIVATE .
)

add_executable(test_number_to_string_c
    test_number_to_string.c
    number.c
)
target_link_libraries(test_number_to_string_c
    bid
)
add_test(
    NAME test_number_to_string_c
    COMMAND test_number_to_string_c
)

add_executable(test_path_support
    test_path_support.c
    support.c
    util.c
)
add_test(
    NAME test_path_support
    COMMAND test_path_support
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set_target_properties(cnex PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror")
    set_target_properties(test_number_to_string_c PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror")
    set_target_properties(test_path_support PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror")
endif ()

target_link_libraries(cnex
    bid
)

if (WIN32)
else (WIN32)
    target_link_libraries(cnex m)
    target_link_libraries(test_number_to_string_c m)
    target_link_libraries(test_path_support m)
endif (WIN32)
