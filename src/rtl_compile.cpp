#include "rtl_compile.h"

#include "functions_compile.inc"
#include "variables_compile.inc"

static const ast::Type *resolve_type(const PredefinedType &ptype, ast::Scope *scope)
{
    auto *type = ptype.type;
    if (type == nullptr && scope != nullptr) {
        ast::Name *name = scope->lookupName(ptype.modtypename);
        type = dynamic_cast<const ast::Type *>(name);
    }
    return type;
}

void rtl_compile_init(ast::Scope *scope)
{
    init_builtin_functions();
    //init_builtin_constants(scope);
    for (auto f: BuiltinFunctions) {
        std::vector<const ast::ParameterType *> params;
        for (int i = 0; i < f.count; i++) {
            auto &p = f.params[i];
            params.push_back(new ast::ParameterType(Token(p.name), p.mode, resolve_type(p.ptype, nullptr), nullptr));
        }
        scope->addName(Token(IDENTIFIER, f.name), f.name, new ast::PredefinedFunction(f.name, new ast::TypeFunction(resolve_type(f.returntype, nullptr), params, false)));
    }
}

bool rtl_import(const std::string &module, ast::Module *mod)
{
    std::string prefix = module + "$";
    init_builtin_variables(module, mod->scope);
    bool any = false;
    for (auto f: BuiltinFunctions) {
        std::string qualified_name(f.name);
        if (f.exported && qualified_name.substr(0, prefix.length()) == prefix) {
            std::vector<const ast::ParameterType *> params;
            for (int i = 0; i < f.count; i++) {
                auto &p = f.params[i];
                params.push_back(new ast::ParameterType(Token(p.name), p.mode, resolve_type(p.ptype, mod->scope), nullptr));
            }
            mod->scope->addName(Token(IDENTIFIER, f.name), qualified_name.substr(prefix.length()), new ast::PredefinedFunction(f.name, new ast::TypeFunction(resolve_type(f.returntype, mod->scope), params, false)));
            any = true;
        }
    }
    return any;
}
