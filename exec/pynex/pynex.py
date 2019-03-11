from __future__ import print_function

import decimal
import math
import os
import re
import sys
import time

enable_assert = True

def is_integer(x):
    return x == int(x)

def get_vint(b, i):
    r = 0
    while True:
        x = ord(b[i])
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
        return "HEXBYTES \"{}\"".format(" ".join("{:02x}".format(ord(x)) for x in self.s))

class Type:
    def __init__(self):
        self.name = 0
        self.descriptor = 0

class Constant:
    def __init__(self):
        self.name = 0
        self.type = 0
        self.value = 0

class ExportFunction:
    def __init__(self):
        self.name = 0
        self.descriptor = 0
        self.index = 0

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
        self.hash = None

class Bytecode:
    def __init__(self, bytecode):
        i = 0
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
        while interfaceexportsize > 0:
            assert False, interfaceexportsize

        importsize, i = get_vint(bytecode, i)
        self.imports = []
        while importsize > 0:
            imp = Import()
            imp.name, i = get_vint(bytecode, i)
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
        while classsize > 0:
            assert False, classsize

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

class Value:
    def __init__(self, value):
        self.value = value
    def __repr__(self):
        return "Value({})".format(repr(self.value))
    def copy(self):
        if self.value is None or isinstance(self.value, (bool, str, unicode, Bytes, decimal.Decimal)):
            return Value(self.value)
        elif isinstance(self.value, list):
            return Value([x.copy() for x in self.value])
        elif isinstance(self.value, dict):
            return Value({k: v.copy() for k, v in self.value.items()})
        else:
            assert False, " ".join([str(type(self.value)), repr(self.value)])
    def literal(self):
        if isinstance(self.value, str):
            return quoted(self.value)
        else:
            return str(self.value)

Globals = {
    "io$stderr": Value(None),
}

