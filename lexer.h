#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "number.h"

enum TokenType {
    NONE,
    END_OF_FILE,
    NUMBER,
    STRING,
    IDENTIFIER,
    LPAREN,
    RPAREN,
    COLON,
    ASSIGN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    COMMA,
    IF,
    THEN,
    END,
    WHILE,
    DO,
    VAR,
    FUNCTION,
    RETURN,
};

class Token {
public:
    std::string source;
    int line;
    int column;
    TokenType type;
    std::string text;
    Number value;

    std::string tostring() const;
};

std::vector<Token> tokenize(const std::string &source);

#endif
