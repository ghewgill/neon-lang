#!/usr/bin/env python3

import os
import subprocess
import sys

java = "java"

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--java":
        i += 1
        java = sys.argv[i]
        # This gathers a full path name which might contain spaces
        # because through the multiple layers of shell and ctest
        # on win32, a path with spaces tends to get broken up.
        while not os.path.exists(java) and i+1 < len(sys.argv):
            i += 1
            java = java + " " + sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]

subprocess.check_call([java, "-enableassertions", "-cp", "exec/jnex/src", "org.neon_lang.jnex.Executor", fullname + "x"] + sys.argv[i+1:])
