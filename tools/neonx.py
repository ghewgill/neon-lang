from __future__ import print_function

import decimal
import struct
import sys

class Type:
    def __init__(self):
        self.name = 0
        self.descriptor = 0

class Bytecode:
    def __init__(self, bytecode):
        i = 0
        self.source_hash = bytecode[i:i+32]
        i += 32
        self.global_size = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2

        strtablesize = struct.unpack(">L", bytecode[i:i+4])[0]
        i += 4
        self.strtable = self.getstrtable(bytecode[i:i+strtablesize])
        i += strtablesize

        typesize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_types = []
        while typesize > 0:
            t = Type()
            t.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            t.descriptor = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            self.export_types.append(t)
            typesize -= 1

        constantsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_constants = []
        while constantsize > 0:
            c = Constant()
            c.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            c.type = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            size = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            c.value = bytecode[i:i+size]
            i += size
            self.export_constants.append(c)
            constantsize -= 1

        variablesize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_variables = []
        while variablesize > 0:
            v = Variable()
            v.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            v.type = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            v.index = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            self.export_variables.append(v)
            variablesize -= 1

        functionsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_functions = []
        while functionsize > 0:
            f = Function()
            f.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            f.descriptor = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            f.entry = struct.unpack(">H", bytecode[i:i+4])[0]
            i += 4
            self.export_functions.append(f)
            functionsize -= 1

        exceptionexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        self.export_exceptions = []
        while exceptionexportsize > 0:
            e = ExceptionExport()
            e.name = struct.unpack(">H", bytecode[i:i+2])[0]
            i += 2
            self.export_exceptions.append(e)
            exceptionexportsize -= 1

        interfaceexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while interfaceexportsize > 0:
            assert False, interfaceexportsize

        importsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while importsize > 0:
            assert False, importsize

        functionsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while functionsize > 0:
            assert False, functionsize

        exceptionsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while exceptionsize > 0:
            assert False, exceptionsize

        classsize = struct.unpack(">H", bytecode[i:i+2])[0]
        i += 2
        while classsize > 0:
            assert False, classsize

        self.code = bytecode[i:]

    @staticmethod
    def getstrtable(table):
        r = []
        i = 0
        while i < len(table):
            strlen = struct.unpack(">L", table[i:i+4])[0]
            i += 4
            r.append(table[i:i+strlen].decode())
            i += strlen
        return r

class Value:
    def __init__(self, value):
        self.value = value

