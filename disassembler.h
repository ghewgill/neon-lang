#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <ostream>
#include <vector>

void disassemble(const std::vector<unsigned char> &obj, std::ostream &out);

#endif
