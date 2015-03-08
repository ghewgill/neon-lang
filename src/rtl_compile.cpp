#include "rtl_compile.h"

#include "constants_compile.inc"
#include "functions_compile.inc"

void rtl_compile_init(Scope *scope)
{
    init_builtin_constants(scope);
    for (auto f: BuiltinFunctions) {
        std::vector<const ParameterType *> params;
        for (auto p: f.params) {
            if (p.p == nullptr) {
                break;
            }
            params.push_back(new ParameterType(p.m, p.p));
        }
        scope->addName(f.name, new PredefinedFunction(f.name, new TypeFunction(f.returntype, params)));
    }
}

void rtl_import(Scope *scope, const std::string &name)
{
    std::string prefix = name + "$";
    Module *module = new Module(Token(), scope, name);
    init_builtin_constants(name, module->scope);
    for (auto f: BuiltinFunctions) {
        std::string qualified_name(f.name);
        if (qualified_name.substr(0, prefix.length()) == prefix) {
            std::vector<const ParameterType *> params;
            for (auto p: f.params) {
                if (p.p == nullptr) {
                    break;
                }
                params.push_back(new ParameterType(p.m, p.p));
            }
            module->scope->addName(qualified_name.substr(prefix.length()), new PredefinedFunction(f.name, new TypeFunction(f.returntype, params)));
        }
    }
    scope->addName(name, module);
}
