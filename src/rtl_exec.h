#ifndef RTL_H
#define RTL_H

#include "cell.h"
#include "opstack.h"

#include "exceptions.inc"

class RtlException {
public:
    std::string name;
    utf8string info;
public:
    RtlException(const ExceptionName &name, const utf8string &info): name(name.name), info(info) {}
};

void rtl_exec_init(int argc, char *argv[]);
void rtl_call(opstack<Cell> &stack, const std::string &name, size_t &token);
Cell *rtl_variable(const std::string &name);

#endif
