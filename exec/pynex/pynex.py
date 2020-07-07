#!/usr/bin/env python3

import calendar
import decimal
import math
import os
import random
import re
import shutil
import stat
import sys
import time

enable_assert = True

def is_integer(x):
    return x == int(x)

def get_vint(b, i):
    r = 0
    while True:
        x = b[i]
        i += 1
        r = (r << 7) | (x & 0x7f)
        if (x & 0x80) == 0:
            break
    return r, i

def quoted(s):
    return '"' + s.replace('"', r'\"') + '"'

class Bytes:
    def __init__(self, s):
        self.s = s
    def literal(self):
        return "HEXBYTES \"{}\"".format(" ".join("{:02x}".format(x) for x in self.s))

class Type:
    def __init__(self):
        self.name = 0
        self.descriptor = 0

class Constant:
    def __init__(self):
        self.name = 0
        self.type = 0
        self.value = 0

class Variable:
    def __init__(self):
        self.name = 0
        self.type = 0
        self.value = 0

class ExportFunction:
    def __init__(self):
        self.name = 0
        self.descriptor = 0
        self.index = 0

class ExceptionExport:
    def __init__(self):
        self.name = 0

class ExportInterface:
    def __init__(self):
        self.name = 0
        self.method_descriptors = []

class Function:
    def __init__(self):
        self.name = 0
        self.nest = 0
        self.params = 0
        self.locals = 0
        self.entry = 0

class Import:
    def __init__(self):
        self.name = 0
        self.optional = False
        self.hash = None

class ClassInfo:
    def __init__(self):
        self.name = 0
        self.interfaces = []

class Bytecode:
    def __init__(self, bytecode):
        i = 0

        assert bytecode[i:i+4] == b"Ne\0n"
        i += 4

        self.source_hash = bytecode[i:i+32]
        i += 32
        self.global_size, i = get_vint(bytecode, i)

        strtablesize, i = get_vint(bytecode, i)
        self.strtable = self.getstrtable(bytecode[i:i+strtablesize])
        i += strtablesize

        typesize, i = get_vint(bytecode, i)
        self.export_types = []
        while typesize > 0:
            t = Type()
            t.name, i = get_vint(bytecode, i)
            t.descriptor, i = get_vint(bytecode, i)
            self.export_types.append(t)
            typesize -= 1

        constantsize, i = get_vint(bytecode, i)
        self.export_constants = []
        while constantsize > 0:
            c = Constant()
            c.name, i = get_vint(bytecode, i)
            c.type, i = get_vint(bytecode, i)
            size, i = get_vint(bytecode, i)
            c.value = bytecode[i:i+size]
            i += size
            self.export_constants.append(c)
            constantsize -= 1

        variablesize, i = get_vint(bytecode, i)
        self.export_variables = []
        while variablesize > 0:
            v = Variable()
            v.name, i = get_vint(bytecode, i)
            v.type, i = get_vint(bytecode, i)
            v.index, i = get_vint(bytecode, i)
            self.export_variables.append(v)
            variablesize -= 1

        functionsize, i = get_vint(bytecode, i)
        self.export_functions = []
        while functionsize > 0:
            f = ExportFunction()
            f.name, i = get_vint(bytecode, i)
            f.descriptor, i = get_vint(bytecode, i)
            f.index, i = get_vint(bytecode, i)
            self.export_functions.append(f)
            functionsize -= 1

        exceptionexportsize, i = get_vint(bytecode, i)
        self.export_exceptions = []
        while exceptionexportsize > 0:
            e = ExceptionExport()
            e.name, i = get_vint(bytecode, i)
            self.export_exceptions.append(e)
            exceptionexportsize -= 1

        interfaceexportsize, i = get_vint(bytecode, i)
        self.export_interfaces = []
        while interfaceexportsize > 0:
            iface = ExportInterface()
            iface.name, i = get_vint(bytecode, i)
            methoddescriptorsize, i = get_vint(bytecode, i)
            while methoddescriptorsize > 0:
                # TODO
                first, i = get_vint(bytecode, i)
                second, i = get_vint(bytecode, i)
                methoddescriptorsize -= 1
            self.export_interfaces.append(iface)
            interfaceexportsize -= 1

        importsize, i = get_vint(bytecode, i)
        self.imports = []
        while importsize > 0:
            imp = Import()
            imp.name, i = get_vint(bytecode, i)
            imp.optional, i = get_vint(bytecode, i)
            imp.hash = bytecode[i:i+32]
            i += 32
            self.imports.append(imp)
            importsize -= 1

        functionsize, i = get_vint(bytecode, i)
        self.functions = []
        while functionsize > 0:
            f = Function()
            f.name, i = get_vint(bytecode, i)
            f.nest, i = get_vint(bytecode, i)
            f.params, i = get_vint(bytecode, i)
            f.locals, i = get_vint(bytecode, i)
            f.entry, i = get_vint(bytecode, i)
            self.functions.append(f)
            functionsize -= 1

        exceptionsize, i = get_vint(bytecode, i)
        self.exceptions = []
        while exceptionsize > 0:
            e = Exception()
            e.start, i = get_vint(bytecode, i)
            e.end, i = get_vint(bytecode, i)
            e.excid, i = get_vint(bytecode, i)
            e.handler, i = get_vint(bytecode, i)
            e.stack_depth, i = get_vint(bytecode, i)
            self.exceptions.append(e)
            exceptionsize -= 1

        classsize, i = get_vint(bytecode, i)
        self.classes = []
        while classsize > 0:
            c = ClassInfo()
            c.name, i = get_vint(bytecode, i)
            interfacecount, i = get_vint(bytecode, i)
            while interfacecount > 0:
                methods = []
                methodcount, i = get_vint(bytecode, i)
                while methodcount > 0:
                    m, i = get_vint(bytecode, i)
                    methods.append(m)
                    methodcount -= 1
                c.interfaces.append(methods)
                interfacecount -= 1
            self.classes.append(c)
            classsize -= 1

        self.code = bytecode[i:]

    @staticmethod
    def getstrtable(table):
        r = []
        i = 0
        while i < len(table):
            strlen, i = get_vint(table, i)
            r.append(table[i:i+strlen])
            i += strlen
        return r

