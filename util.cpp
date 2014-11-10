#include "util.h"

#include <stdlib.h>

#include "lexer.h"

void abort_error(const char *file, int line, int number, const Token &token, const std::string &message)
{
    fprintf(stderr, "%s\n", token.source.c_str());
    fprintf(stderr, "%*s\n", token.column, "^");
    fprintf(stderr, "Error S%d: %d:%d %s %s (%s:%d)\n", number, token.line, token.column, token.tostring().c_str(), message.c_str(), file, line);
    exit(1);
}

void abort_internal_error(const char *file, int line, const std::string &message)
{
    fprintf(stderr, "Compiler Internal Error: %s (%s:%d)\n", message.c_str(), file, line);
    exit(1);
}
