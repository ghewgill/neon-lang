#ifndef RTL_COMPILE_H
#define RTL_COMPILE_H

#include "ast.h"

void rtl_compile_init(Scope *scope);
void rtl_import(const Token &token, Scope *scope, const std::string &name);

#endif
