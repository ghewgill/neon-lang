#ifndef SUPPORT_H
#define SUPPORT_H

#include <string>
#include <vector>

#include "analyzer.h"

class ICompilerSupport {
public:
    virtual ~ICompilerSupport() {}
    virtual bool loadBytecode(const std::string &module, Bytecode &bytecode) = 0;
};

class PathSupport: public ICompilerSupport {
public:
    PathSupport(const std::string &source_path);
    std::pair<std::string, std::string> findModule(const std::string &name);
private:
    std::vector<std::string> paths;
};

class CompilerSupport: public PathSupport {
public:
    CompilerSupport(const std::string &source_path): PathSupport(source_path) {}
    virtual bool loadBytecode(const std::string &name, Bytecode &object) override;
};

class RuntimeSupport: public PathSupport {
public:
    RuntimeSupport(const std::string &source_path): PathSupport(source_path) {}
    virtual bool loadBytecode(const std::string &name, Bytecode &object) override;
};

#endif
