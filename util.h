#ifndef UTIL_H
#define UTIL_H

#include <string>

class Token;

#define error(token, message) abort_error(__FILE__, __LINE__, token, message)

void abort_error(const char *file, int line, const Token &token, const std::string &message) __attribute__((noreturn));

#endif
