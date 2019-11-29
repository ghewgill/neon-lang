#!/usr/bin/env python3

import os
import subprocess
import sys

java = None
i = 1
if sys.argv[i] == "--java":
    java = sys.argv[i+1]
    i += 2
fullname = sys.argv[i]
path, name = os.path.split(fullname)

if java is None:
    java = "java"
    if os.name == "nt" and os.path.exists(r"c:\ProgramData\Oracle\Java\javapath\java.exe"):
        java = r"c:\ProgramData\Oracle\Java\javapath\java.exe"

subprocess.check_call([os.path.join("bin", "neonc"), "-q", "-t", "jvm", fullname])
classpath = path if path else "."
subprocess.check_call([java, "-cp", os.pathsep.join([classpath, "rtl/jvm"]), name.replace(".neon", ""), " ".join(sys.argv[2:])])
