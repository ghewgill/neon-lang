#ifndef REPL_H
#define REPL_H

#include <map>
#include <memory>
#include <vector>

#include "ast.h"
#include "exec.h"
#include "support.h"

class Cell;

class Repl {
public:
    Repl(int argc, char *argv[], bool no_prompt, bool stop_on_any_error, bool dump_listing, const ExecOptions &options);
    Repl(const Repl &) = delete;
    Repl operator=(const Repl &) = delete;
    std::string prompt() const;
    void handle(const std::string &line);
private:
    const int argc;
    char **argv;
    const bool stop_on_any_error;
    const bool dump_listing;
    const ExecOptions options;
    CompilerSupport compiler_support;
    RuntimeSupport runtime_support;
    std::map<std::string, ast::ExternalGlobalInfo> globals_ast;
    std::map<std::string, Cell *> globals_cells;
    std::vector<std::string> input;
};

#endif // REPL_H
