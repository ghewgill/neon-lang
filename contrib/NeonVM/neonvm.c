#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4001)
#endif

#ifdef __MS_HEAP_DBG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#ifndef __MS_HEAP_DBG
#include <stdlib.h>
#endif
#include <string.h>

#include "cell.h"
#include "global.h"
#include "framestack.h"
#include "number.h"
#include "opcode.h"
#include "stack.h"
#include "string.h"
#include "util.h"


typedef struct tagTCommandLineOptions {
    BOOL ExecutorDebugStats;
    char *pszFilename;
    char *pszExecutableName;
} TOptions;

static TOptions gOptions = { FALSE, NULL, NULL };

static uint16_t get_uint16(const uint8_t *pobj, size_t nBuffSize, uint32_t *i)
{
    if (*i+2 > nBuffSize) {
        fatal_error("Bytecode exception: Read past EOF.");
    }
    uint16_t r = (pobj[*i] << 8) | pobj[*i+1];
    *i += 2;
    return r;
}

static uint32_t get_uint32(const uint8_t *pobj, size_t nBuffSize, uint32_t *i)
{
    if (*i+4 > nBuffSize) {
        fatal_error("Bytecode excpetion: Read past EOF.");
    }
    uint32_t r = (pobj[*i] << 24) | (pobj[*i+1] << 16) | (pobj[*i+2] << 8) | pobj[*i+3];
    *i += 4;
    return r;
}

typedef struct tagTType {
    int16_t name;
    int16_t descriptor;
} Type;

typedef struct tagTFunction {
    uint16_t name;
    uint32_t entry;
} Function;

typedef struct tagTExportFunction {
    uint16_t name;
    uint16_t descriptor;
    uint32_t entry;
} ExportFunction;

typedef struct tagTImport {
    uint16_t name;
    uint8_t hash[32];
} Import;

typedef struct tagTBytecode {
    uint8_t source_hash[32];
    uint16_t global_size;
    uint32_t strtablesize;
    uint32_t strtablelen;
    TString **strings;
    char **strtable;
    uint32_t typesize;
    uint32_t constantsize;
    uint32_t variablesize;
    uint32_t export_functionsize;
    uint32_t functionsize;
    uint32_t exceptionsize;
    uint32_t exceptionexportsize;
    uint32_t interfaceexportsize;
    uint32_t importsize;
    uint32_t classsize;
    const uint8_t *code;
    size_t codelen;

    struct Type *pExportTypes;
    struct tagTExportFunction *export_functions;
    struct tagTFunction *pFunctions;
    struct tagTImport  *imports;
} TBytecode;

static char **getstrtable(TBytecode *pBytecode, const uint8_t *start, const uint8_t *end, uint32_t *count)
{
    char **r = NULL;
    uint32_t i = 0;

    /* First, initialize the string count to zero. */
    *count = 0;
    /* ToDo: Do this in a single pass, a linked list of strings, perhaps.
    /  We're going to iterate the string table first, to get the count,
    /  then we'll allocate the data. */
    const uint8_t *s = start;
    while (s != end) {
        s += ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3]) + 4;
        (*count)++;
    }

    r = malloc((sizeof(uint8_t *)) * *count);
    if (r == NULL) {
        fatal_error("Could not allocate memory for %d strings.", *count);
    }
    pBytecode->strings = malloc(sizeof(TString *) * *count);

    while (start != end) {
        TString *ts = malloc(sizeof(TString));
        size_t len = (start[0] << 24) | (start[1] << 16) | (start[2] << 8) | start[3];
        start += 4;

        ts->length = len;
        ts->data = malloc(len+1); /* Always add null termination regardless of length. */
        //r[i] = malloc(len+1);
        //if (r[i] == NULL) {
        //    fatal_error("Could not allocate %d bytes for string index %d in string table.", len + 1, i);
        //}
        //memcpy(r[i], start, len);
        memcpy(ts->data, start, len);
        //r[i][len] = '\0';
        ts->data[len] = '\0'; /* Null terminate all strings, regardless of string type. */
        pBytecode->strings[i++] = ts;
        start += len;
    }
    return r;
}

