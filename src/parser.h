#ifndef PARSE_H
#define PARSE_H

#include <memory>
#include <vector>

#include "lexer.h"
#include "pt.h"

std::unique_ptr<pt::Program> parse(const TokenizedSource &tokens);

#endif
