#include <assert.h>

#include "compiler.h"
#include "disassembler.h"
#include "lexer.h"
#include "opcode.h"
#include "parser.h"

int main()
{
    {
        auto obj = compile(parse(tokenize("a := 1")));
        disassemble(obj);
        unsigned char expected[] = {
            0, 0,
            PUSHI, 0, 0, 0, 1,
            PUSHI, 0, 0, 0, 0,
            STOREI,
        };
        assert(obj == std::vector<unsigned char>(expected, expected+sizeof(expected)));
    }

    {
        auto obj = compile(parse(tokenize("a := 0\nIF a THEN print(1) END")));
        disassemble(obj);
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
