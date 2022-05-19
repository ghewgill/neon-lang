#ifndef EXEC_H
#define EXEC_H
#include <stdint.h>
#include <time.h>

#include "httpserver.h"
#include "number.h"
#include "util.h"

struct tagTStringArray;

typedef enum eDebuggerState {
    dbgSTOPPED,
    dbgRUN,
    dbgSTEP_INSTRUCTION,
    dbgSTEP_SOURCE,
    dbgQUIT,
} DebuggerState;

typedef struct tagTExecutor {
    unsigned int ip;
    struct tagTStack *stack;
    struct CallStack {
        struct tagTModule *mod;
        unsigned int ip;
    } *callstack;
    int32_t callstacktop;
    int32_t callstacksize;
    int32_t param_recursion_limit;
    struct tagTFrameStack *framestack;
    BOOL enable_assert;
    BOOL enable_debug;
    BOOL debug;
    BOOL disassemble;
    void (*rtl_raise)(struct tagTExecutor *, const char *, const char *);
    unsigned int module_count;
    struct tagTModule *module;
    struct tagTModule **modules;
    unsigned int *init_order;
    unsigned int init_count;
    int exit_code;
    struct tagTAllocationObject *firstObject;
    size_t allocations;
    size_t collection_interval;

    /* Debug / Diagnostic fields */
    struct {
        uint64_t total_opcodes;
        uint32_t callstack_max_height;
        clock_t time_start;
        clock_t time_end;
        size_t total_allocations;
        size_t collected_objects;
    } diagnostics;

    BOOL debugging;
    HttpServerImpl *debug_server;
    IHttpServerHandler handlers;
    DebuggerState debugger_state;
    int64_t debugger_step_source_depth;
    char *debugger_breakpoints;
    struct tagTStringArray *debugger_log;
} TExecutor;

int exec_loop(TExecutor *self, int64_t min_callstack_depth);
int exec_run(struct tagTExecutor *self, BOOL enable_assert, BOOL enable_debug);

void invoke(struct tagTExecutor *self, struct tagTModule *m, int index);

void exec_rtl_raiseException(struct tagTExecutor *self, const char *name, const char *info);

#endif
