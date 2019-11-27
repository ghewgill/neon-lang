#!/usr/bin/env python3

import os
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

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

if os.system("{} -q -t js {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
if os.system("{} {} {}".format(node, fullname.replace(".neon", ".js"), " ".join(sys.argv[2:]))) != 0:
    sys.exit(1)
