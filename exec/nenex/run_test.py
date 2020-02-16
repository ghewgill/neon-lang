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

subprocess.check_call([neon, "exec/nenex/nenex.neon", fullname + "x"] + sys.argv[i+1:])
