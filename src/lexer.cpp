#include "lexer.h"

#include <algorithm>
#include <iso646.h>
#include <sstream>

#include <sha256.h>
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
        case CONCAT:      s << "CONCAT"; break;
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
        case SUBBEGIN:    s << "SUBBEGIN"; break;
        case SUBFMT:      s << "SUBFMT"; break;
        case SUBEND:      s << "SUBEND"; break;
        case LET:         s << "LET"; break;
        case FIRST:       s << "FIRST"; break;
        case LAST:        s << "LAST"; break;
        case AS:          s << "AS"; break;
        case DEFAULT:     s << "DEFAULT"; break;
        case EXPORT:      s << "EXPORT"; break;
        case PRIVATE:     s << "PRIVATE"; break;
        case NATIVE:      s << "NATIVE"; break;
        case FOREACH:     s << "FOREACH"; break;
        case OF:          s << "OF"; break;
        case INDEX:       s << "INDEX"; break;
        case ASSERT:      s << "ASSERT"; break;
        case MAX_TOKEN:   s << "MAX_TOKEN"; break;
    }
    s << ">";
    return s.str();
}

inline bool identifier_start(uint32_t c)
{
    return c < 256 && isalpha(c);
}

inline bool identifier_body(uint32_t c)
{
    return c < 256 && (isalnum(c) || c == '_');
}

inline bool number_start(uint32_t c)
{
    return c >= '0' && c <= '9';
}

