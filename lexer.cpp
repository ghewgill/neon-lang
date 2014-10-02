#include "lexer.h"

#include <sstream>

#include "util.h"

std::string Token::tostring() const
{
    std::stringstream s;
    s << "<" << line << ":" << column << " ";
    switch (type) {
        case NONE:        s << "NONE"; break;
        case END_OF_FILE: s << "END_OF_FILE"; break;
        case NUMBER:      s << "NUMBER:" << number_to_string(value); break;
        case STRING:      s << "STRING:" << text; break;
        case IDENTIFIER:  s << "IDENTIFIER:" << text; break;
        case LPAREN:      s << "LPAREN"; break;
        case RPAREN:      s << "RPAREN"; break;
        case COLON:       s << "COLON"; break;
        case ASSIGN:      s << "ASSIGN"; break;
        case PLUS:        s << "PLUS"; break;
        case MINUS:       s << "MINUS"; break;
        case TIMES:       s << "TIMES"; break;
        case DIVIDE:      s << "DIVIDE"; break;
        case COMMA:       s << "COMMA"; break;
        case IF:          s << "IF"; break;
        case THEN:        s << "THEN"; break;
        case END:         s << "END"; break;
        case WHILE:       s << "WHILE"; break;
        case DO:          s << "DO"; break;
        case VAR:         s << "VAR"; break;
        case FUNCTION:    s << "FUNCTION"; break;
        case RETURN:      s << "RETURN"; break;
    }
    s << ">";
    return s.str();
}

std::vector<Token> tokenize(const std::string &source)
{
    int line = 1;
    int column = 1;
    std::vector<Token> tokens;
    std::string::size_type linestart = 0;
    std::string::size_type lineend = source.find('\n');
    std::string::size_type i = 0;
    while (i < source.length()) {
        char c = source.at(i);
        //printf("index %lu char %c\n", i, c);
        int startindex = i;
        Token t;
        t.source = source.substr(linestart, lineend-linestart);
        t.line = line;
        t.column = column;
        t.type = NONE;
             if (c == '(') { t.type = LPAREN; i++; }
        else if (c == ')') { t.type = RPAREN; i++; }
        else if (c == '+') { t.type = PLUS; i++; }
        else if (c == '-') { t.type = MINUS; i++; }
        else if (c == '*') { t.type = TIMES; i++; }
        else if (c == '/') { t.type = DIVIDE; i++; }
        else if (c == ',') { t.type = COMMA; i++; }
        else if (c == ':') {
            if (source.at(i+1) == '=') {
                t.type = ASSIGN;
                i += 2;
            } else {
                t.type = COLON;
                i++;
            }
        } else if (isalpha(c)) {
            t.type = IDENTIFIER;
            std::string::size_type j = i;
            while (j < source.length() && isalnum(source.at(j))) {
                j++;
            }
            t.text = source.substr(i, j-i);
                 if (t.text == "IF") t.type = IF;
            else if (t.text == "THEN") t.type = THEN;
            else if (t.text == "END") t.type = END;
            else if (t.text == "WHILE") t.type = WHILE;
            else if (t.text == "DO") t.type = DO;
            else if (t.text == "VAR") t.type = VAR;
            else if (t.text == "FUNCTION") t.type = FUNCTION;
            else if (t.text == "RETURN") t.type = RETURN;
            i = j;
        } else if (isdigit(c)) {
            t.type = NUMBER;
            auto j = i;
            // TODO: decimal and exponent
            while (j < source.length() && isdigit(source.at(j))) {
                j++;
            }
            t.value = number_from_string(source.substr(i, j-i));
            i = j;
        } else if (c == '"') {
            i++;
            t.type = STRING;
            t.text = "";
            while (i < source.length()) {
                c = source.at(i);
                i++;
                if (c == '"') {
                    break;
                }
                t.text.push_back(c);
            }
        } else if (c == '%') {
            while (i < source.length() && source.at(i) != '\n') {
                i++;
            }
        } else if (isspace(c)) {
            while (i < source.length() && isspace(source.at(i))) {
                if (source.at(i) == '\n') {
                    line++;
                    column = 0;
                    linestart = i+1;
                    lineend = source.find('\n', i+1);
                }
                i++;
            }
        } else {
            error(t, "Unexpected character");
        }
        if (t.type != NONE) {
            tokens.push_back(t);
        }
        column += (i - startindex);
    }
    Token t;
    t.line = line + 1;
    t.column = 1;
    t.type = END_OF_FILE;
    tokens.push_back(t);
    return tokens;
}
