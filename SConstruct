import os
import sys
import tarfile
import zipfile
from SCons.Script.SConscript import SConsEnvironment

def check_output(s, **kwargs):
    p = subprocess.Popen(s, stdout=subprocess.PIPE, **kwargs)
    (outdata, errdata) = p.communicate()
    r = p.returncode
    assert r is not None
    if r != 0:
        raise subprocess.CalledProcessError(r, s)
    return outdata
subprocess.check_output = check_output

# Assume a UTF-8 capable terminal.
os.putenv("PYTHONIOENCODING", "UTF-8")

coverage = ARGUMENTS.get("coverage", 0)
# This is needed on OS X because clang has a bug where this isn't included automatically.
coverage_lib = (["/Library/Developer/CommandLineTools/usr/lib/clang/6.0/lib/darwin/libclang_rt.profile_osx.a"] if coverage else [])

env = Environment()

env["ENV"]["PROCESSOR_ARCHITECURE"] = os.getenv("PROCESSOR_ARCHITECTURE")
env["ENV"]["PROCESSOR_ARCHITEW6432"] = os.getenv("PROCESSOR_ARCHITEW6432")

env.Command("external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "external/IntelRDFPMathLib20U1.tar.gz", lambda target, source, env: tarfile.open(source[0].path).extractall("external"))
if sys.platform == "win32":
    libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.lib", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && nmake -fmakefile.mak CC=cl GLOBAL_RND=1 GLOBAL_FLAGS=1")
else:
    libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.a", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && make CC=gcc GLOBAL_RND=1 GLOBAL_FLAGS=1")

libffi = SConscript("SConscript-libffi", exports=["env"])

if sys.platform == "win32":
    env.Command("external/PDCurses-3.4/win32/vcwin32.mak", "external/PDCurses-3.4.tar.gz", lambda target, source, env: tarfile.open(source[0].path).extractall("external"))
    libs_curses = [env.Command("external/PDCurses-3.4/win32/pdcurses.lib", "external/PDCurses-3.4/win32/vcwin32.mak", "cd external/PDCurses-3.4/win32 && nmake -fvcwin32.mak WIDE=Y UTF8=Y")]
    libs_curses.extend(["advapi32", "user32"])
else:
    libs_curses = ["ncurses"]

env.Command("external/utf8/source/utf8.h", "external/utf8_v2_3_4.zip", lambda target, source, env: zipfile.ZipFile(source[0].path).extractall("external/utf8"))

env.Command("external/easysid-version-1.0/SConstruct", "external/easysid-version-1.0.tar.gz", lambda target, source, env: tarfile.open(source[0].path).extractall("external"))
libeasysid = env.Command("external/easysid-version-1.0/libeasysid"+env["SHLIBSUFFIX"], "external/easysid-version-1.0/SConstruct", "cd external/easysid-version-1.0 && " + sys.executable + " " + sys.argv[0])

env.Append(CPPPATH=[
    "external/IntelRDFPMathLib20U1/LIBRARY/src",
    "external/utf8/source",
    "external/lib/libffi-3.2.1/include",
    "external/PDCurses-3.4",
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
env.Append(LIBS=[libbid, libffi] + libs_curses)
if os.name == "posix":
    env.Append(LIBS=["dl"])

if coverage:
    env.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

rtl_const = [
    "lib/math_const.cpp",
]

rtl = rtl_const + [
    "lib/bitwise.cpp",
    "lib/curses.cpp",
    "lib/global.cpp",
    "lib/math.cpp",
    "lib/random.cpp",
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
    rtl_const,
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

neondis = env.Program("bin/neondis", [
    "src/bytecode.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/neondis.cpp",
    "src/number.cpp",
    "src/util.cpp",
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

test_number_to_string = env.Program("bin/test_number_to_string", [
    "tests/test_number_to_string.cpp",
    "src/number.cpp",
] + coverage_lib,
)

env.UnitTest("bin/test_lexer", [
    "tests/test_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
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
    rtl_const,
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
env.Command("tests_number", test_number_to_string, test_number_to_string[0].path)

for sample in Glob("samples/*.neon"):
    env.Command(sample.path+"x", [sample, neonc], neonc[0].abspath + " $SOURCE")
env.Command("tests_2", ["samples/hello.neonx", neonx], neonx[0].abspath + " $SOURCE")

if os.name == "posix":
    env.Command("samples/hello", "samples/hello.neon", "echo '#!/usr/bin/env neon' | cat - $SOURCE >$TARGET && chmod +x $TARGET")
    env.Command("tests_script", "samples/hello", "env PATH=bin samples/hello")
