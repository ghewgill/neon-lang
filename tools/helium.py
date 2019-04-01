from __future__ import division, print_function

import codecs
import copy
import math
import os
import random
import re
import shutil
import sys
import unicodedata

class Symbol:
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return "<Symbol:{}>".format(self.name)

class Keyword:
    keywords = {}

    def __init__(self, name):
        self.name = name
        Keyword.keywords[name] = self
    def __repr__(self):
        return "<Keyword:{}>".format(self.name)

class Number:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return "<Number:{}>".format(self.value)

class String:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return "<String:{}>".format(self.value)

class Identifier:
    def __init__(self, name):
        self.name = name
    def __repr__(self):
        return "<Identifier:{}>".format(self.name)

NONE = Symbol("NONE")
END_OF_FILE = Symbol("END_OF_FILE")
LPAREN = Symbol("LPAREN")
RPAREN = Symbol("RPAREN")
LBRACKET = Symbol("LBRACKET")
RBRACKET = Symbol("RBRACKET")
LBRACE = Symbol("LBRACE")
RBRACE = Symbol("RBRACE")
COLON = Symbol("COLON")
ASSIGN = Symbol("ASSIGN")
PLUS = Symbol("PLUS")
MINUS = Symbol("MINUS")
TIMES = Symbol("TIMES")
DIVIDE = Symbol("DIVIDE")
MOD = Keyword("MOD")
EXP = Symbol("EXP")
CONCAT = Symbol("CONCAT")
EQUAL = Symbol("EQUAL")
NOTEQUAL = Symbol("NOTEQUAL")
LESS = Symbol("LESS")
GREATER = Symbol("GREATER")
LESSEQ = Symbol("LESSEQ")
GREATEREQ = Symbol("GREATEREQ")
COMMA = Symbol("COMMA")
IF = Keyword("IF")
THEN = Keyword("THEN")
ELSE = Keyword("ELSE")
END = Keyword("END")
WHILE = Keyword("WHILE")
DO = Keyword("DO")
VAR = Keyword("VAR")
FUNCTION = Keyword("FUNCTION")
RETURN = Keyword("RETURN")
FALSE = Keyword("FALSE")
TRUE = Keyword("TRUE")
AND = Keyword("AND")
OR = Keyword("OR")
NOT = Keyword("NOT")
FOR = Keyword("FOR")
TO = Keyword("TO")
STEP = Keyword("STEP")
ARRAY = Keyword("Array")
DICTIONARY = Keyword("Dictionary")
DOT = Symbol("DOT")
TYPE = Keyword("TYPE")
RECORD = Keyword("RECORD")
ENUM = Keyword("ENUM")
CONSTANT = Keyword("CONSTANT")
IMPORT = Keyword("IMPORT")
IN = Keyword("IN")
OUT = Keyword("OUT")
INOUT = Keyword("INOUT")
ELSIF = Keyword("ELSIF")
CASE = Keyword("CASE")
WHEN = Keyword("WHEN")
FOREIGN = Keyword("FOREIGN")
EXIT = Keyword("EXIT")
NEXT = Keyword("NEXT")
LOOP = Keyword("LOOP")
REPEAT = Keyword("REPEAT")
UNTIL = Keyword("UNTIL")
DECLARE = Keyword("DECLARE")
EXCEPTION = Keyword("EXCEPTION")
TRY = Keyword("TRY")
RAISE = Keyword("RAISE")
POINTER = Keyword("POINTER")
NEW = Keyword("NEW")
NIL = Keyword("NIL")
VALID = Keyword("VALID")
ARROW = Symbol("ARROW")
SUBBEGIN = Symbol("SUBBEGIN")
SUBFMT = Symbol("SUBFMT")
SUBEND = Symbol("SUBEND")
LET = Keyword("LET")
FIRST = Keyword("FIRST")
LAST = Keyword("LAST")
AS = Keyword("AS")
DEFAULT = Keyword("DEFAULT")
EXPORT = Keyword("EXPORT")
PRIVATE = Keyword("PRIVATE")
NATIVE = Keyword("NATIVE")
FOREACH = Keyword("FOREACH")
INDEX = Keyword("INDEX")
ASSERT = Keyword("ASSERT")
EMBED = Keyword("EMBED")
ALIAS = Keyword("ALIAS")
IS = Keyword("IS")
BEGIN = Keyword("BEGIN")
MAIN = Keyword("MAIN")
HEXBYTES = Keyword("HEXBYTES")
INC = Keyword("INC")
DEC = Keyword("DEC")
OTHERS = Keyword("OTHERS")
WITH = Keyword("WITH")
CHECK = Keyword("CHECK")
GIVES = Keyword("GIVES")
NOWHERE = Keyword("NOWHERE")
INTDIV = Keyword("INTDIV")
LABEL = Keyword("LABEL")
CLASS = Keyword("CLASS")
TRAP = Keyword("TRAP")
EXTENSION = Keyword("EXTENSION")
INTERFACE = Keyword("INTERFACE")
IMPLEMENTS = Keyword("IMPLEMENTS")
UNUSED = Keyword("UNUSED")
ISA = Keyword("ISA")

class bytes:
    def __init__(self, a):
        self.a = a
    def __getitem__(self, key):
        return bytes(self.a.__getitem__(key))

def identifier_start(c):
    return c.isalpha() or c == "_"

def identifier_body(c):
    return c.isalnum() or c == "_"

def number_start(c):
    return c.isdigit()

def number_body(c):
    return c.isdigit() or c == "." or c == "x"

def space(c):
    return c.isspace()

def tokenize_fragment(source):
    r = []
    i = 0
    while i < len(source):
        if   source[i] == "(": r.append(LPAREN); i += 1
        elif source[i] == ")": r.append(RPAREN); i += 1
        elif source[i] == "[": r.append(LBRACKET); i += 1
        elif source[i] == "]": r.append(RBRACKET); i += 1
        elif source[i] == "{": r.append(LBRACE); i += 1
        elif source[i] == "}": r.append(RBRACE); i += 1
        elif source[i] == "+": r.append(PLUS); i += 1
        elif source[i] == "*": r.append(TIMES); i += 1
        elif source[i] == "/": r.append(DIVIDE); i += 1
        elif source[i] == "^": r.append(EXP); i += 1
        elif source[i] == "&": r.append(CONCAT); i += 1
        elif source[i] == "=": r.append(EQUAL); i += 1
        elif source[i] == ",": r.append(COMMA); i += 1
        elif source[i] == ".": r.append(DOT); i += 1
        elif source[i] == "-":
            if source[i+1] == ">":
                r.append(ARROW)
                i += 2
            else:
                r.append(MINUS)
                i += 1
        elif source[i] == "<":
            if source[i+1] == "=":
                r.append(LESSEQ)
                i += 2
            elif source[i+1] == ">":
                r.append(NOTEQUAL)
                i += 2
            else:
                r.append(LESS)
                i += 1
        elif source[i] == ">":
            if source[i+1] == "=":
                r.append(GREATEREQ)
                i += 2
            else:
                r.append(GREATER)
                i += 1
        elif source[i] == ":":
            if source[i+1] == "=":
                r.append(ASSIGN)
                i += 2
            else:
                r.append(COLON)
                i += 1
        elif identifier_start(source[i]):
            start = i
            while i < len(source) and identifier_body(source[i]):
                i += 1
            text = source[start:i]
            if text in Keyword.keywords:
                r.append(Keyword.keywords[text])
            elif all(c.isupper() for c in text):
                assert False, text
            else:
                r.append(Identifier(text))
        elif number_start(source[i]):
            start = i
            while i < len(source) and number_body(source[i]):
                i += 1
            t = source[start:i]
            try:
                num = int(t, base=0)
            except ValueError:
                num = float(t)
            r.append(Number(num))
        elif source[i] == '"':
            i += 1
            string = ""
            while i < len(source):
                c = source[i]
                i += 1
                if c == '"':
                    break
                if c == "\\":
                    c = source[i]
                    i += 1
                    if   c == '"': pass
                    elif c == "\\": pass
                    elif c == "b": c = "\b"
                    elif c == "f": c = "\f"
                    elif c == "n": c = "\n"
                    elif c == "r": c = "\r"
                    elif c == "t": c = "\t"
                    elif c in ["u", "U"]:
                        if source[i] == "{":
                            close = source.find("}", i)
                            c = unicodedata.lookup(source[i+1:close])
                            i = close + 1
                        else:
                            width = 8 if c == "U" else 4
                            c = unichr(int(source[i:i+width], 16))
                            i += width
                    elif c == "(":
                        r.append(String(string))
                        r.append(SUBBEGIN)
                        start = i
                        colon = start
                        nest = 1
                        inquote = False
                        while nest > 0:
                            c = source[i]
                            i += 1
                            if   c == "(" and not inquote: nest += 1
                            elif c == ")" and not inquote: nest -= 1
                            elif c == ":" and not inquote and nest == 1: colon = i - 1
                            elif c == "\"": inquote = not inquote
                        end = i - 1
                        if colon > start:
                            end = colon
                        r.extend(tokenize_fragment(source[start:end]))
                        if colon > start:
                            r.append(SUBFMT)
                            r.append(String(source[colon+1:i-1]))
                        r.append(SUBEND)
                        string = ""
                        continue
                    else:
                        assert False, c
                string += c
            r.append(String(string))
        elif source[i] == "%":
            if i+1 < len(source) and source[i+1] == "|":
                while i < len(source) and (source[i] != "|" or source[i+1] != "%"):
                    i += 1
                i += 2
            else:
                while i < len(source) and source[i] != "\n":
                    i += 1
        elif space(source[i]):
            while i < len(source) and space(source[i]):
                i += 1
        else:
            assert False, repr(source[i])
    return r

def tokenize(source):
    r = tokenize_fragment(source)
    r.append(END_OF_FILE)
    return r

class TypeSimple:
    def __init__(self, name):
        self.name = name
    def resolve(self, env):
        return env.get_value(self.name)

class TypeParameterised:
    def __init__(self, kind, elementtype):
        self.kind = kind
        self.elementtype = elementtype
    def resolve(self, env):
        if self.kind is ARRAY: return ClassArray(self.elementtype)
        if self.kind is DICTIONARY: return ClassDictionary(self.elementtype)

