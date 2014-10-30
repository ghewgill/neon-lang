import codecs
import os
import re
import subprocess
import sys

def run(fn):
    print ("Running {}...".format(fn))

    src = codecs.open(fn, encoding="UTF-8").read()

    all_comments = re.findall("^%(.*)$", src, re.MULTILINE)
    todo = any("TODO" in x for x in all_comments)
    if any("SKIP" in x for x in all_comments):
        print("skipped")
        return True

    out_comments = re.findall("^%=\s*(.*)$", src, re.MULTILINE)
    expected_stdout = "".join([x + "\n" for x in out_comments])

    err_comments = re.findall("^%!\s*(.*)$", src, re.MULTILINE)
    expected_stderr = "".join([x + "\n" for x in err_comments]).strip()

    p = subprocess.Popen(["./simple", fn], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    out = out.decode().replace("\r\n", "\n")
    err = err.decode().replace("\r\n", "\n")

    if not expected_stderr:
        sys.stderr.write(err)
        if p.returncode != 0:
            if todo:
                return False
            assert p.returncode == 0, p.returncode

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
    total = 0
    succeeded = 0
    failed = 0
    for a in sys.argv[1:]:
        if os.path.isdir(a):
            for fn in sorted(os.listdir(a)):
                if fn.endswith(".simple"):
                    total += 1
                    if run(os.path.join(a, fn)):
                        succeeded += 1
                    else:
                        failed += 1
        elif os.path.isfile(a):
            total += 1
            if run(a):
                succeeded += 1
            else:
                failed += 1
    print("{} tests, {} succeeded, {} failed".format(total, succeeded, failed))

main()
