#ifndef UTIL_H
#define UTIL_H

#include <string>

#include "lexer.h"

class CompilerError {
public:
    CompilerError(const std::string &file, int line, const std::string &message): file(file), line(line), message(message) {}
    virtual ~CompilerError() {}
    const std::string file;
    const int line;
    const std::string message;
private:
    CompilerError &operator=(const CompilerError &);
};

class InternalError: public CompilerError {
public:
    InternalError(const std::string &file, int line, const std::string &message): CompilerError(file, line, message) {}
};

class SourceError: public CompilerError {
public:
    SourceError(const std::string &file, int line, int number, const Token &token, const std::string &message): CompilerError(file, line, message), number(number), token(token) {}
    const int number;
    const Token token;
};

#define error(number, token, message) abort_error(__FILE__, __LINE__, number, token, message)
#define internal_error(message) abort_internal_error(__FILE__, __LINE__, message)

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error(const char *file, int line, int number, const Token &token, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_internal_error(const char *file, int line, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#endif