struct tagTExecutor;

void exec_loop(struct tagTExecutor *self);
int exec_run(struct tagTExecutor *self);

TBytecode *bytecode_newBytecode()
{
    TBytecode *pBytecode = malloc(sizeof(TBytecode));
    if (!pBytecode) {
        fatal_error("Could not allocate memory for neon bytecode.");
    }

    memset(pBytecode, 0x00, sizeof(TBytecode));
    return pBytecode;
}

void bytecode_freeBytecode(TBytecode *b)
{
    uint32_t i;
    if (!b) {
        return;
    }

    free(b->pFunctions);
    free(b->imports);
    free(b->export_functions);
    free(b->pExportTypes);
    for (i = 0; i < b->strtablelen; i++) {
        free(b->strings[i]->data);
        free(b->strings[i]);
    }
    free(b->strtable);
    free(b->strings);
    free(b);
    b = NULL;
}

typedef struct tagTExecutor {
    struct tagTBytecode *object;
    uint32_t ip;
    struct tagTStack *stack;
    /*struct tagTStack *callstack; */
    uint32_t callstack[300];
    int32_t callstacktop;
    int32_t param_recursion_limit;
    Cell *globals;
    TFrameStack *framestack;
    //uint64_t frames;

    /* Debug / Diagnostic fields */
    uint64_t total_opcodes;
    uint64_t callstack_depth;
    clock_t time_start;
    clock_t time_end;
} TExecutor;

void exec_freeExecutor(TExecutor *e)
{
    uint32_t i;



    for (i = 0; i < e->object->global_size; i++) {
        cell_clearCell(&e->globals[i]);
    }
    free(e->globals);
    destroyStack(e->stack);
    free(e->stack);
    destroyFrameStack(e->framestack);
    free(e->framestack);

    bytecode_freeBytecode(e->object);

    free(e);
}

struct tagTExecutor *exec_newExecuter(struct tagTBytecode *object);

