#ifdef __MS_HEAP_DBG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#endif
// The following is needed for strdup on Posix platforms.
#define _POSIX_C_SOURCE 200809L

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
#ifdef _WIN32
#define CJSON_HIDE_SYMBOLS
#endif
#include "cJSON.h"
#include "dictionary.h"
#include "disassembly.h"
#include "exec.h"
#include "gc.h"
#include "global.h"
#include "httpserver.h"
#include "framestack.h"
#include "module.h"
#include "neonext.h"
#include "nstring.h"
#include "number.h"
#include "object.h"
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

const char *DebuggerStateName[] = {
    "stopped",
    "run",
    "single_step",
    "source_step",
    "quit",
};


TExtensionModule *g_ExtensionModules;
extern struct Ne_MethodTable ExtensionMethodTable;
TExecutor *g_executor;

void invoke(TExecutor *self, TModule *m, int index);
void exec_handleGET(TExecutor *exec, TString *path, HttpResponse *response);
void exec_handlePOST(TExecutor *exec, TString *path, TString *data, HttpResponse *response);

typedef struct tagTCommandLineOptions {
    BOOL ExecutorDebugStats;
    BOOL ExecutorDisassembly;
    BOOL EnableAssertions;
    BOOL EnableDebug;
    int  ArgStart;
    int  DebugPort;
    char *pszFilename;
    char *pszExecutableName;
    char *pszExecutablePath;
} TOptions;

static TOptions gOptions = { FALSE, FALSE, TRUE, FALSE, 0, 0, NULL, NULL, NULL };

void exec_freeExecutor(TExecutor *e)
{
    // First, assert that we've emptied the stack during normal execution.
    // Note: We don't want to assert here if the user was running under
    // the debugger.  In that case, they could have just stopped debugging.
    if (e->exit_code == 0 && e->debugging == FALSE) {
        assert(isEmpty(e->stack));
    }
    // Next, destroy the opstack.
    destroyStack(e->stack);
    free(e->stack);

    // Next, free all Neon-allocated objects.
    heap_freeHeap(e);

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

    // Destroy the callstack.
    free(e->callstack);

    // Free the debugger
    server_freeServer(e->debug_server);
    free(e->debugger_breakpoints);
    string_freeStringArray(e->debugger_log);

    // Finally, free the TExecutor object.
    free(e);
}

TExecutor *exec_newExecutor(TModule *object);

void showUsage(void)
{
    fprintf(stderr, "Usage:\n\n");
    fprintf(stderr, "   %s [options] program.neonx\n", gOptions.pszExecutableName);
    fprintf(stderr, "\n Where [options] is one or more of the following:\n");
    fprintf(stderr, "     -D       Display executor debug stats.\n");
    fprintf(stderr, "     --debug-port port  Use debug [port] for interactive debugger.\n");
    fprintf(stderr, "     -t       Trace execution disassembly during run.\n");
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
            } else if (argv[nIndex][1] == 't') {
                gOptions.ExecutorDisassembly = TRUE;
            } else if (argv[nIndex][1] == 'D') {
                gOptions.ExecutorDebugStats = TRUE;
            } else if (argv[nIndex][1] == 'd') {
                gOptions.EnableDebug = TRUE;
            } else if (strcmp(argv[nIndex], "--debug-port") == 0) {
                nIndex++;
                if (argv[nIndex]) {
                    gOptions.DebugPort = atoi(argv[nIndex]);
                    if (gOptions.DebugPort <= 0 || gOptions.DebugPort > 65535) {
                        printf("--debug-port requires a valid debug port between 1 and 65535.\n");
                        return FALSE;
                    }
                } else {
                    printf("A debug port is required after the --debug-port parameter.\n");
                    return FALSE;
                }
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
#ifdef __MS_HEAP_DBG
    /* Used for debugging cnex, looking for memory leaks. */
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    if (IsDebuggerPresent()) {
        _CrtSetBreakAlloc(0);  // Can be set to break on specific allocation.
    }
#endif
    int ret = 0;
    NumberDisplayMode = nfFull;
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
    ret = exec_run(g_executor, gOptions.EnableAssertions, gOptions.EnableDebug);
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
                        "Max Allocations        : %zu\n"
                        "Remaining Objects      : %zu\n"
                        "Execution Time         : %fms\n",
                        g_executor->diagnostics.total_opcodes,
                        g_executor->stack->max + 1,
                        g_executor->stack->top,
                        g_executor->diagnostics.callstack_max_height + 1,
                        g_executor->callstacktop,
                        g_executor->module[0].bytecode->global_size,
                        g_executor->framestack->max,
                        g_executor->diagnostics.total_allocations,
                        heap_getObjectCount(g_executor),
                        (((float)g_executor->diagnostics.time_end - g_executor->diagnostics.time_start) / CLOCKS_PER_SEC) * 1000
        );
    }

    exec_freeExecutor(g_executor);

