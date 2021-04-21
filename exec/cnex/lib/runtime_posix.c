#include "runtime.h"

#include <iso646.h>

#include "assert.h"
#include "cell.h"
#include "enums.h"
#include "exec.h"
#include "object.h"
#include "stack.h"


void runtime_createObject(TExecutor *exec)
{
    pop(exec->stack);

    push(exec->stack, cell_fromObject(NULL));
}

