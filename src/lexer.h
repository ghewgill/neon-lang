#ifndef LEXER_H
#define LEXER_H

#include <memory>
#include <vector>

#include "token.h"

class TokenizedSource {
public:
    TokenizedSource(): source_path(), source_hash(), source_text(), tokens() {}
    std::string source_path;
    std::string source_hash;
    std::string source_text;
    std::vector<Token> tokens;
};

std::unique_ptr<TokenizedSource> tokenize(const std::string &source_path, const std::string &source);

#endif
