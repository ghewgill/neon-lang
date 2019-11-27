#!/usr/bin/env python3

import os
import sys

with open("src/rtlx.inc", "w") as f:
    for fn in sys.argv[1:]:
        bytecode = open(fn, "rb").read()
        print("static const unsigned char bytecode_{}[] = {{{}}};".format(
            os.path.basename(fn).replace(".neonx", ""),
            ",".join("0x{:02x}".format(ord(x)) for x in bytecode)
        ), file=f)
    print("static struct {", file=f)
    print("    const char *name;", file=f)
    print("    size_t length;", file=f)
    print("    const unsigned char *bytecode;", file=f)
    print("} rtl_bytecode[] = {", file=f)
    for fn in sys.argv[1:]:
        modname = os.path.basename(fn).replace(".neonx", "")
        bytecode = open(fn, "rb").read()
        print("    {{\"{}\", {}, bytecode_{}}},".format(modname, len(bytecode), modname), file=f)
    print("};", file=f)
