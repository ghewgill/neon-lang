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
    auto tokens = dump(tokenize("1 a ( ) := + - * / , IF THEN END \"a\""));
    assert(tokens.size() == 15);
    assert(tokens[0].type == NUMBER);
    assert(number_is_equal(tokens[0].value, number_from_uint32(1)));
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
    assert(tokens[10].type == IF);
    assert(tokens[11].type == THEN);
    assert(tokens[12].type == END);
    assert(tokens[13].type == STRING);
    assert(tokens[13].text == "a");
    assert(tokens[14].type == END_OF_FILE);

    tokens = dump(tokenize("a 1 % foo"));
    assert(tokens.size() == 3);
    assert(tokens[0].type == IDENTIFIER);
    assert(tokens[1].type == NUMBER);
    assert(tokens[2].type == END_OF_FILE);

    tokens = dump(tokenize("a 1 %| foo bar %| nest |% |% baz"));
    assert(tokens.size() == 4);
    assert(tokens[0].type == IDENTIFIER);
    assert(tokens[1].type == NUMBER);
    assert(tokens[2].type == IDENTIFIER);
    assert(tokens[3].type == END_OF_FILE);
}
