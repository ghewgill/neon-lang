#ifndef ANALYZER_H
#define ANALYZER_H

#include <map>
#include <string>

#include "ast.h"
#include "bytecode.h"

class ICompilerSupport;
namespace ast { class Program; }
namespace pt { class Program; }

const ast::Program *analyze(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ast::ExternalGlobalInfo> *external_globals = nullptr);

#endif