class Executor:
    def __init__(self, bytecode):
        self.object = Bytecode(bytecode)
        self.ip = 0
        self.stack = []
        self.callstack = []
        self.globals = [None] * self.object.global_size
        for i in range(len(self.globals)):
            self.globals[i] = Value(None)

    def run(self):
        self.callstack.append((None, len(self.object.code)))
        while self.ip < len(self.object.code):
            Dispatch[ord(self.object.code[self.ip])](self)

    def ENTER(self):
        assert False

    def LEAVE(self):
        assert False

    def PUSHB(self):
        assert False

    def PUSHN(self):
        val = struct.unpack_from(">L", self.object.code, self.ip+1)[0]
        self.ip += 5
        self.stack.append(Value(decimal.Decimal(self.object.strtable[val])))

    def PUSHS(self):
        val = struct.unpack_from(">L", self.object.code, self.ip+1)[0]
        self.ip += 5
        self.stack.append(Value(self.object.strtable[val]))

    def PUSHPG(self):
        addr = struct.unpack_from(">L", self.object.code, self.ip+1)[0]
        self.ip += 5
        self.stack.append(self.globals[addr])

    def PUSHPPG(self):
        assert False

    def PUSHPMG(self):
        assert False

    def PUSHPL(self):
        assert False

    def PUSHPOL(self):
        assert False

    def PUSHI(self):
        assert False

    def LOADB(self):
        assert False

    def LOADN(self):
        self.ip += 1
        addr = self.stack.pop()
        self.stack.append(Value(addr.value))

    def LOADS(self):
        assert False

    def LOADA(self):
        assert False

    def LOADD(self):
        assert False

    def LOADP(self):
        assert False

    def STOREB(self):
        assert False

    def STOREN(self):
        self.ip += 1
        addr = self.stack.pop()
        value = self.stack.pop().value
        addr.value = value

    def STORES(self):
        assert False

    def STOREA(self):
        assert False

    def STORED(self):
        assert False

    def STOREP(self):
        assert False

    def NEGN(self):
        assert False

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
        self.stack.append(Value(a / b))

    def MODN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a % b))

    def EXPN(self):
        self.ip += 1
        b = self.stack.pop().value
        a = self.stack.pop().value
        self.stack.append(Value(a ** b))

    def EQB(self):
        assert False

    def NEB(self):
        assert False

    def EQN(self):
        assert False

    def NEN(self):
        assert False

    def LTN(self):
        assert False

    def GTN(self):
        assert False

    def LEN(self):
        assert False

    def GEN(self):
        assert False

    def EQS(self):
        assert False

    def NES(self):
        assert False

    def LTS(self):
        assert False

    def GTS(self):
        assert False

    def LES(self):
        assert False

    def GES(self):
        assert False

    def EQA(self):
        assert False

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
        assert False

    def INDEXAR(self):
        assert False

    def INDEXAW(self):
        assert False

    def INDEXAV(self):
        assert False

    def INDEXAN(self):
        assert False

    def INDEXDR(self):
        assert False

    def INDEXDW(self):
        assert False

    def INDEXDV(self):
        assert False

    def INA(self):
        assert False

    def IND(self):
        assert False

    def CALLP(self):
        val = struct.unpack_from(">L", self.object.code, self.ip+1)[0]
        self.ip += 5
        func = self.object.strtable[val]
        if func == "print":
            s = self.stack.pop().value
            print(s)
        elif func == "str":
            v = self.stack.pop().value
            self.stack.append(Value(str(v)))
        else:
            assert False, func

    def CALLF(self):
        assert False

    def CALLMF(self):
        assert False

    def CALLI(self):
        assert False

    def JUMP(self):
        assert False

    def JF(self):
        assert False

    def JT(self):
        assert False

    def JFCHAIN(self):
        assert False

    def DUP(self):
        assert False

    def DUPX1(self):
        assert False

    def DROP(self):
        assert False

    def RET(self):
        (_module, self.ip) = self.callstack.pop()

    def CALLE(self):
        assert False

    def CONSA(self):
        assert False

    def CONSD(self):
        assert False

    def EXCEPT(self):
        assert False

    def ALLOC(self):
        assert False

    def PUSHNIL(self):
        assert False

    def JNASSERT(self):
        assert False

    def RESETC(self):
        self.ip += 1
        value = self.stack.pop()
        value.value = None

    def PUSHPEG(self):
        assert False

    def JUMPTBL(self):
        assert False

    def CALLX(self):
        assert False

    def SWAP(self):
        assert False

    def DROPN(self):
        assert False

    def PUSHM(self):
        assert False

    def CALLV(self):
        assert False

    def PUSHCI(self):
        assert False

Dispatch = [
    Executor.ENTER,
    Executor.LEAVE,
    Executor.PUSHB,
    Executor.PUSHN,
    Executor.PUSHS,
    Executor.PUSHPG,
    Executor.PUSHPPG,
    Executor.PUSHPMG,
    Executor.PUSHPL,
    Executor.PUSHPOL,
    Executor.PUSHI,
    Executor.LOADB,
    Executor.LOADN,
    Executor.LOADS,
    Executor.LOADA,
    Executor.LOADD,
    Executor.LOADP,
    Executor.STOREB,
    Executor.STOREN,
    Executor.STORES,
    Executor.STOREA,
    Executor.STORED,
    Executor.STOREP,
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

Executor(open(sys.argv[1], "rb").read()).run()
