#ifndef TOKEN_H
#define TOKEN_H

#include <string>

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
    LBRACE,
    RBRACE,
    COLON,
    ASSIGN,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    MOD,
    EXP,
    CONCAT,
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
    VAR,
    FUNCTION,
    RETURN,
    FALSE,
    TRUE,
    AND,
    OR,
    NOT,
    FOR,
    TO,
    STEP,
    ARRAY,
    DICTIONARY,
    DOT,
    TYPE,
    RECORD,
    ENUM,
    CONST,
    IMPORT,
    IN,
    OUT,
    INOUT,
    ELSIF,
    CASE,
    WHEN,
    EXTERNAL,
    EXIT,
    NEXT,
    LOOP,
    REPEAT,
    UNTIL,
    DECLARE,
    EXCEPTION,
    TRY,
    RAISE,
    POINTER,
    NEW,
    NIL,
    VALID,
    ARROW,
    SUBBEGIN,
    SUBFMT,
    SUBEND,
    LET,
    FIRST,
    LAST,
    AS,
    DEFAULT,
    EXPORT,
    PRIVATE,
    NATIVE,
    MAX_TOKEN
};

class Token {
public:
    Token(): source(), line(0), column(0), type(NONE), text(), value() {}
    Token(const std::string &text): source(), line(), column(), type(NONE), text(text), value() {}
    std::string source;
    int line;
    int column;
    TokenType type;
    std::string text;
    Number value;

    std::string tostring() const;
};

#endif
