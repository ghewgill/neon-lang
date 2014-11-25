#ifndef BYTECODE_H
#define BYTECODE_H

#include <string>
#include <vector>

class Bytecode {
public:
    Bytecode(const std::vector<unsigned char> &obj);

    typedef std::vector<unsigned char> bytecode;

    class ExceptionInfo {
    public:
        unsigned int start;
        unsigned int end;
        unsigned int excid;
        unsigned int handler;
    };

    size_t global_size;
    std::vector<std::string> strtable;
    std::vector<ExceptionInfo> exceptions;
    bytecode code;

private:
    std::vector<std::string> getstrtable(bytecode::const_iterator start, bytecode::const_iterator end, size_t &i);
};

#endif
