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
    if not expected_stderr:
        assert p.returncode == 0, err

    out = out.decode()
    err = err.decode()

    assert expected_stderr in err, (err, expected_stderr)
    assert out == expected_stdout, (out, expected_stdout)

def main():
    for a in sys.argv[1:]:
        if os.path.isdir(a):
            for fn in sorted(os.listdir(a)):
                if fn.endswith(".simple"):
                    run(os.path.join(a, fn))
        elif os.path.isreg(a):
            run(a)

main()
