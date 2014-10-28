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
    LBRACKET,
    RBRACKET,
    COLON,
    ASSIGN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    MOD,
    EXP,
    EQUAL,
    NOTEQUAL,
    LESS,
    GREATER,
    LESSEQ,
    GREATEREQ,
    COMMA,
    IF,
    THEN,
    ELSE,
    END,
    WHILE,
    DO,
    FOR,
    IN,
    TO,
    VAR,
    FUNCTION,
    RETURN,
    FALSE,
    TRUE,
    AND,
    OR,
    NOT,
    ARRAY,
    DICTIONARY,
    DOT,
    TYPE,
    RECORD,
};

class Token {
public:
    Token(): source(), line(0), column(0), type(), text(), value() {}
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
