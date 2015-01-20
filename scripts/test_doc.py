import os
import subprocess
import sys

def check_file(source):
    lines = source.split("\n")
    code = ""
    lastblank = True
    for s in lines:
        if not s:
            lastblank = True
            continue
        if s.startswith("    ") and (code or lastblank):
            code += s[4:] + "\n"
        else:
            if code:
                p = subprocess.Popen(["bin/neonc", "-"], stdin=subprocess.PIPE)
                p.communicate(code)
                p.wait()
                assert p.returncode == 0
            code = ""
        lastblank = False

def get_branch_files(prefix):
    try:
        files = [x for x in subprocess.check_output(["git", "ls-tree", "-r", "--name-only", "origin/gh-pages"]).split("\n") if x.endswith("md")]
    except (subprocess.CalledProcessError, OSError):
        # no git, just exit
        sys.exit(0)
    for fn in files:
        yield (fn, subprocess.check_output(["git", "show", "origin/gh-pages:"+fn]))

def get_path_files(prefix):
    for path, dirs, files in os.walk(prefix):
        for f in files:
            if f.endswith(".md"):
                fn = os.path.join(path, f)
                yield (fn, open(fn).read())

if len(sys.argv) < 2:
    path = "origin/gh-pages:"
else:
    path = sys.argv[1]

if path.endswith(":"):
    files = get_branch_files(path)
else:
    files = get_path_files(path)

for fn, source in files:
    print "Checking {}...".format(fn)
    check_file(source)