static void bytecode_loadBytecode(const uint8_t *bytecode, size_t len, struct tagTBytecode *pBytecode)
{
    uint32_t i = 0;

    if (!pBytecode) {
        pBytecode = bytecode_newBytecode();
    }

    memcpy(pBytecode->source_hash, bytecode, 32);
    i += 32;
    pBytecode->global_size = get_uint16(bytecode, len, &i);

    pBytecode->strtablesize = get_uint32(bytecode, len, &i);
    //i += 4;
    pBytecode->strtable = getstrtable(pBytecode, &bytecode[i], &bytecode[i + pBytecode->strtablesize], &pBytecode->strtablelen);
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
    pBytecode->export_functionsize = get_uint16(bytecode, len, &i);
    pBytecode->export_functions = malloc(sizeof(ExportFunction) * pBytecode->export_functionsize);
    if (pBytecode->export_functions == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
    }
    for (uint32_t f = 0; f < pBytecode->export_functionsize; f++) {
        pBytecode->export_functions[f].name = get_uint16(bytecode, len, &i);
        pBytecode->export_functions[f].descriptor = get_uint16(bytecode, len, &i);
        pBytecode->export_functions[f].entry = get_uint32(bytecode, len, &i);
    }
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
    pBytecode->imports = malloc(sizeof(Import) * pBytecode->importsize);
    if (pBytecode->imports == NULL) {
        fatal_error("Could not allocate memory for exported function info.");
    }
    for (uint32_t f = 0; f < pBytecode->importsize; f++) {
        pBytecode->imports[f].name = get_uint16(bytecode, len, &i);
        memcpy(pBytecode->imports[f].hash, bytecode, 32);
        i+=32;
    }
    pBytecode->functionsize = get_uint16(bytecode, len, &i);
    pBytecode->pFunctions = malloc(sizeof(Function) * pBytecode->functionsize);
    if (pBytecode->pFunctions == NULL) {
        fatal_error("Could not allocate memory for function info.");
    }
    for (uint32_t f = 0; f < pBytecode->functionsize; f++) {
        pBytecode->pFunctions[f].name = get_uint16(bytecode, len, &i);
        pBytecode->pFunctions[f].entry = get_uint32(bytecode, len, &i);
    }
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

BOOL Usage(BOOL bShowHelp)
{
    if(bShowHelp) {
        fprintf(stderr, "Usage:\n\n");
        fprintf(stderr, "   %s [options] program.neonx\n", gOptions.pszExecutableName);
        fprintf(stderr, "\n Where [options] is one or more of the following:\n");
        fprintf(stderr, "     -d       Display executor debug stats.\n");
        fprintf(stderr, "     -h       Display this help screen.\n");
        return FALSE;
    }
    fprintf(stderr, "\n");
    return TRUE;
}

BOOL ParseOptions(int argc, char* argv[])
{
    BOOL Retval = FALSE;
    for (int nIndex = 1; nIndex < argc; nIndex++) {
        if (*argv[nIndex] == '-') {
            if(argv[nIndex][1] == 'H' || argv[nIndex][1] == 'h' || argv[nIndex][1] == '?' || ((argv[nIndex][1] == '-' && argv[nIndex][2] != '\0') && (argv[nIndex][2] == 'H' || argv[nIndex][2] == 'h'))) {
                Usage(TRUE);
                exit(1);
            } else if (argv[nIndex][1] == 'D' || argv[nIndex][1] == 'd') {
                gOptions.ExecutorDebugStats = TRUE;
            }
        } else {
            Retval = TRUE;
            gOptions.pszFilename = strdup(argv[nIndex]);
        }
    }
    if (!gOptions.pszFilename) {
        printf("You must provide a Neon binary file to execute.\n");
        Retval = FALSE;
    }
    return Retval;
}

int main(int argc, char* argv[])
{
    char *p = argv[0];
    char *s = p;

    while (*p) {
        if (*p == '\\' || *p == '/') {
            s = p;
        }
        p++;
    }
    gOptions.pszExecutableName = ++s;

#ifdef __MS_HEAP_DBG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(119);
    //_CrtSetBreakAlloc(306);
    //_CrtSetBreakAlloc(307);
#endif
    if (!ParseOptions(argc, argv)) {
        return 3;
    }
    FILE *fp = fopen(gOptions.pszFilename, "rb");
    if (!fp) {
        fprintf(stderr, "Could not open Neon executable: %s\n Error: %d - %s.\n", gOptions.pszFilename, errno, strerror(errno));
        return 2;
    }
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    TBytecode *pModule = bytecode_newBytecode();
    uint8_t *bytecode = malloc(nSize);
    if (bytecode == NULL) {
        fatal_error("Could not allocate memory for neon bytecode.");
    }
    size_t bytes_read = fread(bytecode, 1, nSize, fp);

    bytecode_loadBytecode(bytecode, bytes_read, pModule);

    TExecutor *exec = exec_newExecuter(pModule);
    
    clock_t t;

    t = clock();
    exec->time_start = clock();
    exec_run(exec);
    exec->time_end = clock();

    if (gOptions.ExecutorDebugStats) {
        fprintf(stderr, "** Neon C Executor Statistics **\n--------------------------------\n"
                        "Total Opcodes Executed : %lld\n"
                        "Max Opstack Height     : %d\n"
                        "Opstack Height         : %d\n"
                        "Max Callstack Height   : %lld\n"
                        "CallStack Height       : %d\n"
                        "Global Size            : %d\n"
                        "Max Framesets          : %d\n"
                        "Execution Time         : %fms\n",
                        exec->total_opcodes,
                        exec->stack->max,
                        exec->stack->top,
                        exec->callstack_depth, 
                        exec->callstacktop, 
                        exec->object->global_size, 
                        exec->framestack->max,
                        (((float)exec->time_end - exec->time_start) / CLOCKS_PER_SEC) * 1000
        );
    }

    exec_freeExecutor(exec);
    free(bytecode);

    return 0;
}

int exec_run(struct tagTExecutor *self)
{
    self->callstack[++self->callstacktop] = (uint32_t)self->object->codelen;

    exec_loop(self);
    //assert(isEmpty(self->stack));

    return 0;
}

struct tagTExecutor *exec_newExecuter(struct tagTBytecode *object)
{
    uint32_t i;
    struct tagTExecutor *r = malloc(sizeof(struct tagTExecutor));
    if (r == NULL) {
        fatal_error("Failed to allocate memory for Neon executable.");
    }
    r->object = object;
    r->stack = createStack(300);
    r->ip = 0;
    r->callstacktop = -1;
    r->param_recursion_limit = 1000;
    r->framestack = createFrameStack(r->param_recursion_limit);
    r->globals = malloc(sizeof(Cell) * r->object->global_size);
    if (r->globals == NULL) {
        fatal_error("Failed to allocate memory for global variables.");
    }
    for (i = 0; i < r->object->global_size; i++) {
        cell_resetCell(&r->globals[i]);
    }

    // Debug / Diagnostic fields
    r->total_opcodes = 0;
    r->callstack_depth = 0;
    return r;
}

static uint32_t exec_getOperand(struct tagTExecutor *self)
{
    uint32_t r = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    self->ip += 5;
    return r;
}

typedef struct tagTActivationFrame {
    size_t count;
    Cell *locals;
} TActivationFrame;

void exec_ENTER(struct tagTExecutor *self)
{
    self->ip++;
    uint32_t nest = (self->object->code[self->ip] << 24) | (self->object->code[self->ip+1] << 16) | (self->object->code[self->ip+2] << 8) | self->object->code[self->ip+3];
    nest = nest; // ToDo: Please remove!
    self->ip += 4;
    uint32_t val = (self->object->code[self->ip] << 24) | (self->object->code[self->ip+1] << 16) | (self->object->code[self->ip+2] << 8) | self->object->code[self->ip+3];
    self->ip += 4;

    //self->frame_size = val;
    //self->frames = malloc(sizeof(Cell) * self->frame_size);
    //if (self->frames == NULL) {
    //    fatal_error("Failed to allocate memory for function local variables.");
    //}
    framestack_pushFrame(self->framestack, frame_createFrame(val));

    // ToDo: Implement Activiation frame support
    //add(frame_newFrame(val));
    //nested_frames.resize(nest-1);
    //nested_frames.push_back(&frames.back());
}

void exec_LEAVE(struct tagTExecutor *self)
{
    framestack_popFrame(self->framestack);
    self->ip++;
}

void exec_PUSHB(struct tagTExecutor *self)
{
    BOOL val = self->object->code[self->ip+1] != 0;
    self->ip += 2;
    push(self->stack, cell_fromBoolean(val));
}

void exec_PUSHN(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    push(self->stack, cell_fromNumber(bid128_from_string(self->object->strings[val]->data)));
}

void exec_PUSHS(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    push(self->stack, cell_fromString(self->object->strings[val]->data, self->object->strings[val]->length));
}

void exec_PUSHPG(struct tagTExecutor *self)
{
    uint32_t addr = exec_getOperand(self);
    assert(addr < self->object->global_size);
    push(self->stack, cell_fromAddress(&self->globals[addr]));
}

void exec_PUSHPPG()
{
    fatal_error("not implemented");
}

void exec_PUSHPMG()
{
    fatal_error("not implemented");
}

void exec_PUSHPL(struct tagTExecutor *self)
{
    uint32_t addr = exec_getOperand(self);
    //push(self->stack, cell_fromAddress(&self->frames[addr]));
    push(self->stack, cell_fromAddress(&framestack_topFrame(self->framestack)->locals[addr]));
}

void exec_PUSHPOL()
{
    fatal_error("not implemented");
}

void exec_PUSHI()
{
    fatal_error("not implemented");
}

void exec_LOADB(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADN(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADS(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADA(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADD(struct tagTExecutor *self)
{
    self = self;
    fatal_error("not implemented");
}

void exec_LOADP(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_STOREB(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREN(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STORES(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREA(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STORED()
{
    fatal_error("not implemented");
}

void exec_STOREP(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_NEGN(struct tagTExecutor *self)
{
    self->ip++;
    Number x = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_negate(x)));
}

void exec_ADDN(struct tagTExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_add(a, b)));
}

void exec_SUBN(struct tagTExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_sub(a, b)));
}

void exec_MULN(struct tagTExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_mul(a, b)));
}

void exec_DIVN(struct tagTExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_div(a, b)));
}

void exec_MODN(struct tagTExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_modulo(a, b)));
}

