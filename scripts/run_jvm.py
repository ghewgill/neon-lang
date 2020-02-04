#!/usr/bin/env python3

import os
import subprocess
import sys

java = None
neonc = os.path.join("bin", "neonc")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--java":
        i += 1
        java = sys.argv[i]
        # This gathers a full path name which might contain spaces
        # because through the multiple layers of shell and ctest
        # on win32, a path with spaces tends to get broken up.
        while not os.path.exists(java) and i+1 < len(sys.argv):
            i += 1
            java = java + " " + sys.argv[i]
    elif sys.argv[i] == "--neonc":
        i += 1
        neonc = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]
path, name = os.path.split(fullname)

if java is None:
    java = "java"
    if os.name == "nt" and os.path.exists(r"c:\ProgramData\Oracle\Java\javapath\java.exe"):
        java = r"c:\ProgramData\Oracle\Java\javapath\java.exe"

subprocess.check_call([neonc, "-q", "-t", "jvm", fullname])
classpath = path if path else "."
subprocess.check_call([java, "-cp", os.pathsep.join([classpath, "rtl/jvm"]), name.replace(".neon", "")] + sys.argv[i+1:])
