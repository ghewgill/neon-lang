import distutils.spawn
import operator
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

squeeze = lambda a: [x for x in a if x]

# Assume a UTF-8 capable terminal.
os.putenv("PYTHONIOENCODING", "UTF-8")

coverage = ARGUMENTS.get("coverage", 0)
# This is needed on OS X because clang has a bug where this isn't included automatically.
coverage_lib = (["/Library/Developer/CommandLineTools/usr/lib/clang/6.0/lib/darwin/libclang_rt.profile_osx.a"] if coverage else [])

# Check for any files that accidentally contain \r\n. Only do this
# on non-windows platforms, because windows users may set Git to
# use crlf line endings.
if sys.platform != "nt":
    for subdir in ["contrib", "gh-pages", "lib", "samples", "scripts", "src", "t", "tests", "tools"]:
        for path, files, dirs in os.walk(subdir):
            for fn in files:
                if fn.endswith((".cpp", ".neon", ".txt", ".md")):
                    with open(os.path.join(path, fn), "rb") as f:
                        data = f.read()
                        assert "\r\n" not in data, fn

env = Environment()

vars = Variables(["config.cache", "config.py"])
vars.AddVariables(
    BoolVariable("MINIMAL", "Set to 1 to build without most 3rd party libraries", False),
    BoolVariable("RELEASE", "Set to 1 to build for release", False),
)
vars.Update(env)
if GetOption("clean"):
    try:
        os.remove("config.cache")
    except OSError:
        pass
else:
    vars.Save("config.cache", env)

env["ENV"]["PROCESSOR_ARCHITECTURE"] = os.getenv("PROCESSOR_ARCHITECTURE")
env["ENV"]["PROCESSOR_ARCHITEW6432"] = os.getenv("PROCESSOR_ARCHITEW6432")

# Add path of Python itself to shell PATH.
env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.path.dirname(sys.executable)

# Find where javac.exe is and add it to our PATH.
env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "javac.exe")))

env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "node")) or os.path.exists(os.path.join(x, "node.exe")))
use_node = os.system("node --version") == 0

def add_external(target):
    env.Depends("external", target)
    return target

use_curl = not env["MINIMAL"]
use_easysid = not env["MINIMAL"]
use_sqlite = not env["MINIMAL"] or True # Need this for embedded sql
use_ssl = not env["MINIMAL"]
use_posix = os.name == "posix"

add_external(SConscript("external/SConscript-libutf8", exports=["env"]))
libbid = add_external(SConscript("external/SConscript-libbid", exports=["env"]))
libffi = add_external(SConscript("external/SConscript-libffi", exports=["env"]))
libcurl = add_external(SConscript("external/SConscript-libcurl", exports=["env"])) if use_curl else None
libeasysid = add_external(SConscript("external/SConscript-libeasysid", exports=["env"])) if use_easysid else None
libhash = add_external(SConscript("external/SConscript-libhash", exports=["env"]))
libsqlite = add_external(SConscript("external/SConscript-libsqlite", exports=["env"])) if use_sqlite else None
libz = add_external(SConscript("external/SConscript-libz", exports=["env"]))
libminizip = add_external(SConscript("external/SConscript-libminizip", exports=["env"]))
libssl = add_external(SConscript("external/SConscript-libssl", exports=["env"])) if use_ssl else None
add_external(SConscript("external/SConscript-minijson", exports=["env"]))
add_external(SConscript("external/SConscript-pyparsing", exports=["env"]))

SConscript("lib/compress/SConstruct")
SConscript("lib/curses/SConstruct")
SConscript("lib/extsample/SConstruct")
SConscript("lib/regex/SConstruct")
SConscript("lib/sdl/SConstruct")
SConscript("lib/sodium/SConstruct")

env.Depends(libcurl, libssl)
env.Depends(libcurl, libz)

SConscript("external/SConscript-naturaldocs")

env.Append(CPPPATH=[
    "src",
])
if sys.platform == "win32":
    env.Append(CXXFLAGS=[
        "/EHsc",
        "/W4",
        "/WX",
        "/FS",
    ])
    if not env["RELEASE"]:
        env.Append(LINKFLAGS=[
            "/DEBUG",
        ])
        env.Append(CXXFLAGS=[
            "/MDd",
            "/Zi",
            "/Od",
        ])
    else:
        env.Append(CXXFLAGS=[
            "/Ox",
            "/MD",
        ])
    env.Append(LIBS=["user32", "wsock32"])
