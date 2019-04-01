#include "runtime.h"

#include "cell.h"
#include "exec.h"
#include "nstring.h"
#include "stack.h"

void runtime_executorName(TExecutor *exec)
{
    push(exec->stack, cell_fromString(string_createCString("cnex")));
}
