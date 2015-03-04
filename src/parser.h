#ifndef PARSE_H
#define PARSE_H

#include "lexer.h"

namespace pt { class Program; }

const pt::Program *parse(const std::vector<Token> &tokens);

#endif
