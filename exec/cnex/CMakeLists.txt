set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

# This is required for GCC 8+ because it doesn't like
# casting function pointers between incompatible signatures.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_C_COMPILER_VERSION}" VERSION_GREATER "8")
    add_compile_options(-Wno-cast-function-type)
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_cnex
        rtl_posix.c
        lib/file_posix.c
        lib/mmap_posix.c
        lib/time_posix.c
        lib/time_darwin.c
        lib/os_posix.c
        lib/posix.c
        lib/process_posix.c
        lib/random_posix.c
        lib/runtime_posix.c
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
    set(platform_cnex
        rtl_posix.c
        lib/file_posix.c
        lib/mmap_posix.c
        lib/time_posix.c
        lib/time_linux.c
        lib/os_posix.c
        lib/posix.c
        lib/process_posix.c
        lib/random_posix.c
        lib/runtime_posix.c
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(platform_cnex
        rtl_win32.c
        lib/file_win32.c
        lib/mmap_win32.c
        lib/time_win32.c
        lib/os_win32.c
        lib/process_win32.c
        lib/random_win32.c
        lib/runtime_win32.c
    )
endif ()


set(CNEX_RTL
    lib/binary.c
    lib/datetime.c
    lib/file.c
    lib/io.c
    lib/math.c
    lib/net.c
    lib/os.c
    lib/random.c
    lib/runtime.c
    lib/sqlite.c
    lib/string.c
    lib/struct.c
    lib/sys.c
    lib/textio.c
    ${platform_cnex}
)

add_executable(cnex
    array.c
    bytecode.c
    cell.c
    cJSON.c
    cnex.c
    dictionary.c
    disassembly.c
    extension.c
    framestack.c
    gc.c
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
    PRIVATE ../../common
)

add_executable(test_string_support
    test_string_support.c
    nstring.c
    util.c
)
add_test(
    NAME test_string_support
    COMMAND test_string_support
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
    set_target_properties(test_string_support PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror")
    set_target_properties(test_number_to_string_c PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror")
    set_target_properties(test_path_support PROPERTIES COMPILE_FLAGS "-std=c99 -Wall -Werror")
endif ()

target_link_libraries(cnex
    bid
    sqlite3
)

if (WIN32)
else (WIN32)
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        target_link_libraries(cnex dl)
    endif ()
    target_link_libraries(cnex m)
    target_link_libraries(test_string_support m)
    target_link_libraries(test_number_to_string_c m)
    target_link_libraries(test_path_support m)
endif (WIN32)