class TypeCompound:
    def __init__(self, name):
        self.name = name
    def resolve(self, env):
        return g_Modules[self.name[0]].env.get_value(self.name[1])

class Field:
    def __init__(self, name, type):
        self.name = name
        self.type = type

class TypeRecord:
    def __init__(self, fields):
        self.fields = fields
        self.methods = {}
    def resolve(self, env):
        return ClassRecord(self.fields, self.methods)

class TypeEnum:
    def __init__(self, names):
        self.names = names
    def resolve(self, env):
        return ClassEnum(self.names)

class TypePointer:
    def __init__(self, type):
        self.type = type
    def resolve(self, env):
        return ClassPointer(self.type)

class TypeFunction:
    def __init__(self, returntype, args):
        self.returntype = returntype
        self.args = args
    def resolve(self, env):
        return ClassFunctionPointer(self.returntype, self.args)

class ImportDeclaration:
    def __init__(self, module, name):
        self.module = module
        self.name = name
    def declare(self, env):
        if self.name:
            assert False
        else:
            env.declare(self.module, ClassModule(), import_module(self.module))
    def run(self, env):
        pass

class TypeDeclaration:
    def __init__(self, name, type):
        self.name = name
        self.type = type
    def declare(self, env):
        type = self.type.resolve(env)
        env.declare(self.name, Class(), type)
    def run(self, env):
        pass

class ConstantDeclaration:
    def __init__(self, name, type, value):
        self.name = name
        self.type = type
        self.value = value
    def declare(self, env):
        type = self.type.resolve(env)
        env.declare(self.name, type, self.value.eval(env) if self.value else type.default(env))
    def run(self, env):
        pass

class VariableDeclaration:
    def __init__(self, names, type, expr):
        self.names = names
        self.type = type
        self.expr = expr
    def declare(self, env):
        type = self.type.resolve(env)
        for name in self.names:
            env.declare(name, type, type.default(env))
    def run(self, env):
        type = self.type.resolve(env)
        for name in self.names:
            if self.expr:
                env.set(name, self.expr.eval(env))
            else:
                env.set(name, type.default(env))

class LetDeclaration:
    def __init__(self, name, type, expr):
        self.name = name
        self.type = type
        self.expr = expr
    def declare(self, env):
        type = self.type.resolve(env)
        env.declare(self.name, type, type.default(env))
    def run(self, env):
        env.set(self.name, self.expr.eval(env))

class ExceptionDeclaration:
    def __init__(self, name):
        self.name = name
    def declare(self, env):
        if len(self.name) == 1:
            env.declare(self.name[0], ClassException(), None)
        else:
            pass
    def run(self, env):
        pass

class IdentifierExpression:
    def __init__(self, name):
        self.name = name
    def eval(self, env):
        return env.get_value(self.name)
    def set(self, env, value):
        env.set(self.name, value)

class BooleanLiteralExpression:
    def __init__(self, value):
        self.value = value
    def eval(self, env):
        return self.value

class NumberLiteralExpression:
    def __init__(self, value):
        self.value = value
    def eval(self, env):
        return self.value

class StringLiteralExpression:
    def __init__(self, value):
        self.value = value
    def eval(self, env):
        return self.value

class ArrayLiteralExpression:
    def __init__(self, elements):
        self.elements = elements
    def eval(self, env):
        return [x.eval(env) for x in self.elements]

class ArrayLiteralRangeExpression:
    def __init__(self, first, last, step):
        self.first = first
        self.last = last
        self.step = step
    def eval(self, env):
        r = []
        step = self.step.eval(env)
        assert step != 0
        if step < 0:
            i = self.first.eval(env)
            last = self.last.eval(env)
            while i >= last:
                r.append(i)
                i += step
        else:
            i = self.first.eval(env)
            last = self.last.eval(env)
            while i <= last:
                r.append(i)
                i += step
        return r

class DictionaryLiteralExpression:
    def __init__(self, elements):
        self.elements = elements
    def eval(self, env):
        return {k.eval(env): v.eval(env) for k, v in self.elements}

class NilLiteralExpression:
    def __init__(self):
        pass
    def eval(self, env):
        return None

class NowhereLiteralExpression:
    def __init__(self):
        pass
    def eval(self, env):
        return None

class InterpolatedStringExpression:
    def __init__(self, parts):
        self.parts = parts
    def eval(self, env):
        r = ""
        for e, f in self.parts:
            x = e.eval(env)
            s = (x if isinstance(x, (str, unicode))
                  else ("TRUE" if x else "FALSE") if isinstance(x, bool)
                  else neon_str(env, x) if isinstance(x, (int, float))
                  else "HEXBYTES \"{}\"".format(" ".join("{:02x}".format(b) for b in x.a)) if isinstance(x, bytes)
                  else "[{}]".format(", ".join(('"{}"'.format(e) if isinstance(e, (str, unicode)) else str(e)) for e in x)) if isinstance(x, list)
                  else "{{{}}}".format(", ".join(('"{}": {}'.format(k, ('"{}"'.format(v) if isinstance(v, (str, unicode)) else str(v))) for k, v in x.items()))) if isinstance(x, dict)
                  else x.toString(env, x))
            r += neon_format(env, s, f) if f else s
        return r

class NewRecordExpression:
    def __init__(self, type, value):
        self.type = type
        self.value = value
    def eval(self, env):
        if self.value:
            return self.type.resolve(env).make(env, [x[0] for x in self.value.args], [x[1].eval(env) for x in self.value.args])
        else:
            return self.type.resolve(env).make(env, [], [])

class UnaryPlusExpression:
    def __init__(self, expr):
        self.expr = expr
    def eval(self, env):
        return self.expr.eval(env)

class UnaryMinusExpression:
    def __init__(self, expr):
        self.expr = expr
    def eval(self, env):
        return -self.expr.eval(env)

class LogicalNotExpression:
    def __init__(self, expr):
        self.expr = expr
    def eval(self, env):
        return not self.expr.eval(env)

class SubscriptExpression:
    def __init__(self, expr, index):
        self.expr = expr
        self.index = index
    def eval(self, env):
        i = self.index.eval(env)
        try:
            return self.expr.eval(env)[i]
        except TypeError:
            if i != int(i):
                raise NeonException("ArrayIndexException")
            assert False
        except IndexError:
            raise NeonException("ArrayIndexException")
        except KeyError:
            raise NeonException("DictionaryIndexException")
    def set(self, env, value):
        a = self.expr.eval(env)
        i = self.index.eval(env)
        if isinstance(a, list):
            while len(a) <= i:
                a.append(None) # TODO: default()
        a[i] = value

class RangeSubscriptExpression:
    def __init__(self, expr, first, first_from_end, last, last_from_end):
        self.expr = expr
        self.first = first
        self.first_from_end = first_from_end
        self.last = last
        self.last_from_end = last_from_end
    def eval(self, env):
        f = self.first.eval(env)
        l = self.last.eval(env)
        if self.first_from_end:
            f = f if f < 0 else None
        else:
            f = max(0, f)
        if self.last_from_end:
            l = l if l < 0 else None
        else:
            l = max(0, l+1)
        return self.expr.eval(env)[f:l]
    def set(self, env, value):
        a = self.expr.eval(env)
        f = self.first.eval(env)
        l = self.last.eval(env)
        if self.first_from_end:
            f = f if f < 0 else None
        else:
            f = max(0, f)
        if self.last_from_end:
            l = l if l < 0 else None
        else:
            l = max(0, l+1)
        a[f:l] = value

class DotExpression:
    def __init__(self, expr, field):
        self.expr = expr
        self.field = field
    def eval(self, env):
        obj = self.expr.eval(env)
        return self.eval_obj(env, obj)
    def eval_obj(self, env, obj):
        if isinstance(obj, bool):
            if self.field == "toString": return lambda env, self: "TRUE" if obj else "FALSE"
        elif isinstance(obj, int):
            if self.field == "toString": return lambda env, self: str(obj)
        elif isinstance(obj, (str, unicode)):
            if self.field == "append": return neon_string_append
            if self.field == "length": return lambda env, self: len(self)
            if self.field == "toArray": return lambda env, self: [ord(x) for x in obj]
            if self.field == "toBytes": return lambda env, self: bytes([ord(x) for x in obj.encode("utf-8")])
            if self.field == "toString": return lambda env, self: obj
        elif isinstance(obj, bytes):
            if self.field == "decodeToString": return lambda env, self: "".join(map(chr, obj.a)).decode("utf-8")
            if self.field == "size": return lambda env, self: len(obj.a)
            if self.field == "toArray": return lambda env, self: obj.a
            if self.field == "toString": return lambda env, self: "HEXBYTES \"{}\"".format(" ".join("{:02x}".format(x) for x in obj.a))
        elif isinstance(obj, list):
            if self.field == "append": return lambda env, self, x: obj.append(x)
            if self.field == "extend": return lambda env, self, x: obj.extend(x)
            if self.field == "resize": return lambda env, self, n: neon_array_resize(obj, n)
            if self.field == "size": return lambda env, self: len(obj)
            if self.field == "toBytes": return lambda env, self: bytes(obj)
            if self.field == "toString": return lambda env, self: "[{}]".format(", ".join(('"{}"'.format(e) if isinstance(e, (str, unicode)) else str(e)) for e in obj))
        elif isinstance(obj, dict):
            if self.field == "keys": return lambda env, self: sorted(obj.keys())
            return obj[self.field] # Support a.b syntax where a is an object.
        elif isinstance(obj, Program):
            return obj.env.get_value(self.field)
        elif hasattr(obj, self.field):
            return getattr(obj, self.field)
        if self.field == "isNull":
            return lambda env, self: obj is None
        assert False, (self.field, obj)
    def set(self, env, value):
        setattr(self.expr.eval(env), self.field, value)

class ArrowExpression:
    def __init__(self, expr, field):
        self.expr = expr
        self.field = field
    def eval(self, env):
        obj = self.expr.eval(env)
        return self.eval_obj(env, obj)
    def eval_obj(self, env, obj):
        return getattr(obj, self.field)
    def set(self, env, value):
        obj = self.expr.eval(env)
        setattr(obj, self.field, value)

class AdditionExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) + self.right.eval(env)

class SubtractionExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) - self.right.eval(env)

class ConcatenationExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) + self.right.eval(env)

class AppendExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        left = self.left.eval(env)
        right = self.right.eval(env)
        if isinstance(left, (str, unicode)):
            return left + right
        elif isinstance(left, list):
            return left + [right]
        else:
            assert False

class ExponentiationExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        x = self.left.eval(env)
        y = self.right.eval(env)
        if x == int(x) and y == int(y) and y >= 0:
            x = int(x)
            y = int(y)
            r = 1
            for _ in range(y):
                r *= x
            return r
        else:
            return math.pow(x, y)

class MultiplicationExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) * self.right.eval(env)

class DivisionExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        try:
            x = self.left.eval(env)
            y = self.right.eval(env)
            if x == int(x) and y == int(y):
                x = int(x)
                y = int(y)
                if x % y == 0:
                    return x // y
                else:
                    return x / y
            else:
                return x / y
        except ZeroDivisionError:
            raise NeonException("DivideByZeroException")

class IntegerDivisionExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        try:
            return math.trunc(self.left.eval(env) / self.right.eval(env))
        except ZeroDivisionError:
            raise NeonException("DivideByZeroException")

class ModuloExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) % self.right.eval(env)

class ComparisonExpression:
    def __init__(self, left, right, cond):
        self.left = left
        self.right = right
        self.cond = cond
    def eval(self, env):
        left = self.left.eval(env)
        right = self.right.eval(env)
        return eval_cond(left, self.cond, right)

class ChainedComparisonExpression:
    def __init__(self, comps):
        self.comps = comps
    def eval(self, env):
        return all(comp.eval(env) for comp in self.comps)

class ConditionalExpression:
    def __init__(self, cond, left, right):
        self.cond = cond
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) if self.cond.eval(env) else self.right.eval(env)

class TryExpression:
    def __init__(self, expr, catches):
        self.expr = expr
        self.catches = catches
    def eval(self, env):
        try:
            return self.expr.eval(env)
        except NeonException as x:
            for exceptions, name, statements in self.catches:
                # Match either unqualified or module qualified name.
                if any(x.name[:len(h)] == h or x.name[:len(h)-1] == h[1:] for h in exceptions):
                    if isinstance(statements, list):
                        for s in statements:
                            s.declare(env)
                        for s in statements:
                            s.run(env)
                    else:
                        return statements.eval(env)
                    break
            else:
                raise

class TypeTestExpression:
    def __init__(self, left, target):
        self.left = left
        self.target = target
    def eval(self, env):
        v = self.left.eval(env)
        if isinstance(self.target, TypeSimple):
            if self.target.name == "Boolean":
                return isinstance(v, bool)
            if self.target.name == "Number":
                return isinstance(v, (int, float)) and not isinstance(v, bool)
            if self.target.name == "String":
                return isinstance(v, (str, unicode))
            if self.target.name == "Bytes":
                return isinstance(v, bytes)
            if self.target.name == "Object":
                return True
            assert False, "add type ISA support for target {}".format(self.target.name)
        if isinstance(self.target, TypeParameterised):
            if self.target.kind is ARRAY:
                if not isinstance(v, list):
                    return False
                if self.target.elementtype.name == "Number":
                    return all(isinstance(x, int) and not isinstance(x, bool) for x in v)
                if self.target.elementtype.name == "Object":
                    return True
            if self.target.kind is DICTIONARY:
                if not isinstance(v, dict):
                    return False
                if self.target.elementtype.name == "Number":
                    return all(isinstance(x, int) and not isinstance(x, bool) for x in v.values())
                if self.target.elementtype.name == "Object":
                    return True
        assert False, "add type ISA support for target {}".format(self.target)

class MembershipExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) in self.right.eval(env)

class ConjunctionExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) and self.right.eval(env)

class DisjunctionExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return self.left.eval(env) or self.right.eval(env)

class ValidPointerExpression:
    def __init__(self, tests):
        self.tests = tests
    def eval(self, env):
        return all(x[0].eval(env) for x in self.tests)

class NativeFunction:
    def __init__(self, name, returntype, args):
        self.name = name
        self.returntype = returntype
        self.args = args
    def declare(self, env):
        f = globals()["neon_{}_{}".format(env.module(), self.name)]
        outs = [x.mode is not IN for x in self.args]
        if any(outs):
            f._outs = outs
        env.declare(self.name, ClassFunction(self.returntype.resolve(env) if self.returntype else None, self.args), f)
    def run(self, env):
        pass

class NativeVariable:
    def __init__(self, name, type):
        self.name = name
        self.type = type
    def declare(self, env):
        if self.name == "args":
            env.declare(self.name, self.type.resolve(env), sys.argv[1:])
        elif self.name == "stdin":
            env.declare(self.name, self.type.resolve(env), sys.stdin)
        elif self.name == "stdout":
            env.declare(self.name, self.type.resolve(env), sys.stdout)
        elif self.name == "stderr":
            env.declare(self.name, self.type.resolve(env), sys.stderr)
        else:
            assert False, self.name
    def run(self, env):
        pass

class FunctionParameter:
    def __init__(self, name, type, mode, default):
        self.name = name
        self.type = type
        self.mode = mode
        self.default = default

class FunctionDeclaration:
    def __init__(self, type, name, returntype, args, statements):
        self.type = type
        self.name = name
        self.returntype = returntype
        self.args = args
        self.statements = statements
    def declare(self, env):
        type = ClassFunction(self.returntype.resolve(env) if self.returntype else None, self.args)
        def func(env2, *a):
            e = Environment(env)
            for i, arg in enumerate(self.args):
                e.declare(arg.name, None, self.args[i].type.resolve(e).default(e) if self.args[i].mode is OUT else a[i] if i < len(a) else arg.default.eval(e))
            for s in self.statements:
                s.declare(e)
            r = None
            try:
                for s in self.statements:
                    s.run(e)
            except ReturnException as x:
                r = x.expr
            if hasattr(func, "_outs"):
                return [r] + [e.get_value(arg.name) for i, arg in enumerate(self.args) if func._outs[i]]
            else:
                return r
        outs = [x.mode is not IN for x in self.args]
        if any(outs):
            func._outs = outs
        if self.type is not None:
            env.get_value(self.type).methods[self.name] = func
        else:
            env.declare(self.name, type, func)
    def run(self, env):
        pass

class FunctionCallExpression:
    def __init__(self, func, args):
        self.func = func
        self.args = args
    def eval(self, env):
        args = [a[1].eval(env) for a in self.args]
        obj = None
        if isinstance(self.func, (DotExpression, ArrowExpression)):
            # Evaluate and save obj once so we don't evaluate it twice for one call.
            obj = self.func.expr.eval(env)
            f = self.func.eval_obj(env, obj)
        else:
            f = self.func.eval(env)
        if callable(f):
            e = env
            while e.parent is not None:
                e = e.parent
            funcenv = Environment(e)
            if isinstance(self.func, DotExpression) and not (isinstance(self.func.expr, IdentifierExpression) and isinstance(env.get_type(self.func.expr.name), ClassModule)):
                args = [obj] + args
            elif isinstance(self.func, ArrowExpression):
                args = [obj] + args
            r = f(funcenv, *args)
            if hasattr(f, "_outs"):
                j = 1
                for i, out in enumerate(f._outs):
                    if out:
                        if i < len(self.args):
                            self.args[i][1].set(env, r[j])
                        j += 1
                r = r[0]
            return r
        elif isinstance(f, ClassRecord):
            return f.make(env, [x[0] for x in self.args], args)
        assert False, (self.func, f)

class ExpressionStatement:
    def __init__(self, expr):
        self.expr = expr
    def declare(self, env):
        pass
    def run(self, env):
        self.expr.eval(env)

class AssertStatement:
    def __init__(self, expr, parts):
        self.expr = expr
        self.parts = parts
    def declare(self, env):
        pass
    def run(self, env):
        assert self.expr.eval(env), [x.eval(env) for x in self.parts]

class AssignmentStatement:
    def __init__(self, var, rhs):
        self.var = var
        self.rhs = rhs
    def declare(self, env):
        pass
    def run(self, env):
        x = self.rhs.eval(env)
        if not isinstance(x, file):
            x = copy.deepcopy(x)
        self.var.set(env, x)
    def eval(self, env):
        # This is used in the rewrite of a.append(b) to a := a & b.
        r = copy.deepcopy(self.rhs.eval(env))
        self.var.set(env, r)
        return r

class CaseStatement:
    class ComparisonWhenCondition:
        def __init__(self, op, value):
            self.op = op
            self.value = value
        def check(self, env, x):
            return eval_cond(x, self.op, self.value.eval(env))
    class RangeWhenCondition:
        def __init__(self, low, high):
            self.low = low
            self.high = high
        def check(self, env, x):
            return self.low.eval(env) <= x <= self.high.eval(env)
    class TypeTestWhenCondition:
        def __init__(self, target):
            self.target = target
        def check(self, env, x):
            if isinstance(self.target, TypeSimple):
                if self.target.name == "Boolean":
                    return isinstance(x, bool)
                if self.target.name == "Number":
                    return isinstance(x, (int, float))
                if self.target.name == "String":
                    return isinstance(x, (str, unicode))
            if isinstance(self.target, TypeParameterised):
                if self.target.elementtype.name == "Number":
                    return all(isinstance(t, int) for t in x)
                if self.target.elementtype.name == "Object":
                    return True
    def __init__(self, expr, clauses):
        self.expr = expr
        self.clauses = clauses
    def declare(self, env):
        pass
    def run(self, env):
        expr = self.expr.eval(env)
        for conds, statements in self.clauses:
            if conds is None or any(c.check(env, expr) for c in conds):
                for s in statements:
                    s.declare(env)
                for s in statements:
                    s.run(env)
                break

class ExitStatement:
    def __init__(self, label):
        self.label = label
    def declare(self, env):
        pass
    def run(self, env):
        if self.label == "FUNCTION":
            raise ReturnException(None)
        else:
            raise ExitException(self.label)

