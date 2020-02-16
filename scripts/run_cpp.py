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

subprocess.check_call([neonc, "-q", "-t", "cpp", fullname])
if sys.platform == "win32":
    subprocess.check_call(["cl", "/Fe{}".format(fullname.replace(".neon", "")), "/EHsc", "/Irtl/cpp", fullname.replace(".neon", ".cpp"), "rtl/cpp/neon.cpp"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
else:
    subprocess.check_call(["c++", "-o", fullname.replace(".neon", ""), "-Irtl/cpp", fullname.replace(".neon", ".cpp"), "rtl/cpp/neon.cpp"])
subprocess.check_call([fullname.replace(".neon", "")] + sys.argv[i+1:])
