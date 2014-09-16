#include <assert.h>
#include <iostream>

#include "ast.h"
#include "compiler.h"
#include "disassembler.h"
#include "exec.h"
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

int main(int /*argc*/, char * /*argv*/[])
{
    {
        std::string source = "VAR a: number a := 5";

        std::vector<Token> tokens = tokenize(source);

        dump(tokens);

        assert(tokens.size() == 8);
        assert(tokens[0].type == VAR);
        assert(tokens[1].type == IDENTIFIER);
        assert(tokens[1].text == "a");
        assert(tokens[2].type == COLON);
        assert(tokens[3].type == IDENTIFIER);
        assert(tokens[3].text == "number");
        assert(tokens[4].type == IDENTIFIER);
        assert(tokens[4].text == "a");
        assert(tokens[5].type == ASSIGN);
        assert(tokens[6].type == NUMBER);
        assert(tokens[6].value == 5);
        assert(tokens[7].type == END_OF_FILE);

        const Program *ast = parse(tokens);

        ast->dump();
    }

    {
        auto t = tokenize("VAR a: number a := 42 print(str(a))");
        auto p = parse(t);
        dump(p);
        auto c = compile(p);
        disassemble(c);
        exec(c);
    }

    {
        auto *ast = dump(parse(dump(tokenize("VAR a: number VAR b: number b := 4\na := abs(-5 * (3 + b))\nprint(str(a))"))));

        auto obj = compile(ast);

        disassemble(obj);

        exec(obj);
    }

    {
        auto obj = compile(parse(tokenize("VAR a: number a := 1\nIF a THEN print(\"1\") END")));
        exec(obj);
    }

    {
        auto obj = compile(parse(tokenize("VAR a: number a := 5\nWHILE a DO print(str(a)) a := a - 1 END")));
        disassemble(obj);
        exec(obj);
    }

    {
        auto obj = compile(parse(tokenize("VAR a: string a := \"hello world\" print(a)")));
        disassemble(obj);
        exec(obj);
    }

    {
        auto obj = compile(parse(tokenize("VAR a: string VAR b: string a := \"hello \" b := \"world\" print(a + b)")));
        disassemble(obj);
        exec(obj);
    }
}