class ForStatement:
    def __init__(self, var, start, end, step, label, statements):
        self.var = var
        self.start = start
        self.end = end
        self.step = step
        self.label = label
        self.statements = statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        i = self.start.eval(env)
        end = self.end.eval(env)
        step = self.step.eval(env) if self.step else 1
        env.declare(self.var, ClassNumber(), i)
        try:
            while (step > 0 and i <= end) or (step < 0 and i >= end):
                env.set(self.var, i)
                try:
                    for s in self.statements:
                        s.run(env)
                except NextException as x:
                    if x.label != self.label:
                        raise
                i += step
        except ExitException as x:
            if x.label != self.label:
                raise
        env = env.parent

class ForeachStatement:
    def __init__(self, var, array, index, label, statements):
        self.var = var
        self.array = array
        self.index = index
        self.label = label
        self.statements = statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        env.declare(self.var, None, None)
        if self.index:
            env.declare(self.index, ClassNumber(), None)
        try:
            for i, x in enumerate(self.array.eval(env)):
                if self.index:
                    env.set(self.index, i)
                env.set(self.var, x)
                try:
                    for s in self.statements:
                        s.run(env)
                except NextException as x:
                    if x.label != self.label:
                        raise
        except ExitException as x:
            if x.label != self.label:
                raise
        env = env.parent

class IfStatement:
    def __init__(self, condition_statements, else_statements):
        self.condition_statements = condition_statements
        self.else_statements = else_statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for cond, statements in self.condition_statements:
            if cond.eval(env):
                if isinstance(cond, ValidPointerExpression):
                    for expr, name in cond.tests:
                        env.declare(name, None, expr.eval(env))
                for s in statements:
                    s.declare(env)
                for s in statements:
                    s.run(env)
                break
        else:
            for s in self.else_statements:
                s.declare(env)
            for s in self.else_statements:
                s.run(env)
        env = env.parent

class IncrementStatement:
    def __init__(self, expr, delta):
        self.expr = expr
        self.delta = delta
    def declare(self, env):
        pass
    def run(self, env):
        self.expr.set(env, self.expr.eval(env) + self.delta)

class LoopStatement:
    def __init__(self, label, statements):
        self.label = label
        self.statements = statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        try:
            while True:
                try:
                    for s in self.statements:
                        s.run(env)
                except NextException as x:
                    if x.label != self.label:
                        raise
        except ExitException as x:
            if x.label != self.label:
                raise
        env = env.parent

class NextStatement:
    def __init__(self, label):
        self.label = label
    def declare(self, env):
        pass
    def run(self, env):
        raise NextException(self.label)

class RaiseStatement:
    def __init__(self, name, info):
        self.name = name
        self.info = info
    def declare(self, env):
        pass
    def run(self, env):
        if self.info is not None:
            info = self.info.eval(env)
            raise NeonException(self.name, info.info, info.code)
        else:
            raise NeonException(self.name)

class RepeatStatement:
    def __init__(self, label, cond, statements):
        self.label = label
        self.cond = cond
        self.statements = statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        try:
            while True:
                try:
                    for s in self.statements:
                        s.run(env)
                except NextException as x:
                    if x.label != self.label:
                        raise
                    continue
                if self.cond.eval(env):
                    break
        except ExitException as x:
            if x.label != self.label:
                raise
        env = env.parent

class ReturnStatement:
    def __init__(self, expr):
        self.expr = expr
    def declare(self, env):
        pass
    def run(self, env):
        raise ReturnException(self.expr.eval(env))

class TryStatement:
    def __init__(self, statements, catches):
        self.statements = statements
        self.catches = catches
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        try:
            for s in self.statements:
                s.run(env)
        except NeonException as x:
            for exceptions, name, statements in self.catches:
                # Match either unqualified or module qualified name.
                if any(x.name[:len(h)] == h or x.name[:len(h)-1] == h[1:] for h in exceptions):
                    env.declare(name, None, x)
                    for s in statements:
                        s.declare(env)
                    for s in statements:
                        s.run(env)
                    break
            else:
                raise
        env = env.parent

class WhileStatement:
    def __init__(self, cond, label, statements):
        self.cond = cond
        self.label = label
        self.statements = statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        try:
            while self.cond.eval(env):
                if isinstance(self.cond, ValidPointerExpression):
                    for expr, name in self.cond.tests:
                        env.declare(name, None, expr.eval(env))
                try:
                    for s in self.statements:
                        s.run(env)
                except NextException as x:
                    if x.label != self.label:
                        raise
        except ExitException as x:
            if x.label != self.label:
                raise
        env = env.parent

class Program:
    def __init__(self, statements):
        self.statements = statements
        self.env = Environment()
    def run(self, env):
        for s in self.statements:
            s.declare(env)
        for s in self.statements:
            s.run(env)
        main = env.names.get("MAIN")
        if main is not None:
            main[1](env)

