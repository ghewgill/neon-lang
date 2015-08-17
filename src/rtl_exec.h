#ifndef RTL_H
#define RTL_H

#include "cell.h"
#include "opstack.h"

#include "exceptions.inc"

class RtlException {
public:
    RtlException(const ExceptionName &name, const std::string &info): name(name.name), info(info) {}
    std::string name;
    std::string info;
};

void rtl_exec_init(int argc, char *argv[]);
void rtl_call(opstack<Cell> &stack, const std::string &name, size_t &token);

#endif
