from __future__ import division, print_function

import codecs
import math
import os
import re
import shutil
import sys

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
EXTERNAL = Keyword("EXTERNAL")
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
OF = Keyword("OF")
INDEX = Keyword("INDEX")
ASSERT = Keyword("ASSERT")
EMBED = Keyword("EMBED")
ALIAS = Keyword("ALIAS")
IS = Keyword("IS")

def identifier_start(c):
    return c.isalpha()

def identifier_body(c):
    return c.isalnum() or c == "_"

def number_start(c):
    return c.isdigit()

def number_body(c):
    return c.isdigit() or c == "."

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
        elif source[i] == "#": r.append(NOTEQUAL); i += 1
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
            else:
                r.append(Identifier(text))
        elif number_start(source[i]):
            start = i
            while i < len(source) and number_body(source[i]):
                i += 1
            t = source[start:i]
            num = int(t) if t.isdigit() else float(t)
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
                    elif c == "u":
                        c = unichr(int(source[i:i+4], 16))
                        i += 4
                    elif c == "(":
                        r.append(String(string))
                        r.append(SUBBEGIN)
                        start = i
                        colon = start
                        nest = 1
                        while nest > 0:
                            c = source[i]
                            i += 1
                            if   c == "(": nest += 1
                            elif c == ")": nest -= 1
                            elif c == ":" and nest == 1: colon = i - 1
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

class Field:
    def __init__(self, name, type):
        self.name = name
        self.type = type

class TypeRecord:
    def __init__(self, fields):
        self.fields = fields
    def resolve(self, env):
        return ClassRecord(self.fields)

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
        env.declare(self.name, ClassException(), None)
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

class DictionaryLiteralExpression:
    def __init__(self, elements):
        self.elements = elements
    def eval(self, env):
        return {k: v.eval(env) for k, v in self.elements}

class NilLiteralExpression:
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
                  else neon_strb(env, x) if isinstance(x, bool)
                  else str(x) if isinstance(x, int)
                  else "[{}]".format(", ".join(('"{}"'.format(e) if isinstance(e, (str, unicode)) else str(e)) for e in x)) if isinstance(x, list)
                  else x.toString(x))
            r += neon_format(env, s, f) if f else s
        return r

class NewRecordExpression:
    def __init__(self, type):
        self.type = type
    def eval(self, env):
        return self.type.resolve(env).make(env, [])

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
        try:
            return self.expr.eval(env)[self.index.eval(env)]
        except IndexError:
            raise NeonException("ArrayIndex")
        except KeyError:
            raise NeonException("DictionaryIndex")
    def set(self, env, value):
        a = self.expr.eval(env)
        i = self.index.eval(env)
        if isinstance(a, list):
            while len(a) <= i:
                a.append(None) # TODO: default()
        a[i] = value

class DotExpression:
    def __init__(self, expr, field):
        self.expr = expr
        self.field = field
    def eval(self, env):
        obj = self.expr.eval(env)
        if isinstance(obj, bool):
            if self.field == "toString": return lambda env: neon_strb(env, obj)
            assert False, self.field
        elif isinstance(obj, int):
            if self.field == "toString": return lambda env: str(obj)
            assert False, self.field
        elif isinstance(obj, (str, unicode)):
            if self.field == "append": return neon_string_append
            if self.field == "toArray": return lambda env: [ord(x) for x in obj]
            if self.field == "toBytes": return lambda env: "".join(x for x in obj.encode("utf-8"))
            if self.field == "toString": return lambda env: obj.decode("utf-8")
            assert False, self.field
        elif isinstance(obj, list):
            if self.field == "append": return lambda env, x: obj.append(x)
            if self.field == "extend": return lambda env, x: obj.extend(x)
            if self.field == "resize": return lambda env, n: neon_array_resize(obj, n)
            if self.field == "size": return lambda env: len(obj)
            if self.field == "toString": return lambda env: "[{}]".format(", ".join(('"{}"'.format(e) if isinstance(e, (str, unicode)) else str(e)) for e in obj))
            assert False, self.field
        elif isinstance(obj, dict):
            if self.field == "keys": return lambda env: obj.keys()
            assert False, self.field
        elif isinstance(obj, Program):
            return obj.env.get_value(self.field)
        elif hasattr(obj, self.field):
            return getattr(obj, self.field)
        assert False, (self.field, obj)
    def set(self, env, value):
        setattr(self.expr.eval(env), self.field, value)