class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.i = 0

    def expect(self, token):
        assert self.tokens[self.i] is token, self.tokens[self.i]
        self.i += 1

    def identifier(self):
        assert isinstance(self.tokens[self.i], Identifier), self.tokens[self.i]
        r = self.tokens[self.i].name
        self.i += 1
        return r

    def parse_parameterised_type(self):
        kind = self.tokens[self.i]
        assert kind is ARRAY or kind is DICTIONARY, kind
        self.i += 1
        self.expect(LESS)
        elementtype = self.parse_type()
        self.expect(GREATER)
        return TypeParameterised(kind, elementtype)

    def parse_record_type(self):
        self.expect(RECORD)
        fields = []
        while self.tokens[self.i] is not END:
            is_private = self.tokens[self.i] is PRIVATE
            if is_private:
                self.i += 1
            name = self.identifier()
            self.expect(COLON)
            type = self.parse_type()
            fields.append(Field(name, type))
        self.expect(END)
        self.expect(RECORD)
        return TypeRecord(fields)

    def parse_class_type(self):
        self.expect(CLASS)
        while self.tokens[self.i] is IMPLEMENTS:
            self.i += 1
            interface = self.identifier()
        fields = []
        while self.tokens[self.i] is not END:
            is_private = self.tokens[self.i] is PRIVATE
            if is_private:
                self.i += 1
            name = self.identifier()
            self.expect(COLON)
            type = self.parse_type()
            fields.append(Field(name, type))
        self.expect(END)
        self.expect(CLASS)
        return TypeRecord(fields)

    def parse_enum_type(self):
        self.expect(ENUM)
        names = []
        while self.tokens[self.i] is not END:
            name = self.identifier()
            names.append(name)
        self.expect(END)
        self.expect(ENUM)
        return TypeEnum(names)

    def parse_pointer_type(self):
        self.expect(POINTER)
        if self.tokens[self.i] is not TO:
            return TypePointer(None)
        self.expect(TO)
        type = self.parse_type()
        return TypePointer(type)

    def parse_valid_pointer_type(self):
        self.expect(VALID)
        self.expect(POINTER)
        self.expect(TO)
        type = self.parse_type()
        return TypePointer(type)

    def parse_function_type(self):
        self.expect(FUNCTION)
        returntype, args = self.parse_function_parameters()
        return TypeFunction(returntype, args)

    def parse_type(self):
        if self.tokens[self.i] is ARRAY or self.tokens[self.i] is DICTIONARY:
            return self.parse_parameterised_type()
        if self.tokens[self.i] is RECORD:
            return self.parse_record_type()
        if self.tokens[self.i] is CLASS:
            return self.parse_class_type()
        if self.tokens[self.i] is ENUM:
            return self.parse_enum_type()
        if self.tokens[self.i] is POINTER:
            return self.parse_pointer_type()
        if self.tokens[self.i] is VALID:
            return self.parse_valid_pointer_type()
        if self.tokens[self.i] is FUNCTION:
            return self.parse_function_type()
        name = self.identifier()
        if self.tokens[self.i] is not DOT:
            return TypeSimple(name)
        self.i += 1
        module = name
        name = self.identifier()
        return TypeCompound((module, name))

    def parse_import(self):
        self.expect(IMPORT)
        module = self.identifier()
        name = None
        if self.tokens[self.i] is DOT:
            self.i += 1
            name = self.identifier()
        return ImportDeclaration(module, name)

    def parse_type_definition(self):
        self.expect(TYPE)
        name = self.identifier()
        self.expect(IS)
        type = self.parse_type()
        return TypeDeclaration(name, type)

    def parse_constant_definition(self):
        self.expect(CONSTANT)
        name = self.identifier()
        self.expect(COLON)
        type = self.parse_type()
        self.expect(ASSIGN)
        value = self.parse_expression()
        return ConstantDeclaration(name, type, value)

    def parse_function_parameters(self):
        self.expect(LPAREN)
        args = []
        if self.tokens[self.i] is not RPAREN:
            while True:
                mode = IN
                if self.tokens[self.i] in [IN, OUT, INOUT]:
                    mode = self.tokens[self.i]
                    self.i += 1
                vars = self.parse_variable_declaration()
                default = None
                if self.tokens[self.i] is DEFAULT:
                    self.i += 1
                    default = self.parse_expression()
                args.extend([FunctionParameter(x, vars[1], mode, default) for x in vars[0]])
                if self.tokens[self.i] is not COMMA:
                    break
                self.i += 1
        self.expect(RPAREN)
        returntype = None
        if self.tokens[self.i] is COLON:
            self.i += 1
            returntype = self.parse_type()
        return returntype, args

    def parse_function_header(self):
        self.expect(FUNCTION)
        name = self.identifier()
        type = None
        if self.tokens[self.i] is DOT:
            self.i += 1
            type = name
            name = self.identifier()
        return tuple([type, name] + list(self.parse_function_parameters()))

    def parse_function_definition(self):
        type, name, returntype, args = self.parse_function_header()
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(FUNCTION)
        return FunctionDeclaration(type, name, returntype, args, statements)

    def parse_foreign_definition(self):
        self.expect(FOREIGN)
        type, name, returntype, args = self.parse_function_header()
        self.parse_dictionary_literal()
        self.expect(END)
        self.expect(FUNCTION)

    def parse_variable_declaration(self):
        names = []
        while True:
            names.append(self.identifier())
            if self.tokens[self.i] is not COMMA:
                break
            self.i += 1
        self.expect(COLON)
        t = self.parse_type()
        return names, t

    def parse_function_call(self, func):
        self.i += 1
        args = []
        if self.tokens[self.i] is not RPAREN:
            while True:
                if self.tokens[self.i] in [IN, OUT, INOUT]:
                    self.i += 1
                name = None
                if isinstance(self.tokens[self.i], Identifier) and self.tokens[self.i+1] is WITH:
                    name = self.tokens[self.i].name
                    self.i += 2
                e = self.parse_expression()
                args.append((name, e))
                if self.tokens[self.i] is not COMMA:
                    break
                self.i += 1
        self.expect(RPAREN)
        return FunctionCallExpression(func, args)

    def parse_array_literal(self):
        self.expect(LBRACKET)
        elements = []
        while self.tokens[self.i] is not RBRACKET:
            element = self.parse_expression()
            elements.append(element)
            if self.tokens[self.i] is COMMA:
                self.i += 1
            elif self.tokens[self.i] is TO:
                self.i += 1
                first = element
                last = self.parse_expression()
                if self.tokens[self.i] is STEP:
                    self.i += 1
                    step = self.parse_expression()
                else:
                    step = NumberLiteralExpression(1)
                self.expect(RBRACKET)
                return ArrayLiteralRangeExpression(first, last, step)
        self.expect(RBRACKET)
        return ArrayLiteralExpression(elements)

    def parse_dictionary_literal(self):
        self.expect(LBRACE)
        elements = []
        while self.tokens[self.i] is not RBRACE:
            key = self.parse_expression()
            self.expect(COLON)
            element = self.parse_expression()
            elements.append((key, element))
            if self.tokens[self.i] is COMMA:
                self.i += 1
        self.expect(RBRACE)
        return DictionaryLiteralExpression(elements)

    def parse_interpolated_string_expression(self):
        parts = []
        while True:
            parts.append((StringLiteralExpression(self.tokens[self.i].value), None))
            self.i += 1
            if self.tokens[self.i] is not SUBBEGIN:
                break
            self.i += 1
            e = self.parse_expression()
            fmt = None
            if self.tokens[self.i] is SUBFMT:
                self.i += 1
                fmt = self.tokens[self.i].value
                self.i += 1
            parts.append((e, fmt))
            self.expect(SUBEND)
            assert isinstance(self.tokens[self.i], String), self.tokens[self.i]
        return InterpolatedStringExpression(parts)

    def parse_atom(self):
        t = self.tokens[self.i]
        if t is LPAREN:
            self.i += 1
            expr = self.parse_expression()
            self.expect(RPAREN)
            return expr
        elif t is LBRACKET:
            return self.parse_array_literal()
        elif t is LBRACE:
            return self.parse_dictionary_literal()
        elif t is FALSE:
            self.i += 1
            return BooleanLiteralExpression(False)
        elif t is TRUE:
            self.i += 1
            return BooleanLiteralExpression(True)
        elif isinstance(t, Number):
            self.i += 1
            return NumberLiteralExpression(t.value)
        elif isinstance(t, String):
            if self.tokens[self.i+1] is SUBBEGIN:
                return self.parse_interpolated_string_expression()
            self.i += 1
            return StringLiteralExpression(t.value)
        elif t is HEXBYTES:
            self.i += 1
            assert isinstance(self.tokens[self.i], String)
            b = bytes([int(x, 16) for x in re.findall(r"[0-9a-z]{1,2}", self.tokens[self.i].value)])
            self.i += 1
            return StringLiteralExpression(b)
        elif t is PLUS:
            self.i += 1
            atom = self.parse_atom()
            return UnaryPlusExpression(atom)
        elif t is MINUS:
            self.i += 1
            atom = self.parse_atom()
            return UnaryMinusExpression(atom)
        elif t is NOT:
            self.i += 1
            atom = self.parse_atom()
            return LogicalNotExpression(atom)
        elif t is NEW:
            self.i += 1
            type = IdentifierExpression(self.identifier())
            # TODO: modules
            #if self.tokens[self.i] is DOT:
            #    self.i += 1
            #    type = DotExpression(type, self.identifier())
            type = TypeSimple(type.name)
            value = None
            if self.tokens[self.i] is LPAREN:
                value = self.parse_function_call(type)
            return NewRecordExpression(type, value)
        elif t is NIL:
            self.i += 1
            return NilLiteralExpression()
        elif t is NOWHERE:
            self.i += 1
            return NowhereLiteralExpression()
        elif isinstance(t, Identifier):
            self.i += 1
            expr = IdentifierExpression(t.name)
            while True:
                if self.tokens[self.i] is LBRACKET:
                    self.i += 1
                    while True:
                        first_from_end = False
                        last_from_end = False
                        if self.tokens[self.i] is FIRST:
                            self.i += 1
                            if self.tokens[self.i] in [PLUS, MINUS]:
                                index = self.parse_expression()
                            else:
                                index = NumberLiteralExpression(0)
                        elif self.tokens[self.i] == LAST:
                            self.i += 1
                            first_from_end = True
                            if self.tokens[self.i] in [PLUS, MINUS]:
                                index = SubtractionExpression(self.parse_expression(), NumberLiteralExpression(1))
                            else:
                                index = NumberLiteralExpression(-1)
                        else:
                            index = self.parse_expression()
                        if self.tokens[self.i] == TO:
                            self.i += 1
                            if self.tokens[self.i] is FIRST:
                                self.i += 1
                                if self.tokens[self.i] in [PLUS, MINUS]:
                                    last = self.parse_expression()
                                else:
                                    last = NumberLiteralExpression(0)
                            elif self.tokens[self.i] is LAST:
                                self.i += 1
                                last_from_end = True
                                if self.tokens[self.i] in [PLUS, MINUS]:
                                    last = self.parse_expression()
                                else:
                                    last = NumberLiteralExpression(0)
                            else:
                                last = self.parse_expression()
                            expr = RangeSubscriptExpression(expr, index, first_from_end, last, last_from_end)
                        else:
                            expr = SubscriptExpression(expr, index)
                        if self.tokens[self.i] is RBRACKET:
                            self.i += 1
                            break
                        elif self.tokens[self.i] is COMMA:
                            self.i += 1
                        else:
                            self.expect(RBRACKET)
                elif self.tokens[self.i] is LPAREN:
                    expr = self.parse_function_call(expr)
                elif self.tokens[self.i] is DOT:
                    self.i += 1
                    field = self.identifier()
                    expr = DotExpression(expr, field)
                elif self.tokens[self.i] is ARROW:
                    self.i += 1
                    field = self.identifier()
                    expr = ArrowExpression(expr, field)
                else:
                    break
            # This hack transforms a statement like a.append(b) into a := a & b.
            # The magic AppendExpression does the assignment and (because it's an expression)
            # returns the result, which should be unused since .append() doesn't actually
            # return a value.
            if isinstance(expr, FunctionCallExpression) and isinstance(expr.func, DotExpression) and expr.func.field == "append":
                return AssignmentStatement(expr.func.expr, AppendExpression(expr.func.expr, expr.args[0][1]))
            return expr
        else:
            assert False, t

    def parse_exponentiation(self):
        left = self.parse_atom()
        while True:
            if self.tokens[self.i] is EXP:
                self.i += 1
                right = self.parse_atom()
                left = ExponentiationExpression(left, right)
            else:
                break
        return left

    def parse_multiplication(self):
        left = self.parse_exponentiation()
        while True:
            if self.tokens[self.i] is TIMES:
                self.i += 1
                right = self.parse_exponentiation()
                left = MultiplicationExpression(left, right)
            elif self.tokens[self.i] is DIVIDE:
                self.i += 1
                right = self.parse_exponentiation()
                left = DivisionExpression(left, right)
            elif self.tokens[self.i] is INTDIV:
                self.i += 1
                right = self.parse_exponentiation()
                left = IntegerDivisionExpression(left, right)
            elif self.tokens[self.i] is MOD:
                self.i += 1
                right = self.parse_exponentiation()
                left = ModuloExpression(left, right)
            else:
                break
        return left

    def parse_addition(self):
        left = self.parse_multiplication()
        while True:
            if self.tokens[self.i] is PLUS:
                self.i += 1
                right = self.parse_multiplication()
                left = AdditionExpression(left, right)
            elif self.tokens[self.i] is MINUS:
                self.i += 1
                right = self.parse_multiplication()
                left = SubtractionExpression(left, right)
            elif self.tokens[self.i] is CONCAT:
                self.i += 1
                right = self.parse_multiplication()
                left = ConcatenationExpression(left, right)
            else:
                break
        return left

    def parse_comparison(self):
        left = self.parse_addition()
        comps = []
        while (self.tokens[self.i] is EQUAL
            or self.tokens[self.i] is NOTEQUAL
            or self.tokens[self.i] is LESS
            or self.tokens[self.i] is GREATER
            or self.tokens[self.i] is LESSEQ
            or self.tokens[self.i] is GREATEREQ):
            comp = self.tokens[self.i]
            self.i += 1
            right = self.parse_addition()
            comps.append(ComparisonExpression(left, right, comp))
            left = right
        if not comps:
            return left
        elif len(comps) == 1:
            return comps[0]
        else:
            return ChainedComparisonExpression(comps)

    def parse_typetest(self):
        left = self.parse_comparison()
        if self.tokens[self.i] is ISA:
            self.i += 1
            target = self.parse_type()
            return TypeTestExpression(left, target)
        else:
            return left

    def parse_membership(self):
        left = self.parse_typetest()
        if self.tokens[self.i] is IN or (self.tokens[self.i] is NOT and self.tokens[self.i+1] is IN):
            notin = self.tokens[self.i] is NOT
            if notin:
                self.i += 1
            self.i += 1
            right = self.parse_typetest()
            r = MembershipExpression(left, right)
            if notin:
                r = LogicalNotExpression(r)
            return r
        else:
            return left

    def parse_conjunction(self):
        left = self.parse_membership()
        while self.tokens[self.i] is AND:
            self.i += 1
            right = self.parse_membership()
            left = ConjunctionExpression(left, right)
        return left

    def parse_disjunction(self):
        left = self.parse_conjunction()
        while self.tokens[self.i] is OR:
            self.i += 1
            right = self.parse_conjunction()
            left = DisjunctionExpression(left, right)
        return left

    def parse_conditional(self):
        if self.tokens[self.i] is IF:
            self.i += 1
            cond = self.parse_expression()
            self.expect(THEN)
            left = self.parse_expression()
            self.expect(ELSE)
            right = self.parse_expression()
            return ConditionalExpression(cond, left, right)
        elif self.tokens[self.i] is TRY:
            self.i += 1
            expr = self.parse_expression()
            catches = []
            while self.tokens[self.i] is TRAP:
                self.i += 1
                name = []
                while True:
                    name.append(self.identifier())
                    if self.tokens[self.i] is not DOT:
                        break
                    self.i += 1
                exceptions = [name]
                infoname = None
                if self.tokens[self.i] is AS:
                    self.i += 1
                    infoname = self.identifier()
                if self.tokens[self.i] is DO:
                    self.i += 1
                    handler = []
                    while self.tokens[self.i] is not TRAP and self.tokens[self.i] is not RPAREN and self.tokens[self.i] is not END_OF_FILE:
                        s = self.parse_statement()
                        if s is not None:
                            handler.append(s)
                    catches.append((exceptions, infoname, handler))
                elif self.tokens[self.i] is GIVES:
                    self.i += 1
                    g = self.parse_expression()
                    catches.append((exceptions, infoname, g))
            return TryExpression(expr, catches)
        else:
            return self.parse_disjunction()

    def parse_expression(self):
        return self.parse_conditional()

    def parse_assert(self):
        self.expect(ASSERT)
        expr = self.parse_expression()
        parts = [expr]
        while self.tokens[self.i] is COMMA:
            self.i += 1
            e = self.parse_expression()
            parts.append(e)
        return AssertStatement(expr, parts)

    def parse_case_statement(self):
        self.expect(CASE)
        expr = self.parse_expression()
        clauses = []
        while self.tokens[self.i] is WHEN and self.tokens[self.i+1] is not OTHERS:
            self.i += 1
            conditions = []
            while True:
                t = self.tokens[self.i]
                if t is EQUAL or t is NOTEQUAL or t is LESS or t is GREATER or t is LESSEQ or t is GREATEREQ:
                    op = t
                    self.i += 1
                    when = self.parse_expression()
                    conditions.append(CaseStatement.ComparisonWhenCondition(op, when))
                elif t is ISA:
                    op = t
                    self.i += 1
                    when = self.parse_type()
                    conditions.append(CaseStatement.TypeTestWhenCondition(when))
                else:
                    when = self.parse_expression()
                    if self.tokens[self.i] is TO:
                        self.i += 1
                        when2 = self.parse_expression()
                        conditions.append(CaseStatement.RangeWhenCondition(when, when2))
                    else:
                        conditions.append(CaseStatement.ComparisonWhenCondition(EQUAL, when))
                if self.tokens[self.i] is not COMMA:
                    break
                self.i += 1
            self.expect(DO)
            statements = []
            while self.tokens[self.i] is not WHEN and self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
                s = self.parse_statement()
                if s is not None:
                    statements.append(s)
            clauses.append((conditions, statements))
        others_statements = []
        if self.tokens[self.i] is WHEN and self.tokens[self.i+1] is OTHERS and self.tokens[self.i+2] is DO:
            self.i += 3
            while self.tokens[self.i] is not END:
                s = self.parse_statement()
                if s is not None:
                    others_statements.append(s)
        self.expect(END)
        self.expect(CASE)
        clauses.append((None, others_statements))
        return CaseStatement(expr, clauses)

    def parse_check_statement(self):
        self.expect(CHECK)
        if self.tokens[self.i] is VALID:
            tests = []
            while True:
                self.i += 1
                ptr = self.parse_expression()
                if self.tokens[self.i] is AS:
                    self.i += 1
                    name = self.identifier()
                else:
                    assert isinstance(ptr, IdentifierExpression)
                    name = ptr.name
                tests.append((ptr, name))
                if self.tokens[self.i] is not COMMA:
                    break
            cond = ValidPointerExpression(tests)
        else:
            cond = self.parse_expression()
        self.expect(ELSE)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(CHECK)
        return IfStatement([(cond, [])], statements)

    def parse_declaration(self):
        self.expect(DECLARE)
        if self.tokens[self.i] in [NATIVE, EXTENSION]:
            self.i += 1
            if self.tokens[self.i] == CONSTANT:
                self.i += 1
                name = self.identifier()
                self.expect(COLON)
                type = self.parse_type()
                if name == "Separator":
                    return ConstantDeclaration(name, type, StringLiteralExpression(os.sep))
            elif self.tokens[self.i] == FUNCTION:
                type, name, returntype, args = self.parse_function_header()
                return NativeFunction(name, returntype, args)
            elif self.tokens[self.i] == VAR:
                self.i += 1
                name = self.identifier()
                self.expect(COLON)
                type = self.parse_type()
                return NativeVariable(name, type)
        else:
            assert False

    def parse_exception(self):
        self.expect(EXCEPTION)
        name = []
        while True:
            name.append(self.identifier())
            if self.tokens[self.i] is not DOT:
                break
            self.i += 1
        return ExceptionDeclaration(name)

    def parse_interface(self):
        self.expect(INTERFACE)
        name = self.identifier()
        while self.tokens[self.i] is FUNCTION:
            self.i += 1
            method = self.identifier()
            args = self.parse_function_parameters()
        self.expect(END)
        self.expect(INTERFACE)

    def parse_exit_statement(self):
        self.expect(EXIT)
        label = self.tokens[self.i].name
        self.i += 1
        return ExitStatement(label)

    def parse_export(self):
        self.expect(EXPORT)
        if isinstance(self.tokens[self.i], Identifier):
            name = self.identifier()
        else:
            return self.parse_statement()

    def parse_if_statement(self):
        self.expect(IF)
        condition_statements = []
        else_statements = []
        while True:
            if self.tokens[self.i] is VALID:
                tests = []
                while True:
                    self.i += 1
                    ptr = self.parse_expression()
                    if self.tokens[self.i] is AS:
                        self.i += 1
                        name = self.identifier()
                    else:
                        assert isinstance(ptr, IdentifierExpression)
                        name = ptr.name
                    tests.append((ptr, name))
                    if self.tokens[self.i] is not COMMA:
                        break
                cond = ValidPointerExpression(tests)
            else:
                cond = self.parse_expression()
            self.expect(THEN)
            statements = []
            while (self.tokens[self.i] is not ELSIF
               and self.tokens[self.i] is not ELSE
               and self.tokens[self.i] is not END
               and self.tokens[self.i] is not END_OF_FILE):
                s = self.parse_statement()
                if s is not None:
                    statements.append(s)
            condition_statements.append((cond, statements))
            if self.tokens[self.i] is not ELSIF:
                break
            self.i += 1
        if self.tokens[self.i] is ELSE:
            self.i += 1
            while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
                s = self.parse_statement()
                if s is not None:
                    else_statements.append(s)
        self.expect(END)
        self.expect(IF)
        return IfStatement(condition_statements, else_statements)

    def parse_increment_statement(self):
        if self.tokens[self.i] is INC:
            delta = 1
        elif self.tokens[self.i] is DEC:
            delta = -1
        else:
            assert False, self.tokens[self.i]
        self.i += 1
        expr = self.parse_expression()
        return IncrementStatement(expr, delta)

    def parse_for_statement(self):
        self.expect(FOR)
        var = self.identifier()
        self.expect(ASSIGN)
        start = self.parse_expression()
        self.expect(TO)
        end = self.parse_expression()
        step = None
        if self.tokens[self.i] is STEP:
            self.i += 1
            step = self.parse_expression()
        label = "FOR"
        if self.tokens[self.i] is LABEL:
            self.i += 1
            label = self.identifier()
        self.expect(DO)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(FOR)
        return ForStatement(var, start, end, step, label, statements)

    def parse_foreach_statement(self):
        self.expect(FOREACH)
        var = self.identifier()
        self.expect(IN)
        array = self.parse_expression()
        index = None
        if self.tokens[self.i] is INDEX:
            self.i += 1
            index = self.identifier()
        label = "FOREACH"
        if self.tokens[self.i] is LABEL:
            self.i += 1
            label = self.identifier()
        self.expect(DO)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(FOREACH)
        return ForeachStatement(var, array, index, label, statements)

    def parse_let_statement(self):
        self.expect(LET)
        name = self.identifier()
        self.expect(COLON)
        type = self.parse_type()
        self.expect(ASSIGN)
        expr = self.parse_expression()
        return LetDeclaration(name, type, expr)

    def parse_loop_statement(self):
        self.expect(LOOP)
        label = "LOOP"
        if self.tokens[self.i] is LABEL:
            self.i += 1
            label = self.identifier()
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(LOOP)
        return LoopStatement(label, statements)

    def parse_main_statement(self):
        self.expect(BEGIN)
        self.expect(MAIN)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(MAIN)
        return FunctionDeclaration(None, "MAIN", None, [], statements)

    def parse_next_statement(self):
        self.expect(NEXT)
        label = self.tokens[self.i].name
        self.i += 1
        return NextStatement(label)

    def parse_raise_statement(self):
        self.expect(RAISE)
        name = []
        while True:
            name.append(self.identifier())
            if self.tokens[self.i] is not DOT:
                break
            self.i += 1
        if self.tokens[self.i] is LPAREN:
            class ExceptionType:
                def __init__(self):
                    self.type = ClassRecord(
                        [
                            Field("info", TypeSimple("String")),
                            Field("code", TypeSimple("Number")),
                            Field("offset", TypeSimple("Number")),
                        ],
                        {}
                    )
                def resolve(self, env):
                    return self.type
            et = ExceptionType()
            info = NewRecordExpression(
                et,
                self.parse_function_call(et.type)
            )
        else:
            info = None
        return RaiseStatement(name, info)

    def parse_repeat_statement(self):
        self.expect(REPEAT)
        label = "REPEAT"
        if self.tokens[self.i] is LABEL:
            self.i += 1
            label = self.identifier()
        statements = []
        while self.tokens[self.i] is not UNTIL and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(UNTIL)
        cond = self.parse_expression()
        return RepeatStatement(label, cond, statements)

    def parse_return_statement(self):
        self.expect(RETURN)
        expr = self.parse_expression()
        return ReturnStatement(expr)

    def parse_try_statement(self):
        self.expect(TRY)
        statements = []
        while self.tokens[self.i] is not TRAP and self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        catches = []
        while self.tokens[self.i] is TRAP:
            self.i += 1
            name = []
            while True:
                name.append(self.identifier())
                if self.tokens[self.i] is not DOT:
                    break
                self.i += 1
            exceptions = [name]
            infoname = None
            if self.tokens[self.i] is AS:
                self.i += 1
                infoname = self.identifier()
            self.expect(DO)
            handler = []
            while self.tokens[self.i] is not TRAP and self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
                s = self.parse_statement()
                if s is not None:
                    handler.append(s)
            catches.append((exceptions, infoname, handler))
        self.expect(END)
        self.expect(TRY)
        return TryStatement(statements, catches)

    def parse_unused_statement(self):
        self.expect(UNUSED)
        while True:
            self.identifier()
            if self.tokens[self.i] is not COMMA:
                break
            self.expect(COMMA)
        return None

    def parse_var_statement(self):
        self.expect(VAR)
        vars = self.parse_variable_declaration()
        expr = None
        if self.tokens[self.i] is ASSIGN:
            self.i += 1
            expr = self.parse_expression()
        return VariableDeclaration(vars[0], vars[1], expr)

    def parse_while_statement(self):
        self.expect(WHILE)
        if self.tokens[self.i] is VALID:
            tests = []
            while True:
                self.i += 1
                ptr = self.parse_expression()
                if self.tokens[self.i] is AS:
                    self.i += 1
                    name = self.identifier()
                else:
                    assert isinstance(ptr, IdentifierExpression)
                    name = ptr.name
                tests.append((ptr, name))
                if self.tokens[self.i] is not COMMA:
                    break
            cond = ValidPointerExpression(tests)
        else:
            cond = self.parse_expression()
        label = "WHILE"
        if self.tokens[self.i] is LABEL:
            self.i += 1
            label = self.identifier()
        self.expect(DO)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(WHILE)
        return WhileStatement(cond, label, statements)

    def parse_statement(self):
        if self.tokens[self.i] is IMPORT:   return self.parse_import()
        if self.tokens[self.i] is TYPE:     return self.parse_type_definition()
        if self.tokens[self.i] is CONSTANT: return self.parse_constant_definition()
        if self.tokens[self.i] is FUNCTION: return self.parse_function_definition()
        if self.tokens[self.i] is FOREIGN:  return self.parse_foreign_definition()
        if self.tokens[self.i] is DECLARE:  return self.parse_declaration()
        if self.tokens[self.i] is EXCEPTION:return self.parse_exception()
        if self.tokens[self.i] is INTERFACE:return self.parse_interface()
        if self.tokens[self.i] is EXPORT:   return self.parse_export()
        if self.tokens[self.i] is IF:       return self.parse_if_statement()
        if self.tokens[self.i] in [INC,DEC]:return self.parse_increment_statement()
        if self.tokens[self.i] is RETURN:   return self.parse_return_statement()
        if self.tokens[self.i] is VAR:      return self.parse_var_statement()
        if self.tokens[self.i] is LET:      return self.parse_let_statement()
        if self.tokens[self.i] is WHILE:    return self.parse_while_statement()
        if self.tokens[self.i] is CASE:     return self.parse_case_statement()
        if self.tokens[self.i] is FOR:      return self.parse_for_statement()
        if self.tokens[self.i] is FOREACH:  return self.parse_foreach_statement()
        if self.tokens[self.i] is LOOP:     return self.parse_loop_statement()
        if self.tokens[self.i] is REPEAT:   return self.parse_repeat_statement()
        if self.tokens[self.i] is EXIT:     return self.parse_exit_statement()
        if self.tokens[self.i] is NEXT:     return self.parse_next_statement()
        if self.tokens[self.i] is TRY:      return self.parse_try_statement()
        if self.tokens[self.i] is RAISE:    return self.parse_raise_statement()
        if self.tokens[self.i] is ASSERT:   return self.parse_assert()
        if self.tokens[self.i] is CHECK:    return self.parse_check_statement()
        if self.tokens[self.i] is UNUSED:   return self.parse_unused_statement()
        if self.tokens[self.i] is BEGIN:    return self.parse_main_statement()
        if isinstance(self.tokens[self.i], Identifier):
            expr = self.parse_expression()
            if self.tokens[self.i] is ASSIGN:
                self.i += 1
                rhs = self.parse_expression()
                return AssignmentStatement(expr, rhs)
            else:
                return ExpressionStatement(expr)
        else:
            assert False, self.tokens[self.i:self.i+10]

    def parse(self):
        statements = []
        while self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        return Program(statements)

