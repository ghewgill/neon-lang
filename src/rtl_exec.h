#ifndef RTL_H
#define RTL_H

#include "cell.h"
#include "opstack.h"

#include "exceptions.inc"

class RtlException {
public:
    RtlException(const ExceptionName &name, const std::string &info, uint32_t code = 0): name(name.name), info(info), code(code) {}
    std::string name;
    std::string info;
    uint32_t code;
};

void rtl_exec_init(int argc, char *argv[]);
void rtl_call(opstack<Cell> &stack, const std::string &name, size_t &token);
Cell *rtl_variable(const std::string &name);

#endif
