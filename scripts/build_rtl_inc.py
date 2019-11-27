#!/usr/bin/env python3

import sys

with open("src/rtl.inc", "w") as f:
    print("static struct {", file=f)
    print("    const char *name;", file=f)
    print("    const char *source;", file=f)
    print("} rtl_sources[] = {", file=f)
    for fn in sys.argv[1:]:
        print("    {{\"{}\",".format(fn.replace(".neon", "")), file=f)
        f.write("        \"" + "\\n\"\n        \"".join(x.replace("\\", "\\\\").replace('"', '\\"') for x in open(fn).read().split("\n")))
        print("\"},", file=f)
    print("};", file=f)
