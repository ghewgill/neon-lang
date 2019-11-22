#ifdef __MS_HEAP_DBG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef __MS_HEAP_DBG
#include <stdlib.h>
#endif

#include "array.h"
#include "bytecode.h"
#include "cell.h"
#include "dictionary.h"
#include "exec.h"
#include "global.h"
#include "framestack.h"
#include "module.h"
#include "neonext.h"
#include "nstring.h"
#include "number.h"
#include "opcode.h"
#include "rtl_platform.h"
#include "stack.h"
#include "support.h"
#include "util.h"

#ifdef _WIN32
#define PATHSEP "\\"
#define LIBRARY_NAME_PREFIX ""
#else
#define PATHSEP "/"
#define LIBRARY_NAME_PREFIX "lib"
#endif

TExtensionModule *g_ExtensionModules;
extern struct Ne_MethodTable ExtensionMethodTable;
TExecutor *g_executor;

void invoke(TExecutor *self, TModule *m, int index);

typedef struct tagTCommandLineOptions {
    BOOL ExecutorDebugStats;
    BOOL ExecutorDisassembly;
    BOOL EnableAssertions;
    int  ArgStart;
    char *pszFilename;
    char *pszExecutableName;
    char *pszExecutablePath;
} TOptions;

static TOptions gOptions = { FALSE, FALSE, TRUE, 0, NULL, NULL, NULL };


void exec_freeExecutor(TExecutor *e)
{
    // First, assert that we've emptied the stack during normal execution.
    if (e->exit_code == 0) {
        assert(isEmpty(e->stack));
    }
    destroyStack(e->stack);
    free(e->stack);

    // Next, iterate all allocated modules, freeing them as we go.
    for (unsigned int m = e->module_count; m > 0; m--) {
        if (e->debug == TRUE) {
            fprintf(stderr, "Free module #%d - %s\n", m-1, e->modules[m-1]->name);
        }
        module_freeModule(e->modules[m-1]);
    }
    free(e->modules);
    free(e->init_order);

    // Then, destroy the frame stack.
    framestack_destroyFrameStack(e->framestack);
    free(e->framestack);

    // Finally, free the TExecutor object.
    free(e);
}

TExecutor *exec_newExecutor(TModule *object);

void showUsage(void)
{
    fprintf(stderr, "Usage:\n\n");
    fprintf(stderr, "   %s [options] program.neonx\n", gOptions.pszExecutableName);
    fprintf(stderr, "\n Where [options] is one or more of the following:\n");
    fprintf(stderr, "     -d       Display executor debug stats.\n");
    fprintf(stderr, "     -D       Display executor disassembly during run.\n");
    fprintf(stderr, "     -h       Display this help screen.\n");
    fprintf(stderr, "     -n       No Assertions\n");
}

BOOL ParseOptions(int argc, char* argv[])
{
    BOOL Retval = FALSE;
    for (int nIndex = 1; nIndex < argc; nIndex++) {
        if (*argv[nIndex] == '-') {
            if(argv[nIndex][1] == 'h' || argv[nIndex][1] == '?' || ((argv[nIndex][1] == '-' && argv[nIndex][2] != '\0') && (argv[nIndex][2] == 'h'))) {
                showUsage();
                exit(1);
            } else if (argv[nIndex][1] == 'D') {
                gOptions.ExecutorDisassembly = TRUE;
            } else if (argv[nIndex][1] == 'd') {
                gOptions.ExecutorDebugStats = TRUE;
            } else if (argv[nIndex][1] == 'n') {
                gOptions.EnableAssertions = FALSE;
            } else {
                printf("Unknown option %s\n", argv[nIndex]);
                return FALSE;
            }
        } else {
            // Once we assign the name of the application we're going to execute, stop looking for switches.
            gOptions.pszFilename = argv[nIndex];
            gOptions.pszExecutablePath = path_getPathOnly(argv[nIndex]);
            gOptions.ArgStart = nIndex;
            return TRUE;
        }
    }
    if (gOptions.pszFilename == NULL) {
        printf("You must provide a Neon binary file to execute.\n");
        Retval = FALSE;
    }
    return Retval;
}

