import os
import subprocess
import sys

IncludeTests = [
    "t/arithmetic.neon",
    "t/hello.neon",
]

ExcludeTests = [
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") not in IncludeTests:
    sys.exit(99)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call(["contrib/msvc/Debug/neonvm.exe", fullname + "x"])