shutdown:
    global_shutdown();
    path_freePaths();
    ext_cleanup();
    number_cleanup();

    free(gOptions.pszExecutablePath);
#ifdef __MS_HEAP_DBG
    /* If you're doing HEAP tracking, and cnex leaks memory, then 
       _CrtDempMemoryLeaks() will return true, to ensure that the 
       test that is being debugged will fail.  (This will also dump
       all of the allocations that were never free()'ed.)       */
    if (_CrtDumpMemoryLeaks()) {
        return 1;
    }
#endif
    return ret;
}

int exec_run(TExecutor *self, BOOL enable_assert, BOOL enable_debug)
{
    self->enable_assert = enable_assert;
    self->enable_debug = enable_debug;
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
            cell_initCell(&mod->globals[i]);
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
    r->callstacksize = 10;
    r->callstack = malloc(sizeof(struct CallStack) * r->callstacksize);
    if (r->callstack == NULL) {
        fatal_error("Failed to allocate Call stack memory.");
    }
    r->enable_assert = TRUE;
    r->enable_debug = FALSE;
    r->debug = gOptions.ExecutorDebugStats;
    r->disassemble = gOptions.ExecutorDisassembly;
    r->framestack = framestack_createFrameStack(r->param_recursion_limit);
    r->allocations = 0;
    r->collection_interval = 1000;
    r->firstObject = NULL;

    // Initialize the Debugger
    r->debugging = FALSE;
    r->debug_server = NULL;
    r->debugger_state = dbgSTOPPED;
    r->debugger_step_source_depth = 0;
    r->debugger_breakpoints = NULL;
    // Always create the debug_log, in case a program calls debugger.log(); we'll need a place to store the entry.
    r->debugger_log = string_createStringArray();
    if (gOptions.DebugPort) {
        r->debugging = TRUE;
        r->handlers.handle_GET = exec_handleGET;
        r->handlers.handle_POST = exec_handlePOST;
        r->debug_server = server_createServer((unsigned short)gOptions.DebugPort, &r->handlers);
        if (r->debug_server) {
            r->debug_server->exec = r;
        }
        r->debugger_breakpoints = malloc(sizeof(char) * object->bytecode->codelen);
        memset(r->debugger_breakpoints, 0, object->bytecode->codelen);
    }

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
        cell_initCell(&r->modules[0]->globals[i]);
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
    r->diagnostics.total_allocations = 0;
    r->diagnostics.collected_objects = 0;
    return r;
}

int findDebugLine(TModule *m, int ip)
{
    cJSON *jip = NULL;
    cJSON *jln = NULL;
    cJSON *jelm = NULL;
    cJSON *lines = NULL;
    // Iterate the line_numbers JSON element, looking for the source code line that is
    // associated with the provided Instruction Pointer (ip) value.
    lines = cJSON_GetObjectItem(m->debug_symbols, "line_numbers");
    cJSON_ArrayForEach(jelm, lines)
    {
        jip = cJSON_GetArrayItem(jelm, 0);
        if (!cJSON_IsInvalid(jip) && jip->valueint == ip) {
            // We found the line, so break out, and return it.
            jln = cJSON_GetArrayItem(jelm, 1);
            break;
        }
    }
    // If we don't find a matching line number, just return 0.
    if (jln == NULL || cJSON_IsInvalid(jln)) {
        return 0;
    }
    return jln->valueint;
}

TStringArray *getSourceLines(TModule *m)
{
    TStringArray *arr = string_createStringArray();
    cJSON *jelm = NULL;
    cJSON *lines = cJSON_GetObjectItem(m->debug_symbols, "source");
    cJSON_ArrayForEach(jelm, lines)
    {
        if (!cJSON_IsInvalid(jelm) && jelm->type == cJSON_String) {
            string_appendStringArrayElement(arr, string_createCString(jelm->valuestring));
        }
    }
    return arr;
}

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

