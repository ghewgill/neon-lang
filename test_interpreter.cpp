#include <assert.h>
#include <sstream>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

int main()
{
    {
        std::stringstream out;
        interpret(parse(tokenize("a := 1\nprint(a)")), out);
        assert(out.str() == "1\n");
    }

    {
        std::stringstream out;
        interpret(parse(tokenize("a := 0\nIF a THEN print(1) END")), out);
        assert(out.str() == "");
    }

    {
        std::stringstream out;
        interpret(parse(tokenize("a := 1\nIF a THEN print(1) END")), out);
        assert(out.str() == "1\n");
    }
}
