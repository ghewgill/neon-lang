#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum TokenType {
    NONE,
    END_OF_FILE,
    NUMBER,
    IDENTIFIER,
    LPAREN,
    RPAREN,
    ASSIGN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    COMMA,
};

class Token {
public:
    std::string source;
    int line;
    int column;
    TokenType type;
    std::string text;
    int value;

    std::string tostring() const;
};

std::vector<Token> tokenize(const std::string &source);

#endif
