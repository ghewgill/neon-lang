#ifndef _EXEC_H
#define _EXEC_H
#include <stdint.h>
#include <time.h>

#include "number.h"
#include "util.h"

typedef struct tagTModule {
    char *name;
} TModule;

typedef struct tagTExecutor {
    struct tagTBytecode *object;
    unsigned int ip;
    struct tagTStack *stack;
    /*struct tagTStack *callstack; */
    unsigned int callstack[300];
    int32_t callstacktop;
    int32_t param_recursion_limit;
    struct tagTCell *globals;
    struct tagTFrameStack *framestack;
    BOOL enable_assert;
    BOOL debug;
    void (*rtl_raise)(struct tagTExecutor *, const char *, const char *, Number);
    struct tagTModule *module;

    /* Debug / Diagnostic fields */
    struct {
        uint64_t total_opcodes;
        uint32_t callstack_max_height;
        clock_t time_start;
        clock_t time_end;
    } diagnostics;
} TExecutor;

void exec_loop(struct tagTExecutor *self);
int exec_run(struct tagTExecutor *self, BOOL enable_assert);

void exec_rtl_raiseException(struct tagTExecutor *self, const char *name, const char *info, Number code);

#endif
