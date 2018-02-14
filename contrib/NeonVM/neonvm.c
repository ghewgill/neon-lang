#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcode.h"
#include "stack.h"
#include "util.h"

typedef unsigned char       uint8_t;
typedef signed   char       int8_t;
typedef unsigned short      uint16_t;
typedef signed   short      int16_t;
typedef unsigned int        uint32_t;
typedef signed   int        int32_t;

static uint16_t get_uint16(const uint8_t *pobj, uint32_t nBuffSize, uint32_t *i)
{
    if (*i+2 > nBuffSize) {
        fatal_error("Bytecode exception: Read past EOF.");
    }
    uint16_t r = (pobj[*i] << 8) | pobj[*i+1];
    *i += 2;
    return r;
}

static uint32_t get_uint32(const uint8_t *pobj, uint32_t nBuffSize, uint32_t *i)
{
    if (*i+4 > nBuffSize) {
        fatal_error("Bytecode excpetion: Read past EOF.");
    }
    uint32_t r = (pobj[*i] << 24) | (pobj[*i+1] << 16) | (pobj[*i+2] << 8) | pobj[*i+3];
    *i += 4;
    return r;
}

struct NString {
    uint32_t length;
    int8_t *data;
};

static uint8_t **getstrtable(const uint8_t *start, const uint8_t *end, uint32_t *count)
{
    uint8_t **r = NULL;
    uint32_t i = 0;

    // First, initialize the string count to zero.
    *count = 0;
    // We're going to iterate the string table first, to get the count,
    // then we'll allocate the data.
    const uint8_t *s = start;
    while (s != end) {
        s += ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3]) + 4;
        (*count)++;
    }

    r = malloc((sizeof(uint8_t *)) * *count);
    if (r == NULL) {
        fatal_error("Could not allocate memory for %d strings.", *count);
    }

    while (start != end) {
        size_t len = (start[0] << 24) | (start[1] << 16) | (start[2] << 8) | start[3];
        start += 4;

        r[i] = malloc(len+1);
        if (r[i] == NULL) {
            fatal_error("Could not allocate %d bytes for string index %d in string table.", len + 1, i);
        }
        memcpy(r[i], start, len);
        r[i][len] = '\0';
        i++;
        start += len;
    }
    return r;
}

typedef struct tagTType {
    int16_t name;
    int16_t descriptor;
} Type;

typedef struct tagTBytecode {
    uint8_t source_hash[32];
    uint16_t global_size;
    uint32_t strtablesize;
    uint32_t strtablelen;
    uint8_t **strtable;
    uint32_t typesize;
    uint32_t constantsize;
    uint32_t variablesize;
    uint32_t functionsize;
    uint32_t exceptionsize;
    uint32_t exceptionexportsize;
    uint32_t interfaceexportsize;
    uint32_t importsize;
    uint32_t classsize;
    const uint8_t *code;
    uint32_t codelen;

    struct Type *pExportTypes;
} TBytecode;

struct tagTExecutor;

void exec_loop(struct tagTExecutor *self);
void exec_run(struct tagTExecutor *self);

typedef struct tagTExecutor {
    struct tagTBytecode *object;
    uint32_t ip;
    struct tagTStack *stack;
    struct tagTStack *callstack;
} TExecutor;

struct tagTExecutor *new_executer(struct tagTBytecode *object);

static void loadBytecode(const uint8_t *bytecode, uint32_t len, struct tagTBytecode *pBytecode)
{
    uint32_t i = 0;

    memcpy(pBytecode->source_hash, bytecode, 32);
    i += 32;
    pBytecode->global_size = get_uint16(bytecode, len, &i);

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
        printf("\nusage: %s program.neonx\n", argv[0]);
        return 1;
    }
    struct tagTBytecode *pModule;
    FILE *fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    pModule = malloc((sizeof(struct tagTBytecode)));
    if (pModule == NULL) {
        fatal_error("Could not allocate memory for neon executable.  Error %s", _strerror(NULL));
    }
    uint8_t *bytecode = malloc(nSize);
    if (bytecode == NULL) {
        fatal_error("Could not allocate memory for neon bytecode.  Error %s.", _strerror(NULL));
    }
    size_t bytes_read = fread(bytecode, 1, nSize, fp);

    loadBytecode(bytecode, bytes_read, pModule);

    struct tagTExecutor *exec = new_executer(pModule);
    exec_run(exec);
}

