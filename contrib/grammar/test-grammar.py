#!/usr/bin/env python3

import functools
import glob
import sys

from pyparsing import *

import grammar

KnownParseFailures = [          # Reason:
    "t/assign2.neon",           #
    "t/comments.neon",          # nested block comments
    "t/lexer-raw.neon",         # unicode
    "t/lexer-unicode.neon",     # unicode
    "t/os-test.neon",           # double-at raw string with embedded quote at end
    "t/unicode-source.neon",    # unicode
    "t/utf8-invalid.neon",      # invalid utf-8
    "t/errors/N3060.neon",      #
    "t/errors/N3061.neon",      #
]

# blockComment is similar to cStyleComment form pyparsing
blockComment = Regex(r"%\|(?:[^|]*\|+)+?%")
lineComment = "%" + restOfLine
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
        if "%!" in open(fn, encoding="utf-8").read():
            print("skipped, failure")
            if fn in KnownParseFailures:
                print("Unneeded known failure:", fn)
            continue
    except UnicodeDecodeError:
        print("skipped, encoding")
        continue
    try:
        parser.parseFile(open(fn, encoding="utf-8"), parseAll=True)
        if fn in KnownParseFailures:
            print("Incorrect known failure:", fn)
    except ParseException as e:
        if fn in KnownParseFailures:
            print("Known failure:", e)
        else:
            print("Failure parsing:", fn, file=sys.stderr)
            raise