def parse(tokens):
    return Parser(tokens).parse()

class Class:
    pass

class ClassBoolean(Class):
    def default(self, env):
        return False

class ClassNumber(Class):
    def default(self, env):
        return 0

class ClassString(Class):
    def default(self, env):
        return ""

class ClassBytes(Class):
    def default(self, env):
        class Bytes:
            def __init__(self):
                self.a = []
            def __eq__(self, rhs):
                return self.a == rhs.a
            def fromArray(self, env, a):
                self.a = list(a)
            def size(self, env):
                return len(self.a)
            def toArray(self, env):
                return list(self.a)
            def decodeToString(self, env, obj):
                if isinstance(self.a, list):
                    # Convert bytes to string.
                    self.a = "".join(map(chr, self.a))
                return self.a.decode("utf-8")
        return Bytes()

class ClassObject(Class):
    def default(self, env):
        return None

class ClassArray(Class):
    def __init__(self, elementtype):
        self.elementtype = elementtype
    def default(self, env):
        return []

class ClassDictionary(Class):
    def __init__(self, elementtype):
        self.elementtype = elementtype
    def default(self, env):
        return {}

class ClassRecord(Class):
    class Instance:
        def __init__(self, fields):
            self._fields = fields
            for x in fields:
                setattr(self, x.name, None) # TODO: default()
        def __eq__(self, other):
            return all(getattr(self, x.name) == getattr(other, x.name) for x in self._fields)
    def __init__(self, fields, methods):
        self.fields = fields
        self.methods = methods
    def default(self, env):
        r = ClassRecord.Instance(self.fields)
        for f in self.fields:
            setattr(r, f.name, f.type.resolve(env).default(env))
        for n, f in self.methods.items():
            setattr(r, n, f)
        return r
    def make(self, env, names, values):
        r = self.default(env)
        for n, v in zip(names, values):
            setattr(r, n, v)
        return r

