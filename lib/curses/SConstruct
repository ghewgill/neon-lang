from __future__ import print_function

import os
import shutil
import sys
import tarfile

env = Environment()
if sys.platform == "win32":
    env.Append(CXXFLAGS=["/EHsc", "/W4"])
else:
    env.Append(CXXFLAGS=["-std=c++0x"])

cursesenv = Environment()

if sys.platform == "win32":
    if GetOption("clean"):
        shutil.rmtree("PDCurses-3.4", ignore_errors=True)
    elif not os.path.exists("PDCurses-3.4/win32/vcwin32.mak"):
        tarfile.open("PDCurses-3.4.tar.gz").extractall(".")
        #if not env["RELEASE"]:
        #    makefile = open("PDCurses-3.4/win32/vcwin32.mak").read()
        #    makefile = makefile.replace("= -Z7 -DPDCDEBUG", "= -Z7 -DPDCDEBUG -MTd -Od -Zi")
        #    open("PDCurses-3.4/win32/vcwin32.mak", "w").write(makefile)

    libs_curses = [cursesenv.Command("PDCurses-3.4/win32/pdcurses.lib", "PDCurses-3.4/win32/vcwin32.mak", "cd lib/curses/PDCurses-3.4/win32 && nmake -fvcwin32.mak WIDE=Y UTF8=Y DEBUG=Y")]
    libs_curses.extend(["advapi32", "user32"])
    env.Append(CPPPATH=["PDCurses-3.4"])
    env.Append(LIBS=libs_curses)
else:
    conf = Configure(env)
    if not conf.CheckLibWithHeader("ncurses", "curses.h", "C++"):
        print("Could not find ncurses library", file=sys.stderr)
        Exit(1)
    conf.Finish()

env.Append(CPPPATH=["../../src"])
env.SharedLibrary("neon_curses", "curses.cpp")
