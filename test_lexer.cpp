#include <assert.h>
#include <iostream>

#include "lexer.h"

static std::vector<Token> dump(const std::vector<Token> &tokens)
{
    for (auto t: tokens) {
        std::cout << t.tostring() << "\n";
    }
    return tokens;
}

int main()
{
    auto tokens = dump(tokenize("1 a ( ) := + - * / ,"));
    assert(tokens.size() == 11);
    assert(tokens[0].type == NUMBER);
    assert(tokens[0].value == 1);
    assert(tokens[1].type == IDENTIFIER);
    assert(tokens[1].text == "a");
    assert(tokens[2].type == LPAREN);
    assert(tokens[3].type == RPAREN);
    assert(tokens[4].type == ASSIGN);
    assert(tokens[5].type == PLUS);
    assert(tokens[6].type == MINUS);
    assert(tokens[7].type == TIMES);
    assert(tokens[8].type == DIVIDE);
    assert(tokens[9].type == COMMA);
    assert(tokens[10].type == END_OF_FILE);
}