else:
    env.Append(CXXFLAGS=[
        "-std=c++0x",
        "-Wall",
        "-Wextra",
        "-Weffc++",
        #"-Wold-style-cast",    # Enable this temporarily to check, but it breaks with gcc and #defines with C casts in standard headers.
        "-Werror",
    ])
    if not env["RELEASE"]:
        env.Append(CXXFLAGS=[
            "-g",
        ])
    else:
        env.Append(CXXFLAGS=[
            "-O3",
        ])
env.Prepend(LIBS=squeeze([libbid, libffi, libcurl, libhash, libsqlite, libminizip, libz, libssl]))
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
        if "g++" in ver.split()[0]:
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

rtl_const = []

if os.name == "posix":
    rtl_const.extend([
        "lib/file_const_posix.cpp",
    ])
elif os.name == "nt":
    rtl_const.extend([
        "lib/file_const_win32.cpp",
    ])
else:
    print "Unsupported platform:", os.name
    sys.exit(1)

rtl_cpp = rtl_const + squeeze([
    "lib/binary.cpp",
    "lib/crypto.cpp" if use_ssl else None,
    "lib/datetime.cpp",
    "lib/debugger.cpp",
    "lib/global.cpp",
    "lib/file.cpp",
    "lib/hash.cpp" if use_ssl else None,
    "lib/hash_ressl.cpp" if use_ssl else None,
    "lib/http.cpp" if use_curl else None,
    "lib/io.cpp",
    "lib/math.cpp",
    "lib/net.cpp",
    "lib/os.cpp",
    "lib/posix.cpp" if use_posix else None,
    "lib/random.cpp",
    "lib/runtime.cpp",
    "lib/sqlite.cpp" if use_sqlite else None,
    "lib/string.cpp",
    "lib/sys.cpp",
    "lib/time.cpp",
])

env.Depends("lib/http.cpp", libcurl)

rtl_neon = squeeze([
    "lib/binary.neon",
    "lib/crypto.neon" if use_ssl else None,
    "lib/datetime.neon",
    "lib/debugger.neon",
    "lib/file.neon",
    "lib/global.neon",
    "lib/hash.neon" if use_ssl else None,
    "lib/http.neon" if use_curl else None,
    "lib/io.neon",
    "lib/math.neon",
    "lib/mmap.neon",
    "lib/net.neon",
    "lib/os.neon",
    "lib/posix.neon" if use_posix else None,
    "lib/process.neon",
    "lib/random.neon",
    "lib/runtime.neon",
    "lib/sqlite.neon" if use_sqlite else None,
    "lib/string.neon",
    "lib/sys.neon",
    "lib/time.neon",
])

lib_neon = Glob("lib/*.neon")

def build_rtl_inc(target, source, env):
    with open("src/rtl.inc", "w") as f:
        print >>f, "static struct {"
        print >>f, "    const char *name;"
        print >>f, "    const char *source;"
        print >>f, "} rtl_sources[] = {"
        for fn in source:
            print >>f, "    {{\"{}\",".format(fn.name.replace(".neon", ""))
            f.write("        \"" + "\\n\"\n        \"".join(x.replace("\\", "\\\\").replace('"', '\\"') for x in open(fn.abspath).read().split("\n")))
            print >>f, "\"},"
        print >>f, "};"

env.Command("src/rtl.inc", lib_neon, build_rtl_inc)

if os.name == "posix":
    rtl_cpp.extend([
        "lib/file_posix.cpp",
        "lib/mmap_posix.cpp",
        "lib/os_posix.cpp",
        "lib/process_posix.cpp",
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
        "lib/mmap_win32.cpp",
        "lib/os_win32.cpp",
        "lib/process_win32.cpp",
        "lib/time_win32.cpp",
    ])
    rtl_platform = "src/rtl_win32.cpp"
else:
    print "Unsupported platform:", os.name
    sys.exit(1)

env.Command(["src/thunks.inc", "src/functions_compile.inc", "src/functions_exec.inc", "src/enums.inc", "src/exceptions.inc", "src/constants_compile.inc"], [rtl_neon, "scripts/make_thunks.py"], sys.executable + " scripts/make_thunks.py " + " ".join(rtl_neon))

jvm_classes = env.Java("jvm", "jvm")