class Executor:
    def __init__(self, bytecode):
        self.object = Bytecode(bytecode)
        self.ip = 0
        self.stack = []
        self.callstack = []
        self.frames = []
        self.globals = [None] * self.object.global_size
        for i in range(len(self.globals)):
            self.globals[i] = Value(None)

    def run(self):
        self.ip = len(self.object.code)
        self.invoke(None, 0)
        while self.ip < len(self.object.code):
            #print("ip={}".format(self.ip)); print(repr(self.stack))
            Dispatch[ord(self.object.code[self.ip])](self)

    def PUSHB(self):
        self.ip += 1
        val = self.object.code[self.ip]
        self.ip += 1
        self.stack.append(Value(ord(val) != 0))

    def PUSHN(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(Value(decimal.Decimal(self.object.strtable[val])))

    def PUSHS(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(Value(self.object.strtable[val]))

    def PUSHY(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(Value(Bytes(self.object.strtable[val])))

    def PUSHPG(self):
        self.ip += 1
        addr, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(self.globals[addr])

    def PUSHPPG(self):
        self.ip += 1
        name, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(Globals[self.object.strtable[name]])

    def PUSHPMG(self):
        assert False

    def PUSHPL(self):
        self.ip += 1
        addr, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(self.frames[-1][addr])

    def PUSHPOL(self):
        assert False

    def PUSHI(self):
        self.ip += 1
        x, self.ip = get_vint(self.object.code, self.ip)
        self.stack.append(Value(decimal.Decimal(x)))

    def LOADB(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = False
        self.stack.append(addr.copy())

    def LOADN(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = decimal.Decimal(0)
        self.stack.append(addr.copy())

    def LOADS(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = ""
        self.stack.append(addr.copy())

    def LOADY(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = ""
        self.stack.append(addr.copy())

    def LOADA(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = []
        self.stack.append(addr.copy())

    def LOADD(self):
        self.ip += 1
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = {}
        self.stack.append(addr.copy())

    def LOADP(self):
        self.ip += 1
        addr = self.stack.pop()
        self.stack.append(addr.copy())

    def LOADJ(self):
        self.ip += 1
        addr = self.stack.pop()
        self.stack.append(addr.copy())

    def STOREB(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STOREN(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STORES(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STOREY(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STOREA(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STORED(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STOREP(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def STOREJ(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop()
        addr.value = value.value

    def NEGN(self):
        self.ip += 1
        x = self.stack.pop().value
        self.stack.append(Value(-x))

    def ADDN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a + b))

    def SUBN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a - b))

    def MULN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a * b))

    def DIVN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        if b == 0:
            self.raise_literal("DivideByZeroException", ("", 0))
            return
        self.stack.append(Value(a / b))

    def MODN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
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
        self.stack.append(Value(r))

    def EXPN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a ** b))

    def EQB(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a == b))

    def NEB(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a != b))

    def EQN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a == b))

    def NEN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a != b))

    def LTN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a < b))

    def GTN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a > b))

    def LEN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a <= b))

    def GEN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a >= b))

    def EQS(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a == b))

    def NES(self):
        assert False

    def LTS(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a < b))

    def GTS(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a > b))

    def LES(self):
        assert False

    def GES(self):
        assert False

    def EQY(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a.s == b.s))

    def NEY(self):
        assert False

    def LTY(self):
        assert False

    def GTY(self):
        assert False

    def LEY(self):
        assert False

    def GEY(self):
        assert False

    def EQA(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        # TODO: equality check needs to recurse
        self.stack.append(Value(len(a) == len(b) and all(a[x].value == b[x].value for x in range(len(a)))))

    def NEA(self):
        assert False

    def EQD(self):
        assert False

    def NED(self):
        assert False

    def EQP(self):
        assert False

    def NEP(self):
        assert False

    def ANDB(self):
        assert False

    def ORB(self):
        assert False

    def NOTB(self):
        self.ip += 1
        a = self.stack.pop().value
        self.stack.append(Value(not a))

    def INDEXAR(self):
        self.ip += 1
        index = self.stack.pop().value
        addr = self.stack.pop().value
        if not is_integer(index) or index.is_signed() or int(index) >= len(addr):
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        self.stack.append(addr[int(index)])

    def INDEXAW(self):
        self.ip += 1
        index = self.stack.pop().value
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
        index = self.stack.pop().value
        addr = self.stack.pop().value
        if not is_integer(index) or index.is_signed():
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        self.stack.append(addr[int(index)].copy())

    def INDEXAN(self):
        self.ip += 1
        index = self.stack.pop().value
        addr = self.stack.pop().value
        if not is_integer(index):
            self.raise_literal("ArrayIndexException", (str(index), 0))
            return
        self.stack.append(addr[int(index)])

    def INDEXDR(self):
        self.ip += 1
        key = self.stack.pop().value
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
        key = self.stack.pop().value
        addr = self.stack.pop()
        if addr.value is None:
            addr.value = {}
        d = addr.value
        if key not in d:
            d[key] = Value(None)
        self.stack.append(d[key])

    def INDEXDV(self):
        self.ip += 1
        key = self.stack.pop().value
        addr = self.stack.pop().value
        self.stack.append(addr[key])

    def INA(self):
        self.ip += 1
        a = self.stack.pop().value
        v = self.stack.pop().value
        self.stack.append(Value(v in [x.value for x in a]))

    def IND(self):
        self.ip += 1
        d = self.stack.pop().value
        k = self.stack.pop().value
        self.stack.append(Value(k in d))

    def CALLP(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        func = self.object.strtable[val]
        f = globals()["neon_{}".format(func.replace("$", "_"))]
        f(self)

    def CALLF(self):
        self.ip += 1
        target, self.ip = get_vint(self.object.code, self.ip)
        self.invoke(None, target)

    def CALLMF(self):
        assert False

    def CALLI(self):
        assert False

    def JUMP(self):
        self.ip += 1
        target, self.ip = get_vint(self.object.code, self.ip)
        self.ip = target

    def JF(self):
        self.ip += 1
        target, self.ip = get_vint(self.object.code, self.ip)
        a = self.stack.pop().value
        if not a:
            self.ip = target

    def JT(self):
        self.ip += 1
        target, self.ip = get_vint(self.object.code, self.ip)
        a = self.stack.pop().value
        if a:
            self.ip = target

    def JFCHAIN(self):
        self.ip += 1
        target, self.ip = get_vint(self.object.code, self.ip)
        a = self.stack.pop().value
        if not a:
            self.ip = target
            self.stack.pop()
            self.stack.append(Value(a))

    def DUP(self):
        self.ip += 1
        self.stack.append(self.stack[-1].copy())

    def DUPX1(self):
        self.ip += 1
        a = self.stack.pop()
        b = self.stack.pop()
        self.stack.append(a.copy())
        self.stack.append(b.copy())
        self.stack.append(a.copy())

    def DROP(self):
        self.ip += 1
        self.stack.pop()

    def RET(self):
        self.frames.pop()
        (_module, self.ip) = self.callstack.pop()

    def CALLE(self):
        assert False

    def CONSA(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        a = []
        for _ in range(val):
            a.append(self.stack.pop().copy())
        self.stack.append(Value(a))

    def CONSD(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        d = {}
        for _ in range(val):
            value = self.stack.pop()
            key = self.stack.pop().value
            d[key] = value
        self.stack.append(Value(d))

    def EXCEPT(self):
        start_ip = self.ip
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        self.ip = start_ip
        a = self.stack.pop().value
        info = a[0].value if len(a) >= 1 else ""
        code = a[1].value if len(a) >= 2 else decimal.Decimal(0)
        self.raise_literal(self.object.strtable[val], (info, code))

    def ALLOC(self):
        assert False

    def PUSHNIL(self):
        self.ip += 1
        self.stack.append(Value(None))

    def JNASSERT(self):
        self.ip += 1
        target, self.ip = get_vint(self.object.code, self.ip)
        if not enable_assert:
            self.ip = target

    def RESETC(self):
        self.ip += 1
        value = self.stack.pop()
        value.value = None

    def PUSHPEG(self):
        assert False

    def JUMPTBL(self):
        self.ip += 1
        val, self.ip = get_vint(self.object.code, self.ip)
        n = self.stack.pop().value
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

    def PUSHM(self):
        assert False

    def CALLV(self):
        assert False

    def PUSHCI(self):
        assert False

    def invoke(self, module, index):
        self.callstack.append((None, self.ip))
        f = [None] * self.object.functions[index].locals
        for i in range(len(f)):
            f[i] = Value(None)
        self.frames.append(f)
        self.ip = self.object.functions[index].entry

    def raise_literal(self, name, info):
        exceptionvar = [
            Value(name),
            Value(info[0]),
            Value(info[1]),
            Value(decimal.Decimal(self.ip))
        ]

        tip = self.ip
        sp = len(self.callstack)
        while True:
            for e in self.object.exceptions:
                if e.start <= tip < e.end:
                    handler = self.object.strtable[e.excid]
                    if name == handler or (len(name) > len(handler) and name.startswith(handler) and name[len(handler)] == "."):
                        self.ip = e.handler
                        self.callstack[sp:] = []
                        self.stack.append(Value(exceptionvar))
                        return
            if sp == 0:
                break
            sp -= 1
            tip = self.callstack[sp][1]

        print("Unhandled exception {} ({}) code {})".format(name, info[0], info[1]), file=sys.stderr)
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
    Executor.STOREB,
    Executor.STOREN,
    Executor.STORES,
    Executor.STOREY,
    Executor.STOREA,
    Executor.STORED,
    Executor.STOREP,
    Executor.STOREJ,
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
    Executor.CALLE,
    Executor.CONSA,
    Executor.CONSD,
    Executor.EXCEPT,
    Executor.ALLOC,
    Executor.PUSHNIL,
    Executor.JNASSERT,
    Executor.RESETC,
    Executor.PUSHPEG,
    Executor.JUMPTBL,
    Executor.CALLX,
    Executor.SWAP,
    Executor.DROPN,
    Executor.PUSHM,
    Executor.CALLV,
    Executor.PUSHCI,
]

def neon_array__append(self):
    v = self.stack.pop().value
    a = self.stack.pop().value
    a.append(Value(v))

def neon_array__concat(self):
    b = self.stack.pop().value
    a = self.stack.pop().value
    self.stack.append(Value(a + b))

def neon_array__extend(self):
    v = self.stack.pop().value
    a = self.stack.pop().value
    a.extend(v)

def neon_array__range(self):
    step = self.stack.pop().value
    last = self.stack.pop().value
    first = self.stack.pop().value
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
    self.stack.append(Value(r))

def neon_array__resize(self):
    size = self.stack.pop().value
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
    a = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(len(a))))

def neon_array__slice(self):
    last_from_end = self.stack.pop().value
    last = int(self.stack.pop().value)
    first_from_end = self.stack.pop().value
    first = int(self.stack.pop().value)
    a = self.stack.pop().value
    if first_from_end:
        first += len(a) - 1
    if last_from_end:
        last += len(a) - 1
    self.stack.append(Value(a[first:last+1]))

def neon_array__splice(self):
    last_from_end = self.stack.pop().value
    last = int(self.stack.pop().value)
    first_from_end = self.stack.pop().value
    first = int(self.stack.pop().value)
    a = self.stack.pop().value
    b = self.stack.pop().value
    if first_from_end:
        first += len(a) - 1
    if last_from_end:
        last += len(a) - 1
    r = a[:first] + b + a[last+1:]
    self.stack.append(Value(r))

def neon_array__toBytes__number(self):
    a = self.stack.pop().value
    self.stack.append(Value(Bytes("".join(chr(int(x.value)) for x in a))))

def neon_array__toString__number(self):
    a = self.stack.pop().value
    self.stack.append(Value("[{}]".format(", ".join(str(x.value) for x in a))))

def neon_array__toString__object(self):
    a = self.stack.pop().value
    self.stack.append(Value("[{}]".format(", ".join(str(x.value) for x in a))))

def neon_array__toString__string(self):
    a = self.stack.pop().value
    self.stack.append(Value("[{}]".format(", ".join(x.value for x in a))))

def neon_boolean__toString(self):
    x = self.stack.pop().value
    self.stack.append(Value("TRUE" if x else "FALSE"))

def neon_bytes__decodeToString(self):
    b = self.stack.pop().value
    self.stack.append(Value(b.s))

def neon_bytes__range(self):
    last_from_end = self.stack.pop().value
    last = int(self.stack.pop().value)
    first_from_end = self.stack.pop().value
    first = int(self.stack.pop().value)
    b = self.stack.pop().value
    if first_from_end:
        first += len(b.s) - 1
    if last_from_end:
        last += len(b.s) - 1
    self.stack.append(Value(Bytes(b.s[first:last+1])))

def neon_bytes__size(self):
    b = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(len(b.s))))