void exec_raiseLiteral(TExecutor *self, TString *name, Cell *info)
{
    Cell *exceptionvar = cell_createArrayCell(3);
    cell_setString(&exceptionvar->array->data[0], string_fromString(name));
    cell_copyCell(&exceptionvar->array->data[1], info);
    cell_setNumber(&exceptionvar->array->data[2], number_from_uint64(self->ip));
    _IDEC_glbflags = 0;
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
                    while (self->stack->top > (((framestack_isEmpty(self->framestack) ? -1 : framestack_topFrame(self->framestack)->opstack_depth) + (int32_t)self->module->bytecode->exceptions[i].stack_depth))) {
                        pop(self->stack);
                    }
                    self->callstacktop = sp;
                    push(self->stack, exceptionvar);
                    cell_freeCell(info);
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
    Cell *inf = object_toString(info->object);
    fprintf(stderr, "Unhandled exception %s (%s)\n", TCSTR(name), TCSTR(inf->string));
    cell_freeCell(exceptionvar);
    cell_freeCell(info);
    cell_freeCell(inf);
    cJSON *symbols = NULL;
    cJSON *pStart = NULL;
    while (self->ip < self->module->bytecode->codelen) {
        char *dbgpath = strdup(self->module->source_path);
        dbgpath[strlen(self->module->source_path)-1] = 'd';

        FILE *fp = fopen(dbgpath, "rb");
        // Don't report out of memory errors, let the stack unwinder report the best it can.
        if (fp != NULL) {
            fseek(fp, 0, SEEK_END);
            long nSize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            char *debug = malloc(nSize + 1);
            if (debug != NULL) {
                size_t nr = fread(debug, 1, nSize, fp);
                fclose(fp);
                debug[nr] = '\0'; // Ensure data is null-terminated.
                symbols = cJSON_Parse(debug);
                // Remember the start of the symbols, so we can always go back to them.
                pStart = symbols;
                free(debug);
            }
        }
        free(dbgpath);

        if (symbols != NULL && !cJSON_IsInvalid(symbols)) {
            uint64_t p = self->ip;
            cJSON *source = cJSON_GetObjectItem(symbols, "source");
            char hash_string[65];
            for (int i = 0; i < 32; i++) {
                snprintf(&hash_string[i*2], 3, "%2.2x", self->module->bytecode->source_hash[i]);
            }
            hash_string[64] = '\0';
            const char *symbol_hash = cJSON_GetObjectItem(symbols, "source_hash")->valuestring;
            if (strcmp(hash_string, symbol_hash) != 0) {
                fprintf(stderr, "  Stack frame #%d: file %s address %d (no debug information, symbols are out of date. %.7s..%.7s)\n", self->callstacktop+1, self->module->source_path, self->ip, hash_string, symbol_hash);
                cJSON_Delete(pStart);
                goto nextframe;
            }
            cJSON *jip = NULL;
            cJSON *jln = NULL;
            cJSON *lines = NULL;
            for (;;) {
                lines = cJSON_GetObjectItem(symbols, "line_numbers");
                cJSON_ArrayForEach(symbols, lines)
                {
                    jip = cJSON_GetArrayItem(symbols, 0);
                    if (!cJSON_IsInvalid(jip) && (uint64_t)jip->valueint == p) {
                        jln = cJSON_GetArrayItem(symbols, 1);
                        break;
                    }
                }
                if (jln != NULL) {
                    break;
                }
                if (p == 0) {
                    fprintf(stderr, "No matching debug information found.\n");
                    break;
                }
                p--;
                symbols = pStart;
            }
            if (!cJSON_IsInvalid(jln)) {
                cJSON *source_line = cJSON_GetArrayItem(source, jln->valueint);
                fprintf(stderr, "  Stack frame #%d: file %s line %d address %d\n", self->callstacktop+1, self->module->source_path, jln->valueint, self->ip);
                if (!cJSON_IsInvalid(source_line)) {
                    fprintf(stderr, "    %s\n", source_line->valuestring);
                } else {
                    fprintf(stderr, "    *Source line missing from symbols file!*\n");
                }
            } else {
                fprintf(stderr, "  Stack frame #%d: file %s address %d (line number not found)\n", self->callstacktop+1, self->module->source_path, self->ip);
            }
            cJSON_Delete(pStart);
        } else {
            fprintf(stderr, "  Stack frame #%d: file %s address %d (no debug info available)\n", self->callstacktop+1, self->module->source_path, self->ip);
        }
nextframe:
        if (self->callstacktop == 0) {
            break;
        }
        self->module = self->callstack[self->callstacktop].mod;
        self->ip = self->callstack[self->callstacktop].ip;
        self->callstacktop--;
    }

    // Destroy the stack if there are no exception handlers available.
    // This is important to properly shut cnex down.
    while (!isEmpty(self->stack)) {
        pop(self->stack);
    }
    // Setting exit_code here will cause exec_loop to terminate and return this exit code.
    self->exit_code = 1;
}

void exec_rtl_raiseException(TExecutor *self, const char *name, const char *info)
{
    TString *n = string_createCString(name);
    TString *i = string_createCString(info);

    Cell *ex = cell_fromObject(object_createStringObject(i));

    exec_raiseLiteral(self, n, ex);
    string_freeString(n);
    string_freeString(i);
}

