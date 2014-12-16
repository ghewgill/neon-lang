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
        case LBRACE:      s << "LBRACE"; break;
        case RBRACE:      s << "RBRACE"; break;
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
        case FOR:         s << "FOR"; break;
        case TO:          s << "TO"; break;
        case STEP:        s << "STEP"; break;
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
        case EXTERNAL:    s << "EXTERNAL"; break;
        case EXIT:        s << "EXIT"; break;
        case NEXT:        s << "NEXT"; break;
        case LOOP:        s << "LOOP"; break;
        case REPEAT:      s << "REPEAT"; break;
        case UNTIL:       s << "UNTIL"; break;
        case DECLARE:     s << "DECLARE"; break;
        case EXCEPTION:   s << "EXCEPTION"; break;
        case TRY:         s << "TRY"; break;
        case RAISE:       s << "RAISE"; break;
        case POINTER:     s << "POINTER"; break;
        case NEW:         s << "NEW"; break;
        case NIL:         s << "NIL"; break;
        case VALID:       s << "VALID"; break;
        case ARROW:       s << "ARROW"; break;
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
    std::string::const_iterator linestart = source.begin();
    std::string::const_iterator lineend = std::find(source.begin(), source.end(), '\n');
    std::string::const_iterator i = source.begin();
    while (i != source.end()) {
        uint32_t c = utf8::peek_next(i, source.end());
        //printf("index %lu char %c\n", i, c);
        auto startindex = i;
        Token t;
        t.source = std::string(linestart, lineend);
        t.line = line;
        t.column = column;
        t.type = NONE;
             if (c == '(') { t.type = LPAREN; utf8::advance(i, 1, source.end()); }
        else if (c == ')') { t.type = RPAREN; utf8::advance(i, 1, source.end()); }
        else if (c == '[') { t.type = LBRACKET; utf8::advance(i, 1, source.end()); }
        else if (c == ']') { t.type = RBRACKET; utf8::advance(i, 1, source.end()); }
        else if (c == '{') { t.type = LBRACE; utf8::advance(i, 1, source.end()); }
        else if (c == '}') { t.type = RBRACE; utf8::advance(i, 1, source.end()); }
        else if (c == '+') { t.type = PLUS; utf8::advance(i, 1, source.end()); }
        else if (c == '*') { t.type = TIMES; utf8::advance(i, 1, source.end()); }
        else if (c == '/') { t.type = DIVIDE; utf8::advance(i, 1, source.end()); }
        else if (c == '^') { t.type = EXP; utf8::advance(i, 1, source.end()); }
        else if (c == '=') { t.type = EQUAL; utf8::advance(i, 1, source.end()); }
        else if (c == '#') { t.type = NOTEQUAL; utf8::advance(i, 1, source.end()); }
        else if (c == ',') { t.type = COMMA; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2260 /*'≠'*/) { t.type = NOTEQUAL; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2264 /*'≤'*/) { t.type = LESSEQ; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2265 /*'≥'*/) { t.type = GREATEREQ; utf8::advance(i, 1, source.end()); }
        else if (c == 0x00ac /*'¬'*/) { t.type = NOT; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2227 /*'∧'*/) { t.type = AND; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2228 /*'∨'*/) { t.type = OR; utf8::advance(i, 1, source.end()); }
        else if (c == '-') {
            if (i+1 != source.end() && *(i+1) == '>') {
                t.type = ARROW;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = MINUS;
                utf8::advance(i, 1, source.end());
            }
        } else if (c == '<') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = LESSEQ;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = LESS;
                utf8::advance(i, 1, source.end());
            }
        } else if (c == '>') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = GREATEREQ;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = GREATER;
                utf8::advance(i, 1, source.end());
            }
        } else if (c == '.') {
            if (i+1 != source.end() && *(i+1) == '.') {
                t.type = DOTDOT;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = DOT;
                utf8::advance(i, 1, source.end());
            }
        } else if (c == ':') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = ASSIGN;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = COLON;
                utf8::advance(i, 1, source.end());
            }
        } else if (isalpha(c)) {
            t.type = IDENTIFIER;
            auto const start = i;
            while (i != source.end() && (isalnum(*i) || *i == '_')) {
                utf8::advance(i, 1, source.end());
            }
            t.text = std::string(start, i);
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
            else if (t.text == "FOR") t.type = FOR;
            else if (t.text == "TO") t.type = TO;
            else if (t.text == "STEP") t.type = STEP;
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
            else if (t.text == "EXTERNAL") t.type = EXTERNAL;
            else if (t.text == "EXIT") t.type = EXIT;
            else if (t.text == "NEXT") t.type = NEXT;
            else if (t.text == "LOOP") t.type = LOOP;
            else if (t.text == "REPEAT") t.type = REPEAT;
            else if (t.text == "UNTIL") t.type = UNTIL;
            else if (t.text == "DECLARE") t.type = DECLARE;
            else if (t.text == "EXCEPTION") t.type = EXCEPTION;
            else if (t.text == "TRY") t.type = TRY;
            else if (t.text == "RAISE") t.type = RAISE;
            else if (t.text == "POINTER") t.type = POINTER;
            else if (t.text == "NEW") t.type = NEW;
            else if (t.text == "NIL") t.type = NIL;
            else if (t.text == "VALID") t.type = VALID;
        } else if (isdigit(c)) {
            t.type = NUMBER;
            if (c == '0' && (i+1 != source.end()) && *(i+1) != '.' && tolower(*(i+1)) != 'e' && not isdigit(*(i+1))) {
                utf8::advance(i, 1, source.end());
                c = static_cast<char>(tolower(*i));
                if (isalpha(c) || c == '#') {
                    long base;
                    if (c == 'b') {
                        base = 2;
                        utf8::advance(i, 1, source.end());
                    } else if (c == 'o') {
                        base = 8;
                        utf8::advance(i, 1, source.end());
                    } else if (c == 'x') {
                        base = 16;
                        utf8::advance(i, 1, source.end());
                    } else if (c == '#') {
                        utf8::advance(i, 1, source.end());
                        char *end = NULL;
                        base = strtol(&*i, &end, 10);
                        if (base < 2 || base > 36) {
                            error(1001, t, "invalid base");
                        }
                        utf8::advance(i, (end - &*i), source.end());
                        if (i != source.end() && *i != '#') {
                            error(1002, t, "'#' expected");
                        }
                        utf8::advance(i, 1, source.end());
                    } else {
                        error(1003, t, "invalid base character");
                    }
                    Number value = number_from_uint32(0);
                    const auto start = i;
                    while (i != source.end()) {
                        c = static_cast<char>(tolower(*i));
                        if (c == '.') {
                            error(1004, t, "non-decimal fraction not supported");
                        }
                        if (not isalnum(c)) {
                            break;
                        }
                        int d = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'z' ? c - 'a' + 10 : -1;
                        if (d < 0 || d >= base) {
                            error(1005, t, "invalid digit for given base");
                        }
                        value = number_add(number_multiply(value, number_from_uint32(base)), number_from_uint32(d));
                        utf8::advance(i, 1, source.end());
                    }
                    if (i == start) {
                        error(1008, t, "numeric constants must have at least one digit");
                    }
                    t.value = value;
                } else {
                    t.value = number_from_uint32(0);
                }
            } else {
                const auto start = i;
                while (i != source.end() && isdigit(*i)) {
                    utf8::advance(i, 1, source.end());
                }
                if (i+1 != source.end() && *i == '.' && *(i+1) != '.') {
                    utf8::advance(i, 1, source.end());
                    while (i != source.end() && isdigit(*i)) {
                        utf8::advance(i, 1, source.end());
                    }
                }
                if (i != source.end() && tolower(*i) == 'e') {
                    utf8::advance(i, 1, source.end());
                    if (i != source.end() && (*i == '+' || *i == '-')) {
                        utf8::advance(i, 1, source.end());
                    }
                    while (i != source.end() && isdigit(*i)) {
                        utf8::advance(i, 1, source.end());
                    }
                }
                t.value = number_from_string(std::string(start, i));
            }
        } else if (c == '"') {
            utf8::advance(i, 1, source.end());
            t.type = STRING;
            t.text = "";
            while (i != source.end()) {
                c = utf8::next(i, source.end());
                if (c == '"') {
                    if (i == source.end()) {
                        break;
                    }
                    auto j = i;
                    c = utf8::next(j, source.end());
                    if (c != '"') {
                        break;
                    }
                    i = j;
                }
                utf8::append(c, std::back_inserter(t.text));
            }
        } else if (c == '%') {
            if (i+1 == source.end()) {
                break;
            }
            if (*(i+1) == '|') {
                int level = 0;
                do {
                    if (i+1 == source.end()) {
                        error(1006, t, "Missing closing comment '|%'");
                    }
                    if (*i == '%' && *(i+1) == '|') {
                        level++;
                        utf8::advance(i, 2, source.end());
                    } else if (*i == '|' && *(i+1) == '%') {
                        level--;
                        utf8::advance(i, 2, source.end());
                    } else if (*i == '\n') {
                        line++;
                        column = 0;
                        linestart = i+1;
                        lineend = std::find(i+1, source.end(), '\n');
                        utf8::advance(i, 1, source.end());
                    } else {
                        utf8::advance(i, 1, source.end());
                    }
                } while (level > 0);
            } else {
                while (i != source.end() && *i != '\n') {
                    utf8::advance(i, 1, source.end());
                }
            }
        } else if (isspace(c)) {
            while (i != source.end() && isspace(*i)) {
                if (*i == '\n') {
                    line++;
                    column = 0;
                    linestart = i+1;
                    lineend = std::find(i+1, source.end(), '\n');
                }
                utf8::advance(i, 1, source.end());
            }
        } else {
            error(1007, t, "Unexpected character");
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
