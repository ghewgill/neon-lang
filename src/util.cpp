#include "util.h"

#include <stdlib.h>

void abort_error(const char *file, int line, int number, const Token &token, const std::string &message)
{
    throw SourceError(file, line, number, token, message);
}

void abort_internal_error(const char *file, int line, const std::string &message)
{
    throw InternalError(file, line, message);
}
