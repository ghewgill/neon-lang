#ifndef EXEC_H
#define EXEC_H

#include <vector>

class DebugInfo;
class ICompilerSupport;

void exec(const std::vector<unsigned char> &obj, const DebugInfo *debug, ICompilerSupport *support, int argc, char *argv[]);

#endif
