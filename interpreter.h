#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>

class Program;

void interpret(const Program *program, std::ostream &stdout);

#endif
