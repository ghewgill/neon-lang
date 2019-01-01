import os
import subprocess
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call([sys.executable, "exec/pynex/pynex.py", fullname + "x"])