def neon_bytes__splice(self):
    last_from_end = self.stack.pop().value
    last = int(self.stack.pop().value)
    first_from_end = self.stack.pop().value
    first = int(self.stack.pop().value)
    a = self.stack.pop().value
    b = self.stack.pop().value
    if first_from_end:
        first += len(a.s) - 1
    if last_from_end:
        last += len(a.s) - 1
    r = a.s[:first] + b.s + a.s[last+1:]
    self.stack.append(Value(Bytes(r)))

def neon_bytes__toArray(self):
    b = self.stack.pop().value
    self.stack.append(Value([Value(decimal.Decimal(ord(x))) for x in b.s]))

def neon_bytes__toString(self):
    b = self.stack.pop().value
    self.stack.append(Value("HEXBYTES \"{}\"".format(" ".join("{:02x}".format(ord(x)) for x in b.s))))

def neon_chr(self):
    n = self.stack.pop().value
    if n != int(n):
        self.raise_literal("ValueRangeException", ("chr() argument not an integer", 0))
        return
    if not (0 <= n <= 0x10ffff):
        self.raise_literal("ValueRangeException", ("chr() argument out of range 0-0x10ffff", 0))
        return
    self.stack.append(Value(unichr(int(n))))

def neon_concat(self):
    b = self.stack.pop().value
    a = self.stack.pop().value
    self.stack.append(Value(a + b))

