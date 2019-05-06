import os
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if os.name == "nt":
    mono = ""
else:
    mono = "mono"

if os.system("{} -q -t cli {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
if os.system("{} {} {}".format(mono, fullname.replace(".neon", ".exe"), " ".join(sys.argv[2:]))) != 0:
    sys.exit(1)
