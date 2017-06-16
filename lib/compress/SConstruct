import os
import shutil
import sys
import tarfile

env = Environment()
if sys.platform == "win32":
    env.Append(CXXFLAGS=["/EHsc", "/MDd", "/W4", "/WX"])
else:
    env.Append(CXXFLAGS=["-std=c++0x"])

libz = None

libzenv = Environment()
if "RELEASE" not in env or not env["RELEASE"]:
    if sys.platform == "win32":
        libzenv.Append(CFLAGS=[
            "/MDd",
            "/Zi",
            "/Od",
        ])

if GetOption("clean"):
    shutil.rmtree("zlib-1.2.8", ignore_errors=True)
elif not os.path.exists("zlib-1.2.8/configure"):
    tarfile.open("zlib-1.2.8.tar.gz").extractall(".")

if sys.platform == "win32":
    libz = libzenv.Library("zlib-1.2.8/zlib.lib", [
        "zlib-1.2.8/adler32.c",
        "zlib-1.2.8/compress.c",
        "zlib-1.2.8/crc32.c",
        "zlib-1.2.8/deflate.c",
        "zlib-1.2.8/inffast.c",
        "zlib-1.2.8/inflate.c",
        "zlib-1.2.8/inftrees.c",
        "zlib-1.2.8/trees.c",
        "zlib-1.2.8/uncompr.c",
        "zlib-1.2.8/zutil.c",
    ])
    env.Append(CPPPATH=["zlib-1.2.8"])
else:
    conf = Configure(env)
    if not conf.CheckLibWithHeader("z", "zlib.h", "C++"):
        libz = libzenv.Command("zlib-1.2.8/libz.a", "zlib-1.2.8/configure", "cd lib/compress/zlib-1.2.8 && env CFLAGS=-fPIC ./configure --static && make")
        env.Append(CPPPATH=["zlib-1.2.8"])
    conf.Finish()

libbz2 = None

bz2env = Environment()
if "RELEASE" not in env or not env["RELEASE"]:
    if sys.platform == "win32":
        bz2env.Append(CFLAGS=[
            "/MDd",
            "/Zi",
            "/Od",
        ])

if GetOption("clean"):
    shutil.rmtree("bzip2-1.0.6", ignore_errors=True)
elif not os.path.exists("bzip2-1.0.6/Makefile"):
    tarfile.open("bzip2-1.0.6.tar.gz").extractall(".")
    makefile = open("bzip2-1.0.6/Makefile").read()
    makefile = makefile.replace("CFLAGS=", "CFLAGS=-fPIC ")
    open("bzip2-1.0.6/Makefile", "w").write(makefile)

if sys.platform == "win32":
    libbz2 = bz2env.Library("bzip2-1.0.6/libbz2.lib", [
        "bzip2-1.0.6/blocksort.c",
        "bzip2-1.0.6/bzlib.c",
        "bzip2-1.0.6/compress.c",
        "bzip2-1.0.6/crctable.c",
        "bzip2-1.0.6/decompress.c",
        "bzip2-1.0.6/huffman.c",
        "bzip2-1.0.6/randtable.c",
    ])
    env.Append(CPPPATH=["bzip2-1.0.6"])
else:
    conf = Configure(env)
    if not conf.CheckLibWithHeader("bz2", "bzlib.h", "C++"):
        libbz2 = bz2env.Command("bzip2-1.0.6/libbz2.a", "bzip2-1.0.6/Makefile", "cd lib/compress/bzip2-1.0.6 && make")
        env.Append(CPPPATH=["bzip2-1.0.6"])
    conf.Finish()

liblzma = None

lzmaenv = Environment()
if "RELEASE" not in env or not env["RELEASE"]:
    if sys.platform == "win32":
        lzmaenv.Append(CFLAGS=[
            "/MDd",
            "/Zi",
            "/Od",
        ])

if GetOption("clean"):
    shutil.rmtree("xz-5.2.1", ignore_errors=True)
elif not os.path.exists("xz-5.2.1/configure"):
    tarfile.open("xz-5.2.1.tar.gz").extractall(".")

