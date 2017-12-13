import os
import shutil
import sys
import tarfile

env = Environment()

easysidenv = Environment()

easysidenv["ENV"]["PROCESSOR_ARCHITECTURE"] = os.getenv("PROCESSOR_ARCHITECTURE")
easysidenv["ENV"]["PROCESSOR_ARCHITEW6432"] = os.getenv("PROCESSOR_ARCHITEW6432")

if GetOption("clean"):
    shutil.rmtree("easysid-version-1.0", ignore_errors=True)
elif not os.path.exists("easysid-version-1.0/SConstruct"):
    tarfile.open("easysid-version-1.0.tar.gz").extractall(".")
libeasysid = easysidenv.Command("easysid-version-1.0/libeasysid"+easysidenv["SHLIBSUFFIX"], "easysid-version-1.0/SConstruct", "cd lib/easysid/easysid-version-1.0 && " + sys.executable + " " + sys.argv[0])

# No interface library yet.
#env.SharedLibrary("neon_easysid", "easysid.cpp")