def equals(a, b):
    if isinstance(a, list):
        return len(a) == len(b) and all(equals(a[x].value, b[x].value) for x in range(len(a)))
    if isinstance(a, dict):
        return len(a) == len(b) and all(k in b and v.value.equals(b[k].value) for k, v in a.items())
    return a == b

def literal(v):
    if v is None:
        return "null"
    if isinstance(v, bool):
        return "TRUE" if v else "FALSE"
    if isinstance(v, str):
        return quoted(v)
    if isinstance(v, Bytes):
        return v.literal()
    return str(v)

class Value:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return "Value({}:{})".format(id(self), repr(self.value))
    def copy(self):
        if self.value is None or isinstance(self.value, (bool, str, Bytes, decimal.Decimal)):
            return Value(self.value)
        elif isinstance(self.value, list):
            return Value([x.copy() for x in self.value])
        elif isinstance(self.value, dict):
            return Value({k: v.copy() for k, v in self.value.items()})
        else:
            return self

Globals = {
    "sys$args": Value([Value(x) for x in sys.argv[1:]]),
    "textio$stderr": Value(sys.stderr),
    "textio$stdout": Value(sys.stdout),
}

class ActivationFrame:
    def __init__(self, nesting_depth, outer, count, opstack_depth):
        self.nesting_depth = nesting_depth
        self.outer = outer
        self.locals = [Value(None) for _ in range(count)]
        self.opstack_depth = opstack_depth

class Module:
    def __init__(self, name, bytecode):
        self.name = name
        self.object = Bytecode(bytecode)
        self.globals = [Value(None) for _ in range(self.object.global_size)]

