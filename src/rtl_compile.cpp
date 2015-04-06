#include "rtl_compile.h"

#include "constants_compile.inc"
#include "functions_compile.inc"

void rtl_compile_init(Scope *scope)
{
    init_builtin_constants(scope);
    for (auto f: BuiltinFunctions) {
        std::vector<const ParameterType *> params;
        for (int i = 0; i < f.count; i++) {
            auto &p = f.params[i];
            params.push_back(new ParameterType(Token(), p.m, p.p, nullptr));
        }
        scope->addName(Token(), f.name, new PredefinedFunction(f.name, new TypeFunction(f.returntype, params)));
    }
}

bool rtl_import(const std::string &module, Module *mod)
{
    std::string prefix = module + "$";
    init_builtin_constants(module, mod->scope);
    bool any = false;
    for (auto f: BuiltinFunctions) {
        std::string qualified_name(f.name);
        if (qualified_name.substr(0, prefix.length()) == prefix) {
            std::vector<const ParameterType *> params;
            for (auto p: f.params) {
                if (p.p == nullptr) {
                    break;
                }
                params.push_back(new ParameterType(Token(), p.m, p.p, nullptr));
            }
            mod->scope->addName(Token(), qualified_name.substr(prefix.length()), new PredefinedFunction(f.name, new TypeFunction(f.returntype, params)));
            any = true;
        }
    }
    return any;
}
