#ifndef EXEC_H
#define EXEC_H

#include <string>
#include <vector>

class DebugInfo;
class ICompilerSupport;

void exec(const std::string &source_path, const std::vector<unsigned char> &obj, const DebugInfo *debug, ICompilerSupport *support, bool enable_assert, int argc, char *argv[]);

#endif
