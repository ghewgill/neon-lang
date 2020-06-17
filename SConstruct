from __future__ import print_function

import distutils.spawn
import functools
import operator
import os
import re
import shutil
import subprocess
import sys
import tarfile
import zipfile
from SCons.Script.SConscript import SConsEnvironment

squeeze = lambda a: [x for x in a if x]

# Assume a UTF-8 capable terminal.
os.putenv("PYTHONIOENCODING", "UTF-8")

coverage = ARGUMENTS.get("coverage", 0)
# This is needed on OS X because clang has a bug where this isn't included automatically.
coverage_lib = (["/Library/Developer/CommandLineTools/usr/lib/clang/6.0/lib/darwin/libclang_rt.profile_osx.a"] if coverage else [])

# Check for any files that accidentally contain \r\n. Only do this
# on non-windows platforms, because windows users may set Git to
# use crlf line endings.
if os.name != "nt":
    for subdir in ["contrib", "exec", "gh-pages", "lib", "samples", "scripts", "src", "t", "tests", "tools"]:
        for path, dirs, files in os.walk(subdir):
            if subdir == "lib":
                # Do not check subdirectories of lib (extension modules).
                dirs[:] = []
            for fn in files:
                if fn.endswith((".cpp", ".c", ".h", ".neon", ".txt", ".java", ".py", ".md")):
                    with open(os.path.join(path, fn), "rb") as f:
                        data = f.read()
                        assert b"\r\n" not in data, os.path.join(path, fn)

env = Environment()

vars = Variables(["config.cache", "config.py"])
vars.AddVariables(
    BoolVariable("BID_GENERIC", "Set to 1 to build a generic bid library", False),
    BoolVariable("RELEASE", "Set to 1 to build for release", False),
    ("CLANG_SANITIZE", "Options to the -fsanitize= flag for clang"),
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
#env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.path.dirname(sys.executable)
sys_executable = sys.executable

# Find where javac.exe is and add it to our PATH.
env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "javac.exe")))
try:
    s = subprocess.check_output("javac -version", stderr=subprocess.STDOUT, shell=True).decode()
except subprocess.CalledProcessError:
    s = None
m = s and re.search(r"^javac (\d+)\.(\d+)", s, re.MULTILINE)
use_java = m is not None and (int(m.group(1)), int(m.group(2))) >= (1, 8)
print("use_java: {}".format(use_java or (False, repr(s))))

if os.name == "nt":
    csc = "csc"
else:
    env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "mono")))
    try:
        s = subprocess.check_output("mono --version", stderr=subprocess.STDOUT, shell=True)
    except subprocess.CalledProcessError:
        s = None
    use_mono = s is not None
    print("use_mono: {}".format(use_mono or (False, repr(s))))
    if use_mono:
        csc = "mcs"

env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "rustc")) or os.path.exists(os.path.join(x, "rustc.exe")))
try:
    s = subprocess.check_output("rustc --version", stderr=subprocess.STDOUT, shell=True)
except subprocess.CalledProcessError:
    s = None
use_rust = s is not None
print("use_rust: {}".format(use_rust or (False, repr(s))))

env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "go")) or os.path.exists(os.path.join(x, "go.exe")))
try:
    s = subprocess.check_output("go version", stderr=subprocess.STDOUT, shell=True)
except subprocess.CalledProcessError:
    s = None
use_go = False; s = "not currently working" #s is not None
print("use_go: {}".format(use_go or (False, repr(s))))

env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.pathsep.join(x for x in os.getenv("PATH").split(os.pathsep) if os.path.exists(os.path.join(x, "node")) or os.path.exists(os.path.join(x, "node.exe")))
use_node = os.system("node --version") == 0
print("use_node: {}".format(use_node))

def add_external(target):
    env.Depends("external", target)
    return target

use_posix = os.name == "posix"

