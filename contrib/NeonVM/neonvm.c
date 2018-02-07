#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcode.h"
#include "stack.h"

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

static short get_uint16(const unsigned char *pobj, unsigned int nBuffSize, unsigned int *i)
{
    if (*i+2 > nBuffSize) {
        perror("Bytecode exception: Read past EOF.");
    }
    short r = (pobj[*i] << 8) | pobj[*i+1];
    *i += 2;
    return r;
}

static unsigned int get_uint32(const unsigned char *pobj, unsigned int nBuffSize, unsigned int *i)
{
    if (*i+4 > nBuffSize) {
        perror("Bytecode excpetion: Read past EOF.");
    }
    long r = (pobj[*i] << 24) | (pobj[*i+1] << 16) | (pobj[*i+2] << 8) | pobj[*i+3];
    *i += 4;
    return r;
}

struct NString {
    unsigned int length;
    char *data;
};

static unsigned char **getstrtable(const unsigned char *start, const unsigned char *end, unsigned int *cnt)
{
    unsigned char **r = NULL;
    unsigned int i = 0;

    *cnt = 0;
    // We're going to iterate the string table first, to get the count,
    // then we'll allocate the data.
    const unsigned char *s = start;
    while (s != end) {
        s += ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3]) + 4;
        *cnt = *cnt + 1;
    }

    r = malloc((sizeof(unsigned char *)) * *cnt);
    while (start != end) {
        size_t strlen = (start[0] << 24) | (start[1] << 16) | (start[2] << 8) | start[3];
        start += 4;

        r[i] = (unsigned char*)malloc(strlen+1);
        memcpy(r[i], start, strlen);
        r[i][strlen] = '\0';
        i++;
        start += strlen;
    }
    return r;
}

typedef struct tagTType {
    short name;
    short descriptor;
} Type, *pType;

typedef struct tagTBytecode {
    unsigned char source_hash[32];
    unsigned short global_size;
    unsigned int strtablesize;
    unsigned int strtablelen;
    unsigned char **strtable;
    unsigned int typesize;
    unsigned int constantsize;
    unsigned int variablesize;
    unsigned int functionsize;
    unsigned int exceptionsize;
    unsigned int exceptionexportsize;
    unsigned int interfaceexportsize;
    unsigned int importsize;
    unsigned int classsize;
    const unsigned char *code;
    unsigned int codelen;

    struct Type *pExportTypes;
} TBytecode, *pBytecode;

struct tagTExecutor;

void exec_loop(struct tagTExecutor *self);

typedef struct tagTExecutor {
    struct tagTBytecode *object;
    unsigned int ip;
    struct TStackNode *stack;
    struct TStackNode *callstack;

    void(*exec_loop)(struct tagTExecutor *self);
    void(*exec_run)(struct tagTExecutor *self);
} TExecutor, *pTExecutor;

struct tagTExecutor *new_executer(struct tagTBytecode *object);

