#ifndef SUPPORT_H
#define SUPPORT_H

#include <string>
#include <vector>

class Bytecode;
class CompilerSupport;
namespace ast { class Program; }

typedef void (*CompileProc)(CompilerSupport *support, const ast::Program *);

class ICompilerSupport {
public:
    virtual ~ICompilerSupport() {}
    virtual void loadBytecode(const std::string &module, Bytecode &bytecode) = 0;
    virtual void writeOutput(const std::string &name, const std::vector<unsigned char> &content) = 0;
};

class PathSupport: public ICompilerSupport {
public:
    explicit PathSupport(const std::string &source_path, const std::vector<std::string> &libpath);
    std::pair<std::string, std::string> findModule(const std::string &name);
private:
    std::vector<std::string> paths;
};

class CompilerSupport: public PathSupport {
public:
    CompilerSupport(const std::string &source_path, const std::vector<std::string> &libpath, CompileProc cproc): PathSupport(source_path, libpath), cproc(cproc) {}
    virtual void loadBytecode(const std::string &name, Bytecode &object) override;
    virtual void writeOutput(const std::string &name, const std::vector<unsigned char> &content) override;
private:
    CompileProc cproc;
};

class RuntimeSupport: public PathSupport {
public:
    explicit RuntimeSupport(const std::string &source_path, const std::vector<std::string> &libpath): PathSupport(source_path, libpath) {}
    virtual void loadBytecode(const std::string &name, Bytecode &object) override;
    virtual void writeOutput(const std::string &, const std::vector<unsigned char> &) override {}
};

#endif
