#ifndef RTL_H
#define RTL_H

#include <stack>

#include "ast.h"
#include "cell.h"

void rtl_init(Scope *scope);
void rtl_import(Scope *scope, const std::string &name);
void rtl_call(std::stack<Variant> &stack, const std::string &name);

#endif