add_external(SConscript("external/SConscript-libutf8", exports=["env"]))
libbid = add_external(SConscript("external/SConscript-libbid", exports=["env"]))
libgmp = add_external(SConscript("external/SConscript-libgmp", exports=["env"]))
libhash = add_external(SConscript("external/SConscript-libhash", exports=["env"]))
libsqlite = add_external(SConscript("external/SConscript-libsqlite", exports=["env"]))
libminizip = add_external(SConscript("external/SConscript-libminizip", exports=["env"]))
libz = add_external(SConscript("external/SConscript-libz", exports=["env"]))
add_external(SConscript("external/SConscript-minijson", exports=["env"]))
add_external(SConscript("external/SConscript-pyparsing", exports=["env"]))

version_cpp = open("src/version.cpp.in").read()
version_cpp = version_cpp.replace("${GIT_DESCRIBE}", subprocess.check_output(["git", "describe", "--always", "--long", "--dirty"]).decode().strip())
open("src/version.cpp", "w").write(version_cpp)

modules = []
for module in os.listdir("lib"):
    sconstruct = os.path.join("lib", module, "SConstruct")
    if os.path.exists(sconstruct):
        modules.append(module)
        SConscript(sconstruct)
modules.sort()

SConscript("external/SConscript-naturaldocs")

buildenv = env.Clone()

buildenv.Append(CPPPATH=[
    Dir("common"),
    Dir("src"),
])
if sys.platform == "win32":
    buildenv.Append(CXXFLAGS=[
        "/EHsc",
        "/W4",
        "/WX",
        "/FS",
        "/wd4324", # structure was padded due to alignment specifier
    ])
    if not buildenv["RELEASE"]:
        buildenv.Append(LINKFLAGS=[
            "/DEBUG",
        ])
        buildenv.Append(CXXFLAGS=[
            "/MDd",
            "/Zi",
            "/Od",
        ])
    else:
        buildenv.Append(CXXFLAGS=[
            "/Ox",
            "/MD",
        ])
    buildenv.Append(LIBS=["user32", "wsock32"])
else:
    buildenv.Append(CXXFLAGS=[
        "-std=c++0x",
        "-Wall",
        "-Wextra",
        "-Weffc++",
        #"-Wold-style-cast",    # Enable this temporarily to check, but it breaks with gcc and #defines with C casts in standard headers.
        #"-Wshadow",            # Want to enable this, but the compiler used by travis is overzealous about enforcing it where we don't want it to
        "-Werror",
    ])
    if "CLANG_SANITIZE" in buildenv:
        buildenv.Append(CXXFLAGS=[
            "-fsanitize=" + buildenv["CLANG_SANITIZE"],
        ])
        buildenv.Append(LINKFLAGS=[
            "-fsanitize=" + buildenv["CLANG_SANITIZE"],
        ])
    if not buildenv["RELEASE"]:
        buildenv.Append(CXXFLAGS=[
            "-g",
        ])
    else:
        buildenv.Append(CXXFLAGS=[
            "-O3",
        ])
if os.name == "posix":
    buildenv.Append(LIBS=["dl"])
if sys.platform.startswith("linux"):
    buildenv.Append(LIBS=["rt"])

if "g++" in buildenv.subst("$CXX"):
    # This adds -Doverride= for GCC earlier than 4.7.
    # (GCC does not support 'override' before 4.7, but
    # it supports everything else we need.)
    try:
        ver = subprocess.check_output([buildenv.subst("$CXX"), "--version"])
        if "g++" in ver.split()[0]:
            ver = ver.split("\n")[0]
            ver = re.sub(r"\(.*?\)", "", ver)
            ver = float(re.search(r"(\d+\.\d+)\.", ver).group(1))
            if ver < 4.7:
                buildenv.Append(CXXFLAGS=["-Doverride="])
    except Exception as x:
        pass

if coverage:
    buildenv.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

