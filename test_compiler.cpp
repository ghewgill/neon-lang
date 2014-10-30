#include <assert.h>
#include <iostream>

#include "compiler.h"
#include "disassembler.h"
#include "lexer.h"
#include "opcode.h"
#include "parser.h"

int main()
{
    {
        auto obj = compile(parse(tokenize("VAR a: Number a := 1")));
        disassemble(obj, std::cout);
        /*
        unsigned char expected[] = {
            0, 0,
            PUSHI, 0, 0, 0, 1,
            STOREI, 0, 0, 0, 0,
            RET,
        };
        assert(obj == std::vector<unsigned char>(expected, expected+sizeof(expected)));
        */
    }

    {
        auto obj = compile(parse(tokenize("VAR a: Number a := 0\nIF a # 0 THEN print(\"1\") END")));
        disassemble(obj, std::cout);
        /*
        unsigned char expected[] = {
            0, 0,
            PUSHI, 0, 0, 0, 0,
            PUSHI, 0, 0, 0, 0,
            STOREI,
            PUSHI, 0, 0, 0, 0,
            LOADI,
            JZ, 0, 0, 0, 33,
            PUSHI, 0, 0, 0, 1,
            PUSHI, 0, 0, 0, 0,
            CALL,
        };
        assert(obj == std::vector<unsigned char>(expected, expected+sizeof(expected)));
        */
    }
}
