#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>

#include "bytecode.h"

class ICompilerSupport;
class Program;
namespace pt { class Program; }

const Program *analyze(ICompilerSupport *support, const pt::Program *program);

#endif
