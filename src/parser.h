#ifndef PARSE_H
#define PARSE_H

#include <memory>
#include <vector>

#include "lexer.h"
#include "pt.h"

std::unique_ptr<pt::Program> parse(const TokenizedSource &tokens);
std::unique_ptr<pt::Expression> parseExpression(const TokenizedSource &tokens);

#endif
