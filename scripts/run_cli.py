import os
import sys

ExcludeTests = [
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if name != "hello.neon":
    sys.exit(99)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

if os.name == "posix":
    node = "node"
else:
    node = "\"c:\\program files\\nodejs\\node\""

if os.system("{} -q -t cli {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
if os.system(fullname.replace(".neon", "")) != 0:
    sys.exit(1)
