#ifndef COMPILER_H
#define COMPILER_H

#include <vector>

class Program;

std::vector<unsigned char> compile(const Program *p);

#endif
