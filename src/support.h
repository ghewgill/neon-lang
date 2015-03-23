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

extern CompilerSupport g_compiler_support;

class RuntimeSupport: public ICompilerSupport {
public:
    virtual bool loadBytecode(const std::string &name, Bytecode &object) override;
};

extern RuntimeSupport g_runtime_support;

#endif