static void loadBytecode(const unsigned char *bytecode, unsigned int len, struct tagTBytecode *pBytecode)
{
    unsigned int i = 0;

    memcpy(pBytecode->source_hash, bytecode, 32);
    i += 32;
    pBytecode->global_size = get_uint16(bytecode, len, &i);
    //i += 2;

    pBytecode->strtablesize = get_uint32(bytecode, len, &i);
    //i += 4;
    pBytecode->strtable = getstrtable(&bytecode[i], &bytecode[i + pBytecode->strtablesize], &pBytecode->strtablelen);
    i += pBytecode->strtablesize;

    pBytecode->typesize = get_uint16(bytecode, len, &i);
    //    typesize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    self.export_types = []
    //    while typesize > 0:
    //        t = Type()
    //        t.name = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        t.descriptor = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        self.export_types.append(t)
    //        typesize -= 1
    pBytecode->constantsize = get_uint16(bytecode, len, &i);
    //    constantsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    self.export_constants = []
    //    while constantsize > 0:
    //        c = Constant()
    //        c.name = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        c.type = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        size = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        c.value = bytecode[i:i+size]
    //        i += size
    //        self.export_constants.append(c)
    //        constantsize -= 1
    pBytecode->variablesize = get_uint16(bytecode, len, &i);
    //    variablesize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    self.export_variables = []
    //    while variablesize > 0:
    //        v = Variable()
    //        v.name = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        v.type = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        v.index = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        self.export_variables.append(v)
    //        variablesize -= 1
    pBytecode->functionsize = get_uint16(bytecode, len, &i);
    //    functionsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    self.export_functions = []
    //    while functionsize > 0:
    //        f = Function()
    //        f.name = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        f.descriptor = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        f.entry = struct.unpack(">H", bytecode[i:i+4])[0]
    //        i += 4
    //        self.export_functions.append(f)
    //        functionsize -= 1
    pBytecode->exceptionexportsize = get_uint16(bytecode, len, &i);
    //    exceptionexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    self.export_exceptions = []
    //    while exceptionexportsize > 0:
    //        e = ExceptionExport()
    //        e.name = struct.unpack(">H", bytecode[i:i+2])[0]
    //        i += 2
    //        self.export_exceptions.append(e)
    //        exceptionexportsize -= 1
    pBytecode->interfaceexportsize = get_uint16(bytecode, len, &i);
    //    interfaceexportsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    while interfaceexportsize > 0:
    //        assert False, interfaceexportsize
    pBytecode->importsize = get_uint16(bytecode, len, &i);
    //    importsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    while importsize > 0:
    //        assert False, importsize
    pBytecode->functionsize = get_uint16(bytecode, len, &i);
    //    functionsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    while functionsize > 0:
    //        assert False, functionsize
    pBytecode->exceptionsize = get_uint16(bytecode, len, &i);
    //    exceptionsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    while exceptionsize > 0:
    //        assert False, exceptionsize
    pBytecode->classsize = get_uint16(bytecode, len, &i);
    //    classsize = struct.unpack(">H", bytecode[i:i+2])[0]
    //    i += 2
    //    while classsize > 0:
    //        assert False, classsize
    pBytecode->code = bytecode + i;
    pBytecode->codelen = len - i;
}

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        printf("\nusage: %s program.neon\n", argv[0]);
    }
    struct tagTBytecode *pModule;
    FILE *fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    pModule = (struct tagTBytecode *)malloc((sizeof(struct tagTBytecode)));
    unsigned char *bytecode = (unsigned char*)malloc(nSize);
    size_t bytes_read = fread(bytecode, 1, nSize, fp);

    loadBytecode(bytecode, bytes_read, pModule);

    struct tagTExecutor *exec = new_executer(pModule);
    exec->exec_run(exec);

}

void exec_run(struct tagTExecutor *self)
{
    push(&self->callstack, &self->object->codelen);
    exec_loop(self);

    for (unsigned int i = 0; i < self->object->strtablelen; i++) {
        free(self->object->strtable[i]);
    }
}

struct tagTExecutor *new_executer(struct tagTBytecode *object)
{
    struct tagTExecutor *r = malloc(sizeof(struct tagTExecutor));
    r->object = object;
    r->callstack = NULL;
    r->stack = NULL;
    r->ip = 0;
    r->exec_loop = exec_loop;
    r->exec_run = exec_run;
    return r;
}

void exec_ENTER()
{
    perror("not implemented");}

void exec_LEAVE()
{
    perror("not implemented");}

void exec_PUSHB()
{
    perror("not implemented");}

void exec_PUSHN()
{
    perror("not implemented");
}

void exec_PUSHS(struct tagTExecutor *self)
{
    uint32_t val = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    self->ip += 5;
    push(&self->stack, self->object->strtable[val]);
}

void exec_PUSHPG()
{
    perror("not implemented");
}

void exec_PUSHPPG()
{
    perror("not implemented");
}

void exec_PUSHPMG()
{
    perror("not implemented");
}

void exec_PUSHPL()
{
    perror("not implemented");
}

void exec_PUSHPOL()
{
    perror("not implemented");
}

void exec_PUSHI()
{
    perror("not implemented");
}

void exec_LOADB()
{
    perror("not implemented");
}

void exec_LOADN()
{
    perror("not implemented");
}

void exec_LOADS()
{
    perror("not implemented");
}

void exec_LOADA()
{
    perror("not implemented");
}

void exec_LOADD()
{
    perror("not implemented");
}

void exec_LOADP()
{
    perror("not implemented");
}

void exec_STOREB()
{
    perror("not implemented");
}

void exec_STOREN()
{
    perror("not implemented");
}

void exec_STORES()
{
    perror("not implemented");
}

