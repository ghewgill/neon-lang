#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "token.h"

std::vector<Token> tokenize(const std::string &source);

#endif
