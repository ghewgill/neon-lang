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

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

add_subdirectory(external)

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
    lib/sys.neon
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
    src/format.cpp
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

set(NEON_JVM
    jvm/neon/Binary.java
    jvm/neon/Datetime.java
    jvm/neon/File.java
    jvm/neon/Global.java
    jvm/neon/Hash.java
    jvm/neon/Io.java
    jvm/neon/Math.java
    jvm/neon/Os.java
    jvm/neon/Sys.java
    jvm/neon/Time.java
    jvm/neon/type/Array.java
    jvm/neon/type/NeonException.java
    jvm/neon/type/Number.java
)
string(REPLACE ".java" ".class" NEON_JVM_CLASSES "${NEON_JVM}")
foreach (java ${NEON_JVM})
    string(REPLACE ".java" ".class" class ${java})
    add_custom_command(
        OUTPUT ${class}
        COMMAND javac -cp jvm ${java}
        DEPENDS ${java}
    )
endforeach ()
add_custom_target(neon_jvm ALL
    DEPENDS ${NEON_JVM_CLASSES}
)

set(JNEX
    exec/jnex/src/org/nevec/rjm/BigIntegerMath.java
    exec/jnex/src/org/nevec/rjm/BigComplex.java
    exec/jnex/src/org/nevec/rjm/Ifactor.java
    exec/jnex/src/org/nevec/rjm/Rational.java
    exec/jnex/src/org/nevec/rjm/Factorial.java
    exec/jnex/src/org/nevec/rjm/Prime.java
    exec/jnex/src/org/nevec/rjm/Bernoulli.java
    exec/jnex/src/org/nevec/rjm/BigDecimalMath.java
    exec/jnex/src/org/neon_lang/jnex/Bytecode.java
    exec/jnex/src/org/neon_lang/jnex/Cell.java
    exec/jnex/src/org/neon_lang/jnex/Executor.java
)
string(REPLACE ".java" ".class" JNEX_CLASSES "${JNEX}")
foreach (java ${JNEX})
    string(REPLACE ".java" ".class" class ${java})
    add_custom_command(
        OUTPUT ${class}
        COMMAND javac -cp exec/jnex/src ${java}
        DEPENDS ${java}
    )
endforeach ()
add_custom_target(jnex ALL
    DEPENDS ${JNEX_CLASSES}
)

add_library(common OBJECT
    src/bytecode.cpp
    src/format.cpp
    src/intrinsic.cpp
    src/number.cpp
    src/support.cpp
)
target_include_directories(common
    PUBLIC external/IntelRDFPMathLib20U1/LIBRARY/src
    PRIVATE external/utfcpp-master/source
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_compile
        lib/file_const_posix.cpp
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(platform_compile
        lib/file_const_posix.cpp
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(platform_compile
        lib/file_const_win32.cpp
    )
endif ()

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
    PRIVATE external/IntelRDFPMathLib20U1/LIBRARY/src
    PRIVATE external/hash-library
    PRIVATE external/minijson_writer-master
    PRIVATE external/utfcpp-master/source
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
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
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
    lib/sys.cpp
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
    PRIVATE src
    PRIVATE external/IntelRDFPMathLib20U1/LIBRARY/src
    PRIVATE external/lib/libffi-3.2.1/include
    PRIVATE external/minijson_writer-master
    PRIVATE external/sqlite-amalgamation-3080803
    PRIVATE external/utfcpp-master/source
)

add_executable(neonc
    src/neonc.cpp
    $<TARGET_OBJECTS:common>
    $<TARGET_OBJECTS:compiler>
)
target_include_directories(neonc
    PRIVATE external/minijson_writer-master
)
target_link_libraries(neonc bid hash-library)

add_executable(neon
    src/main.cpp
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
else (WIN32)
    target_link_libraries(neon dl)
endif (WIN32)
target_link_libraries(neon
    bid
    ${ffi}
    hash-library
    sqlite3
)

add_executable(neonx
    src/neonx.cpp
    src/bundle.cpp
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
    ${ffi}
    hash-library
    minizip
    sqlite3
)
if (WIN32)
    target_link_libraries(neonx wsock32)
else (WIN32)
    target_link_libraries(neonx dl)
endif (WIN32)

add_executable(neonstub
    src/neonstub.cpp
    src/bundle.cpp
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
    ${ffi}
    hash-library
    minizip
    sqlite3
)
if (WIN32)
    target_link_libraries(neonstub wsock32)
else (WIN32)
    target_link_libraries(neonstub dl)
endif (WIN32)

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
    external/utfcpp-master/source
)
target_link_libraries(neondis
    bid
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
    external/utfcpp-master/source
)
target_link_libraries(test_lexer
    bid
    hash-library
)
add_test(
    NAME test_lexer
    COMMAND test_lexer tests/lexer_coverage.neon
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
    external/utfcpp-master/source
)
target_link_libraries(test_parser
    bid
    hash-library
)
add_test(
    NAME test_parser
    COMMAND test_parser tests/parser_coverage.neon
)

add_executable(test_format
    tests/test_format.cpp
    src/format.cpp
    src/number.cpp
)
target_include_directories(test_format PRIVATE
    src
    external/utfcpp-master/source
)
target_link_libraries(test_format bid)
add_test(
    NAME test_format
    COMMAND test_format
)

add_executable(test_number_to_string
    tests/test_number_to_string.cpp
    src/number.cpp
)
target_include_directories(test_number_to_string PRIVATE
    src
)
target_link_libraries(test_number_to_string bid)
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
    COMMAND python scripts/extract_errors.py
    DEPENDS src/*.cpp
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

file(GLOB TESTS t/*.neon)
foreach (TEST ${TESTS})
    get_filename_component(T "${TEST}" NAME)
    add_test(
        NAME "${T}"
        COMMAND python scripts/run_test.py t/${T}
    )
    add_test(
        NAME "helium:${T}"
        COMMAND python scripts/run_test.py --runner "python tools/helium.py" t/${T}
    )
    add_test(
        NAME "cpp:${T}"
        COMMAND python scripts/run_test.py --runner "python scripts/run_cpp.py" t/${T}
    )
    add_test(
        NAME "js:${T}"
        COMMAND python scripts/run_test.py --runner "python scripts/run_js.py" t/${T}
    )
    add_test(
        NAME "jvm:${T}"
        COMMAND python scripts/run_test.py --runner "python scripts/run_jvm.py" t/${T}
    )
    add_test(
        NAME "pynex:${T}"
        COMMAND python scripts/run_test.py --runner "python scripts/run_pynex.py" t/${T}
    )
    add_test(
        NAME "jnex:${T}"
        COMMAND python scripts/run_test.py --runner "python scripts/run_jnex.py" t/${T}
    )
endforeach ()

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
