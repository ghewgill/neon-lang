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
    for (int i = 0; i < exec->module_count; i++) {
        TString *modname = string_createCString(exec->modules[i]->name);
        r = string_compareString(name, modname);
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
