#ifndef DEBUGGER_H
#define DEBUGGER_H

struct tagTExecutor;

void debugger_breakpoint(struct tagTExecutor *exec);
void debugger_log(struct tagTExecutor *exec);

#endif
