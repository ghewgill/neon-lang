#ifndef RTL_COMPILE_H
#define RTL_COMPILE_H

#include "ast.h"

void rtl_compile_init(ast::Scope *scope);
void rtl_compile_init_methods(ast::Scope *scope);
bool rtl_import(const std::string &module, ast::Module *mod);

#endif
