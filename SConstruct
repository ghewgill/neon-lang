import os
import re
import shutil
import subprocess
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

env["ENV"]["PROCESSOR_ARCHITECTURE"] = os.getenv("PROCESSOR_ARCHITECTURE")
env["ENV"]["PROCESSOR_ARCHITEW6432"] = os.getenv("PROCESSOR_ARCHITEW6432")

# Add path of Python itself to shell PATH.
env["ENV"]["PATH"] = env["ENV"]["PATH"] + os.pathsep + os.path.dirname(sys.executable)

if not os.path.exists("external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak"):
    tarfile.open("external/IntelRDFPMathLib20U1.tar.gz").extractall("external")
if sys.platform == "win32":
    libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.lib", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && nmake -fmakefile.mak CC=cl GLOBAL_RND=1 GLOBAL_FLAGS=1")
else:
    libbid = env.Command("external/IntelRDFPMathLib20U1/LIBRARY/libbid.a", "external/IntelRDFPMathLib20U1/LIBRARY/makefile.mak", "cd external/IntelRDFPMathLib20U1/LIBRARY && make CC=gcc GLOBAL_RND=1 GLOBAL_FLAGS=1")

libffi = SConscript("SConscript-libffi", exports=["env"])

if sys.platform == "win32":
    if not os.path.exists("external/PDCurses-3.4/win32/vcwin32.mak"):
        tarfile.open("external/PDCurses-3.4.tar.gz").extractall("external")
    libs_curses = [env.Command("external/PDCurses-3.4/win32/pdcurses.lib", "external/PDCurses-3.4/win32/vcwin32.mak", "cd external/PDCurses-3.4/win32 && nmake -fvcwin32.mak WIDE=Y UTF8=Y")]
    libs_curses.extend(["advapi32", "user32"])
else:
    libs_curses = ["ncurses"]

if not os.path.exists("external/utf8/source/utf8.h"):
    zipfile.ZipFile("external/utf8_v2_3_4.zip").extractall("external/utf8")

if not os.path.exists("external/pcre2-10.10/configure"):
    tarfile.open("external/pcre2-10.10.tar.gz").extractall("external")
if sys.platform == "win32":
    shutil.copyfile("external/pcre2-10.10/src/config.h.generic", "external/pcre2-10.10/src/config.h")
    shutil.copyfile("external/pcre2-10.10/src/pcre2.h.generic", "external/pcre2-10.10/src/pcre2.h")
    pcreenv = env.Clone()
    pcreenv.Append(CPPFLAGS=[
        "-DHAVE_CONFIG_H",
        "-DPCRE2_CODE_UNIT_WIDTH=8",
        "-DPCRE2_STATIC",
    ])
    pcreenv.Command("external/pcre2-10.10/src/pcre2_chartables.c", "external/pcre2-10.10/src/pcre2_chartables.c.dist", lambda target, source, env: shutil.copyfile(source[0].path, target[0].path))
    libpcre = pcreenv.Library("external/pcre2-10.10/pcre2.lib", [
        "external/pcre2-10.10/src/pcre2_auto_possess.c",
        "external/pcre2-10.10/src/pcre2_chartables.c",
        "external/pcre2-10.10/src/pcre2_compile.c",
        "external/pcre2-10.10/src/pcre2_config.c",
        "external/pcre2-10.10/src/pcre2_context.c",
        "external/pcre2-10.10/src/pcre2_dfa_match.c",
        "external/pcre2-10.10/src/pcre2_error.c",
        "external/pcre2-10.10/src/pcre2_jit_compile.c",
        "external/pcre2-10.10/src/pcre2_maketables.c",
        "external/pcre2-10.10/src/pcre2_match.c",
        "external/pcre2-10.10/src/pcre2_match_data.c",
        "external/pcre2-10.10/src/pcre2_newline.c",
        "external/pcre2-10.10/src/pcre2_ord2utf.c",
        "external/pcre2-10.10/src/pcre2_pattern_info.c",
        "external/pcre2-10.10/src/pcre2_serialize.c",
        "external/pcre2-10.10/src/pcre2_string_utils.c",
        "external/pcre2-10.10/src/pcre2_study.c",
        "external/pcre2-10.10/src/pcre2_substitute.c",
        "external/pcre2-10.10/src/pcre2_substring.c",
        "external/pcre2-10.10/src/pcre2_tables.c",
        "external/pcre2-10.10/src/pcre2_ucd.c",
        "external/pcre2-10.10/src/pcre2_valid_utf.c",
        "external/pcre2-10.10/src/pcre2_xclass.c",
    ])
