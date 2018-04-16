#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4001)      /* Disable single line comment warnings that appear in MS header files. */
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

#include "bytecode.h"
#include "cell.h"
#include "exec.h"
#include "global.h"
#include "framestack.h"
#include "number.h"
#include "opcode.h"
#include "stack.h"
#include "nstring.h"
#include "util.h"

#define EXECUTOR_DEBUG_OUTPUT       TRUE


typedef struct tagTCommandLineOptions {
    BOOL ExecutorDebugStats;
    BOOL EnableAssertions;
    char *pszFilename;
    char *pszExecutableName;
} TOptions;

static TOptions gOptions = { FALSE, TRUE, NULL, NULL };

void exec_freeExecutor(TExecutor *e)
{
    uint32_t i;

    for (i = 0; i < e->object->global_size; i++) {
        cell_clearCell(&e->globals[i]);
    }
    free(e->globals);
    destroyStack(e->stack);
    free(e->stack);
    framestack_destroyFrameStack(e->framestack);
    free(e->framestack);
    free(e->module);

    bytecode_freeBytecode(e->object);

    free(e);
}

TExecutor *exec_newExecutor(TBytecode *object);

BOOL Usage(BOOL bShowHelp)
{
    if(bShowHelp) {
        fprintf(stderr, "Usage:\n\n");
        fprintf(stderr, "   %s [options] program.neonx\n", gOptions.pszExecutableName);
        fprintf(stderr, "\n Where [options] is one or more of the following:\n");
        fprintf(stderr, "     -d       Display executor debug stats.\n");
        fprintf(stderr, "     -h       Display this help screen.\n");
        fprintf(stderr, "     -n       No Assertions\n");
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
            } else if (argv[nIndex][1] == 'N' || argv[nIndex][1] == 'n') {
                gOptions.EnableAssertions = FALSE;
            }
        } else {
            Retval = TRUE;
            gOptions.pszFilename = argv[nIndex];
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
    clock_t t;
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
    /* ToDo: Remove this!  This is only for debugging. */
    /* gOptions.ExecutorDebugStats = TRUE; */
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetBreakAlloc(107);
#endif
    if (!ParseOptions(argc, argv)) {
        return 3;
    }
    FILE *fp = fopen(gOptions.pszFilename, "rb");
    if (!fp) {
        fprintf(stderr, "Could not open Neon executable: %s\nError: %d - %s.\n", gOptions.pszFilename, errno, strerror(errno));
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

    bytecode_loadBytecode(pModule, bytecode, bytes_read);

    TExecutor *exec = exec_newExecutor(pModule);

    t = clock();
    exec->time_start = clock();
    exec_run(exec, gOptions.EnableAssertions);
    exec->time_end = clock();

    if (gOptions.ExecutorDebugStats) {
        fprintf(stderr, "\n*** Neon C Executor Statistics ***\n----------------------------------\n"
                        "Total Opcodes Executed : %lld\n"
                        "Max Opstack Height     : %ld\n"
                        "Opstack Height         : %ld\n"
                        "Max Callstack Height   : %ld\n"
                        "CallStack Height       : %ld\n"
                        "Global Size            : %ld\n"
                        "Max Framesets          : %ld\n"
                        "Execution Time         : %fms\n",
                        exec->total_opcodes,
                        exec->stack->max + 1,
                        exec->stack->top,
                        exec->callstack_max_height + 1, 
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

int exec_run(TExecutor *self, BOOL enable_assert)
{
    self->callstack[++self->callstacktop] = (uint32_t)self->object->codelen;
    self->callstack_max_height = self->callstacktop;
    self->enable_assert = enable_assert;

    exec_loop(self);
    if (gOptions.ExecutorDebugStats) {
        assert(isEmpty(self->stack));
    }

    return 0;
}

TExecutor *exec_newExecutor(TBytecode *object)
{
    uint32_t i;
    TExecutor *r = malloc(sizeof(TExecutor));
    if (r == NULL) {
        fatal_error("Failed to allocate memory for Neon executable.");
    }
    r->object = object;
    r->rtl_raise = exec_rtl_raiseException;
    r->stack = createStack(300);
    r->ip = 0;
    r->callstacktop = -1;
    r->param_recursion_limit = 1000;
    r->enable_assert = TRUE;
    r->debug = EXECUTOR_DEBUG_OUTPUT;
    r->framestack = framestack_createFrameStack(r->param_recursion_limit);
    r->globals = malloc(sizeof(Cell) * r->object->global_size);
    if (r->globals == NULL) {
        fatal_error("Failed to allocate memory for global variables.");
    }
    for (i = 0; i < r->object->global_size; i++) {
        cell_resetCell(&r->globals[i]);
    }
    r->module = malloc(sizeof(TModule) * 1);
    if (!r->module) {
        fatal_error("Failed to allocatge memory for neon module(s).");
    }
    r->module->name = "";

    /* Debug / Diagnostic fields */
    r->total_opcodes = 0;
    r->callstack_max_height = 0;
    return r;
}

typedef struct tagTExceptionInfo {
    TString **info;
    size_t size;
    Number code;
} ExceptionInfo;

void exec_raiseLiteral(TExecutor *self, TString *name, TString *info, Number code)
{
    Cell *exceptionvar = cell_createArrayCell(4);
    cell_setString(&exceptionvar->array[0], string_fromString(name));
    cell_setString(&exceptionvar->array[1], string_fromString(info));
    cell_setNumber(&exceptionvar->array[2], code);
    cell_setNumber(&exceptionvar->array[3], number_from_uint64(self->ip));
    uint64_t tip = self->ip;
    uint32_t sp = self->callstacktop;
    while (1) {
        uint64_t i;
        for (i = 0; i < self->object->exceptionsize; i++) {
            if ((self->object->exceptions[i].start <= tip) && (tip < self->object->exceptions[i].end)) {
                TString *handler = self->object->strings[self->object->exceptions[i].exid];
                if ((string_compareString(name, handler) == 0) || (name->length > handler->length && string_startsWith(name, handler) && name->data[handler->length] == '.')) {
                    self->ip = self->object->exceptions[i].handler;
                    self->callstacktop = sp;
                    push(self->stack, exceptionvar);
                    return;
                }
            }
        }
        if (sp == 0) {
            break;
        }
        tip = self->callstack[sp--];
        framestack_popFrame(self->framestack);
    }
    fprintf(stderr, "Unhandled exception %s (%s) (code %d)\n", TCSTR(name), TCSTR(info), number_to_sint32(code));
    exit(1);
}

void exec_rtl_raiseException(TExecutor *self, const char *name, const char *info, Number code)
{
    TString *n, *i;

    n = string_createCString(name);
    i = string_createCString(info);

    exec_raiseLiteral(self, n, i, code);
    string_freeString(n);
    string_freeString(i);
}

static uint32_t exec_getOperand(TExecutor *self)
{
    uint32_t r = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    self->ip += 5;
    return r;
}

void exec_ENTER(TExecutor *self)
{
    self->ip++;
    uint32_t nest = (self->object->code[self->ip] << 24) | (self->object->code[self->ip+1] << 16) | (self->object->code[self->ip+2] << 8) | self->object->code[self->ip+3];
    nest = nest; /* ToDo: Please remove! */
    self->ip += 4;
    uint32_t val = (self->object->code[self->ip] << 24) | (self->object->code[self->ip+1] << 16) | (self->object->code[self->ip+2] << 8) | self->object->code[self->ip+3];
    self->ip += 4;

    framestack_pushFrame(self->framestack, frame_createFrame(val));

    /* ToDo: Implement Activiation frame support
    //add(frame_newFrame(val));
    //nested_frames.resize(nest-1);
    //nested_frames.push_back(&frames.back());
    */
}

void exec_LEAVE(TExecutor *self)
{
    framestack_popFrame(self->framestack);
    self->ip++;
}

void exec_PUSHB(TExecutor *self)
{
    BOOL val = self->object->code[self->ip+1] != 0;
    self->ip += 2;
    push(self->stack, cell_fromBoolean(val));
}

void exec_PUSHN(TExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    push(self->stack, cell_fromNumber(bid128_from_string(self->object->strings[val]->data)));
}

void exec_PUSHS(TExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    push(self->stack, cell_fromString(self->object->strings[val]->data, self->object->strings[val]->length));
}

void exec_PUSHT(TExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    push(self->stack, cell_fromString(self->object->strings[val]->data, self->object->strings[val]->length));
}

void exec_PUSHPG(TExecutor *self)
{
    uint32_t addr = exec_getOperand(self);
    assert(addr < self->object->global_size);
    push(self->stack, cell_fromAddress(&self->globals[addr]));
}

/* push pointer to predefined global */
void exec_PUSHPPG(TExecutor *self)
{
    uint32_t addr = exec_getOperand(self);
    /* assert(addr < self->object->global_variables->size); */
    push(self->stack, cell_fromAddress(cell_newCell()));
}

void exec_PUSHPMG()
{
    fatal_error("PUSHPMG not implemented");
}

void exec_PUSHPL(TExecutor *self)
{
    uint32_t addr = exec_getOperand(self);
    /* push(self->stack, cell_fromAddress(&self->frames[addr])); */
    push(self->stack, cell_fromAddress(&framestack_topFrame(self->framestack)->locals[addr]));
}

void exec_PUSHPOL()
{
    fatal_error("exec_PUSHPOL not implemented");
}

void exec_PUSHI()
{
    fatal_error("exec_PUSHI not implemented");
}

void exec_LOADB(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADN(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADS(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADT(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADA(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADD(TExecutor *self)
{
    self = self;
    fatal_error("exec_LOADD not implemented");
}

void exec_LOADP(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_STOREB(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREN(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STORES(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STORET(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREA(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STORED()
{
    fatal_error("exec_STORED not implemented");
}

void exec_STOREP(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_NEGN(TExecutor *self)
{
    self->ip++;
    Number x = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_negate(x)));
}

void exec_ADDN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_add(a, b)));
}

void exec_SUBN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_sub(a, b)));
}

void exec_MULN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_mul(a, b)));
}

void exec_DIVN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_div(a, b)));
}

void exec_MODN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_modulo(a, b)));
}

