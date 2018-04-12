#ifndef _EXEC_H
#define _EXEC_H
#include <stdint.h>
#include <time.h>

#include "bytecode.h"
#include "cell.h"
#include "framestack.h"
#include "stack.h"

typedef struct tagTModule {
    char *name;
} TModule;

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
    BOOL enable_assert;
    BOOL debug;
    void (*rtl_raise)(struct tagTExecutor *, const char *, const char *, Number);
    struct tagTModule *module;

    /* Debug / Diagnostic fields */
    uint64_t total_opcodes;
    uint32_t callstack_max_height;
    clock_t time_start;
    clock_t time_end;
} TExecutor;

void exec_loop(struct tagTExecutor *self);
int exec_run(struct tagTExecutor *self, BOOL enable_assert);

void exec_rtl_raiseException(TExecutor *self, const char *name, const char *info, Number code);

#endif
