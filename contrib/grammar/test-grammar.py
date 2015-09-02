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

blockComment = "%|" + ZeroOrMore(CharsNotIn("|") | ("|" + ~Literal("%"))) + "|%"
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
for fn in sys.argv[1:]:
    fn = fn.replace("\\", "/")
    print(fn)
    if "%!" in open(fn).read():
        print("skipped, failure")
        if fn in KnownParseFailures:
            print "Unneeded known failure:", fn
        continue
    try:
        parser.parseFile(fn, parseAll=True)
        if fn in KnownParseFailures:
            print "Incorrect known failure:", fn
    except ParseException, e:
        if fn in KnownParseFailures:
            print "Known failure:", e
        else:
            raise