def neon_concatBytes(self):
    b = self.stack.pop().value
    a = self.stack.pop().value
    self.stack.append(Value(Bytes(a.s + b.s)))

def neon_dictionary__keys(self):
    d = self.stack.pop().value
    self.stack.append(Value([Value(x) for x in d.keys()]))

def neon_int(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(int(x))))

def neon_max(self):
    b = self.stack.pop().value
    a = self.stack.pop().value
    self.stack.append(Value(max(a, b)))

def neon_min(self):
    b = self.stack.pop().value
    a = self.stack.pop().value
    self.stack.append(Value(min(a, b)))

def neon_num(self):
    s = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(s)))

def neon_number__toString(self):
    neon_str(self)

def neon_object__getArray(self):
    v = self.stack.pop().value
    if not isinstance(v, list):
        self.raise_literal("DynamicConversionException", ("to Array", 0))
        return
    self.stack.append(Value(v))

def neon_object__getBoolean(self):
    v = self.stack.pop().value
    if not isinstance(v, bool):
        self.raise_literal("DynamicConversionException", ("to Boolean", 0))
        return
    self.stack.append(Value(v))

def neon_object__getBytes(self):
    v = self.stack.pop().value
    if not isinstance(v, Bytes):
        self.raise_literal("DynamicConversionException", ("to Bytes", 0))
        return
    self.stack.append(Value(v))

