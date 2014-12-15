#ifndef PARSE_H
#define PARSE_H

#include "lexer.h"

class Program;

const Program *parse(const std::vector<Token> &tokens);

#endif
