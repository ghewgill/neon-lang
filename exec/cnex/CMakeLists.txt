set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

set(CNEX_RTL
    lib/binary.c
    lib/io.c
    lib/random.c
    lib/sys.c
)

add_executable(cnex
    array.c
    bytecode.c
    cell.c
    cnex.c
    dictionary.c
    framestack.c
    global.c
    nstring.c
    number.c
    stack.c
    util.c
    ${CNEX_RTL}
)

target_include_directories(cnex
    PRIVATE .
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set_target_properties(cnex PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror -D_POSIX_C_SOURCE=200112L")
endif ()

target_link_libraries(cnex
    bid
)