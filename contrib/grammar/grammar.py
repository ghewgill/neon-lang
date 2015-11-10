from pyparsing import *
import ebnf

ParserElement.enablePackrat()

singleRawString = QuotedString(quoteChar='@"', endQuoteChar='"')
doubleRawString = QuotedString(quoteChar='@@"', endQuoteChar='"@@', multiline=True)
rawString = singleRawString | doubleRawString

table = {
    # Non-greedy trick from http://pyparsing.wikispaces.com/share/view/178079
    "Identifier": ~Literal('END') + Word(alphas, alphanums + "_"),
    "Number": Word(nums, nums + "_e.") ^ ("0b" + Word("01_")) ^ ("0o" + Word("01234567_")) ^ ("0x" + Word("0123456789abcdefABCDEF_")) ^ ("0#" + Word(nums) + "#" + Word(alphanums + "_")),
    "StringLiteral": Regex(r'"(?:[^"\r\n\\]|(?:\\\((?:[^")]|"[^"]*")*\))|(?:\\.))*"') | rawString,
    "restOfLine": restOfLine,
}

parsers = ebnf.parse(open("contrib/grammar/neon.ebnf").read(), table)
