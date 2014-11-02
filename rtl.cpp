#include <assert.h>
#include <iostream>
#include <stack>
#include <string>
#include <sys/time.h>

#include "ast.h"
#include "cell.h"
#include "number.h"

typedef void (*Thunk)(std::stack<Variant> &stack, void *func);

static std::map<std::string, std::pair<Thunk, void *> > Functions;

namespace rtl {

Number abs(Number x)
{
    return number_abs(x);
}

std::string chr(Number x)
{
    assert(number_is_integer(x));
    return std::string(1, static_cast<char>(number_to_uint32(x)));
}

std::string concat(const std::string &a, const std::string &b)
{
    return a + b;
}

std::string input(const std::string &prompt)
{
    std::cout << prompt;
    std::string r;
    std::getline(std::cin, r);
    return r;
}

Number max(Number a, Number b)
{
    if (number_is_greater(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number min(Number a, Number b)
{
    if (number_is_less(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number num(const std::string &s)
{
    return number_from_string(s);
}

Number ord(const std::string &s)
{
    assert(s.length() == 1);
    return number_from_uint32(s.at(0));
}

void print(const std::string &s)
{
    std::cout << s << "\n";
}

std::string splice(const std::string &t, const std::string &s, Number offset, Number length)
{
    uint32_t o = number_to_uint32(offset);
    return s.substr(0, o) + t + s.substr(o + number_to_uint32(length));
}

std::string str(Number x)
{
    return number_to_string(x);
}

std::string strb(bool x)
{
    return x ? "TRUE" : "FALSE";
}

std::string substring(const std::string &s, Number offset, Number length)
{
    assert(number_is_integer(offset));
    assert(number_is_integer(length));
    return s.substr(number_to_uint32(offset), number_to_uint32(length));
}

Number math$acos(Number x)
{
    return number_acos(x);
}

Number math$asin(Number x)
{
    return number_asin(x);
}

Number math$atan(Number x)
{
    return number_atan(x);
}

Number math$ceil(Number x)
{
    return number_ceil(x);
}

Number math$cos(Number x)
{
    return number_cos(x);
}

Number math$exp(Number x)
{
    return number_exp(x);
}

Number math$floor(Number x)
{
    return number_floor(x);
}

Number math$log(Number x)
{
    return number_log(x);
}

Number math$sin(Number x)
{
    return number_sin(x);
}

Number math$sqrt(Number x)
{
    return number_sqrt(x);
}

Number math$tan(Number x)
{
    return number_tan(x);
}

Number time$now()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        return number_from_uint32(0);
    }
    return number_add(number_from_uint32(tv.tv_sec), number_divide(number_from_uint32(tv.tv_usec), number_from_uint32(1e6)));
}

} // namespace rtl

#include "thunks.inc"
#include "functions.inc"

void rtl_init(Scope *scope)
{
    for (auto f: BuiltinFunctions) {
        std::vector<const ParameterType *> params;
        for (auto p: f.params) {
            if (p == nullptr) {
                break;
            }
            params.push_back(new ParameterType(ParameterType::IN, p));
        }
        scope->names[f.name] = new PredefinedFunction(f.name, new TypeFunction(f.returntype, params));
        Functions[f.name] = std::make_pair(f.thunk, f.func);
    }
}

void rtl_import(Scope *scope, const std::string &name)
{
    std::string prefix = name + "$";
    Module *module = new Module(scope, name);
    for (auto f: BuiltinFunctions) {
        std::string qualified_name(f.name);
        if (qualified_name.substr(0, prefix.length()) == prefix) {
            std::vector<const ParameterType *> params;
            for (auto p: f.params) {
                if (p == nullptr) {
                    break;
                }
                params.push_back(new ParameterType(ParameterType::IN, p));
            }
            module->scope->names[qualified_name.substr(prefix.length())] = new PredefinedFunction(f.name, new TypeFunction(f.returntype, params));
        }
    }
    scope->names[name] = module;
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
