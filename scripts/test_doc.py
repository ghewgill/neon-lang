import subprocess

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
                #assert p.retcode == 0
            code = ""
        lastblank = False

files = [x for x in subprocess.check_output(["git", "ls-tree", "-r", "--name-only", "origin/gh-pages"]).split("\n") if x.endswith("md")]
for fn in files:
    print "Checking {}...".format(fn)
    check_file(subprocess.check_output(["git", "show", "origin/gh-pages:"+fn]))
