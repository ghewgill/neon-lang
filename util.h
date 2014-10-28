#ifndef UTIL_H
#define UTIL_H

#include <string>

class Token;

#define error(token, message) abort_error(__FILE__, __LINE__, token, message)

<<<<<<< HEAD
#ifdef _MSC_VER
__declspec(noreturn)
#endif
void abort_error(const char *file, int line, const Token &token, const std::string &message)
#ifdef __GNUC__
__attribute__((noreturn))
#endif
;
=======
#ifdef _MSC_VER 
void __declspec(noreturn) abort_error(const char *file, int line, const Token &token, const std::string &message);
#else
void abort_error(const char *file, int line, const Token &token, const std::string &message) __attribute__((noreturn));
#endif
>>>>>>> 33d411a7804696a520d71707dd6f0ac2e9a49a47

#endif

