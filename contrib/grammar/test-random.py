#!/usr/bin/env python3

import random
import re
import subprocess
import sys

sys.path.append("external/pyparsing-2.4.5")
from pyparsing import *

import grammar

def random_ident():
    s = "".join(random.sample("abcdefghijklmnopqrstuvwxyz", random.randint(2, 9)))
    return s

class Generator:
    def __init__(self):
        self.tokens = []
        self.depth = 0

    def eval(self, node):
        self.depth += 1
        if hasattr(node, "name") and node.name == "Statement":
            self.tokens.append("\n" + " "*self.depth)
        if hasattr(node, "name") and node.name in ["Expression", "BracketedExpression"] and (random.uniform(0, 1) >= 0.05 or self.depth > 5):
            self.tokens.append(random_ident())
        elif node is grammar.table["Number"]:
            self.tokens.append("12345")
        elif node is grammar.table["Identifier"]:
            self.tokens.append(random_ident())
        elif node is grammar.table["StringLiteral"]:
            self.tokens.append('"foo"')
        elif node is grammar.table["ExecBody"]:
            self.tokens.append("select * from table;")
        elif isinstance(node, And):
            for e in node.exprs:
                self.eval(e)
        elif isinstance(node, Forward):
            self.eval(node.expr)
        elif isinstance(node, Group):
            self.eval(node.expr)
        elif isinstance(node, (Keyword, Literal)):
            self.tokens.append(node.match)
        elif isinstance(node, MatchFirst):
            self.eval(random.choice(node.exprs))
        elif isinstance(node, NotAny):
            pass
        elif isinstance(node, Optional):
            if random.choice([False, True]):
                self.eval(node.expr)
        elif isinstance(node, Or):
            self.eval(random.choice(node.exprs))
        elif isinstance(node, QuotedString):
            self.tokens.append('"foo"')
        elif isinstance(node, ZeroOrMore):
            while random.choice([False, True]):
                self.eval(node.expr)
        else:
            assert False, type(node)
        self.depth -= 1

neonc = "bin/neonc"

i = 1
while i < len(sys.argv):
    if sys.argv[i].startswith("-"):
        if sys.argv[i] == "--neonc":
            i += 1
            neonc = sys.argv[i]
        else:
            print("test-random: unknown option {}".format(sys.argv[i]))
            sys.exit(1)
    else:
        break
    i += 1

sys.setrecursionlimit(5000)
for _ in range(100):
    g = Generator()
    g.eval(grammar.parsers["Statement"])
    source = " ".join(map(str, g.tokens))
    #print(source)
    p = subprocess.Popen([neonc, "-"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate(source.encode())
    out = out.decode()
    err = err.decode()
    r = p.wait()
    if r < 0 or "Error N1" in err or "Error N2" in err:
        with open("tmp/random.out", "w") as f:
            f.write(source)
        print(err)
        print("Randomly generated source saved in tmp/random.out")
        sys.exit(1)
