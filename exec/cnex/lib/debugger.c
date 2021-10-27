#include "debugger.h"

#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "stack.h"


void debugger_breakpoint(TExecutor *exec)
{
    exec->debugger_state = dbgSTOPPED;
}

void debugger_log(TExecutor *exec)
{
    TString *msg = string_fromString(top(exec->stack)->string);

    string_appendStringArrayElement(exec->debugger_log, msg);
    pop(exec->stack);
}
