#include "sys.h"

#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "cell.h"
#include "exec.h"
#include "global.h"
#include "number.h"
#include "stack.h"

void sys_initModule(int argc, char *argv[], int iStart, Cell *pDest)
{
    for (int i = iStart; i < argc; i++) {
        Cell *parm = cell_fromCString(argv[i]);
        cell_arrayAppendElement(pDest, *parm);
        cell_freeCell(parm);
    }
}

void sys_exit(TExecutor *exec)
{
    char ex[64];
    Number x = top(exec->stack)->number; pop(exec->stack);

    if (!number_is_integer(x)) {
        sprintf(ex, "%s %s", "sys.exit invalid parameter:", number_to_string(x));
        exec->rtl_raise(exec, "InvalidValueException", ex);
        return;
    }
    int r = number_to_sint32(x);
    if (r < 0 || r > 255) {
        sprintf(ex, "%s %s", "sys.exit invalid parameter:", number_to_string(x));
        exec->rtl_raise(exec, "InvalidValueException", ex);
        return;
    }
    exit(r);
}
