import os
import subprocess
import sys

fullname = sys.argv[1]
path, name = os.path.split(fullname)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call(["java", "-enableassertions", "-cp", "exec/jnex/src", "org.neon_lang.jnex.Executor", fullname + "x"] + sys.argv[2:])
