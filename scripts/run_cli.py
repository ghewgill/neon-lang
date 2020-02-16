#!/usr/bin/env python3

import os
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

if os.name == "nt":
    mono = ""
else:
    mono = "mono"

if os.system("{} -q -t cli {}".format(neonc, fullname)) != 0:
    sys.exit(1)
if os.system("{} {} {}".format(mono, fullname.replace(".neon", ".exe"), " ".join(sys.argv[i+1:]))) != 0:
    sys.exit(1)
