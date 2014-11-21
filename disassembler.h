#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <ostream>
#include <vector>

class DebugInfo;

void disassemble(const std::vector<unsigned char> &obj, std::ostream &out, const DebugInfo *debug);

#endif
