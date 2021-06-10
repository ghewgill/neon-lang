cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0054 NEW)
project(neon)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)

find_program(JAVAC javac)
if (JAVAC)
    execute_process(
        COMMAND javac -version
        OUTPUT_VARIABLE JAVAC_VERSION_OUT
        ERROR_VARIABLE JAVAC_VERSION_ERR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    )
    if ("${JAVAC_VERSION_OUT}" MATCHES "javac ([0-9]+\\.[0-9]+)")
        set(JAVAC_VERSION "${CMAKE_MATCH_1}")
    endif ()
    if ("${JAVAC_VERSION_ERR}" MATCHES "javac ([0-9]+\\.[0-9]+)")
        set(JAVAC_VERSION "${CMAKE_MATCH_1}")
    endif ()
    if ("${JAVAC_VERSION}")
        if (NOT "${JAVAC_VERSION}" VERSION_LESS "1.8")
            message("javac found: ${JAVAC}")
        else ()
            message("javac found, but version ${JAVAC_VERSION} is < 1.8")
            set(JAVAC "JAVAC-NOTFOUND")
        endif ()
    else ()
        message("javac not found")
        set(JAVAC "JAVAC-NOTFOUND")
    endif ()
else (JAVAC)
    message("javac not found")
endif (JAVAC)
find_program(JAVA java)
if (NOT JAVA)
    message("java not found")
endif (NOT JAVA)

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

if (WIN32)
    find_program(SCONS scons.bat)
else()
    find_program(SCONS scons)
endif()
if (SCONS)
    message("scons found: ${SCONS}")
else (SCONS)
    message("scons not found")
endif (SCONS)

execute_process(
    COMMAND git describe --always --long --dirty
    OUTPUT_VARIABLE GIT_DESCRIBE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set_property(DIRECTORY APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/.git/index
)
configure_file("src/version.cpp.in" "gen/version.cpp")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options(/EHsc /W2 /WX /FS /wd4324)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    add_compile_options(-Wall -Wextra -Werror -g)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GMP)
find_package(ZLIB)
add_subdirectory(external)

if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
    include_directories(/usr/local/include)
endif ()

file(GLOB EXTENSION_DIRS "${CMAKE_CURRENT_LIST_DIR}/lib/*")
foreach (extension_dir ${EXTENSION_DIRS})
    if (NOT IS_DIRECTORY ${extension_dir})
        continue()
    endif()
    get_filename_component(libname ${extension_dir} NAME)
    if (EXISTS "${extension_dir}/CMakeLists.txt")
        add_custom_target(neon_${libname} ALL
            COMMAND cmake .
            COMMAND cmake --build .
            WORKING_DIRECTORY ${extension_dir}
        )
        message("building extension module ${libname} with cmake")
    elseif (EXISTS "${extension_dir}/SConstruct" AND SCONS)
        add_custom_target(neon_${libname} ALL
            COMMAND ${SCONS}
            WORKING_DIRECTORY ${extension_dir}
        )
        message("building extension module ${libname} with scons")
    elseif (EXISTS "${extension_dir}/configure")
        add_custom_target(neon_${libname} ALL
            COMMAND ./configure
            COMMAND make
            WORKING_DIRECTORY ${extension_dir}
        )
        message("building extension module ${libname} with configure")
    elseif (EXISTS "${extension_dir}/build" AND NOT WIN32)
        add_custom_target(neon_${libname} ALL
            COMMAND ./build
            WORKING_DIRECTORY ${extension_dir}
        )
        message("building extension module ${libname} with build")
    elseif (EXISTS "${extension_dir}/build.cmd" AND WIN32)
        add_custom_target(neon_${libname} ALL
            COMMAND build.cmd
            WORKING_DIRECTORY ${extension_dir}
        )
        message("building extension module ${libname} with build.cmd")
    else ()
        message("skipping extension module ${libname} because no usable build method found")
    endif ()
endforeach ()