void exec_numberCheckAndRaise(TExecutor *self, unsigned int start_ip, const char *what)
{
    if (_IDEC_glbflags & BID_OVERFLOW_EXCEPTION) {
        self->ip = start_ip;
        exec_rtl_raiseException(self, "NumberException.Overflow", what);
        return;
    }
    if (_IDEC_glbflags & BID_ZERO_DIVIDE_EXCEPTION) {
        self->ip = start_ip;
        exec_rtl_raiseException(self, "NumberException.DivideByZero", what);
        return;
    }
    if (_IDEC_glbflags & BID_INVALID_EXCEPTION) {
        self->ip = start_ip;
        exec_rtl_raiseException(self, "NumberException.Invalid", what);
        return;
    }
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
    unsigned int name = exec_getOperand(self);
    TModule *m = module_findModule(self, self->module->bytecode->strings[mod]->data);
    if (m == NULL) {
        fatal_error("fatal: module not found: %s\n", self->module->bytecode->strings[mod]->data);
    }
    for (unsigned int v = 0; v < m->bytecode->variablesize; v++) {
        if (string_compareString(m->bytecode->strings[m->bytecode->variables[v].name], self->module->bytecode->strings[name]) == 0) {
            unsigned int addr = m->bytecode->variables[v].index;
            assert(addr < m->bytecode->global_size);
            push(self->stack, cell_fromAddress(&m->globals[addr]));
            return;
        }
    }
    fatal_error("fatal: variable not found: %s\n", self->module->bytecode->strings[name]->data);
}

void exec_PUSHPL(TExecutor *self)
{
    self->ip++;
    unsigned int addr = exec_getOperand(self);
    /* push(self->stack, cell_fromAddress(&self->frames[addr])); */
    push(self->stack, cell_fromAddress(&framestack_topFrame(self->framestack)->locals[addr]));
}

void exec_PUSHPOL(TExecutor *self)
{
    self->ip++;
    unsigned int back = exec_getOperand(self);
    unsigned int addr = exec_getOperand(self);
    TFrame *frame = framestack_topFrame(self->framestack);
    while (back > 0) {
        frame = frame->outer;
        back--;
    }

    push(self->stack, cell_fromAddress(&frame->locals[addr]));
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

void exec_NEGN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number x = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_negate(x)));
    exec_numberCheckAndRaise(self, start_ip, "negate");
}

void exec_ADDN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_add(a, b)));
    exec_numberCheckAndRaise(self, start_ip, "add");
}

void exec_SUBN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_subtract(a, b)));
    exec_numberCheckAndRaise(self, start_ip, "subtract");
}

void exec_MULN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_multiply(a, b)));
    exec_numberCheckAndRaise(self, start_ip, "multiply");
}

void exec_DIVN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_divide(a, b)));
    exec_numberCheckAndRaise(self, start_ip, "divide");
}

void exec_MODN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_modulo(a, b)));
    exec_numberCheckAndRaise(self, start_ip, "modulo");
}

void exec_EXPN(TExecutor *self)
{
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    Number b = top(self->stack)->number; pop(self->stack);
    Number a = top(self->stack)->number; pop(self->stack);
    push(self->stack, cell_fromNumber(number_pow(a, b)));
    exec_numberCheckAndRaise(self, start_ip, "exponentiation");
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
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index not an integer: %s", number_to_string(index));
        self->rtl_raise(self, "PANIC", buf);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index is negative: %"PRId64, i);
        self->rtl_raise(self, "PANIC", buf);
        return;
    }
    uint64_t j = (uint64_t)i;
    if (j >= addr->array->size) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index exceeds size %zd: %"PRIu64, addr->array->size, j);
        self->rtl_raise(self, "PANIC", buf);
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
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index is not an integer: %s", number_to_string(index));
        self->rtl_raise(self, "PANIC", buf);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index is negative: %"PRId64, i);
        self->rtl_raise(self, "PANIC", buf);
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
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index not an integer: %s", number_to_string(index));
        self->rtl_raise(self, "PANIC", buf);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index is negative: %"PRId64, i);
        self->rtl_raise(self, "PANIC", buf);
        return;
    }
    uint64_t j = (uint64_t)i;
    if (j >= array->array->size) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index exceeds size %zd: %"PRIu64, array->array->size, j);
        self->rtl_raise(self, "PANIC", buf);
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
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index not an integer: %s", number_to_string(index));
        self->rtl_raise(self, "PANIC", buf);
        return;
    }
    int64_t i = number_to_sint64(index);
    if (i < 0) {
        char buf[100];
        snprintf(buf, sizeof(buf), "Array index is negative: %"PRId64, i);
        self->rtl_raise(self, "PANIC", buf);
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
        self->rtl_raise(self, "DictionaryIndexException", pszIndex);
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
        self->rtl_raise(self, "DictionaryIndexException", pszIndex);
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
    _IDEC_glbflags = 0;
    unsigned int start_ip = self->ip;
    self->ip++;
    unsigned int val = exec_getOperand(self);
    const char *func = self->module->bytecode->strings[val]->data;

    global_callFunction(func, self);
    exec_numberCheckAndRaise(self, start_ip, func);
}

