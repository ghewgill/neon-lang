#include "bytecode.h"

Bytecode::Bytecode(const std::vector<unsigned char> &obj)
  : strtable(),
    code()
{
    unsigned int strtablesize = (obj[0] << 8) | obj[1];
    strtable = getstrtable(obj.begin() + 2, obj.begin() + 2 + strtablesize);
    code = bytecode(obj.begin() + 2 + strtablesize, obj.end());
}

std::vector<std::string> Bytecode::getstrtable(bytecode::const_iterator start, bytecode::const_iterator end)
{
    std::vector<std::string> r;
    while (start != end) {
        std::string s;
        while (*start != 0) {
            s.push_back(*start);
            ++start;
        }
        r.push_back(s);
        ++start;
    }
    return r;
}