set(RTL_NEON
    lib/base.neon
    lib/binary.neon
    lib/complex.neon
    lib/console.neon
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
    OUTPUT gen/thunks.inc gen/functions_compile.inc gen/functions_exec.inc gen/enums.inc gen/exceptions.inc
    COMMAND python3 scripts/make_thunks.py ${RTL_NEON}
    DEPENDS scripts/make_thunks.py
    DEPENDS ${RTL_NEON}
)
set_source_files_properties(
    src/ast.cpp
    src/intrinsic.cpp
    PROPERTIES OBJECT_DEPENDS gen/exceptions.inc
)

add_custom_command(
    OUTPUT gen/unicodedata.inc
    COMMAND python3 tools/helium.py scripts/make_unicode.neon data/UnicodeData.txt >gen/unicodedata.inc
    DEPENDS tools/helium.py
    DEPENDS scripts/make_unicode.neon
    DEPENDS data/UnicodeData.txt
)
set_source_files_properties(
    src/lexer.cpp
    PROPERTIES OBJECT_DEPENDS gen/unicodedata.inc
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
        rtl/jvm/neon/String.java
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
    string(REPLACE "lib/global.neon" "" RTL_NEON_WITHOUT_GLOBAL "${RTL_NEON}")
    string(REPLACE "lib/struct.neon" "" RTL_NEON_WITHOUT_GLOBAL "${RTL_NEON_WITHOUT_GLOBAL}") # TODO
    string(REPLACE ".neon" ".class" RTL_CLASSES "${RTL_NEON_WITHOUT_GLOBAL}")
    foreach (rtl_neon ${RTL_NEON_WITHOUT_GLOBAL})
        string(REPLACE ".neon" ".class" class ${rtl_neon})
        add_custom_command(
            OUTPUT ${class}
            COMMAND neonc -q -t jvm ${rtl_neon}
            DEPENDS neonc ${rtl_neon}
        )
    endforeach ()
    add_custom_target(neon_jvm_rtl ALL
        DEPENDS ${RTL_CLASSES}
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
if (CSC)
    add_subdirectory("exec/csnex")
endif (CSC)
if (RUSTC)
    add_subdirectory("exec/rsnex")
endif (RUSTC)
if (GO)
    add_subdirectory("exec/gonex")
endif (GO)

add_library(common STATIC
    src/bytecode.cpp
    src/disassembler.cpp
    src/intrinsic.cpp
    src/number.cpp
    src/support.cpp
    gen/version.cpp
)
target_include_directories(common
    INTERFACE external/hash-library
    INTERFACE external/minijson_writer-master
    PUBLIC external/utfcpp-master/source
    PRIVATE gen
)
target_link_libraries(common
    bid
    ${GMP_TARGET}
    hash-library
)

add_custom_command(
    OUTPUT gen/rtl.inc
    COMMAND python3 scripts/build_rtl_inc.py ${RTL_NEON}
    DEPENDS scripts/build_rtl_inc.py
    DEPENDS ${RTL_NEON}
)
add_library(compiler STATIC
    src/analyzer.cpp
    src/ast.cpp
    src/compiler.cpp
    src/debuginfo.cpp
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
    PROPERTIES OBJECT_DEPENDS gen/rtl.inc
)
target_include_directories(compiler
    PRIVATE gen
)
target_link_libraries(compiler
    common
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_executor
        src/rtl_posix.cpp
        lib/file_posix.cpp
        lib/mmap_posix.cpp
        lib/os_posix.cpp
        lib/posix.cpp
        lib/process_posix.cpp
        lib/random_posix.cpp
        lib/runtime_posix.cpp
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
        lib/random_posix.cpp
        lib/runtime_posix.cpp
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
        lib/random_win32.cpp
        lib/runtime_win32.cpp
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
#    OUTPUT gen/rtlx.inc
#    COMMAND python3 scripts/build_rtlx_inc.py ${RTL_NEONX}
#    DEPENDS scripts/build_rtlx_inc.py
#    DEPENDS ${RTL_NEONX}
#)

add_library(executor STATIC
    src/cell.cpp
    src/exec.cpp
    src/httpserver.cpp
    src/object.cpp
    src/rtl_exec.cpp
    src/support_exec.cpp
    lib/binary.cpp
    lib/console.cpp
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
#    PROPERTIES OBJECT_DEPENDS gen/rtlx.inc
#)
target_compile_options(executor PRIVATE)
set_source_files_properties(
    src/exec.cpp
    PROPERTIES OBJECT_DEPENDS gen/exceptions.inc
)
target_include_directories(executor
    PRIVATE common
    PRIVATE gen
    PRIVATE src
)
target_link_libraries(executor
    common
    sqlite3
)
if (WIN32)
    target_link_libraries(executor wsock32)
endif (WIN32)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(executor dl)
endif ()
if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    target_link_libraries(executor pthread)
endif ()

add_executable(neonc
    src/neonc.cpp
    src/compiler_cli.cpp
    src/compiler_c.cpp
    src/compiler_cpp.cpp
    src/compiler_js.cpp
    src/compiler_jvm.cpp
)
target_include_directories(neonc
    PRIVATE gen
)
target_link_libraries(neonc
    compiler
)

add_executable(neon
    src/neon.cpp
    src/repl.cpp
)
target_link_libraries(neon
    compiler
    executor
)

add_executable(neonx
    src/neonx.cpp
    src/bundle.cpp
)
target_link_libraries(neonx
    executor
    minizip
)

add_executable(neonstub
    src/neonstub.cpp
    src/bundle.cpp
)
target_link_libraries(neonstub
    executor
    minizip
)

add_executable(neondis
    src/neondis.cpp
)
target_link_libraries(neondis
    compiler
)

add_executable(neonbind
    src/neonbind.cpp
    src/support_exec.cpp
)
target_link_libraries(neonbind
    common
    minizip
)

add_executable(fuzz_lexer
    tests/fuzz_lexer.cpp
)
target_include_directories(fuzz_lexer PRIVATE
    src
)
target_link_libraries(fuzz_lexer
    compiler
)

add_executable(fuzz_parser
    tests/fuzz_parser.cpp
)
target_include_directories(fuzz_parser PRIVATE
    src
)
target_link_libraries(fuzz_parser
    compiler
)

add_executable(perf_lexer
    tests/perf_lexer.cpp
)
target_include_directories(perf_lexer PRIVATE
    src
)
target_link_libraries(perf_lexer
    compiler
)

add_executable(test_lexer
    tests/test_lexer.cpp
)
target_include_directories(test_lexer PRIVATE
    src
)
target_link_libraries(test_lexer
    compiler
)
add_test(
    NAME test_lexer
    COMMAND test_lexer
)
add_test(
    NAME test_lexer_coverage
    COMMAND test_lexer tests/lexer-coverage.neon
)

add_executable(test_number
    tests/test_number.cpp
)
target_include_directories(test_number PRIVATE
    src
)
target_link_libraries(test_number
    common
)
add_test(
    NAME test_number
    COMMAND test_number
)

add_executable(test_parser
    tests/test_parser.cpp
)
target_include_directories(test_parser PRIVATE
    src
)
target_link_libraries(test_parser
    compiler
)
add_test(
    NAME test_parser
    COMMAND test_parser tests/parser_coverage.neon
)

add_executable(test_number_to_string
    tests/test_number_to_string.cpp
)
target_include_directories(test_number_to_string PRIVATE
    src
)
target_link_libraries(test_number_to_string
    common
)
add_test(
    NAME test_number_to_string
    COMMAND test_number_to_string
)

add_custom_command(
    OUTPUT gen/errors.txt
    COMMAND python3 tools/helium.py scripts/extract_errors.neon
    DEPENDS src/lexer.cpp
    DEPENDS src/parser.cpp
    DEPENDS src/analyzer.cpp
    DEPENDS src/sql.cpp
    DEPENDS src/ast.cpp
)
add_custom_target(errors_txt ALL
    DEPENDS gen/errors.txt
)

add_custom_command(
    OUTPUT contrib/grammar/neon.w3c.ebnf
    COMMAND python3 tools/helium.py contrib/grammar/ebnf_w3c.neon <contrib/grammar/neon.ebnf >contrib/grammar/neon.w3c.ebnf
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
                    COMMAND python3 scripts/run_test.py --runner "${RUNNER}" ${TEST}
                )
            endif()
        else()
            add_test(
                NAME "${T}"
                COMMAND python3 scripts/run_test.py --runner "${RUNNER}" ${TEST}
            )
        endif()
    endforeach()
endfunction(add_tests)

file(GLOB COMPILE_TESTS t/compile-only/*.neon)
add_tests("${COMPILE_TESTS}" "compile" "$<TARGET_FILE:neonc> -q" "")

file(GLOB RUN_TESTS t/*.neon)
foreach (extension_dir ${EXTENSION_DIRS})
    file(GLOB extension_tests ${extension_dir}/t/*.neon)
    foreach (extension_test ${extension_tests})
        list(APPEND RUN_TESTS ${extension_test})
    endforeach()
endforeach()
set(TESTS "")
set(TESTSX "")
foreach (TEST ${RUN_TESTS})
    get_filename_component(TEST_NAME "${TEST}" NAME)
    string(REGEX MATCH "^(posix|win32)-" TEST_PLATFORM ${TEST_NAME})
    if (TEST_PLATFORM)
        if (${TEST_PLATFORM} STREQUAL "posix-" AND NOT (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "FreeBSD" OR ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "OpenBSD"))
            continue()
        endif()
        if (${TEST_PLATFORM} STREQUAL "win32-" AND NOT (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows"))
            continue()
        endif()
    endif (TEST_PLATFORM)
    add_custom_command(
        OUTPUT "${TEST}x"
        COMMAND neonc -q ${TEST}
        DEPENDS neonc ${TEST}
    )
    list(APPEND TESTS ${TEST})
    list(APPEND TESTSX "${TEST}x")
endforeach ()
add_custom_target(testx ALL
    DEPENDS ${TESTSX}
)
add_tests("${TESTS}" "" $<TARGET_FILE:neon> "")
add_tests("${TESTS}" "helium" "python3 tools/helium.py" "tools/helium-exclude.txt")
if (NOT (WIN32 AND DEFINED ENV{GITHUB_ACTIONS}))
    # TODO: Github Actions does not seem to be able to run the C++ compiler on Windows sensibly.
    add_tests("${TESTS}" "c" "python3 scripts/run_c.py --neon $<TARGET_FILE:neon> --neonc $<TARGET_FILE:neonc>" "scripts/c-exclude.txt")
    add_tests("${TESTS}" "cpp" "python3 scripts/run_cpp.py --neonc $<TARGET_FILE:neonc>" "scripts/cpp-exclude.txt")
endif ()
if (NODEJS)
    add_tests("${TESTS}" "js" "python3 scripts/run_js.py --neonc $<TARGET_FILE:neonc>" "scripts/js-exclude.txt")
endif (NODEJS)
if (JAVAC)
    add_tests("${TESTS}" "jvm" "python3 scripts/run_jvm.py  --neonc $<TARGET_FILE:neonc> --java ${JAVA}" "scripts/jvm-exclude.txt")
endif (JAVAC)
if (CSC)
    add_tests("${TESTS}" "cli" "python3 scripts/run_cli.py --neonc $<TARGET_FILE:neonc>" "scripts/cli-exclude.txt")
endif ()
add_tests("${TESTS}" "nenex" "python3 exec/nenex/run_test.py --neon $<TARGET_FILE:neon>" "exec/nenex/exclude.txt")
add_tests("${TESTS}" "pynex" "python3 exec/pynex/run_test.py" "exec/pynex/exclude.txt")
if (JAVAC)
    add_tests("${TESTS}" "jnex" "python3 exec/jnex/run_test.py --java ${JAVA}" "exec/jnex/exclude.txt")
endif (JAVAC)
add_tests("${TESTS}" "cnex" "python3 exec/cnex/run_test.py --cnex $<TARGET_FILE:cnex>" "exec/cnex/exclude.txt")
if (CSC)
    add_tests("${TESTS}" "csnex" "python3 exec/csnex/run_test.py" "exec/csnex/exclude.txt")
endif (CSC)
if (RUSTC)
    add_tests("${TESTS}" "rsnex" "python3 exec/rsnex/run_test.py" "exec/rsnex/exclude.txt")
endif (RUSTC)
if (GO)
    add_tests("${TESTS}" "gonex" "python3 exec/gonex/run_test.py" "exec/gonex/exclude.txt")
endif (GO)

if (NOT WIN32)
    # Disabled for WIN32 because hard coded paths (to bin/neon etc) are different
    # paths on in WIN32 cmake builds.
    file(GLOB INTEGRATION_TESTS t/integration/*.neon)
    add_tests("${INTEGRATION_TESTS}" "integration" "$<TARGET_FILE:neon>" "")
endif (NOT WIN32)

file(GLOB TESTS t/todo/*.neon)
add_tests("${TESTS}" "" "$<TARGET_FILE:neon> -q" "")

add_test(
    NAME "import-optional:neonx"
    COMMAND python3 scripts/test_import_optional.py $<TARGET_FILE:neonc> $<TARGET_FILE:neonx>
)

# TODO: optional modules in nenex
#add_test(
#    NAME "import-optional:nenex"
#    COMMAND python3 scripts/test_import_optional.py $<TARGET_FILE:neonc> $<TARGET_FILE:neon> exec/nenex/nenex.neon
#)

# TODO: optional modules in pynex
#add_test(
#    NAME "import-optional:pynex"
#    COMMAND python3 scripts/test_import_optional.py $<TARGET_FILE:neonc> python3 exec/pynex/pynex.py
#)

# TODO: optional modules in jnex
#add_test(
#    NAME "import-optional:jnex"
#    COMMAND python3 scripts/test_import_optional.py $<TARGET_FILE:neonc> java -cp exec/jnex/src org.neon_lang.jnex.Executor
#)

# TODO: optional modules in cnex
#add_test(
#    NAME "import-optional:cnex"
#    COMMAND python3 scripts/test_import_optional.py $<TARGET_FILE:neonc> $<TARGET_FILE:cnex>
#)

# TODO: optional modules in gonex
#add_test(
#    NAME "import-optional:gonex"
#    COMMAND python3 scripts/test_import_optional.py $<TARGET_FILE:neonc> exec/gonex/gonex
#)

add_test(
    NAME "errors"
    COMMAND python3 scripts/run_test.py --runner "$<TARGET_FILE:neonc> -q" --errors t/errors
)

file(GLOB TESTS_REPL t/repl_*.neon)
foreach (TEST ${TESTS_REPL})
    get_filename_component(T "${TEST}" NAME)
    add_test(
        NAME "repl:${T}"
        COMMAND neon --repl-no-prompt --repl-stop-on-any-error --repl-input ${TEST}
    )
endforeach ()

file(GLOB_RECURSE SAMPLES neon/*.neon samples/*.neon tools/*.neon)
foreach (SAMPLE ${SAMPLES})
    execute_process(
        COMMAND python3 tools/helium.py tools/imports.neon ${SAMPLE}
        OUTPUT_VARIABLE IMPORTS_NL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REPLACE "\n" ";" IMPORTS "${IMPORTS_NL}")
    set(MISSING_IMPORT "")
    foreach (IMPORT ${IMPORTS})
        if (NOT EXISTS "lib/${IMPORT}.neon" AND NOT EXISTS "lib/${IMPORT}/${IMPORT}.neon")
            set(MISSING_IMPORT ${IMPORT})
            break()
        endif()
    endforeach()
    if (NOT MISSING_IMPORT)
        get_filename_component(X "${SAMPLE}" NAME)
        add_test(
            NAME "sample:${X}"
            COMMAND neonc ${SAMPLE}
        )
        # TODO: various language features still not understood by neon/parser
        #add_test(
        #    NAME "sample.neon:${X}"
        #    COMMAND neon neon/parser.neon ${SAMPLE}
        #)
    else()
        message("skipping ${SAMPLE} due to missing ${MISSING_IMPORT}")
    endif()
endforeach()

file(GLOB_RECURSE LIBS lib/*.neon)
add_test(
    NAME test_grammar_libs
    COMMAND python3 contrib/grammar/test-grammar.py ${LIBS}
)
add_test(
    NAME test_grammar_tools
    COMMAND python3 contrib/grammar/test-grammar.py tools/*.neon
)
add_test(
    NAME test_grammar_samples
    COMMAND python3 contrib/grammar/test-grammar.py ${SAMPLES}
)
add_test(
    NAME test_grammar_tests
    COMMAND python3 contrib/grammar/test-grammar.py t/*.neon
)
add_test(
    NAME test_grammar_tests_n3
    COMMAND python3 contrib/grammar/test-grammar.py t/errors/N3*.neon
)
add_test(
    NAME test_grammar_random
    COMMAND python3 contrib/grammar/test-random.py --neonc $<TARGET_FILE:neonc>
)

# TODO: This fails because cmake does not seem to accept the # on a COMMAND line.
#add_custom_command(
#    OUTPUT tmp/hello
#    COMMAND echo '#!/usr/bin/env neon' | cat - samples/hello/hello.neon >tmp/hello && chmod +x tmp/hello
#    DEPENDS samples/hello/hello.neon
#)
#add_custom_target(
#    test_shebang ALL
#    DEPENDS tmp/hello
#)
#add_test(
#    NAME test_shebang
#    COMMAND tmp/hello
#)

add_test(
    NAME "test_doc"
    COMMAND python3 scripts/test_doc.py --neon $<TARGET_FILE:neon> --neonc $<TARGET_FILE:neonc>
)

add_custom_target(docs ALL
    COMMAND perl external/NaturalDocs/NaturalDocs -i lib -o HTML gh-pages/html -p lib/nd.proj -ro -xi lib/compress/bzip2-1.0.6 -xi lib/compress/zlib-1.2.8 -xi lib/crypto/include -xi lib/crypto/libressl-2.2.4 -xi lib/fltk/fltk-1.3.4-1 -xi lib/hash/include -xi lib/hash/libressl-2.2.4 -xi lib/http/curl-7.41.0 -xi lib/http/libressl-2.2.4 -xi lib/http/zlib-1.2.8 -xi lib/http/include -xi lib/regex/pcre2-10.10 -xi lib/sdl/SDL2-2.0.3
)
add_custom_target(docs_samples ALL
    COMMAND perl external/NaturalDocs/NaturalDocs -i samples -o HTML gh-pages/samples -p samples/nd.proj -ro
)

add_custom_command(
    OUTPUT samples/hello/hello.neonx
    COMMAND neonc samples/hello/hello.neon
    DEPENDS neonc samples/hello/hello.neon
)
add_custom_target(hello_neb ALL
    COMMAND neonbind -stub $<TARGET_FILE:neonstub> tmp/hello.neb samples/hello/hello.neonx
    DEPENDS neonbind samples/hello/hello.neonx
)
add_test(
    NAME hello_neb
    COMMAND neonx tmp/hello.neb
)
add_custom_target(hello_exe ALL
    COMMAND neonbind -stub $<TARGET_FILE:neonstub> -e tmp/hello.exe samples/hello/hello.neonx
    DEPENDS neonbind neonstub samples/hello/hello.neonx
)
add_test(
    NAME hello_exe
    COMMAND tmp/hello.exe
)

add_test(
    NAME lexer_coverage_cpp
    COMMAND python3 tests/compare_expected.py tests/lexer-coverage.expected $<TARGET_FILE:test_lexer> tests/lexer-coverage.neon
)

add_test(
    NAME lexer_coverage_neon
    COMMAND python3 tests/compare_expected.py tests/lexer-coverage.expected $<TARGET_FILE:neon> neon/lexer.neon tests/lexer-coverage.neon
)

add_test(
    NAME lexer_coverage_helium
    COMMAND python3 tests/compare_expected.py tests/lexer-coverage.expected python3 tools/helium.py neon/lexer.neon tests/lexer-coverage.neon
)

add_test(
    NAME parser_coverage_cpp
    COMMAND python3 tests/compare_expected.py tests/parser-coverage.expected $<TARGET_FILE:test_parser> tests/parser-coverage.neon
)

add_test(
    NAME parser_coverage_neon
    COMMAND python3 tests/compare_expected.py tests/parser-coverage.expected $<TARGET_FILE:neon> neon/parser.neon tests/parser-coverage.neon
)

#add_test(
#    NAME parser_coverage_helium
#    COMMAND python3 tests/compare_expected.py tests/parser-coverage.expected python3 tools/helium.py neon/parser.neon tests/parser-coverage.neon
#)
