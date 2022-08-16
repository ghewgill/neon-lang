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
            params.push_back(new ast::ParameterType(Token(p.name), p.mode, resolve_type(p.ptype, scope), nullptr));
        }
        ast::PredefinedFunction *func = new ast::PredefinedFunction(f.name, new ast::TypeFunction(resolve_type(f.returntype, scope), params, f.variadic));
        std::string fname = f.name;
        if (not (fname.substr(0, 7) == "global$" && fname.find("__") != std::string::npos)) {
            scope->addName(Token(IDENTIFIER, f.name), f.name, func);
        }
    }
}

void rtl_compile_init_methods(ast::Scope *scope)
{
    for (auto f: BuiltinFunctions) {
        std::vector<const ast::ParameterType *> params;
        for (int i = 0; i < f.count; i++) {
            auto &p = f.params[i];
            params.push_back(new ast::ParameterType(Token(p.name), p.mode, resolve_type(p.ptype, scope), nullptr));
        }
        ast::PredefinedFunction *func = new ast::PredefinedFunction(f.name, new ast::TypeFunction(resolve_type(f.returntype, scope), params, f.variadic));
        std::string fname = f.name;
        if (fname.substr(0, 7) == "global$" && fname.find("__") != std::string::npos) {
            auto under = fname.find("__");
            auto classname = fname.substr(7, under-7);
            auto *name = scope->lookupName(classname);
            auto *type = dynamic_cast<ast::Type *>(name);
            type->methods[fname.substr(under+2)] = func;
        }
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
            mod->scope->addName(Token(IDENTIFIER, f.name), qualified_name.substr(prefix.length()), new ast::PredefinedFunction(f.name, new ast::TypeFunction(resolve_type(f.returntype, mod->scope), params, f.variadic)));
            any = true;
        }
    }
    return any;
}