void exec_CALLF(TExecutor *self)
{
    self->ip++;
    unsigned int val = exec_getOperand(self);
    if (self->callstacktop >= self->param_recursion_limit) {
        char buf[100];
        snprintf(buf, sizeof(buf), "StackOverflow: Stack depth exceeds recursion limit of %u", self->param_recursion_limit);
        self->rtl_raise(self, "PANIC", buf);
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
        char buf[100];
        snprintf(buf, sizeof(buf), "StackOverflow: Stack depth exceeds recursion limit of %u", self->param_recursion_limit);
        self->rtl_raise(self, "PANIC", buf);
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
        char buf[100];
        snprintf(buf, sizeof(buf), "StackOverflow: Stack depth exceeds recursion limit of %u", self->param_recursion_limit);
        self->rtl_raise(self, "PANIC", buf);
        return;
    }

    Cell *a = top(self->stack);
    TModule *mod = (TModule*)a->array->data[0].other;
    Number nindex = a->array->data[1].number;
    pop(self->stack);
    if (number_is_zero(nindex) || !number_is_integer(nindex)) {
        self->rtl_raise(self, "PANIC", "Invalid function pointer");
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
    Cell *info = cell_fromCell(top(self->stack));
    pop(self->stack);
    exec_raiseLiteral(self, self->module->bytecode->strings[val], info);
}

void exec_ALLOC(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);

    Cell *cell = heap_allocObject(self);
    cell->array = array_createArrayFromSize(val);
    cell->type = cArray;
    push(self->stack, cell_fromAddress(cell));
    self->allocations++;
    if (self->collection_interval > 0 && self->allocations >= self->collection_interval) {
        // ToDo: Implement heap sweep and clear
        //heap_sweepHeap(self, gOptions.ExecutorDebugStats);
    }
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
    cell_clearCell(addr);
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
            exec_rtl_raiseException(self, exceptionname, info);
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

void exec_CALLV(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);
    if (self->callstacktop >= self->param_recursion_limit) {
        char buf[100];
        snprintf(buf, sizeof(buf), "StackOverflow: Stack depth exceeds recursion limit of %u", self->param_recursion_limit);
        self->rtl_raise(self, "PANIC", buf);
        return;
    }

    Cell *pi = top(self->stack);
    Cell *instance = pi->array->data[0].address;
    size_t interface_index = number_to_uint32(pi->array->data[1].number);
    TModule *m = (TModule*)instance->array->data[0].array->data[0].other;
    Class *classinfo = instance->array->data[0].array->data[1].other;
    pop(self->stack);

    invoke(self, m, classinfo->interfaces[interface_index].methods[val]);
}

void exec_PUSHCI(TExecutor *self)
{
    self->ip++;
    uint32_t val = exec_getOperand(self);

    size_t dot = string_findChar(self->module->bytecode->strings[val], '.');
    if (dot == NPOS) {
        for (unsigned int c = 0; c < self->module->bytecode->classsize; c++) {
            if (self->module->bytecode->classes[c].name == val) {
                Cell *ci = cell_createArrayCell(2);
                ci->array->data[0].other = self->module;
                ci->array->data[0].type = cOther;
                ci->array->data[1].other = &self->module->bytecode->classes[c];
                ci->array->data[1].type = cOther;
                push(self->stack, ci);
                return;
            }
        }
    } else {
        TString *modname = string_subString(self->module->bytecode->strings[val], 0, dot);
        TString *methodname = string_subString(self->module->bytecode->strings[val], dot+1, self->module->bytecode->strings[val]->length - dot - 1);
        TModule *mod = module_findModule(self, TCSTR(modname));
        if (mod != NULL) {
            for (unsigned int c = 0; c < mod->bytecode->classsize; c++) {
                if (string_compareString(mod->bytecode->strings[mod->bytecode->classes[c].name], methodname) == 0) {
                    Cell *ci = cell_createArrayCell(2);
                    ci->array->data[0].other = mod;
                    ci->array->data[0].type = cOther;
                    ci->array->data[1].other = &mod->bytecode->classes[c];
                    ci->array->data[1].type = cOther;
                    push(self->stack, ci);
                    string_freeString(modname);
                    string_freeString(methodname);
                    return;
                }
            }
        }
        string_freeString(modname);
        string_freeString(methodname);
    }
    fatal_error("cnex: unknown class name %s\n", TCSTR(self->module->bytecode->strings[val]));
}