void exec_EXPN(struct tagTExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_pow(a, b)));
}

void exec_EQB()
{
    fatal_error("not implemented");
}

void exec_NEB()
{
    fatal_error("not implemented");
}

void exec_EQN(struct tagTExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_equal(a, b)));
}

void exec_NEN(struct tagTExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_not_equal(a, b)));
}

void exec_LTN(struct tagTExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_less(a, b)));
}

void exec_GTN(struct tagTExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_greater(a, b)));
}

void exec_LEN(struct tagTExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_less_equal(a, b)));
}

void exec_GEN(struct tagTExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_greater_equal(a, b)));
}

void exec_EQS(struct tagTExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) == 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_NES(struct tagTExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) != 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LTS(struct tagTExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) < 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GTS(struct tagTExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) > 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LES(struct tagTExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) <= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GES(struct tagTExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) >= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
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

void exec_NOTB(struct tagTExecutor *self)
{
    self->ip++;
    BOOL x = top(self->stack)->boolean; pop(self->stack);
    push(self->stack, cell_fromBoolean(!x));
}

void exec_INDEXAR(struct tagTExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *addr = top(self->stack)->address; pop(self->stack);

    if (!number_is_integer(index)) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = bid128_to_int64_int(index);
    if (i < 0) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
    }
    uint64_t j = (uint64_t)i;
    if (j >= addr->array_size) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(j)));
        return;
    }
    push(self->stack, cell_fromAddress(cell_arrayIndexForRead(addr, j)));
}