buildenv.Command("src/unicodedata.inc", ["tools/helium.py", "scripts/make_unicode.neon", "data/UnicodeData.txt"], sys_executable + " $SOURCES > $TARGET")

rtl_cpp = squeeze([
    "lib/binary.cpp",
    "lib/console.cpp",
    "lib/datetime.cpp",
    "lib/debugger.cpp",
    "lib/global.cpp",
    "lib/file.cpp",
    "lib/io.cpp",
    "lib/math.cpp",
    "lib/net.cpp",
    "lib/os.cpp",
    "lib/posix.cpp" if use_posix else None,
    "lib/random.cpp",
    "lib/runtime.cpp",
    "lib/sqlite.cpp",
    "lib/string.cpp",
    "lib/struct.cpp",
    "lib/sys.cpp",
    "lib/textio.cpp",
    "lib/time.cpp",
])

rtl_neon = squeeze([
    "lib/binary.neon",
    "lib/console.neon",
    "lib/datetime.neon",
    "lib/debugger.neon",
    "lib/file.neon",
    "lib/global.neon",
    "lib/io.neon",
    "lib/math.neon",
    "lib/mmap.neon",
    "lib/net.neon",
    "lib/os.neon",
    "lib/posix.neon" if use_posix else None,
    "lib/process.neon",
    "lib/random.neon",
    "lib/runtime.neon",
    "lib/sqlite.neon",
    "lib/string.neon",
    "lib/struct.neon",
    "lib/sys.neon",
    "lib/textio.neon",
    "lib/time.neon",
])

lib_neon = Glob("lib/*.neon")

def build_rtl_inc(target, source, env):
    with open("src/rtl.inc", "w") as f:
        print("static struct {", file=f)
        print("    const char *name;", file=f)
        print("    const char *source;", file=f)
        print("} rtl_sources[] = {", file=f)
        for fn in source:
            print("    {{\"{}\",".format(fn.name.replace(".neon", "")), file=f)
            f.write("        \"" + "\\n\"\n        \"".join(x.replace("\\", "\\\\").replace('"', '\\"') for x in open(fn.abspath).read().split("\n")))
            print("\"},", file=f)
        print("};", file=f)

buildenv.Command("src/rtl.inc", lib_neon, build_rtl_inc)

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
        print("Unsupported platform:", sys.platform, file=sys.stderr)
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
    print("Unsupported platform:", os.name, file=sys.stderr)
    sys.exit(1)

buildenv.Command(["src/thunks.inc", "src/functions_compile.inc", "src/functions_exec.inc", "src/enums.inc", "src/exceptions.inc"], [rtl_neon, "scripts/make_thunks.py"], sys_executable + " scripts/make_thunks.py " + " ".join(rtl_neon))

if use_java:
    jvm_classes = buildenv.Java("rtl/jvm", "rtl/jvm")
    jnex = SConscript("exec/jnex/SConstruct")

if os.name == "nt" or use_mono:
    cli_library = env.Command("t/Neon.dll", "rtl/cli/Global.cs", csc + " /out:$TARGET /target:library $SOURCES")

neonc = buildenv.Program("bin/neonc", [
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/compiler.cpp",
    "src/compiler_cli.cpp",
    "src/compiler_cpp.cpp",
    "src/compiler_js.cpp",
    "src/compiler_jvm.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/intrinsic.cpp",
    "src/lexer.cpp",
    "src/neonc.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/rtl_compile.cpp",
    "src/sql.cpp",
    "src/support.cpp",
    "src/support_compiler.cpp",
    "src/util.cpp",
] + coverage_lib,
)
if use_java:
    buildenv.Depends(neonc, jvm_classes)