inline bool number_body(uint32_t c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline bool number_base(uint32_t c)
{
    return c == '#' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline bool number_decimal_body(uint32_t c)
{
    return c >= '0' && c <= '9';
}

inline bool space(uint32_t c)
{
    return c < 256 && isspace(c);
}

static std::vector<Token> tokenize_fragment(int &line, int column, const std::string &source, const std::string &actual_source_line = std::string())
{
    std::vector<Token> tokens;
    std::string::const_iterator linestart = source.begin();
    std::string::const_iterator lineend = std::find(source.begin(), source.end(), '\n');
    std::string::const_iterator i = source.begin();
    while (i != source.end()) {
        uint32_t c = utf8::peek_next(i, source.end());
        //printf("index %lu char %c\n", i, c);
        auto startindex = i;
        Token t;
        t.source = actual_source_line.empty() ? std::string(linestart, lineend) : actual_source_line;
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
        else if (c == '&') { t.type = CONCAT; utf8::advance(i, 1, source.end()); }
        else if (c == '=') { t.type = EQUAL; utf8::advance(i, 1, source.end()); }
        else if (c == '#') { t.type = NOTEQUAL; utf8::advance(i, 1, source.end()); }
        else if (c == ',') { t.type = COMMA; utf8::advance(i, 1, source.end()); }
        else if (c == '.') { t.type = DOT; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2212 /*'−'*/) { t.type = MINUS; utf8::advance(i, 1, source.end()); }
        else if (c == 0x00D7 /*'×'*/) { t.type = TIMES; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2215 /*'∕'*/) { t.type = DIVIDE; utf8::advance(i, 1, source.end()); }
        else if (c == 0x00F7 /*'÷'*/) { t.type = DIVIDE; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2260 /*'≠'*/) { t.type = NOTEQUAL; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2264 /*'≤'*/) { t.type = LESSEQ; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2265 /*'≥'*/) { t.type = GREATEREQ; utf8::advance(i, 1, source.end()); }
        else if (c == 0x00ac /*'¬'*/) { t.type = NOT; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2227 /*'∧'*/) { t.type = AND; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2228 /*'∨'*/) { t.type = OR; utf8::advance(i, 1, source.end()); }
        else if (c == 0x2208 /*'∈'*/) { t.type = IN; utf8::advance(i, 1, source.end()); }
        // TODO else if (c == 0x2209 /*'∉'*/) { t.type = NOTIN; utf8::advance(i, 1, source.end()); }
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
        } else if (c == ':') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = ASSIGN;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = COLON;
                utf8::advance(i, 1, source.end());
            }
        } else if (identifier_start(c)) {
            t.type = IDENTIFIER;
            auto const start = i;
            while (i != source.end() && identifier_body(*i)) {
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
            else if (t.text == "LET") t.type = LET;
            else if (t.text == "FIRST") t.type = FIRST;
            else if (t.text == "LAST") t.type = LAST;
            else if (t.text == "AS") t.type = AS;
            else if (t.text == "DEFAULT") t.type = DEFAULT;
            else if (t.text == "EXPORT") t.type = EXPORT;
            else if (t.text == "PRIVATE") t.type = PRIVATE;
            else if (t.text == "NATIVE") t.type = NATIVE;
            else if (t.text == "FOREACH") t.type = FOREACH;
            else if (t.text == "OF") t.type = OF;
            else if (t.text == "INDEX") t.type = INDEX;
            else if (t.text == "ASSERT") t.type = ASSERT;
        } else if (number_start(c)) {
            t.type = NUMBER;
            if (c == '0' && (i+1 != source.end()) && *(i+1) != '.' && tolower(*(i+1)) != 'e' && not number_decimal_body(*(i+1))) {
                utf8::advance(i, 1, source.end());
                c = static_cast<char>(tolower(*i));
                if (number_base(c)) {
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
                        char *end = nullptr;
                        base = strtol(&*i, &end, 10);
                        if (base < 2 || base > 36) {
                            error(1001, t, "invalid base");
                        }
                        utf8::advance(i, (end - &*i), source.end());
                        if (i != source.end()) {
                            if (*i != '#') {
                                error(1002, t, "'#' expected");
                            }
                            utf8::advance(i, 1, source.end());
                        }
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
                        if (not number_body(c)) {
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
                while (i != source.end() && number_decimal_body(*i)) {
                    utf8::advance(i, 1, source.end());
                }
                if (i != source.end() && *i == '.') {
                    utf8::advance(i, 1, source.end());
                    while (i != source.end() && number_decimal_body(*i)) {
                        utf8::advance(i, 1, source.end());
                    }
                }
                if (i != source.end() && tolower(*i) == 'e') {
                    utf8::advance(i, 1, source.end());
                    if (i != source.end() && (*i == '+' || *i == '-')) {
                        utf8::advance(i, 1, source.end());
                    }
                    while (i != source.end() && number_decimal_body(*i)) {
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
                    break;
                }
                if (i == source.end()) {
                    error(1010, t, "unterminated string");
                }
                if (c == '\\') {
                    c = utf8::next(i, source.end());
                    if (i == source.end()) {
                        error(1011, t, "unterminated string");
                    }
                    switch (c) {
                        case '"': break;
                        case '\\': break;
                        case 'b': c = '\b'; break;
                        case 'f': c = '\f'; break;
                        case 'n': c = '\n'; break;
                        case 'r': c = '\r'; break;
                        case 't': c = '\t'; break;
                        case 'u':
                            if (source.end() - i < 4) {
                                error(1012, t, "unterminated string");
                            }
                            for (int j = 0; j < 4; j++) {
                                if (not isxdigit(i[j])) {
                                    error(1013, t, "invalid hex character");
                                }
                            }
                            c = std::stoul(std::string(i, i+4), nullptr, 16);
                            utf8::advance(i, 4, source.end());
                            break;
                        case '(': {
                            tokens.push_back(t);
                            t.column = column + static_cast<int>(i - startindex) - 1;
                            t.type = SUBBEGIN;
                            tokens.push_back(t);
                            auto start = i;
                            auto colon = start;
                            int nest = 1;
                            while (nest > 0) {
                                if (i == source.end()) {
                                    error(1015, t, "unexpected end of file");
                                }
                                c = utf8::next(i, source.end());
                                switch (c) {
                                    case '(':
                                        nest++;
                                        break;
                                    case ')':
                                        nest--;
                                        break;
                                    case ':':
                                        if (nest == 1) {
                                            colon = i - 1;
                                        }
                                        break;
                                    case '"':
                                    case '\\':
                                    case '\n':
                                        error(1014, t, "invalid char embedded in string substitution");
                                }
                            }
                            auto end = i - 1;
                            if (colon > start) {
                                end = colon;
                            }
                            int col = column + static_cast<int>(start - startindex);
                            auto subtokens = tokenize_fragment(line, col, std::string(start, end), t.source);
                            std::copy(subtokens.begin(), subtokens.end(), std::back_inserter(tokens));
                            if (colon > start) {
                                t.column = column + static_cast<int>(colon - startindex);
                                t.type = SUBFMT;
                                tokens.push_back(t);
                                t.column += 1;
                                t.type = STRING;
                                t.text = std::string(colon + 1, i - 1);
                                tokens.push_back(t);
                            }
                            t.column = column + static_cast<int>(i - startindex) - 1;
                            t.type = SUBEND;
                            tokens.push_back(t);
                            t.column = column + static_cast<int>(i - startindex);
                            t.type = STRING;
                            t.text = "";
                            continue;
                        }
                        default:
                            error(1009, t, "invalid escape character");
                    }
                }
                utf8::append(c, std::back_inserter(t.text));
            }
        } else if (c == '@') {
            utf8::advance(i, 1, source.end());
            t.type = STRING;
            t.text = "";
            if (i == source.end()) {
                error(1016, t, "unterminated raw string");
            }
            c = utf8::peek_next(i, source.end());
            if (c == '"') {
                utf8::advance(i, 1, source.end());
                while (i != source.end()) {
                    c = utf8::next(i, source.end());
                    if (c == '"') {
                        break;
                    }
                    if (i == source.end()) {
                        error(1017, t, "unterminated raw string");
                    }
                    utf8::append(c, std::back_inserter(t.text));
                }
            } else {
                std::string delimiter;
                while (i != source.end()) {
                    c = utf8::next(i, source.end());
                    if (c == '@') {
                        break;
                    }
                    if (i == source.end()) {
                        error(1018, t, "unterminated delimiter");
                    }
                    utf8::append(c, std::back_inserter(delimiter));
                }
                if (i == source.end() || utf8::next(i, source.end()) != '"') {
                    error(1019, t, "'\"' expected");
                }
                delimiter = "\"@" + delimiter + "@";
                auto d = delimiter.begin();
                while (i != source.end()) {
                    c = utf8::next(i, source.end());
                    if (c == utf8::next(d, delimiter.end())) {
                        if (d == delimiter.end()) {
                            auto j = t.text.end();
                            for (std::string::size_type trunc = delimiter.length() - 1; trunc > 0; trunc--) {
                                utf8::prior(j, t.text.begin());
                            }
                            t.text = std::string(t.text.begin(), j);
                            break;
                        }
                    } else {
                        d = delimiter.begin();
                        if (c == '"') {
                            utf8::advance(d, 1, delimiter.end());
                        }
                    }
                    if (i == source.end()) {
                        error(1020, t, "unterminated raw string");
                    }
                    utf8::append(c, std::back_inserter(t.text));
                }
            }
        } else if (c == '%') {
            if (i+1 == source.end()) {
                break;
            }
            if (*(i+1) == '|') {
                int level = 0;
                do {
                    if (i == source.end() || i+1 == source.end()) {
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
        } else if (space(c)) {
            while (i != source.end() && space(*i)) {
                if (*i == '\n') {
                    line++;
                    startindex = i+1;
                    column = 1;
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
    return tokens;
}

TokenizedSource tokenize(const std::string &source_path, const std::string &source)
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
        Token t;
        t.line = line;
        t.column = column;
        error(1000, t, "invalid UTF-8 data in source");
    }
    int line = 1;
    std::string::const_iterator i = source.begin();
    if (source.substr(0, 2) == "#!") {
        while (i != source.end() && *i != '\n') {
            utf8::advance(i, 1, source.end());
        }
        line++;
    }
    TokenizedSource r;
    SHA256 sha256;
    sha256(source);
    unsigned char h[SHA256::HashBytes];
    sha256.getHash(h);
    r.source_path = source_path;
    r.source_hash = std::string(h, h+sizeof(h));
    r.tokens = tokenize_fragment(line, 1, std::string(i, source.end()));
    Token t;
    t.line = line;
    t.column = 1;
    t.type = END_OF_FILE;
    r.tokens.push_back(t);
    return r;
}
