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
        Type(): name(0), descriptor(0) {}
        unsigned int name;
        unsigned int descriptor;
    };

    struct Constant {
        Constant(): name(0), type(0), value() {}
        unsigned int name;
        unsigned int type;
        Bytes value;
    };

    struct Variable {
        Variable(): name(0), type(0), index(0) {}
        unsigned int name;
        unsigned int type;
        unsigned int index;
    };

    struct Function {
        Function(): name(0), descriptor(0), entry(0) {}
        unsigned int name;
        unsigned int descriptor;
        unsigned int entry;
    };

    struct ExceptionExport {
        ExceptionExport(): name(0) {}
        unsigned int name;
    };

    struct ExceptionInfo {
        unsigned int start;
        unsigned int end;
        unsigned int excid;
        unsigned int handler;
    };

    Bytes obj;
    std::string source_hash;
    size_t global_size;
    std::vector<std::string> strtable;
    std::vector<Type> types;
    std::vector<Constant> constants;
    std::vector<Variable> variables;
    std::vector<Function> functions;
    std::vector<ExceptionExport> exception_exports;
    std::vector<std::pair<unsigned int, std::string>> imports;
    std::vector<ExceptionInfo> exceptions;
    Bytes code;

    Bytes getBytes() const;

private:
    std::vector<std::string> getstrtable(const unsigned char *start, const unsigned char *end);
};

#endif