class Executor:
    def __init__(self, modulefilename, bytecode):
        self.modulefilename = modulefilename
        self.modules = {}
        self.init_order = []
        self.module = None
        self.ip = 0
        self.stack = []
        self.callstack = []
        self.frames = []

        self.import_module("", bytecode)
        self.module = self.modules[""]

    def import_module(self, name, bytecode):
        m = self.modules.get(name)
        if m is not None:
            return
        m = Module(name, bytecode)
        self.modules[name] = m
        for imp in m.object.imports:
            imported_name = m.object.strtable[imp.name].decode()
            if "/" in imported_name:
                bytes = open(imported_name + ".neonx", "rb").read()
            else:
                try:
                    bytes = open(os.path.join("lib", imported_name + ".neonx"), "rb").read()
                except FileNotFoundError:
                    bytes = open(os.path.join(os.path.dirname(self.modulefilename), imported_name + ".neonx"), "rb").read()
            self.import_module(imported_name, bytes)
        if name:
            self.init_order.insert(0, m)

    def run(self):
        self.ip = len(self.module.object.code)
        self.invoke(self.module, 0)
        for m in self.init_order:
            self.invoke(m, 0)
        while self.ip < len(self.module.object.code):
            #print(repr(self.stack)); print("ip={} op={}".format(self.ip, Dispatch[self.module.object.code[self.ip]]))
            Dispatch[self.module.object.code[self.ip]](self)

    def PUSHB(self):
        self.ip += 1
        val = self.module.object.code[self.ip]
        self.ip += 1
        self.stack.append(val != 0)

    def PUSHN(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(decimal.Decimal(self.module.object.strtable[val].decode()))

    def PUSHS(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(self.module.object.strtable[val].decode())

    def PUSHY(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(Bytes(self.module.object.strtable[val]))

    def PUSHPG(self):
        self.ip += 1
        addr, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(self.module.globals[addr])

    def PUSHPPG(self):
        self.ip += 1
        name, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(Globals[self.module.object.strtable[name].decode()])

    def PUSHPMG(self):
        self.ip += 1
        mod, self.ip = get_vint(self.module.object.code, self.ip)
        addr, self.ip = get_vint(self.module.object.code, self.ip)
        module_name = self.module.object.strtable[mod].decode()
        for m in self.modules.values():
            if m.name == module_name:
                self.stack.append(m.globals[addr])
                return
        print("module not found: {}".format(module_name), file=sys.stderr)

    def PUSHPL(self):
        self.ip += 1
        addr, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(self.frames[-1].locals[addr])

    def PUSHPOL(self):
        self.ip += 1
        back, self.ip = get_vint(self.module.object.code, self.ip)
        addr, self.ip = get_vint(self.module.object.code, self.ip)
        frame = self.frames[-1]
        while back > 0:
            frame = frame.outer
            back -= 1
        self.stack.append(frame.locals[addr])

    def PUSHI(self):
        self.ip += 1
        x, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(decimal.Decimal(x))

    def LOADB(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = False
        self.stack.append(addr.value)

    def LOADN(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = decimal.Decimal(0)
        self.stack.append(addr.value)

    def LOADS(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = ""
        self.stack.append(addr.value)

    def LOADY(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = ""
        self.stack.append(addr.value)

    def LOADA(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = []
        self.stack.append([x.copy() for x in addr.value])

    def LOADD(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = {}
        self.stack.append({k: v.copy() for k, v in addr.value.items()})

    def LOADP(self):
        self.ip += 1
        addr = self.stack.pop()
        self.stack.append(addr.value)

    def LOADJ(self):
        self.ip += 1
        addr = self.stack.pop()
        self.stack.append(addr.value)

    def LOADV(self):
        self.ip += 1
        addr = self.stack.pop()
        self.stack.append(addr.value)

    def STOREB(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STOREN(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STORES(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STOREY(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STOREA(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STORED(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STOREP(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STOREJ(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def STOREV(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value

    def NEGN(self):
        self.ip += 1
        x = self.stack.pop()
        self.stack.append(-x)

    def ADDN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a + b)

    def SUBN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a - b)

    def MULN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a * b)

    def DIVN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        if b == 0:
            self.raise_literal("DivideByZeroException", ("", 0))
            return
        self.stack.append(a / b)

    def MODN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        if b == 0:
            self.raise_literal("DivideByZeroException", ("", 0))
            return
        m = abs(b)
        if a.is_signed():
            q = decimal.Decimal(math.ceil(float(abs(a) / m)))
            a += m * q
        r = a % m
        if b.is_signed():
            r -= m
        self.stack.append(r)

    def EXPN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a ** b)

    def EQB(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a == b)

    def NEB(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a != b)

    def EQN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a == b)

    def NEN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a != b)

    def LTN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a < b)

    def GTN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a > b)

    def LEN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a <= b)

    def GEN(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a >= b)

    def EQS(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a == b)

    def NES(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a != b)

    def LTS(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a < b)

    def GTS(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a > b)

    def LES(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a <= b)

    def GES(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a >= b)

    def EQY(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a.s == b.s)

    def NEY(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a.s != b.s)

    def LTY(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a.s < b.s)

    def GTY(self):
        assert False

    def LEY(self):
        assert False

    def GEY(self):
        assert False

    def EQA(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(equals(a, b))

    def NEA(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(not equals(a, b))

    def EQD(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(equals(a, b))

    def NED(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(not equals(a, b))

    def EQP(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a is b)

    def NEP(self):
        self.ip += 1
        b = self.stack.pop()
        a = self.stack.pop()
        self.stack.append(a is not b)

    def EQV(self):
        assert False

    def NEV(self):
        assert False

    def ANDB(self):
        assert False

    def ORB(self):
        assert False

    def NOTB(self):
        self.ip += 1
        a = self.stack.pop()
        self.stack.append(not a)

    def INDEXAR(self):
        self.ip += 1
        index = self.stack.pop()
        addr = self.stack.pop().value
        if not is_integer(index) or index.is_signed() or int(index) >= len(addr):
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        self.stack.append(addr[int(index)])

    def INDEXAW(self):
        self.ip += 1
        index = self.stack.pop()
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = []
        if not is_integer(index) or index.is_signed():
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        a = addr.value
        i = int(index)
        while i >= len(a):
            a.append(Value(None))
        self.stack.append(a[i])

    def INDEXAV(self):
        self.ip += 1
        index = self.stack.pop()
        addr = self.stack.pop()
        if not is_integer(index) or index.is_signed():
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        self.stack.append(addr[int(index)].value)

    def INDEXAN(self):
        self.ip += 1
        index = self.stack.pop()
        addr = self.stack.pop()
        if not is_integer(index) or index.is_signed():
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        self.stack.append(addr[int(index)].value)

    def INDEXDR(self):
        self.ip += 1
        key = self.stack.pop()
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = {}
        d = addr.value
        if key not in d:
            self.raise_literal("DictionaryIndexException", (key, 0))
            return
        self.stack.append(d[key])

    def INDEXDW(self):
        self.ip += 1
        key = self.stack.pop()
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = {}
        d = addr.value
        if key not in d:
            d[key] = Value(None)
        self.stack.append(d[key])

    def INDEXDV(self):
        self.ip += 1
        key = self.stack.pop()
        addr = self.stack.pop()
        self.stack.append(addr[key].value)

    def INA(self):
        self.ip += 1
        a = self.stack.pop()
        v = self.stack.pop()
        self.stack.append(v in [x.value for x in a])

    def IND(self):
        self.ip += 1
        d = self.stack.pop()
        k = self.stack.pop()
        self.stack.append(k in d)

    def CALLP(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        func = self.module.object.strtable[val]
        f = globals()["neon_{}".format(func.decode().replace("$", "_"))]
        f(self)

    def CALLF(self):
        self.ip += 1
        target, self.ip = get_vint(self.module.object.code, self.ip)
        self.invoke(self.module, target)

    def CALLMF(self):
        self.ip += 1
        mod, self.ip = get_vint(self.module.object.code, self.ip)
        fun, self.ip = get_vint(self.module.object.code, self.ip)
        module_name = self.module.object.strtable[mod].decode()
        m = self.modules.get(module_name)
        if m is None:
            print("module not found: {}".format(module_name), file=sys.stderr);
            sys.exit(1)
        function_name = self.module.object.strtable[fun]
        for ef in m.object.export_functions:
            if m.object.strtable[ef.name] + b"," + m.object.strtable[ef.descriptor] == function_name:
                self.invoke(m, ef.index)
                return
        print("function not found: {}".format(function_name))
        sys.exit(1)

    def CALLI(self):
        self.ip += 1
        a = self.stack.pop()
        m = a[0].value
        index = a[1].value
        self.invoke(m, index)

    def JUMP(self):
        self.ip += 1
        target, self.ip = get_vint(self.module.object.code, self.ip)
        self.ip = target

    def JF(self):
        self.ip += 1
        target, self.ip = get_vint(self.module.object.code, self.ip)
        a = self.stack.pop()
        if not a:
            self.ip = target

    def JT(self):
        self.ip += 1
        target, self.ip = get_vint(self.module.object.code, self.ip)
        a = self.stack.pop()
        if a:
            self.ip = target

    def JFCHAIN(self):
        self.ip += 1
        target, self.ip = get_vint(self.module.object.code, self.ip)
        a = self.stack.pop()
        if not a:
            self.ip = target
            self.stack.pop()
            self.stack.append(a)

    def DUP(self):
        self.ip += 1
        self.stack.append(self.stack[-1])

    def DUPX1(self):
        self.ip += 1
        a = self.stack.pop()
        b = self.stack.pop()
        self.stack.append(a)
        self.stack.append(b)
        self.stack.append(a)

    def DROP(self):
        self.ip += 1
        self.stack.pop()

    def RET(self):
        self.frames.pop()
        (self.module, self.ip) = self.callstack.pop()

    def CONSA(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        a = []
        for _ in range(val):
            a.append(Value(self.stack.pop()))
        self.stack.append(a)

    def CONSD(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        d = {}
        for _ in range(val):
            value = self.stack.pop()
            key = self.stack.pop()
            d[key] = Value(value)
        self.stack.append(d)

    def EXCEPT(self):
        start_ip = self.ip
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        self.ip = start_ip
        a = self.stack.pop()
        info = a[0].value if len(a) >= 1 else ""
        code = a[1].value if len(a) >= 2 else decimal.Decimal(0)
        self.raise_literal(self.module.object.strtable[val].decode(), (info, code))

    def ALLOC(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append(Value([Value(None) for _ in range(val)]))

    def PUSHNIL(self):
        self.ip += 1
        self.stack.append(None)

    def RESETC(self):
        self.ip += 1
        value = self.stack.pop()
        value.value = None

    def PUSHPEG(self):
        assert False

    def JUMPTBL(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        n = self.stack.pop()
        if is_integer(n):
            i = int(n)
            if 0 <= i < val:
                self.ip += 6 * i
            else:
                self.ip += 6 * val
        else:
            self.ip += 6 * val

    def CALLX(self):
        assert False

    def SWAP(self):
        self.ip += 1
        self.stack = self.stack[:-2] + [self.stack[-1], self.stack[-2]]

    def DROPN(self):
        assert False

    def PUSHFP(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        self.stack.append([Value(self.module), Value(val)])

    def CALLV(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        pi = self.stack.pop()
        instance = pi[0].value.value
        interface_index = int(pi[1].value)
        classinfo = instance[0].value
        self.invoke(self.module, classinfo.interfaces[interface_index][val])

    def PUSHCI(self):
        self.ip += 1
        val, self.ip = get_vint(self.module.object.code, self.ip)
        if "." not in self.module.object.strtable[val].decode():
            for c in self.module.object.classes:
                if c.name == val:
                    self.stack.append(c)
                    return
        else:
            assert False
        print("neon: unknown class name {0}".format(self.module.object.strtable[val].decode()), file=sys.stderr)
        sys.exit(1)

    def invoke(self, module, index):
        self.callstack.append((self.module, self.ip))
        outer = None
        nest = module.object.functions[index].nest
        params = module.object.functions[index].params
        locals = module.object.functions[index].locals
        if self.frames:
            assert nest <= self.frames[-1].nesting_depth + 1
            outer = self.frames[-1]
            while outer is not None and nest <= outer.nesting_depth:
                assert outer.outer is None or outer.nesting_depth == outer.outer.nesting_depth + 1
                outer = outer.outer
        self.frames.append(ActivationFrame(nest, outer, locals, len(self.stack) - params))
        self.module = module
        self.ip = self.module.object.functions[index].entry

    def raise_literal(self, name, info):
        exceptionvar = [
            Value(name),
            Value(info[0]),
            Value(info[1]),
            Value(decimal.Decimal(self.ip))
        ]

        tmodule = self.module
        tip = self.ip
        sp = len(self.callstack)
        while True:
            for e in tmodule.object.exceptions:
                if e.start <= tip < e.end:
                    handler = tmodule.object.strtable[e.excid].decode()
                    if name == handler or (len(name) > len(handler) and name.startswith(handler) and name[len(handler)] == "."):
                        self.module = tmodule
                        self.ip = e.handler
                        while len(self.stack) > (self.frames[-1].opstack_depth if self.frames else 0) + e.stack_depth:
                            self.stack.pop()
                        self.callstack[sp:] = []
                        self.stack.append(exceptionvar)
                        return
            if sp == 0:
                break
            sp -= 1
            if self.frames:
                self.frames.pop()
            (tmodule, tip) = self.callstack[sp]

        print("Unhandled exception {} ({}) (code {})".format(name, info[0], info[1]), file=sys.stderr)
        sys.exit(1)

Dispatch = [
    Executor.PUSHB,
    Executor.PUSHN,
    Executor.PUSHS,
    Executor.PUSHY,
    Executor.PUSHPG,
    Executor.PUSHPPG,
    Executor.PUSHPMG,
    Executor.PUSHPL,
    Executor.PUSHPOL,
    Executor.PUSHI,
    Executor.LOADB,
    Executor.LOADN,
    Executor.LOADS,
    Executor.LOADY,
    Executor.LOADA,
    Executor.LOADD,
    Executor.LOADP,
    Executor.LOADJ,
    Executor.LOADV,
    Executor.STOREB,
    Executor.STOREN,
    Executor.STORES,
    Executor.STOREY,
    Executor.STOREA,
    Executor.STORED,
    Executor.STOREP,
    Executor.STOREJ,
    Executor.STOREV,
    Executor.NEGN,
    Executor.ADDN,
    Executor.SUBN,
    Executor.MULN,
    Executor.DIVN,
    Executor.MODN,
    Executor.EXPN,
    Executor.EQB,
    Executor.NEB,
    Executor.EQN,
    Executor.NEN,
    Executor.LTN,
    Executor.GTN,
    Executor.LEN,
    Executor.GEN,
    Executor.EQS,
    Executor.NES,
    Executor.LTS,
    Executor.GTS,
    Executor.LES,
    Executor.GES,
    Executor.EQY,
    Executor.NEY,
    Executor.LTY,
    Executor.GTY,
    Executor.LEY,
    Executor.GEY,
    Executor.EQA,
    Executor.NEA,
    Executor.EQD,
    Executor.NED,
    Executor.EQP,
    Executor.NEP,
    Executor.EQV,
    Executor.NEV,
    Executor.ANDB,
    Executor.ORB,
    Executor.NOTB,
    Executor.INDEXAR,
    Executor.INDEXAW,
    Executor.INDEXAV,
    Executor.INDEXAN,
    Executor.INDEXDR,
    Executor.INDEXDW,
    Executor.INDEXDV,
    Executor.INA,
    Executor.IND,
    Executor.CALLP,
    Executor.CALLF,
    Executor.CALLMF,
    Executor.CALLI,
    Executor.JUMP,
    Executor.JF,
    Executor.JT,
    Executor.JFCHAIN,
    Executor.DUP,
    Executor.DUPX1,
    Executor.DROP,
    Executor.RET,
    Executor.CONSA,
    Executor.CONSD,
    Executor.EXCEPT,
    Executor.ALLOC,
    Executor.PUSHNIL,
    Executor.RESETC,
    Executor.PUSHPEG,
    Executor.JUMPTBL,
    Executor.CALLX,
    Executor.SWAP,
    Executor.DROPN,
    Executor.PUSHFP,
    Executor.CALLV,
    Executor.PUSHCI,
]

def neon_array__append(self):
    v = self.stack.pop()
    a = self.stack.pop().value
    a.append(Value(v))

def neon_array__concat(self):
    b = self.stack.pop()
    a = self.stack.pop()
    self.stack.append(a + b)

def neon_array__extend(self):
    v = self.stack.pop()
    a = self.stack.pop().value
    a.extend(v)

def neon_array__range(self):
    step = self.stack.pop()
    last = self.stack.pop()
    first = self.stack.pop()
    r = []
    if step < 0:
        i = first
        while i >= last:
            r.append(Value(i))
            i += step
    else:
        i = first
        while i <= last:
            r.append(Value(i))
            i += step
    self.stack.append(r)

def neon_array__remove(self):
    index = self.stack.pop()
    a = self.stack.pop().value
    if not is_integer(index):
        self.raise_literal("ArrayIndexException", (str(index), 0))
        return
    del a[int(index)]

def neon_array__resize(self):
    size = self.stack.pop()
    a = self.stack.pop().value
    if not is_integer(size):
        self.raise_literal("ArrayIndexException", (str(size), 0))
        return
    size = int(size)
    if size < len(a):
        a[size:] = []
    else:
        while len(a) < size:
            a.append(Value(None))

def neon_array__size(self):
    a = self.stack.pop()
    self.stack.append(decimal.Decimal(len(a)))

def neon_array__slice(self):
    last_from_end = self.stack.pop()
    last = int(self.stack.pop())
    first_from_end = self.stack.pop()
    first = int(self.stack.pop())
    a = self.stack.pop()
    if first_from_end:
        first += len(a) - 1
    if last_from_end:
        last += len(a) - 1
    self.stack.append(a[first:last+1])

def neon_array__splice(self):
    last_from_end = self.stack.pop()
    last = int(self.stack.pop())
    first_from_end = self.stack.pop()
    first = int(self.stack.pop())
    a = self.stack.pop()
    b = self.stack.pop()
    if first_from_end:
        first += len(a) - 1
    if last_from_end:
        last += len(a) - 1
    r = a[:first] + b + a[last+1:]
    self.stack.append(r)

def neon_array__toBytes__number(self):
    a = self.stack.pop()
    self.stack.append(Bytes(bytearray(int(x.value) for x in a)))

def neon_array__toString__number(self):
    a = self.stack.pop()
    self.stack.append("[{}]".format(", ".join(str(x.value) for x in a)))

def neon_array__toString__object(self):
    a = self.stack.pop()
    self.stack.append("[{}]".format(", ".join(str(x.value) for x in a)))

def neon_array__toString__string(self):
    a = self.stack.pop()
    self.stack.append("[{}]".format(", ".join(quoted(x.value) for x in a)))

def neon_boolean__toString(self):
    x = self.stack.pop()
    self.stack.append("TRUE" if x else "FALSE")

def neon_bytes__concat(self):
    b = self.stack.pop()
    a = self.stack.pop()
    self.stack.append(Bytes(a.s + b.s))

def neon_bytes__decodeToString(self):
    b = self.stack.pop()
    self.stack.append(b.s.decode())

def neon_bytes__range(self):
    last_from_end = self.stack.pop()
    last = int(self.stack.pop())
    first_from_end = self.stack.pop()
    first = int(self.stack.pop())
    b = self.stack.pop()
    if first_from_end:
        first += len(b.s) - 1
    if last_from_end:
        last += len(b.s) - 1
    self.stack.append(Bytes(b.s[first:last+1]))

def neon_bytes__size(self):
    b = self.stack.pop()
    self.stack.append(decimal.Decimal(len(b.s)))

def neon_bytes__splice(self):
    last_from_end = self.stack.pop()
    last = int(self.stack.pop())
    first_from_end = self.stack.pop()
    first = int(self.stack.pop())
    a = self.stack.pop()
    b = self.stack.pop()
    if first_from_end:
        first += len(a.s) - 1
    if last_from_end:
        last += len(a.s) - 1
    r = a.s[:first] + b.s + a.s[last+1:]
    self.stack.append(Bytes(r))

def neon_bytes__toArray(self):
    b = self.stack.pop()
    self.stack.append([Value(decimal.Decimal(x)) for x in b.s])

def neon_bytes__toString(self):
    b = self.stack.pop()
    self.stack.append("HEXBYTES \"{}\"".format(" ".join("{:02x}".format(x) for x in b.s)))

def neon_console_input(self):
    prompt = self.stack.pop()
    s = input(prompt)
    self.stack.append(s)

def neon_datetime_gmtime(self):
    t = int(self.stack.pop())
    tm = time.gmtime(t)
    r = [
        tm.tm_sec,
        tm.tm_min,
        tm.tm_hour,
        tm.tm_mday,
        tm.tm_mon - 1,
        tm.tm_year - 1900,
        tm.tm_wday + 1,
        tm.tm_yday,
        tm.tm_isdst,
    ]
    self.stack.append([Value(x) for x in r])

def neon_datetime_timegm(self):
    a = self.stack.pop()
    a = [x.value for x in a]
    tm = time.struct_time([1900 + a[5], 1 + a[4], a[3], a[2], a[1], a[0], 0, 0, 0])
    r = calendar.timegm(tm)
    self.stack.append(r)

def neon_dictionary__keys(self):
    d = self.stack.pop()
    self.stack.append([Value(x) for x in sorted(d.keys())])

def neon_exceptiontype__toString(self):
    ei = self.stack.pop()
    self.stack.append("<ExceptionType:{},{},{},{}>".format(ei[0].value, ei[1].value, ei[2].value, ei[3].value))

def neon_num(self):
    s = self.stack.pop()
    try:
        self.stack.append(decimal.Decimal(s))
    except decimal.InvalidOperation:
        self.raise_literal("ValueRangeException", (s, 0))
        return

def neon_number__toString(self):
    neon_str(self)

def neon_object__getArray(self):
    v = self.stack.pop()
    if not isinstance(v, list):
        self.raise_literal("DynamicConversionException", ("to Array", 0))
        return
    self.stack.append(v)

def neon_object__getBoolean(self):
    v = self.stack.pop()
    if not isinstance(v, bool):
        self.raise_literal("DynamicConversionException", ("to Boolean", 0))
        return
    self.stack.append(v)

def neon_object__getBytes(self):
    v = self.stack.pop()
    if not isinstance(v, Bytes):
        self.raise_literal("DynamicConversionException", ("to Bytes", 0))
        return
    self.stack.append(v)

def neon_object__getDictionary(self):
    v = self.stack.pop()
    if not isinstance(v, dict):
        self.raise_literal("DynamicConversionException", ("to Dictionary", 0))
        return
    self.stack.append(v)

def neon_object__getNumber(self):
    v = self.stack.pop()
    if not isinstance(v, decimal.Decimal):
        self.raise_literal("DynamicConversionException", ("to Number", 0))
        return
    self.stack.append(v)

def neon_object__getString(self):
    v = self.stack.pop()
    if not isinstance(v, str):
        self.raise_literal("DynamicConversionException", ("to String", 0))
        return
    self.stack.append(v)

def neon_object__isNull(self):
    v = self.stack.pop()
    self.stack.append(v is None)

def neon_object__makeArray(self):
    v = self.stack.pop()
    self.stack.append(v)

def neon_object__makeBoolean(self):
    v = self.stack.pop()
    self.stack.append(v)

def neon_object__makeBytes(self):
    v = self.stack.pop()
    self.stack.append(v)

def neon_object__makeDictionary(self):
    v = self.stack.pop()
    self.stack.append(v)

def neon_object__makeNull(self):
    self.stack.append(None)

def neon_object__makeNumber(self):
    v = self.stack.pop()
    self.stack.append(v)

def neon_object__makeString(self):
    v = self.stack.pop()
    self.stack.append(v)

def neon_object__subscript(self):
    i = self.stack.pop()
    v = self.stack.pop()
    if v is None:
        self.raise_literal("DynamicConversionException", ("object is null", 0))
    elif i is None:
        self.raise_literal("DynamicConversionException", ("index is null", 0))
    elif isinstance(v, list):
        try:
            ii = math.trunc(i)
        except:
            self.raise_literal("DynamicConversionException", ("to Number", 0))
            return
        try:
            self.stack.append(v[ii].value)
        except IndexError:
            self.raise_literal("ArrayIndexException", (str(ii), 0))
            return
    elif isinstance(v, dict):
        if not isinstance(i, str):
            self.raise_literal("DynamicConversionException", ("to String", 0))
            return
        if not i in v:
            self.raise_literal("ObjectSubscriptException", ('"' + i + '"', 0))
            return
        self.stack.append(v[i].value)
    else:
        self.raise_literal("ObjectSubscriptException", (i, 0))

def neon_object__toString(self):
    v = self.stack.pop()
    if isinstance(v, list):
        self.stack.append("[{}]".format(", ".join(literal(x.value) for x in v)))
    elif isinstance(v, dict):
        self.stack.append("{{{}}}".format(", ".join("{}: {}".format(quoted(k), literal(x.value)) for k, x in sorted(v.items()))))
    else:
        self.stack.append(literal(v))

def neon_pointer__toString(self):
    v = self.stack.pop()
    self.stack.append("<p:{}>".format(id(v)))

def neon_print(self):
    s = self.stack.pop()
    print(s)

def neon_str(self):
    v = self.stack.pop()
    if isinstance(v, decimal.Decimal):
        # Remove trailing zeros and decimal point if possible.
        v = str(v)
        if "." in v:
            v = re.sub("\\.0+(?![\\d])", "", v)
            v = re.sub("(\\.[\\d]+?)0+(?![\\d])", "\\1", v)
    self.stack.append(str(v))

def neon_string__append(self):
    b = self.stack.pop()
    a = self.stack.pop()
    a.value = a.value + b

def neon_string__concat(self):
    b = self.stack.pop()
    a = self.stack.pop()
    self.stack.append(a + b)

def neon_string__length(self):
    s = self.stack.pop()
    self.stack.append(decimal.Decimal(len(s)))

def neon_string__splice(self):
    last_from_end = self.stack.pop()
    last = int(self.stack.pop())
    first_from_end = self.stack.pop()
    first = int(self.stack.pop())
    a = self.stack.pop()
    b = self.stack.pop()
    if first_from_end:
        first += len(a) - 1
    if last_from_end:
        last += len(a) - 1
    r = a[:first] + b + a[last+1:]
    self.stack.append(r)

def neon_string__substring(self):
    last_from_end = self.stack.pop()
    last = int(self.stack.pop())
    first_from_end = self.stack.pop()
    first = int(self.stack.pop())
    s = self.stack.pop()
    if first_from_end:
        first += len(s) - 1
    if last_from_end:
        last += len(s) - 1
    self.stack.append(s[first:last+1])

def neon_string__toBytes(self):
    s = self.stack.pop()
    self.stack.append(Bytes(s))

def neon_substring(self):
    length = int(self.stack.pop())
    offset = int(self.stack.pop())
    s = self.stack.pop().value
    self.stack.append(s[offset:offset+length])

def neon_file__CONSTANT_Separator(self):
    self.stack.append(os.sep)

def neon_file_copy(self):
    dest = self.stack.pop()
    src = self.stack.pop()
    try:
        destf = open(dest, "xb")
    except FileExistsError:
        self.raise_literal("FileException.Exists", (dest, 0))
        return
    srcf = open(src, "rb")
    shutil.copyfileobj(srcf, destf)
    destf.close()
    srcf.close()

def neon_file_copyOverwriteIfExists(self):
    dest = self.stack.pop()
    src = self.stack.pop()
    shutil.copyfile(src, dest)

def neon_file_delete(self):
    fn = self.stack.pop()
    try:
        os.remove(fn)
    except FileNotFoundError:
        pass

def neon_file_exists(self):
    n = self.stack.pop()
    self.stack.append(os.path.exists(n))

def neon_file_files(self):
    path = self.stack.pop()
    self.stack.append([Value(x) for x in os.listdir(path)])

def neon_file_getInfo(self):
    fn = self.stack.pop()
    st = os.stat(fn)
    r = [
        os.path.basename(fn),
        decimal.Decimal(st.st_size),
        (st.st_mode & stat.S_IRUSR) != 0,
        (st.st_mode & stat.S_IWUSR) != 0,
        (st.st_mode & stat.S_IXUSR) != 0,
        decimal.Decimal(0 if stat.S_ISREG(st.st_mode) else 1 if stat.S_ISDIR(st.st_mode) else 2),
        st.st_ctime,
        st.st_atime,
        st.st_mtime,
    ]
    self.stack.append([Value(x) for x in r])

def neon_file_isDirectory(self):
    fn = self.stack.pop()
    try:
        st = os.stat(fn)
        self.stack.append(stat.S_ISDIR(st.st_mode))
    except FileNotFoundError:
        self.stack.append(False)

def neon_file_mkdir(self):
    fn = self.stack.pop()
    try:
        os.mkdir(fn)
    except FileExistsError:
        self.raise_literal("FileException.DirectoryExists", (fn, 0))

def neon_file_readBytes(self):
    fn = self.stack.pop()
    r = open(fn, "rb").read()
    self.stack.append(Bytes(r))

def neon_file_readLines(self):
    fn = self.stack.pop()
    r = [Value(x.rstrip("\n")) for x in open(fn).readlines()]
    self.stack.append(r)

def neon_file_removeEmptyDirectory(self):
    fn = self.stack.pop()
    try:
        os.rmdir(fn)
    except OSError:
        self.raise_literal("FileException", ("", 0))

def neon_file_rename(self):
    newname = self.stack.pop()
    oldname = self.stack.pop()
    os.rename(oldname, newname)

def neon_file_symlink(self):
    targetIsDirectory = self.stack.pop()
    newlink = self.stack.pop()
    target = self.stack.pop()
    try:
        os.symlink(target, newlink, targetIsDirectory)
    except OSError:
        self.raise_literal("FileException.Open", ("", 0))

def neon_file_writeBytes(self):
    data = self.stack.pop()
    fn = self.stack.pop()
    with open(fn, "wb") as f:
        f.write(data.s)

def neon_file_writeLines(self):
    data = self.stack.pop()
    fn = self.stack.pop()
    with open(fn, "w") as f:
        for s in data:
            print(s.value, file=f)

def neon_math_abs(self):
    x = self.stack.pop()
    self.stack.append(abs(x))

def neon_math_acos(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.acos(float(x))))

def neon_math_acosh(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.acosh(float(x))))

def neon_math_asin(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.asin(float(x))))

def neon_math_asinh(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.asinh(float(x))))

def neon_math_atan(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.atan(float(x))))

def neon_math_atan2(self):
    y = self.stack.pop()
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.atan2(float(x), float(y))))

def neon_math_atanh(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.atanh(float(x))))

def neon_math_cbrt(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(float(x) ** (1/3.0)))

def neon_math_ceil(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.ceil(float(x))))

def neon_math_cos(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.cos(float(x))))

def neon_math_cosh(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.cosh(float(x))))

def neon_math_erf(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.erf(float(x))))

def neon_math_erfc(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.erfc(float(x))))

def neon_math_exp(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.exp(float(x))))

def neon_math_exp2(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(2 ** float(x)))

def neon_math_expm1(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.expm1(float(x))))

def neon_math_floor(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.floor(float(x))))

def neon_math_frexp(self):
    x = self.stack.pop()
    (m, e) = math.frexp(float(x))
    self.stack.append(decimal.Decimal(e))
    self.stack.append(decimal.Decimal(m))

def neon_math_hypot(self):
    y = self.stack.pop()
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.hypot(float(x), float(y))))

def neon_math_intdiv(self):
    y = self.stack.pop()
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(x // y))

def neon_math_ldexp(self):
    i = self.stack.pop()
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.ldexp(float(x), int(i))))

def neon_math_lgamma(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.lgamma(float(x))))

