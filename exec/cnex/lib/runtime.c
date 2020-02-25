#include "runtime.h"

#include "cell.h"
#include "exec.h"
#include "module.h"
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

void runtime_isModuleImported(TExecutor *exec)
{
    TString *name = top(exec->stack)->string;
    int r = 0;
    for (unsigned int i = 0; i < exec->module_count; i++) {
        TString *modname = string_createCString(exec->modules[i]->name);
        r = string_compareString(name, modname) == 0;
        string_freeString(modname);
        if (r) {
            break;
        }
    }
    pop(exec->stack);
    push(exec->stack, cell_fromBoolean(r));
}

void runtime_moduleIsMain(TExecutor *exec)
{
    push(exec->stack, cell_fromBoolean(exec->module == exec->modules[0]));
}

void runtime_setRecursionLimit(TExecutor *exec)
{
    Number n = top(exec->stack)->number; pop(exec->stack);

    exec->param_recursion_limit = number_to_sint32(n);
}
