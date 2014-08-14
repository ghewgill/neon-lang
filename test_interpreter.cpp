#include <assert.h>
#include <sstream>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

int main()
{
    std::stringstream out;
    interpret(parse(tokenize("a := 1\nprint(a)")), out);
    assert(out.str() == "1\n");
}