void exec_EXPN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(bid128_pow(a, b)));
}

void exec_EQB()
{
    fatal_error("exec_EQB not implemented");
}

void exec_NEB()
{
    fatal_error("exec_NEB not implemented");
}

void exec_EQN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_equal(a, b)));
}

void exec_NEN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_not_equal(a, b)));
}

void exec_LTN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_less(a, b)));
}

void exec_GTN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_greater(a, b)));
}

void exec_LEN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_less_equal(a, b)));
}

void exec_GEN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(bid128_quiet_greater_equal(a, b)));
}

void exec_EQS(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) == 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_NES(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) != 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LTS(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) < 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GTS(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) > 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LES(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) <= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GES(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) >= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_EQT(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) == 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_NET(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) != 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LTT(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) < 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GTT(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) > 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LET(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) <= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GET(TExecutor*self)
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
    fatal_error("exec_EQA not implemented");
}

void exec_NEA()
{
    fatal_error("exec_NEA not implemented");
}

void exec_EQD()
{
    fatal_error("exec_EQD not implemented");
}

void exec_NED()
{
    fatal_error("exec_NED not implemented");
}

void exec_EQP()
{
    fatal_error("exec_EQP not implemented");
}

void exec_NEP()
{
    fatal_error("exec_NEP not implemented");
}

