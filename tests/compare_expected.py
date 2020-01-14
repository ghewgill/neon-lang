#!/usr/bin/env python3

import difflib
import subprocess
import sys

expected_name = sys.argv[1]
args = sys.argv[2:]

out = subprocess.check_output(args, universal_newlines=True)
expected = open(expected_name).read()
diff = difflib.context_diff(expected.splitlines(keepends=True), out.splitlines(keepends=True), fromfile="expected", tofile="actual")
print(diff)
if diff:
    sys.stdout.writelines(diff)
assert out == expected

