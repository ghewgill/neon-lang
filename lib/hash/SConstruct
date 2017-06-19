import os
import shutil
import subprocess
import sys
import tarfile
import zipfile

def replace(fn, t, r):
    data = open(fn).read()
    data = data.replace(t, r)
    open(fn, "w").write(data)

env = Environment()
if sys.platform == "win32":
    env.Append(CXXFLAGS=["/EHsc", "/MDd", "/W4", "/WX"])
else:
    env.Append(CXXFLAGS=["-std=c++0x"])

hashenv = Environment()
if "RELEASE" not in env or not env["RELEASE"]:
    if sys.platform == "win32":
        hashenv.Append(CXXFLAGS=[
            "/MDd",
            "/Zi",
            "/Od",
        ])

if GetOption("clean"):
    shutil.rmtree("hash-library", ignore_errors=True)
elif not os.path.exists("hash-library/sha256.cpp"):
    zipfile.ZipFile("hash-library.zip").extractall("hash-library")
    if sys.platform == "darwin":
        subprocess.check_call("perl -n -i -e 'print unless /<endian.h>/' hash-library/*.cpp", shell=True)

if sys.platform != "win32":
    hashenv.Append(CXXFLAGS=["-fPIC"])
libhash = hashenv.Library("hash-library/hash-library", [
    "hash-library/crc32.cpp",
    "hash-library/md5.cpp",
    "hash-library/sha1.cpp",
    "hash-library/sha256.cpp",
    "hash-library/sha3.cpp",
])

env.Append(CPPPATH=["hash-library"])

libssl = None

sslenv = Environment()
if "RELEASE" not in env or not env["RELEASE"]:
    if sys.platform == "win32":
        sslenv.Append(CFLAGS=[
            "/MTd",
            "/Zi",
            "/Od",
        ])

if GetOption("clean"):
    def remove_readonly(function, path, excinfo):
        os.chmod(path, stat.S_IWRITE)
        os.remove(path)
    shutil.rmtree("libressl-2.2.4", ignore_errors=True, onerror=remove_readonly)
elif not os.path.exists("libressl-2.2.4/configure"):
    tarfile.open("libressl-2.2.4.tar.gz").extractall(".")
    if sys.platform == "win32":
        replace("libressl-2.2.4/include/compat/string.h", "static inline", "static")
        replace("libressl-2.2.4/crypto/md32_common.h", "static inline", "static")

# This is just enough of libressl to build the SHA hash functions.
sslenv.Append(CPPPATH=["libressl-2.2.4/include"])
sslenv.Append(CPPPATH=["libressl-2.2.4/include/compat"])
sslenv.Append(CPPPATH=["libressl-2.2.4/crypto"])
if sys.platform == "darwin":
    # Without these, compiling on macOS gets errors in compat/string.h.
    # But we need the compat include directory for other things like
    # machine/endian.h. So turn off functions that have already been declared.
    sslenv.Append(CPPDEFINES=["HAVE_STRLCPY", "HAVE_STRLCAT"])
if sys.platform != "win32":
    sslenv.Append(CFLAGS=["-fPIC"])
libssl = sslenv.Library("libressl-2.2.4/libssl.lib", [
    "libressl-2.2.4/crypto/compat/explicit_bzero_win.c" if sys.platform == "win32" else "libressl-2.2.4/crypto/compat/explicit_bzero.c",
    "libressl-2.2.4/crypto/mem_clr.c",
    "libressl-2.2.4/crypto/sha/sha1_one.c",
    "libressl-2.2.4/crypto/sha/sha1dgst.c",
    "libressl-2.2.4/crypto/sha/sha256.c",
    "libressl-2.2.4/crypto/sha/sha512.c",
])
env.Append(CPPPATH=["libressl-2.2.4/include"])

env.Depends("hash_ressl.cpp", libssl)

env.Append(CPPPATH=["../../src"])
env.Append(LIBS=[x for x in [libhash, libssl] if x])
env.SharedLibrary("neon_hash", ["hash.cpp", "hash_ressl.cpp"])
