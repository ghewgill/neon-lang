#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "bytecode.h"
#include "cell.h"
#include "dictionary.h"
#include "exec.h"
#include "global.h"
#include "framestack.h"
#include "number.h"
#include "opcode.h"
#include "stack.h"
#include "nstring.h"
#include "util.h"

void invoke(TExecutor *self, int index);

typedef struct tagTCommandLineOptions {
    BOOL ExecutorDebugStats;
    BOOL ExecutorDisassembly;
    BOOL EnableAssertions;
    int  ArgStart;
    char *pszFilename;
    char *pszExecutableName;
} TOptions;

static TOptions gOptions = { FALSE, FALSE, TRUE, 0, NULL, NULL };

void module_freeModule(TModule *m)
{
    for (unsigned int i = 0; i < m->bytecode->global_size; i++) {
        cell_clearCell(&m->globals[i]);
    }
    free(m->globals);
    bytecode_freeBytecode(m->bytecode);
    free(m->name);
    free(m->code);
    free(m);
}

void exec_freeExecutor(TExecutor *e)
{
    for (unsigned int m = 0; m < e->module_count; m++) {
        //printf("Free: %d - %s\n", m, e->modules[m]->name);
        //e->module = e->modules[m];
        module_freeModule(e->modules[m]);
        //for (i = 0; i < e->module->bytecode->global_size; i++) {
        //    cell_clearCell(&e->module->globals[i]);
        //}
        //free(e->module->globals);
        //free(e->module);
    }
    assert(isEmpty(e->stack));
    destroyStack(e->stack);
    free(e->stack);
    framestack_destroyFrameStack(e->framestack);
    free(e->framestack);
    free(e->modules);
    free(e);
}

TExecutor *exec_newExecutor(TModule *object);
TModule *module_newModule(const char *name);

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

char *getApplicationName(char *arg)
{
    char *p = arg;
    char *s = p;

    while (*p) {
        if (*p == '\\' || *p == '/') {
            s = p;
        }
        p++;
    }
    return ++s;
}

int main(int argc, char* argv[])
{
    gOptions.pszExecutableName = getApplicationName(argv[0]);
    if (!ParseOptions(argc, argv)) {
        return 3;
    }
    global_init(argc, argv, gOptions.ArgStart);

    FILE *fp = fopen(gOptions.pszFilename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open Neon executable: %s\nError: %d - %s.\n", gOptions.pszFilename, errno, strerror(errno));
        return 2;
    }
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //TBytecode *pModule = bytecode_newBytecode();
    TModule *pModule = module_newModule("");
    uint8_t *bytecode = malloc(nSize);
    if (bytecode == NULL) {
        fatal_error("Could not allocate memory for neon bytecode.");
    }
    unsigned int bytes_read = (unsigned int)fread(bytecode, 1, nSize, fp);
    fclose(fp);

    bytecode_loadBytecode(pModule->bytecode, bytecode, bytes_read);

    TExecutor *exec = exec_newExecutor(pModule);

    exec->diagnostics.time_start = clock();
    exec_run(exec, gOptions.EnableAssertions);
    exec->diagnostics.time_end = clock();

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
                        exec->diagnostics.total_opcodes,
                        exec->stack->max + 1,
                        exec->stack->top,
                        exec->diagnostics.callstack_max_height + 1,
                        exec->callstacktop,
                        exec->module[0].bytecode->global_size,
                        exec->framestack->max,
                        (((float)exec->diagnostics.time_end - exec->diagnostics.time_start) / CLOCKS_PER_SEC) * 1000
        );
    }

    global_shutdown();
    exec_freeExecutor(exec);
    //bytecode_freeBytecode(pModule);

    free(bytecode);

    return 0;
}

int exec_run(TExecutor *self, BOOL enable_assert)
{
    self->enable_assert = enable_assert;
    self->ip = (uint32_t)self->modules[0]->bytecode->codelen;
    invoke(self, 0);

    //for (unsigned int m = self->module_count; m > 0; m--) {
    for (unsigned int m = 1 ; m < self->module_count; m++) {
        self->module = self->modules[m];
        invoke(self, 0);
    }

    exec_loop(self);
    if (gOptions.ExecutorDebugStats) {
        assert(isEmpty(self->stack));
    }

    return 0;
}

TModule *module_findModule(TExecutor *self, const char *name)
{
    TModule *r = NULL;
    for (unsigned int i = 0; i < self->module_count; i++) {
        if (strcmp(self->modules[i]->name, name) == 0) {
            r = self->modules[i];
            break;
        }
    }
    return r;
}

//void module_loadModule(TExecutor *self, TModule *mod)
//{
//    for (unsigned int m = 0; m < mod->bytecode->importsize; m++) {
//        TModule *lm = module_findModule(self, mod->name);
//        if (lm != NULL) {
//            continue;
//        }
//
//        TBytecode *code = bytecode_newBytecode();
//        code = exec_readBytecode(self, mod->name)
//    }
//
//}

