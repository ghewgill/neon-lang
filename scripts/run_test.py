import codecs
import os
import re
import subprocess
import sys

class TestSkipped: pass

errors = None

def run(fn):
    print ("Running {}...".format(fn))

    try:
        src = codecs.open(fn, encoding="UTF-8").read().replace("\r\n", "\n")
    except UnicodeDecodeError:
        # One test has invalid UTF-8 data, so read it as the default encoding.
        src = open(fn).read().replace("\r\n", "\n")

    all_comments = re.findall("^%(.*)$", src, re.MULTILINE)
    todo = any("TODO" in x for x in all_comments)
    if any("SKIP" in x for x in all_comments):
        print("skipped")
        raise TestSkipped()
    args = []
    a = [x for x in all_comments if "ARGS" in x]
    if a:
        args = a[0][a[0].index("ARGS")+4:].split()

    errnum = None
    expected_stdout = ""
    expected_stderr = ""
    expected_error_pos = {}

    if errors:
        for i, s in enumerate(src.split("\n"), 1):
            m = re.match(r"(\s*)%<(\d)?", s)
            if m is not None:
                n = int(m.group(2)) if m.group(2) else 1
                expected_error_pos[n] = (i - 1, 1 + len(m.group(1)))
        errnum = os.path.splitext(os.path.basename(fn))[0]
        expected_stderr = errnum
        del errors[errnum]
    else:
        out_comments = re.findall("^%=\s*(.*)$", src, re.MULTILINE)
        expected_stdout = "".join([x + "\n" for x in out_comments])

        err_comments = re.findall("^%!\s*(.*)$", src, re.MULTILINE)
        expected_stderr = "".join([x + "\n" for x in err_comments]).strip()

    p = subprocess.Popen(["bin/neon", fn] + args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    out = out.decode().replace("\r\n", "\n")
    try:
        err = err.decode().replace("\r\n", "\n")
    except UnicodeDecodeError:
        err = repr(err)

    if not expected_stderr:
        sys.stderr.write(err)
        if p.returncode != 0:
            if todo:
                return False
            assert p.returncode == 0, p.returncode

    if expected_stderr:
        if expected_stderr not in err:
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

    if out != expected_stdout:
        print("*** EXPECTED OUTPUT")
        print("")
        sys.stdout.write(expected_stdout)
        print("")
        print("*** ACTUAL OUTPUT")
        print("")
        sys.stdout.write(out)
        if todo:
            return False
        sys.exit(1)

    if todo:
        sys.stderr.write("TODO comment exists, but test succeeded\n")
        sys.exit(1)

    return True

def main():
    global errors
    for a in sys.argv[1:]:
        if a == "--errors":
            errors = dict(x.strip().split(" ", 1) for x in open("src/errors.txt"))

    total = 0
    succeeded = 0
    failed = 0
    skipped = 0
    for a in sys.argv[1:]:
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
        print "Untested errors:"
        for number, message in sorted(errors.items()):
            print number, message
        sys.exit(1)

main()
