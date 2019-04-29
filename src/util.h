#ifndef UTIL_H
#define UTIL_H

#include <string>

#include "token.h"

class CompilerError {
public:
    CompilerError(const std::string &compiler_file, int compiler_line, const std::string &message): compiler_file(compiler_file), compiler_line(compiler_line), message(message) {}
    virtual ~CompilerError() {}
    virtual void write(std::ostream &out) = 0;
    virtual void write_json(std::ostream &out) = 0;
    const std::string compiler_file;
    const int compiler_line;
    const std::string message;
private:
    CompilerError &operator=(const CompilerError &);
};

class InternalError: public CompilerError {
public:
    InternalError(const std::string &compiler_file, int compiler_line, const std::string &message): CompilerError(compiler_file, compiler_line, message) {}
    virtual void write(std::ostream &out) override;
    virtual void write_json(std::ostream &out) override;
};

class SourceErrorToken {
public:
    SourceErrorToken(): token(), file(), source_line(), line(0), column(0), type(NONE) {}
    explicit SourceErrorToken(const Token &token): token(token), file(token.file()), source_line(token.source_line()), line(token.line), column(token.column), type(token.type) {}
    Token token;
    std::string file;
    std::string source_line;
    int line;
    size_t column;
    TokenType type;
};

class SourceError: public CompilerError {
public:
    SourceError(const std::string &compiler_file, int compiler_line, int number, const Token &token, const std::string &message): CompilerError(compiler_file, compiler_line, message), number(number), token(token), token2(), message2() {}
    SourceError(const std::string &compiler_file, int compiler_line, int number, const Token &token, const std::string &message, const Token &token2, const std::string &message2): CompilerError(compiler_file, compiler_line, message), number(number), token(token), token2(token2), message2(message2) {}
    virtual void write(std::ostream &out) override;
    virtual void write_json(std::ostream &out) override;
    const int number;
    const SourceErrorToken token;
    const SourceErrorToken token2;
    const std::string message2;
};

#define error(number, token, message) abort_error(__FILE__, __LINE__, number, token, message)
#define error_a(number, token_before, token, message) abort_error_a(__FILE__, __LINE__, number, token_before, token, message)
#define error2(number, token, message, token2, message2) abort_error(__FILE__, __LINE__, number, token, message, token2, message2)
#define internal_error(message) abort_internal_error(__FILE__, __LINE__, message)

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error(const char *compiler_file, int compiler_line, int number, const Token &token, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error_a(const char *compiler_file, int compiler_line, int number, const Token &token_before, const Token &token, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error(const char *compiler_file, int compiler_line, int number, const Token &token, const std::string &message, const Token &token2, const std::string &message2)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_internal_error(const char *compiler_file, int compiler_line, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#endif