else:
    libpcre = env.Command("external/pcre2-10.10/.libs/libpcre2-8.a", "external/pcre2-10.10/configure", "cd external/pcre2-10.10 && ./configure && make")

if not os.path.exists("external/curl-7.41.0/configure"):
    tarfile.open("external/curl-7.41.0.tar.gz").extractall("external")
    config_win32 = open("external/curl-7.41.0/lib/config-win32.h").read()
    config_win32 = "#define HTTP_ONLY\n" + config_win32
    open("external/curl-7.41.0/lib/config-win32.h", "w").write(config_win32)
if sys.platform == "win32":
    libcurl = env.Command("external/curl-7.41.0/lib/libcurl.lib", "external/curl-7.41.0/winbuild/Makefile.vc", "cd external/curl-7.41.0/lib && nmake /f Makefile.vc10 CFG=release MACHINE=x64 RTLIBCFG=static")
    env.Append(CPPFLAGS=["-DCURL_STATICLIB"])
    env.Append(LIBS=["ws2_32"])
else:
    libcurl = env.Command("external/curl-7.41.0/lib/.libs/libcurl.a", "external/curl-7.41.0/configure", "cd external/curl-7.41.0 && ./configure --disable-ares --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-proxy --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher --disable-manual --disable-ipv6 --disable-threaded-resolver --disable-sspi --disable-crypto-auth --disable-ntlm-wb --disable-tls-srp --disable-unix-sockets --disable-cookies --disable-soname-bump --without-zlib --without-winssl --without-darwinssl --without-ssl --without-gnutls --without-polarssl --without-cyassl --without-nss --without-axtls --without-libmetalink --without-libssh2 --without-librtmp --without-winidn --without-libidn --without-nghttp2 && make")

if not os.path.exists("external/easysid-version-1.0/SConstruct"):
    tarfile.open("external/easysid-version-1.0.tar.gz").extractall("external")
libeasysid = env.Command("external/easysid-version-1.0/libeasysid"+env["SHLIBSUFFIX"], "external/easysid-version-1.0/SConstruct", "cd external/easysid-version-1.0 && " + sys.executable + " " + sys.argv[0])

if not os.path.exists("external/hash-library/sha256.cpp"):
    zipfile.ZipFile("external/hash-library.zip").extractall("external/hash-library")
    if sys.platform == "darwin":
        subprocess.check_call("perl -n -i -e 'print unless /<endian.h>/' external/hash-library/*.cpp", shell=True)
hash_env = env.Clone()
hash_lib = hash_env.Library("external/hash-library/hash-library", [
    "external/hash-library/crc32.cpp",
    "external/hash-library/md5.cpp",
    "external/hash-library/sha1.cpp",
    "external/hash-library/sha256.cpp",
    "external/hash-library/sha3.cpp",
])

if not os.path.exists("external/sqlite-amalgamation-3080803/sqlite3.c"):
    zipfile.ZipFile("external/sqlite-amalgamation-3080803.zip").extractall("external")
sqliteenv = env.Clone()
sqliteenv.Append(CPPFLAGS=["-DSQLITE_THREADSAFE=0"])
sqlite = sqliteenv.Object("external/sqlite-amalgamation-3080803/sqlite3.c")

if not os.path.exists("external/zlib-1.2.8/configure"):
    tarfile.open("external/zlib-1.2.8.tar.gz").extractall("external")
if sys.platform == "win32":
    libz = env.Library("external/zlib-1.2.8/libz.lib", [
        "external/zlib-1.2.8/adler32.c",
        "external/zlib-1.2.8/compress.c",
        "external/zlib-1.2.8/crc32.c",
        "external/zlib-1.2.8/deflate.c",
        "external/zlib-1.2.8/inffast.c",
        "external/zlib-1.2.8/inflate.c",
        "external/zlib-1.2.8/inftrees.c",
        "external/zlib-1.2.8/trees.c",
        "external/zlib-1.2.8/uncompr.c",
        "external/zlib-1.2.8/zutil.c",
    ])
