#include <assert.h>
#include <iostream>
#include <stack>
#include <string>

#include "ast.h"
#include "cell.h"
#include "number.h"

typedef void (*Thunk)(std::stack<Variant> &stack, void *func);

static std::map<std::string, std::pair<Thunk, void *> > Functions;

namespace rtl {

std::string concat(const std::string &a, const std::string &b)
{
    return a + b;
}

std::string substring(const std::string &s, Number offset, Number length)
{
    assert(number_is_integer(offset));
    assert(number_is_integer(length));
    return s.substr(number_to_uint32(offset), number_to_uint32(length));
}

void print(const std::string &s)
{
    std::cout << s << "\n";
}

Number abs(Number x)
{
    return number_abs(x);
}

std::string str(Number x)
{
    return number_to_string(x);
}

std::string strb(bool x)
{
    return x ? "TRUE" : "FALSE";
}

} // namespace rtl

#include "thunks.inc"
#include "functions.inc"

void rtl_init(Scope *scope)
{
    for (auto f: BuiltinFunctions) {
        std::vector<const Type *> args;
        for (auto a: f.args) {
            if (a == nullptr) {
                break;
            }
            args.push_back(a);
        }
        scope->names[f.name] = new PredefinedFunction(f.name, new TypeFunction(f.returntype, args));
        Functions[f.name] = std::make_pair(f.thunk, f.func);
    }
}

void rtl_call(std::stack<Variant> &stack, const std::string &name)
{
    auto f = Functions.find(name);
    if (f == Functions.end()) {
        fprintf(stderr, "simple: function not found: %s\n", name.c_str());
        abort();
    }
    f->second.first(stack, f->second.second);
}
