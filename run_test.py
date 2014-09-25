import os
import re
import subprocess
import sys

def run(fn):
    print ("Running {}...".format(fn))

    src = open(fn).read()

    out_comments = re.findall("^%=\s*(.*)$", src, re.MULTILINE)
    expected_stdout = "".join([x + "\n" for x in out_comments])

    err_comments = re.findall("^%!\s*(.*)$", src, re.MULTILINE)
    expected_stderr = "".join([x + "\n" for x in err_comments]).strip()

    p = subprocess.Popen(["./simple", fn], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    out = out.decode()
    err = err.decode()

    if not expected_stderr:
        sys.stderr.write(err)
        assert p.returncode == 0, p.returncode

    assert expected_stderr in err, (err, expected_stderr)
    if out != expected_stdout:
        print("*** EXPECTED OUTPUT")
        print()
        sys.stdout.write(expected_stdout)
        print()
        print("*** ACTUAL OUTPUT")
        print()
        sys.stdout.write(out)
        sys.exit(1)

def main():
    for a in sys.argv[1:]:
        if os.path.isdir(a):
            for fn in sorted(os.listdir(a)):
                if fn.endswith(".simple"):
                    run(os.path.join(a, fn))
        elif os.path.isfile(a):
            run(a)

main()