void exec_INDEXAW(struct tagTExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *addr = top(self->stack)->address; pop(self->stack);

    if (!number_is_integer(index)) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = bid128_to_int64_int(index);
    if (i < 0) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
    }
    uint64_t j = (uint64_t)i;
    push(self->stack, cell_fromAddress(cell_arrayIndexForWrite(addr, j)));
}

void exec_INDEXAV(struct tagTExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *array = top(self->stack);

    if (!number_is_integer(index)) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = bid128_to_int64_int(index);
    if (i < 0) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
    }
    uint64_t j = (uint64_t)i;
    if (j >= array->array_size) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(j)));
        return;
    }
    assert(j < array->array_size);
    Cell *val = cell_fromCell(&array->array[j]);
    pop(self->stack);
    push(self->stack, val);
}

void exec_INDEXAN(struct tagTExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *array = top(self->stack);

    if (!number_is_integer(index)) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(number_to_string(index)));
        return;
    }
    int64_t i = bid128_to_int64_int(index);
    if (i < 0) {
        // ToDo: Implement runtime exceptions
        assert(FALSE);
        //raise(rtl::global::Exception_ArrayIndexException, ExceptionInfo(std::to_string(i)));
        return;
    }
    uint64_t j = (uint64_t)i;
    Cell *val = (j < array->array_size ? cell_fromCell(&array->array[j]) : cell_newCell());
    pop(self->stack);
    push(self->stack, val);
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