def neon_math_log(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.log(float(x))))

def neon_math_log10(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.log10(float(x))))

def neon_math_log1p(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.log1p(float(x))))

def neon_math_log2(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.log(float(x))/math.log(2)))

def neon_math_max(self):
    b = self.stack.pop()
    a = self.stack.pop()
    self.stack.append(max(a, b))

def neon_math_min(self):
    b = self.stack.pop()
    a = self.stack.pop()
    self.stack.append(min(a, b))

def neon_math_nearbyint(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(round(float(x))))

def neon_math_odd(self):
    v = self.stack.pop()
    if v != int(v):
        self.raise_literal("ValueRangeException", ("odd() requires integer", 0))
        return
    self.stack.append((v % 2) != 0)

def neon_math_round(self):
    value = self.stack.pop()
    places = self.stack.pop()
    if places == 0:
        self.stack.append(decimal.Decimal(int(value)))
    else:
        self.stack.append(value.quantize(decimal.Decimal("1."+("0"*int(places)))).normalize())

def neon_math_sign(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(0 if x == 0 else -1 if x.is_signed() else 1))

def neon_math_sin(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.sin(float(x))))

def neon_math_sinh(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.sinh(float(x))))

def neon_math_sqrt(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.sqrt(float(x))))

def neon_math_tan(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.tan(float(x))))