neonc = env.Program("bin/neonc", [
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/compiler.cpp",
    "src/compiler_cpp.cpp",
    "src/compiler_js.cpp",
    "src/compiler_jvm.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/format.cpp",
    "src/intrinsic.cpp",
    "src/lexer.cpp",
    "src/neonc.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/rtl_compile.cpp",
    rtl_const,
    "src/sql.cpp",
    "src/support.cpp",
    "src/support_compiler.cpp",
    "src/util.cpp",
] + coverage_lib,
)
env.Depends(neonc, jvm_classes)

def build_rtlx_inc(target, source, env):
    with open("src/rtlx.inc", "w") as f:
        for fn in source:
            bytecode = open(fn.abspath, "rb").read()
            print >>f, "static const unsigned char bytecode_{}[] = {{{}}};".format(
                fn.name.replace(".neonx", ""),
                ",".join("0x{:02x}".format(ord(x)) for x in bytecode)
            )
        print >>f, "static struct {"
        print >>f, "    const char *name;"
        print >>f, "    size_t length;"
        print >>f, "    const unsigned char *bytecode;"
        print >>f, "} rtl_bytecode[] = {"
        for fn in source:
            modname = fn.name.replace(".neonx", "")
            bytecode = open(fn.abspath, "rb").read()
            print >>f, "    {{\"{}\", {}, bytecode_{}}},".format(modname, len(bytecode), modname)
        print >>f, "};"

lib_neon_without_global = [x for x in lib_neon if x.name != "global.neon"]
for fn in lib_neon_without_global:
    env.Command(fn.abspath+"x", [fn, neonc], neonc[0].abspath + " $SOURCE")
env.Command("src/rtlx.inc", [x.abspath+"x" for x in lib_neon_without_global], build_rtlx_inc)

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
    "src/httpserver.cpp",
    "src/intrinsic.cpp",
    "src/lexer.cpp",
    "src/main.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/rtl_compile.cpp",
    "src/rtl_exec.cpp",
    rtl_cpp,
    rtl_platform,
    "src/sql.cpp",
    "src/support.cpp",
    "src/support_compiler.cpp",
    "src/support_exec.cpp",
    "src/util.cpp",
] + coverage_lib,
)

neonx = env.Program("bin/neonx", [
    "src/bundle.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/exec.cpp",
    "src/format.cpp",
    "src/httpserver.cpp",
    "src/intrinsic.cpp",
    "src/neonx.cpp",
    "src/number.cpp",
    "src/rtl_exec.cpp",
    rtl_cpp,
    rtl_platform,
    "src/support.cpp",
    "src/support_exec.cpp",
] + coverage_lib,
)