uint8_t *exec_readBytecode(/*TExecutor *self, */const char *name, unsigned int *code_size)
{
    static BOOL bDone = FALSE;
    bDone = FALSE;
    //printf("looking for module %s\n", name);
    char filename[256];
    sprintf(filename, "f:\\dropbox\\projects\\neon-lang\\neon-lang\\%s.neonx", name);
    FILE *fp = NULL;
load:
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        sprintf(filename, "f:\\dropbox\\projects\\neon-lang\\neon-lang\\lib\\%s.neonx", name);
        if (!bDone) {
            bDone = TRUE;
            goto load;
        }
        fatal_error("Could not locate Neon module %s", name);
    }
    fseek(fp, 0, SEEK_END);
    long nSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t *data = malloc(nSize);
    if (data == NULL) {
        fatal_error("Could not allocate bytecode memory for %s module.", name);
    }
    *code_size = (unsigned int)fread(data, 1, nSize, fp);
    fclose(fp);

    return data;
}

TModule *module_newModule(const char *name)
{
    TModule *r = malloc(sizeof(TModule));
    memset(r, 0x00, sizeof(TModule));

    r->name = malloc(strlen(name) + 1);
    strcpy(r->name, name);

    r->bytecode = bytecode_newBytecode();

    return r;
}

TModule *exec_locateModule(TExecutor *self, const char *name)
{
    // First, check to see if we have already loaded this module.
    TModule *r = module_findModule(self, name);
    if (r != NULL) {
        // We have already loaded this module, so bail out.
        return r;
    }
    // Setup this module, and load it.
    self->modules = realloc(self->modules, sizeof(TModule*) * ++self->module_count);
    r = module_newModule(name);
    self->modules[self->module_count-1] = r;

    unsigned int code_size = 0;
    r->code = exec_readBytecode(name, &code_size);
    bytecode_loadBytecode(r->bytecode, r->code, code_size);
    //free(data);

    for (unsigned int m = 0; m < r->bytecode->importsize; m++) {
        exec_locateModule(self, r->bytecode->strings[r->bytecode->imports[m].name]->data);
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
    r->modules[0] = object;

    r->modules[0]->globals = malloc(sizeof(Cell) * r->modules[0]->bytecode->global_size);
    if (r->modules[0]->globals == NULL) {
        fatal_error("Failed to allocate memory for global storage.");
    }
    for (i = 0; i < r->modules[0]->bytecode->global_size; i++) {
        cell_resetCell(&r->modules[0]->globals[i]);
    }
    // Set current modules to module::main.
    r->module = r->modules[0];

    // Next, iterate the imports, loading all the module code as necessary.
    for (unsigned int m = 0; m < r->module->bytecode->importsize; m++) {
        TModule *mod = exec_locateModule(r, r->module->bytecode->strings[r->module->bytecode->imports[m].name]->data);
        // mod will ALWAYS point to a module.  Either an existing module we've already loaded, or a new
        // module that has already been pushed onto the module stack.
        // Allocate globals, and initialize the cells.
        mod->globals = malloc(sizeof(Cell) * mod->bytecode->global_size);
        if (mod->globals == NULL) {
            fatal_error("Failed to allocate memory for %s module global storage.", mod->name);
        }
        for (i = 0; i < mod->bytecode->global_size; i++) {
            cell_resetCell(&mod->globals[i]);
        }
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
            printf("  %d { locals=%d }\n", i, f->frame_size);
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
    uint32_t sp = self->callstacktop;
    for (;;) {
        uint64_t i;
        for (i = 0; i < self->module->bytecode->exceptionsize; i++) {
            if ((self->module->bytecode->exceptions[i].start <= tip) && (tip < self->module->bytecode->exceptions[i].end)) {
                TString *handler = self->module->bytecode->strings[self->module->bytecode->exceptions[i].exid];
                if ((string_compareString(name, handler) == 0) || (name->length > handler->length && string_startsWith(name, handler) && name->data[handler->length] == '.')) {
                    self->ip = self->module->bytecode->exceptions[i].handler;
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
        tip = self->callstack[sp--].ip;
        self->module = self->callstack[sp].mod;
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
    push(self->stack, cell_fromAddress(&self->module[0].globals[addr] ));
}

/* push pointer to predefined global */
void exec_PUSHPPG(TExecutor *self)
{
    self->ip++;
    unsigned int addr = exec_getOperand(self);
    const char *var = self->module->bytecode->strings[addr]->data;

    push(self->stack, cell_fromAddress(global_getVariable(var)));
}

void exec_PUSHPMG(void)
{
    fatal_error("PUSHPMG not implemented");
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

void exec_PUSHI(void)
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

void exec_LOADY(TExecutor *self)
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
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADP(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADJ(TExecutor *self)
{
    self->ip++;
    Cell *addr = top(self->stack)->address; pop(self->stack);
    push(self->stack, cell_fromCell(addr));
}

void exec_LOADV(TExecutor *self)
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

void exec_EQN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_equal(a, b)));
}

void exec_NEN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_not_equal(a, b)));
}

void exec_LTN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_less(a, b)));
}

