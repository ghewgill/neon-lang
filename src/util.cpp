#include "util.h"

#include <iomanip>
#include <ostream>

// TODO: Currently throwing compiler exceptions by pointer in order
// to maintain alignment requirements of BID_UINT128 type on some
// platforms. See https://stackoverflow.com/questions/30885997/clang-runtime-fault-when-throwing-aligned-type-compiler-bug
// for full explanation.

void abort_error(const char *file, int line, int number, const Token &token, const std::string &message)
{
    throw new SourceError(file, line, number, token, message);
}

void abort_error_a(const char *file, int line, int number, const Token &token_before, const Token &token, const std::string &message)
{
    if (token_before.line == token.line) {
        throw new SourceError(file, line, number, token, message);
    }
    Token tok = token_before;
    tok.column = static_cast<int>(1 + tok.source.length());
    if (tok.source[tok.source.length()-1] != ' ') {
        tok.column++;
    }
    throw new SourceError(file, line, number, tok, message);
}

void abort_error(const char *file, int line, int number, const Token &token, const Token &token2, const std::string &message)
{
    throw new SourceError(file, line, number, token, token2, message);
}

void abort_internal_error(const char *file, int line, const std::string &message)
{
    throw new InternalError(file, line, message);
}

void InternalError::write(std::ostream &out)
{
    out << "Compiler Internal Error: " << message << " (" << file << ":" << line << ")\n";
}

void SourceError::write(std::ostream &out)
{
    out << token.source << "\n";
    out << std::setw(token.column) << "^" << "\n";
    out << "Error N" << number << ": " << token.line << ":" << token.column << " " << token.tostring() << " " << message << " (" << file << ":" << line << ")\n";
    if (token2.type != NONE) {
        out << token2.source << "\n";
        out << std::setw(token2.column) << "^" << "\n";
        out << "Error N" << number << ": " << token2.line << ":" << token2.column << " " << token2.tostring() << "\n";
    }
}
