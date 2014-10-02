import os
from SCons.Script.SConscript import SConsEnvironment

coverage = ARGUMENTS.get("coverage", 0)
# This is needed on OS X because clang has a bug where this isn't included automatically.
coverage_lib = (["/Library/Developer/CommandLineTools/usr/lib/clang/6.0/lib/darwin/libclang_rt.profile_osx.a"] if coverage else [])

env = Environment()

env.Command("external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "external/IntelRDFPMathLib20U1.tar.gz", "cd external && tar zxvf IntelRDFPMathLib20U1.tar.gz")
libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.a", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && make CC=gcc GLOBAL_RND=1 GLOBAL_FLAGS=1")

env.Append(CPPPATH=[
    "external/IntelRDFPMathLib20U1/LIBRARY/src",
])
env.Append(CXXFLAGS=[
    "-Wall",
    "-Wextra",
    "-Weffc++",
    "-Werror",
    "-pedantic",
    "-Wno-c++11-extensions",
    "-Wno-unused-parameter",
    "-g",
])
env.Append(LIBS=[libbid])

if coverage:
    env.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

env.Program("simple", [
    "ast.cpp",
    "bytecode.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "exec.cpp",
    "lexer.cpp",
    "main.cpp",
    "number.cpp",
    "parser.cpp",
    "util.cpp",
] + coverage_lib,
)

def UnitTest(env, target, source, **kwargs):
    t = env.Program(target, source, **kwargs)
    # see the following for the reason why this lambda is necessary:
    # http://stackoverflow.com/questions/8219743/scons-addpostaction-causes-dependency-check-error-work-around
    env.AddPostAction(t, lambda *_, **__: os.system(t[0].abspath))
    env.Alias("test", t)
    return t

SConsEnvironment.UnitTest = UnitTest

env.UnitTest("test_lexer", [
    "test_lexer.cpp",
    "lexer.cpp",
    "number.cpp",
    "util.cpp",
] + coverage_lib,
)

env.UnitTest("test_parser", [
    "test_parser.cpp",
    "ast.cpp",
    "compiler.cpp",
    "parser.cpp",
    "lexer.cpp",
    "number.cpp",
    "util.cpp",
] + coverage_lib,
)

env.UnitTest("test_compiler", [
    "test_compiler.cpp",
    "ast.cpp",
    "bytecode.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "lexer.cpp",
    "number.cpp",
    "parser.cpp",
    "util.cpp",
] + coverage_lib,
)

env.Command("dummy", ["simple", "run_test.py", Glob("t/*")], "python3 run_test.py t")
