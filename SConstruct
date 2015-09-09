import distutils.spawn
import os
import re
import shutil
import subprocess
import sys
import tarfile
import zipfile
from SCons.Script.SConscript import SConsEnvironment

# Compatibility function for Python 2.6.
if not hasattr(subprocess, "check_output"):
    def check_output(args):
        p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        if p.returncode != 0:
            raise subprocess.CalledProcessError(args)
        return out
    subprocess.check_output = check_output

# Assume a UTF-8 capable terminal.
os.putenv("PYTHONIOENCODING", "UTF-8")

coverage = ARGUMENTS.get("coverage", 0)
# This is needed on OS X because clang has a bug where this isn't included automatically.
coverage_lib = (["/Library/Developer/CommandLineTools/usr/lib/clang/6.0/lib/darwin/libclang_rt.profile_osx.a"] if coverage else [])

default_release = 0
if GetOption("clean"):
    try:
        os.remove("config.py")
    except OSError:
        pass
    release = default_release
else:
    release = int(ARGUMENTS.get("release", default_release))
    stored_release = None
    try:
        with open("config.py") as config:
            for s in config:
                a = s.strip().split("=")
                if a[0] == "release":
                    stored_release = int(a[1])
    except IOError:
        pass
    if stored_release is not None:
        if release != stored_release:
            print >>sys.stderr, "Requested release flag ({}) different from last build ({}).".format(release, stored_release)
            print >>sys.stderr, "Run 'scons -c' first."
            sys.exit(1)
    else:
        with open("config.py", "w") as config:
            print >>config, "release={}".format(release)

env = Environment()

env["RELEASE"] = release

env["ENV"]["PROCESSOR_ARCHITECTURE"] = os.getenv("PROCESSOR_ARCHITECTURE")
env["ENV"]["PROCESSOR_ARCHITEW6432"] = os.getenv("PROCESSOR_ARCHITEW6432")

# Add path of Python itself to shell PATH.
env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.path.dirname(sys.executable)

def add_external(target):
    env.Depends("external", target)
    return target

add_external(SConscript("external/SConscript-libutf8", exports=["env"]))
libbid = add_external(SConscript("external/SConscript-libbid", exports=["env"]))
libffi = add_external(SConscript("external/SConscript-libffi", exports=["env"]))
libs_curses = add_external(SConscript("external/SConscript-libcurses", exports=["env"]))
libpcre = add_external(SConscript("external/SConscript-libpcre", exports=["env"]))
libcurl = add_external(SConscript("external/SConscript-libcurl", exports=["env"]))
libeasysid = add_external(SConscript("external/SConscript-libeasysid", exports=["env"]))
libhash = add_external(SConscript("external/SConscript-libhash", exports=["env"]))
libsqlite = add_external(SConscript("external/SConscript-libsqlite", exports=["env"]))
libz = add_external(SConscript("external/SConscript-libz", exports=["env"]))
libbz2 = add_external(SConscript("external/SConscript-libbz2", exports=["env"]))
liblzma = add_external(SConscript("external/SConscript-liblzma", exports=["env"]))
add_external(SConscript("external/SConscript-minijson", exports=["env"]))
add_external(SConscript("external/SConscript-pyparsing", exports=["env"]))

SConscript("external/SConscript-naturaldocs")

env.Append(CPPPATH=[
    "src",
])
if sys.platform == "win32":
    env.Append(CXXFLAGS=[
        "/EHsc",
        "/W4",
        "/WX",
    ])
    if not env["RELEASE"]:
        env.Append(CXXFLAGS=[
            "/MTd",
            "/Zi",
        ])
else:
    env.Append(CXXFLAGS=[
        "-std=c++0x",
        "-Wall",
        "-Wextra",
        "-Weffc++",
        "-Werror",
    ])
    if not env["RELEASE"]:
        env.Append(CXXFLAGS=[
            "-g",
        ])
for lib in [libbid, libffi, libpcre, libcurl, libhash, libsqlite, libz, libbz2, liblzma]:
    if lib is not None:
        env.Append(LIBS=[lib])
env.Append(LIBS=libs_curses)
if os.name == "posix":
    env.Append(LIBS=["dl"])
if sys.platform.startswith("linux"):
    env.Append(LIBS=["rt"])

if "g++" in env.subst("$CXX"):
    # This adds -Doverride= for GCC earlier than 4.7.
    # (GCC does not support 'override' before 4.7, but
    # it supports everything else we need.)
    try:
        ver = subprocess.check_output([env.subst("$CXX"), "--version"])
        if ver.startswith("g++"):
            ver = ver.split("\n")[0]
            ver = re.sub(r"\(.*?\)", "", ver)
            ver = float(re.search(r"(\d+\.\d+)\.", ver).group(1))
            if ver < 4.7:
                env.Append(CXXFLAGS=["-Doverride="])
    except Exception as x:
        pass

if coverage:
    env.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

rtl_const = [
]

rtl_cpp = rtl_const + [
    "lib/bitwise.cpp",
    "lib/compress.cpp",
    "lib/curses.cpp",
    "lib/datetime.cpp",
    "lib/global.cpp",
    "lib/file.cpp",
    "lib/hash.cpp",
    "lib/http.cpp",
    "lib/io.cpp",
    "lib/math.cpp",
    "lib/net.cpp",
    "lib/os.cpp",
    "lib/random.cpp",
    "lib/runtime.cpp",
    "lib/regex.cpp",
    "lib/sqlite.cpp",
    "lib/string.cpp",
    "lib/sys.cpp",
    "lib/time.cpp",
]

