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
    CONSTANT,
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
    FOREACH,
    OF,
    INDEX,
    ASSERT,
    EMBED,
    ALIAS,
    IS,
    BEGIN,
    MAIN,
    HEXBYTES,
    INC,
    DEC,
    UNDERSCORE,
    OTHERS,
    WITH,
    CHECK,
    GIVES,
    NOWHERE,
    INTDIV,
    MAX_TOKEN
};

class Token {
public:
    Token(): file(), source(), line(0), column(0), type(NONE), text(), value() {}
    explicit Token(const std::string &text): file(), source(), line(0), column(0), type(NONE), text(text), value() {}
    std::string file;
    std::string source;
    int line;
    size_t column;
    TokenType type;
    std::string text;
    Number value;

    std::string tostring() const;
};

#endif
