#!/usr/bin/env python3

import os
import subprocess
import sys

pynex = "exec/pynex/pynex.py"

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--pynex":
        i += 1
        pynex = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]
path, name = os.path.split(fullname)

subprocess.check_call([sys.executable, pynex, fullname + "x"] + sys.argv[2:])
