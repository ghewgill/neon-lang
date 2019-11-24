import os
import sys

print >>sys.stderr, sys.argv
java = None
i = 1
if sys.argv[i] == "--java":
    java = sys.argv[i+1]
    i += 2
fullname = sys.argv[i]
path, name = os.path.split(fullname)

if java is None:
    java = "java"
    if os.name == "nt" and os.path.exists(r"c:\ProgramData\Oracle\Java\javapath\java.exe"):
        java = r"c:\ProgramData\Oracle\Java\javapath\java.exe"

if os.system("{} -q -t jvm {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
classpath = path if path else "."
if os.system("{} -cp {} {} {}".format(java, os.pathsep.join([classpath, "rtl/jvm"]), name.replace(".neon", ""), " ".join(sys.argv[2:]))) != 0:
    sys.exit(1)
