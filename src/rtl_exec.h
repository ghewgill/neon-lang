#ifndef RTL_H
#define RTL_H

#include <stack>

#include "cell.h"

void rtl_exec_init(int argc, char *argv[]);
void rtl_call(std::stack<Cell> &stack, const std::string &name, size_t &token);

#endif