void exec_INA(struct tagTExecutor *self)
{
    self->ip++;
    Cell *array = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *val = cell_fromCell(top(self->stack)); pop(self->stack);

    BOOL v = cell_arrayElementExists(array, val);
    cell_freeCell(val);
    cell_freeCell(array);

    push(self->stack, cell_fromBoolean(v));
}

void exec_IND()
{
    fatal_error("not implemented");
}

void exec_CALLP(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    const char *func = self->object->strings[val]->data;

    if (!global_callFunction(func, self->stack)) {
        fatal_error("exec_CALLP(): \"%s\" - invalid or unsupported predefined function call.", func);
    }
}

void exec_CALLF(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    if (self->callstacktop >= self->param_recursion_limit) {
        // ToDo: handle runtime exceptions
        //raise(rtl::global::Exception_StackOverflowException, ExceptionInfo(""));
        //return;
    }
    self->callstack[++self->callstacktop] = self->ip;
    self->ip = val;
}

void exec_CALLMF()
{
    fatal_error("not implemented");
}

void exec_CALLI()
{
    fatal_error("not implemented");
}

void exec_JUMP(struct tagTExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    self->ip = target;
}

void exec_JF(struct tagTExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    if (!a) {
        self->ip = target;
    }
}

void exec_JT(struct tagTExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    if (a) {
        self->ip = target;
    }
}

void exec_JFCHAIN(struct tagTExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    if (!a->boolean) {
        self->ip = target;
        pop(self->stack);
        push(self->stack, a);
        return;
    }
    cell_freeCell(a);
}

void exec_DUP(struct tagTExecutor *self)
{
    self->ip++;
    push(self->stack, cell_fromCell(top(self->stack)));
}

void exec_DUPX1(struct tagTExecutor*self)
{
    self->ip++;
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    push(self->stack, a);
    push(self->stack, b);
    push(self->stack, cell_fromCell(a));
}

void exec_DROP(struct tagTExecutor *self)
{
    self->ip++;
    pop(self->stack);
}

void exec_RET(struct tagTExecutor *self)
{
    self->ip = self->callstack[self->callstacktop--];
}

void exec_CALLE()
{
    fatal_error("not implemented");
}

void exec_CONSA(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    Cell *a = cell_createArrayCell(val);

    while (val > 0) {
        cell_copyCell(&a->array[a->array_size - val], top(self->stack));
        val--;
        pop(self->stack);
    }
    push(self->stack, a);
}

void exec_CONSD(struct tagTExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    Cell *a = cell_createDictionaryCell(val);

    //while (val > 0) {
    //    cell_copyCell(&a->array[a->array_size - val], top(self->stack));
    //    val--;
    //    pop(self->stack);
    //}
    push(self->stack, a);

    //uint32_t val = (module->object.code[ip+1] << 24) | (module->object.code[ip+2] << 16) | (module->object.code[ip+3] << 8) | module->object.code[ip+4];
    //ip += 5;
    //Cell d;
    //while (val > 0) {
    //    Cell value = stack.top(); stack.pop();
    //    utf8string key = stack.top().string(); stack.pop();
    //    d.dictionary_index_for_write(key) = value;
    //    val--;
    //}
    //stack.push(d);
}

void exec_EXCEPT()
{
    fatal_error("not implemented");
}

void exec_ALLOC()
{
    fatal_error("not implemented");
}

void exec_PUSHNIL(struct tagTExecutor *self)
{
    self->ip++;
    push(self->stack, NULL);
}

void exec_JNASSERT()
{
    fatal_error("not implemented");
}