void exec_GTN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_greater(a, b)));
}

void exec_LEN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_less_equal(a, b)));
}

void exec_GEN(TExecutor*self)
{
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromBoolean(number_is_greater_equal(a, b)));
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

void exec_EQY(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) == 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_NEY(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) != 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LTY(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) < 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GTY(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) > 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_LEY(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) <= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_GEY(TExecutor*self)
{
    self->ip++;
    Cell *b = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *a = cell_fromCell(top(self->stack)); pop(self->stack);
    Cell *r = cell_fromBoolean(string_compareString(a->string, b->string) >= 0);
    cell_freeCell(b);
    cell_freeCell(a);
    push(self->stack, r);
}

void exec_EQA(void)
{
    fatal_error("exec_EQA not implemented");
}

void exec_NEA(void)
{
    fatal_error("exec_NEA not implemented");
}

void exec_EQD(void)
{
    fatal_error("exec_EQD not implemented");
}

void exec_NED(void)
{
    fatal_error("exec_NED not implemented");
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
    invoke(self, val);
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
                self->module = m;
                invoke(self, m->bytecode->export_functions[efi].index);
                string_freeString(funcsig);
                return;
            }
            string_freeString(funcsig);
        }
        fatal_error("function not found: %s", self->module->bytecode->strings[fun]->data);
    }
    fatal_error("module not found: %s", self->module->bytecode->strings[mod]->data);
}

void exec_CALLI(void)
{
    fatal_error("exec_CALLI not implemented");
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
    framestack_popFrame(self->framestack);
    self->ip = self->callstack[self->callstacktop--].ip;
    if (self->callstacktop >= 0) {
        self->module = self->callstack[self->callstacktop].mod;
    }
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

void exec_JNASSERT(TExecutor *self)
{
    self->ip++;
    unsigned int target = exec_getOperand(self);
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

void exec_CALLX(void)
{
    fatal_error("exec_CALLX not implemented");
}

void exec_SWAP(TExecutor *self)
{
    self->ip++;
    int top = self->stack->top;
    Cell *t = self->stack->data[top];
    self->stack->data[top] = self->stack->data[top-1];
    self->stack->data[top-1] = t;
}

void exec_DROPN(void)
{
    fatal_error("exec_DROPN not implemented");
}

void exec_PUSHM(void)
{
    fatal_error("exec_PUSHM not implemented");
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

void invoke(TExecutor *self, int index)
{
    self->callstack[++self->callstacktop].ip = self->ip;
    self->callstack[self->callstacktop].mod = self->module;
    self->diagnostics.callstack_max_height = self->callstacktop;

    // ToDo: Fix function nesting
    framestack_pushFrame(self->framestack, frame_createFrame(self->module->bytecode->functions[index].locals, self->stack->top - self->module->bytecode->functions[index].params));
    dump_frames(self);
    /* ToDo: Implement Activiation frame support */
    //add(frame_newFrame(val));
    //nested_frames.resize(nest-1);
    //nested_frames.push_back(&frames.back());

    self->ip = self->module->bytecode->functions[index].entry;
}

void exec_loop(TExecutor *self)
{
    while (self->ip < self->module->bytecode->codelen) {
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
            case PUSHPMG: exec_PUSHPMG(); break;
            case PUSHPL:  exec_PUSHPL(self); break;
            case PUSHPOL: exec_PUSHPOL(); break;
            case PUSHI:   exec_PUSHI(); break;
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
            case EQA:     exec_EQA(); break;
            case NEA:     exec_NEA(); break;
            case EQD:     exec_EQD(); break;
            case NED:     exec_NED(); break;
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
            case ALLOC:   exec_ALLOC(self); break;
            case PUSHNIL: exec_PUSHNIL(self); break;
            case JNASSERT:exec_JNASSERT(self); break;
            case RESETC:  exec_RESETC(self); break;
            case PUSHPEG: exec_PUSHPEG(); break;
            case JUMPTBL: exec_JUMPTBL(self); break;
            case CALLX:   exec_CALLX(); break;
            case SWAP:    exec_SWAP(self); break;
            case DROPN:   exec_DROPN(); break;
            case PUSHM:   exec_PUSHM(); break;
            case CALLV:   exec_CALLV(); break;
            case PUSHCI:  exec_PUSHCI(self); break;
            default:
                fatal_error("exec: Unexpected opcode: %d\n", self->module->bytecode->code[self->ip]);
        }
        self->diagnostics.total_opcodes++;
    }
}