void exec_STOREA()
{
    perror("not implemented");
}

void exec_STORED()
{
    perror("not implemented");
}

void exec_STOREP()
{
    perror("not implemented");
}

void exec_NEGN()
{
    perror("not implemented");
}

void exec_ADDN()
{
    perror("not implemented");
}

void exec_SUBN()
{
    perror("not implemented");
}

void exec_MULN()
{
    perror("not implemented");
}

void exec_DIVN()
{
    perror("not implemented");
}

void exec_MODN()
{
    perror("not implemented");
}

void exec_EXPN()
{
    perror("not implemented");
}

void exec_EQB()
{
    perror("not implemented");
}

void exec_NEB()
{
    perror("not implemented");
}

void exec_EQN()
{
    perror("not implemented");
}

void exec_NEN()
{
    perror("not implemented");
}

void exec_LTN()
{
    perror("not implemented");
}

void exec_GTN()
{
    perror("not implemented");
}

void exec_LEN()
{
    perror("not implemented");
}

void exec_GEN()
{
    perror("not implemented");
}

void exec_EQS()
{
    perror("not implemented");
}

void exec_NES()
{
    perror("not implemented");
}

void exec_LTS()
{
    perror("not implemented");
}

void exec_GTS()
{
    perror("not implemented");
}

void exec_LES()
{
    perror("not implemented");
}

void exec_GES()
{
    perror("not implemented");
}

void exec_EQA()
{
    perror("not implemented");
}

void exec_NEA()
{
    perror("not implemented");
}

void exec_EQD()
{
    perror("not implemented");
}

void exec_NED()
{
    perror("not implemented");
}

void exec_EQP()
{
    perror("not implemented");
}

void exec_NEP()
{
    perror("not implemented");
}

void exec_ANDB()
{
    perror("not implemented");
}

void exec_ORB()
{
    perror("not implemented");
}

void exec_NOTB()
{
    perror("not implemented");
}

void exec_INDEXAR()
{
    perror("not implemented");
}

void exec_INDEXAW()
{
    perror("not implemented");
}

void exec_INDEXAV()
{
    perror("not implemented");
}

void exec_INDEXAN()
{
    perror("not implemented");
}

void exec_INDEXDR()
{
    perror("not implemented");
}

void exec_INDEXDW()
{
    perror("not implemented");
}

void exec_INDEXDV()
{
    perror("not implemented");
}

void exec_INA()
{
    perror("not implemented");
}

void exec_IND()
{
    perror("not implemented");
}

void exec_CALLP(struct tagTExecutor *self)
{
    uint32_t val = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    self->ip += 5;
    const unsigned char *func = self->object->strtable[val];

    if (strcmp((const char*)func, "print") == 0) {
        const char *s = pop(&self->stack);
        printf("%s\n", s);
    } else {
        perror("exec_CALLP(): Invalid function call");
    }
}

void exec_CALLF()
{
    perror("not implemented");
}

void exec_CALLMF()
{
    perror("not implemented");
}

void exec_CALLI()
{
    perror("not implemented");
}

void exec_JUMP()
{
    perror("not implemented");
}

void exec_JF()
{
    perror("not implemented");
}

void exec_JT()
{
    perror("not implemented");
}

void exec_JFCHAIN()
{
    perror("not implemented");
}

void exec_DUP()
{
    perror("not implemented");
}

void exec_DUPX1()
{
    perror("not implemented");
}

void exec_DROP()
{
    perror("not implemented");
}

void exec_RET(struct tagTExecutor *self)
{
    const char *code = pop(&self->callstack);
    self->ip = (code[3] << 24) | (code[2] << 16) | (code[1] << 8) | code[0];
}

void exec_CALLE()
{
    perror("not implemented");
}

void exec_CONSA()
{
    perror("not implemented");
}

void exec_CONSD()
{
    perror("not implemented");
}

void exec_EXCEPT()
{
    perror("not implemented");
}

void exec_ALLOC()
{
    perror("not implemented");
}

void exec_PUSHNIL()
{
    perror("not implemented");
}

void exec_JNASSERT()
{
    perror("not implemented");
}

void exec_RESETC()
{
    perror("not implemented");
}

void exec_PUSHPEG()
{
    perror("not implemented");
}

void exec_JUMPTBL()
{
    perror("not implemented");
}

void exec_CALLX()
{
    perror("not implemented");
}

