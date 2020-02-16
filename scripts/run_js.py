#!/usr/bin/env python3

import os
import sys

neonc = os.path.join("bin", "neonc")

i = 1
while i < len(sys.argv):
    if sys.argv[i] == "--neonc":
        i += 1
        neonc = sys.argv[i]
    else:
        break
    i += 1

fullname = sys.argv[i]

if os.name == "posix":
    if os.system("nodejs -e ''") == 0:
        node = "nodejs"
    elif os.system("node -e ''") == 0:
        node = "node"
    else:
        print("nodejs or node not found")
        sys.exit(1)
elif os.path.exists("c:\\program files\\nodejs\\node.exe"):
    node = "\"c:\\program files\\nodejs\\node\""
else:
    node = "node"

if os.system("{} -q -t js {}".format(neonc, fullname)) != 0:
    sys.exit(1)
if os.system("{} {} {}".format(node, fullname.replace(".neon", ".js"), " ".join(sys.argv[i+1:]))) != 0:
    sys.exit(1)
