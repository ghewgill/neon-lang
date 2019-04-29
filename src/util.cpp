#include "util.h"

#include <iomanip>
#include <ostream>

#include "minijson_writer.hpp"

// TODO: Currently throwing compiler exceptions by pointer in order
// to maintain alignment requirements of BID_UINT128 type on some
// platforms. See https://stackoverflow.com/questions/30885997/clang-runtime-fault-when-throwing-aligned-type-compiler-bug
// for full explanation.

void abort_error(const char *compiler_file, int compiler_line, int number, const Token &token, const std::string &message)
{
    throw new SourceError(compiler_file, compiler_line, number, token, message);
}

void abort_error_a(const char *compiler_file, int compiler_line, int number, const Token &token_before, const Token &token, const std::string &message)
{
    if (token_before.line == token.line) {
        throw new SourceError(compiler_file, compiler_line, number, token, message);
    }
    Token tok = token_before;
    tok.column = static_cast<int>(1 + tok.source_line().length());
    if (tok.source_line()[tok.source_line().length()-1] != ' ') {
        tok.column++;
    }
    throw new SourceError(compiler_file, compiler_line, number, tok, message);
}

void abort_error(const char *compiler_file, int compiler_line, int number, const Token &token, const std::string &message, const Token &token2, const std::string &message2)
{
    throw new SourceError(compiler_file, compiler_line, number, token, message, token2, message2);
}

void abort_internal_error(const char *compiler_file, int compiler_line, const std::string &message)
{
    throw new InternalError(compiler_file, compiler_line, message);
}

void InternalError::write(std::ostream &out)
{
    out << "Compiler Internal Error: " << message << " (" << compiler_file << ":" << compiler_line << ")\n";
}

void InternalError::write_json(std::ostream &out)
{
    minijson::object_writer writer(out);
    writer.write("message", message);
    writer.close();
}

void SourceError::write(std::ostream &out)
{
    out << "Error in file: " << token.file << "\n";
    out << "\n";
    out << token.line << "| " << token.source_line << "\n";
    out << std::string(std::to_string(token.line).length()+1+token.column, ' ') << std::string(token.token.text.length(), '~') << "\n";
    out << std::setw(std::to_string(token.line).length()+2+token.column) << "^" << "\n";
    out << "Error N" << number << ": " << token.line << ":" << token.column << " " << message << "\n";
    if (token2.type != NONE) {
        out << "\n";
        out << "Related line information:\n";
        out << "\n";
        out << token2.line << "| " << token2.source_line << "\n";
        out << std::setw(std::to_string(token2.line).length()+2+token2.column) << "^" << "\n";
        out << "Error N" << number << ": " << token2.line << ":" << token2.column << " " << message2 << "\n";
    }
}

void SourceError::write_json(std::ostream &out)
{
    minijson::object_writer writer(out);
    writer.write("file", token.file);
    writer.write("line", token.line);
    writer.write("column", token.column);
    writer.write("error", "N" + std::to_string(number));
    writer.write("message", message);
    if (token2.type != NONE) {
        auto w2 = writer.nested_object("related");
        w2.write("file", token2.file);
        w2.write("line", token2.line);
        w2.write("column", token2.column);
    }
    writer.close();
    out << "\n";
}
