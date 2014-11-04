#include "lexer.h"

#include <algorithm>
#include <iso646.h>
#include <sstream>

#include <utf8.h>

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
        case LBRACKET:    s << "LBRACKET"; break;
        case RBRACKET:    s << "RBRACKET"; break;
        case COLON:       s << "COLON"; break;
        case ASSIGN:      s << "ASSIGN"; break;
        case PLUS:        s << "PLUS"; break;
        case MINUS:       s << "MINUS"; break;
        case TIMES:       s << "TIMES"; break;
        case DIVIDE:      s << "DIVIDE"; break;
        case MOD:         s << "MOD"; break;
        case EXP:         s << "EXP"; break;
        case EQUAL:       s << "EQUAL"; break;
        case NOTEQUAL:    s << "NOTEQUAL"; break;
        case LESS:        s << "LESS"; break;
        case GREATER:     s << "GREATER"; break;
        case LESSEQ:      s << "LESSEQ"; break;
        case GREATEREQ:   s << "GREATEREQ"; break;
        case COMMA:       s << "COMMA"; break;
        case IF:          s << "IF"; break;
        case THEN:        s << "THEN"; break;
        case ELSE:        s << "ELSE"; break;
        case END:         s << "END"; break;
        case WHILE:       s << "WHILE"; break;
        case DO:          s << "DO"; break;
        case VAR:         s << "VAR"; break;
        case FUNCTION:    s << "FUNCTION"; break;
        case RETURN:      s << "RETURN"; break;
        case FALSE:       s << "FALSE"; break;
        case TRUE:        s << "TRUE"; break;
        case AND:         s << "AND"; break;
        case OR:          s << "OR"; break;
        case NOT:         s << "NOT"; break;
        case ARRAY:       s << "ARRAY"; break;
        case DICTIONARY:  s << "DICTIONARY"; break;
        case DOT:         s << "DOT"; break;
        case TYPE:        s << "TYPE"; break;
        case RECORD:      s << "RECORD"; break;
        case ENUM:        s << "ENUM"; break;
        case CONST:       s << "CONST"; break;
        case IMPORT:      s << "IMPORT"; break;
        case IN:          s << "IN"; break;
        case OUT:         s << "OUT"; break;
        case INOUT:       s << "INOUT"; break;
        case ELSIF:       s << "ELSIF"; break;
        case CASE:        s << "CASE"; break;
        case WHEN:        s << "WHEN"; break;
        case DOTDOT:      s << "DOTDOT"; break;
    }
    s << ">";
    return s.str();
}

