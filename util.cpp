#include "util.h"

#include <stdlib.h>

#include "lexer.h"

void abort_error(const char *file, int line, const Token &token, const std::string &message)
{
    fprintf(stderr, "%s\n", token.source.c_str());
    fprintf(stderr, "%*s\n", token.column, "^");
    fprintf(stderr, "%d:%d %s %s (%s:%d)\n", token.line, token.column, token.tostring().c_str(), message.c_str(), file, line);
    exit(1);
}