def neon_object__getDictionary(self):
    v = self.stack.pop().value
    if not isinstance(v, dict):
        self.raise_literal("DynamicConversionException", ("to Dictionary", 0))
        return
    self.stack.append(Value(v))

def neon_object__getNumber(self):
    v = self.stack.pop().value
    if not isinstance(v, decimal.Decimal):
        self.raise_literal("DynamicConversionException", ("to Number", 0))
        return
    self.stack.append(Value(v))

def neon_object__getString(self):
    v = self.stack.pop().value
    if not isinstance(v, str):
        self.raise_literal("DynamicConversionException", ("to String", 0))
        return
    self.stack.append(Value(v))

def neon_object__isNull(self):
    v = self.stack.pop().value
    self.stack.append(Value(v is None))

def neon_object__makeArray(self):
    v = self.stack.pop().value
    self.stack.append(Value(v))

def neon_object__makeBoolean(self):
    v = self.stack.pop().value
    self.stack.append(Value(v))

def neon_object__makeBytes(self):
    v = self.stack.pop().value
    self.stack.append(Value(v))

def neon_object__makeDictionary(self):
    v = self.stack.pop().value
    self.stack.append(Value(v))

def neon_object__makeNull(self):
    self.stack.append(Value(None))

def neon_object__makeNumber(self):
    v = self.stack.pop().value
    self.stack.append(Value(v))

def neon_object__makeString(self):
    v = self.stack.pop().value
    self.stack.append(Value(v))

def neon_object__subscript(self):
    i = self.stack.pop().value
    v = self.stack.pop().value
    if isinstance(v, list):
        self.stack.append(v[int(i)])
    elif isinstance(v, dict):
        if not i in v:
            self.raise_literal("ObjectSubscriptException", (i, 0))
            return
        self.stack.append(v[i])
    else:
        assert False, v

