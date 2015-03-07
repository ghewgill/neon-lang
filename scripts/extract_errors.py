import glob
import re
import sys

r = re.compile(r'\berror\d?\((\d+),.*?"(.*?)"')

errors = {}
for fn in glob.glob("src/*.cpp"):
    with open(fn) as f:
        enable = True
        for s in f:
            if s.startswith("#if 0"):
                enable = False
            elif s.startswith("#endif"):
                enable = True
            elif enable and (" error(" in s or " error2(" in s):
                m = r.search(s)
                assert m is not None, s
                number = int(m.group(1))
                message = m.group(2)
                if number in errors:
                    print >>sys.stderr, "Duplicate error number:", (number)
                    sys.exit(1)
                errors[number] = message

with open("src/errors.txt", "w") as f:
    for number, message in sorted(errors.items()):
        print >>f, "N{} {}".format(number, message)