void invoke(TExecutor *self, TModule *m, int index)
{
    self->callstacktop++;
    if (self->callstacktop + 1 > self->callstacksize) {
        self->callstacksize *= 2;
        self->callstack = realloc(self->callstack, sizeof(struct CallStack) * self->callstacksize);
        if (self->callstack == NULL) {
            fatal_error("Could not allocate %d callstack space.", self->callstacksize);
        }
    }
    self->callstack[self->callstacktop].ip = self->ip;
    self->callstack[self->callstacktop].mod = self->module;
    self->diagnostics.callstack_max_height = self->callstacktop;

    TFrame *outer = NULL;
    unsigned int nest = m->bytecode->functions[index].nest;
    unsigned int params = m->bytecode->functions[index].params;
    unsigned int locals = m->bytecode->functions[index].locals;
    if (self->framestack->top >= 0) {
        assert(nest <= self->framestack->data[self->framestack->top]->nesting_depth+1);
        outer = self->framestack->data[self->framestack->top];
        while (outer != NULL && nest <= outer->nesting_depth) {
            assert(outer->outer == NULL || outer->nesting_depth == outer->outer->nesting_depth+1);
            outer = outer->outer;
        }
    }

    framestack_pushFrame(self->framestack, frame_createFrame(nest, outer, locals, self->stack->top - params));
    dump_frames(self);

    self->module = m;
    self->ip = self->module->bytecode->functions[index].entry;
}

int exec_loop(TExecutor *self, int64_t min_callstack_depth)
{
    while ((self->callstacktop +1) > min_callstack_depth && self->ip < self->module->bytecode->codelen && self->exit_code == 0) {
        if (self->disassemble) {
            fprintf(stderr, "mod %s ip %d (%d) %s\n", self->module->name, self->ip, self->stack->top, disasm_disassembleInstruction(self));
        }
        if (self->debug_server != NULL && self->debugging == TRUE) {
            switch (self->debugger_state) {
                case dbgSTOPPED:
                    break;
                case dbgRUN:
                    if (self->debugger_breakpoints[self->ip] == 1) {
                        self->debugger_state = dbgSTOPPED;
                    }
                    break;
                case dbgSTEP_INSTRUCTION:
                    self->debugger_state = dbgSTOPPED;
                    break;
                case dbgSTEP_SOURCE:
                    if (self->callstacktop+1 <= self->debugger_step_source_depth && self->module->debug_symbols != NULL && findDebugLine(self->module, self->ip)) {
                        self->debugger_state = dbgSTOPPED;
                    }
                    break;
                case dbgQUIT:
                    break;
            }
            server_service(self->debug_server, FALSE);
            while (self->debugger_state == dbgSTOPPED) {
                server_service(self->debug_server, TRUE);
            }
            if (self->debugger_state == dbgQUIT) {
                return 1;
            }
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
            case PUSHPOL: exec_PUSHPOL(self); break;
            case PUSHI:   exec_PUSHI(self); break;
            case LOADB:   exec_LOADB(self); break;
            case LOADN:   exec_LOADN(self); break;
            case LOADS:   exec_LOADS(self); break;
            case LOADY:   exec_LOADY(self); break;
            case LOADA:   exec_LOADA(self); break;
            case LOADD:   exec_LOADD(self); break;
            case LOADP:   exec_LOADP(self); break;
            case LOADJ:   exec_LOADJ(self); break;
            case STOREB:  exec_STOREB(self); break;
            case STOREN:  exec_STOREN(self); break;
            case STORES:  exec_STORES(self); break;
            case STOREY:  exec_STOREY(self); break;
            case STOREA:  exec_STOREA(self); break;
            case STORED:  exec_STORED(self); break;
            case STOREP:  exec_STOREP(self); break;
            case STOREJ:  exec_STOREJ(self); break;
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
            case DUP:     exec_DUP(self); break;
            case DUPX1:   exec_DUPX1(self); break;
            case DROP:    exec_DROP(self); break;
            case RET:     exec_RET(self); break;
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
            case CALLV:   exec_CALLV(self); break;
            case PUSHCI:  exec_PUSHCI(self); break;
            default:
                fatal_error("exec: Unexpected opcode: %d\n", self->module->bytecode->code[self->ip]);
        }
        self->diagnostics.total_opcodes++;
    }
    return self->exit_code;
}

