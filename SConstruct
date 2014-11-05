import os
import sys
import tarfile
import zipfile
from SCons.Script.SConscript import SConsEnvironment

# Assume a UTF-8 capable terminal.
os.putenv("PYTHONIOENCODING", "UTF-8")

coverage = ARGUMENTS.get("coverage", 0)
# This is needed on OS X because clang has a bug where this isn't included automatically.
coverage_lib = (["/Library/Developer/CommandLineTools/usr/lib/clang/6.0/lib/darwin/libclang_rt.profile_osx.a"] if coverage else [])

env = Environment()

env.Command("external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "external/IntelRDFPMathLib20U1.tar.gz", lambda target, source, env: tarfile.open(source[0].path).extractall("external"))
if sys.platform == "win32":
    libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.lib", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && nmake -fmakefile.mak CC=cl GLOBAL_RND=1 GLOBAL_FLAGS=1")
else:
    libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.a", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && make CC=gcc GLOBAL_RND=1 GLOBAL_FLAGS=1")

env.Command("external/utf8/source/utf8.h", "external/utf8_v2_3_4.zip", lambda target, source, env: zipfile.ZipFile(source[0].path).extractall("external/utf8"))

env.Append(CPPPATH=[
    "external/IntelRDFPMathLib20U1/LIBRARY/src",
    "external/utf8/source",
])
if sys.platform == "win32":
    env.Append(CXXFLAGS=[
        "/EHsc",
        "/W4",
        "/wd4100", # unreferenced formal parameter
        "/WX",
    ])
else:
    env.Append(CXXFLAGS=[
        "-std=c++0x",
        "-Wall",
        "-Wextra",
        "-Weffc++",
        "-Werror",
        "-Wno-unused-parameter",
        "-g",
    ])
env.Append(LIBS=[libbid])

if coverage:
    env.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

env.Command(["thunks.inc", "functions.inc"], ["rtl.cpp", "make_thunks.py"], sys.executable + " make_thunks.py")

if os.name == "posix":
    rtl_platform = "rtl_posix.cpp"
elif os.name == "nt":
    rtl_platform = "rtl_win32.cpp"
else:
    print "Unsupported platform:", os.name
    sys.exit(1)

simple = env.Program("simple", [
    "ast.cpp",
    "bytecode.cpp",
    "cell.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "exec.cpp",
    "lexer.cpp",
    "main.cpp",
    "number.cpp",
    "parser.cpp",
    "rtl.cpp", rtl_platform,
    "util.cpp",
] + coverage_lib,
)

env.Depends("number.h", libbid)

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
    "cell.cpp",
    "compiler.cpp",
    "parser.cpp",
    "lexer.cpp",
    "number.cpp",
    "rtl.cpp", rtl_platform,
    "util.cpp",
] + coverage_lib,
)

env.UnitTest("test_compiler", [
    "test_compiler.cpp",
    "ast.cpp",
    "bytecode.cpp",
    "cell.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "lexer.cpp",
    "number.cpp",
    "parser.cpp",
    "rtl.cpp", rtl_platform,
    "util.cpp",
] + coverage_lib,
)

env.Command("dummy", [simple, "run_test.py", Glob("t/*")], sys.executable + " run_test.py t")
