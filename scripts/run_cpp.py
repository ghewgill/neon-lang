#!/usr/bin/env python3

import os
import subprocess
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", "-t", "cpp", fullname])
if sys.platform == "win32":
    subprocess.check_call(["cl", "/Fe{}".format(fullname.replace(".neon", "")), "/EHsc", "/Irtl/cpp", fullname.replace(".neon", ".cpp"), "rtl/cpp/neon.cpp"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
else:
    subprocess.check_call(["c++", "-o", fullname.replace(".neon", ""), "-Irtl/cpp", fullname.replace(".neon", ".cpp"), "rtl/cpp/neon.cpp"])
subprocess.check_call([fullname.replace(".neon", "")] + sys.argv[2:])
