#ifndef COMPILER_H
#define COMPILER_H

#include <map>
#include <vector>

namespace ast { class Program; }
class DebugInfo;

std::vector<unsigned char> compile(const ast::Program *p, DebugInfo *debug);

#endif
