#include <fstream>
#include <iostream>
#include <sstream>

#include "parser.h"
#include "util.h"

std::string reconstitute(const Token &t)
{
    switch (t.type) {
        case NONE:        exit(1);
        case END_OF_FILE: return "";
        case NUMBER:      return number_to_string(t.value);
        case STRING:      return "\"" + t.text + "\"";
        case IDENTIFIER:  return t.text;
        case LPAREN:      return "(";
        case RPAREN:      return ")";
        case LBRACKET:    return "[";
        case RBRACKET:    return "]";
        case LBRACE:      return "{";
        case RBRACE:      return "}";
        case COLON:       return ":";
        case ASSIGN:      return ":=";
        case PLUS:        return "+";
        case MINUS:       return "-";
        case TIMES:       return "*";
        case DIVIDE:      return "/";
        case MOD:         return "MOD";
        case EXP:         return "^";
        case CONCAT:      return "&";
        case EQUAL:       return "=";
        case NOTEQUAL:    return "#";
        case LESS:        return "<";
        case GREATER:     return ">";
        case LESSEQ:      return "<=";
        case GREATEREQ:   return ">=";
        case COMMA:       return ",";
        case IF:          return "IF";
        case THEN:        return "THEN";
        case ELSE:        return "ELSE";
        case END:         return "END";
        case WHILE:       return "WHILE";
        case DO:          return "DO";
        case VAR:         return "VAR";
        case FUNCTION:    return "FUNCTION";
        case RETURN:      return "RETURN";
        case FALSE:       return "FALSE";
        case TRUE:        return "TRUE";
        case AND:         return "AND";
        case OR:          return "OR";
        case NOT:         return "NOT";
        case FOR:         return "FOR";
        case TO:          return "TO";
        case STEP:        return "STEP";
        case ARRAY:       return "Array";
        case DICTIONARY:  return "Dictionary";
        case DOT:         return ".";
        case TYPE:        return "TYPE";
        case RECORD:      return "RECORD";
        case ENUM:        return "ENUM";
        case CONSTANT:    return "CONSTANT";
        case IMPORT:      return "IMPORT";
        case IN:          return "IN";
        case OUT:         return "OUT";
        case INOUT:       return "INOUT";
        case ELSIF:       return "ELSIF";
        case CASE:        return "CASE";
        case WHEN:        return "WHEN";
        case EXIT:        return "EXIT";
        case NEXT:        return "NEXT";
        case LOOP:        return "LOOP";
        case REPEAT:      return "REPEAT";
        case UNTIL:       return "UNTIL";
        case DECLARE:     return "DECLARE";
        case EXCEPTION:   return "EXCEPTION";
        case TRY:         return "TRY";
        case RAISE:       return "RAISE";
        case POINTER:     return "POINTER";
        case NEW:         return "NEW";
        case NIL:         return "NIL";
        case VALID:       return "VALID";
        case ARROW:       return "->";
        case SUBBEGIN:    return "";
        case SUBFMT:      return "";
        case SUBEND:      return "";
        case LET:         return "LET";
        case FIRST:       return "FIRST";
        case LAST:        return "LAST";
        case AS:          return "AS";
        case DEFAULT:     return "DEFAULT";
        case EXPORT:      return "EXPORT";
        case PRIVATE:     return "PRIVATE";
        case NATIVE:      return "NATIVE";
        case FOREACH:     return "FOREACH";
        case INDEX:       return "INDEX";
        case ASSERT:      return "ASSERT";
        case EMBED:       return "EMBED";
        case ALIAS:       return "ALIAS";
        case IS:          return "IS";
        case BEGIN:       return "BEGIN";
        case MAIN:        return "MAIN";
        case HEXBYTES:    return "HEXBYTES";
        case INC:         return "INC";
        case DEC:         return "DEC";
        case UNDERSCORE:  return "_";
        case OTHERS:      return "OTHERS";
        case WITH:        return "WITH";
        case CHECK:       return "CHECK";
        case GIVES:       return "GIVES";
        case NOWHERE:     return "NOWHERE";
        case INTDIV:      return "INTDIV";
        case EXEC:        return "EXEC";
        case LABEL:       return "LABEL";
        case CLASS:       return "CLASS";
        case TRAP:        return "TRAP";
        case EXTENSION:   return "EXTENSION";
        case INTERFACE:   return "INTERFACE";
        case IMPLEMENTS:  return "IMPLEMENTS";
        case UNUSED:      return "UNUSED";
        case ISA:         return "ISA";
        case ELLIPSIS:    return "ELLIPSIS";
        case OPTIONAL:    return "OPTIONAL";
        case IMPORTED:    return "IMPORTED";
        case TESTCASE:    return "TESTCASE";
        case EXPECT:      return "EXPECT";
        case CHOICE:      return "CHOICE";
        case PROCESS:     return "PROCESS";
        case SUCCESS:     return "SUCCESS";
        case FAILURE:     return "FAILURE";
        case PANIC:       return "PANIC";
        case UNKNOWN:     return "UNKNOWN";
        case MAX_TOKEN:   return "";
    }
    return "";
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        exit(1);
    }
    std::ifstream inf(argv[1]);
    std::stringstream buf;
    buf << inf.rdbuf();
    TokenizedSource tokens;
    int i = 0;
    for (auto b: buf.str()) {
        if (b > END_OF_FILE && b < MAX_TOKEN) {
            Token t;
            t.type = static_cast<TokenType>(b);
            if (t.type == IDENTIFIER) {
                t.text = std::string(1, 'a'+(i%4));
            }
            tokens.tokens.push_back(t);
        } else {
            exit(1);
        }
        i++;
    }
    Token eof;
    eof.type = END_OF_FILE;
    tokens.tokens.push_back(eof);
    for (auto t: tokens.tokens) {
        std::cout << reconstitute(t) << " ";
    }
    std::cout << std::endl;
    try {
        parse(tokens);
    } catch (SourceError &) {
        exit(1);
    }
}
