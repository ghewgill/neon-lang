#!/usr/bin/env python3

import os
import subprocess

with open("t/missing.neon", "w") as f:
    print("EXPORT FUNCTION f() END FUNCTION", file=f)
subprocess.check_call(["bin/neonc", "t/import-optional-missing.neon"])
os.unlink("t/missing.neon")
os.unlink("t/missing.neonx")
subprocess.check_call(["bin/neonx", "t/import-optional-missing.neonx"])