else:
    libz = env.Command("external/zlib-1.2.8/libz.a", "external/zlib-1.2.8/configure", "cd external/zlib-1.2.8 && ./configure --static && make")

if not os.path.exists("external/bzip2-1.0.6/Makefile"):
    tarfile.open("external/bzip2-1.0.6.tar.gz").extractall("external")
if sys.platform == "win32":
    libbz2 = env.Library("external/bzip2-1.0.6/libbz2.lib", [
        "external/bzip2-1.0.6/blocksort.c",
        "external/bzip2-1.0.6/bzlib.c",
        "external/bzip2-1.0.6/compress.c",
        "external/bzip2-1.0.6/crctable.c",
        "external/bzip2-1.0.6/decompress.c",
        "external/bzip2-1.0.6/huffman.c",
        "external/bzip2-1.0.6/randtable.c",
    ])
else:
    libbz2 = env.Command("external/bzip2-1.0.6/libbz2.a", "external/bzip2-1.0.6/Makefile", "cd external/bzip2-1.0.6 && make")

if not os.path.exists("external/xz-5.2.1/configure"):
    tarfile.open("external/xz-5.2.1.tar.gz").extractall("external")
if sys.platform == "win32":
    env.Append(CPPDEFINES=[
        "LZMA_API_STATIC",
    ])
    lzmaenv = env.Clone()
    lzmaenv.Append(CPPPATH=[
        "external/xz-5.2.1/src/common",
        "external/xz-5.2.1/src/liblzma/api",
        "external/xz-5.2.1/src/liblzma/check",
        "external/xz-5.2.1/src/liblzma/common",
        "external/xz-5.2.1/src/liblzma/delta",
        "external/xz-5.2.1/src/liblzma/lz",
        "external/xz-5.2.1/src/liblzma/lzma",
        "external/xz-5.2.1/src/liblzma/rangecoder",
        "external/xz-5.2.1/src/liblzma/simple",
        "external/xz-5.2.1/windows",
    ])
    lzmaenv.Append(CPPDEFINES=[
        ("DWORD", "unsigned long"),
        "HAVE_CONFIG_H",
    ])
    liblzma = lzmaenv.Library("external/xz-5.2.1/src/liblzma/liblzma.lib", [
        "external/xz-5.2.1/src/liblzma/check/check.c",
        "external/xz-5.2.1/src/liblzma/check/crc32_fast.c",
        "external/xz-5.2.1/src/liblzma/check/crc32_table.c",
        "external/xz-5.2.1/src/liblzma/check/crc64_fast.c",
        "external/xz-5.2.1/src/liblzma/check/crc64_table.c",
        "external/xz-5.2.1/src/liblzma/check/sha256.c",
        "external/xz-5.2.1/src/liblzma/delta/delta_common.c",
        "external/xz-5.2.1/src/liblzma/delta/delta_decoder.c",
        "external/xz-5.2.1/src/liblzma/delta/delta_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/block_header_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/block_header_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/block_buffer_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/block_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/block_util.c",
        "external/xz-5.2.1/src/liblzma/common/common.c",
        "external/xz-5.2.1/src/liblzma/common/easy_buffer_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/easy_preset.c",
        "external/xz-5.2.1/src/liblzma/common/filter_common.c",
        "external/xz-5.2.1/src/liblzma/common/filter_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/filter_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/filter_flags_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/filter_flags_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/index.c",
        "external/xz-5.2.1/src/liblzma/common/index_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/index_hash.c",
        "external/xz-5.2.1/src/liblzma/common/stream_buffer_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/stream_buffer_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/stream_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/stream_flags_common.c",
        "external/xz-5.2.1/src/liblzma/common/stream_flags_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/stream_flags_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/vli_decoder.c",
        "external/xz-5.2.1/src/liblzma/common/vli_encoder.c",
        "external/xz-5.2.1/src/liblzma/common/vli_size.c",
        "external/xz-5.2.1/src/liblzma/lz/lz_decoder.c",
        "external/xz-5.2.1/src/liblzma/lz/lz_encoder.c",
        "external/xz-5.2.1/src/liblzma/lz/lz_encoder_mf.c",
        "external/xz-5.2.1/src/liblzma/lzma/fastpos_table.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma_decoder.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma_encoder.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma_encoder_optimum_fast.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma_encoder_optimum_normal.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma_encoder_presets.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma2_decoder.c",
        "external/xz-5.2.1/src/liblzma/lzma/lzma2_encoder.c",
        "external/xz-5.2.1/src/liblzma/rangecoder/price_table.c",
        "external/xz-5.2.1/src/liblzma/simple/arm.c",
        "external/xz-5.2.1/src/liblzma/simple/armthumb.c",
        "external/xz-5.2.1/src/liblzma/simple/ia64.c",
        "external/xz-5.2.1/src/liblzma/simple/powerpc.c",
        "external/xz-5.2.1/src/liblzma/simple/simple_coder.c",
        "external/xz-5.2.1/src/liblzma/simple/simple_decoder.c",
        "external/xz-5.2.1/src/liblzma/simple/simple_encoder.c",
        "external/xz-5.2.1/src/liblzma/simple/sparc.c",
        "external/xz-5.2.1/src/liblzma/simple/x86.c",
    ])
