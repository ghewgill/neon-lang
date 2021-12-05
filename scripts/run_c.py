#!/usr/bin/env python3

import os
import subprocess
import sys

neon = os.path.join("bin", "neon")
neonc = os.path.join("bin", "neonc")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--neon":
        i += 1
        neon = sys.argv[i]
    elif sys.argv[i] == "--neonc":
        i += 1
        neonc = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]

os.putenv("NEONC", neonc)
subprocess.check_call([neon, "tools/neonbuild.neon", fullname])
subprocess.check_call([os.path.abspath(fullname.replace(".neon", ""))] + sys.argv[i+1:])