int main(int argc, char* argv[])
{
    int ret = 0;
    gOptions.pszExecutableName = path_getFileNameOnly(argv[0]);

    if (!ParseOptions(argc, argv)) {
        return 3;
    }

    global_init(argc, argv, gOptions.ArgStart);
    path_initPaths(gOptions.pszExecutablePath);

    TModule *pModule = module_loadNeonProgram(gOptions.pszFilename);
    if (pModule == NULL) {
        goto shutdown;
    }
    g_executor = exec_newExecutor(pModule);

    g_executor->diagnostics.time_start = clock();
    ret = exec_run(g_executor, gOptions.EnableAssertions);
    g_executor->diagnostics.time_end = clock();

    if (gOptions.ExecutorDebugStats) {
        fprintf(stderr, "\n*** Neon C Executor Statistics ***\n----------------------------------\n"
                        "Total Opcodes Executed : %" PRIu64 "\n"
                        "Max Opstack Height     : %d\n"
                        "Opstack Height         : %d\n"
                        "Max Callstack Height   : %" PRIu32 "\n"
                        "CallStack Height       : %" PRId32 "\n"
                        "Global Size            : %" PRIu32 "\n"
                        "Max Framesets          : %d\n"
                        "Execution Time         : %fms\n",
                        g_executor->diagnostics.total_opcodes,
                        g_executor->stack->max + 1,
                        g_executor->stack->top,
                        g_executor->diagnostics.callstack_max_height + 1,
                        g_executor->callstacktop,
                        g_executor->module[0].bytecode->global_size,
                        g_executor->framestack->max,
                        (((float)g_executor->diagnostics.time_end - g_executor->diagnostics.time_start) / CLOCKS_PER_SEC) * 1000
        );
    }

    exec_freeExecutor(g_executor);

shutdown:
    global_shutdown();
    path_freePaths();
    ext_cleanup();

    free(gOptions.pszExecutablePath);

    return ret;
}

int exec_run(TExecutor *self, BOOL enable_assert)
{
    self->enable_assert = enable_assert;
    self->ip = (uint32_t)self->module->bytecode->codelen;
    invoke(self, self->modules[0], 0);

    for (unsigned int m = 0; m < self->init_count; m++) {
        // Allocate all of the modules globals
        TModule *mod = self->modules[self->init_order[(self->init_count - 1) - m]];
        mod->globals = malloc(sizeof(Cell) * mod->bytecode->global_size);
        if (mod->globals == NULL) {
            fatal_error("Failed to allocate global variables for module %s.", mod->name);
        }
        // Initialize the modules globals.
        for (unsigned int i = 0; i < mod->bytecode->global_size; i++) {
            cell_resetCell(&mod->globals[i]);
        }
        // Execute the module initialization code.
        invoke(self, self->modules[self->init_order[(self->init_count - 1) - m]], 0);
    }
    // Execute main Neon program.
    int r = exec_loop(self, 0);

    if (r == 0 && gOptions.ExecutorDebugStats) {
        assert(isEmpty(self->stack));
    }
    return r;
}

TExecutor *exec_newExecutor(TModule *object)
{
    uint32_t i;
    TExecutor *r = malloc(sizeof(TExecutor));
    if (r == NULL) {
        fatal_error("Failed to allocate memory for Neon executable.");
    }
    r->rtl_raise = exec_rtl_raiseException;
    r->stack = createStack(300);
    r->ip = 0;
    r->callstacktop = -1;
    r->exit_code = 0;
    r->param_recursion_limit = 1000;
    r->enable_assert = TRUE;
    r->debug = gOptions.ExecutorDebugStats;
    r->disassemble = gOptions.ExecutorDisassembly;
    r->framestack = framestack_createFrameStack(r->param_recursion_limit);

    // Load and initialize all the module code.  Note that there is always at least
    // one module!  See: runtime$moduleIsMain() call.
    r->module_count = 1;
    r->modules = malloc(sizeof(TModule*) * r->module_count);
    if (r->modules == NULL) {
        fatal_error("Failed to allocate memory for %d neon module(s).", r->module_count);
    }
    // Setup the init_order array.  This stores the index of the modules that need to be initialized before
    // other modules.  Some modules depend on other modules. See t/module-load-order.neonx.
    r->init_order = NULL;
    r->init_count = 0;
    r->modules[0] = object;

    // Allocate and initialize all global variables.
    r->modules[0]->globals = malloc(sizeof(Cell) * r->modules[0]->bytecode->global_size);
    if (r->modules[0]->globals == NULL) {
        fatal_error("Failed to allocate memory for global storage.");
    }
    for (i = 0; i < r->modules[0]->bytecode->global_size; i++) {
        cell_resetCell(&r->modules[0]->globals[i]);
    }

    // Set current module to module::main.
    r->module = r->modules[0];

    // Next, iterate the imports, loading all the module code recursively, as necessary.
    for (unsigned int m = 0; m < r->module->bytecode->importsize; m++) {
        module_loadModule(r, r->module->bytecode->strings[r->module->bytecode->imports[m].name]->data, r->module_count);
    }

    /* Debug / Diagnostic fields */
    r->diagnostics.total_opcodes = 0;
    r->diagnostics.callstack_max_height = 0;
    return r;
}

