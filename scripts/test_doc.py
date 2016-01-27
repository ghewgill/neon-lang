import itertools
import os
import re
import subprocess
import sys

def test(code):
    if code:
        compile_only = re.search(r"\binput\b", code) is not None
        p = subprocess.Popen(["bin/neonc" if compile_only else "bin/neon", "-"], stdin=subprocess.PIPE)
        p.communicate(code)
        p.wait()
        assert p.returncode == 0

def check_file_md(source):
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
            test(code)
            code = ""
        lastblank = False
    test(code)

def check_file_neon(fn, source):
    lines = enumerate(source.split("\n"), 1)
    for incomment, doc in itertools.groupby(lines, lambda line: len(line[1]) >= 2 and line[1][1] == "|"):
        if incomment:
            full = list(doc)
            doc = itertools.dropwhile(lambda x: "Example:" not in x, [x[1] for x in full])
            try:
                next(doc)
                test("IMPORT {}\n".format(os.path.basename(fn)[:-5]) + "\n".join(x[3:] for x in doc))
            except StopIteration:
                firstline = next(itertools.dropwhile(lambda x: not x[1][3:].strip(), full))
                undocumented.append("no example in {}:{} for {}".format(fn, firstline[0], firstline[1][3:].strip()))

def check_file(fn, source):
    if fn.endswith(".md"):
        check_file_md(source)
    elif fn.endswith(".neon"):
        check_file_neon(fn, source)
    else:
        assert False, fn

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
            if f.endswith((".md", ".neon")):
                fn = os.path.join(path, f)
                yield (fn, open(fn).read())

if len(sys.argv) < 2:
    paths = ["gh-pages", "lib"]
else:
    paths = [sys.argv[1]]

undocumented = []
for path in paths:
    if path.endswith(":"):
        files = get_branch_files(path)
    else:
        files = get_path_files(path)

    for fn, source in files:
        print "Checking {}...".format(fn)
        check_file(fn, source)

for u in undocumented:
    print "test_doc.py:", u
