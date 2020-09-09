#!/usr/bin/env python3

import functools
import glob
import os
import sys

sys.path.append("external/pyparsing-2.4.5")
from pyparsing import *

import grammar

KnownParseFailures = [          # Reason:
    "assign2.neon",             #
    "comments.neon",            # nested block comments
    "lexer-raw.neon",           # unicode
    "lexer-unicode.neon",       # unicode
    "os-test.neon",             # double-at raw string with embedded quote at end
    "unicode-source.neon",      # unicode
    "utf8-invalid.neon",        # invalid utf-8
    "N3060.neon",               #
    "N3061.neon",               #
]

# blockComment is similar to cStyleComment form pyparsing
blockComment = Regex(r"\/\*(?:[^*]*\*+)+?/")
lineComment = "--" + restOfLine
comments = blockComment | lineComment

#grammar.parsers["Program"].setDebug()
#grammar.parsers["Statement"].setDebug()
#grammar.parsers["EnumType"].setDebug()
#grammar.parsers["Type"].setDebug()
#grammar.parsers["ForeachStatement"].setDebug()
#ParserElement.verbose_stacktrace = True

parser = grammar.parsers["Program"]
parser.ignore(comments)
for fn in functools.reduce(lambda x, y: x + y, [glob.glob(x) for x in sys.argv[1:]]):
    fn = fn.replace("\\", "/")
    print(fn)
    print(fn, file=sys.stderr)
    try:
        if "#!" in open(fn, encoding="utf-8").read():
            print("skipped, failure")
            if os.path.basename(fn) in KnownParseFailures:
                print("Unneeded known failure:", fn)
            continue
    except UnicodeDecodeError:
        print("skipped, encoding")
        continue
    try:
        parser.parseFile(open(fn, encoding="utf-8"), parseAll=True)
        if os.path.basename(fn) in KnownParseFailures:
            print("Incorrect known failure:", fn)
    except ParseException as e:
        if os.path.basename(fn) in KnownParseFailures:
            print("Known failure:", e)
        else:
            print("Failure parsing:", fn, file=sys.stderr)
            raise
