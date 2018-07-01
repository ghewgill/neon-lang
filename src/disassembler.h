#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <ostream>
#include <vector>

class Bytecode;
class DebugInfo;

std::string disassemble_instruction(const Bytecode &obj, std::size_t &index);
void disassemble(const std::vector<unsigned char> &obj, std::ostream &out, const DebugInfo *debug);

#endif
