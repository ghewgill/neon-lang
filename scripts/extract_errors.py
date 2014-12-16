import glob
import re
import sys

r = re.compile(r'\berror\((\d+),.*?"(.*?)"')

errors = {}
for fn in glob.glob("src/*.cpp"):
    with open(fn) as f:
        for s in f:
            if " error(" in s:
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