# Needed for USE_RTLX
#def build_rtlx_inc(target, source, env):
#    with open("src/rtlx.inc", "w") as f:
#        for fn in source:
#            bytecode = open(fn.abspath, "rb").read()
#            print("static const unsigned char bytecode_{}[] = {{{}}};".format(
#                fn.name.replace(".neonx", ""),
#                ",".join("0x{:02x}".format(x) for x in bytecode)
#            ), file=f)
#        print("static struct {", file=f)
#        print("    const char *name;", file=f)
#        print("    size_t length;", file=f)
#        print("    const unsigned char *bytecode;", file=f)
#        print("} rtl_bytecode[] = {", file=f)
#        for fn in source:
#            modname = fn.name.replace(".neonx", "")
#            bytecode = open(fn.abspath, "rb").read()
#            print("    {{\"{}\", {}, bytecode_{}}},".format(modname, len(bytecode), modname), file=f)
#        print("};", file=f)
#
#lib_neon_without_global = [x for x in lib_neon if x.name != "global.neon"]
#for fn in lib_neon_without_global:
#    buildenv.Command(fn.abspath+"x", [fn, neonc], neonc[0].abspath + " $SOURCE")
#buildenv.Command("src/rtlx.inc", [x.abspath+"x" for x in lib_neon_without_global], build_rtlx_inc)

neon = buildenv.Program("bin/neon", [
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/compiler.cpp",
    "src/debuginfo.cpp",
    "src/disassembler.cpp",
    "src/exec.cpp",
    "src/httpserver.cpp",
    "src/intrinsic.cpp",
    "src/lexer.cpp",
    "src/neon.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/repl.cpp",
    "src/rtl_compile.cpp",
    "src/rtl_exec.cpp",
    rtl_cpp,
    rtl_platform,
    "src/sql.cpp",
    "src/support.cpp",
    "src/support_compiler.cpp",
    "src/support_exec.cpp",
    "src/util.cpp",
    "src/version.cpp",
] + coverage_lib,
)

