#ifndef EXEC_H
#define EXEC_H

#include <vector>

class DebugInfo;

void exec(const std::vector<unsigned char> &obj, const DebugInfo *debug, int argc, char *argv[]);

#endif
