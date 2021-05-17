#!/usr/bin/env python3

import os
import subprocess
import sys

neonc = os.path.join("bin", "neonc")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--neonc":
        i += 1
        neonc = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]

subprocess.check_call([neonc, "-q", "-t", "c", fullname])
if sys.platform == "win32":
    subprocess.check_call(["cl", "/Fe{}".format(fullname.replace(".neon", "")), "/EHsc", "/Irtl/c", fullname.replace(".neon", ".c"), "rtl/c/neon.c"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
else:
    subprocess.check_call(["cc", "-std=c99", "-pedantic", "-Wall", "-Werror", "-o", fullname.replace(".neon", ""), "-Irtl/c", fullname.replace(".neon", ".c"), "rtl/c/neon.c", "-lm"])
subprocess.check_call([fullname.replace(".neon", "")] + sys.argv[i+1:])
