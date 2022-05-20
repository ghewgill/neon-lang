#ifndef TOKEN_H
#define TOKEN_H

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
    EXEC,
    LABEL,
    CLASS,
    TRAP,
    EXTENSION,
    INTERFACE,
    IMPLEMENTS,
    UNUSED,
    ISA,
    ELLIPSIS,
    OPTIONAL,
    IMPORTED,
    TESTCASE,
    EXPECT,
    CHOICE,
    PROCESS,
    SUCCESS,
    FAILURE,
    PANIC,
    DEBUG,
    UNKNOWN,
    MAX_TOKEN
};

class TokenizedSource;

class Token {
public:
    explicit Token(const TokenizedSource *source = nullptr): source(source), line(0), column(0), type(NONE), text(), value() {}
    explicit Token(const std::string &text): source(nullptr), line(0), column(0), type(NONE), text(text), value() {}
    Token(TokenType type, const std::string &text): source(nullptr), line(0), column(0), type(type), text(text), value() {}
    const TokenizedSource *source;
    int line;
    size_t column;
    TokenType type;
    std::string text;
    std::vector<int> text_source_offset;
    Number value;

    std::string file() const;
    std::string source_line() const;
    std::string tostring() const;
};

#endif
