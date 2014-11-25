#include "bytecode.h"

Bytecode::Bytecode(const std::vector<unsigned char> &obj)
  : global_size(0),
    strtable(),
    exceptions(),
    code()
{
    size_t i = 0;
    global_size = (obj[i] << 8 | obj[i+1]);
    i += 2;
    unsigned int strtablesize = (obj[i] << 8) | obj[i+1];
    i += 2;
    strtable = getstrtable(obj.begin() + 4, obj.begin() + 4 + strtablesize, i);
    unsigned int exceptionsize = (obj[i] << 8) | obj[i+1];
    i += 2;
    while (exceptionsize > 0) {
        ExceptionInfo e;
        e.start = (obj[i] << 8) | obj[i+1];
        i += 2;
        e.end = (obj[i] << 8) | obj[i+1];
        i += 2;
        e.excid = (obj[i] << 8) | obj[i+1];
        i += 2;
        e.handler = (obj[i] << 8) | obj[i+1];
        i += 2;
        exceptions.push_back(e);
        exceptionsize--;
    }
    code = bytecode(obj.begin() + i, obj.end());
}

std::vector<std::string> Bytecode::getstrtable(bytecode::const_iterator start, bytecode::const_iterator end, size_t &i)
{
    std::vector<std::string> r;
    while (start != end) {
        std::string s;
        while (*start != 0) {
            s.push_back(*start);
            ++start;
            i++;
        }
        r.push_back(s);
        ++start;
        i++;
    }
    return r;
}
