#ifndef SUPPORT_H
#define SUPPORT_H

#include <string>

#include "analyzer.h"

class CompilerSupport: public ICompilerSupport {
public:
    virtual bool loadBytecode(const std::string &name, Bytecode &object) override;
};

extern CompilerSupport g_compiler_support;

#endif
