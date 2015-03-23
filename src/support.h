#ifndef SUPPORT_H
#define SUPPORT_H

#include <string>

#include "analyzer.h"

class ICompilerSupport {
public:
    virtual ~ICompilerSupport() {}
    virtual bool loadBytecode(const std::string &module, Bytecode &bytecode) = 0;
};

class CompilerSupport: public ICompilerSupport {
public:
    virtual bool loadBytecode(const std::string &name, Bytecode &object) override;
};

class RuntimeSupport: public ICompilerSupport {
public:
    virtual bool loadBytecode(const std::string &name, Bytecode &object) override;
};

#endif
