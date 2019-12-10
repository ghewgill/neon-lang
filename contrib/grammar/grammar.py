#!/usr/bin/env python3

import sys

sys.path.append("external/pyparsing-2.4.5")
from pyparsing import *
import ebnf

ParserElement.enablePackrat()

singleRawString = QuotedString(quoteChar='@"', endQuoteChar='"')
doubleRawString = QuotedString(quoteChar='@@"', endQuoteChar='"@@', multiline=True)
rawString = singleRawString | doubleRawString

table = {
    # Non-greedy trick from http://pyparsing.wikispaces.com/share/view/178079
    "Identifier": ~Literal('END') + Word(alphas, alphanums + "_"),
    "Number": Word(nums, nums + "_e.") ^ ("0b" + Word("01_")) ^ ("0o" + Word("01234567_")) ^ ("0x" + Word("0123456789abcdefABCDEF_")),
    "StringLiteral": Regex(r'"(?:[^"\r\n\\]|(?:\\\((?:[^")]|"[^"]*")*\))|(?:\\.))*"') | rawString,
    "restOfLine": restOfLine,
    "ExecBody": Regex(r".*?;"),
}

parsers = ebnf.parse(open("contrib/grammar/neon.ebnf").read(), table)