def neon_object__toString(self):
    v = self.stack.pop().value
    if isinstance(v, list):
        self.stack.append(Value("[{}]".format(", ".join(x.literal() for x in v))))
    elif isinstance(v, dict):
        self.stack.append(Value("{{{}}}".format(", ".join("{}: {}".format(quoted(k), x.literal()) for k, x in sorted(v.items())))))
    else:
        self.stack.append(Value(v.literal()))

def neon_odd(self):
    v = self.stack.pop().value
    if v != int(v):
        self.raise_literal("ValueRangeException", ("odd() requires integer", 0))
        return
    self.stack.append(Value((v % 2) != 0))

def neon_ord(self):
    s = self.stack.pop().value
    if len(s) != 1:
        self.raise_literal("ArrayIndexException", ("ord() requires string of length 1", 0))
        return
    self.stack.append(Value(decimal.Decimal(ord(s))))

def neon_print(self):
    s = self.stack.pop().value
    print("".join(x.value for x in s))

def neon_round(self):
    value = self.stack.pop().value
    places = self.stack.pop().value
    if places == 0:
        self.stack.append(Value(decimal.Decimal(int(value))))
    else:
        self.stack.append(Value(value.quantize(decimal.Decimal("1."+("0"*int(places)))).normalize()))

def neon_str(self):
    v = self.stack.pop().value
    if isinstance(v, decimal.Decimal):
        # Remove trailing zeros and decimal point if possible.
        v = re.sub("\\.0*(?![\\d])", "", str(v))
    self.stack.append(Value(str(v)))

def neon_string__append(self):
    b = self.stack.pop().value
    a = self.stack.pop()
    a.value = a.value + b

def neon_string__length(self):
    s = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(len(s))))

def neon_string__splice(self):
    last_from_end = self.stack.pop().value
    last = int(self.stack.pop().value)
    first_from_end = self.stack.pop().value
    first = int(self.stack.pop().value)
    a = self.stack.pop().value
    b = self.stack.pop().value
    if first_from_end:
        first += len(a) - 1
    if last_from_end:
        last += len(a) - 1
    r = a[:first] + b + a[last+1:]
    self.stack.append(Value(r))

def neon_string__substring(self):
    last_from_end = self.stack.pop().value
    last = int(self.stack.pop().value)
    first_from_end = self.stack.pop().value
    first = int(self.stack.pop().value)
    s = self.stack.pop().value
    if first_from_end:
        first += len(s) - 1
    if last_from_end:
        last += len(s) - 1
    self.stack.append(Value(s[first:last+1]))

def neon_string__toBytes(self):
    s = self.stack.pop().value
    self.stack.append(Value(Bytes(s)))

def neon_substring(self):
    length = int(self.stack.pop().value)
    offset = int(self.stack.pop().value)
    s = self.stack.pop().value
    self.stack.append(Value(s[offset:offset+length]))

def neon_file_exists(self):
    n = self.stack.pop().value
    self.stack.append(Value(os.path.exists(n)))

def neon_io_fprint(self):
    s = self.stack.pop().value
    f = self.stack.pop().value
    if f is Globals["io$stderr"]:
        sys.stderr.write(s)
    else:
        assert False, f

def neon_math_abs(self):
    x = self.stack.pop().value
    self.stack.append(Value(abs(x)))

def neon_math_acos(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.acos(float(x)))))

def neon_math_acosh(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.acosh(float(x)))))

def neon_math_asin(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.asin(float(x)))))

def neon_math_asinh(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.asinh(float(x)))))

def neon_math_atan(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.atan(float(x)))))

def neon_math_atan2(self):
    y = self.stack.pop().value
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.atan2(float(x), float(y)))))

def neon_math_atanh(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.atanh(float(x)))))

def neon_math_cbrt(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(float(x) ** (1/3.0))))