class ClassEnum(Class):
    class Instance:
        def __init__(self, name):
            self.name = name
        def __call__(self, env):
            return self
        def __eq__(self, rhs):
            return self.name == rhs.name
        def toString(self, env, obj):
            return self.name
    def __init__(self, names):
        self.names = names
        for name in self.names:
            setattr(self, name, ClassEnum.Instance(name))
    def default(self, env):
        return getattr(self, self.names[0])

class ClassPointer(Class):
    def __init__(self, type):
        self.type = type
    def default(self, env):
        return None

class ClassFunctionPointer(Class):
    def __init__(self, returntype, args):
        self.returntype = returntype
        self.args = args
    def default(self, env):
        return None

class ClassFunction(Class):
    def __init__(self, returntype, args):
        self.returntype = returntype
        self.args = args

class ClassException(Class):
    pass

class ClassModule(Class):
    pass

class Environment:
    def __init__(self, parent=None):
        self.parent = parent
        self.module_name = None
        self.names = {}
    def declare(self, name, type, value):
        assert type is None or isinstance(type, Class), type
        self.names[name] = [type, value]
    def get_type(self, name):
        e = self
        while e is not None:
            if name in e.names:
                return e.names[name][0]
            e = e.parent
        assert False, name
    def get_value(self, name):
        if name == "_":
            return None
        e = self
        while e is not None:
            if name in e.names:
                return e.names[name][1]
            e = e.parent
        assert False, name
    def module(self):
        return self.parent.module() if self.parent else self.module_name
    def set(self, name, value):
        if name == "_":
            return
        e = self
        while e is not None:
            if name in e.names:
                e.names[name][1] = value
                break
            e = e.parent
        else:
            assert False, name

class ExitException:
    def __init__(self, label):
        self.label = label

class NeonException:
    def __init__(self, name, info=None, code=None):
        if isinstance(name, str):
            self.name = [name]
        else:
            self.name = name
        self.info = info
        self.code = code
        self.offset = 0

class NextException:
    def __init__(self, label):
        self.label = label

class ReturnException:
    def __init__(self, expr):
        self.expr = expr

g_Modules = {}

def import_regex():
    return parse(tokenize("""
%|
 |  File: regex
 |
 |  Functions for using regular expressions for text searching.
 |%

EXPORT Group
EXPORT Match

EXPORT search

EXPORT EXCEPTION SyntaxException

%|
 |  Type: Group
 |
 |  Represents a matching group as part of a <Match> array.
 |
 |  Fields:
 |      start - starting index of group
 |      end - ending index of group
 |      group - text of group
 |%
TYPE Group IS RECORD
    matched: Boolean
    start: Number
    end: Number
    group: String
END RECORD

%|
 |  Type: Match
 |
 |  Represents the result of a successful regex match.
 |%
TYPE Match IS Array<Group>

%|
 |  Function: search
 |
 |  Search a string for a given subject regex.
 |%
DECLARE EXTENSION FUNCTION search(pattern: String, subject: String, OUT match: Match): Boolean
    """))

def import_module(name):
    m = g_Modules.get(name)
    if m is None:
        importer = globals().get("import_"+name)
        if importer is not None:
            m = importer()
        else:
            m = parse(tokenize(codecs.open("lib/{}.neon".format(name), encoding="utf-8").read()))
        g_Modules[name] = m
        m.env.module_name = name
        run(m)
    return m