neonx = buildenv.Program("bin/neonx", [
    "src/bundle.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/disassembler.cpp",
    "src/exec.cpp",
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

neonstub = buildenv.Program("bin/neonstub", [
    "src/bundle.cpp",
    "src/bytecode.cpp",
    "src/cell.cpp",
    "src/disassembler.cpp",
    "src/exec.cpp",
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

neondis = buildenv.Program("bin/neondis", [
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

neonbind = buildenv.Program("bin/neonbind", [
    "src/bytecode.cpp",
    "src/neonbind.cpp",
    "src/support.cpp",
    "src/support_exec.cpp",
])

cnex = SConscript("exec/cnex/SConstruct", exports=["env"])
if use_rust:
    rsnex = SConscript("exec/rsnex/SConstruct")
if use_go:
    gonex = SConscript("exec/gonex/SConstruct")

buildenv.Depends("src/number.h", libbid)
buildenv.Depends("src/number.h", libgmp)

def UnitTest(env, target, source, **kwargs):
    t = env.Program(target, source, **kwargs)
    # see the following for the reason why this lambda is necessary:
    # http://stackoverflow.com/questions/8219743/scons-addpostaction-causes-dependency-check-error-work-around
    env.AddPostAction(t, lambda *_, **__: os.system(t[0].abspath))
    env.Alias("test", t)
    return t

buildenv.Command("src/errors.txt", ["scripts/extract_errors.neon"] + Glob("src/*.cpp"), sys_executable + " tools/helium.py scripts/extract_errors.neon")

SConsEnvironment.UnitTest = UnitTest

test_number = buildenv.Program("bin/test_number", [
    "tests/test_number.cpp",
    "src/number.cpp",
] + coverage_lib,
)

test_number_to_string = buildenv.Program("bin/test_number_to_string", [
    "tests/test_number_to_string.cpp",
    "src/number.cpp",
] + coverage_lib,
)

test_lexer = buildenv.UnitTest("bin/test_lexer", [
    "tests/test_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

test_parser = buildenv.Program("bin/test_parser", [
    "tests/test_parser.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/pt_dump.cpp",
    "src/sql.cpp",
    "src/util.cpp",
] + coverage_lib,
)

buildenv.Program("bin/fuzz_lexer", [
    "tests/fuzz_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

buildenv.Program("bin/fuzz_parser", [
    "tests/fuzz_parser.cpp",
    "src/analyzer.cpp",
    "src/ast.cpp",
    "src/bytecode.cpp",
    "src/compiler.cpp",
    "src/lexer.cpp",
    "src/intrinsic.cpp",
    "src/number.cpp",
    "src/parser.cpp",
    "src/rtl_compile.cpp",
    "src/sql.cpp",
    "src/util.cpp",
] + coverage_lib,
)

buildenv.Program("bin/perf_lexer", [
    "tests/perf_lexer.cpp",
    "src/lexer.cpp",
    "src/number.cpp",
    "src/util.cpp",
] + coverage_lib,
)

# TESTS # def filter_tests(tests, excludefile):
# TESTS #     with open(excludefile) as f:
# TESTS #         exclude = [re.sub(r"\s*(#.*)?$", "", x) for x in f.readlines()]
# TESTS #     return [x for x in tests if os.path.basename(x) not in exclude]
# TESTS #
# TESTS # test_sources = []
# TESTS # for f in Glob("t/*.neon"):
# TESTS #     test_sources.append(f)
# TESTS # for m in modules:
# TESTS #     for f in Glob(os.path.join("lib", m, "t", "*.neon")):
# TESTS #         test_sources.append(f)
# TESTS # tests = buildenv.Command("tests_normal", [neon, "scripts/run_test.py", test_sources], sys_executable + " scripts/run_test.py " + " ".join(x.path for x in test_sources))
# TESTS # buildenv.Command("tests_helium", [neon, "scripts/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " tools/helium.py\" " + " ".join(filter_tests((x.path for x in test_sources), "tools/helium-exclude.txt")))
# TESTS # if use_node:
# TESTS #     tests_js = buildenv.Command("tests_js", [neonc, "scripts/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " scripts/run_js.py\" " + " ".join(filter_tests((x.path for x in test_sources), "scripts/js-exclude.txt")))
# TESTS # if os.name == "nt" or use_mono:
# TESTS #     tests_cli = buildenv.Command("tests_cli", [neonc, "scripts/run_test.py", "scripts/run_cli.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " scripts/run_cli.py\" " + " ".join(filter_tests((x.path for x in test_sources), "scripts/cli-exclude.txt")))
# TESTS #     buildenv.Depends(tests_cli, cli_library)
# TESTS # if use_java:
# TESTS #     tests_jvm = buildenv.Command("tests_jvm", [neonc, "scripts/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " scripts/run_jvm.py\" " + " ".join(filter_tests((x.path for x in test_sources), "scripts/jvm-exclude.txt")))
# TESTS #     buildenv.Depends(tests_jvm, jvm_classes)
# TESTS # tests_cpp = buildenv.Command("tests_cpp", [neonc, "scripts/run_test.py", "scripts/run_cpp.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " scripts/run_cpp.py\" " + " ".join(filter_tests((x.path for x in test_sources), "scripts/cpp-exclude.txt")))
# TESTS # tests_nenex = buildenv.Command("tests_nenex", [neonc, "scripts/run_test.py", "exec/nenex/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " exec/nenex/run_test.py\" " + " ".join(filter_tests((x.path for x in test_sources), "exec/nenex/exclude.txt")))
# TESTS # tests_pynex = buildenv.Command("tests_pynex", [neonc, "scripts/run_test.py", "exec/pynex/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " exec/pynex/run_test.py\" " + " ".join(filter_tests((x.path for x in test_sources), "exec/pynex/exclude.txt")))
# TESTS # if use_java:
# TESTS #     tests_jnex = buildenv.Command("tests_jnex", [neonc, "scripts/run_test.py", "exec/jnex/run_test.py", jnex, test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " exec/jnex/run_test.py\" " + " ".join(filter_tests((x.path for x in test_sources), "exec/jnex/exclude.txt")))
# TESTS # tests_cnex = buildenv.Command("tests_cnex", [neonc, cnex, "scripts/run_test.py", "exec/cnex/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " exec/cnex/run_test.py\" " + " ".join(filter_tests((x.path for x in test_sources), "exec/cnex/exclude.txt")))
# TESTS # if use_rust:
# TESTS #     tests_rsnex = buildenv.Command("tests_rsnex", [neonc, rsnex, "scripts/run_test.py", "exec/rsnex/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " exec/rsnex/run_test.py\" " + " ".join(filter_tests((x.path for x in test_sources), "exec/rsnex/exclude.txt")))
# TESTS # if use_go:
# TESTS #     tests_gonex = buildenv.Command("tests_gonex", [neonc, gonex, "scripts/run_test.py", "exec/gonex/run_test.py", test_sources], sys_executable + " scripts/run_test.py --runner \"" + sys_executable + " exec/gonex/run_test.py\" " + " ".join(filter_tests((x.path for x in test_sources), "exec/gonex/exclude.txt")))
# TESTS # testenv = buildenv.Clone()
# TESTS # testenv["ENV"]["NEONPATH"] = "t/"
# TESTS # testenv.Command("tests_error", [neon, "scripts/run_test.py", "src/errors.txt", Glob("t/errors/*")], sys_executable + " scripts/run_test.py --errors t/errors")
# TESTS # buildenv.Command("tests_number", test_number, test_number[0].path)
# TESTS # buildenv.Command("tests_number_to_string", test_number_to_string, test_number_to_string[0].path)
# TESTS # for f in Glob("t/repl_*.neon"):
# TESTS #     result = f.abspath + ".result"
# TESTS #     t = buildenv.Command(result, [f, neon], neon[0].abspath + " --repl-no-prompt --repl-stop-on-any-error <$SOURCE")
# TESTS #     buildenv.Alias("tests_repl", t)
# TESTS #
# TESTS # samples = []
# TESTS # for path, dirs, files in os.walk("."):
# TESTS #     if all(x not in ["t", "tests"] for x in path.split(os.sep)):
# TESTS #         for f in files:
# TESTS #             if f.endswith(".neon") and f != "global.neon":
# TESTS #                 if f in ["sdl.neon", "flappy.neon", "life.neon", "mandelbrot.neon", "spacedebris.neon"]:
# TESTS #                     continue
# TESTS #                 modules = subprocess.check_output([sys_executable, "tools/helium.py", "tools/imports.neon", os.path.join(path, f)]).decode().split()
# TESTS #                 missing = [m for m in modules if not os.path.exists("lib/{}.neon".format(m)) and not os.path.exists("lib/{}/{}.neon".format(m, m)) and not os.path.exists(os.path.join(path, m+".neon"))]
# TESTS #                 if not missing:
# TESTS #                     samples.append(os.path.join(path, f))
# TESTS #                 else:
# TESTS #                     print("note: skipping sample {} because of missing modules {}".format(f, missing))
# TESTS # for sample in samples:
# TESTS #     buildenv.Command(sample+"x", [sample, neonc], neonc[0].abspath + " $SOURCE")
# TESTS # buildenv.Command("tests_2", ["samples/hello/hello.neonx", neonx], neonx[0].abspath + " $SOURCE")
# TESTS #
# TESTS # # Removed neon/*.neon because those take too long with pyparsing under python3
# TESTS # buildenv.Command("test_grammar", ["contrib/grammar/neon.ebnf", "src/token.h", "src/parser.cpp"], sys_executable + " contrib/grammar/test-grammar.py lib/*.neon tools/*.neon {} t/*.neon t/errors/N3*.neon >$TARGET".format(" ".join(x for x in functools.reduce(operator.add, ([os.path.join(path, x) for x in files] for path, dirs, files in os.walk("samples"))) if x.endswith(".neon"))))
# TESTS # buildenv.Command("test_grammar_random", "contrib/grammar/neon.ebnf", sys_executable + " contrib/grammar/test-random.py")
# TESTS # buildenv.Command("contrib/grammar/neon.w3c.ebnf", ["contrib/grammar/neon.ebnf", "contrib/grammar/ebnf_w3c.neon", neon], sys_executable + " tools/helium.py contrib/grammar/ebnf_w3c.neon <$SOURCE >$TARGET")
# TESTS #
# TESTS # buildenv.Command("test_doc", None, sys_executable + " scripts/test_doc.py")
# TESTS #
# TESTS # if os.name == "posix":
# TESTS #     buildenv.Command("tmp/hello", "samples/hello/hello.neon", "echo '#!/usr/bin/env neon' | cat - $SOURCE >$TARGET && chmod +x $TARGET")
# TESTS #     buildenv.Command("tests_script", "tmp/hello", "env PATH=bin tmp/hello")
# TESTS #
# TESTS # hello_neb = buildenv.Command("tmp/hello.neb", ["samples/hello/hello.neonx", neonbind], "{} $TARGET $SOURCE".format(neonbind[0].path))
# TESTS # buildenv.Command("test_hello_neb", [hello_neb, neonx], "{} $SOURCE".format(neonx[0].path))
# TESTS #
# TESTS # hello_exe = buildenv.Command("tmp/hello.exe", ["samples/hello/hello.neonx", neonbind, neonstub], "{} -e $TARGET $SOURCE".format(neonbind[0].path))
# TESTS # buildenv.Command("test_hello_exe", hello_exe, hello_exe[0].path)
# TESTS # cal_exe = buildenv.Command("tmp/cal.exe", ["samples/cal/cal.neonx", neonbind, neonstub], "{} -e $TARGET $SOURCE".format(neonbind[0].path))
# TESTS # buildenv.Command("test_cal", cal_exe, cal_exe[0].path)
# TESTS #
# TESTS # # Need to find where perl actually is, in case it's not in
# TESTS # # one of the paths supplied by scons by default (for example,
# TESTS # # on Windows with the GitHub command prompt).
# TESTS # perl = distutils.spawn.find_executable("perl")
# TESTS # if perl:
# TESTS #     buildenv.Command("docs", None, perl + " external/NaturalDocs/NaturalDocs -i lib -o HTML gh-pages/html -p lib/nd.proj -ro -xi lib/compress/bzip2-1.0.6 -xi lib/compress/zlib-1.2.8 -xi lib/crypto/include -xi lib/crypto/libressl-2.2.4 -xi lib/fltk/fltk-1.3.4-1 -xi lib/hash/include -xi lib/hash/libressl-2.2.4 -xi lib/http/curl-7.41.0 -xi lib/http/libressl-2.2.4 -xi lib/http/zlib-1.2.8 -xi lib/http/include -xi lib/regex/pcre2-10.10 -xi lib/sdl/SDL2-2.0.3")
# TESTS #     buildenv.Command("docs_samples", None, perl + " external/NaturalDocs/NaturalDocs -i samples -o HTML gh-pages/samples -p samples/nd.proj -ro")
# TESTS #
# TESTS # def compare(target, source, env):
# TESTS #     os.system("diff -u -w {} {}".format(source[0].path, source[1].path))
# TESTS #     assert open(source[0].path).read() == open(source[1].path).read()
# TESTS #
# TESTS # lexer_dump_cpp = buildenv.Command("tmp/lexer-coverage.dump_cpp", ["tests/lexer-coverage.neon", test_lexer], "-{} $SOURCE >$TARGET".format(test_lexer[0]))
# TESTS # lexer_dump_neon = buildenv.Command("tmp/lexer-coverage.dump_neon", ["tests/lexer-coverage.neon", "neon/lexer.neon", neon], "-{} neon/lexer.neon $SOURCE >$TARGET".format(neon[0]))
# TESTS # lexer_dump_helium = buildenv.Command("tmp/lexer-coverage.dump_helium", ["tests/lexer-coverage.neon", "neon/lexer.neon", "tools/helium.py"], sys_executable + " tools/helium.py neon/lexer.neon $SOURCE >$TARGET")
# TESTS # for i, d in enumerate([lexer_dump_cpp, lexer_dump_neon, lexer_dump_helium]):
# TESTS #     buildenv.Command("tmp/lexer-coverage.dump.dummy{}".format(i), ["tests/lexer-coverage.expected", d], compare)
# TESTS #
# TESTS # parser_dump_cpp = buildenv.Command("tmp/parser-coverage.dump_cpp", ["tests/parser-coverage.neon", test_parser], "-{} $SOURCE >$TARGET".format(test_parser[0]))
# TESTS # parser_dump_neon = buildenv.Command("tmp/parser-coverage.dump_neon", ["tests/parser-coverage.neon", "neon/parser.neon", neon], "-{} neon/parser.neon $SOURCE >$TARGET".format(neon[0]))
# TESTS # # TODO parser_dump_helium = buildenv.Command("tmp/parser-coverage.dump_helium", ["tests/parser-coverage.neon", "neon/parser.neon", "tools/helium.py"], sys_executable + " tools/helium.py neon/parser.neon $SOURCE >$TARGET")
# TESTS # for i, d in enumerate([parser_dump_cpp, parser_dump_neon]): # TODO , parser_dump_helium]):
# TESTS #     buildenv.Command("tmp/parser-coverage.dump.dummy{}".format(i), ["tests/parser-coverage.expected", d], compare)
# TESTS #
# TESTS # if False: # This takes rather too long.
# TESTS #     for fn in Glob("t/*.neon") + Glob("t/errors/*.neon"):
# TESTS #         if fn.name in ["N1000.neon"]:
# TESTS #             continue # Unicode issues
# TESTS #         if fn.name in ["decimal.neon", "number-ceil.neon", "number-underscore.neon"]:
# TESTS #             continue # Python floats are not decimal floating point
# TESTS #         if fn.name in ["sodium-test.neon"]:
# TESTS #             continue # Just too big
# TESTS #         if fn.name in ["string-escape.neon"]:
# TESTS #             continue # Unicode output to file not working with ascii codec
# TESTS #         if fn.name in ["lexer-unicode.neon", "string-bytes.neon", "unicode-char.neon", "unicode-length.neon", "unicode-source.neon", "unicode-string.neon", "utf8-invalid.neon"]:
# TESTS #             continue # TODO (see t/unicode-length.neon)
# TESTS #         dump_cpp = buildenv.Command("tmp/"+fn.name+".dump_cpp", [fn, test_lexer], "-{} $SOURCE >$TARGET".format(test_lexer[0]))
# TESTS #         dump_neon = buildenv.Command("tmp/"+fn.name+".dump_neon", [fn, "neon/lexer.neon", neon], "-{} neon/lexer.neon $SOURCE >$TARGET".format(neon[0]))
# TESTS #         dump_helium = buildenv.Command("tmp/"+fn.name+".dump_helium", [fn, "neon/lexer.neon", "tools/helium.py"], sys_executable + " tools/helium.py neon/lexer.neon $SOURCE >$TARGET")
# TESTS #         comp1 = buildenv.Command("tmp/"+fn.name+".dump.dummy1", [dump_cpp, dump_neon], compare)
# TESTS #         comp2 = buildenv.Command("tmp/"+fn.name+".dump.dummy2", [dump_cpp, dump_helium], compare)
# TESTS #         buildenv.Depends("test_compiler", [comp1, comp2])