def neon_math_tanh(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.tanh(float(x))))

def neon_math_tgamma(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.gamma(float(x))))

def neon_math_trunc(self):
    x = self.stack.pop()
    self.stack.append(decimal.Decimal(math.trunc(float(x))))

def neon_os_system(self):
    s = self.stack.pop()
    self.stack.append(decimal.Decimal(os.system(s)))

def neon_posix_fork(self):
    self.stack.append(decimal.Decimal(os.fork()))

def neon_random_uint32(self):
    self.stack.append(decimal.Decimal(random.randrange(0x100000000)))

def neon_runtime_assertionsEnabled(self):
    self.stack.append(Value(enable_assert))

def neon_runtime_executorName(self):
    self.stack.append("pynex")

def neon_runtime_moduleIsMain(self):
    self.stack.append(self.module is self.modules[""])

def neon_string_find(self):
    t = self.stack.pop()
    s = self.stack.pop()
    self.stack.append(decimal.Decimal(s.find(t)))

def neon_string_hasPrefix(self):
    prefix = self.stack.pop()
    s = self.stack.pop()
    self.stack.append(s.startswith(prefix))

def neon_string_hasSuffix(self):
    prefix = self.stack.pop()
    s = self.stack.pop()
    self.stack.append(s.endswith(prefix))

