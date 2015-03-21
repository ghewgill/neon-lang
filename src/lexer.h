#ifndef LEXER_H
#define LEXER_H

#include <vector>

#include "token.h"

struct TokenizedSource {
    TokenizedSource(): source_hash(), tokens() {}
    std::string source_hash;
    std::vector<Token> tokens;
};

TokenizedSource tokenize(const std::string &source);

#endif
