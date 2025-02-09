#include "lexer.h"

#include <algorithm>
#include <iso646.h>
#include <map>
#include <sstream>
#include <string.h>

#include <sha256.h>
#include <utf8.h>

#include "util.h"

#include "unicodedata.inc"

std::string TokenizedSource::source_line(int line) const
{
    if (line == 0 || static_cast<size_t>(line) >= source_lines.size()) {
        return std::string();
    }
    return std::string(source_text, source_lines[line].first, source_lines[line].second);
}

std::string Token::file() const
{
    return source != nullptr ? source->source_path : std::string();
}

std::string Token::source_line() const
{
    if (source == nullptr) {
        return std::string();
    }
    return source->source_line(line);
}

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
        case CONSTANT:    s << "CONSTANT"; break;
        case IMPORT:      s << "IMPORT"; break;
        case IN:          s << "IN"; break;
        case OUT:         s << "OUT"; break;
        case INOUT:       s << "INOUT"; break;
        case ELSIF:       s << "ELSIF"; break;
        case CASE:        s << "CASE"; break;
        case WHEN:        s << "WHEN"; break;
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
        case INDEX:       s << "INDEX"; break;
        case ASSERT:      s << "ASSERT"; break;
        case EMBED:       s << "EMBED"; break;
        case ALIAS:       s << "ALIAS"; break;
        case IS:          s << "IS"; break;
        case BEGIN:       s << "BEGIN"; break;
        case MAIN:        s << "MAIN"; break;
        case HEXBYTES:    s << "HEXBYTES"; break;
        case INC:         s << "INC"; break;
        case DEC:         s << "DEC"; break;
        case UNDERSCORE:  s << "UNDERSCORE"; break;
        case OTHERS:      s << "OTHERS"; break;
        case WITH:        s << "WITH"; break;
        case CHECK:       s << "CHECK"; break;
        case GIVES:       s << "GIVES"; break;
        case NOWHERE:     s << "NOWHERE"; break;
        case INTDIV:      s << "INTDIV"; break;
        case EXEC:        s << "EXEC"; break;
        case LABEL:       s << "LABEL"; break;
        case CLASS:       s << "CLASS"; break;
        case TRAP:        s << "TRAP"; break;
        case EXTENSION:   s << "EXTENSION"; break;
        case INTERFACE:   s << "INTERFACE"; break;
        case IMPLEMENTS:  s << "IMPLEMENTS"; break;
        case UNUSED:      s << "UNUSED"; break;
        case ISA:         s << "ISA"; break;
        case ELLIPSIS:    s << "ELLIPSIS"; break;
        case OPTIONAL:    s << "OPTIONAL"; break;
        case IMPORTED:    s << "IMPORTED"; break;
        case TESTCASE:    s << "TESTCASE"; break;
        case EXPECT:      s << "EXPECT"; break;
        case CHOICE:      s << "CHOICE"; break;
        case PROCESS:     s << "PROCESS"; break;
        case SUCCESS:     s << "SUCCESS"; break;
        case FAILURE:     s << "FAILURE"; break;
        case PANIC:       s << "PANIC"; break;
        case DEBUG:       s << "DEBUG"; break;
        case UNKNOWN:     s << "UNKNOWN"; break;
        case MAX_TOKEN:   s << "MAX_TOKEN"; break;
    }
    s << ">";
    return s.str();
}

inline bool identifier_start(uint32_t c)
{
    return c < 256 && (isalpha(c) || c == '_');
}

inline bool identifier_body(uint32_t c)
{
    return c < 256 && (isalnum(c) || c == '_');
}

inline bool all_upper(const std::string &s)
{
    for (auto c: s) {
        if (not isupper(c)) {
            return false;
        }
    }
    return true;
}

inline bool number_start(uint32_t c)
{
    return c >= '0' && c <= '9';
}

