import os
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if os.system("bin/neonc -q -t jvm {}".format(fullname)) != 0:
    sys.exit(1)
classpath = path if path else "."
if os.system("java -cp {} {}".format(os.pathsep.join([classpath, "jvm"]), name.replace(".neon", ""))) != 0:
    sys.exit(1)
