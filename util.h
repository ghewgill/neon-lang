#ifndef UTIL_H
#define UTIL_H

#include <string>

class Token;

#define error(token, message) abort_error(__FILE__, __LINE__, token, message)

#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error(const char *file, int line, const Token &token, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;

#endif
