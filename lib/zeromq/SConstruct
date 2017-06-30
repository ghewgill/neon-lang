import os
import shutil
import sys
import tarfile

env = Environment()
if sys.platform == "win32":
    env.Append(CXXFLAGS=["/EHsc", "/MTd", "/W4", "/WX"])
else:
    env.Append(CXXFLAGS=["-std=c++0x"])

libzeromq = None

libzeromqenv = Environment()

if GetOption("clean"):
    if sys.platform == "win32":
        shutil.rmtree("libzmq", ignore_errors=True)
    else:
        shutil.rmtree("zeromq-4.2.0", ignore_errors=True)
elif sys.platform == "win32":
    libzeromqenv["ENV"]["PROGRAMFILES(X86)"] = os.getenv("PROGRAMFILES(X86)")
    if not os.path.exists("libzmq/builds/msvc/build/buildall.bat"):
        tarfile.open("zeromq-4.2.0.tar.gz").extractall(".")
        os.rename("zeromq-4.2.0", "libzmq")
        buildall = open("libzmq/builds/msvc/build/buildall.bat").read()
        buildall = buildall.replace("PAUSE", "rem PAUSE")
        open("libzmq/builds/msvc/build/buildall.bat", "w").write(buildall)
else:
    if not os.path.exists("zeromq-4.2.0/configure"):
        tarfile.open("zeromq-4.2.0.tar.gz").extractall(".")

if sys.platform == "win32":
    libzeromq = libzeromqenv.Command("libzmq/bin/x64/Debug/v120/static/libzmq.lib", "libzmq/builds/msvc/build/buildall.bat", "cd lib/zeromq/libzmq/builds/msvc/build && buildall.bat")
    env.Append(CPPPATH=["libzmq/include"])
    env.Append(CPPFLAGS=["-DZMQ_STATIC"])
    env.Append(LIBS=["iphlpapi"])
else:
    conf = Configure(env)
    if not conf.CheckLibWithHeader("zmq", "zmq.h", "C++"):
        libzeromq = libzeromqenv.Command("zeromq-4.2.0/src/.libs/libzmq.a", "zeromq-4.2.0/configure", "cd lib/zeromq/zeromq-4.2.0 && ./configure --enable-static --with-pic=yes && make")
        env.Append(CPPPATH=["zeromq-4.2.0/include"])
    conf.Finish()

env.Append(CPPPATH="../../src")
env.Append(LIBS=[x for x in [libzeromq] if x])
if sys.platform == "win32":
    env.Append(LIBS=["advapi32", "wsock32", "ws2_32"])
env.SharedLibrary("neon_zeromq", "zeromq.cpp")