def neon_string_fromCodePoint(self):
    n = self.stack.pop()
    if n != int(n):
        self.raise_literal("ValueRangeException", ("fromCodePoint() argument not an integer", 0))
        return
    if not (0 <= n <= 0x10ffff):
        self.raise_literal("ValueRangeException", ("fromCodePoint() argument out of range 0-0x10ffff", 0))
        return
    self.stack.append(chr(int(n)))

def neon_string_join(self):
    d = self.stack.pop()
    a = self.stack.pop()
    self.stack.append(d.join(x.value for x in a))

def neon_string_lower(self):
    s = self.stack.pop()
    self.stack.append(s.lower())

def neon_string_split(self):
    d = self.stack.pop()
    s = self.stack.pop()
    self.stack.append([Value(x) for x in s.split(d)])

def neon_string_splitLines(self):
    s = self.stack.pop()
    if s == "":
        self.stack.append([])
    else:
        r = [Value(x) for x in re.split("\r?\n", s)]
        if s.endswith("\n"):
            r[-1:] = []
        self.stack.append(r)

def neon_string_toCodePoint(self):
    s = self.stack.pop()
    if len(s) != 1:
        self.raise_literal("ArrayIndexException", ("toCodePoint() requires string of length 1", 0))
        return
    self.stack.append(decimal.Decimal(ord(s)))

