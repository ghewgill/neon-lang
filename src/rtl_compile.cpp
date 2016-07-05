#include "rtl_compile.h"

#include "functions_compile.inc"
#include "variables_compile.inc"

static const Type *resolve_type(const PredefinedType &ptype, Scope *scope)
{
    auto *type = ptype.type;
    if (type == nullptr && scope != nullptr) {
        Name *name = scope->lookupName(ptype.modtypename);
        type = dynamic_cast<const Type *>(name);
    }
    return type;
}

void rtl_compile_init(Scope *scope)
{
    //init_builtin_constants(scope);
    for (auto f: BuiltinFunctions) {
        std::vector<const ParameterType *> params;
        for (int i = 0; i < f.count; i++) {
            auto &p = f.params[i];
            params.push_back(new ParameterType(Token(p.name), p.mode, resolve_type(p.ptype, nullptr), nullptr));
        }
        scope->addName(Token(IDENTIFIER, f.name), f.name, new PredefinedFunction(f.name, new TypeFunction(resolve_type(f.returntype, nullptr), params)));
    }
}

bool rtl_import(const std::string &module, Module *mod)
{
    std::string prefix = module + "$";
    init_builtin_variables(module, mod->scope);
    bool any = false;
    for (auto f: BuiltinFunctions) {
        std::string qualified_name(f.name);
        if (f.exported && qualified_name.substr(0, prefix.length()) == prefix) {
            std::vector<const ParameterType *> params;
            for (int i = 0; i < f.count; i++) {
                auto &p = f.params[i];
                params.push_back(new ParameterType(Token(p.name), p.mode, resolve_type(p.ptype, mod->scope), nullptr));
            }
            mod->scope->addName(Token(IDENTIFIER, f.name), qualified_name.substr(prefix.length()), new PredefinedFunction(f.name, new TypeFunction(resolve_type(f.returntype, mod->scope), params)));
            any = true;
        }
    }
    return any;
}