neonstub = env.Program("bin/neonstub", [
    "src/bundle.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/exec.cpp",
    "src/format.cpp",
    "src/httpserver.cpp",
    "src/intrinsic.cpp",
    "src/neonstub.cpp",
    "src/number.cpp",
    "src/rtl_exec.cpp",
    rtl_cpp,
    rtl_platform,
    "src/support.cpp",
    "src/support_exec.cpp",
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

neonbind = env.Program("bin/neonbind", [
    "src/bytecode.cpp",
    "src/neonbind.cpp",
    "src/support.cpp",
    "src/support_exec.cpp",
])

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

test_lexer = env.UnitTest("bin/test_lexer", [
    "tests/test_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

test_parser = env.Program("bin/test_parser", [
    "tests/test_parser.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/sql.cpp",
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
    "src/intrinsic.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/rtl_compile.cpp",
    rtl_const,
    "src/sql.cpp",
    "src/util.cpp",
] + coverage_lib,
)

env.Program("bin/perf_lexer", [
    "tests/perf_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

if sys.platform == "win32":
    test_ffi = env.SharedLibrary("bin/libtest_ffi", "tests/test_ffi.c")
else:
    test_ffi = env.SharedLibrary("bin/test_ffi", "tests/test_ffi.c")

test_sources = []
for f in Glob("t/*.neon"):
    if not use_curl and f.name in ["debug-server.neon", "http-test.neon"]:
        continue
    if not use_sqlite and f.name in ["sqlite-test.neon"]:
        continue
    if not use_sqlite and f.name.startswith("sql-"):
        continue
    if not use_ssl and f.name in ["hash-test.neon"]:
        continue
    test_sources.append(f)
tests = env.Command("tests_normal", [neon, "scripts/run_test.py", test_sources], sys.executable + " scripts/run_test.py " + " ".join(x.path for x in test_sources))
env.Depends(tests, test_ffi)
env.Command("tests_helium", [neon, "scripts/run_test.py", test_sources], sys.executable + " scripts/run_test.py --runner \"" + sys.executable + " tools/helium.py\" t")
if use_node:
    tests_js = env.Command("tests_js", [neonc, "scripts/run_test.py", test_sources], sys.executable + " scripts/run_test.py --runner \"" + sys.executable + " scripts/run_js.py\" t")
tests_jvm = env.Command("tests_jvm", [neonc, "scripts/run_test.py", test_sources], sys.executable + " scripts/run_test.py --runner \"" + sys.executable + " scripts/run_jvm.py\" t")
tests_cpp = env.Command("tests_cpp", [neonc, "scripts/run_test.py", "scripts/run_cpp.py", test_sources], sys.executable + " scripts/run_test.py --runner \"" + sys.executable + " scripts/run_cpp.py\" t")
env.Depends(tests_jvm, jvm_classes)
testenv = env.Clone()
testenv["ENV"]["NEONPATH"] = "t/"
testenv.Command("tests_error", [neon, "scripts/run_test.py", "src/errors.txt", Glob("t/errors/*")], sys.executable + " scripts/run_test.py --errors t/errors")
env.Command("tests_number", test_number_to_string, test_number_to_string[0].path)
for f in Glob("t/repl_*.neon"):
    result = f.abspath + ".result"
    t = env.Command(result, [f, neon], neon[0].abspath + " --repl-no-prompt --repl-stop-on-any-error <$SOURCE")
    env.Alias("tests_repl", t)

samples = []
for path, dirs, files in os.walk("."):
    if all(x not in ["t", "tests"] for x in path.split(os.sep)):
        for f in files:
            if f.endswith(".neon") and f != "global.neon":
                if not use_curl and f in ["coverage.neon", "ndb.neon"]:
                    continue
                if f in ["sdl.neon", "flappy.neon", "life.neon", "mandelbrot.neon", "spacedebris.neon"]:
                    continue
                samples.append(os.path.join(path, f))
for sample in samples:
    env.Command(sample+"x", [sample, neonc], neonc[0].abspath + " $SOURCE")
env.Command("tests_2", ["samples/hello/hello.neonx", neonx], neonx[0].abspath + " $SOURCE")

env.Command("test_grammar", ["contrib/grammar/neon.ebnf", "src/token.h", "src/parser.cpp"], sys.executable + " contrib/grammar/test-grammar.py lib/*.neon neon/*.neon tools/*.neon {} t/*.neon t/errors/N3*.neon >$TARGET".format(" ".join(x for x in reduce(operator.add, ([os.path.join(path, x) for x in files] for path, dirs, files in os.walk("samples"))) if x.endswith(".neon"))))
env.Command("test_grammar_random", "contrib/grammar/neon.ebnf", sys.executable + " contrib/grammar/test-random.py")
env.Command("contrib/grammar/neon.w3c.ebnf", ["contrib/grammar/neon.ebnf", "contrib/grammar/ebnf_w3c.neon", neon], neon[0].path + " contrib/grammar/ebnf_w3c.neon <$SOURCE >$TARGET")

env.Command("test_doc", None, sys.executable + " scripts/test_doc.py")

if os.name == "posix":
    env.Command("tmp/hello", "samples/hello/hello.neon", "echo '#!/usr/bin/env neon' | cat - $SOURCE >$TARGET && chmod +x $TARGET")
    env.Command("tests_script", "tmp/hello", "env PATH=bin tmp/hello")

hello_neb = env.Command("tmp/hello.neb", ["samples/hello/hello.neonx", neonbind], "{} $TARGET $SOURCE".format(neonbind[0].path))
env.Command("test_hello_neb", [hello_neb, neonx], "{} $SOURCE".format(neonx[0].path))

hello_exe = env.Command("tmp/hello.exe", ["samples/hello/hello.neonx", neonbind, neonstub], "{} -e $TARGET $SOURCE".format(neonbind[0].path))
env.Command("test_hello_exe", hello_exe, hello_exe[0].path)
cal_exe = env.Command("tmp/cal.exe", ["samples/cal/cal.neonx", neonbind, neonstub], "{} -e $TARGET $SOURCE".format(neonbind[0].path))
env.Command("test_cal", cal_exe, cal_exe[0].path)

# Need to find where perl actually is, in case it's not in
# one of the paths supplied by scons by default (for example,
# on Windows with the GitHub command prompt).
perl = distutils.spawn.find_executable("perl")
if perl:
    env.Command("docs", None, perl + " external/NaturalDocs/NaturalDocs -i lib -o HTML gh-pages/html -p lib/nd.proj -ro -xi lib/compress/bzip2-1.0.6 -xi lib/compress/zlib-1.2.8 -xi lib/regex/pcre2-10.10 -xi lib/sdl/SDL2-2.0.3")
    env.Command("docs_samples", None, perl + " external/NaturalDocs/NaturalDocs -i samples -o HTML gh-pages/samples -p samples/nd.proj -ro")

def compare(target, source, env):
    os.system("diff -u -w {} {}".format(source[0].path, source[1].path))
    assert open(source[0].path).read() == open(source[1].path).read()

lexer_dump_cpp = env.Command("tmp/lexer-coverage.dump_cpp", ["tests/lexer-coverage.neon", test_lexer], "-{} $SOURCE >$TARGET".format(test_lexer[0]))
lexer_dump_neon = env.Command("tmp/lexer-coverage.dump_neon", ["tests/lexer-coverage.neon", "neon/lexer.neon", neon], "-{} neon/lexer.neon $SOURCE >$TARGET".format(neon[0]))
lexer_dump_helium = env.Command("tmp/lexer-coverage.dump_helium", ["tests/lexer-coverage.neon", "neon/lexer.neon", "tools/helium.py"], "-python tools/helium.py neon/lexer.neon $SOURCE >$TARGET")
for i, d in enumerate([lexer_dump_cpp, lexer_dump_neon, lexer_dump_helium]):
    env.Command("tmp/lexer-coverage.dump.dummy{}".format(i), ["tests/lexer-coverage.expected", d], compare)

parser_dump_cpp = env.Command("tmp/parser-coverage.dump_cpp", ["tests/parser-coverage.neon", test_parser], "-{} $SOURCE >$TARGET".format(test_parser[0]))
parser_dump_neon = env.Command("tmp/parser-coverage.dump_neon", ["tests/parser-coverage.neon", "neon/parser.neon", neon], "-{} neon/parser.neon $SOURCE >$TARGET".format(neon[0]))
# TODO parser_dump_helium = env.Command("tmp/parser-coverage.dump_helium", ["tests/parser-coverage.neon", "neon/parser.neon", "tools/helium.py"], "-python tools/helium.py neon/parser.neon $SOURCE >$TARGET")
for i, d in enumerate([parser_dump_cpp, parser_dump_neon]): # TODO , parser_dump_helium]):
    env.Command("tmp/parser-coverage.dump.dummy{}".format(i), ["tests/parser-coverage.expected", d], compare)

if False: # This takes rather too long.
    for fn in Glob("t/*.neon") + Glob("t/errors/*.neon"):
        if fn.name in ["N1000.neon"]:
            continue # Unicode issues
        if fn.name in ["bigint-test.neon", "decimal.neon", "number-ceil.neon", "number-underscore.neon"]:
            continue # Python floats are not decimal floating point
        if fn.name in ["sodium-test.neon"]:
            continue # Just too big
        if fn.name in ["string-escape.neon"]:
            continue # Unicode output to file not working with ascii codec
        if fn.name in ["lexer-unicode.neon", "string-bytes.neon", "unicode-char.neon", "unicode-length.neon", "unicode-source.neon", "unicode-string.neon", "utf8-invalid.neon"]:
            continue # TODO (see t/unicode-length.neon)
        dump_cpp = env.Command("tmp/"+fn.name+".dump_cpp", [fn, test_lexer], "-{} $SOURCE >$TARGET".format(test_lexer[0]))
        dump_neon = env.Command("tmp/"+fn.name+".dump_neon", [fn, "neon/lexer.neon", neon], "-{} neon/lexer.neon $SOURCE >$TARGET".format(neon[0]))
        dump_helium = env.Command("tmp/"+fn.name+".dump_helium", [fn, "neon/lexer.neon", "tools/helium.py"], "-python tools/helium.py neon/lexer.neon $SOURCE >$TARGET")
        comp1 = env.Command("tmp/"+fn.name+".dump.dummy1", [dump_cpp, dump_neon], compare)
        comp2 = env.Command("tmp/"+fn.name+".dump.dummy2", [dump_cpp, dump_helium], compare)
        env.Depends("test_compiler", [comp1, comp2])
