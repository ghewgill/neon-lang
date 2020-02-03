#!/usr/bin/env python3

import os
import subprocess
import sys

neonc = sys.argv[1]
executor = sys.argv[2:]

out = subprocess.check_output([neonc, "-o", "tmp/import-optional-missing.neonx", "t/import-optional-missing.neon"], env={"NEONPATH": "t/compile-time-only"}, stderr=subprocess.STDOUT, universal_newlines=True)
sys.stdout.write(out)
if "not found" in out:
    print("{}: Failed: expected compile step to find module2.neon".format(sys.argv[0]), file=sys.stderr)
    sys.exit(1)
subprocess.check_call(executor + ["tmp/import-optional-missing.neonx"])