if sys.platform == "win32":
    env.Append(CPPDEFINES=[
        "LZMA_API_STATIC",
    ])
    lzmaenv.Append(CPPDEFINES=[
        "LZMA_API_STATIC",
    ])
    lzmaenv.Append(CPPPATH=[
        "xz-5.2.1/src/common",
        "xz-5.2.1/src/liblzma/api",
        "xz-5.2.1/src/liblzma/check",
        "xz-5.2.1/src/liblzma/common",
        "xz-5.2.1/src/liblzma/delta",
        "xz-5.2.1/src/liblzma/lz",
        "xz-5.2.1/src/liblzma/lzma",
        "xz-5.2.1/src/liblzma/rangecoder",
        "xz-5.2.1/src/liblzma/simple",
        "xz-5.2.1/windows",
    ])
    lzmaenv.Append(CPPDEFINES=[
        ("DWORD", "unsigned long"),
        "HAVE_CONFIG_H",
    ])
    liblzma = lzmaenv.Library("xz-5.2.1/src/liblzma/liblzma.lib", [
        "xz-5.2.1/src/liblzma/check/check.c",
        "xz-5.2.1/src/liblzma/check/crc32_fast.c",
        "xz-5.2.1/src/liblzma/check/crc32_table.c",
        "xz-5.2.1/src/liblzma/check/crc64_fast.c",
        "xz-5.2.1/src/liblzma/check/crc64_table.c",
        "xz-5.2.1/src/liblzma/check/sha256.c",
        "xz-5.2.1/src/liblzma/delta/delta_common.c",
        "xz-5.2.1/src/liblzma/delta/delta_decoder.c",
        "xz-5.2.1/src/liblzma/delta/delta_encoder.c",
        "xz-5.2.1/src/liblzma/common/block_header_decoder.c",
        "xz-5.2.1/src/liblzma/common/block_header_encoder.c",
        "xz-5.2.1/src/liblzma/common/block_buffer_encoder.c",
        "xz-5.2.1/src/liblzma/common/block_decoder.c",
        "xz-5.2.1/src/liblzma/common/block_util.c",
        "xz-5.2.1/src/liblzma/common/common.c",
        "xz-5.2.1/src/liblzma/common/easy_buffer_encoder.c",
        "xz-5.2.1/src/liblzma/common/easy_preset.c",
        "xz-5.2.1/src/liblzma/common/filter_common.c",
        "xz-5.2.1/src/liblzma/common/filter_decoder.c",
        "xz-5.2.1/src/liblzma/common/filter_encoder.c",
        "xz-5.2.1/src/liblzma/common/filter_flags_decoder.c",
        "xz-5.2.1/src/liblzma/common/filter_flags_encoder.c",
        "xz-5.2.1/src/liblzma/common/index.c",
        "xz-5.2.1/src/liblzma/common/index_encoder.c",
        "xz-5.2.1/src/liblzma/common/index_hash.c",
        "xz-5.2.1/src/liblzma/common/stream_buffer_decoder.c",
        "xz-5.2.1/src/liblzma/common/stream_buffer_encoder.c",
        "xz-5.2.1/src/liblzma/common/stream_decoder.c",
        "xz-5.2.1/src/liblzma/common/stream_flags_common.c",
        "xz-5.2.1/src/liblzma/common/stream_flags_decoder.c",
        "xz-5.2.1/src/liblzma/common/stream_flags_encoder.c",
        "xz-5.2.1/src/liblzma/common/vli_decoder.c",
        "xz-5.2.1/src/liblzma/common/vli_encoder.c",
        "xz-5.2.1/src/liblzma/common/vli_size.c",
        "xz-5.2.1/src/liblzma/lz/lz_decoder.c",
        "xz-5.2.1/src/liblzma/lz/lz_encoder.c",
        "xz-5.2.1/src/liblzma/lz/lz_encoder_mf.c",
        "xz-5.2.1/src/liblzma/lzma/fastpos_table.c",
        "xz-5.2.1/src/liblzma/lzma/lzma_decoder.c",
        "xz-5.2.1/src/liblzma/lzma/lzma_encoder.c",
        "xz-5.2.1/src/liblzma/lzma/lzma_encoder_optimum_fast.c",
        "xz-5.2.1/src/liblzma/lzma/lzma_encoder_optimum_normal.c",
        "xz-5.2.1/src/liblzma/lzma/lzma_encoder_presets.c",
        "xz-5.2.1/src/liblzma/lzma/lzma2_decoder.c",
        "xz-5.2.1/src/liblzma/lzma/lzma2_encoder.c",
        "xz-5.2.1/src/liblzma/rangecoder/price_table.c",
        "xz-5.2.1/src/liblzma/simple/arm.c",
        "xz-5.2.1/src/liblzma/simple/armthumb.c",
        "xz-5.2.1/src/liblzma/simple/ia64.c",
        "xz-5.2.1/src/liblzma/simple/powerpc.c",
        "xz-5.2.1/src/liblzma/simple/simple_coder.c",
        "xz-5.2.1/src/liblzma/simple/simple_decoder.c",
        "xz-5.2.1/src/liblzma/simple/simple_encoder.c",
        "xz-5.2.1/src/liblzma/simple/sparc.c",
        "xz-5.2.1/src/liblzma/simple/x86.c",
    ])
    env.Append(CPPPATH=["xz-5.2.1/src/liblzma/api"])
else:
    conf = Configure(env)
    if not conf.CheckLibWithHeader("lzma", "lzma.h", "C++"):
        liblzma = lzmaenv.Command("xz-5.2.1/src/liblzma/.libs/liblzma.a", "xz-5.2.1/configure", "cd lib/compress/xz-5.2.1 && ./configure --with-pic --enable-static && make")
        env.Append(CPPPATH=["xz-5.2.1/src/liblzma/api"])
    conf.Finish()

env.Append(CPPPATH=["../../src"])
env.Append(LIBS=[x for x in [libz, libbz2, liblzma] if x])
env.SharedLibrary("neon_compress", "compress.cpp")
