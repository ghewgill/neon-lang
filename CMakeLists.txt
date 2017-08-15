# This is currently the beginnings of a cmake build file for neon.
# It is not complete and the best way to build neon now is with scons.
# This may depend on things that are built by a regular scons build,
# but over time that dependency may be eliminated and this will be
# a standalone build system.

cmake_minimum_required(VERSION 3.0)

link_directories(external/IntelRDFPMathLib20U1/LIBRARY external/hash-library)

add_executable(neonc
    src/analyzer.cpp
    src/ast.cpp
    src/bytecode.cpp
    src/compiler.cpp
    src/compiler_cpp.cpp
    src/compiler_js.cpp
    src/compiler_jvm.cpp
    src/debuginfo.cpp
    src/disassembler.cpp
    src/format.cpp
    src/intrinsic.cpp
    src/lexer.cpp
    src/neonc.cpp
    src/number.cpp
    src/parser.cpp
    src/rtl_compile.cpp
    src/sql.cpp
    src/support.cpp
    src/support_compiler.cpp
    src/util.cpp

    lib/file_const_posix.cpp
)
target_include_directories(neonc PUBLIC external/IntelRDFPMathLib20U1/LIBRARY/src)
target_include_directories(neonc PUBLIC external/hash-library)
target_include_directories(neonc PUBLIC external/minijson_writer-master)
target_include_directories(neonc PUBLIC external/utfcpp-master/source)
target_include_directories(neonc PUBLIC src)
target_compile_options(neonc PUBLIC -std=c++11)
if (WIN32)
    target_link_libraries(neonc libbid)
else (WIN32)
    target_link_libraries(neonc bid)
endif (WIN32)
target_link_libraries(neonc hash-library)