def neon_string_trimCharacters(self):
    trailing = self.stack.pop()
    leading = self.stack.pop()
    s = self.stack.pop()
    self.stack.append(s.lstrip(leading).rstrip(trailing))

def neon_string_upper(self):
    s = self.stack.pop()
    self.stack.append(s.upper())

def neon_sys_exit(self):
    x = self.stack.pop()
    if not is_integer(x) or x < 0 or x > 255:
        self.raise_literal("InvalidValueException", ("sys.exit invalid parameter: {}".format(x), 0))
        return
    sys.exit(int(x))

def neon_textio_close(self):
    f = self.stack.pop()
    f.close()

def neon_textio_open(self):
    mode = int(self.stack.pop())
    fn = self.stack.pop()
    try:
        f = open(fn, "r" if mode == 0 else "w")
        self.stack.append(f)
    except FileNotFoundError:
        self.raise_literal("TextioException.Open", fn)

def neon_textio_readLine(self):
    f = self.stack.pop()
    try:
        s = f.readline().rstrip("\n")
        self.stack.append(True)
        self.stack.append(s)
    except IOError:
        self.raise_literal("TextioException")

def neon_textio_writeLine(self):
    s = self.stack.pop()
    f = self.stack.pop()
    f.write(s + "\n")

def neon_time_now(self):
    self.stack.append(decimal.Decimal(time.time()))

Executor(sys.argv[1], open(sys.argv[1], "rb").read()).run()
