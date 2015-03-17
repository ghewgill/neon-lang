#ifndef UTIL_H
#define UTIL_H

#include <string>

#include "token.h"

class CompilerError {
public:
    CompilerError(const std::string &file, int line, const std::string &message): file(file), line(line), message(message) {}
    virtual ~CompilerError() {}
    virtual void write(std::ostream &out) = 0;
    const std::string file;
    const int line;
    const std::string message;
private:
    CompilerError &operator=(const CompilerError &);
};

class InternalError: public CompilerError {
public:
    InternalError(const std::string &file, int line, const std::string &message): CompilerError(file, line, message) {}
    virtual void write(std::ostream &out) override;
};

class SourceError: public CompilerError {
public:
    SourceError(const std::string &file, int line, int number, const Token &token, const std::string &message): CompilerError(file, line, message), number(number), token(token), token2() {}
    SourceError(const std::string &file, int line, int number, const Token &token, const Token &token2, const std::string &message): CompilerError(file, line, message), number(number), token(token), token2(token2) {}
    virtual void write(std::ostream &out) override;
    const int number;
    const Token token;
    const Token token2;
};

#define error(number, token, message) abort_error(__FILE__, __LINE__, number, token, message)
#define error2(number, token, token2, message) abort_error(__FILE__, __LINE__, number, token, token2, message)
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
void abort_error(const char *file, int line, int number, const Token &token, const Token &token2, const std::string &message)
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
