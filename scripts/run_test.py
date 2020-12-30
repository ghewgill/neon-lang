#!/usr/bin/env python3

import codecs
import difflib
import os
import re
import subprocess
import sys

class TestSkipped(BaseException): pass

errors = None
runner = ["bin/neon"]

def run(fn):
    print("Running {} {}...".format(" ".join(runner), fn))

    try:
        src = codecs.open(fn, encoding="UTF-8").read().replace("\r\n", "\n")
    except UnicodeDecodeError:
        # One test has invalid UTF-8 data, so read it as the default encoding.
        src = open(fn, encoding="latin-1").read().replace("\r\n", "\n")

    all_comments = re.findall("^--(.*)$", src, re.MULTILINE)
    todo = any("TODO" in x for x in all_comments)
    if any("SKIP" in x for x in all_comments):
        print("skipped")
        raise TestSkipped()
    platforms = [m and m.group(1) for m in [re.search(r"PLATFORM:(\w+)", x) for x in all_comments]]
    if any(platforms):
        while True:
            if "win32" in platforms and os.name == "nt":
                break
            if "posix" in platforms and os.name == "posix":
                break
            print("skipped: {}".format(",".join(x for x in platforms if x)))
            raise TestSkipped()
    args = []
    a = [x for x in all_comments if "ARGS" in x]
    if a:
        args = a[0][a[0].index("ARGS")+4:].split()

    errnum = None
    expected_stdout = ""
    regex_stdout = None
    expected_stderr = ""
    regex_stderr = None
    expected_error_pos = {}

    if errors:
        lines = src.strip().split("\n")
        for i, s in enumerate(lines, 1):
            for m in re.finditer(r"(--<)(\d)?", s):
                n = int(m.group(2)) if m.group(2) else 1
                expected_error_pos[n] = (i - 1, 1 + m.start(1))
        if lines[-1] == "--$":
            expected_error_pos[1] = (len(lines) + 1, 1)
        errnum = os.path.splitext(os.path.basename(fn))[0]
        expected_stderr = "Error " + errnum
        del errors[errnum]
    else:
        out_comments = re.findall("^(--[=?])\s(.*)$", src, re.MULTILINE)
        if any(x[0] == "--?" for x in out_comments):
            regex_stdout = "".join([(re.escape(x[1]) if x[0] == "--=" else x[1]) + "\n" for x in out_comments])
        else:
            expected_stdout = "".join([x[1] + "\n" for x in out_comments])

        err_comments = re.findall("^(--[!~])\s(.*)$", src, re.MULTILINE)
        if any(x[0] == "--~" for x in err_comments):
            regex_stderr = "".join([(re.escape(x[1]) if x[0] == "--!" else x[1]) + "\n" for x in err_comments])
        else:
            expected_stderr = "".join([x[1] + "\n" for x in err_comments])

    p = subprocess.Popen(runner + [fn] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    out = out.decode().replace("\r\n", "\n")
    try:
        err = err.decode().replace("\r\n", "\n")
    except UnicodeDecodeError:
        err = repr(err)

    if not expected_stderr and not regex_stderr:
        sys.stderr.write(err)
        if p.returncode != 0:
            if todo:
                return False
            assert p.returncode == 0, p.returncode

    if regex_stderr:
        if not re.match(regex_stderr, err):
            print("*** EXPECTED OUTPUT (regex)")
            print("")
            sys.stdout.write(regex_stderr)
            print("")
            print("*** ACTUAL OUTPUT")
            print("")
            sys.stdout.write(err)
            if todo:
                return False
            sys.exit(1)
    elif expected_stderr:
        if expected_stderr.strip() not in err:
            print("*** EXPECTED ERROR")
            print("")
            sys.stdout.write(expected_stderr)
            print("")
            print("*** ACTUAL ERROR")
            print("")
            sys.stdout.write(err)
            if todo:
                return False
            sys.exit(1)
        if expected_error_pos:
            found = 0
            for m in re.finditer(r"Error N\d+: (\d+):(\d+)", err):
                actual_line = int(m.group(1))
                actual_col = int(m.group(2))
                if (actual_line, actual_col) != expected_error_pos[1+found]:
                    print("")
                    print("*** WRONG ERROR LOCATION (expected {})".format(expected_error_pos[1+found]))
                    print("")
                    sys.stdout.write(err)
                    if todo:
                        return False
                    sys.exit(1)
                found += 1
            assert found == len(expected_error_pos), (found, len(expected_error_pos))
        elif errnum and errnum > "N2000":
            print("Need error location information for {}".format(errnum))
            if todo:
                return False
            sys.exit(1)

    if regex_stdout:
        if not re.match(regex_stdout, out):
            print("*** EXPECTED OUTPUT (regex)")
            print("")
            sys.stdout.write(regex_stdout)
            print("")
            print("*** ACTUAL OUTPUT")
            print("")
            sys.stdout.write(out)
            if todo:
                return False
            sys.exit(1)
    elif out != expected_stdout:
        print("*** EXPECTED OUTPUT")
        print("")
        sys.stdout.write(expected_stdout)
        print("")
        print("*** ACTUAL OUTPUT")
        print("")
        sys.stdout.write(out)
        print("")
        print("*** DIFF")
        print("")
        print("\n".join(x.rstrip() for x in difflib.unified_diff(expected_stdout.split("\n"), out.split("\n"))))
        if todo:
            return False
        sys.exit(1)

    if todo:
        sys.stderr.write("TODO comment exists, but test succeeded\n")
        sys.exit(1)

    return True

def main():
    global errors
    global runner

    i = 1
    while i < len(sys.argv):
        a = sys.argv[i]
        if a == "--errors":
            errors = dict(x.strip().split(" ", 1) for x in open("gen/errors.txt"))
        elif a == "--runner":
            i += 1
            runner = sys.argv[i].split()
        else:
            break
        i += 1

    total = 0
    succeeded = 0
    failed = 0
    skipped = 0
    for a in sys.argv[i:]:
        if os.path.isdir(a):
            for fn in sorted(os.listdir(a)):
                if fn.endswith(".neon"):
                    total += 1
                    try:
                        if run(os.path.join(a, fn)):
                            succeeded += 1
                        else:
                            failed += 1
                    except TestSkipped:
                        skipped += 1
        elif os.path.isfile(a):
            total += 1
            try:
                if run(a):
                    succeeded += 1
                else:
                    failed += 1
            except TestSkipped:
                skipped += 1
    print("{} tests, {} succeeded, {} failed, {} skipped".format(total, succeeded, failed, skipped))
    if errors:
        print("Untested errors:")
        for number, message in sorted(errors.items()):
            print(number, message)
        sys.exit(1)

main()
