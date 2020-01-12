#ifndef BYTECODE_H
#define BYTECODE_H

#include <string>
#include <vector>

class Bytecode {
public:
    Bytecode();
    bool load(const std::string &source_path, const std::vector<unsigned char> &bytes);

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
        Function(): name(0), descriptor(0), index(0) {}
        unsigned int name;
        unsigned int descriptor;
        unsigned int index;
    };

    struct ExceptionExport {
        ExceptionExport(): name(0) {}
        unsigned int name;
    };

    struct Interface {
        Interface(): name(0), method_descriptors() {}
        unsigned int name;
        std::vector<std::pair<unsigned int, unsigned int>> method_descriptors;
    };

    struct ModuleImport {
        ModuleImport(): name(0), hash() {}
        unsigned int name;
        std::string hash;
    };

    struct FunctionInfo {
        FunctionInfo(): name(0), nest(0), params(0), locals(0), entry(0) {}
        FunctionInfo(unsigned int name, unsigned int nest, unsigned int params, unsigned int locals, unsigned int entry): name(name), nest(nest), params(params), locals(locals), entry(entry) {}
        unsigned int name;
        unsigned int nest;
        unsigned int params;
        unsigned int locals;
        unsigned int entry;
    };

    struct ExceptionInfo {
        unsigned int start;
        unsigned int end;
        unsigned int excid;
        unsigned int handler;
        unsigned int stack_depth;
    };

    struct ClassInfo {
        ClassInfo(): name(0), interfaces() {}
        unsigned int name;
        std::vector<std::vector<unsigned int>> interfaces;
    };

    Bytes obj;
    std::string source_path;
    std::string source_hash;
    size_t global_size;
    std::vector<std::string> strtable;
    std::vector<Type> export_types;
    std::vector<Constant> export_constants;
    std::vector<Variable> export_variables;
    std::vector<Function> export_functions;
    std::vector<ExceptionExport> export_exceptions;
    std::vector<Interface> export_interfaces;
    std::vector<ModuleImport> imports;
    std::vector<FunctionInfo> functions;
    std::vector<ExceptionInfo> exceptions;
    std::vector<ClassInfo> classes;
    Bytes code;

    Bytes getBytes() const;

    static void put_vint(std::vector<unsigned char> &obj, unsigned int x);
    static void put_vint(std::vector<unsigned char> &obj, unsigned int x, size_t width);
    static void put_vint_size(std::vector<unsigned char> &obj, size_t x);
    static unsigned int get_vint(const std::vector<unsigned char> &obj, size_t &i);
};

#endif
