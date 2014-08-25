#include <assert.h>
#include <iostream>

#include "ast.h"
#include "compiler.h"
#include "disassembler.h"
#include "exec.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"

static std::vector<Token> dump(const std::vector<Token> &tokens)
{
    for (auto t: tokens) {
        std::cout << t.tostring() << "\n";
    }
    return tokens;
}

static const Program *dump(const Program *program)
{
    program->dump();
    return program;
}

int main(int /*argc*/, char */*argv*/[])
{
    {
        std::string source = "a := 5";

        std::vector<Token> tokens = tokenize(source);

        dump(tokens);

        assert(tokens.size() == 4);
        assert(tokens[0].type == IDENTIFIER);
        assert(tokens[0].text == "a");
        assert(tokens[1].type == ASSIGN);
        assert(tokens[2].type == NUMBER);
        assert(tokens[2].value == 5);
        assert(tokens[3].type == END_OF_FILE);

        const Program *ast = parse(tokens);

        ast->dump();
    }

    {
        auto *ast = dump(parse(dump(tokenize("b := 4\na := abs(-5 * (3 + b))\nprint(a)"))));
        interpret(ast, std::cout);

        auto obj = compile(ast);

        disassemble(obj);

        exec(obj);
    }

    {
        auto obj = compile(parse(tokenize("a := 1\nIF a THEN print(1) END")));
        exec(obj);
    }
}
