#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>

#include "bytecode.h"

class Program;
namespace pt { class Program; }

class ICompilerSupport {
public:
    virtual ~ICompilerSupport() {}
    virtual bool loadBytecode(const std::string &module, Bytecode &bytecode) = 0;
};

const Program *analyze(ICompilerSupport *support, const pt::Program *program);

#endif