std::vector<Token> tokenize(const std::string &source)
{
    auto inv = utf8::find_invalid(source.begin(), source.end());
    if (inv != source.end()) {
        int line = static_cast<int>(1 + std::count(source.begin(), inv, '\n'));
        auto bol = source.rfind('\n', inv-source.begin());
        if (bol != std::string::npos) {
            bol++;
        } else {
            bol = 0;
        }
        int column = static_cast<int>(1 + inv - (source.begin() + bol));
        fprintf(stderr, "%d:%d invalid utf8 data in source\n", line, column);
        exit(1);
    }
    int line = 1;
    int column = 1;
    std::vector<Token> tokens;
    std::string::size_type linestart = 0;
    std::string::size_type lineend = source.find('\n');
    std::string::size_type i = 0;
    while (i < source.length()) {
        char c = source.at(i);
        //printf("index %lu char %c\n", i, c);
        auto startindex = i;
        Token t;
        t.source = source.substr(linestart, lineend-linestart);
        t.line = line;
        t.column = column;
        t.type = NONE;
             if (c == '(') { t.type = LPAREN; i++; }
        else if (c == ')') { t.type = RPAREN; i++; }
        else if (c == '[') { t.type = LBRACKET; i++; }
        else if (c == ']') { t.type = RBRACKET; i++; }
        else if (c == '+') { t.type = PLUS; i++; }
        else if (c == '-') { t.type = MINUS; i++; }
        else if (c == '*') { t.type = TIMES; i++; }
        else if (c == '/') { t.type = DIVIDE; i++; }
        else if (c == '^') { t.type = EXP; i++; }
        else if (c == '=') { t.type = EQUAL; i++; }
        else if (c == '#') { t.type = NOTEQUAL; i++; }
        else if (c == ',') { t.type = COMMA; i++; }
        else if (c == '<') {
            if (source.at(i+1) == '=') {
                t.type = LESSEQ;
                i += 2;
            } else {
                t.type = LESS;
                i++;
            }
        } else if (c == '>') {
            if (source.at(i+1) == '=') {
                t.type = GREATEREQ;
                i += 2;
            } else {
                t.type = GREATER;
                i++;
            }
        } else if (c == '.') {
            if (source.at(i+1) == '.') {
                t.type = DOTDOT;
                i += 2;
            } else {
                t.type = DOT;
                i++;
            }
        } else if (c == ':') {
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
            else if (t.text == "ELSE") t.type = ELSE;
            else if (t.text == "END") t.type = END;
            else if (t.text == "WHILE") t.type = WHILE;
            else if (t.text == "DO") t.type = DO;
            else if (t.text == "VAR") t.type = VAR;
            else if (t.text == "FUNCTION") t.type = FUNCTION;
            else if (t.text == "RETURN") t.type = RETURN;
            else if (t.text == "FALSE") t.type = FALSE;
            else if (t.text == "TRUE") t.type = TRUE;
            else if (t.text == "MOD") t.type = MOD;
            else if (t.text == "AND") t.type = AND;
            else if (t.text == "OR") t.type = OR;
            else if (t.text == "NOT") t.type = NOT;
            else if (t.text == "Array") t.type = ARRAY;
            else if (t.text == "Dictionary") t.type = DICTIONARY;
            else if (t.text == "TYPE") t.type = TYPE;
            else if (t.text == "RECORD") t.type = RECORD;
            else if (t.text == "ENUM") t.type = ENUM;
            else if (t.text == "CONST") t.type = CONST;
            else if (t.text == "IMPORT") t.type = IMPORT;
            else if (t.text == "IN") t.type = IN;
            else if (t.text == "OUT") t.type = OUT;
            else if (t.text == "INOUT") t.type = INOUT;
            else if (t.text == "ELSIF") t.type = ELSIF;
            else if (t.text == "CASE") t.type = CASE;
            else if (t.text == "WHEN") t.type = WHEN;
            i = j;
        } else if (isdigit(c)) {
            t.type = NUMBER;
            if (c == '0' && source.at(i+1) != '.' && tolower(source.at(i+1)) != 'e' && not isdigit(source.at(i+1))) {
                c = static_cast<char>(tolower(source.at(i+1)));
                if (isalpha(c) || c == '#') {
                    long base;
                    if (c == 'b') {
                        base = 2;
                        i += 2;
                    } else if (c == 'o') {
                        base = 8;
                        i += 2;
                    } else if (c == 'x') {
                        base = 16;
                        i += 2;
                    } else if (c == '#') {
                        i += 2;
                        char *end = NULL;
                        base = strtol(source.substr(i).c_str(), &end, 10);
                        if (base < 2 || base > 36) {
                            error(t, "invalid base");
                        }
                        auto j = i + (end - source.substr(i).c_str());
                        if (source.at(j) != '#') {
                            error(t, "'#' expected");
                        }
                        i = j + 1;
                    } else {
                        error(t, "invalid base character");
                    }
                    Number value = number_from_uint32(0);
                    for (;;) {
                        c = static_cast<char>(tolower(source.at(i)));
                        if (c == '.') {
                            error(t, "non-decimal fraction not supported");
                        }
                        if (not isalnum(c)) {
                            break;
                        }
                        int d = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'z' ? c - 'a' + 10 : -1;
                        if (d < 0) {
                            error(t, "invalid digit for given base");
                        }
                        value = number_add(number_multiply(value, number_from_uint32(base)), number_from_uint32(d));
                        i++;
                    }
                    t.value = value;
                } else {
                    t.value = number_from_uint32(0);
                    i++;
                }
            } else {
                auto j = i;
                while (j < source.length() && isdigit(source.at(j))) {
                    j++;
                }
                if (j < source.length() && source.at(j) == '.' && source.at(j+1) != '.') {
                    j++;
                    while (j < source.length() && isdigit(source.at(j))) {
                        j++;
                    }
                }
                if (j < source.length() && tolower(source.at(j)) == 'e') {
                    j++;
                    if (j < source.length() && (source.at(j) == '+' || source.at(j) == '-')) {
                        j++;
                    }
                    while (j < source.length() && isdigit(source.at(j))) {
                        j++;
                    }
                }
                t.value = number_from_string(source.substr(i, j-i));
                i = j;
            }
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
            if (i+1 >= source.length()) {
                break;
            }
            if (source.at(i+1) == '|') {
                int level = 0;
                do {
                    if (i+1 >= source.length()) {
                        error(t, "Missing closing comment '|%'");
                    }
                    if (source.at(i) == '%' && source.at(i+1) == '|') {
                        level++;
                        i += 2;
                    } else if (source.at(i) == '|' && source.at(i+1) == '%') {
                        level--;
                        i += 2;
                    } else if (source.at(i) == '\n') {
                        line++;
                        column = 0;
                        linestart = i+1;
                        lineend = source.find('\n', i+1);
                        i++;
                    } else {
                        i++;
                    }
                } while (level > 0);
            } else {
                while (i < source.length() && source.at(i) != '\n') {
                    i++;
                }
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
        column += static_cast<int>(i - startindex);
    }
    Token t;
    t.line = line + 1;
    t.column = 1;
    t.type = END_OF_FILE;
    tokens.push_back(t);
    return tokens;
}
