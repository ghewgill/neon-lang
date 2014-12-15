#ifndef COMPILER_H
#define COMPILER_H

#include <map>
#include <vector>

class Program;
class DebugInfo;

std::vector<unsigned char> compile(const Program *p, DebugInfo *debug);

#endif
