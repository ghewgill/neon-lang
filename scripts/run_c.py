#!/usr/bin/env python3

import os
import subprocess
import sys

neon = os.path.join("bin", "neon")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--neon":
        i += 1
        neon = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]

if sys.platform == "win32":
    os.putenv("CFLAGS", "/nologo")
subprocess.check_call([neon, "tools/neonbuild.neon", fullname])
subprocess.check_call([fullname.replace(".neon", "")] + sys.argv[i+1:])
