#ifndef BYTECODE_H
#define BYTECODE_H

#include <string>
#include <vector>

class Bytecode {
public:
    Bytecode();
    explicit Bytecode(const std::vector<unsigned char> &obj);

    typedef std::vector<unsigned char> Bytes;

    /*
     * Type descriptors are one of the following patterns:
     *
     * B        - boolean
     * N        - number
     * S        - string
     * Y        - bytes
     * A<x>     - array
     * D<x>     - dictionary
     * R[xyz..] - record, fields x, y, z
     * P<x>     - pointer
     * E[id1,id2,...] - enum
     */

    struct Type {
        unsigned int name;
        unsigned int descriptor;
    };

    struct Constant {
        unsigned int name;
        unsigned int type;
        Bytes value;
    };

    struct Variable {
        unsigned int name;
        unsigned int type;
        unsigned int index;
    };

    struct Function {
        unsigned int name;
        unsigned int descriptor;
        unsigned int offset;
    };

    struct ExceptionInfo {
        unsigned int start;
        unsigned int end;
        unsigned int excid;
        unsigned int handler;
    };

    size_t global_size;
    std::vector<std::string> strtable;
    std::vector<Type> types;
    std::vector<Constant> constants;
    std::vector<Variable> variables;
    std::vector<Function> functions;
    std::vector<ExceptionInfo> exceptions;
    Bytes code;

    Bytes getBytes() const;

private:
    std::vector<std::string> getstrtable(Bytes::const_iterator start, Bytes::const_iterator end, size_t &i);
};

#endif