typedef struct tagTExceptionInfo {
    TString **info;
    size_t size;
    Number code;
} ExceptionInfo;

void dump_frames(TExecutor *exec)
{
    if (0) {
        printf("Frames:\n");
        for (int i = 0; i < exec->framestack->top; i++) {
            TFrame *f = exec->framestack->data[i];
            printf("\t %04d\t{ locals=%04d }\n", i, f->frame_size);
            for (int l = 0; 0 < f->frame_size; l++) {
                TString *x = cell_toString(&f->locals[l]);
                printf("%04d: %01d %s", l, f->locals[l].type, x->data);
                string_freeString(x);
            }
        }
    }
}

void exec_raiseLiteral(TExecutor *self, TString *name, TString *info, Number code)
{
    Cell *exceptionvar = cell_createArrayCell(4);
    cell_setString(&exceptionvar->array->data[0], string_fromString(name));
    cell_setString(&exceptionvar->array->data[1], info);
    cell_setNumber(&exceptionvar->array->data[2], code);
    cell_setNumber(&exceptionvar->array->data[3], number_from_uint64(self->ip));
    uint64_t tip = self->ip;
    TModule *tmodule = self->module;
    uint32_t sp = self->callstacktop;
    for (;;) {
        uint64_t i;
        for (i = 0; i < tmodule->bytecode->exceptionsize; i++) {
            if ((tmodule->bytecode->exceptions[i].start <= tip) && (tip < tmodule->bytecode->exceptions[i].end)) {
                TString *handler = tmodule->bytecode->strings[tmodule->bytecode->exceptions[i].exid];
                if ((string_compareString(name, handler) == 0) || (name->length > handler->length && string_startsWith(name, handler) && name->data[handler->length] == '.')) {
                    self->ip = tmodule->bytecode->exceptions[i].handler;
                    self->module = tmodule;
                    while (self->stack->top > (((framestack_isEmpty(self->framestack) ? -1 : framestack_topFrame(self->framestack)->stack_depth) + (int32_t)self->module->bytecode->exceptions[i].stack_depth))) {
                        pop(self->stack);
                    }
                    self->callstacktop = sp;
                    push(self->stack, exceptionvar);
                    return;
                }
            }
        }
        if (sp == 0) {
            break;
        }
        if (!framestack_isEmpty(self->framestack)) {
            framestack_popFrame(self->framestack);
        }
        tip = self->callstack[sp].ip;
        tmodule = self->callstack[sp].mod;
        sp -= 1;
    }
    fprintf(stderr, "Unhandled exception %s (%s) (code %d)\n", TCSTR(name), TCSTR(info), number_to_sint32(code));
    cell_freeCell(exceptionvar);
    // ToDo: Initiate a stack dump here...

    // Setting exit_code here will cause exec_loop to terminate and return this exit code.
    self->exit_code = 1;
}

void exec_rtl_raiseException(TExecutor *self, const char *name, const char *info, Number code)
{
    TString *n, *i;

    n = string_createCString(name);
    i = string_createCString(info);

    exec_raiseLiteral(self, n, i, code);
    string_freeString(n);
}

static unsigned int exec_getOperand(TExecutor *self)
{
    return get_vint(self->module->bytecode->code, self->module->bytecode->codelen, &self->ip);
}

void exec_PUSHB(TExecutor *self)
{
    BOOL val = self->module->bytecode->code[self->ip+1] != 0;
    self->ip += 2;
    push(self->stack, cell_fromBoolean(val));
}

void exec_PUSHN(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    push(self->stack, cell_fromNumber(number_from_string(self->module->bytecode->strings[val]->data)));
}

void exec_PUSHS(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    push(self->stack, cell_fromStringLength(self->module->bytecode->strings[val]->data, self->module->bytecode->strings[val]->length));
}

void exec_PUSHY(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    push(self->stack, cell_fromBytes(self->module->bytecode->strings[val]));
}

void exec_PUSHPG(TExecutor *self)
{
    self->ip++;
    unsigned int addr = exec_getOperand(self);
    assert(addr < self->module->bytecode->global_size);
    push(self->stack, cell_fromAddress(&self->module->globals[addr]));
}

/* push pointer to predefined global */
void exec_PUSHPPG(TExecutor *self)
{
    self->ip++;
    unsigned int addr = exec_getOperand(self);
    const char *var = self->module->bytecode->strings[addr]->data;

    push(self->stack, cell_fromAddress(global_getVariable(var)));
}