else:
    liblzma = env.Command("external/xz-5.2.1/src/liblzma/.libs/liblzma.a", "external/xz-5.2.1/configure", "cd external/xz-5.2.1 && ./configure && make")

env.Append(CPPPATH=[
    "external/IntelRDFPMathLib20U1/LIBRARY/src",
    "external/utf8/source",
    "external/lib/libffi-3.2.1/include",
    "external/PDCurses-3.4",
    "external/hash-library",
    "external/pcre2-10.10/src",
    "external/curl-7.41.0/include",
    "external/sqlite-amalgamation-3080803",
    "external/zlib-1.2.8",
    "external/bzip2-1.0.6",
    "external/xz-5.2.1/src/liblzma/api",
    "src",
])
env.Append(LIBS=[hash_lib])
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
env.Append(LIBS=[libbid, libffi, libpcre, libcurl, libz, libbz2, liblzma] + libs_curses)
if os.name == "posix":
    env.Append(LIBS=["dl"])
if sys.platform.startswith("linux"):
    env.Append(LIBS=["rt"])

# This adds -Doverride= for GCC earlier than 4.7.
# (GCC does not support 'override' before 4.7, but
# it supports everything else we need.)
try:
    ver = subprocess.check_output([env["CXX"], "--version"])
    if ver.startswith("g++"):
        ver = ver.split("\n")[0]
        ver = re.sub(r"\(.*?\)", "", ver)
        ver = float(re.search(r"(\d+\.\d+)\.", ver).group(1))
        if ver < 4.7:
            env.Append(CXXFLAGS=["-Doverride="])
except Exception as x:
    print x
    pass

if coverage:
    env.Append(CXXFLAGS=[
        "--coverage", "-O0",
    ])

rtl_const = [
    "lib/math_const.cpp",
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

env.Command(["src/thunks.inc", "src/functions_compile.inc", "src/functions_exec.inc"], [rtl_neon, "scripts/make_thunks.py"], sys.executable + " scripts/make_thunks.py " + " ".join(rtl_neon))

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
    sqlite,
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
    sqlite,
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
env.Depends(tests, test_ffi)
testenv = env.Clone()
testenv["ENV"]["NEONPATH"] = "t/"
testenv.Command("tests_error", [neon, "scripts/run_test.py", "src/errors.txt", Glob("t/errors/*")], sys.executable + " scripts/run_test.py --errors t/errors")
env.Command("tests_number", test_number_to_string, test_number_to_string[0].path)

for sample in Glob("samples/*.neon"):
    env.Command(sample.path+"x", [sample, neonc], neonc[0].abspath + " $SOURCE")
env.Command("tests_2", ["samples/hello.neonx", neonx], neonx[0].abspath + " $SOURCE")

env.Command("test_doc", None, sys.executable + " scripts/test_doc.py")

if os.name == "posix":
    env.Command("samples/hello", "samples/hello.neon", "echo '#!/usr/bin/env neon' | cat - $SOURCE >$TARGET && chmod +x $TARGET")
    env.Command("tests_script", "samples/hello", "env PATH=bin samples/hello")