void exec_SWAP()
{
    perror("not implemented");
}

void exec_DROPN()
{
    perror("not implemented");
}

void exec_PUSHM()
{
    perror("not implemented");
}

void exec_CALLV()
{
    perror("not implemented");}

void exec_PUSHCI()
{
    perror("not implemented");
}

void exec_loop(struct tagTExecutor *self)
{
    while (self->ip < self->object->codelen) {
        //std::cerr << "mod " << module->name << " ip " << ip << " op " << (int)module->object.code[ip] << " st " << stack.depth() << "\n";
        switch (self->object->code[self->ip]) {
            case ENTER:   exec_ENTER(); break;
            case LEAVE:   exec_LEAVE(); break;
            case PUSHB:   exec_PUSHB(); break;
            case PUSHN:   exec_PUSHN(); break;
            case PUSHS:   exec_PUSHS(self); break;
            case PUSHPG:  exec_PUSHPG(); break;
            case PUSHPPG: exec_PUSHPPG(); break;
            case PUSHPMG: exec_PUSHPMG(); break;
            case PUSHPL:  exec_PUSHPL(); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case PUSHI:   exec_PUSHI(); break;
            case LOADB:   exec_LOADB(); break;
            case LOADN:   exec_LOADN(); break;
            case LOADS:   exec_LOADS(); break;
            case LOADA:   exec_LOADA(); break;
            case LOADD:   exec_LOADD(); break;
            case LOADP:   exec_LOADP(); break;
            case STOREB:  exec_STOREB(); break;
            case STOREN:  exec_STOREN(); break;
            case STORES:  exec_STORES(); break;
            case STOREA:  exec_STOREA(); break;
            case STORED:  exec_STORED(); break;
            case STOREP:  exec_STOREP(); break;
            case NEGN:    exec_NEGN(); break;
            case ADDN:    exec_ADDN(); break;
            case SUBN:    exec_SUBN(); break;
            case MULN:    exec_MULN(); break;
            case DIVN:    exec_DIVN(); break;
            case MODN:    exec_MODN(); break;
            case EXPN:    exec_EXPN(); break;
            case EQB:     exec_EQB(); break;
            case NEB:     exec_NEB(); break;
            case EQN:     exec_EQN(); break;
            case NEN:     exec_NEN(); break;
            case LTN:     exec_LTN(); break;
            case GTN:     exec_GTN(); break;
            case LEN:     exec_LEN(); break;
            case GEN:     exec_GEN(); break;
            case EQS:     exec_EQS(); break;
            case NES:     exec_NES(); break;
            case LTS:     exec_LTS(); break;
            case GTS:     exec_GTS(); break;
            case LES:     exec_LES(); break;
            case GES:     exec_GES(); break;
            case EQA:     exec_EQA(); break;
            case NEA:     exec_NEA(); break;
            case EQD:     exec_EQD(); break;
            case NED:     exec_NED(); break;
            case EQP:     exec_EQP(); break;
            case NEP:     exec_NEP(); break;
            case ANDB:    exec_ANDB(); break;
            case ORB:     exec_ORB(); break;
            case NOTB:    exec_NOTB(); break;
            case INDEXAR: exec_INDEXAR(); break;
            case INDEXAW: exec_INDEXAW(); break;
            case INDEXAV: exec_INDEXAV(); break;
            case INDEXAN: exec_INDEXAN(); break;
            case INDEXDR: exec_INDEXDR(); break;
            case INDEXDW: exec_INDEXDW(); break;
            case INDEXDV: exec_INDEXDV(); break;
            case INA:     exec_INA(); break;
            case IND:     exec_IND(); break;
            case CALLP:   exec_CALLP(self); break;
            case CALLF:   exec_CALLF(); break;
            case CALLMF:  exec_CALLMF(); break;
            case CALLI:   exec_CALLI(); break;
            case JUMP:    exec_JUMP(); break;
            case JF:      exec_JF(); break;
            case JT:      exec_JT(); break;
            case JFCHAIN: exec_JFCHAIN(); break;
            case DUP:     exec_DUP(); break;
            case DUPX1:   exec_DUPX1(); break;
            case DROP:    exec_DROP(); break;
            case RET:     exec_RET(self); break;
            case CALLE:   exec_CALLE(); break;
            case CONSA:   exec_CONSA(); break;
            case CONSD:   exec_CONSD(); break;
            case EXCEPT:  exec_EXCEPT(); break;
            case ALLOC:   exec_ALLOC(); break;
            case PUSHNIL: exec_PUSHNIL(); break;
            case JNASSERT:exec_JNASSERT(); break;
            case RESETC:  exec_RESETC(); break;
            case PUSHPEG: exec_PUSHPEG(); break;
            case JUMPTBL: exec_JUMPTBL(); break;
            case CALLX:   exec_CALLX(); break;
            case SWAP:    exec_SWAP(); break;
            case DROPN:   exec_DROPN(); break;
            case PUSHM:   exec_PUSHM(); break;
            case CALLV:   exec_CALLV(); break;
            case PUSHCI:  exec_PUSHCI(); break;
            default:
                fprintf(stderr, "exec: Unexpected opcode: %d\n", self->object->code[self->ip]);
                abort();
        }
    }
}


