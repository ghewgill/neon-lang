#define __STDC_LIMIT_MACROS // for SIZE_MAX, see https://stackoverflow.com/questions/30472731/which-c-standard-header-defines-size-max

#include "rtl_exec.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

typedef void (*Thunk)(opstack<Cell> &stack, void *func);

static std::map<std::string, size_t> FunctionNames;
static std::map<std::string, size_t> VariableNames;

#include "thunks.inc"
#include "functions_exec.inc"
#include "variables_exec.inc"

void rtl_exec_init(int argc, char *argv[])
{
    extern void rtl_os_init();
    rtl_os_init();
    extern void rtl_sys_init(int, char *[]);
    rtl_sys_init(argc, argv);

    FunctionNames.clear();
    VariableNames.clear();

    size_t i = 0;
    for (auto f: BuiltinFunctions) {
        FunctionNames[f.name] = i;
        i++;
    }

    i = 0;
    for (auto v: BuiltinVariables) {
        VariableNames[v.name] = i;
        i++;
    }
}

void rtl_call(opstack<Cell> &stack, const std::string &name, size_t &token)
{
    if (token == SIZE_MAX) {
        auto f = FunctionNames.find(name);
        if (f == FunctionNames.end()) {
            fprintf(stderr, "neon: function not found: %s\n", name.c_str());
            abort();
        }
        token = f->second;
    }
    auto &fn = BuiltinFunctions[token];
    fn.thunk(stack, fn.func);
}

Cell *rtl_variable(const std::string &name)
{
    return BuiltinVariables[VariableNames[name]].value;
}
