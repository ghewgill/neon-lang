#!/usr/bin/env python3

import itertools
import os
import re
import subprocess
import sys

neon = "bin/neon"
neonc = "bin/neonc"

def test(code):
    if code:
        compile_only = (re.search(r"\binput\b", code) is not None) or ("# exception" in code)
        p = subprocess.Popen([neonc if compile_only else neon, "-"], stdin=subprocess.PIPE)
        p.communicate(code.encode())
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
                test("IMPORT {}\n".format(os.path.basename(fn)[:-5]) + "\n".join(re.sub(r"^ \|\s*[>:|]", "", x) for x in doc if x.startswith(" | ")))
            except StopIteration:
                firstline = next(itertools.dropwhile(lambda x: not x[1][3:].strip(), full))
                undocumented.append("no example in {}:{} for {}".format(fn, firstline[0], firstline[1][3:].strip()))

def check_file(fn, source):
    if fn.endswith(".md"):
        if len(fn.split(os.sep)) <= 3:
            check_file_md(source)
    elif fn.endswith(".rst"):
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
            if f.endswith((".md", ".neon", ".rst")):
                fn = os.path.join(path, f)
                yield (fn, open(fn, encoding="utf8").read())

i = 1
while i < len(sys.argv):
    if sys.argv[i].startswith("-"):
        if sys.argv[i] == "--neon":
            i += 1
            neon = sys.argv[i]
        elif sys.argv[i] == "--neonc":
            i += 1
            neonc = sys.argv[i]
        else:
            print("test_doc: unknown option {}".format(sys.argv[i]))
            sys.exit(1)
    else:
        break
    i += 1

if i < len(sys.argv):
    paths = [sys.argv[i]]
else:
    paths = ["docs", "gh-pages", "lib"]

undocumented = []
for path in paths:
    if path.endswith(":"):
        files = get_branch_files(path)
    else:
        files = get_path_files(path)

    for fn, source in files:
        print("Checking {}...".format(fn))
        check_file(fn, source)

for u in undocumented:
    print("test_doc.py:", u)
