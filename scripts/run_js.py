import os
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if os.name == "posix":
    node = "node"
elif os.path.exists("c:\\program files\\nodejs\\node.exe"):
    node = "\"c:\\program files\\nodejs\\node\""
else:
    sys.exit(99)

if os.system("{} -q -t js {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
if os.system("{} {}".format(node, fullname.replace(".neon", ".js"))) != 0:
    sys.exit(1)
