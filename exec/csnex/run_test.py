#!/usr/bin/env python3

import os
import subprocess
import sys

java = "java"

i = 1

fullname = sys.argv[i]

if sys.platform == "win32":
    subprocess.check_call(["exec/csnex/csnex.exe", fullname + "x"] + sys.argv[i+1:])
else:
    subprocess.check_call(["mono", "exec/csnex/csnex.exe", fullname + "x"] + sys.argv[i+1:])
