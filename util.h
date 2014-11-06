#ifndef UTIL_H
#define UTIL_H

#include <string>

class Token;

#define error(number, token, message) abort_error(__FILE__, __LINE__, number, token, message)

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error(const char *file, int line, int number, const Token &token, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#endif
