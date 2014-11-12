#include "bytecode.h"

Bytecode::Bytecode(const std::vector<unsigned char> &obj)
  : global_size(0),
    strtable(),
    code()
{
    global_size = (obj[0] << 8 | obj[1]);
    unsigned int strtablesize = (obj[2] << 8) | obj[3];
    strtable = getstrtable(obj.begin() + 4, obj.begin() + 4 + strtablesize);
    code = bytecode(obj.begin() + 4 + strtablesize, obj.end());
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