cJSON *cell_writer(Cell *c)
{
    char addr[32];
    cJSON *writer = cJSON_CreateObject();
    switch (c->type) {
        case cNothing:
            cJSON_AddStringToObject(writer, "type", "none");
            cJSON_AddNullToObject(writer, "value");
            break;
        case cAddress:
            cJSON_AddStringToObject(writer, "type", "address");
            snprintf(addr, sizeof(addr), "%p", c->address);
            cJSON_AddNumberToObject(writer, "value", atof(addr)); // cJSON uses Floats for all numbers.
            break;
        case cBoolean:
            cJSON_AddStringToObject(writer, "type", "boolean");
            cJSON_AddBoolToObject(writer, "value", c->boolean);
            break;
        case cNumber:
            cJSON_AddStringToObject(writer, "type", "number");
            cJSON_AddStringToObject(writer, "value", number_to_string(c->number));
            break;
        case cString:
            cJSON_AddStringToObject(writer, "type", "string");
            cJSON_AddStringToObject(writer, "value", string_ensureNullTerminated(c->string));
            break;
        case cBytes:
            // ToDo: I don't think this is correct?  Perhaps we want a more controlled
            // output of bytes?  Maybe a hex output so every byte is listed as 0x00, 0x01, 0x02, 0x03, etc.
            // Maybe just an array of numbers that are the byte values?
            cJSON_AddStringToObject(writer, "type", "bytes");
            cJSON_AddStringToObject(writer, "value", string_ensureNullTerminated(c->string));
            break;
        case cObject:
            cJSON_AddStringToObject(writer, "type", "object");
            snprintf(addr, sizeof(addr), "%p", c->object->ptr);
            cJSON_AddNumberToObject(writer, "value", atof(addr)); // cJSON uses Floats for all numbers.
            break;
        case cArray:
            cJSON_AddStringToObject(writer, "type", "array");
            cJSON *array = cJSON_CreateArray();
            for (size_t i = 0; i < c->array->size; i++) {
                cJSON_AddItemToArray(array, cell_writer(&c->array->data[i]));
            }
            cJSON_AddItemToObject(writer, "value", array);
            break;
        case cDictionary:
            cJSON_AddStringToObject(writer, "type", "dictionary");
            cJSON *dict = cJSON_CreateObject();
            for (int i = 0; i < c->dictionary->len; i++) {
                cJSON_AddItemToObject(dict, string_ensureNullTerminated(c->dictionary->data[i].key), cell_writer(c->dictionary->data[i].value));
            }
            cJSON_AddItemToObject(writer, "value", dict);
            break;
        case cOther:
            cJSON_AddStringToObject(writer, "type", "other");
            snprintf(addr, sizeof(addr), "%p", c->other);
            cJSON_AddNumberToObject(writer, "value", atof(addr)); // cJSON uses Floats for all numbers.
            break;
    }
    return writer;
}

void exec_handleGET(TExecutor *exec, TString *path, HttpResponse *response)
{
    cJSON *writer = NULL;
    TString *r = NULL;
    TStringArray *parts = string_splitString(path, '/');

    if (string_compareCString(path, "/break") == 0) {
        response->code = 200;
        writer = cJSON_CreateObject();
        cJSON *ar = cJSON_CreateArray();
        // ToDo: Convert this to a dynamic array of breakpoints, or some other method to hold all of the break points.
        for (unsigned int i = 0; i < exec->modules[0]->bytecode->codelen; i++) {
            if (exec->debugger_breakpoints[i]) {
                 cJSON_AddItemToArray(ar, cJSON_CreateNumber(i));
            }
        }
        cJSON_AddItemToObject(writer, "debugger_breakpoints", ar);
    } else if (string_compareCString(path, "/callstack") == 0) {
        response->code = 200;
        writer = cJSON_CreateArray();
        for (int i = exec->callstacktop; i != -1; i--) {
            cJSON *element = cJSON_CreateObject();
            cJSON_AddStringToObject(element, "module",  exec->callstack[i].mod->name ? exec->callstack[i].mod->name : "");
            cJSON_AddNumberToObject(element, "ip",      (double)exec->callstack[i].ip);  // cJSON uses Floats for all numbers.
            cJSON_AddItemToArray(writer, element);
        }
    } else if (string_compareCString(path, "/frames") == 0) {
        response->code = 200;
        writer = cJSON_CreateArray();
        for (int i = exec->framestack->top; i != -1; i--) {
            cJSON *frame = cJSON_CreateObject();
            cJSON *cells = cJSON_CreateArray();
            for (uint32_t l = 0; l < exec->framestack->data[i]->frame_size; l++) {
                cJSON_AddItemToArray(cells, cell_writer(&exec->framestack->data[i]->locals[l]));
            }
            cJSON_AddItemToObject(frame, "locals", cells);
            cJSON_AddItemToArray(writer, frame);
        }
    } else if (parts->size >= 3 && string_compareCString(parts->data[1], "module") == 0) {
        char *modname = string_asCString(parts->data[2]);
        if (strcmp(modname, "-") == 0) {
            modname[0] = '\0';
        }
        TModule *m = module_findModule(exec, modname);
        if (m != NULL) {
            if (parts->size == 4 && string_compareCString(parts->data[3], "bytecode") == 0) {
                writer = cJSON_CreateArray();
                response->code = 200;
                for (size_t i = 0; i < m->codelen; i++) {
                    cJSON *byte = cJSON_CreateNumber((double)m->code[i]); // cJSON uses Floats for all numbers.
                    cJSON_AddItemToArray(writer, byte);
                }
            } else if (parts->size == 4 && string_compareCString(parts->data[3], "debuginfo") == 0) {
                response->code = 200;
                writer = cJSON_Duplicate(m->debug_symbols, TRUE);
            } else if (parts->size == 5 && string_compareCString(parts->data[3], "global") == 0) {
                response->code = 200;
                uint64_t index = number_to_uint64(number_from_string(string_ensureNullTerminated(parts->data[4])));
                if (index > m->bytecode->global_size) {
                    response->code = 202;
                } else {
                    writer = cell_writer(&m->globals[index]);
                }
            } else if (parts->size == 4 && string_compareCString(parts->data[3], "source") == 0) {
                response->code = 200;
                writer = cJSON_CreateArray();
                TStringArray *source = getSourceLines(m);
                for (int i = 0; i < source->size; i++) {
                    cJSON_AddItemToArray(writer, cJSON_CreateString(string_ensureNullTerminated(source->data[i])));
                }
                string_freeStringArray(source);
            }
        }
        free(modname);
    } else if (string_compareCString(path, "/modules") == 0) {
        response->code = 200;
        writer = cJSON_CreateArray();
        for (unsigned int i = 0; i < exec->module_count; i++) {
            cJSON_AddItemToArray(writer, cJSON_CreateString(exec->modules[i]->name));
        }
    } else if (string_compareCString(path, "/opstack") == 0) {
        response->code = 200;
        writer = cJSON_CreateArray();
        for (int i = exec->stack->top; i != -1; i--) {
            cJSON_AddItemToArray(writer, cell_writer(exec->stack->data[i]));
        }
    } else if (string_compareCString(path, "/status") == 0) {
        response->code = 200;
        writer = cJSON_CreateObject();
        cJSON_AddStringToObject(writer, "state",        DebuggerStateName[exec->debugger_state]);
        cJSON_AddStringToObject(writer, "module",       exec->module->name);
        cJSON_AddNumberToObject(writer, "ip",           exec->ip);
        cJSON_AddNumberToObject(writer, "log_messages", (double)exec->debugger_log->size); // cJSON uses Floats for all numbers.
    }
    if (response->code == 200 && writer != NULL) {
        char *s = cJSON_Print(writer);
        r = string_createCString(s);
        free(s);
        cJSON_Delete(writer);
    } else if (response->code == 404) {
        string_ensureNullTerminated(path);
        char err[1056]; // 1024 just for the potential URL path + error text.
        snprintf(err, sizeof(err), "[debug server] path not found: %s", path->data);
        r = string_createCString(err);
    }
    if (r == NULL) {
        r = string_createCString("{}");
    }
    response->content = r;
    string_freeStringArray(parts);
}

