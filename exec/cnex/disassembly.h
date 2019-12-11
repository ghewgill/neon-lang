#ifndef DISASSEMBLY_H
#define DISASSEMBLY_H
#include <stdlib.h>

struct tagTExecutor;

const char *disasm_disassembleInstruction(struct tagTExecutor *exec);

#endif