void exec_RESETC(struct tagTExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_resetCell(addr);
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
            case ENTER:   exec_ENTER(self); break;
            case LEAVE:   exec_LEAVE(self); break;
            case PUSHB:   exec_PUSHB(self); break;
            case PUSHN:   exec_PUSHN(self); break;
            case PUSHS:   exec_PUSHS(self); break;
            case PUSHPG:  exec_PUSHPG(self); break;
            case PUSHPPG: exec_PUSHPPG(); break;
            case PUSHPMG: exec_PUSHPMG(); break;
            case PUSHPL:  exec_PUSHPL(self); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case PUSHI:   exec_PUSHI(); break;
            case LOADB:   exec_LOADB(self); break;
            case LOADN:   exec_LOADN(self); break;
            case LOADS:   exec_LOADS(self); break;
            case LOADA:   exec_LOADA(self); break;
            case LOADD:   exec_LOADD(self); break;
            case LOADP:   exec_LOADP(self); break;
            case STOREB:  exec_STOREB(self); break;
            case STOREN:  exec_STOREN(self); break;
            case STORES:  exec_STORES(self); break;
            case STOREA:  exec_STOREA(self); break;
            case STORED:  exec_STORED(); break;
            case STOREP:  exec_STOREP(self); break;
            case NEGN:    exec_NEGN(self); break;
            case ADDN:    exec_ADDN(self); break;
            case SUBN:    exec_SUBN(self); break;
            case MULN:    exec_MULN(self); break;
            case DIVN:    exec_DIVN(self); break;
            case MODN:    exec_MODN(self); break;
            case EXPN:    exec_EXPN(self); break;
            case EQB:     exec_EQB(); break;
            case NEB:     exec_NEB(); break;
            case EQN:     exec_EQN(self); break;
            case NEN:     exec_NEN(self); break;
            case LTN:     exec_LTN(self); break;
            case GTN:     exec_GTN(self); break;
            case LEN:     exec_LEN(self); break;
            case GEN:     exec_GEN(self); break;
            case EQS:     exec_EQS(self); break;
            case NES:     exec_NES(self); break;
            case LTS:     exec_LTS(self); break;
            case GTS:     exec_GTS(self); break;
            case LES:     exec_LES(self); break;
            case GES:     exec_GES(self); break;
            case EQA:     exec_EQA(); break;
            case NEA:     exec_NEA(); break;
            case EQD:     exec_EQD(); break;
            case NED:     exec_NED(); break;
            case EQP:     exec_EQP(); break;
            case NEP:     exec_NEP(); break;
            case ANDB:    exec_ANDB(); break;
            case ORB:     exec_ORB(); break;
            case NOTB:    exec_NOTB(self); break;
            case INDEXAR: exec_INDEXAR(self); break;
            case INDEXAW: exec_INDEXAW(self); break;
            case INDEXAV: exec_INDEXAV(self); break;
            case INDEXAN: exec_INDEXAN(self); break;
            case INDEXDR: exec_INDEXDR(); break;
            case INDEXDW: exec_INDEXDW(); break;
            case INDEXDV: exec_INDEXDV(); break;
            case INA:     exec_INA(self); break;
            case IND:     exec_IND(); break;
            case CALLP:   exec_CALLP(self); break;
            case CALLF:   exec_CALLF(self); break;
            case CALLMF:  exec_CALLMF(); break;
            case CALLI:   exec_CALLI(); break;
            case JUMP:    exec_JUMP(self); break;
            case JF:      exec_JF(self); break;
            case JT:      exec_JT(self); break;
            case JFCHAIN: exec_JFCHAIN(self); break;
            case DUP:     exec_DUP(self); break;
            case DUPX1:   exec_DUPX1(self); break;
            case DROP:    exec_DROP(self); break;
            case RET:     exec_RET(self); break;
            case CALLE:   exec_CALLE(); break;
            case CONSA:   exec_CONSA(self); break;
            case CONSD:   exec_CONSD(self); break;
            case EXCEPT:  exec_EXCEPT(); break;
            case ALLOC:   exec_ALLOC(); break;
            case PUSHNIL: exec_PUSHNIL(self); break;
            case JNASSERT:exec_JNASSERT(); break;
            case RESETC:  exec_RESETC(self); break;
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
        self->total_opcodes++;
    }
}

#ifdef _MSC_VER
#pragma warning (pop)
#endif
