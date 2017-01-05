import os
import sys

ExcludeTests = [
    "t/array-fraction.neon",
    "t/array-negative.neon",
    "t/array-resize.neon",
    "t/array-slice.neon",
    "t/array-subscript.neon",
    "t/array-tostring.neon",
    "t/array.neon",
    "t/array2d.neon",
    "t/assert-array-member.neon",
    "t/assert-enum.neon",
    "t/assert-fail.neon",
    "t/assert-fail2.neon",
    "t/assert.neon",
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

java = "java"
if os.name == "nt" and os.path.exists(r"c:\ProgramData\Oracle\Java\javapath\java.exe"):
    java = r"c:\ProgramData\Oracle\Java\javapath\java.exe"

if os.system("{} -q -t jvm {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
classpath = path if path else "."
if os.system("{} -cp {} {}".format(java, os.pathsep.join([classpath, "jvm"]), name.replace(".neon", ""))) != 0:
    sys.exit(1)