//self.callstack.append((None, len(self.object.code)))
//        while self.ip < len(self.object.code):
//            Dispatch[ord(self.object.code[self.ip])](self)
//
//    def ENTER(self):
//        assert False
//
//    def LEAVE(self):
//        assert False
//
//    def PUSHB(self):
//        assert False
//
//    def PUSHN(self):
//        assert False
//
//    def PUSHS(self):
//        val = struct.unpack_from(">L", self.object.code, self.ip+1)[0]
//        self.ip += 5
//        self.stack.append(self.object.strtable[val])
//
//    def PUSHPG(self):
//        assert False
//
//    def PUSHPPG(self):
//        assert False
//
//    def PUSHPMG(self):
//        assert False
//
//    def PUSHPL(self):
//        assert False
//
//    def PUSHPOL(self):
//        assert False
//
//    def PUSHI(self):
//        assert False
//
//    def LOADB(self):
//        assert False
//
//    def LOADN(self):
//        assert False
//
//    def LOADS(self):
//        assert False
//
//    def LOADA(self):
//        assert False
//
//    def LOADD(self):
//        assert False
//
//    def LOADP(self):
//        assert False
//
//    def STOREB(self):
//        assert False
//
//    def STOREN(self):
//        assert False
//
//    def STORES(self):
//        assert False
//
//    def STOREA(self):
//        assert False
//
//    def STORED(self):
//        assert False
//
//    def STOREP(self):
//        assert False
//
//    def NEGN(self):
//        assert False
//
//    def ADDN(self):
//        assert False
//
//    def SUBN(self):
//        assert False
//
//    def MULN(self):
//        assert False
//
//    def DIVN(self):
//        assert False
//
//    def MODN(self):
//        assert False
//
//    def EXPN(self):
//        assert False
//
//    def EQB(self):
//        assert False
//
//    def NEB(self):
//        assert False
//
//    def EQN(self):
//        assert False
//
//    def NEN(self):
//        assert False
//
//    def LTN(self):
//        assert False
//
//    def GTN(self):
//        assert False
//
//    def LEN(self):
//        assert False
//
//    def GEN(self):
//        assert False
//
//    def EQS(self):
//        assert False
//
//    def NES(self):
//        assert False
//
//    def LTS(self):
//        assert False
//
//    def GTS(self):
//        assert False
//
//    def LES(self):
//        assert False
//
//    def GES(self):
//        assert False
//
//    def EQA(self):
//        assert False
//
//    def NEA(self):
//        assert False
//
//    def EQD(self):
//        assert False
//
//    def NED(self):
//        assert False
//
//    def EQP(self):
//        assert False
//
//    def NEP(self):
//        assert False
//
//    def ANDB(self):
//        assert False
//
//    def ORB(self):
//        assert False
//
//    def NOTB(self):
//        assert False
//
//    def INDEXAR(self):
//        assert False
//
//    def INDEXAW(self):
//        assert False
//
//    def INDEXAV(self):
//        assert False
//
//    def INDEXAN(self):
//        assert False
//
//    def INDEXDR(self):
//        assert False
//
//    def INDEXDW(self):
//        assert False
//
//    def INDEXDV(self):
//        assert False
//
//    def INA(self):
//        assert False
//
//    def IND(self):
//        assert False
//
//    def CALLP(self):
//        val = struct.unpack_from(">L", self.object.code, self.ip+1)[0]
//        self.ip += 5
//        func = self.object.strtable[val]
//        if func == "print":
//            s = self.stack.pop()
//            print(s)
//        else:
//            assert False
//
//    def CALLF(self):
//        assert False
//
//    def CALLMF(self):
//        assert False
//
//    def CALLI(self):
//        assert False
//
//    def JUMP(self):
//        assert False
//
//    def JF(self):
//        assert False
//
//    def JT(self):
//        assert False
//
//    def JFCHAIN(self):
//        assert False
//
//    def DUP(self):
//        assert False
//
//    def DUPX1(self):
//        assert False
//
//    def DROP(self):
//        assert False
//
//    def RET(self):
//        (_module, self.ip) = self.callstack.pop()
//
//    def CALLE(self):
//        assert False
//
//    def CONSA(self):
//        assert False
//
//    def CONSD(self):
//        assert False
//
//    def EXCEPT(self):
//        assert False
//
//    def ALLOC(self):
//        assert False
//
//    def PUSHNIL(self):
//        assert False
//
//    def JNASSERT(self):
//        assert False
//
//    def RESETC(self):
//        assert False
//
//    def PUSHPEG(self):
//        assert False
//
//    def JUMPTBL(self):
//        assert False
//
//    def CALLX(self):
//        assert False
//
//    def SWAP(self):
//        assert False
//
//    def DROPN(self):
//        assert False
//
//    def PUSHM(self):
//        assert False
//
//    def CALLV(self):
//        assert False
//
//    def PUSHCI(self):
//        assert False
//
//Dispatch = [
//    Executor.ENTER,
//    Executor.LEAVE,
//    Executor.PUSHB,
//    Executor.PUSHN,
//    Executor.PUSHS,
//    Executor.PUSHPG,
//    Executor.PUSHPPG,
//    Executor.PUSHPMG,
//    Executor.PUSHPL,
//    Executor.PUSHPOL,
//    Executor.PUSHI,
//    Executor.LOADB,
//    Executor.LOADN,
//    Executor.LOADS,
//    Executor.LOADA,
//    Executor.LOADD,
//    Executor.LOADP,
//    Executor.STOREB,
//    Executor.STOREN,
//    Executor.STORES,
//    Executor.STOREA,
//    Executor.STORED,
//    Executor.STOREP,
//    Executor.NEGN,
//    Executor.ADDN,
//    Executor.SUBN,
//    Executor.MULN,
//    Executor.DIVN,
//    Executor.MODN,
//    Executor.EXPN,
//    Executor.EQB,
//    Executor.NEB,
//    Executor.EQN,
//    Executor.NEN,
//    Executor.LTN,
//    Executor.GTN,
//    Executor.LEN,
//    Executor.GEN,
//    Executor.EQS,
//    Executor.NES,
//    Executor.LTS,
//    Executor.GTS,
//    Executor.LES,
//    Executor.GES,
//    Executor.EQA,
//    Executor.NEA,
//    Executor.EQD,
//    Executor.NED,
//    Executor.EQP,
//    Executor.NEP,
//    Executor.ANDB,
//    Executor.ORB,
//    Executor.NOTB,
//    Executor.INDEXAR,
//    Executor.INDEXAW,
//    Executor.INDEXAV,
//    Executor.INDEXAN,
//    Executor.INDEXDR,
//    Executor.INDEXDW,
//    Executor.INDEXDV,
//    Executor.INA,
//    Executor.IND,
//    Executor.CALLP,
//    Executor.CALLF,
//    Executor.CALLMF,
//    Executor.CALLI,
//    Executor.JUMP,
//    Executor.JF,
//    Executor.JT,
//    Executor.JFCHAIN,
//    Executor.DUP,
//    Executor.DUPX1,
//    Executor.DROP,
//    Executor.RET,
//    Executor.CALLE,
//    Executor.CONSA,
//    Executor.CONSD,
//    Executor.EXCEPT,
//    Executor.ALLOC,
//    Executor.PUSHNIL,
//    Executor.JNASSERT,
//    Executor.RESETC,
//    Executor.PUSHPEG,
//    Executor.JUMPTBL,
//    Executor.CALLX,
//    Executor.SWAP,
//    Executor.DROPN,
//    Executor.PUSHM,
//    Executor.CALLV,
//    Executor.PUSHCI,
//]

