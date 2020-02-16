#!/usr/bin/env python3

import os
import subprocess
import sys

cnex = os.path.join("exec", "cnex", "cnex")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--cnex":
        i += 1
        cnex = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]

subprocess.check_call([cnex, fullname + "x"] + sys.argv[i+1:])
