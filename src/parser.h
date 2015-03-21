#ifndef PARSE_H
#define PARSE_H

#include <vector>

#include "lexer.h"

namespace pt { class Program; }

const pt::Program *parse(const TokenizedSource &tokens);

#endif