void exec_ANDB()
{
    fatal_error("exec_ANDB not implemented");
}

void exec_ORB()
{
    fatal_error("exec_ORB not implemented");
}

void exec_NOTB(TExecutor *self)
{
    self->ip++;
    BOOL x = top(self->stack)->boolean; pop(self->stack);
    push(self->stack, cell_fromBoolean(!x));
}

void exec_INDEXAR(TExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *addr = top(self->stack)->address; pop(self->stack);

    if (!number_is_integer(index)) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(index), BID_ZERO);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_sint64(i)), BID_ZERO);
        return;
    }
    uint64_t j = (uint64_t)i;
    if (j >= addr->array_size) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_uint64(j)), BID_ZERO);
        return;
    }
    push(self->stack, cell_fromAddress(cell_arrayIndexForRead(addr, j)));
}

void exec_INDEXAW(TExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *addr = top(self->stack)->address; pop(self->stack);

    if (!number_is_integer(index)) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(index), BID_ZERO);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_sint64(i)), BID_ZERO);
        return;
    }
    uint64_t j = (uint64_t)i;
    push(self->stack, cell_fromAddress(cell_arrayIndexForWrite(addr, j)));
}

void exec_INDEXAV(TExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *array = top(self->stack);

    if (!number_is_integer(index)) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(index), BID_ZERO);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_sint64(i)), BID_ZERO);
        return;
    }
    uint64_t j = (uint64_t)i;
    if (j >= array->array_size) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_uint64(j)), BID_ZERO);
        return;
    }
    assert(j < array->array_size);
    Cell *val = cell_fromCell(&array->array[j]);
    pop(self->stack);
    push(self->stack, val);
}