class ArrowExpression:
    def __init__(self, expr, field):
        self.expr = expr
        self.field = field
    def eval(self, env):
        obj = self.expr.eval(env)
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

class ExponentiationExpression:
    def __init__(self, left, right):
        self.left = left
        self.right = right
    def eval(self, env):
        return math.pow(self.left.eval(env), self.right.eval(env))

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
            return self.left.eval(env) / self.right.eval(env)
        except ZeroDivisionError:
            raise NeonException("DivideByZero")

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
        env.declare(self.name, ClassFunction(self.returntype.resolve(env) if self.returntype else None, self.args), globals()["neon_{}_{}".format(env.module(), self.name)])
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
        def func(env, *a):
            for i, arg in enumerate(self.args):
                env.declare(arg.name, None, self.args[i].type.resolve(env).default(env) if self.args[i].mode is OUT else a[i] if i < len(a) else arg.default.eval(env))
            for s in self.statements:
                s.declare(env)
            r = None
            try:
                for s in self.statements:
                    s.run(env)
            except ReturnException as x:
                r = x.expr
            if hasattr(func, "_outs"):
                return [r] + [env.get_value(arg.name) for i, arg in enumerate(self.args) if func._outs[i]]
            else:
                return r
        outs = [x.mode is not IN for x in self.args]
        if any(outs):
            func._outs = outs
        env.declare(self.name, type, func)
    def run(self, env):
        pass

class FunctionCallExpression:
    def __init__(self, func, args):
        self.func = func
        self.args = args
    def eval(self, env):
        args = [a[1].eval(env) for a in self.args]
        f = self.func.eval(env)
        if callable(f):
            #if isinstance(self.func, DotExpression):
            #    return f(self.func.expr.eval(env), *args)
            #else:
                e = env
                while e.parent is not None:
                    e = e.parent
                funcenv = Environment(e)
                r = f(funcenv, *args)
                if hasattr(f, "_outs"):
                    for i, out in enumerate(f._outs):
                        if out:
                            self.args[i][1].set(env, r[1+i])
                    r = r[0]
                return r
        elif isinstance(f, ClassRecord):
            return f.make(env, args)
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
        self.var.set(env, self.rhs.eval(env))

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
    def __init__(self, type):
        self.type = type
    def declare(self, env):
        pass
    def run(self, env):
        if self.type is FUNCTION:
            raise ReturnException(None)
        else:
            raise ExitException(self.type)

class ForStatement:
    def __init__(self, var, start, end, step, statements):
        self.var = var
        self.start = start
        self.end = end
        self.step = step
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
                    if x.type is not FOR:
                        raise
                i += step
        except ExitException as x:
            if x.type is not FOR:
                raise
        env = env.parent

class ForeachStatement:
    def __init__(self, var, array, index, statements):
        self.var = var
        self.array = array
        self.index = index
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
                    if x.type is not FOREACH:
                        raise
        except ExitException as x:
            if x.type is not FOREACH:
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

class LoopStatement:
    def __init__(self, statements):
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
                    if x.type is not LOOP:
                        raise
        except ExitException as x:
            if x.type is not LOOP:
                raise
        env = env.parent

class NextStatement:
    def __init__(self, type):
        self.type = type
    def declare(self, env):
        pass
    def run(self, env):
        raise NextException(self.type)

class RaiseStatement:
    def __init__(self, name, info):
        self.name = name
        self.info = info
    def declare(self, env):
        pass
    def run(self, env):
        raise NeonException(self.name)

