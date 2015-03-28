#ifndef RTL_COMPILE_H
#define RTL_COMPILE_H

#include "ast.h"

void rtl_compile_init(Scope *scope);
bool rtl_import(const Token &token, Scope *scope, const std::string &name, const std::string &alias);

#endif
