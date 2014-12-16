#include <stack>
#include <stdlib.h>
#include <string>

#include "cell.h"

typedef void (*Thunk)(std::stack<Cell> &stack, void *func);

static std::map<std::string, std::pair<Thunk, void *> > Functions;

#include "thunks.inc"
#include "functions_exec.inc"

void rtl_exec_init(int argc, char *argv[])
{
    extern void rtl_sys_init(int, char *[]);
    rtl_sys_init(argc, argv);

    for (auto f: BuiltinFunctions) {
        Functions[f.name] = std::make_pair(f.thunk, f.func);
    }
}

void rtl_call(std::stack<Cell> &stack, const std::string &name)
{
    auto f = Functions.find(name);
    if (f == Functions.end()) {
        fprintf(stderr, "neon: function not found: %s\n", name.c_str());
        abort();
    }
    f->second.first(stack, f->second.second);
}