class RepeatStatement:
    def __init__(self, cond, statements):
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
                    if x.type is not REPEAT:
                        raise
                if self.cond.eval(env):
                    break
        except ExitException as x:
            if x.type is not REPEAT:
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
            for exceptions, statements in self.catches:
                if x.name in [x[1] for x in exceptions]:
                    for s in statements:
                        s.declare(env)
                    for s in statements:
                        s.run(env)
                    break
            else:
                raise
        env = env.parent

class WhileStatement:
    def __init__(self, cond, statements):
        self.cond = cond
        self.statements = statements
    def declare(self, env):
        pass
    def run(self, env):
        env = Environment(env)
        for s in self.statements:
            s.declare(env)
        try:
            while self.cond.eval(env):
                try:
                    for s in self.statements:
                        s.run(env)
                except NextException as x:
                    if x.type is not WHILE:
                        raise
        except ExitException as x:
            if x.type is not WHILE:
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
        if self.tokens[self.i] is ENUM:
            return self.parse_enum_type()
        if self.tokens[self.i] is POINTER:
            return self.parse_pointer_type()
        if self.tokens[self.i] is FUNCTION:
            return self.parse_function_type()
        name = self.identifier()
        return TypeSimple(name)

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
        # TODO: DOT
        return tuple([None, name] + list(self.parse_function_parameters()))

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
                name = None
                if isinstance(self.tokens[self.i], Identifier) and self.tokens[self.i+1] is AS:
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
        self.expect(RBRACKET)
        return ArrayLiteralExpression(elements)

    def parse_dictionary_literal(self):
        self.expect(LBRACE)
        elements = []
        while isinstance(self.tokens[self.i], String):
            key = self.tokens[self.i].value
            self.i += 1
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
            type = self.parse_type()
            return NewRecordExpression(type)
        elif t is NIL:
            self.i += 1
            return NilLiteralExpression()
        elif isinstance(t, Identifier):
            self.i += 1
            expr = IdentifierExpression(t.name)
            while True:
                if self.tokens[self.i] is LBRACKET:
                    self.i += 1
                    if self.tokens[self.i] == FIRST:
                        self.i += 1
                        if self.tokens[self.i] in [PLUS, MINUS]:
                            index = self.parse_expression()
                        else:
                            index = NumberLiteralExpression(0)
                    elif self.tokens[self.i] == LAST:
                        self.i += 1
                        index = NumberLiteralExpression(-1) # TODO: refer to last
                    else:
                        index = self.parse_expression()
                    if self.tokens[self.i] == TO:
                        self.i += 1
                        if self.tokens[self.i] == LAST:
                            self.i += 1
                            if self.tokens[self.i] in [PLUS, MINUS]:
                                last = self.parse_expression()
                            else:
                                last = NumberLiteralExpression(-1) # TODO: refer to last
                        else:
                            last = self.parse_expression()
                    self.expect(RBRACKET)
                    expr = SubscriptExpression(expr, index)
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

    def parse_membership(self):
        left = self.parse_comparison()
        if self.tokens[self.i] is IN or (self.tokens[self.i] is NOT and self.tokens[self.i+1] is IN):
            notin = self.tokens[self.i] is NOT
            if notin:
                self.i += 1
            self.i += 1
            right = self.parse_comparison()
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
        while self.tokens[self.i] is WHEN:
            self.i += 1
            conditions = []
            while True:
                t = self.tokens[self.i]
                if t is EQUAL or t is NOTEQUAL or t is LESS or t is GREATER or t is LESSEQ or t is GREATEREQ:
                    op = t
                    self.i += 1
                    when = self.parse_expression()
                    conditions.append(CaseStatement.ComparisonWhenCondition(op, when))
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
            while self.tokens[self.i] is not WHEN and self.tokens[self.i] is not ELSE and self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
                s = self.parse_statement()
                if s is not None:
                    statements.append(s)
            clauses.append((conditions, statements))
        else_statements = []
        if self.tokens[self.i] is ELSE:
            self.i += 1
            while self.tokens[self.i] is not END:
                s = self.parse_statement()
                if s is not None:
                    else_statements.append(s)
        self.expect(END)
        self.expect(CASE)
        clauses.append((None, else_statements))
        return CaseStatement(expr, clauses)

    def parse_declaration(self):
        self.expect(DECLARE)
        if self.tokens[self.i] == EXCEPTION:
            self.i += 1
            name = self.identifier()
            return ExceptionDeclaration(name)
        elif self.tokens[self.i] == NATIVE:
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

    def parse_exit_statement(self):
        self.expect(EXIT)
        type = self.tokens[self.i]
        self.i += 1
        return ExitStatement(type)

    def parse_export(self):
        self.expect(EXPORT)
        name = self.identifier()

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
        self.expect(DO)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(FOR)
        return ForStatement(var, start, end, step, statements)

    def parse_foreach_statement(self):
        self.expect(FOREACH)
        var = self.identifier()
        self.expect(OF)
        array = self.parse_expression()
        index = None
        if self.tokens[self.i] is INDEX:
            self.i += 1
            index = self.identifier()
        self.expect(DO)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(FOREACH)
        return ForeachStatement(var, array, index, statements)

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
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(LOOP)
        return LoopStatement(statements)

    def parse_next_statement(self):
        self.expect(NEXT)
        type = self.tokens[self.i]
        self.i += 1
        return NextStatement(type)

    def parse_raise_statement(self):
        self.expect(RAISE)
        name = self.identifier()
        if self.tokens[self.i] is DOT:
            self.i += 1
            assert False
        if self.tokens[self.i] is LPAREN:
            info = self.parse_function_call(None)
        else:
            info = None
        return RaiseStatement(name, info)

    def parse_repeat_statement(self):
        self.expect(REPEAT)
        statements = []
        while self.tokens[self.i] is not UNTIL and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(UNTIL)
        cond = self.parse_expression()
        return RepeatStatement(cond, statements)

    def parse_return_statement(self):
        self.expect(RETURN)
        expr = self.parse_expression()
        return ReturnStatement(expr)

    def parse_try_statement(self):
        self.expect(TRY)
        statements = []
        while self.tokens[self.i] is not EXCEPTION and self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        catches = []
        while self.tokens[self.i] is EXCEPTION:
            self.i += 1
            name = (None, self.identifier())
            if self.tokens[self.i] is DOT:
                self.i += 1
                n = self.identifier()
                name = (name[1], n)
            exceptions = [name]
            handler = []
            while self.tokens[self.i] is not EXCEPTION and self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
                s = self.parse_statement()
                if s is not None:
                    handler.append(s)
            catches.append((exceptions, handler))
        self.expect(END)
        self.expect(TRY)
        return TryStatement(statements, catches)

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
        cond = self.parse_expression()
        self.expect(DO)
        statements = []
        while self.tokens[self.i] is not END and self.tokens[self.i] is not END_OF_FILE:
            s = self.parse_statement()
            if s is not None:
                statements.append(s)
        self.expect(END)
        self.expect(WHILE)
        return WhileStatement(cond, statements)

    def parse_statement(self):
        if self.tokens[self.i] is IMPORT:   return self.parse_import()
        if self.tokens[self.i] is TYPE:     return self.parse_type_definition()
        if self.tokens[self.i] is CONSTANT: return self.parse_constant_definition()
        if self.tokens[self.i] is FUNCTION: return self.parse_function_definition()
        if self.tokens[self.i] is EXTERNAL: return self.parse_external_definition()
        if self.tokens[self.i] is DECLARE:  return self.parse_declaration()
        if self.tokens[self.i] is EXPORT:   return self.parse_export()
        if self.tokens[self.i] is IF:       return self.parse_if_statement()
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
        if isinstance(self.tokens[self.i], Identifier):
            expr = self.parse_expression()
            if self.tokens[self.i] is ASSIGN:
                self.i += 1
                rhs = self.parse_expression()
                return AssignmentStatement(expr, rhs)
            else:
                return ExpressionStatement(expr)
        else:
            assert False, self.tokens[self.i]

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
            def fromArray(self, env, a):
                self.a = list(a)
            def size(self, env):
                return len(self.a)
            def toArray(self, env):
                return list(self.a)
        return Bytes()

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
    def __init__(self, fields):
        self.fields = fields
    def default(self, env):
        r = ClassRecord.Instance(self.fields)
        for f in self.fields:
            setattr(r, f.name, f.type.resolve(env).default(env))
        return r
    def make(self, env, args):
        r = self.default(env)
        for f, a in zip(self.fields, args):
            setattr(r, f.name, a)
        return r