void exec_INDEXAN(TExecutor *self)
{
    self->ip++;
    Number index = top(self->stack)->number; pop(self->stack);
    Cell *array = top(self->stack);

    if (!number_is_integer(index)) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(index), BID_ZERO);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_sint64(i)), BID_ZERO);
        return;
    }
    uint64_t j = (uint64_t)i;
    /* ToDo: Debug this.  I think is following code is WRONG! */
    Cell *val = (j < array->array_size ? cell_fromCell(&array->array[j]) : cell_newCell());
    pop(self->stack);
    push(self->stack, val);
}

void exec_INDEXDR()
{
    fatal_error("exec_INDEXDR not implemented");
}

void exec_INDEXDW()
{
    fatal_error("exec_INDEXDW not implemented");
}

void exec_INDEXDV()
{
    fatal_error("exec_INDEXDV not implemented");
}

void exec_INA(TExecutor *self)
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
    fatal_error("exec_IND not implemented");
}

void exec_CALLP(TExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    const char *func = self->object->strings[val]->data;

    if (!global_callFunction(func, self)) {
        fatal_error("exec_CALLP(): \"%s\" - invalid or unsupported predefined function call.", func);
    }
}

void exec_CALLF(TExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    if (self->callstacktop >= self->param_recursion_limit) {
        /* ToDo: handle runtime exceptions
        //raise(rtl::global::Exception_StackOverflowException, ExceptionInfo(""));
        //return;
        */
    }
    self->callstack[++self->callstacktop] = self->ip;
    self->callstack_max_height = self->callstacktop;
    self->ip = val;
}

void exec_CALLMF()
{
    fatal_error("exec_CALLMF not implemented");
}

void exec_CALLI()
{
    fatal_error("exec_CALLI not implemented");
}

void exec_JUMP(TExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    self->ip = target;
}

void exec_JF(TExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    if (!a) {
        self->ip = target;
    }
}

void exec_JT(TExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    if (a) {
        self->ip = target;
    }
}

void exec_JFCHAIN(TExecutor *self)
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

void exec_DUP(TExecutor *self)
{
    self->ip++;
    push(self->stack, cell_fromCell(top(self->stack)));
}

void exec_DUPX1(TExecutor*self)
{
    self->ip++;
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    push(self->stack, a);
    push(self->stack, b);
    push(self->stack, cell_fromCell(a));
}

void exec_DROP(TExecutor *self)
{
    self->ip++;
    pop(self->stack);
}

void exec_RET(TExecutor *self)
{
    self->ip = self->callstack[self->callstacktop--];
}

void exec_CALLE()
{
    fatal_error("exec_CALLE not implemented");
}

void exec_CONSA(TExecutor *self)
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

void exec_CONSD(TExecutor *self)
{
    uint32_t val = exec_getOperand(self);
    Cell *a = cell_createDictionaryCell(val);

    push(self->stack, a);
}