void exec_handlePOST(TExecutor *exec, TString *path, TString *data, HttpResponse *response)
{
    cJSON *writer = NULL;
    TString *r = NULL;
    TStringArray *parts = string_splitString(path, '/');

    if (parts->size == 3 && string_compareCString(parts->data[1], "break") == 0) {
        response->code = 200;
        // ToDo: Allow setting breakpoints inside other modules!
        // ToDo: /break/-/1
        // ToDo: /break/string/1329 - to break on entry of string.isBlank() for example
        size_t addr = atoi(string_ensureNullTerminated(parts->data[2]));
        if (string_compareCString(data, "true") == 0) {
            exec->debugger_breakpoints[addr] = 1;
        } else {
            exec->debugger_breakpoints[addr] = 0;
        }
    } else if (string_compareCString(path, "/continue") == 0) {
        response->code = 200;
        exec->debugger_state = dbgRUN;
    } else if (string_compareCString(path, "/log") == 0) {
        response->code = 200;
        writer = cJSON_CreateArray();
        for (int i = 0; i < exec->debugger_log->size; i++) {
            cJSON_AddItemToArray(writer, cJSON_CreateString(string_ensureNullTerminated(exec->debugger_log->data[i])));
        }
        string_clearStringArray(exec->debugger_log);
    } else if (string_compareCString(path, "/quit") == 0) {
        response->code = 200;
        exec->debugger_state = dbgQUIT;
    } else if (string_compareCString(path, "/step/instruction") == 0) {
        response->code = 200;
        exec->debugger_state = dbgSTEP_INSTRUCTION;
    } else if (parts->size == 4 && string_compareCString(parts->data[1], "step") == 0 && string_compareCString(parts->data[2], "source") == 0) {
        response->code = 200;
        exec->debugger_state = dbgSTEP_SOURCE;
        exec->debugger_step_source_depth = exec->callstacksize + atoi(string_ensureNullTerminated(parts->data[3]));
    } else if (string_compareCString(path, "/stop") == 0) {
        response->code = 200;
        exec->debugger_state = dbgSTOPPED;
    }
    if (response->code == 200) {
        if (writer != NULL) {
            char *s = cJSON_PrintBuffered(writer, 512, 0);
            r = string_createCString(s);
            free(s);
            cJSON_Delete(writer);
        }
    } else if (response->code == 0) {
        response->code = 404;
        const char *pth = string_ensureNullTerminated(path);
        char err[1056]; // 1024 just for the potential URL path + error text.
        snprintf(err, sizeof(err), "[debug server] path not found: %s", pth);
        r = string_createCString(err);
    }
    if (r == NULL) {
        r = string_createCString("{}");
    }
    string_freeStringArray(parts);
    response->content = r;
}