class ClassEnum(Class):
    class Instance:
        def __init__(self, name):
            self.name = name
        def toString(self, env):
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
        e = self
        while e is not None:
            if name in e.names:
                return e.names[name][1]
            e = e.parent
        assert False, name
    def module(self):
        return self.parent.module() if self.parent else self.module_name
    def set(self, name, value):
        e = self
        while e is not None:
            if name in e.names:
                e.names[name][1] = value
                break
            e = e.parent
        else:
            assert False, name

class ExitException:
    def __init__(self, type):
        self.type = type

class NeonException:
    def __init__(self, name):
        self.name = name

class NextException:
    def __init__(self, type):
        self.type = type

class ReturnException:
    def __init__(self, expr):
        self.expr = expr

g_Modules = {}

def import_module(name):
    m = g_Modules.get(name)
    if m is None:
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
    program.env.declare("chr", None, neon_chr)
    program.env.declare("concat", None, neon_concat)
    program.env.declare("dec", None, neon_dec)
    program.env.declare("inc", None, neon_inc)
    program.env.declare("max", None, neon_max)
    program.env.declare("min", None, neon_min)
    program.env.declare("num", None, neon_num)
    program.env.declare("ord", None, neon_ord)
    program.env.declare("print", None, neon_print)
    program.env.declare("str", None, neon_str)
    program.env.declare("strb", None, neon_strb)
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
        raise NeonException("ArrayIndex")
    if n < len(a):
        del a[int(n):]
    elif n > len(a):
        a.extend([0] * (n - len(a)))

