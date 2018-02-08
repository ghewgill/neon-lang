#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcode.h"
#include "stack.h"

#define error(s) fprintf(stderr, "%s\n", s); abort();

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

static short get_uint16(const unsigned char *pobj, unsigned int nBuffSize, unsigned int *i)
{
    if (*i+2 > nBuffSize) {
        error("Bytecode exception: Read past EOF.");
    }
    short r = (pobj[*i] << 8) | pobj[*i+1];
    *i += 2;
    return r;
}

static unsigned int get_uint32(const unsigned char *pobj, unsigned int nBuffSize, unsigned int *i)
{
    if (*i+4 > nBuffSize) {
        error("Bytecode excpetion: Read past EOF.");
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
        return 1;
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
    error("not implemented");}

void exec_LEAVE()
{
    error("not implemented");}

void exec_PUSHB()
{
    error("not implemented");}

void exec_PUSHN()
{
    error("not implemented");
}

void exec_PUSHS(struct tagTExecutor *self)
{
    uint32_t val = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    self->ip += 5;
    push(&self->stack, self->object->strtable[val]);
}

void exec_PUSHPG()
{
    error("not implemented");
}

void exec_PUSHPPG()
{
    error("not implemented");
}

void exec_PUSHPMG()
{
    error("not implemented");
}

void exec_PUSHPL()
{
    error("not implemented");
}

void exec_PUSHPOL()
{
    error("not implemented");
}

void exec_PUSHI()
{
    error("not implemented");
}

void exec_LOADB()
{
    error("not implemented");
}

void exec_LOADN()
{
    error("not implemented");
}

void exec_LOADS()
{
    error("not implemented");
}

void exec_LOADA()
{
    error("not implemented");
}

void exec_LOADD()
{
    error("not implemented");
}

void exec_LOADP()
{
    error("not implemented");
}

void exec_STOREB()
{
    error("not implemented");
}

void exec_STOREN()
{
    error("not implemented");
}

void exec_STORES()
{
    error("not implemented");
}

void exec_STOREA()
{
    error("not implemented");
}

void exec_STORED()
{
    error("not implemented");
}

void exec_STOREP()
{
    error("not implemented");
}

void exec_NEGN()
{
    error("not implemented");
}

void exec_ADDN()
{
    error("not implemented");
}

void exec_SUBN()
{
    error("not implemented");
}

void exec_MULN()
{
    error("not implemented");
}

void exec_DIVN()
{
    error("not implemented");
}

void exec_MODN()
{
    error("not implemented");
}

void exec_EXPN()
{
    error("not implemented");
}

void exec_EQB()
{
    error("not implemented");
}

void exec_NEB()
{
    error("not implemented");
}

void exec_EQN()
{
    error("not implemented");
}

void exec_NEN()
{
    error("not implemented");
}

void exec_LTN()
{
    error("not implemented");
}

void exec_GTN()
{
    error("not implemented");
}

void exec_LEN()
{
    error("not implemented");
}

void exec_GEN()
{
    error("not implemented");
}

void exec_EQS()
{
    error("not implemented");
}

void exec_NES()
{
    error("not implemented");
}

void exec_LTS()
{
    error("not implemented");
}

void exec_GTS()
{
    error("not implemented");
}

void exec_LES()
{
    error("not implemented");
}

void exec_GES()
{
    error("not implemented");
}

void exec_EQA()
{
    error("not implemented");
}

void exec_NEA()
{
    error("not implemented");
}

void exec_EQD()
{
    error("not implemented");
}

void exec_NED()
{
    error("not implemented");
}

void exec_EQP()
{
    error("not implemented");
}

void exec_NEP()
{
    error("not implemented");
}

void exec_ANDB()
{
    error("not implemented");
}

void exec_ORB()
{
    error("not implemented");
}

void exec_NOTB()
{
    error("not implemented");
}

void exec_INDEXAR()
{
    error("not implemented");
}

void exec_INDEXAW()
{
    error("not implemented");
}

void exec_INDEXAV()
{
    error("not implemented");
}

void exec_INDEXAN()
{
    error("not implemented");
}

void exec_INDEXDR()
{
    error("not implemented");
}

void exec_INDEXDW()
{
    error("not implemented");
}

void exec_INDEXDV()
{
    error("not implemented");
}

void exec_INA()
{
    error("not implemented");
}

void exec_IND()
{
    error("not implemented");
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
        error("exec_CALLP(): Invalid function call");
    }
}

void exec_CALLF()
{
    error("not implemented");
}

void exec_CALLMF()
{
    error("not implemented");
}

void exec_CALLI()
{
    error("not implemented");
}

void exec_JUMP()
{
    error("not implemented");
}

void exec_JF()
{
    error("not implemented");
}

void exec_JT()
{
    error("not implemented");
}

void exec_JFCHAIN()
{
    error("not implemented");
}

void exec_DUP()
{
    error("not implemented");
}

void exec_DUPX1()
{
    error("not implemented");
}

void exec_DROP()
{
    error("not implemented");
}

void exec_RET(struct tagTExecutor *self)
{
    const char *code = pop(&self->callstack);
    self->ip = (code[3] << 24) | (code[2] << 16) | (code[1] << 8) | code[0];
}

void exec_CALLE()
{
    error("not implemented");
}

void exec_CONSA()
{
    error("not implemented");
}

void exec_CONSD()
{
    error("not implemented");
}

void exec_EXCEPT()
{
    error("not implemented");
}

void exec_ALLOC()
{
    error("not implemented");
}

void exec_PUSHNIL()
{
    error("not implemented");
}

void exec_JNASSERT()
{
    error("not implemented");
}

void exec_RESETC()
{
    error("not implemented");
}

void exec_PUSHPEG()
{
    error("not implemented");
}

void exec_JUMPTBL()
{
    error("not implemented");
}

void exec_CALLX()
{
    error("not implemented");
}

void exec_SWAP()
{
    error("not implemented");
}

void exec_DROPN()
{
    error("not implemented");
}

void exec_PUSHM()
{
    error("not implemented");
}

void exec_CALLV()
{
    error("not implemented");}

void exec_PUSHCI()
{
    error("not implemented");
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
