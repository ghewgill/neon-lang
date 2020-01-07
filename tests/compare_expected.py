#!/usr/bin/env python3

import subprocess
import sys

expected_name = sys.argv[1]
args = sys.argv[2:]

out = subprocess.check_output(args, universal_newlines=True)
expected = open(expected_name).read()
assert out == expected