void exec_PUSHPMG(TExecutor *self)
{
    self->ip++;
    unsigned int mod = exec_getOperand(self);
    unsigned int addr = exec_getOperand(self);
    TModule *m = module_findModule(self, self->module->bytecode->strings[mod]->data);
    if (m == NULL) {
        fatal_error("fatal: module not found: %s\n", self->module->bytecode->strings[mod]->data);
    }
    assert(addr < m->bytecode->global_size);
    push(self->stack, cell_fromAddress(&m->globals[addr]));
}

void exec_PUSHPL(TExecutor *self)
{
    self->ip++;
    unsigned int addr = exec_getOperand(self);
    /* push(self->stack, cell_fromAddress(&self->frames[addr])); */
    push(self->stack, cell_fromAddress(&framestack_topFrame(self->framestack)->locals[addr]));
}

void exec_PUSHPOL(void)
{
    fatal_error("exec_PUSHPOL not implemented");
}

void exec_PUSHI(TExecutor *self)
{
    self->ip++;
    uint32_t x = exec_getOperand(self);
    push(self->stack, cell_fromNumber(number_from_uint32(x)));
}

void exec_LOADB(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureBoolean(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADN(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureNumber(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADS(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureString(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADY(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureBytes(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADA(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureArray(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADD(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureDictionary(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADP(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureAddress(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADJ(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureObject(addr);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADV(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_ensureOther(addr);
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

void exec_STOREY(TExecutor *self)
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

void exec_STORED(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREP(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREJ(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_STOREV(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_copyCell(addr, top(self->stack)); pop(self->stack);
}

void exec_NEGN(TExecutor *self)
{
    self->ip++;
    Number x = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_negate(x)));
}

void exec_ADDN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_add(a, b)));
}

void exec_SUBN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_subtract(a, b)));
}

void exec_MULN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_multiply(a, b)));
}

void exec_DIVN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
   if (number_is_zero(b)) {
        self->rtl_raise(self, "DivideByZeroException", "", BID_ZERO);
        return;
    }
    push(self->stack, cell_fromNumber(number_divide(a, b)));
}

void exec_MODN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
   if (number_is_zero(b)) {
        self->rtl_raise(self, "DivideByZeroException", "", BID_ZERO);
        return;
    }
    push(self->stack, cell_fromNumber(number_modulo(a, b)));
}

void exec_EXPN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_pow(a, b)));
}

void exec_EQB(TExecutor *self)
{
    self->ip++;
    BOOL b = top(self->stack)->boolean; pop(self->stack);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    push(self->stack, cell_fromBoolean(a == b));
}

void exec_NEB(TExecutor *self)
{
    self->ip++;
    BOOL b = top(self->stack)->boolean; pop(self->stack);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    push(self->stack, cell_fromBoolean(a != b));
}

void exec_EQN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_equal(a, b)));
}

void exec_NEN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_not_equal(a, b)));
}

void exec_LTN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_less(a, b)));
}

void exec_GTN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_greater(a, b)));
}

void exec_LEN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_less_equal(a, b)));
}

void exec_GEN(TExecutor *self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_greater_equal(a, b)));
}

