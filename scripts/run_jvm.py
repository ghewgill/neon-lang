#!/usr/bin/env python3

import os
import sys

java = None
neonc = os.path.join("bin", "neonc")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--java":
        i += 1
        java = sys.argv[i]
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

if os.system("{} -q -t jvm {}".format(neonc, fullname)) != 0:
    sys.exit(1)
classpath = path if path else "."
if os.system("{} -cp {} {} {}".format(java, os.pathsep.join([classpath, "rtl/jvm"]), name.replace(".neon", ""), " ".join(sys.argv[i+1:]))) != 0:
    sys.exit(1)
