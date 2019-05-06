import os
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

java = "java"
if os.name == "nt" and os.path.exists(r"c:\ProgramData\Oracle\Java\javapath\java.exe"):
    java = r"c:\ProgramData\Oracle\Java\javapath\java.exe"

if os.system("{} -q -t jvm {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
classpath = path if path else "."
if os.system("{} -cp {} {} {}".format(java, os.pathsep.join([classpath, "rtl/jvm"]), name.replace(".neon", ""), " ".join(sys.argv[2:]))) != 0:
    sys.exit(1)