void exec_EQS(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) == 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_NES(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) != 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LTS(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) < 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GTS(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) > 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LES(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) <= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GES(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) >= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_EQY(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) == 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_NEY(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) != 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LTY(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) < 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GTY(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) > 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LEY(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) <= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GEY(TExecutor *self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) >= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_EQA(TExecutor *self)
{
    self->ip++;
    Cell *b = top(self->stack);
    Cell *a = peek(self->stack, 1);
    Cell *r = cell_fromBoolean(array_compareArray(a->array, b->array) != 0);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_NEA(TExecutor *self)
{
    self->ip++;
    Cell *b = top(self->stack);
    Cell *a = peek(self->stack, 1);
    Cell *r = cell_fromBoolean(array_compareArray(a->array, b->array) == 0);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_EQD(TExecutor *self)
{
    self->ip++;
    Cell *b = top(self->stack);
    Cell *a = peek(self->stack, 1);
    Cell *r = cell_fromBoolean(dictionary_compareDictionary(a->dictionary, b->dictionary) != 0);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_NED(TExecutor *self)
{
    self->ip++;
    Cell *b = top(self->stack);
    Cell *a = peek(self->stack, 1);
    Cell *r = cell_fromBoolean(dictionary_compareDictionary(a->dictionary, b->dictionary) == 0);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_EQP(TExecutor *self)
{
    self->ip++;
    Cell *b = peek(self->stack, 0)->address;
    Cell *a = peek(self->stack, 1)->address;
    Cell *r = cell_fromBoolean(a == b);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_NEP(TExecutor *self)
{
    self->ip++;
    Cell *b = peek(self->stack, 0)->address;
    Cell *a = peek(self->stack, 1)->address;
    Cell *r = cell_fromBoolean(a != b);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_EQV(TExecutor *self)
{
    self->ip++;
    Cell *b = peek(self->stack, 0)->other;
    Cell *a = peek(self->stack, 1)->other;
    Cell *r = cell_fromBoolean(a == b);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_NEV(TExecutor *self)
{
    self->ip++;
    Cell *b = peek(self->stack, 0)->other;
    Cell *a = peek(self->stack, 1)->other;
    Cell *r = cell_fromBoolean(a != b);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, r);
}

void exec_ANDB(void)
{
    fatal_error("exec_ANDB not implemented");
}

void exec_ORB(void)
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
    if (j >= addr->array->size) {
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
    if (j >= array->array->size) {
        self->rtl_raise(self, "ArrayIndexException", number_to_string(number_from_uint64(j)), BID_ZERO);
        return;
    }
    assert(j < array->array->size);
    Cell *val = cell_fromCell(&array->array->data[j]);
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
    Cell *val = (j < array->array->size ? cell_fromCell(&array->array->data[j]) : cell_newCell());
    pop(self->stack);
    push(self->stack, val);
}

void exec_INDEXDR(TExecutor *self)
{
    self->ip++;
    TString *index = string_fromString(top(self->stack)->string); pop(self->stack);
    Cell *addr = top(self->stack)->address; pop(self->stack);
    Cell *e = dictionary_findDictionaryEntry(addr->dictionary, index);
    if (e == NULL) {
        char *pszIndex = string_asCString(index);
        self->rtl_raise(self, "DictionaryIndexException", pszIndex, BID_ZERO);
        free(pszIndex);
        string_freeString(index);
        return;
    }
    push(self->stack, cell_fromAddress(cell_dictionaryIndexForRead(addr, index)));
    string_freeString(index);
}

void exec_INDEXDW(TExecutor *self)
{
    self->ip++;
    TString *index = string_fromString(top(self->stack)->string); pop(self->stack);
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromAddress(cell_dictionaryIndexForWrite(addr, index)));
    //string_freeString(index);
}

void exec_INDEXDV(TExecutor *self)
{
    self->ip++;
    TString *index = peek(self->stack, 0)->string;
    Cell *dictionary = peek(self->stack, 1);
    Cell *val = dictionary_findDictionaryEntry(dictionary->dictionary, index);
    if (val == NULL) {
        char *pszIndex = string_asCString(index);
        self->rtl_raise(self, "DictionaryIndexException", pszIndex, BID_ZERO);
        free(pszIndex);
        pop(self->stack);
        pop(self->stack);
        return;
    }

    Cell *addr = cell_fromCell(val);
    pop(self->stack);
    pop(self->stack);
    push(self->stack, addr);
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

void exec_IND(TExecutor *self)
{
    self->ip++;
    Dictionary *dictionary = top(self->stack)->dictionary;
    TString *key = peek(self->stack, 1)->string;

    BOOL v = dictionary_findDictionaryEntry(dictionary, key) != NULL;

    pop(self->stack);
    pop(self->stack);
    push(self->stack, cell_fromBoolean(v));
}

void exec_CALLP(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    const char *func = self->module->bytecode->strings[val]->data;

    global_callFunction(func, self);
}

void exec_CALLF(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    if (self->callstacktop >= self->param_recursion_limit) {
        self->rtl_raise(self, "StackOverflowException", "", BID_ZERO);
        return;
    }
    invoke(self, self->module, val);
}

void exec_CALLMF(TExecutor *self)
{
    self->ip++;
    unsigned int mod = exec_getOperand(self);
    unsigned int fun = exec_getOperand(self);
    unsigned int efi = 0;
    if (self->callstacktop >= self->param_recursion_limit) {
        self->rtl_raise(self, "StackOverflowException", "", BID_ZERO);
        return;
    }

    TModule *m = module_findModule(self, self->module->bytecode->strings[mod]->data);
    if (m != NULL) {
        for (efi = 0; efi < m->bytecode->export_functionsize; efi++) {
            TString *funcsig = string_copyString(m->bytecode->strings[m->bytecode->export_functions[efi].name]);
            funcsig = string_appendChar(funcsig, ',');
            funcsig = string_appendString(funcsig, m->bytecode->strings[m->bytecode->export_functions[efi].descriptor]);
            if (string_compareString(funcsig, self->module->bytecode->strings[fun]) == 0) {
                invoke(self, m, m->bytecode->export_functions[efi].index);
                string_freeString(funcsig);
                return;
            }
            string_freeString(funcsig);
        }
        fatal_error("function not found: %s", self->module->bytecode->strings[fun]->data);
    }
    fatal_error("module not found: %s", self->module->bytecode->strings[mod]->data);
}

void exec_CALLI(TExecutor *self)
{
    self->ip++;
    if (self->callstacktop >= self->param_recursion_limit) {
        self->rtl_raise(self, "StackOverflowException", "", BID_ZERO);
        return;
    }

    Cell *a = top(self->stack);
    TModule *mod = (TModule*)a->array->data[0].other;
    Number nindex = a->array->data[1].number;
    pop(self->stack);
    if (number_is_zero(nindex) || !number_is_integer(nindex)) {
        self->rtl_raise(self, "InvalidFunctionException", "", BID_ZERO);
        return;
    }
    uint32_t index = number_to_uint32(nindex);
    invoke(self, mod, index);
}

void exec_JUMP(TExecutor *self)
{
    self->ip++;
    unsigned int target = exec_getOperand(self);
    self->ip = target;
}

void exec_JF(TExecutor *self)
{
    self->ip++;
    unsigned int target = exec_getOperand(self);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    if (!a) {
        self->ip = target;
    }
}

void exec_JT(TExecutor *self)
{
    self->ip++;
    unsigned int target = exec_getOperand(self);
    BOOL a = top(self->stack)->boolean; pop(self->stack);
    if (a) {
        self->ip = target;
    }
}

void exec_JFCHAIN(TExecutor *self)
{
    self->ip++;
    unsigned int target = exec_getOperand(self);
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

void exec_DUPX1(TExecutor *self)
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
    framestack_popFrame(self->framestack);
    self->ip = self->callstack[self->callstacktop].ip;
    self->module = self->callstack[self->callstacktop].mod;
    self->callstacktop--;
}

void exec_CALLE(void)
{
    fatal_error("exec_CALLE not implemented");
}

void exec_CONSA(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    Cell *a = cell_createArrayCell(val);

    while (val > 0) {
        cell_copyCell(&a->array->data[a->array->size - val], top(self->stack));
        val--;
        pop(self->stack);
    }
    push(self->stack, a);
}

void exec_CONSD(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    Cell *d = cell_createDictionaryCell();

    while (val > 0) {
        Cell *value = cell_fromCell(top(self->stack)); pop(self->stack);
        TString *key = string_fromString(top(self->stack)->string); pop(self->stack);
        dictionary_addDictionaryEntry(d->dictionary, key, value);
        val--;
    }
    push(self->stack, d);
}

void exec_EXCEPT(TExecutor *self)
{
    const unsigned int start_ip = self->ip;
    self->ip++;
    unsigned int val = exec_getOperand(self);
    self->ip = start_ip;
    Cell *exinfo = top(self->stack);
    TString *info = NULL;
    Number code = BID_ZERO;

    size_t size = exinfo->array->size;
    if (size >= 1) {
        info = string_fromString(exinfo->array->data[0].string);
    }
    if (size >= 2) {
        code = exinfo->array->data[1].number;
    }
    pop(self->stack);
    exec_raiseLiteral(self, self->module->bytecode->strings[val], info, code);
}

void exec_ALLOC(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);

    Cell *cell = cell_createArrayCell(val);
    push(self->stack, cell_fromAddress(cell));
}

void exec_PUSHNIL(TExecutor *self)
{
    self->ip++;
    push(self->stack, cell_newCell());
}

void exec_RESETC(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    cell_resetCell(addr);
}

void exec_PUSHPEG(void)
{
    fatal_error("exec_PUSHPEG not implemented");
}

void exec_JUMPTBL(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);
    Number n = top(self->stack)->number; pop(self->stack);
    if (number_is_integer(n) && !number_is_negative(n)) {
        uint32_t i = number_to_uint32(n);
        if (i < val) {
            self->ip += 6 * i;
        } else {
            self->ip += 6 * val;
        }
    } else {
        self->ip += 6 * val;
    }
}

void exec_CALLX(TExecutor *self)
{
    self->ip++;
    uint32_t mod = exec_getOperand(self);
    uint32_t name = exec_getOperand(self);
    uint32_t out_param_count = exec_getOperand(self);

    if (self->module->extension_path == NULL) {
        self->module->extension_path = malloc(256);
        if (self->module->extension_path == NULL) {
            fatal_error("Could not allocate extension module path memory for module %s.", self->module->bytecode->strings[mod]->data);
        }
        snprintf(self->module->extension_path, 256, "%s/%sneon_%s", self->module->path_only, LIBRARY_NAME_PREFIX, self->module->bytecode->strings[mod]->data);
    }
    TExtensionModule *exmod = ext_findModule(self->module->extension_path);
    if (exmod == NULL) {
        void_function_t init = rtl_foreign_function(self, self->module->extension_path, "Ne_INIT");
        if (init == NULL) {
            fatal_error("neon_exec: function Ne_INIT not found in %s\n", self->module->extension_path);
        }
        exmod = ext_findModule(self->module->extension_path);
        exmod->methods = malloc(sizeof(TExtensionMethod) * self->module->bytecode->strtablelen); // Alloc enough room to hold all the function pointers.
        if (exmod->methods == NULL) {
            fatal_error("Could not allocate method pointer table for module %s.", self->module->extension_path);
        }
        memset(exmod->methods, 0x00, sizeof(TExtensionMethod) * self->module->bytecode->strtablelen);
        // Call Neon extensions Ne_INIT function only once, the first time we load this module.
        (init)(&ExtensionMethodTable);
    }

    if (exmod->methods[name].proc == NULL) {
        char funcname[256];
        snprintf(funcname, 256, "%s%s", "Ne_", self->module->bytecode->strings[name]->data);
        exmod->methods[name].proc = rtl_foreign_function(self, self->module->extension_path, funcname);
        if (exmod->methods[name].proc == NULL) {
            fatal_error("neon_exec: function %s not found in %s\n", funcname, self->module->extension_path);
        }
    }

    Ne_ExtensionFunction f = (Ne_ExtensionFunction)(exmod->methods[name].proc);
    Cell *out_params = cell_createArrayCell(out_param_count);
    Cell *retval = cell_newCell();
    int r = (*f)((struct Ne_Cell*)retval, (struct Ne_ParameterList*)top(self->stack), (struct Ne_ParameterList*)out_params);
    pop(self->stack);
    switch (r) {
        case Ne_SUCCESS: {
            push(self->stack, cell_fromCell(retval));
            for (size_t i = 0; i < out_params->array->size; i++) {
                push(self->stack, cell_fromCell(&out_params->array->data[i]));
            }
            break;
        }
        case Ne_EXCEPTION: {
            const char *exceptionname = string_ensureNullTerminated(retval->array->data[0].string);
            const char *info = string_ensureNullTerminated(retval->array->data[1].string);
            Number code = retval->array->data[2].number;
            exec_rtl_raiseException(self, exceptionname, info, code);
            break;
        }
        default:
            fatal_error("neon: invalid return value %d from extension function %s.Ne_%s\n", r, exmod->module, self->module->bytecode->strings[name]->data);
    }
    cell_freeCell(retval);
    cell_freeCell(out_params);
}

void exec_SWAP(TExecutor *self)
{
    self->ip++;
    int top = self->stack->top;
    Cell *t = self->stack->data[top];
    self->stack->data[top] = self->stack->data[top-1];
    self->stack->data[top-1] = t;
}

void exec_DROPN(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);
    drop(self->stack, val);
}

void exec_PUSHFP(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);
    Cell *a = cell_createArrayCell(0);
    cell_arrayAppendElementPointer(a, cell_createOtherCell(self->module));
    cell_arrayAppendElementPointer(a, cell_fromNumber(number_from_uint32(val)));
    push(self->stack, a);
}

void exec_CALLV(void)
{
    fatal_error("exec_CALLV not implemented");
}

void exec_PUSHCI(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);

    size_t dot = string_findChar(self->module->bytecode->strings[val], '.');
    if (dot == NPOS) {
        for (unsigned int c = 0; c < self->module->bytecode->classsize; c++) {
            if (self->module->bytecode->classes[c].name == val) {
                push(self->stack, cell_newCell());
                return;
            }
        }
    } else {
        // Locate class interface in module
    }
    fatal_error("cnex: unknown class name %s\n", TCSTR(self->module->bytecode->strings[val]));
}

void invoke(TExecutor *self, TModule *m, int index)
{
    self->callstacktop++;
    self->callstack[self->callstacktop].ip = self->ip;
    self->callstack[self->callstacktop].mod = self->module;
    self->diagnostics.callstack_max_height = self->callstacktop;

    // ToDo: Fix function nesting
    framestack_pushFrame(self->framestack, frame_createFrame(m->bytecode->functions[index].locals, self->stack->top - m->bytecode->functions[index].params));
    dump_frames(self);
    /* ToDo: Implement Activiation frame support */
    //add(frame_newFrame(val));
    //nested_frames.resize(nest-1);
    //nested_frames.push_back(&frames.back());

    self->module = m;
    self->ip = self->module->bytecode->functions[index].entry;
}

int exec_loop(TExecutor *self, int64_t min_callstack_depth)
{
    while ((self->callstacktop +1) > min_callstack_depth && self->ip < self->module->bytecode->codelen && self->exit_code == 0) {
        if (self->disassemble) { 
            fprintf(stderr, "mod:%s\tip: %d\top: %s\tst: %d\n", self->module->name, self->ip, sOpcode[self->module->bytecode->code[self->ip]], self->stack->top); 
        }
        switch (self->module->bytecode->code[self->ip]) {
            case PUSHB:   exec_PUSHB(self); break;
            case PUSHN:   exec_PUSHN(self); break;
            case PUSHS:   exec_PUSHS(self); break;
            case PUSHY:   exec_PUSHY(self); break;
            case PUSHPG:  exec_PUSHPG(self); break;
            case PUSHPPG: exec_PUSHPPG(self); break;
            case PUSHPMG: exec_PUSHPMG(self); break;
            case PUSHPL:  exec_PUSHPL(self); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case PUSHI:   exec_PUSHI(self); break;
            case LOADB:   exec_LOADB(self); break;
            case LOADN:   exec_LOADN(self); break;
            case LOADS:   exec_LOADS(self); break;
            case LOADY:   exec_LOADY(self); break;
            case LOADA:   exec_LOADA(self); break;
            case LOADD:   exec_LOADD(self); break;
            case LOADP:   exec_LOADP(self); break;
            case LOADJ:   exec_LOADJ(self); break;
            case LOADV:   exec_LOADV(self); break;
            case STOREB:  exec_STOREB(self); break;
            case STOREN:  exec_STOREN(self); break;
            case STORES:  exec_STORES(self); break;
            case STOREY:  exec_STOREY(self); break;
            case STOREA:  exec_STOREA(self); break;
            case STORED:  exec_STORED(self); break;
            case STOREP:  exec_STOREP(self); break;
            case STOREJ:  exec_STOREJ(self); break;
            case STOREV:  exec_STOREV(self); break;
            case NEGN:    exec_NEGN(self); break;
            case ADDN:    exec_ADDN(self); break;
            case SUBN:    exec_SUBN(self); break;
            case MULN:    exec_MULN(self); break;
            case DIVN:    exec_DIVN(self); break;
            case MODN:    exec_MODN(self); break;
            case EXPN:    exec_EXPN(self); break;
            case EQB:     exec_EQB(self); break;
            case NEB:     exec_NEB(self); break;
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
            case EQY:     exec_EQY(self); break;
            case NEY:     exec_NEY(self); break;
            case LTY:     exec_LTY(self); break;
            case GTY:     exec_GTY(self); break;
            case LEY:     exec_LEY(self); break;
            case GEY:     exec_GEY(self); break;
            case EQA:     exec_EQA(self); break;
            case NEA:     exec_NEA(self); break;
            case EQD:     exec_EQD(self); break;
            case NED:     exec_NED(self); break;
            case EQP:     exec_EQP(self); break;
            case NEP:     exec_NEP(self); break;
            case EQV:     exec_EQV(self); break;
            case NEV:     exec_NEV(self); break;
            case ANDB:    exec_ANDB(); break;
            case ORB:     exec_ORB(); break;
            case NOTB:    exec_NOTB(self); break;
            case INDEXAR: exec_INDEXAR(self); break;
            case INDEXAW: exec_INDEXAW(self); break;
            case INDEXAV: exec_INDEXAV(self); break;
            case INDEXAN: exec_INDEXAN(self); break;
            case INDEXDR: exec_INDEXDR(self); break;
            case INDEXDW: exec_INDEXDW(self); break;
            case INDEXDV: exec_INDEXDV(self); break;
            case INA:     exec_INA(self); break;
            case IND:     exec_IND(self); break;
            case CALLP:   exec_CALLP(self); break;
            case CALLF:   exec_CALLF(self); break;
            case CALLMF:  exec_CALLMF(self); break;
            case CALLI:   exec_CALLI(self); break;
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
            case ALLOC:   exec_ALLOC(self); break;
            case PUSHNIL: exec_PUSHNIL(self); break;
            case RESETC:  exec_RESETC(self); break;
            case PUSHPEG: exec_PUSHPEG(); break;
            case JUMPTBL: exec_JUMPTBL(self); break;
            case CALLX:   exec_CALLX(self); break;
            case SWAP:    exec_SWAP(self); break;
            case DROPN:   exec_DROPN(self); break;
            case PUSHFP:  exec_PUSHFP(self); break;
            case CALLV:   exec_CALLV(); break;
            case PUSHCI:  exec_PUSHCI(self); break;
            default:
                fatal_error("exec: Unexpected opcode: %d\n", self->module->bytecode->code[self->ip]);
        }
        self->diagnostics.total_opcodes++;
    }
    return self->exit_code;
}