void exec_run(struct tagTExecutor *self)
{
    uint32_t i;
    push(self->callstack, &self->object->codelen);
    exec_loop(self);

    for (i = 0; i < self->object->strtablelen; i++) {
        free(self->object->strtable[i]);
    }
}

struct tagTExecutor *new_executer(struct tagTBytecode *object)
{
    struct tagTExecutor *r = malloc(sizeof(struct tagTExecutor));
    if (r == NULL) {
        fatal_error("Failed to allocate memory for Neon executable.  Error: ", _strerror(NULL));
    }
    r->object = object;
    r->callstack = createStack(300);
    r->stack = createStack(300);
    r->ip = 0;
    return r;
}

static uint32_t exec_getOperand(struct tagTExecutor *self)
{
    uint32_t r = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    self->ip += 5;
    return r;
}

void exec_ENTER()
{
    exec_error("not implemented");
}

void exec_LEAVE()
{
    exec_error("not implemented");
}

void exec_PUSHB()
{
    exec_error("not implemented");
}

void exec_PUSHN()
{
    exec_error("not implemented");
}

void exec_PUSHS(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    push(self->stack, self->object->strtable[val]);
}

void exec_PUSHPG()
{
    fatal_error("not implemented");
}

void exec_PUSHPPG()
{
    fatal_error("not implemented");
}

void exec_PUSHPMG()
{
    fatal_error("not implemented");
}

void exec_PUSHPL()
{
    fatal_error("not implemented");
}

void exec_PUSHPOL()
{
    fatal_error("not implemented");
}

void exec_PUSHI()
{
    fatal_error("not implemented");
}

void exec_LOADB()
{
    fatal_error("not implemented");
}

void exec_LOADN()
{
    fatal_error("not implemented");
}

void exec_LOADS()
{
    fatal_error("not implemented");
}

void exec_LOADA()
{
    fatal_error("not implemented");
}

void exec_LOADD()
{
    fatal_error("not implemented");
}

void exec_LOADP()
{
    fatal_error("not implemented");
}

void exec_STOREB()
{
    fatal_error("not implemented");
}

void exec_STOREN()
{
    fatal_error("not implemented");
}

void exec_STORES()
{
    fatal_error("not implemented");
}

void exec_STOREA()
{
    fatal_error("not implemented");
}

void exec_STORED()
{
    fatal_error("not implemented");
}

void exec_STOREP()
{
    fatal_error("not implemented");
}

void exec_NEGN()
{
    fatal_error("not implemented");
}

void exec_ADDN()
{
    fatal_error("not implemented");
}

void exec_SUBN()
{
    fatal_error("not implemented");
}

void exec_MULN()
{
    fatal_error("not implemented");
}

void exec_DIVN()
{
    fatal_error("not implemented");
}

void exec_MODN()
{
    fatal_error("not implemented");
}

void exec_EXPN()
{
    fatal_error("not implemented");
}

void exec_EQB()
{
    fatal_error("not implemented");
}

void exec_NEB()
{
    fatal_error("not implemented");
}

void exec_EQN()
{
    fatal_error("not implemented");
}

void exec_NEN()
{
    fatal_error("not implemented");
}

void exec_LTN()
{
    fatal_error("not implemented");
}

void exec_GTN()
{
    fatal_error("not implemented");
}

void exec_LEN()
{
    fatal_error("not implemented");
}

void exec_GEN()
{
    fatal_error("not implemented");
}

void exec_EQS()
{
    fatal_error("not implemented");
}

void exec_NES()
{
    fatal_error("not implemented");
}

