#include "runtime.h"

#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "stack.h"

void runtime_assertionsEnabled(TExecutor *exec)
{
    push(exec->stack, cell_fromBoolean(exec->enable_assert));
}

void runtime_executorName(TExecutor *exec)
{
    push(exec->stack, cell_fromCString("cnex"));
}