inline bool number_body(uint32_t c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

inline bool number_base(uint32_t c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline bool number_decimal_body(uint32_t c)
{
    return (c >= '0' && c <= '9') || c == '_';
}

inline bool space(uint32_t c)
{
    return c < 256 && isspace(c);
}

uint32_t unicode_lookup(Token t, size_t offset, const std::string &name)
{
    std::string uname;
    //std::transform(name.begin(), name.end(), std::back_inserter(uname), ::toupper);
    for (auto c: name) {
        uname.push_back(static_cast<char>(::toupper(c)));
    }
    size_t low = 0;
    size_t high = sizeof(UnicodeData) / sizeof(UnicodeData[0]) - 1;
    while (low <= high) {
        size_t mid = (low + high) / 2;
        int c = strcmp(uname.c_str(), UnicodeData[mid].name);
        if (c < 0) {
            high = mid - 1;
        } else if (c > 0) {
            low = mid + 1;
        } else {
            return UnicodeData[mid].value;
        }
    }
    t.column += offset;
    error(1027, t, "unicode character name not found");
}

static const std::map<std::string, TokenType> TokenNames = {
    {"IF", IF},
    {"THEN", THEN},
    {"ELSE", ELSE},
    {"END", END},
    {"WHILE", WHILE},
    {"DO", DO},
    {"VAR", VAR},
    {"FUNCTION", FUNCTION},
    {"RETURN", RETURN},
    {"FALSE", FALSE},
    {"TRUE", TRUE},
    {"MOD", MOD},
    {"AND", AND},
    {"OR", OR},
    {"NOT", NOT},
    {"FOR", FOR},
    {"TO", TO},
    {"STEP", STEP},
    {"Array", ARRAY},
    {"Dictionary", DICTIONARY},
    {"TYPE", TYPE},
    {"RECORD", RECORD},
    {"ENUM", ENUM},
    {"CONSTANT", CONSTANT},
    {"IMPORT", IMPORT},
    {"IN", IN},
    {"OUT", OUT},
    {"INOUT", INOUT},
    {"ELSIF", ELSIF},
    {"CASE", CASE},
    {"WHEN", WHEN},
    {"EXIT", EXIT},
    {"NEXT", NEXT},
    {"LOOP", LOOP},
    {"REPEAT", REPEAT},
    {"UNTIL", UNTIL},
    {"DECLARE", DECLARE},
    {"EXCEPTION", EXCEPTION},
    {"TRY", TRY},
    {"RAISE", RAISE},
    {"POINTER", POINTER},
    {"NEW", NEW},
    {"NIL", NIL},
    {"VALID", VALID},
    {"LET", LET},
    {"FIRST", FIRST},
    {"LAST", LAST},
    {"AS", AS},
    {"DEFAULT", DEFAULT},
    {"EXPORT", EXPORT},
    {"PRIVATE", PRIVATE},
    {"NATIVE", NATIVE},
    {"FOREACH", FOREACH},
    {"INDEX", INDEX},
    {"ASSERT", ASSERT},
    {"EMBED", EMBED},
    {"ALIAS", ALIAS},
    {"IS", IS},
    {"BEGIN", BEGIN},
    {"MAIN", MAIN},
    {"HEXBYTES", HEXBYTES},
    {"INC", INC},
    {"DEC", DEC},
    {"_", UNDERSCORE},
    {"OTHERS", OTHERS},
    {"WITH", WITH},
    {"CHECK", CHECK},
    {"GIVES", GIVES},
    {"NOWHERE", NOWHERE},
    {"INTDIV", INTDIV},
    {"EXEC", EXEC},
    {"LABEL", LABEL},
    {"CLASS", CLASS},
    {"TRAP", TRAP},
    {"EXTENSION", EXTENSION},
    {"INTERFACE", INTERFACE},
    {"IMPLEMENTS", IMPLEMENTS},
    {"UNUSED", UNUSED},
    {"ISA", ISA},
    {"OPTIONAL", OPTIONAL},
    {"IMPORTED", IMPORTED},
    {"TESTCASE", TESTCASE},
    {"EXPECT", EXPECT},
    {"CHOICE", CHOICE},
    {"PROCESS", PROCESS},
    {"SUCCESS", SUCCESS},
    {"FAILURE", FAILURE},
    {"PANIC", PANIC},
    {"DEBUG", DEBUG},
};

static std::vector<Token> tokenize_fragment(TokenizedSource *tsource, const std::string &source_path, int &line, size_t column, const std::string &source)
{
    std::vector<Token> tokens;
    std::string::const_iterator linestart = source.begin();
    std::string::const_iterator lineend = std::find(source.begin(), source.end(), '\n');
    std::string::const_iterator i = source.begin();
    while (i != source.end()) {
        while (static_cast<size_t>(line) >= tsource->source_lines.size()) {
            tsource->source_lines.push_back(std::make_pair(linestart - source.begin(), lineend - linestart));
        }
        uint32_t c = utf8::peek_next(i, source.end());
        //printf("index %lu char %c\n", i, c);
        auto startindex = i;
        Token t(tsource);
        t.line = line;
        t.column = column;
        t.length = 0;
        t.type = NONE;
             if (c == '(') { t.type = LPAREN; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == ')') { t.type = RPAREN; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '[') { t.type = LBRACKET; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == ']') { t.type = RBRACKET; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '{') { t.type = LBRACE; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '}') { t.type = RBRACE; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '+') { t.type = PLUS; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '*') { t.type = TIMES; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '^') { t.type = EXP; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '&') { t.type = CONCAT; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '=') { t.type = EQUAL; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == ',') { t.type = COMMA; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2212 /*'−'*/) { t.type = MINUS; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x00D7 /*'×'*/) { t.type = TIMES; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2215 /*'∕'*/) { t.type = DIVIDE; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x00F7 /*'÷'*/) { t.type = DIVIDE; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2260 /*'≠'*/) { t.type = NOTEQUAL; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2264 /*'≤'*/) { t.type = LESSEQ; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2265 /*'≥'*/) { t.type = GREATEREQ; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2254 /*'≔'*/) { t.type = ASSIGN; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x00ac /*'¬'*/) { t.type = NOT; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2227 /*'∧'*/) { t.type = AND; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2228 /*'∨'*/) { t.type = OR; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == 0x2208 /*'∈'*/) { t.type = IN; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        // TODO else if (c == 0x2209 /*'∉'*/) { t.type = NOTIN; utf8::advance(i, 1, source.end()); t.length = i - startindex; }
        else if (c == '-') {
            if (i+1 != source.end() && *(i+1) == '-') {
                while (i != source.end() && *i != '\n') {
                    utf8::advance(i, 1, source.end());
                }
            } else if (i+1 != source.end() && *(i+1) == '>') {
                t.type = ARROW;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = MINUS;
                utf8::advance(i, 1, source.end());
            }
            t.length = i - startindex;
        } else if (c == '/') {
            if (i+1 != source.end() && *(i+1) == '*') {
                i += 2;
                for (;;) {
                    if (i == source.end() || i+1 == source.end()) {
                        error(1006, t, "Missing closing comment '*/'");
                    } else if (*i == '*' && *(i+1) == '/') {
                        utf8::advance(i, 2, source.end());
                        break;
                    } else if (*i == '\n') {
                        line++;
                        column = 0;
                        linestart = i+1;
                        lineend = std::find(i+1, source.end(), '\n');
                        startindex = i;
                        utf8::advance(i, 1, source.end());
                    } else {
                        utf8::advance(i, 1, source.end());
                    }
                }
            } else {
                t.type = DIVIDE;
                utf8::advance(i, 1, source.end());
            }
            t.length = i - startindex;
        } else if (c == '<') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = LESSEQ;
                utf8::advance(i, 2, source.end());
            } else if (i+1 != source.end() && *(i+1) == '>') {
                t.type = NOTEQUAL;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = LESS;
                utf8::advance(i, 1, source.end());
            }
            t.length = i - startindex;
        } else if (c == '>') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = GREATEREQ;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = GREATER;
                utf8::advance(i, 1, source.end());
            }
            t.length = i - startindex;
        } else if (c == ':') {
            if (i+1 != source.end() && *(i+1) == '=') {
                t.type = ASSIGN;
                utf8::advance(i, 2, source.end());
            } else {
                t.type = COLON;
                utf8::advance(i, 1, source.end());
            }
            t.length = i - startindex;
        } else if (c == '.') {
            if (i+1 != source.end() && *(i+1) == '.' && i+2 != source.end() && *(i+2) == '.') {
                t.type = ELLIPSIS;
                utf8::advance(i, 3, source.end());
            } else {
                t.type = DOT;
                utf8::advance(i, 1, source.end());
            }
            t.length = i - startindex;
        } else if (identifier_start(c)) {
            t.type = IDENTIFIER;
            auto const start = i;
            while (i != source.end() && identifier_body(*i)) {
                utf8::advance(i, 1, source.end());
            }
            t.text = std::string(start, i);
            auto i = TokenNames.find(t.text);
            if (i != TokenNames.end()) {
                t.type = i->second;
            } else if (all_upper(t.text)) {
                t.type = UNKNOWN;
            } else if (t.text.find("__") != std::string::npos) {
                error(1024, t, "identifier cannot contain double underscore (reserved)");
            } else if (t.text.length() >= 2 && t.text[0] == '_') {
                error(1025, t, "identifier cannot start with underscore");
            }
            t.length = t.text.length();
        } else if (number_start(c)) {
            t.type = NUMBER;
            auto start = i;
            if (c == '0' && (i+1 != source.end()) && *(i+1) != '.' && tolower(*(i+1)) != 'e' && not number_decimal_body(*(i+1))) {
                utf8::advance(i, 1, source.end());
                c = static_cast<char>(tolower(*i));
                if (number_base(c)) {
                    long base = 0;
                    if (c == 'b') {
                        base = 2;
                        utf8::advance(i, 1, source.end());
                    } else if (c == 'o') {
                        base = 8;
                        utf8::advance(i, 1, source.end());
                    } else if (c == 'x') {
                        base = 16;
                        utf8::advance(i, 1, source.end());
                    } else {
                        t.column += 1;
                        error(1003, t, "invalid base character");
                    }
                    Number value = number_from_uint32(0);
                    const auto nstart = i;
                    while (i != source.end()) {
                        c = static_cast<char>(tolower(*i));
                        if (c == '.') {
                            t.column += i - start;
                            error(1004, t, "non-decimal fraction not supported");
                        }
                        if (not number_body(c)) {
                            break;
                        }
                        if (c != '_') {
                            int d = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'z' ? c - 'a' + 10 : -1;
                            if (d < 0 || d >= base) {
                                t.column += i - start;
                                error(1005, t, "invalid digit for given base");
                            }
                            value = number_add(number_multiply(value, number_from_uint32(base)), number_from_uint32(d));
                        }
                        utf8::advance(i, 1, source.end());
                    }
                    if (i == nstart) {
                        t.column += i - start;
                        error(1008, t, "numeric constants must have at least one digit");
                    }
                    t.value = value;
                } else {
                    t.value = number_from_uint32(0);
                }
            } else {
                std::string n;
                while (i != source.end() && number_decimal_body(*i)) {
                    if (*i != '_') {
                        n.push_back(*i);
                    }
                    utf8::advance(i, 1, source.end());
                }
                if (i != source.end() && *i == '.') {
                    n.push_back(*i);
                    utf8::advance(i, 1, source.end());
                    if (i != source.end() && not number_start(*i)) {
                        t.column += i - startindex;
                        error(1028, t, "fractional part of number must contain at least one digit");
                    }
                    while (i != source.end() && number_decimal_body(*i)) {
                        if (*i != '_') {
                            n.push_back(*i);
                        }
                        utf8::advance(i, 1, source.end());
                    }
                }
                if (i != source.end() && tolower(*i) == 'e') {
                    n.push_back(*i);
                    utf8::advance(i, 1, source.end());
                    if (i != source.end() && (*i == '+' || *i == '-')) {
                        n.push_back(*i);
                        utf8::advance(i, 1, source.end());
                    }
                    while (i != source.end() && number_decimal_body(*i)) {
                        if (*i != '_') {
                            n.push_back(*i);
                        }
                        utf8::advance(i, 1, source.end());
                    }
                }
                t.value = number_from_string(n);
                if (not number_is_finite(t.value)) {
                    error(1029, t, "floating point number out of range");
                }
            }
            t.text = std::string(start, i);
            t.length = t.text.length();
        } else if (c == '"') {
            utf8::advance(i, 1, source.end());
            t.type = STRING;
            t.text = "";
            while (i != source.end()) {
                const auto charstartindex = i;
                c = utf8::next(i, source.end());
                if (c == '"') {
                    break;
                }
                if (i == source.end()) {
                    error(1010, t, "unterminated string");
                }
                if (c == '\n') {
                    error(1021, t, "unterminated string (must be single line)");
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
                        case 'U': {
                            if (*i == '{') {
                                utf8::advance(i, 1, source.end());
                                auto namestart = i;
                                std::string name;
                                while (*i != '}') {
                                    if (*i == '"') {
                                        t.column += i - startindex;
                                        error(1026, t, "unterminated unicode character name");
                                    }
                                    name.push_back(*i);
                                    utf8::advance(i, 1, source.end());
                                }
                                utf8::advance(i, 1, source.end());
                                c = unicode_lookup(t, namestart - startindex, name);
                            } else {
                                int len = c == 'U' ? 8 : 4;
                                if (source.end() - i < len) {
                                    error(1012, t, "unterminated string");
                                }
                                for (int j = 0; j < len; j++) {
                                    if (not isxdigit(i[j])) {
                                        t.column += (i - startindex) + j;
                                        error(1013, t, "invalid hex character");
                                    }
                                }
                                c = std::stoul(std::string(i, i+len), nullptr, 16);
                                utf8::advance(i, len, source.end());
                            }
                            break;
                        }
                        case '(': {
                            t.length = i - startindex;
                            tokens.push_back(t);
                            t.column = column + (i - startindex) - 1;
                            t.length = 1;
                            t.type = SUBBEGIN;
                            tokens.push_back(t);
                            auto start = i;
                            auto colon = start;
                            int nest = 1;
                            bool inquote = false;
                            while (nest > 0) {
                                if (i == source.end()) {
                                    error(1015, t, "unexpected end of file");
                                }
                                c = utf8::next(i, source.end());
                                switch (c) {
                                    case '(':
                                        if (not inquote) {
                                            nest++;
                                        }
                                        break;
                                    case ')':
                                        if (not inquote) {
                                            nest--;
                                        }
                                        break;
                                    case ':':
                                        if (not inquote && nest == 1) {
                                            colon = i - 1;
                                        }
                                        break;
                                    case '"':
                                        inquote = not inquote;
                                        break;
                                    case '\\':
                                    case '\n':
                                        t.column += i - start;
                                        error(1014, t, "invalid char embedded in string substitution");
                                }
                            }
                            auto end = i - 1;
                            if (colon > start) {
                                end = colon;
                            }
                            size_t col = column + (start - startindex);
                            auto subtokens = tokenize_fragment(tsource, source_path, line, col, std::string(start, end));
                            std::copy(subtokens.begin(), subtokens.end(), std::back_inserter(tokens));
                            if (colon > start) {
                                t.column = column + (colon - startindex);
                                t.length = 0;
                                t.type = SUBFMT;
                                tokens.push_back(t);
                                t.column += 1;
                                t.type = STRING;
                                t.text = std::string(colon + 1, i - 1);
                                t.length = t.text.length();
                                tokens.push_back(t);
                            }
                            t.column = column + (i - startindex) - 1;
                            t.length = 0;
                            t.type = SUBEND;
                            tokens.push_back(t);
                            t.column = column + (i - startindex);
                            t.type = STRING;
                            t.text = "";
                            continue;
                        }
                        default:
                            t.column += i - startindex - 2;
                            t.text = std::string("\\") + (char)c;
                            error(1009, t, "invalid escape character");
                    }
                }
                t.text_source_offset.push_back(charstartindex - startindex - 1);
                utf8::append(c, std::back_inserter(t.text));
            }
            t.text_source_offset.push_back(i - startindex - 2);
            t.length = i - startindex;
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
                    if (c == '\n') {
                        error(1022, t, "unterminated raw string (must be single line)");
                    }
                    t.text_source_offset.push_back(i - startindex - 2);
                    utf8::append(c, std::back_inserter(t.text));
                }
                t.text_source_offset.push_back(i - startindex - 2);
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
                    if (c == '\n') {
                        line++;
                        startindex = i+1;
                        column = 1;
                        linestart = i+1;
                        lineend = std::find(i+1, source.end(), '\n');
                    }
                    t.text_source_offset.push_back(i - startindex - 2);
                    utf8::append(c, std::back_inserter(t.text));
                }
                t.text_source_offset.push_back(i - startindex - 2);
            }
            t.length = i - startindex;
        } else if (c == '\t') {
            column += 8 - ((column - 1) % 8);
            utf8::advance(i, 1, source.end());
            continue;
        } else if (space(c)) {
            if (*i == '\n') {
                line++;
                startindex = i+1;
                column = 1;
                linestart = i+1;
                lineend = std::find(i+1, source.end(), '\n');
            }
            utf8::advance(i, 1, source.end());
        } else {
            error(1007, t, "Unexpected character");
        }
        if (t.type == EXEC) {
            tokens.push_back(t);
            t.type = STRING;
            t.column = column + (i - startindex);
            t.text = "";
            while (i != source.end() && *i != ';') {
                uint32_t e = utf8::peek_next(i, source.end());
                utf8::advance(i, 1, source.end());
                utf8::append(e, std::back_inserter(t.text));
                if (e == '\n' && i != source.end()) {
                    line++;
                    column = 0;
                    linestart = i+1;
                    lineend = std::find(i+1, source.end(), '\n');
                    startindex = i;
                }
            }
            if (i == source.end()) {
                error(4101, tokens.back(), "EXEC statement missing ';' terminator");
            }
            utf8::advance(i, 1, source.end());
        }
        if (t.type != NONE) {
            if (t.text.empty() && t.type != STRING) {
                t.text = std::string(startindex, i);
            }
            tokens.push_back(t);
        }
        column += utf8::distance(startindex, i);
    }
    return tokens;
}

std::unique_ptr<TokenizedSource> tokenize(const std::string &source_path, const std::string &source)
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
        size_t column = (1 + inv - (source.begin() + bol));
        Token t;
        t.line = line;
        t.column = column;
        error(1000, t, "invalid UTF-8 data in source");
    }

    SHA256 sha256;
    sha256(source);
    unsigned char h[SHA256::HashBytes];
    sha256.getHash(h);

    int line = 1;
    std::string::const_iterator i = source.begin();
    if (source.substr(0, 2) == "#!") {
        while (i != source.end() && *i != '\n') {
            utf8::advance(i, 1, source.end());
        }
    }

    std::unique_ptr<TokenizedSource> r { new TokenizedSource() };
    r->source_path = source_path;
    r->source_hash = std::string(h, h+sizeof(h));
    r->source_text = std::string(i, source.end());
    r->source_lines.resize(1); // Leave room for nonexistent line 0.
    r->tokens = tokenize_fragment(r.get(), source_path, line, 1, r->source_text);
    Token t(r.get());
    t.line = line;
    t.column = 1;
    t.type = END_OF_FILE;
    r->tokens.push_back(t);
    return r;
}