def run(program):
    program.env.declare("Boolean", Class(), ClassBoolean())
    program.env.declare("Number", Class(), ClassNumber())
    program.env.declare("String", Class(), ClassString())
    program.env.declare("Bytes", Class(), ClassBytes())
    program.env.declare("Object", Class(), ClassObject())
    program.env.declare("chr", None, neon_chr)
    program.env.declare("concat", None, neon_concat)
    program.env.declare("format", None, neon_format)
    program.env.declare("input", None, neon_input)
    program.env.declare("int", None, neon_int)
    program.env.declare("max", None, neon_max)
    program.env.declare("min", None, neon_min)
    program.env.declare("num", None, neon_num)
    program.env.declare("odd", None, neon_odd)
    program.env.declare("ord", None, neon_ord)
    program.env.declare("print", None, neon_print)
    program.env.declare("round", None, neon_round)
    program.env.declare("str", None, neon_str)
    program.env.declare("substring", None, neon_substring)
    program.run(program.env)

def eval_cond(left, cond, right):
    return (
        left == right if cond is EQUAL else
        left != right if cond is NOTEQUAL else
        left < right if cond is LESS else
        left > right if cond is GREATER else
        left <= right if cond is LESSEQ else
        left >= right if cond is GREATEREQ else False)

def neon_array_resize(a, n):
    if n != int(n):
        raise NeonException("ArrayIndexException")
    if n < len(a):
        del a[int(n):]
    elif n > len(a):
        a.extend([0] * (n - len(a)))

def neon_chr(env, x):
    if x != int(x):
        raise NeonException("ValueRangeException", "chr() argument not an integer")
    if not (0 <= x <= 0x10ffff):
        raise NeonException("ValueRangeException", "chr() argument out of range 0-0x10ffff")
    return unichr(x)

def neon_concat(env, x, y):
    return x + y

def neon_format(env, s, fmt):
    if fmt.endswith("x"):
        return format(int(s), fmt)
    else:
        return format(s, fmt)

def neon_input(env, x):
    return raw_input()

def neon_int(env, x):
    return int(x)

def neon_max(env, x, y):
    return max(x, y)

def neon_min(env, x, y):
    return min(x, y)

def neon_num(env, x):
    return int(x) if x.isdigit() else float(x)

def neon_odd(env, x):
    if x != int(x):
        raise NeonException("ValueRangeException", "odd() requires integer")
    return (x & 1) != 0

def neon_ord(env, x):
    if len(x) != 1:
        raise NeonException("ArrayIndexException", "ord() requires string of length 1")
    return ord(x)

def neon_print(env, x):
    print(x)

def neon_round(env, places, value):
    return round(value, places)

def neon_str(env, x):
    r = str(x)
    if isinstance(x, float):
        if x == int(x):
            r = str(int(x))
        else:
            r = re.sub(r"\.0+$", "", r)
    return r

def neon_substring(env, s, start, length):
    return s[start:start+length]

def neon_file_copy(env, src, dest):
    if neon_file_exists(env, dest):
        raise NeonException(["FileException", "Exists"])
    shutil.copyfile(src, dest)

def neon_file_copyOverwriteIfExists(env, src, dest):
    shutil.copyfile(src, dest)

def neon_file_delete(env, fn):
    try:
        os.unlink(fn)
    except OSError:
        pass

def neon_file_exists(env, fn):
    return os.access(fn, os.F_OK)

def neon_file_files(env, path):
    return os.listdir(path)

def neon_file_getInfo(env, name):
    st = os.stat(name)
    return [
        os.path.basename(name),
        st.st_size,
        (st.st_mode & 0x04) != 0,
        (st.st_mode & 0x02) != 0,
        (st.st_mode & 0x01) != 0,
        0 if stat.S_ISREG(st.st_mode) else 1 if stat.S_ISDIR(st.st_mode) else 2,
        0,
        st.st_atime,
        st.st_mtime,
    ]

def neon_file_isDirectory(env, path):
    return os.path.isdir(path)

def neon_file_mkdir(env, path):
    return os.mkdir(path)

def neon_file_readBytes(env, fn):
    with open(fn, "rb") as f:
        r = ClassBytes().default(env)
        r.fromArray(env, [ord(x) for x in f.read()])
        return r

def neon_file_readLines(env, fn):
    with codecs.open(fn, "r", encoding="utf-8") as f:
        return list(map(lambda x: x.rstrip("\r\n"), f.readlines()))

def neon_file_removeEmptyDirectory(env, path):
    try:
        os.rmdir(path)
    except OSError:
        raise NeonException("FileException")

def neon_file_rename(env, old, new):
    os.rename(old, new)

def neon_file_symlink(env, target, newlink, targetIsDirectory):
    # Note: Python 2 does not support the targetIsDirectory option.
    # Also, Python 2 doesn't support os.symlink() on windows.
    os.symlink(target, newlink)

def neon_file_writeBytes(env, fn, bytes):
    with open(fn, "wb") as f:
        f.write("".join(chr(x) for x in bytes.a))

def neon_file_writeLines(env, fn, lines):
    with open(fn, "wb") as f:
        f.writelines(x.encode()+"\n" for x in lines)

def neon_io_close(env, f):
    f.close()
    return (None, None)

def neon_io_fprint(env, f, s):
    print(s, file=f)

def neon_io_open(env, fn, mode):
    return open(fn, "wb" if mode.name == "write" else "rb")

def neon_io_readBytes(env, f, count):
    r = ClassBytes().default(env)
    r.fromArray(env, [ord(x) for x in f.read(count)])
    return r

def neon_io_readLine(env, f, r):
    r = f.readline()
    return r is not None, r.rstrip("\r\n")

def neon_io_seek(env, f, offset, whence):
    f.seek(offset, {"absolute": os.SEEK_SET, "relative": os.SEEK_CUR, "fromEnd": os.SEEK_END}[whence.name])

def neon_io_tell(env, f):
    return f.tell()

def neon_io_truncate(env, f):
    f.truncate()

def neon_io_write(env, f, s):
    f.write(s)

def neon_io_writeBytes(env, f, buf):
    f.write(buf)

def neon_math_abs(env, x):
    return abs(x)

def neon_math_acos(env, x):
    return math.acos(x)

def neon_math_acosh(env, x):
    return math.acosh(x)

def neon_math_asin(env, x):
    return math.asin(x)

def neon_math_asinh(env, x):
    return math.asinh(x)

def neon_math_atan(env, x):
    return math.atan(x)

def neon_math_atanh(env, x):
    return math.atanh(x)

def neon_math_atan2(env, y, x):
    return math.atan2(y, x)

def neon_math_cbrt(env, x):
    return math.cbrt(x)

def neon_math_ceil(env, x):
    return math.ceil(x)

def neon_math_cos(env, x):
    return math.cos(x)

def neon_math_cosh(env, x):
    return math.cosh(x)

def neon_math_erf(env, x):
    return math.erf(x)

def neon_math_erfc(env, x):
    return math.erfc(x)

def neon_math_exp(env, x):
    return math.exp(x)

def neon_math_exp2(env, x):
    return math.exp2(x)

def neon_math_expm1(env, x):
    return math.expm1(x)

def neon_math_floor(env, x):
    return math.floor(x)

def neon_math_frexp(env, x):
    return math.frexp(x)

def neon_math_hypot(env, x):
    return math.hypot(x)

def neon_math_intdiv(env, x, y):
    return math.intdiv(x, y)

def neon_math_ldexp(env, x):
    return math.ldexp(x)

def neon_math_lgamma(env, x):
    return math.lgamma(x)

def neon_math_log(env, x):
    return math.log(x)

def neon_math_log10(env, x):
    return math.log10(x)

def neon_math_log1p(env, x):
    return math.log1p(x)

def neon_math_log2(env, x):
    return math.log2(x)

def neon_math_nearbyint(env, x):
    return math.nearbyint(x)

def neon_math_sign(env, x):
    return math.copysign(1, x)

def neon_math_sin(env, x):
    return math.sin(x)

def neon_math_sinh(env, x):
    return math.sinh(x)

def neon_math_sqrt(env, x):
    return math.sqrt(x)

def neon_math_tan(env, x):
    return math.tan(x)

def neon_math_tanh(env, x):
    return math.tanh(x)

def neon_math_tgamma(env, x):
    return math.tgamma(x)

def neon_math_trunc(env, x):
    return math.trunc(x)

def neon_random_uint32(env):
    return random.randint(0, 0xffffffff)

def neon_regex_search(env, r, s, match):
    m = re.search(r, s)
    if m is None:
        return None
    for g in range(1 + len(m.groups())):
        tm = g_Modules["regex"].env.get_value("Group")
        r = tm.make(env, ["matched", "start", "end", "group"], [m.group(g) is not None, m.start(g), m.end(g), m.group(g)])
        match.append(r)
    return (True, match)

def neon_runtime_assertionsEnabled(env):
    return True

def neon_runtime_executorName(env):
    return "helium"

def neon_runtime_garbageCollect(env):
    pass

def neon_runtime_getAllocatedObjectCount(env):
    return 0

def neon_runtime_moduleIsMain(env):
    return True # TODO

def neon_runtime_setGarbageCollectionInterval(env, count):
    pass

def neon_runtime_setRecursionLimit(env, depth):
    pass

def neon_string_find(env, s, t):
    return s.find(t)

def neon_string_hasPrefix(env, s, t):
    return s.startswith(t)

def neon_string_hasSuffix(env, s, t):
    return s.endswith(t)

def neon_string_join(env, a, d):
    return d.join(a)

def neon_string_lower(env, s):
    return s.lower()

def neon_string_split(env, s, d):
    return s.split(d)

def neon_string_splitLines(env, s):
    if len(s) == 0:
        return []
    s = re.sub("\r\n", "\n", s)
    s = re.sub("\n$", "", s, 1)
    return s.split("\n")

def neon_string_trimCharacters(env, s, leading, trailing):
    return s.lstrip(leading).rstrip(trailing)

def neon_string_upper(env, s):
    return s.upper()

def neon_sys_exit(env, n):
    if n != int(n) or n < 0 or n > 255:
        raise NeonException("InvalidValueException", "sys.exit invalid parameter: {}".format(n))
    sys.exit(n)

try:
    source = codecs.open(sys.argv[1], encoding="utf-8").read()
except UnicodeDecodeError:
    sys.exit(99)
if re.search(r"^%!", source, re.MULTILINE):
    sys.exit(99)
run(parse(tokenize(source)))