def neon_math_ceil(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.ceil(float(x)))))

def neon_math_cos(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.cos(float(x)))))

def neon_math_cosh(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.cosh(float(x)))))

def neon_math_erf(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.erf(float(x)))))

def neon_math_erfc(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.erfc(float(x)))))

def neon_math_exp(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.exp(float(x)))))

def neon_math_exp2(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(2 ** float(x))))

def neon_math_expm1(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.expm1(float(x)))))

def neon_math_floor(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.floor(float(x)))))

def neon_math_frexp(self):
    x = self.stack.pop().value
    (m, e) = math.frexp(float(x))
    self.stack.append(Value(decimal.Decimal(e)))
    self.stack.append(Value(decimal.Decimal(m)))

def neon_math_hypot(self):
    y = self.stack.pop().value
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.hypot(float(x), float(y)))))

def neon_math_intdiv(self):
    y = self.stack.pop().value
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(x // y)))

def neon_math_ldexp(self):
    i = self.stack.pop().value
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.ldexp(float(x), int(i)))))

def neon_math_lgamma(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.lgamma(float(x)))))

def neon_math_log(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.log(float(x)))))

def neon_math_log10(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.log10(float(x)))))

def neon_math_log1p(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.log1p(float(x)))))

def neon_math_log2(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.log(float(x))/math.log(2))))

def neon_math_nearbyint(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(round(float(x)))))

def neon_math_sign(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(0 if x == 0 else -1 if x.is_signed() else 1)))

def neon_math_sin(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.sin(float(x)))))

def neon_math_sinh(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.sinh(float(x)))))

def neon_math_sqrt(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.sqrt(float(x)))))

def neon_math_tan(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.tan(float(x)))))

def neon_math_tanh(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.tanh(float(x)))))

def neon_math_tgamma(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.gamma(float(x)))))

def neon_math_trunc(self):
    x = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(math.trunc(float(x)))))

def neon_os_system(self):
    s = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(os.system(s))))

def neon_posix_fork(self):
    self.stack.append(Value(decimal.Decimal(os.fork())))

def neon_string_find(self):
    t = self.stack.pop().value
    s = self.stack.pop().value
    self.stack.append(Value(decimal.Decimal(s.find(t))))

def neon_string_hasPrefix(self):
    prefix = self.stack.pop().value
    s = self.stack.pop().value
    self.stack.append(Value(s.startswith(prefix)))

def neon_string_hasSuffix(self):
    prefix = self.stack.pop().value
    s = self.stack.pop().value
    self.stack.append(Value(s.endswith(prefix)))

def neon_string_join(self):
    d = self.stack.pop().value
    a = self.stack.pop().value
    self.stack.append(Value(d.join(x.value for x in a)))

def neon_string_lower(self):
    s = self.stack.pop().value
    self.stack.append(Value(s.lower()))

def neon_string_split(self):
    d = self.stack.pop().value
    s = self.stack.pop().value
    self.stack.append(Value([Value(x) for x in s.split(d)]))

def neon_string_splitLines(self):
    s = self.stack.pop().value
    if s == "":
        self.stack.append(Value([]))
    else:
        r = [Value(x) for x in re.split("\r?\n", s)]
        if s.endswith("\n"):
            r[-1:] = []
        self.stack.append(Value(r))

def neon_string_trim(self):
    s = self.stack.pop().value
    self.stack.append(Value(s.strip()))

def neon_string_upper(self):
    s = self.stack.pop().value
    self.stack.append(Value(s.upper()))

def neon_sys_exit(self):
    x = self.stack.pop().value
    if not is_integer(x) or x < 0 or x > 255:
        self.raise_literal("InvalidValueException", ("sys.exit invalid parameter: {}".format(x), 0))
        return
    sys.exit(int(x))

def neon_time_now(self):
    self.stack.append(Value(decimal.Decimal(time.time())))

Executor(open(sys.argv[1], "rb").read()).run()