void exec_LTS()
{
    fatal_error("not implemented");
}

void exec_GTS()
{
    fatal_error("not implemented");
}

void exec_LES()
{
    fatal_error("not implemented");
}

void exec_GES()
{
    fatal_error("not implemented");
}

void exec_EQA()
{
    fatal_error("not implemented");
}

void exec_NEA()
{
    fatal_error("not implemented");
}

void exec_EQD()
{
    fatal_error("not implemented");
}

void exec_NED()
{
    fatal_error("not implemented");
}

void exec_EQP()
{
    fatal_error("not implemented");
}

void exec_NEP()
{
    fatal_error("not implemented");
}

void exec_ANDB()
{
    fatal_error("not implemented");
}

void exec_ORB()
{
    fatal_error("not implemented");
}

void exec_NOTB()
{
    fatal_error("not implemented");
}

void exec_INDEXAR()
{
    fatal_error("not implemented");
}

void exec_INDEXAW()
{
    fatal_error("not implemented");
}

void exec_INDEXAV()
{
    fatal_error("not implemented");
}

void exec_INDEXAN()
{
    fatal_error("not implemented");
}

void exec_INDEXDR()
{
    fatal_error("not implemented");
}

void exec_INDEXDW()
{
    fatal_error("not implemented");
}

void exec_INDEXDV()
{
    fatal_error("not implemented");
}

void exec_INA()
{
    fatal_error("not implemented");
}

void exec_IND()
{
    fatal_error("not implemented");
}

void exec_CALLP(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    const uint8_t *func = self->object->strtable[val];

    if (strcmp((const char*)func, "print") == 0) {
        const char *s = pop(self->stack);
        printf("%s\n", s);
    } else {
        exec_error("exec_CALLP(): \"%s\" - invalid or unsupported predefined function call.", func);
    }
}

void exec_CALLF()
{
    fatal_error("not implemented");
}

void exec_CALLMF()
{
    fatal_error("not implemented");
}

void exec_CALLI()
{
    fatal_error("not implemented");
}

void exec_JUMP()
{
    fatal_error("not implemented");
}

void exec_JF()
{
    fatal_error("not implemented");
}

void exec_JT()
{
    fatal_error("not implemented");
}

void exec_JFCHAIN()
{
    fatal_error("not implemented");
}

void exec_DUP()
{
    fatal_error("not implemented");
}

void exec_DUPX1()
{
    fatal_error("not implemented");
}

void exec_DROP()
{
    fatal_error("not implemented");
}

void exec_RET(struct tagTExecutor *self)
{
    self->ip = *(uint32_t*)pop(self->callstack);
}

void exec_CALLE()
{
    fatal_error("not implemented");
}

void exec_CONSA()
{
    fatal_error("not implemented");
}

void exec_CONSD()
{
    fatal_error("not implemented");
}

void exec_EXCEPT()
{
    fatal_error("not implemented");
}

void exec_ALLOC()
{
    fatal_error("not implemented");
}

void exec_PUSHNIL()
{
    fatal_error("not implemented");
}

void exec_JNASSERT()
{
    fatal_error("not implemented");
}

void exec_RESETC()
{
    fatal_error("not implemented");
}

void exec_PUSHPEG()
{
    fatal_error("not implemented");
}

void exec_JUMPTBL()
{
    fatal_error("not implemented");
}

void exec_CALLX()
{
    fatal_error("not implemented");
}

void exec_SWAP()
{
    fatal_error("not implemented");
}

void exec_DROPN()
{
    fatal_error("not implemented");
}

void exec_PUSHM()
{
    fatal_error("not implemented");
}

void exec_CALLV()
{
    fatal_error("not implemented");
}

void exec_PUSHCI()
{
    fatal_error("not implemented");
}

void exec_loop(struct tagTExecutor *self)
{
    while (self->ip < self->object->codelen) {
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
                fatal_error("exec: Unexpected opcode: %d\n", self->object->code[self->ip]);
        }
    }
}
