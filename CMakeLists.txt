# This is currently the beginnings of a cmake build file for neon.
# It is not complete and the best way to build neon now is with scons.
# This may depend on things that are built by a regular scons build,
# but over time that dependency may be eliminated and this will be
# a standalone build system.

cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0054 NEW)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)

find_program(JAVAC javac)
if (JAVAC)
    execute_process(
        COMMAND javac -version
        OUTPUT_VARIABLE JAVAC_VERSION_OUT
        ERROR_VARIABLE JAVAC_VERSION_ERR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
    if ("${JAVAC_VERSION_OUT}" MATCHES "javac ([0-9]+\\.[0-9]+)"
     OR "${JAVAC_VERSION_ERR}" MATCHES "javac ([0-9]+\\.[0-9]+)")
        if (NOT "${CMAKE_MATCH_1}" VERSION_LESS "1.8")
            message("javac found: ${JAVAC}")
        else ()
            message("javac found, but version ${CMAKE_MATCH_1} is < 1.8")
            set(JAVAC "JAVAC-NOTFOUND")
        endif ()
    else ()
        message("javac not found")
        set(JAVAC "JAVAC-NOTFOUND")
    endif ()
else (JAVAC)
    message("javac not found")
endif (JAVAC)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    find_program(CSC csc)
    if (CSC)
        message("csc found: ${CSC}")
    else (CSC)
        message("csc not found")
    endif (CSC)
else ()
    find_program(MONO mono)
    if (MONO)
        message("mono found: ${MONO}")
    else (MONO)
        message("mono not found")
    endif (MONO)
    find_program(CSC mcs)
    if (CSC)
        message("mcs found: ${CSC}")
    else (CSC)
        message("mcs not found")
        set(MONO "MONO-NOTFOUND")
    endif (CSC)
endif ()

find_program(NODEJS NAMES nodejs node)
if (NODEJS)
    message("nodejs found: ${NODEJS}")
else (NODEJS)
    message("nodejs not found")
endif (NODEJS)

find_program(RUSTC rustc)
if (RUSTC)
    message("rustc found: ${RUSTC}")
else (RUSTC)
    message("rustc not found")
endif (RUSTC)

find_program(GO go)
if (GO)
    message("go found: ${GO}")
else (GO)
    message("go not found")
endif (GO)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -g")
endif ()

add_subdirectory(external)

if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
    include_directories(/usr/local/include)
endif ()

set(RTL_NEON
    lib/binary.neon
    lib/datetime.neon
    lib/debugger.neon
    lib/file.neon
    lib/global.neon
    lib/io.neon
    lib/math.neon
    lib/mmap.neon
    lib/net.neon
    lib/os.neon
    lib/process.neon
    lib/random.neon
    lib/runtime.neon
    lib/sqlite.neon
    lib/string.neon
    lib/struct.neon
    lib/sys.neon
    lib/textio.neon
    lib/time.neon
)
if (WIN32)
else (WIN32)
    set(RTL_NEON "${RTL_NEON};lib/posix.neon")
endif (WIN32)

add_custom_command(
    OUTPUT src/thunks.inc src/functions_compile.inc src/functions_exec.inc src/enums.inc src/exceptions.inc
    COMMAND python scripts/make_thunks.py ${RTL_NEON}
    DEPENDS scripts/make_thunks.py
    DEPENDS ${RTL_NEON}
)
set_source_files_properties(
    src/ast.cpp
    src/intrinsic.cpp
    PROPERTIES OBJECT_DEPENDS src/exceptions.inc
)

add_custom_command(
    OUTPUT src/unicodedata.inc
    COMMAND python tools/helium.py scripts/make_unicode.neon data/UnicodeData.txt >src/unicodedata.inc
    DEPENDS tools/helium.py
    DEPENDS scripts/make_unicode.neon
    DEPENDS data/UnicodeData.txt
)
set_source_files_properties(
    src/lexer.cpp
    PROPERTIES OBJECT_DEPENDS src/unicodedata.inc
)

if (JAVAC)
    set(NEON_JVM
        rtl/jvm/neon/Binary.java
        rtl/jvm/neon/Datetime.java
        rtl/jvm/neon/File.java
        rtl/jvm/neon/Global.java
        rtl/jvm/neon/Hash.java
        rtl/jvm/neon/Io.java
        rtl/jvm/neon/Math.java
        rtl/jvm/neon/Os.java
        rtl/jvm/neon/Sys.java
        rtl/jvm/neon/Textio.java
        rtl/jvm/neon/Time.java
        rtl/jvm/neon/type/Array.java
        rtl/jvm/neon/type/NeonException.java
        rtl/jvm/neon/type/Number.java
    )
    string(REPLACE ".java" ".class" NEON_JVM_CLASSES "${NEON_JVM}")
    foreach (java ${NEON_JVM})
        string(REPLACE ".java" ".class" class ${java})
        add_custom_command(
            OUTPUT ${class}
            COMMAND javac -cp rtl/jvm ${java}
            DEPENDS ${java}
        )
    endforeach ()
    add_custom_target(neon_jvm ALL
        DEPENDS ${NEON_JVM_CLASSES}
    )
endif (JAVAC)

if (CSC)
    set(NEON_CLI
        rtl/cli/Global.cs
    )
    # TODO: Use cmake C# support (3.8.2+)
    add_custom_command(
        OUTPUT t/Neon.dll
        COMMAND ${CSC} /out:t/Neon.dll /target:library ${NEON_CLI}
        DEPENDS ${NEON_CLI}
    )
    add_custom_target(neon_cli ALL
        DEPENDS t/Neon.dll
    )
endif (CSC)

if (JAVAC)
    add_subdirectory("exec/jnex")
endif (JAVAC)
add_subdirectory("exec/cnex")
if (RUSTC)
    add_subdirectory("exec/rsnex")
endif (RUSTC)
if (GO)
    add_subdirectory("exec/gonex")
endif (GO)

add_library(common OBJECT
    src/bytecode.cpp
    src/intrinsic.cpp
    src/number.cpp
    src/support.cpp
)
target_include_directories(common
    PUBLIC external/IntelRDFPMathLib20U2/LIBRARY/src
    PRIVATE external/utfcpp-master/source
    PRIVATE external/gmp-6.1.2
)
add_dependencies(common
    bid
    gmp
)

add_custom_command(
    OUTPUT src/rtl.inc
    COMMAND python scripts/build_rtl_inc.py ${RTL_NEON}
    DEPENDS scripts/build_rtl_inc.py
    DEPENDS ${RTL_NEON}
)
add_library(compiler OBJECT
    src/analyzer.cpp
    src/ast.cpp
    src/compiler.cpp
    src/compiler_cli.cpp
    src/compiler_cpp.cpp
    src/compiler_js.cpp
    src/compiler_jvm.cpp
    src/debuginfo.cpp
    src/disassembler.cpp
    src/lexer.cpp
    src/parser.cpp
    src/pt_dump.cpp
    src/rtl_compile.cpp
    src/sql.cpp
    src/support_compiler.cpp
    src/util.cpp
    ${platform_compile}
)
set_source_files_properties(
    src/support_compiler.cpp
    PROPERTIES OBJECT_DEPENDS src/rtl.inc
)
target_include_directories(compiler
    PRIVATE external/IntelRDFPMathLib20U2/LIBRARY/src
    PRIVATE external/hash-library
    PRIVATE external/minijson_writer-master
    PRIVATE external/utfcpp-master/source
    PRIVATE external/gmp-6.1.2
)
add_dependencies(compiler
    bid
    gmp
    hash-library
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_executor
        src/rtl_posix.cpp
        lib/file_posix.cpp
        lib/mmap_posix.cpp
        lib/os_posix.cpp
        lib/posix.cpp
        lib/process_posix.cpp
        lib/time_posix.cpp
        lib/time_darwin.cpp
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
    set(platform_executor
        src/rtl_posix.cpp
        lib/file_posix.cpp
        lib/mmap_posix.cpp
        lib/os_posix.cpp
        lib/posix.cpp
        lib/process_posix.cpp
        lib/time_posix.cpp
        lib/time_linux.cpp
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(platform_executor
        src/rtl_win32.cpp
        lib/file_win32.cpp
        lib/mmap_win32.cpp
        lib/os_win32.cpp
        lib/process_win32.cpp
        lib/time_win32.cpp
    )
endif ()

# Needed for USE_RTLX
#string(REPLACE "lib/global.neon" "" RTL_NEON_WITHOUT_GLOBAL "${RTL_NEON}")
#set(RTL_NEONX "")
#foreach (src ${RTL_NEON_WITHOUT_GLOBAL})
#    string(REPLACE ".neon" ".neonx" NEONX ${src})
#    add_custom_command(
#        OUTPUT "${src}x"
#        COMMAND neonc ${src}
#        DEPENDS ${src}
#    )
#    list(APPEND RTL_NEONX ${NEONX})
#endforeach ()
#add_custom_command(
#    OUTPUT src/rtlx.inc
#    COMMAND python scripts/build_rtlx_inc.py ${RTL_NEONX}
#    DEPENDS scripts/build_rtlx_inc.py
#    DEPENDS ${RTL_NEONX}
#)

add_library(executor OBJECT
    src/cell.cpp
    src/exec.cpp
    src/httpserver.cpp
    src/rtl_exec.cpp
    src/support_exec.cpp
    lib/binary.cpp
    lib/datetime.cpp
    lib/debugger.cpp
    lib/global.cpp
    lib/file.cpp
    lib/io.cpp
    lib/math.cpp
    lib/net.cpp
    lib/os.cpp
    lib/random.cpp
    lib/runtime.cpp
    lib/sqlite.cpp
    lib/string.cpp
    lib/struct.cpp
    lib/sys.cpp
    lib/textio.cpp
    lib/time.cpp
    ${platform_executor}
)
#set_source_files_properties(
#    src/support_exec.cpp
#    PROPERTIES OBJECT_DEPENDS src/rtlx.inc
#)
target_compile_options(executor PRIVATE -DFFI_BUILDING)
set_source_files_properties(
    src/exec.cpp
    PROPERTIES OBJECT_DEPENDS src/exceptions.inc
)
target_include_directories(executor
    PRIVATE common
    PRIVATE src
    PRIVATE external/IntelRDFPMathLib20U2/LIBRARY/src
    PRIVATE external/lib/libffi-3.2.1/include
    PRIVATE external/minijson_writer-master
    PRIVATE external/sqlite-amalgamation-3080803
    PRIVATE external/utfcpp-master/source
    PRIVATE external/gmp-6.1.2
)
add_dependencies(executor
    bid
    gmp
    hash-library
    sqlite3
)

add_executable(neonc
    src/neonc.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:compiler>
)
target_include_directories(neonc
    PRIVATE external/minijson_writer-master
    PRIVATE external/utfcpp-master/source
)
target_link_libraries(neonc
    bid
    gmp
    hash-library
)

add_executable(neon
    src/neon.cpp
    src/repl.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:compiler>
    $<TARGET_OBJECTS:executor>
)
target_include_directories(neon PRIVATE
    src
    external/minijson_writer-master
    external/utfcpp-master/source
)
if (WIN32)
    target_link_libraries(neon wsock32)
endif (WIN32)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(neon dl)
endif ()
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(neon pthread)
endif ()
target_link_libraries(neon
    bid
    gmp
    ${ffi}
    hash-library
    sqlite3
)

add_executable(neonx
    src/neonx.cpp
    src/bundle.cpp
    src/disassembler.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:executor>
)
target_include_directories(neonx PRIVATE
    src
    external/minijson_writer-master
    external/utfcpp-master/source
)
target_link_libraries(neonx
    bid
    gmp
    ${ffi}
    hash-library
    minizip
    sqlite3
)
if (WIN32)
    target_link_libraries(neonx wsock32)
endif (WIN32)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(neonx dl)
endif ()
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(neonx pthread)
endif ()

add_executable(neonstub
    src/neonstub.cpp
    src/bundle.cpp
    src/disassembler.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:executor>
)
target_include_directories(neonstub PRIVATE
    src
    external/minijson_writer-master
    external/utfcpp-master/source
)
target_link_libraries(neonstub
    bid
    gmp
    ${ffi}
    hash-library
    minizip
    sqlite3
)
if (WIN32)
    target_link_libraries(neonstub wsock32)
endif (WIN32)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(neonstub dl)
endif ()
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(neonstub pthread)
endif ()

add_executable(neondis
    src/bytecode.cpp
    src/debuginfo.cpp
    src/disassembler.cpp
    src/neondis.cpp
    src/number.cpp
    # The following are just to support internal_error()
    src/lexer.cpp
    src/util.cpp
)
target_include_directories(neondis PRIVATE
    external/minijson_writer-master
    external/utfcpp-master/source
)
target_link_libraries(neondis
    bid
    gmp
    hash-library
)

add_executable(neonbind
    src/bytecode.cpp
    src/neonbind.cpp
    src/support.cpp
    src/support_exec.cpp
)
target_include_directories(neonbind PRIVATE
    src
)
target_link_libraries(neonbind
    minizip
)

add_executable(test_lexer
    tests/test_lexer.cpp
    src/lexer.cpp
    src/number.cpp
    src/util.cpp
)
target_include_directories(test_lexer PRIVATE
    src
    external/minijson_writer-master
    external/utfcpp-master/source
)
target_link_libraries(test_lexer
    bid
    gmp
    hash-library
)
add_test(
    NAME test_lexer
    COMMAND test_lexer tests/lexer_coverage.neon
)

add_executable(test_number
    tests/test_number.cpp
    src/number.cpp
)
target_include_directories(test_number PRIVATE
    src
)
target_link_libraries(test_number
    bid
    gmp
)
add_test(
    NAME test_number
    COMMAND test_number
)

add_executable(test_parser
    tests/test_parser.cpp
    src/lexer.cpp
    src/number.cpp
    src/parser.cpp
    src/pt_dump.cpp
    src/sql.cpp
    src/util.cpp
)
target_include_directories(test_parser PRIVATE
    src
    external/minijson_writer-master
    external/utfcpp-master/source
)
target_link_libraries(test_parser
    bid
    gmp
    hash-library
)
add_test(
    NAME test_parser
    COMMAND test_parser tests/parser_coverage.neon
)

add_executable(test_number_to_string
    tests/test_number_to_string.cpp
    src/number.cpp
)
target_include_directories(test_number_to_string PRIVATE
    src
)
target_link_libraries(test_number_to_string
    bid
    gmp
)
add_test(
    NAME test_number_to_string
    COMMAND test_number_to_string
)

if (WIN32)
    add_library(libtest_ffi SHARED
        tests/test_ffi.c
    )
else (WIN32)
    add_library(test_ffi SHARED
        tests/test_ffi.c
    )
endif (WIN32)

add_custom_command(
    OUTPUT src/errors.txt
    COMMAND python tools/helium.py scripts/extract_errors.neon
    DEPENDS src/lexer.cpp
    DEPENDS src/parser.cpp
    DEPENDS src/analyzer.cpp
    DEPENDS src/sql.cpp
    DEPENDS src/ast.cpp
)
add_custom_target(errors_txt ALL
    DEPENDS src/errors.txt
)

add_custom_command(
    OUTPUT contrib/grammar/neon.w3c.ebnf
    COMMAND python tools/helium.py contrib/grammar/ebnf_w3c.neon <contrib/grammar/neon.ebnf >contrib/grammar/neon.w3c.ebnf
    DEPENDS tools/helium.py
    DEPENDS contrib/grammar/ebnf_w3c.neon
    DEPENDS contrib/grammar/neon.ebnf
)
add_custom_target(w3c_ebnf ALL
    DEPENDS contrib/grammar/neon.w3c.ebnf
)

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
                    COMMAND python scripts/run_test.py --runner "python ${RUNNER}" ${TEST}
                )
            endif()
        else()
            add_test(
                NAME "${T}"
                COMMAND python scripts/run_test.py ${TEST}
            )
        endif()
    endforeach()
endfunction(add_tests)

file(GLOB TESTS t/*.neon)
add_tests("${TESTS}" "" "" "")
add_tests("${TESTS}" "helium" "tools/helium.py" "tools/helium-exclude.txt")
add_tests("${TESTS}" "cpp" "scripts/run_cpp.py" "scripts/cpp-exclude.txt")
if (NODEJS)
    add_tests("${TESTS}" "js" "scripts/run_js.py" "scripts/js-exclude.txt")
endif (NODEJS)
if (JAVAC)
    add_tests("${TESTS}" "jvm" "scripts/run_jvm.py" "scripts/jvm-exclude.txt")
endif (JAVAC)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" OR MONO)
    add_tests("${TESTS}" "cli" "scripts/run_cli.py" "scripts/cli-exclude.txt")
endif ()
add_tests("${TESTS}" "nenex" "exec/nenex/run_test.py" "exec/nenex/exclude.txt")
add_tests("${TESTS}" "pynex" "exec/pynex/run_test.py" "exec/pynex/exclude.txt")
if (JAVAC)
    add_tests("${TESTS}" "jnex" "exec/jnex/run_test.py" "exec/jnex/exclude.txt")
endif (JAVAC)
add_tests("${TESTS}" "cnex" "exec/cnex/run_test.py" "exec/cnex/exclude.txt")
if (RUSTC)
    add_tests("${TESTS}" "rsnex" "exec/rsnex/run_test.py" "exec/rsnex/exclude.txt")
endif (RUSTC)
if (GO)
    add_tests("${TESTS}" "gonex" "exec/gonex/run_test.py" "exec/gonex/exclude.txt")
endif (GO)

add_test(
    NAME "errors"
    COMMAND python scripts/run_test.py --errors t/errors
)

file(GLOB TESTS_REPL t/repl_*.neon)
foreach (TEST ${TESTS_REPL})
    get_filename_component(T "${TEST}" NAME)
    add_test(
        NAME "repl:${T}"
        COMMAND neon --repl-no-prompt --repl-stop-on-any-error ${TEST}
    )
endforeach ()

file(GLOB_RECURSE SAMPLES samples/*.neon)
# TODO: Compile samples

# TODO: needs pyparsing
#add_test(
#    NAME "test_grammar"
#    COMMAND python contrib/grammar/test-grammar.py lib/*.neon neon/*.neon tools/*.neon ${SAMPLES} t/*.neon t/errors/N3*.neon
#)
#add_test(
#    NAME "test_grammar_random"
#    COMMAND python contrib/grammar/test-random.py
#)

add_test(
    NAME "test_doc"
    COMMAND python scripts/test_doc.py
)

# TODO: coverage tests
