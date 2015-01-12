#include <stack>
#include <stdlib.h>
#include <string>

#include "cell.h"

typedef void (*Thunk)(std::stack<Cell> &stack, void *func);

static std::map<std::string, size_t> FunctionNames;

#include "thunks.inc"
#include "functions_exec.inc"

void rtl_exec_init(int argc, char *argv[])
{
    extern void rtl_sys_init(int, char *[]);
    rtl_sys_init(argc, argv);

    size_t i = 0;
    for (auto f: BuiltinFunctions) {
        FunctionNames[f.name] = i;
        i++;
    }
}

void rtl_call(std::stack<Cell> &stack, const std::string &name, size_t &token)
{
    if (token == SIZE_MAX) {
        auto f = FunctionNames.find(name);
        if (f == FunctionNames.end()) {
            fprintf(stderr, "neon: function not found: %s\n", name.c_str());
            abort();
        }
        token = f->second;
    }
    auto fn = BuiltinFunctions[token];
    fn.thunk(stack, fn.func);
}
