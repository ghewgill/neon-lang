#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>

#include "lexer.h"
#include "util.h"

static TokenizedSource dump(const TokenizedSource &tokens)
{
    for (auto t: tokens.tokens) {
        std::cout << t.tostring() << "\n";
    }
    return tokens;
}

void exhaustive_test(int depth)
{
    char buf[8] = {}; // all zeros
    for (;;) {
        int i = depth - 1;
        while (i >= 0) {
            buf[i]++;
            if (buf[i] != 0) {
                break;
            }
            i--;
        }
        if (i < 0) {
            break;
        }
        try {
            tokenize("", buf);
        } catch (SourceError *) {
            // ignore
        } catch (...) {
            fprintf(stderr, "fail: %s\n", buf);
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        auto tokens = dump(*tokenize("", "1 a ( ) := + - * / , IF THEN END \"a\"")).tokens;
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

        tokens = dump(*tokenize("", "a 1 # foo")).tokens;
        assert(tokens.size() == 3);
        assert(tokens[0].type == IDENTIFIER);
        assert(tokens[1].type == NUMBER);
        assert(tokens[2].type == END_OF_FILE);

        tokens = dump(*tokenize("", "a 1 #| foo bar #| nest |# |# baz")).tokens;
        assert(tokens.size() == 4);
        assert(tokens[0].type == IDENTIFIER);
        assert(tokens[1].type == NUMBER);
        assert(tokens[2].type == IDENTIFIER);
        assert(tokens[3].type == END_OF_FILE);

        tokens = dump(*tokenize("", "\"string \\(expr) foo \\(bar(baz)) \\(quux:4)\"")).tokens;
        //                      1 23456789 012345678901 234567890123 456789012
        assert(tokens.size() == 19);
        assert(tokens[0].type == STRING);       assert(tokens[0].column == 1);      assert(tokens[0].text == "string ");
        assert(tokens[1].type == SUBBEGIN);     assert(tokens[1].column == 10);
        assert(tokens[2].type == IDENTIFIER);   assert(tokens[2].column == 11);     assert(tokens[2].text == "expr");
        assert(tokens[3].type == SUBEND);       assert(tokens[3].column == 15);
        assert(tokens[4].type == STRING);       assert(tokens[4].column == 16);     assert(tokens[4].text == " foo ");
        assert(tokens[5].type == SUBBEGIN);     assert(tokens[5].column == 22);
        assert(tokens[6].type == IDENTIFIER);   assert(tokens[6].column == 23);     assert(tokens[6].text == "bar");
        assert(tokens[7].type == LPAREN);       assert(tokens[7].column == 26);
        assert(tokens[8].type == IDENTIFIER);   assert(tokens[8].column == 27);     assert(tokens[8].text == "baz");
        assert(tokens[9].type == RPAREN);       assert(tokens[9].column == 30);
        assert(tokens[10].type == SUBEND);      assert(tokens[10].column == 31);
        assert(tokens[11].type == STRING);      assert(tokens[11].column == 32);    assert(tokens[11].text == " ");
        assert(tokens[12].type == SUBBEGIN);    assert(tokens[12].column == 34);
        assert(tokens[13].type == IDENTIFIER);  assert(tokens[13].column == 35);    assert(tokens[13].text == "quux");
        assert(tokens[14].type == SUBFMT);      assert(tokens[14].column == 39);
        assert(tokens[15].type == STRING);      assert(tokens[15].column == 40);    assert(tokens[15].text == "4");
        assert(tokens[16].type == SUBEND);      assert(tokens[16].column == 41);
        assert(tokens[17].type == STRING);      assert(tokens[17].column == 42);    assert(tokens[17].text == "");
        assert(tokens[18].type == END_OF_FILE);

        tokens = dump(*tokenize("", "IF x THEN\n#|\n|#END LOOP")).tokens;
        assert(tokens.size() == 6);
        assert(tokens[0].type == IF);           assert(tokens[0].column == 1);
        assert(tokens[1].type == IDENTIFIER);   assert(tokens[1].column == 4);
        assert(tokens[2].type == THEN);         assert(tokens[2].column == 6);
        assert(tokens[3].type == END);          assert(tokens[3].column == 3);
        assert(tokens[4].type == LOOP);         assert(tokens[4].column == 7);
        assert(tokens[5].type == END_OF_FILE);

        tokens = dump(*tokenize("", "a\n\nb")).tokens;
        assert(tokens.size() == 3);
        assert(tokens[0].type == IDENTIFIER);
        assert(tokens[0].text == "a");
        assert(tokens[0].line == 1);
        assert(tokens[0].column == 1);
        assert(tokens[1].type == IDENTIFIER);
        assert(tokens[1].text == "b");
        assert(tokens[1].line == 3);
        assert(tokens[1].column == 1);
        assert(tokens[2].type == END_OF_FILE);

        {
            auto tokenized = tokenize("", "#!/bin/neon\na\n");
            dump(*tokenized);
            //for (size_t i = 0; i < tokenized->source_lines.size(); i++) {
            //    printf("%lu: %lu %lu\n", i, tokenized->source_lines[i].first, tokenized->source_lines[i].second);
            //}
            auto &tt = tokenized->tokens;
            assert(tt.size() == 2);
            assert(tt[0].type == IDENTIFIER); assert(tt[0].line == 2); assert(tt[0].column == 1); assert(tt[0].text == "a");
            assert(tt[0].source->source_lines[2].first == 1); assert(tt[0].source->source_lines[2].second == 1);
            assert(tt[0].source_line() == "a");
            assert(tt[1].type == END_OF_FILE);
        }
    }

    exhaustive_test(2);

    int a = 1;
    while (a < argc) {
        if (strcmp(argv[a], "-depth") == 0) {
            a++;
            int depth = atoi(argv[a]);
            assert(depth > 0);
            assert(depth < 8);
            exhaustive_test(depth);
        } else {
            std::fstream inf(argv[a]);
            assert(inf.good());
            std::stringstream ss;
            ss << inf.rdbuf();
            try {
                dump(*tokenize(argv[a], ss.str()));
            } catch (SourceError *e) {
                e->write(std::cout);
            }
        }
        a++;
    }
}