rtl_neon = [
    "lib/bitwise.neon",
    "lib/compress.neon",
    "lib/curses.neon",
    "lib/datetime.neon",
    "lib/file.neon",
    "lib/global.neon",
    "lib/hash.neon",
    "lib/http.neon",
    "lib/io.neon",
    "lib/math.neon",
    "lib/net.neon",
    "lib/os.neon",
    "lib/random.neon",
    "lib/runtime.neon",
    "lib/regex.neon",
    "lib/sqlite.neon",
    "lib/string.neon",
    "lib/sys.neon",
    "lib/time.neon",
]

if os.name == "posix":
    rtl_cpp.extend([
        "lib/file_posix.cpp",
        "lib/os_posix.cpp",
        "lib/time_posix.cpp",
    ])
    if sys.platform.startswith("darwin"):
        rtl_cpp.extend([
            "lib/time_darwin.cpp",
        ])
    elif sys.platform.startswith("linux"):
        rtl_cpp.extend([
            "lib/time_linux.cpp",
        ])
    else:
        print >>sys.stderr, "Unsupported platform:", sys.platform
        sys.exit(1)
    rtl_platform = "src/rtl_posix.cpp"
elif os.name == "nt":
    rtl_cpp.extend([
        "lib/file_win32.cpp",
        "lib/os_win32.cpp",
        "lib/time_win32.cpp",
    ])
    rtl_platform = "src/rtl_win32.cpp"
else:
    print "Unsupported platform:", os.name
    sys.exit(1)

env.Command(["src/thunks.inc", "src/functions_compile.inc", "src/functions_exec.inc", "src/enums.inc", "src/exceptions.inc"], [rtl_neon, "scripts/make_thunks.py"], sys.executable + " scripts/make_thunks.py " + " ".join(rtl_neon))

neon = env.Program("bin/neon", [
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/compiler.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/exec.cpp",
    "src/format.cpp",
    "src/lexer.cpp",
    "src/main.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/rtl_compile.cpp",
    "src/rtl_exec.cpp",
    rtl_cpp,
    rtl_platform,
    "src/support.cpp",
    "src/support_compiler.cpp",
    "src/util.cpp",
] + coverage_lib,
)

neonc = env.Program("bin/neonc", [
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/compiler.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/format.cpp",
    "src/lexer.cpp",
    "src/neonc.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/rtl_compile.cpp",
    rtl_const,
    "src/support.cpp",
    "src/support_compiler.cpp",
    "src/util.cpp",
] + coverage_lib,
)

neonx = env.Program("bin/neonx", [
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/exec.cpp",
    "src/format.cpp",
    "src/neonx.cpp",
    "src/number.cpp",
    "src/rtl_exec.cpp",
    rtl_cpp,
    rtl_platform,
    "src/support.cpp",
] + coverage_lib,
)

neondis = env.Program("bin/neondis", [
    "src/bytecode.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/neondis.cpp",
    "src/number.cpp",
    # The following are just to support internal_error()
    "src/lexer.cpp",
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

env.UnitTest("bin/test_format", [
    "tests/test_format.cpp",
    "src/format.cpp",
    "src/number.cpp",
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
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/compiler.cpp",
    "src/lexer.cpp",
    "src/format.cpp",
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
tests = env.Command("tests_helium", [neon, "scripts/run_test.py", Glob("t/*")], sys.executable + " scripts/run_test.py --runner \"" + sys.executable + " helium/helium.py\" t")
env.Depends(tests, test_ffi)
testenv = env.Clone()
testenv["ENV"]["NEONPATH"] = "t/"
testenv.Command("tests_error", [neon, "scripts/run_test.py", "src/errors.txt", Glob("t/errors/*")], sys.executable + " scripts/run_test.py --errors t/errors")
env.Command("tests_number", test_number_to_string, test_number_to_string[0].path)

for sample in Glob("samples/*.neon"):
    env.Command(sample.path+"x", [sample, neonc], neonc[0].abspath + " $SOURCE")
env.Command("tests_2", ["samples/hello.neonx", neonx], neonx[0].abspath + " $SOURCE")

env.Command("test_grammar", "contrib/grammar/neon.ebnf", sys.executable + " contrib/grammar/test-grammar.py samples/*.neon t/*.neon t/errors/N3*.neon >$TARGET")
env.Command("test_grammar_random", "contrib/grammar/neon.ebnf", sys.executable + " contrib/grammar/test-random.py")
env.Command("contrib/grammar/neon.w3c.ebnf", ["contrib/grammar/neon.ebnf", "contrib/grammar/ebnf_w3c.neon", neon], neon[0].path + " contrib/grammar/ebnf_w3c.neon <$SOURCE >$TARGET")

env.Command("test_doc", None, sys.executable + " scripts/test_doc.py")

if os.name == "posix":
    env.Command("samples/hello", "samples/hello.neon", "echo '#!/usr/bin/env neon' | cat - $SOURCE >$TARGET && chmod +x $TARGET")
    env.Command("tests_script", "samples/hello", "env PATH=bin samples/hello")

# Need to find where perl actually is, in case it's not in
# one of the paths supplied by scons by default (for example,
# on Windows with the GitHub command prompt).
perl = distutils.spawn.find_executable("perl")
if perl:
    env.Command("docs", None, perl + " external/NaturalDocs/NaturalDocs -i lib -o HTML lib/html -p lib/nd.proj")