void exec_EXCEPT(TExecutor *self)
{
    uint32_t val = (self->object->code[self->ip+1] << 24) | (self->object->code[self->ip+2] << 16) | (self->object->code[self->ip+3] << 8) | self->object->code[self->ip+4];
    Cell *exinfo = cell_fromCell(top(self->stack)); pop(self->stack);
    TString *info = NULL;
    Number code = BID_ZERO;

    size_t size = exinfo->array_size;
    if (size >= 1) {
        info = string_fromString(exinfo->array[0].string);
    }
    if (size >= 2) {
        code = exinfo->array[1].number;
    }
    cell_freeCell(exinfo);
    exec_raiseLiteral(self, self->object->strings[val], info, code);
}

void exec_ALLOC()
{
    fatal_error("exec_ALLOC not implemented");
}

void exec_PUSHNIL(TExecutor *self)
{
    self->ip++;
    push(self->stack, NULL);
}

void exec_JNASSERT(TExecutor *self)
{
    uint32_t target = exec_getOperand(self);
    if (!self->enable_assert) {
        self->ip = target;
    }
}

void exec_RESETC(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_resetCell(addr);
}

void exec_PUSHPEG()
{
    fatal_error("exec_PUSHPEG not implemented");
}

void exec_JUMPTBL()
{
    fatal_error("exec_JUMPTBL not implemented");
}

void exec_CALLX()
{
    fatal_error("exec_CALLX not implemented");
}

void exec_SWAP()
{
    fatal_error("exec_SWAP not implemented");
}

void exec_DROPN()
{
    fatal_error("exec_DROPN not implemented");
}

void exec_PUSHM()
{
    fatal_error("exec_PUSHM not implemented");
}

void exec_CALLV()
{
    fatal_error("exec_CALLV not implemented");
}

void exec_PUSHCI()
{
    fatal_error("exec_PUSHCI not implemented");
}

void exec_loop(struct tagTExecutor *self)
{
    while (self->ip < self->object->codelen) {
        //if (self->debug) { fprintf(stderr, "mod\t%s\tip:%d\top:\t%s\tst\t%d\n", self->module->name, self->ip, sOpcode[self->object->code[self->ip]], self->stack->top); }
        switch (self->object->code[self->ip]) {
            case ENTER:   exec_ENTER(self); break;
            case LEAVE:   exec_LEAVE(self); break;
            case PUSHB:   exec_PUSHB(self); break;
            case PUSHN:   exec_PUSHN(self); break;
            case PUSHS:   exec_PUSHS(self); break;
            case PUSHT:   exec_PUSHT(self); break;
            case PUSHPG:  exec_PUSHPG(self); break;
            case PUSHPPG: exec_PUSHPPG(self); break;
            case PUSHPMG: exec_PUSHPMG(); break;
            case PUSHPL:  exec_PUSHPL(self); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case PUSHI:   exec_PUSHI(); break;
            case LOADB:   exec_LOADB(self); break;
            case LOADN:   exec_LOADN(self); break;
            case LOADS:   exec_LOADS(self); break;
            case LOADT:   exec_LOADT(self); break;
            case LOADA:   exec_LOADA(self); break;
            case LOADD:   exec_LOADD(self); break;
            case LOADP:   exec_LOADP(self); break;
            case STOREB:  exec_STOREB(self); break;
            case STOREN:  exec_STOREN(self); break;
            case STORES:  exec_STORES(self); break;
            case STORET:  exec_STORET(self); break;
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
            case EQT:     exec_EQT(self); break;
            case NET:     exec_NET(self); break;
            case LTT:     exec_LTT(self); break;
            case GTT:     exec_GTT(self); break;
            case LET:     exec_LET(self); break;
            case GET:     exec_GET(self); break;
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
            case EXCEPT:  exec_EXCEPT(self); break;
            case ALLOC:   exec_ALLOC(); break;
            case PUSHNIL: exec_PUSHNIL(self); break;
            case JNASSERT:exec_JNASSERT(self); break;
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
