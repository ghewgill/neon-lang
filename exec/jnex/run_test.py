#!/usr/bin/env python3

import os
import subprocess
import sys

java = "java"
i = 1
if sys.argv[i] == "--java":
    java = sys.argv[i+1]
    i += 2
fullname = sys.argv[i]
path, name = os.path.split(fullname)

subprocess.check_call([java, "-enableassertions", "-cp", "exec/jnex/src", "org.neon_lang.jnex.Executor", fullname + "x"] + sys.argv[2:])
