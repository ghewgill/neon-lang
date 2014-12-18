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

libffi = SConscript("SConscript-libffi", exports=["env"])

env.Command("external/utf8/source/utf8.h", "external/utf8_v2_3_4.zip", lambda target, source, env: zipfile.ZipFile(source[0].path).extractall("external/utf8"))

env.Append(CPPPATH=[
    "external/IntelRDFPMathLib20U1/LIBRARY/src",
    "external/utf8/source",
    "external/lib/libffi-3.2.1/include",
    "src",
])
if sys.platform == "win32":
    env.Append(CXXFLAGS=[
        "/EHsc",
        "/W4",
        "/WX",
    ])
else:
    env.Append(CXXFLAGS=[
        "-std=c++0x",
        "-Wall",
        "-Wextra",
        "-Weffc++",
        "-Werror",
        "-g",
    ])
env.Append(LIBS=[libbid, libffi])
if os.name == "posix":
    env.Append(LIBS=["dl"])

if coverage:
    env.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

rtl = [
    "lib/global.cpp",
    "lib/math.cpp",
    "lib/sys.cpp",
    "lib/time.cpp",
]

env.Command(["src/thunks.inc", "src/functions_compile.inc", "src/functions_exec.inc"], [rtl, "scripts/make_thunks.py"], sys.executable + " scripts/make_thunks.py " + " ".join(rtl))

if os.name == "posix":
    rtl.extend([
        "lib/time_posix.cpp",
    ])
    rtl_platform = "src/rtl_posix.cpp"
elif os.name == "nt":
    rtl.extend([
        "lib/time_win32.cpp",
    ])
    rtl_platform = "src/rtl_win32.cpp"
else:
    print "Unsupported platform:", os.name
    sys.exit(1)

neon = env.Program("bin/neon", [
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/compiler.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/exec.cpp",
    "src/lexer.cpp",
    "src/main.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/rtl_compile.cpp",
    "src/rtl_exec.cpp",
    rtl,
    rtl_platform,
    "src/util.cpp",
] + coverage_lib,
)

neonc = env.Program("bin/neonc", [
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/compiler.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/rtl_compile.cpp",
    "src/neonc.cpp",
    "src/util.cpp",
] + coverage_lib,
)

neonx = env.Program("bin/neonx", [
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/exec.cpp",
    "src/number.cpp",
    "src/rtl_exec.cpp",
    rtl,
    rtl_platform,
    "src/neonx.cpp",
] + coverage_lib,
)

env.Depends("src/number.h", libbid)
env.Depends("src/exec.cpp", libffi)

def UnitTest(env, target, source, **kwargs):
    t = env.Program(target, source, **kwargs)
    # see the following for the reason why this lambda is necessary:
    # http://stackoverflow.com/questions/8219743/scons-addpostaction-causes-dependency-check-error-work-around
    env.AddPostAction(t, lambda *_, **__: os.system(t[0].abspath))
    env.Alias("test", t)
    return t

env.Command("src/errors.txt", ["scripts/extract_errors.py"] + Glob("src/*.cpp"), sys.executable + " scripts/extract_errors.py")

SConsEnvironment.UnitTest = UnitTest

env.UnitTest("bin/test_lexer", [
    "tests/test_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

env.UnitTest("bin/test_parser", [
    "tests/test_parser.cpp",
    "src/ast.cpp",
    "src/cell.cpp",
    "src/compiler.cpp",
    "src/parser.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/rtl_compile.cpp",
    "src/util.cpp",
] + coverage_lib,
)

env.UnitTest("bin/test_compiler", [
    "tests/test_compiler.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/compiler.cpp",
    "src/disassembler.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/rtl_compile.cpp",
    "src/util.cpp",
] + coverage_lib,
)

env.Program("bin/fuzz_lexer", [
    "tests/fuzz_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

env.Program("bin/fuzz_parser", [
    "tests/fuzz_parser.cpp",
    "src/ast.cpp",
    "src/compiler.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/rtl_compile.cpp",
    "src/util.cpp",
] + coverage_lib,
)

if sys.platform == "win32":
    test_ffi = env.SharedLibrary("bin/libtest_ffi", "tests/test_ffi.c")
else:
    test_ffi = env.SharedLibrary("bin/test_ffi", "tests/test_ffi.c")

tests = env.Command("tests_normal", [neon, "scripts/run_test.py", Glob("t/*")], sys.executable + " scripts/run_test.py t")
env.Depends(tests, test_ffi)
env.Command("tests_error", [neon, "scripts/run_test.py", "src/errors.txt", Glob("t/errors/*")], sys.executable + " scripts/run_test.py --errors t/errors")

for sample in Glob("samples/*.neon"):
    env.Command(sample.path+"x", [sample, neonc], neonc[0].abspath + " $SOURCE")
env.Command("tests_2", ["samples/hello.neonx", neonx], neonx[0].abspath + " $SOURCE")