def neon_chr(env, x):
    return unichr(x)

def neon_concat(env, x, y):
    return x + y

def neon_dec(env, x):
    neon_dec._outs = [True]
    return None, x - 1

def neon_format(env, s, fmt):
    if fmt.endswith("x"):
        return format(int(s), fmt)
    else:
        return format(s, fmt)

def neon_inc(env, x):
    neon_inc._outs = [True]
    return None, x + 1

def neon_max(env, x, y):
    return max(x, y)

def neon_min(env, x, y):
    return min(x, y)

def neon_num(env, x):
    return float(x)

def neon_ord(env, x):
    if len(x) != 1:
        raise NeonException("ArrayIndex")
    return ord(x)

def neon_print(env, x):
    print(x)

def neon_str(env, x):
    r = str(x)
    if isinstance(x, float):
        r = re.sub(r"\.0+$", "", r)
    return r

def neon_strb(env, x):
    return "TRUE" if x else "FALSE"

def neon_substring(env, s, start, length):
    return s[start:start+length]

def neon_file_copy(env, src, dest):
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
    with open(fn, "r") as f:
        return list(map(str.rstrip, f.readlines()))

def neon_file_removeEmptyDirectory(env, path):
    try:
        os.rmdir(path)
    except OSError:
        raise NeonException("FileError")

def neon_file_rename(env, old, new):
    os.rename(old, new)

def neon_file_writeBytes(env, fn, bytes):
    with open(fn, "wb") as f:
        f.write("".join(chr(x) for x in bytes.a))

def neon_file_writeLines(env, fn, lines):
    with open(fn, "wb") as f:
        f.writelines(x.encode()+"\n" for x in lines)

def neon_sys_argv(env):
    return sys.argv[1:]

def neon_sys_exit(env, n):
    sys.exit(n)

