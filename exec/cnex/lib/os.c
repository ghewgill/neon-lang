#include "os.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdlib.h>
#include <string.h>

#include "exec.h"
#include "cell.h"
#include "nstring.h"
#include "number.h"
#include "stack.h"



void os_getenv(TExecutor *exec)
{
    char *name = string_asCString(top(exec->stack)->string); pop(exec->stack);

    char *r = getenv(name);
    free(name);
    if (r == NULL) {
        push(exec->stack, cell_fromCString(""));
        return;
    }
    push(exec->stack, cell_fromCString(r));
}

void os_system(TExecutor *exec)
{
    char *cmd = string_asCString(top(exec->stack)->string); pop(exec->stack);
#ifdef _WIN32
    // Terrible hack to change slashes to backslashes so cmd.exe isn't confused.
    char *p = cmd;
    while (*p && *p != ' ') {
        if (*p == '/') {
            *p = '\\';
        }
        p++;
    }
#endif
    push(exec->stack, cell_fromNumber(number_from_sint32(system(cmd))));
    free(cmd);
}