ExcludeTests = [
    "t/base.neon",              # Won't need different base literals
    "t/bigint-test.neon",       # Module not required
    "t/bitwise-test.neon",      # Module not required
    "t/bytes-embed.neon",       # Feature not required
    "t/bytes-literal.neon",     # Feature not required
    "t/cal-test.neon",          # Sample not required
    "t/cformat-test.neon",      # Module not required
    "t/comments.neon",          # Nested comments not required
    "t/complex-test.neon",      # Module not required
    "t/compress-test.neon",     # Module not required
    "t/datetime-test.neon",     # Module not required
    "t/debug-example.neon",     # Feature not required
    "t/debug-server.neon",      # Feature not required
    "t/decimal.neon",           # Module not required
    "t/exception-clear.neon",   # CURRENT_EXCEPTION not required
    "t/exception-code.neon",    # CURRENT_EXCEPTION not required
    "t/exception-current.neon", # CURRENT_EXCEPTION not required
    "t/ffi.neon",               # FFI not required
    "t/fork.neon",              # fork not required
    "t/forth-test.neon",        # Sample not required
    "t/function-namedargs.neon",# Named arguments not required
    "t/gc1.neon",               # Garbage collector not required
    "t/gc2.neon",               # Garbage collector not required
    "t/gc3.neon",               # Garbage collector not required
    "t/gc-array.neon",          # Garbage collector not required
    "t/gc-long-chain.neon",     # Garbage collector not required
    "t/http-test.neon",         # Module not required
    "t/import-string.neon",     # Feature not required
    "t/json-test.neon",         # Module not required
    "t/lexer-raw.neon",         # Feature not required
    "t/lexer-unicode.neon",     # Unicode source not required
    "t/lisp-test.neon",         # Sample not required
    "t/math-test.neon",         # Module not required
    "t/mkdir.neon",             # Feature not required
    "t/module-alias.neon",      # Feature not required
    "t/module-alias2.neon",     # Feature not required
    "t/module-import-name-alias.neon", # Feature not required
    "t/module-import-name-alias2.neon", # Feature not required
    "t/module-import-name.neon", # Feature not required
    "t/module-import-name2.neon", # Feature not required
    "t/module-import-name3.neon", # Feature not required
    "t/multiarray-test.neon",   # Module not required
    "t/net-test.neon",          # Module not required
    "t/number-ceil.neon",       # Feature not required
    "t/number-underscore.neon", # Feature not required
    "t/os-test.neon",           # Module not required
    "t/recursion-limit.neon",   # Feature not required
    "t/regex-test.neon",        # Module not required
    "t/return-case.neon",       # Feature not required
    "t/rtl.neon",               # CURRENT_EXCEPTION not required
    "t/shebang.neon",           # Feature not required
    "t/sqlite-test.neon",       # Module not required
    "t/string-multiline.neon",  # Feature not required
    "t/sudoku-test.neon",       # Sample not required
    "t/sys-exit.neon",          # CURRENT_EXCEPTION not required
    "t/tail-call.neon",         # Feature not required
    "t/time-stopwatch.neon",    # Module not required
    "t/time-test.neon",         # Module not required
    "t/variant-test.neon",      # Module not required
    "t/win32-test.neon",        # Module not required

    "t/array-slice.neon",       # Probably going to need this
    "t/array2d.neon",           # Not implemented in helium yet
    "t/file-test.neon",         # Code in module doesn't work yet
    "t/function-pointer.neon",  # Function pointer not required yet
    "t/hash-test.neon",         # Module not required yet
    "t/import.neon",            # Module import not required yet
    "t/io-test.neon",           # Module not required yet
    "t/methods-declare.neon",   # Methods not required yet
    "t/methods.neon",           # Methods not required yet
    "t/module.neon",            # Feature not required yet
    "t/module2.neon",           # Feature not required yet
    "t/pointer-method.neon",    # Feature not required yet
    "t/record-private.neon",    # Feature not required yet
    "t/string-test.neon",       # Module not required yet
    "t/strings.neon",           # Feature not required yet
    "t/struct-test.neon",       # Module not required yet
    "t/valuecopy.neon",         # Feature not required yet
]

if sys.argv[1].replace("\\", "/") in ExcludeTests:
    sys.exit(99)

try:
    source = codecs.open(sys.argv[1], encoding="utf-8").read()
except UnicodeDecodeError:
    sys.exit(99)
if re.search(r"^%!", source, re.MULTILINE):
    sys.exit(99)
run(parse(tokenize(source)))
