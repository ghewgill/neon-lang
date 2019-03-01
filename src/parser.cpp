#include "parser.h"

#include <iso646.h>
#include <list>
#include <set>
#include <stack>

#include "pt.h"
#include "sql.h"
#include "util.h"

using namespace pt;

class Parser {
public:
    explicit Parser(const TokenizedSource &tokens);

    TokenizedSource source;
    const std::vector<Token> tokens;
    std::vector<Token>::size_type i;
    int expression_depth;
    size_t minimum_column;

    typedef std::pair<std::vector<Token>, std::unique_ptr<Type>> VariableInfo;

    std::unique_ptr<Type> parseParameterisedType();
    std::unique_ptr<Type> parseRecordType();
    std::unique_ptr<Type> parseClassType();
    std::unique_ptr<Type> parseEnumType();
    std::unique_ptr<Type> parsePointerType();
    std::unique_ptr<Type> parseValidPointerType();
    std::unique_ptr<Type> parseFunctionType();
    std::unique_ptr<Type> parseType();
    std::unique_ptr<Declaration> parseTypeDefinition();
    std::unique_ptr<Declaration> parseConstantDefinition();
    std::unique_ptr<FunctionCallExpression> parseFunctionCall(std::unique_ptr<Expression> &&func);
    std::unique_ptr<Expression> parseArrayLiteral();
    std::unique_ptr<DictionaryLiteralExpression> parseDictionaryLiteral();
    std::unique_ptr<Expression> parseInterpolatedStringExpression();
    std::unique_ptr<Expression> parseAtom();
    std::unique_ptr<Expression> parseExponentiation();
    std::unique_ptr<Expression> parseMultiplication();
    std::unique_ptr<Expression> parseAddition();
    std::unique_ptr<Expression> parseComparison();
    std::unique_ptr<Expression> parseTypeTest();
    std::unique_ptr<Expression> parseMembership();
    std::unique_ptr<Expression> parseConjunction();
    std::unique_ptr<Expression> parseDisjunction();
    std::unique_ptr<Expression> parseConditional();
    std::unique_ptr<Expression> parseExpression();
    VariableInfo parseVariableDeclaration();
    void parseFunctionParameters(std::unique_ptr<Type> &returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &args, Token &rparen);
    void parseFunctionHeader(Token &type, Token &name, std::unique_ptr<Type> &returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &args, Token &rparen);
    std::unique_ptr<Declaration> parseFunctionDefinition(size_t start_column);
    std::unique_ptr<Declaration> parseForeignDefinition();
    std::unique_ptr<Declaration> parseDeclaration();
    std::unique_ptr<Declaration> parseException();
    std::unique_ptr<Declaration> parseInterface();
    std::unique_ptr<Statement> parseExport();
    std::unique_ptr<Statement> parseIncrementStatement();
    std::unique_ptr<Statement> parseIfStatement();
    std::unique_ptr<Statement> parseCheckStatement();
    std::unique_ptr<Statement> parseReturnStatement();
    std::unique_ptr<Declaration> parseVarStatement();
    std::unique_ptr<Declaration> parseLetStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<Statement> parseCaseStatement();
    std::unique_ptr<Statement> parseForStatement();
    std::unique_ptr<Statement> parseForeachStatement();
    std::unique_ptr<Statement> parseLoopStatement();
    std::unique_ptr<Statement> parseRepeatStatement();
    std::unique_ptr<Statement> parseExitStatement();
    std::unique_ptr<Statement> parseNextStatement();
    std::unique_ptr<Statement> parseTryStatement();
    std::unique_ptr<Statement> parseRaiseStatement();
    std::unique_ptr<Statement> parseExecStatement();
    std::unique_ptr<Statement> parseUnusedStatement();
    std::unique_ptr<Statement> parseImport();
    std::unique_ptr<Statement> parseAssert();
    std::unique_ptr<Statement> parseBegin();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Program> parse();
private:
    std::vector<std::unique_ptr<TypeRecord::Field>> parseFields();
    Parser(const Parser &);
    Parser &operator=(const Parser &);

    class TemporaryMinimumIndent {
    public:
        TemporaryMinimumIndent(Parser *parser, size_t indent): parser(parser), old_indent(parser->minimum_column) {
            parser->minimum_column = indent;
        }
        ~TemporaryMinimumIndent() {
            parser->minimum_column = old_indent;
        }
    private:
        Parser *parser;
        const size_t old_indent;
    private:
        TemporaryMinimumIndent(const TemporaryMinimumIndent &);
        TemporaryMinimumIndent &operator=(const TemporaryMinimumIndent &);
    };
};

Parser::Parser(const TokenizedSource &tokens)
  : source(tokens),
    tokens(tokens.tokens),
    i(0),
    expression_depth(0),
    minimum_column(1)
{
}

static ComparisonExpression::Comparison comparisonFromToken(const Token &token)
{
    switch (token.type) {
        case EQUAL:     return ComparisonExpression::Comparison::EQ;
        case NOTEQUAL:  return ComparisonExpression::Comparison::NE;
        case LESS:      return ComparisonExpression::Comparison::LT;
        case GREATER:   return ComparisonExpression::Comparison::GT;
        case LESSEQ:    return ComparisonExpression::Comparison::LE;
        case GREATEREQ: return ComparisonExpression::Comparison::GE;
        default:
            internal_error("invalid comparison type");
    }
}

std::unique_ptr<Type> Parser::parseParameterisedType()
{
    auto &tok_type = tokens[i];
    switch (tok_type.type) {
        case ARRAY:      break;
        case DICTIONARY: break;
        default:
            internal_error("unexpected parameterised type");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(2002, tokens[i], "'<' expected");
    }
    auto &tok_less = tokens[i];
    i++;
    std::unique_ptr<Type> elementtype = parseType();
    if (tokens[i].type != GREATER) {
        error2(2003, tokens[i], "'>' expected", tok_less, "opening '<' here");
    }
    i++;
    return std::unique_ptr<Type> { new TypeParameterised(tok_type, std::move(elementtype)) };
}

std::vector<std::unique_ptr<TypeRecord::Field>> Parser::parseFields()
{
    std::vector<std::unique_ptr<TypeRecord::Field>> fields;
    while (tokens[i].type != END) {
        bool is_private = false;
        if (tokens[i].type == PRIVATE) {
            is_private = true;
            ++i;
        }
        if (tokens[i].type != IDENTIFIER) {
            error(2004, tokens[i], "identifier expected");
        }
        const Token &name = tokens[i];
        ++i;
        if (tokens[i].type != COLON) {
            error(2005, tokens[i], "colon expected");
        }
        ++i;
        std::unique_ptr<Type> t = parseType();
        fields.emplace_back(new TypeRecord::Field(name, std::move(t), is_private));
    }
    i++;
    return fields;
}

std::unique_ptr<Type> Parser::parseRecordType()
{
    if (tokens[i].type != RECORD) {
        internal_error("RECORD expected");
    }
    auto &tok_record = tokens[i];
    i++;
    std::vector<std::unique_ptr<TypeRecord::Field>> fields = parseFields();
    if (tokens[i].type != RECORD) {
        error_a(2034, tokens[i-1], tokens[i], "'RECORD' expected");
    }
    i++;
    return std::unique_ptr<Type> { new TypeRecord(tok_record, std::move(fields)) };
}

std::unique_ptr<Type> Parser::parseClassType()
{
    if (tokens[i].type != CLASS) {
        internal_error("CLASS expected");
    }
    auto &tok_class = tokens[i];
    i++;
    std::vector<std::pair<Token, Token>> interfaces;
    while (tokens[i].type == IMPLEMENTS) {
        do {
            i++;
            std::pair<Token, Token> qname;
            if (tokens[i].type != IDENTIFIER) {
                error(2132, tokens[i], "identifier expected");
            }
            qname.second = tokens[i];
            i++;
            if (tokens[i].type == DOT) {
                i++;
                if (tokens[i].type != IDENTIFIER) {
                    error(2133, tokens[i], "identifier expected");
                }
                qname.first = qname.second;
                qname.second = tokens[i];
                i++;
            }
            interfaces.push_back(qname);
        } while (tokens[i].type == COMMA);
    }
    std::vector<std::unique_ptr<TypeRecord::Field>> fields = parseFields();
    if (tokens[i].type != CLASS) {
        error_a(2117, tokens[i-1], tokens[i], "'CLASS' expected");
    }
    i++;
    return std::unique_ptr<Type> { new TypeClass(tok_class, std::move(fields), interfaces) };
}

std::unique_ptr<Type> Parser::parseEnumType()
{
    if (tokens[i].type != ENUM) {
        internal_error("ENUM expected");
    }
    auto &tok_enum = tokens[i];
    i++;
    std::vector<std::pair<Token, int>> names;
    int index = 0;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2006, tokens[i], "identifier expected");
        }
        const Token &name = tokens[i];
        i++;
        names.push_back(std::make_pair(name, index));
        index++;
    }
    i++;
    if (tokens[i].type != ENUM) {
        error_a(2035, tokens[i-1], tokens[i], "'ENUM' expected");
    }
    i++;
    return std::unique_ptr<Type> { new TypeEnum(tok_enum, names) };
}

std::unique_ptr<Type> Parser::parsePointerType()
{
    if (tokens[i].type != POINTER) {
        internal_error("POINTER expected");
    }
    auto &tok_pointer = tokens[i];
    i++;
    if (tokens[i].type == TO) {
        i++;
        std::unique_ptr<Type> reftype = parseType();
        return std::unique_ptr<Type> { new TypePointer(tok_pointer, std::move(reftype)) };
    } else {
        return std::unique_ptr<Type> { new TypePointer(tok_pointer, nullptr) };
    }
}

std::unique_ptr<Type> Parser::parseValidPointerType()
{
    if (tokens[i].type != VALID) {
        internal_error("VALID expected");
    }
    auto &tok_valid = tokens[i];
    i++;
    if (tokens[i].type != POINTER) {
        error(2118, tokens[i], "POINTER expected");
    }
    i++;
    if (tokens[i].type != TO) {
        error(2119, tokens[i], "TO expected");
    }
    i++;
    std::unique_ptr<Type> reftype = parseType();
    return std::unique_ptr<Type> { new TypeValidPointer(tok_valid, std::move(reftype)) };
}

std::unique_ptr<Type> Parser::parseFunctionType()
{
    if (tokens[i].type != FUNCTION) {
        internal_error("FUNCTION expected");
    }
    auto &tok_function = tokens[i];
    i++;
    std::unique_ptr<Type> returntype;
    std::vector<std::unique_ptr<FunctionParameterGroup>> args;
    Token rparen;
    parseFunctionParameters(returntype, args, rparen);
    return std::unique_ptr<Type> { new TypeFunctionPointer(tok_function, std::move(returntype), std::move(args)) };
}

std::unique_ptr<Type> Parser::parseType()
{
    if (tokens[i].type == ARRAY || tokens[i].type == DICTIONARY) {
        return parseParameterisedType();
    }
    if (tokens[i].type == RECORD) {
        return parseRecordType();
    }
    if (tokens[i].type == CLASS) {
        return parseClassType();
    }
    if (tokens[i].type == ENUM) {
        return parseEnumType();
    }
    if (tokens[i].type == POINTER) {
        return parsePointerType();
    }
    if (tokens[i].type == VALID) {
        return parseValidPointerType();
    }
    if (tokens[i].type == FUNCTION) {
        return parseFunctionType();
    }
    if (tokens[i].type != IDENTIFIER) {
        error(2007, tokens[i], "identifier expected");
    }
    const Token &name = tokens[i];
    i++;
    if (tokens[i].type == DOT) {
        i++;
        if (tokens[i].type != IDENTIFIER) {
            error(2075, tokens[i], "identifier expected");
        }
        const Token &subname = tokens[i];
        i++;
        return std::unique_ptr<Type> { new TypeImport(name, name, subname) };
    } else {
        return std::unique_ptr<Type> { new TypeSimple(name, name.text) };
    }
}

std::unique_ptr<Declaration> Parser::parseTypeDefinition()
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2008, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    ++i;
    if (tokens[i].type != IS) {
        error(2009, tokens[i], "'IS' expected");
    }
    ++i;
    std::unique_ptr<Type> type = parseType();
    return std::unique_ptr<Declaration> { new TypeDeclaration(tok_name, std::move(type)) };
}

std::unique_ptr<Declaration> Parser::parseConstantDefinition()
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2010, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    ++i;
    if (tokens[i].type != COLON) {
        error(2011, tokens[i], "':' expected");
    }
    ++i;
    std::unique_ptr<Type> type = parseType();
    if (tokens[i].type != ASSIGN) {
        error(2012, tokens[i], "':=' expected");
    }
    ++i;
    std::unique_ptr<Expression> value = parseExpression();
    return std::unique_ptr<Declaration> { new ConstantDeclaration(tok_name, tok_name, std::move(type), std::move(value)) };
}

std::unique_ptr<FunctionCallExpression> Parser::parseFunctionCall(std::unique_ptr<Expression> &&func)
{
    ++i;
    std::vector<std::unique_ptr<FunctionCallExpression::Argument>> args;
    if (tokens[i].type != RPAREN) {
        for (;;) {
            Token mode;
            switch (tokens[i].type) {
                case IN:
                case INOUT:
                case OUT:
                    mode = tokens[i];
                    ++i;
                    break;
                default:
                    break;
            }
            Token name;
            if (tokens[i].type == IDENTIFIER && tokens[i+1].type == WITH) {
                name = tokens[i];
                i += 2;
            }
            std::unique_ptr<Expression> e { nullptr };
            if (tokens[i].type == UNDERSCORE) {
                e.reset(new DummyExpression(tokens[i], tokens[i].column, tokens[i].column));
                ++i;
            } else {
                e = parseExpression();
            }
            args.emplace_back(new FunctionCallExpression::Argument(mode, name, std::move(e)));
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2013, tokens[i], "')' or ',' expected");
        }
    }
    auto &tok_rparen = tokens[i];
    ++i;
    return std::unique_ptr<FunctionCallExpression> { new FunctionCallExpression(func->token, std::move(func), std::move(args), tok_rparen) };
}

std::unique_ptr<Expression> Parser::parseArrayLiteral()
{
    auto &tok_lbracket = tokens[i];
    ++i;
    std::vector<std::unique_ptr<Expression>> elements;
    while (tokens[i].type != RBRACKET) {
        std::unique_ptr<Expression> element = parseExpression();
        if (tokens[i].type == COMMA) {
            ++i;
        } else if (tokens[i].type == TO) {
            if (elements.size() >= 1) {
                error(2109, tokens[i], "',' expected");
            }
            ++i;
            std::unique_ptr<Expression> first { std::move(element) };
            std::unique_ptr<Expression> last = parseExpression();
            std::unique_ptr<Expression> step { nullptr };
            if (tokens[i].type == STEP) {
                ++i;
                step = parseExpression();
            } else {
                step = std::unique_ptr<Expression> { new NumberLiteralExpression(Token(), number_from_uint32(1)) };
            }
            if (tokens[i].type != RBRACKET) {
                error2(2099, tokens[i], "']' expected", tok_lbracket, "opening '[' here");
            }
            ++i;
            return std::unique_ptr<Expression> { new ArrayLiteralRangeExpression(tok_lbracket, tokens[i-1].column+1, std::move(first), std::move(last), std::move(step)) };
        } else if (tokens[i].type != RBRACKET) {
            error2(2053, tokens[i], "',' or ']' expected", tok_lbracket, "opening '[' here");
        }
        elements.push_back(std::move(element));
    }
    ++i;
    return std::unique_ptr<Expression> { new ArrayLiteralExpression(tok_lbracket, tokens[i-1].column+1, std::move(elements)) };
}

std::unique_ptr<DictionaryLiteralExpression> Parser::parseDictionaryLiteral()
{
    auto &tok_lbrace = tokens[i];
    ++i;
    std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> elements;
    while (tokens[i].type != RBRACE) {
        std::unique_ptr<Expression> key = parseExpression();
        if (tokens[i].type != COLON) {
            error(2048, tokens[i], "':' expected");
        }
        ++i;
        std::unique_ptr<Expression> element = parseExpression();
        elements.push_back(std::make_pair(std::move(key), std::move(element)));
        if (tokens[i].type == COMMA) {
            ++i;
        }
    }
    ++i;
    return std::unique_ptr<DictionaryLiteralExpression> { new DictionaryLiteralExpression(tok_lbrace, tokens[i-1].column+1, std::move(elements)) };
}

std::unique_ptr<Expression> Parser::parseInterpolatedStringExpression()
{
    std::vector<std::pair<std::unique_ptr<Expression>, Token>> parts;
    parts.push_back(std::make_pair(std::unique_ptr<Expression> { new StringLiteralExpression(tokens[i], tokens[i+1].column, utf8string(tokens[i].text)) }, Token()));
    for (;;) {
        ++i;
        if (tokens[i].type != SUBBEGIN) {
            break;
        }
        ++i;
        std::unique_ptr<Expression> e = parseExpression();
        Token fmt;
        if (tokens[i].type == SUBFMT) {
            ++i;
            if (tokens[i].type != STRING) {
                internal_error("parseInterpolatedStringExpression");
            }
            fmt = tokens[i];
            ++i;
        }
        parts.push_back(std::make_pair(std::move(e), fmt));
        if (tokens[i].type != SUBEND) {
            internal_error("parseInterpolatedStringExpression");
        }
        ++i;
        if (tokens[i].type != STRING) {
            internal_error("parseInterpolatedStringExpression");
        }
        e.reset(new StringLiteralExpression(tokens[i], tokens[i+1].column, utf8string(tokens[i].text)));
        parts.push_back(std::make_pair(std::move(e), Token()));
    }
    return std::unique_ptr<Expression> { new InterpolatedStringExpression(parts[0].first->token, std::move(parts)) };
}

/*
 * Operator precedence:
 *
 *  ^        exponentiation                     parseExponentiation
 *  * / MOD  multiplication, division, modulo   parseMultiplication
 *  + -      addition, subtraction              parseAddition
 *  < = >    comparison                         parseComparison
 *  isa      type test                          parseTypeTest
 *  in       membership                         parseMembership
 *  and      conjunction                        parseConjunction
 *  or       disjunction                        parseDisjunction
 *  if       conditional                        parseConditional
 */

std::unique_ptr<Expression> Parser::parseAtom()
{
    switch (tokens[i].type) {
        case LPAREN: {
            auto &tok_lparen = tokens[i];
            ++i;
            std::unique_ptr<Expression> expr = parseConditional();
            if (tokens[i].type != RPAREN) {
                error2(2014, tokens[i], ") expected", tok_lparen, "opening '(' here");
            }
            ++i;
            return std::unique_ptr<Expression> { new IdentityExpression(tok_lparen, tok_lparen.column, tokens[i-1].column+1, std::move(expr)) };
        }
        case LBRACKET: {
            std::unique_ptr<Expression> array = parseArrayLiteral();
            return array;
        }
        case LBRACE: {
            std::unique_ptr<Expression> dict = parseDictionaryLiteral();
            return dict;
        }
        case FALSE: {
            auto &tok_false = tokens[i];
            ++i;
            return std::unique_ptr<Expression> { new BooleanLiteralExpression(tok_false, false) };
        }
        case TRUE: {
            auto &tok_true = tokens[i];
            ++i;
            return std::unique_ptr<Expression> { new BooleanLiteralExpression(tok_true, true) };
        }
        case NUMBER: {
            auto &tok_number = tokens[i];
            i++;
            return std::unique_ptr<Expression> { new NumberLiteralExpression(tok_number, tok_number.value) };
        }
        case STRING: {
            if (tokens[i+1].type == SUBBEGIN) {
                return parseInterpolatedStringExpression();
            } else {
                auto &tok_string = tokens[i];
                i++;
                return std::unique_ptr<Expression> { new StringLiteralExpression(tok_string, tokens[i].column, utf8string(tok_string.text)) };
            }
        }
        case EMBED: {
            ++i;
            auto &tok_file = tokens[i];
            ++i;
            if (tok_file.type != STRING) {
                error(2090, tok_file, "string literal expected");
            }
            return std::unique_ptr<Expression> { new FileLiteralExpression(tok_file, tokens[i].column, tok_file.text) };
        }
        case HEXBYTES: {
            ++i;
            auto &tok_literal = tokens[i];
            ++i;
            if (tok_literal.type != STRING) {
                error(2094, tok_literal, "string literal expected");
            }
            return std::unique_ptr<Expression> { new BytesLiteralExpression(tok_literal, tokens[i].column, tok_literal.text) };
        }
        case PLUS: {
            auto &tok_plus = tokens[i];
            ++i;
            std::unique_ptr<Expression> atom = parseAtom();
            return std::unique_ptr<Expression> { new UnaryPlusExpression(tok_plus, std::move(atom)) };
        }
        case MINUS: {
            auto &tok_minus = tokens[i];
            ++i;
            std::unique_ptr<Expression> atom = parseAtom();
            return std::unique_ptr<Expression> { new UnaryMinusExpression(tok_minus, std::move(atom)) };
        }
        case NOT: {
            auto &tok_not = tokens[i];
            ++i;
            std::unique_ptr<Expression> atom = parseAtom();
            return std::unique_ptr<Expression> { new LogicalNotExpression(tok_not, std::move(atom)) };
        }
        case NEW: {
            auto &tok_new = tokens[i];
            ++i;
            if (tokens[i].type != IDENTIFIER) {
                error(2110, tokens[i], "identifier expected");
            }
            std::unique_ptr<Expression> expr { new IdentifierExpression(tokens[i], tokens[i].text) };
            ++i;
            if (tokens[i].type == DOT) {
                auto &tok_dot = tokens[i];
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2111, tokens[i], "identifier expected");
                }
                expr.reset(new DotExpression(tok_dot, std::move(expr), tokens[i]));
                ++i;
            }
            if (tokens[i].type == LPAREN) {
                expr = parseFunctionCall(std::move(expr));
            }
            return std::unique_ptr<Expression> { new NewClassExpression(tok_new, tokens[i].column, std::move(expr)) };
        }
        case NIL: {
            auto &tok_nil = tokens[i];
            ++i;
            return std::unique_ptr<Expression> { new NilLiteralExpression(tok_nil) };
        }
        case NOWHERE: {
            auto &tok_nowhere = tokens[i];
            ++i;
            return std::unique_ptr<Expression> { new NowhereLiteralExpression(tok_nowhere) };
        }
        case IF: {
            error(2095, tokens[i], "Use parentheses around (IF ... THEN ... ELSE ...)");
        }
        case TRY: {
            error(2106, tokens[i], "Use parentheses around (TRY ... TRAP ...)");
        }
        case IDENTIFIER: {
            std::unique_ptr<Expression> expr { new IdentifierExpression(tokens[i], tokens[i].text) };
            ++i;
            for (;;) {
                if (tokens[i].type == LBRACKET) {
                    auto &tok_lbracket = tokens[i];
                    ++i;
                    do {
                        std::unique_ptr<Expression> index { nullptr };
                        bool first_from_end = false;
                        if (tokens[i].type == FIRST || tokens[i].type == LAST) {
                            first_from_end = tokens[i].type == LAST;
                            ++i;
                            if (tokens[i].type == RBRACKET || tokens[i].type == TO) {
                                index.reset(new NumberLiteralExpression(Token(), number_from_uint32(0)));
                            } else if (tokens[i].type != PLUS && tokens[i].type != MINUS) {
                                error(2072, tokens[i], "'+' or '-' expected");
                            }
                        }
                        if (index == nullptr) {
                            index = parseExpression();
                        }
                        std::unique_ptr<ArrayRange> range { nullptr };
                        std::unique_ptr<Expression> last { nullptr };
                        const bool has_range = tokens[i].type == TO;
                        if (has_range) {
                            ++i;
                            last = nullptr;
                            bool last_from_end = false;
                            if (tokens[i].type == FIRST || tokens[i].type == LAST) {
                                last_from_end = tokens[i].type == LAST;
                                ++i;
                                if (tokens[i].type == RBRACKET) {
                                    last.reset(new NumberLiteralExpression(Token(), number_from_uint32(0)));
                                } else if (tokens[i].type != PLUS && tokens[i].type != MINUS) {
                                    error(2073, tokens[i], "'+' or '-' expected");
                                }
                            }
                            if (last == nullptr) {
                                last = parseExpression();
                            }
                            range.reset(new ArrayRange(tok_lbracket, std::move(index), first_from_end, std::move(last), last_from_end));
                        }
                        if (tokens[i].type != COMMA && tokens[i].type != RBRACKET) {
                            error(2020, tokens[i], "']' expected");
                        }
                        ++i;
                        if (range != nullptr) {
                            expr.reset(new RangeSubscriptExpression(tok_lbracket, tokens[i].column, std::move(expr), std::move(range)));
                        } else {
                            expr.reset(new SubscriptExpression(tok_lbracket, tokens[i].column, std::move(expr), std::move(index), first_from_end));
                        }
                    } while (tokens[i-1].type == COMMA);
                } else if (tokens[i].type == LPAREN) {
                    expr = parseFunctionCall(std::move(expr));
                } else if (tokens[i].type == DOT) {
                    auto &tok_dot = tokens[i];
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2021, tokens[i], "identifier expected");
                    }
                    const Token &field = tokens[i];
                    ++i;
                    expr.reset(new DotExpression(tok_dot, std::move(expr), field));
                } else if (tokens[i].type == ARROW) {
                    auto &tok_arrow = tokens[i];
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2066, tokens[i], "identifier expected");
                    }
                    const Token &field = tokens[i];
                    ++i;
                    expr.reset(new ArrowExpression(tok_arrow, std::move(expr), field));
                } else {
                    // TODO: what happens here?
                    break;
                }
            }
            return expr;
        }
        default:
            error(2015, tokens[i], "Expression expected");
    }
}

std::unique_ptr<Expression> Parser::parseExponentiation()
{
    std::unique_ptr<Expression> left = parseAtom();
    for (;;) {
        auto &tok_op = tokens[i];
        if (tokens[i].type == EXP) {
            ++i;
            std::unique_ptr<Expression> right = parseAtom();
            left.reset(new ExponentiationExpression(tok_op, std::move(left), std::move(right)));
        } else {
            return left;
        }
    }
}

std::unique_ptr<Expression> Parser::parseMultiplication()
{
    std::unique_ptr<Expression> left = parseExponentiation();
    for (;;) {
        auto &tok_op = tokens[i];
        switch (tokens[i].type) {
            case TIMES: {
                ++i;
                std::unique_ptr<Expression> right = parseExponentiation();
                left.reset(new MultiplicationExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            case DIVIDE: {
                ++i;
                std::unique_ptr<Expression> right = parseExponentiation();
                left.reset(new DivisionExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            case INTDIV: {
                ++i;
                std::unique_ptr<Expression> right = parseExponentiation();
                left.reset(new IntegerDivisionExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            case MOD: {
                ++i;
                std::unique_ptr<Expression> right = parseExponentiation();
                left.reset(new ModuloExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            default:
                return left;
        }
    }
}

std::unique_ptr<Expression> Parser::parseAddition()
{
    std::unique_ptr<Expression> left = parseMultiplication();
    for (;;) {
        auto &tok_op = tokens[i];
        switch (tokens[i].type) {
            case PLUS: {
                ++i;
                std::unique_ptr<Expression> right = parseMultiplication();
                left.reset(new AdditionExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            case MINUS: {
                ++i;
                std::unique_ptr<Expression> right = parseMultiplication();
                left.reset(new SubtractionExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            case CONCAT: {
                ++i;
                std::unique_ptr<Expression> right = parseMultiplication();
                left.reset(new ConcatenationExpression(tok_op, std::move(left), std::move(right)));
                break;
            }
            default:
                return left;
        }
    }
}

std::unique_ptr<Expression> Parser::parseComparison()
{
    std::unique_ptr<Expression> left = parseAddition();
    std::vector<std::unique_ptr<ChainedComparisonExpression::Part>> comps;
    Token tok_comp;
    while (tokens[i].type == EQUAL  || tokens[i].type == NOTEQUAL
        || tokens[i].type == LESS   || tokens[i].type == GREATER
        || tokens[i].type == LESSEQ || tokens[i].type == GREATEREQ) {
        tok_comp = tokens[i];
        if (tok_comp.type == EQUAL && tokens[i+1].type == EQUAL) {
            error(2078, tok_comp, "'==' not expected, use '=' for comparison");
        }
        ComparisonExpression::Comparison comp = comparisonFromToken(tok_comp);
        ++i;
        std::unique_ptr<Expression> right = parseAddition();
        comps.emplace_back(new ChainedComparisonExpression::Part(comp, std::move(right)));
    }
    if (comps.empty()) {
        return left;
    } else if (comps.size() == 1) {
        return std::unique_ptr<Expression> { new ComparisonExpression(tok_comp, std::move(left), std::move(comps[0]->right), comps[0]->comp) };
    } else {
        return std::unique_ptr<Expression> { new ChainedComparisonExpression(std::move(left), std::move(comps)) };
    }
}

std::unique_ptr<Expression> Parser::parseTypeTest()
{
    std::unique_ptr<Expression> left = parseComparison();
    if (tokens[i].type == ISA) {
        auto &tok_op = tokens[i];
        ++i;
        std::unique_ptr<Type> target = parseType();
        std::unique_ptr<Expression> r { new TypeTestExpression(tok_op, std::move(left), std::move(target)) };
        return r;
    } else {
        return left;
    }
}

std::unique_ptr<Expression> Parser::parseMembership()
{
    std::unique_ptr<Expression> left = parseTypeTest();
    if (tokens[i].type == IN || (tokens[i].type == NOT && tokens[i+1].type == IN)) {
        auto &tok_op = tokens[i];
        bool notin = tokens[i].type == NOT;
        if (notin) {
            ++i;
        }
        ++i;
        std::unique_ptr<Expression> right = parseTypeTest();
        std::unique_ptr<Expression> r { new MembershipExpression(tok_op, std::move(left), std::move(right)) };
        if (notin) {
            r.reset(new LogicalNotExpression(tok_op, std::move(r)));
        }
        return r;
    } else {
        return left;
    }
}

std::unique_ptr<Expression> Parser::parseConjunction()
{
    std::unique_ptr<Expression> left = parseMembership();
    for (;;) {
        auto &tok_op = tokens[i];
        if (tokens[i].type == AND) {
            ++i;
            std::unique_ptr<Expression> right = parseMembership();
            left.reset(new ConjunctionExpression(tok_op, std::move(left), std::move(right)));
        } else {
            return left;
        }
    }
}

std::unique_ptr<Expression> Parser::parseDisjunction()
{
    std::unique_ptr<Expression> left = parseConjunction();
    for (;;) {
        auto &tok_op = tokens[i];
        if (tokens[i].type == OR) {
            ++i;
            std::unique_ptr<Expression> right = parseConjunction();
            left.reset(new DisjunctionExpression(tok_op, std::move(left), std::move(right)));
        } else {
            return left;
        }
    }
}

std::unique_ptr<Expression> Parser::parseConditional()
{
    if (tokens[i].type == IF) {
        auto &tok_if = tokens[i];
        ++i;
        std::unique_ptr<Expression> cond = parseExpression();
        if (tokens[i].type != THEN) {
            error(2016, tokens[i], "'THEN' expected");
        }
        ++i;
        std::unique_ptr<Expression> left = parseExpression();
        if (tokens[i].type != ELSE) {
            error(2017, tokens[i], "'ELSE' expected");
        }
        ++i;
        std::unique_ptr<Expression> right = parseExpression();
        return std::unique_ptr<Expression> { new ConditionalExpression(tok_if, std::move(cond), std::move(left), std::move(right)) };
    } else if (tokens[i].type == TRY) {
        auto &tok_try = tokens[i];
        ++i;
        std::unique_ptr<Expression> expr = parseExpression();
        std::vector<std::unique_ptr<TryTrap>> catches;
        while (tokens[i].type == TRAP) {
            ++i;
            std::vector<Token> name;
            for (;;) {
                if (tokens[i].type != IDENTIFIER) {
                    error(2107, tokens[i], "identifier expected");
                }
                name.push_back(tokens[i]);
                ++i;
                if (tokens[i].type != DOT) {
                    break;
                }
                ++i;
            }
            std::vector<std::vector<Token>> exceptions;
            exceptions.push_back(name);
            Token infoname;
            if (tokens[i].type == AS) {
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2123, tokens[i], "identifier expected");
                }
                infoname = tokens[i];
                ++i;
            }
            if (tokens[i].type == DO) {
                auto &tok_do = tokens[i];
                std::vector<std::unique_ptr<Statement>> statements;
                ++i;
                while (tokens[i].type != TRAP && tokens[i].type != RPAREN && tokens[i].type != END_OF_FILE) {
                    std::unique_ptr<Statement> stmt = parseStatement();
                    if (stmt != nullptr) {
                        statements.push_back(std::move(stmt));
                    }
                }
                catches.emplace_back(new TryTrap(exceptions, infoname, std::unique_ptr<ParseTreeNode> { new TryHandlerStatement(tok_do, std::move(statements)) }));
            } else if (tokens[i].type == GIVES) {
                ++i;
                catches.emplace_back(new TryTrap(exceptions, infoname, parseExpression()));
            } else {
                error(2108, tokens[i], "DO or GIVES expected");
            }
        }
        return std::unique_ptr<Expression> { new TryExpression(tok_try, std::move(expr), std::move(catches)) };
    } else {
        return parseExpression();
    }
}

std::unique_ptr<Expression> Parser::parseExpression()
{
    expression_depth++;
    if (expression_depth > 100) {
        error(2067, tokens[i], "exceeded maximum nesting depth");
    }
    std::unique_ptr<Expression> r = parseDisjunction();
    expression_depth--;
    return r;
}

Parser::VariableInfo Parser::parseVariableDeclaration()
{
    std::vector<Token> names;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2018, tokens[i], "identifier expected");
        }
        const Token &name = tokens[i];
        ++i;
        names.push_back(name);
        if (tokens[i].type != COMMA) {
            break;
        }
        ++i;
    }
    if (tokens[i].type != COLON) {
        error(2019, tokens[i], "colon expected");
    }
    ++i;
    std::unique_ptr<Type> t = parseType();
    return make_pair(names, std::move(t));
}

void Parser::parseFunctionParameters(std::unique_ptr<Type> &returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &args, Token &rparen)
{
    if (tokens[i].type != LPAREN) {
        error(2024, tokens[i], "'(' expected");
    }
    ++i;
    if (tokens[i].type != RPAREN) {
        for (;;) {
            FunctionParameterGroup::Mode mode = FunctionParameterGroup::Mode::IN;
            switch (tokens[i].type) {
                case IN:    mode = FunctionParameterGroup::Mode::IN;       i++; break;
                case INOUT: mode = FunctionParameterGroup::Mode::INOUT;    i++; break;
                case OUT:   mode = FunctionParameterGroup::Mode::OUT;      i++; break;
                default:
                    break;
            }
            auto &tok_param = tokens[i];
            VariableInfo vars = parseVariableDeclaration();
            std::unique_ptr<Expression> default_value { nullptr };
            if (tokens[i].type == DEFAULT) {
                ++i;
                if (tokens[i].type == UNDERSCORE) {
                    default_value.reset(new DummyExpression(tokens[i], tokens[i].column, tokens[i].column));
                    ++i;
                } else {
                    default_value = parseExpression();
                }
            }
            std::unique_ptr<FunctionParameterGroup> fp { new FunctionParameterGroup(tok_param, vars.first, std::move(vars.second), mode, std::move(default_value)) };
            args.push_back(std::move(fp));
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2025, tokens[i], "')' or ',' expected");
        }
    }
    rparen = tokens[i];
    ++i;
    if (tokens[i].type == COLON) {
        ++i;
        returntype = parseType();
    } else {
        returntype = nullptr;
    }
}

void Parser::parseFunctionHeader(Token &type, Token &name, std::unique_ptr<Type> &returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &args, Token &rparen)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2023, tokens[i], "identifier expected");
    }
    name = tokens[i];
    ++i;
    if (tokens[i].type == DOT) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2068, tokens[i], "identifier expected");
        }
        type = name;
        name = tokens[i];
        ++i;
    }
    parseFunctionParameters(returntype, args, rparen);
}

std::unique_ptr<Declaration> Parser::parseFunctionDefinition(size_t start_column)
{
    auto &tok_function = tokens[i];
    Token type;
    Token name;
    std::unique_ptr<Type> returntype;
    std::vector<std::unique_ptr<FunctionParameterGroup>> args;
    Token rparen;
    parseFunctionHeader(type, name, returntype, args, rparen);
    TemporaryMinimumIndent indent(this, (start_column > 0 ? start_column : tok_function.column) + 1);
    std::vector<std::unique_ptr<Statement>> body;
    while (tokens[i].type != END) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            body.push_back(std::move(s));
        }
    }
    auto &tok_end_function = tokens[i];
    ++i;
    if (tokens[i].type != FUNCTION) {
        error_a(2036, tokens[i-1], tokens[i], "'FUNCTION' expected");
    }
    ++i;
    return std::unique_ptr<Declaration> { new FunctionDeclaration(tok_function, type, name, std::move(returntype), std::move(args), rparen, std::move(body), tok_end_function) };
}

std::unique_ptr<Declaration> Parser::parseForeignDefinition()
{
    auto &tok_foreign = tokens[i];
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2045, tokens[i], "FUNCTION expected");
    }
    Token type;
    Token name;
    std::unique_ptr<Type> returntype;
    std::vector<std::unique_ptr<FunctionParameterGroup>> args;
    Token rparen;
    parseFunctionHeader(type, name, returntype, args, rparen);
    if (tokens[i].type != LBRACE) {
        error(2046, tokens[i], "{ expected");
    }
    std::unique_ptr<DictionaryLiteralExpression> dict = parseDictionaryLiteral();
    if (tokens[i].type != END) {
        error(2050, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error_a(2051, tokens[i-1], tokens[i], "'END FUNCTION' expected");
    }
    ++i;
    return std::unique_ptr<Declaration> { new ForeignFunctionDeclaration(tok_foreign, type, name, std::move(returntype), std::move(args), rparen, std::move(dict)) };
}

std::unique_ptr<Declaration> Parser::parseDeclaration()
{
    ++i;
    switch (tokens[i].type) {
        case NATIVE: {
            ++i;
            if (tokens[i].type == CONSTANT) {
                auto &tok_constant = tokens[i];
                ++i;
                // The form of the native constant declaration is assumed
                // to be syntactically correct:
                //  DECLARE NATIVE CONSTANT name: type
                Token name = tokens[i];
                ++i;
                if (tokens[i].type != COLON) {
                    internal_error("colon expected");
                }
                ++i;
                std::unique_ptr<Type> type = parseType();
                return std::unique_ptr<Declaration> { new NativeConstantDeclaration(tok_constant, name, std::move(type)) };
            } else if (tokens[i].type == FUNCTION) {
                auto &tok_function = tokens[i];
                Token type;
                Token name;
                std::unique_ptr<Type> returntype;
                std::vector<std::unique_ptr<FunctionParameterGroup>> args;
                Token rparen;
                parseFunctionHeader(type, name, returntype, args, rparen);
                return std::unique_ptr<Declaration> { new NativeFunctionDeclaration(tok_function, std::move(type), name, std::move(returntype), std::move(args), rparen) };
            } else if (tokens[i].type == VAR) {
                auto &tok_var = tokens[i];
                ++i;
                // The form of the native var declaration is assumed
                // to be syntactically correct:
                //  DECLARE NATIVE VAR name: type
                Token name = tokens[i];
                ++i;
                if (tokens[i].type != COLON) {
                    internal_error("colon expected");
                }
                ++i;
                std::unique_ptr<Type> type = parseType();
                return std::unique_ptr<Declaration> { new NativeVariableDeclaration(tok_var, name, std::move(type)) };
            }
            error(2120, tokens[i], "CONSTANT, FUNCTION or VAR expected");
        }
        case EXTENSION: {
            ++i;
            if (tokens[i].type == CONSTANT) {
                auto &tok_constant = tokens[i];
                ++i;
                Token name = tokens[i];
                if (name.type != IDENTIFIER) {
                    error(2126, tokens[i], "identifier expected");
                }
                ++i;
                if (tokens[i].type != COLON) {
                    error(2127, tokens[i], "colon expected");
                }
                ++i;
                std::unique_ptr<Type> type = parseType();
                return std::unique_ptr<Declaration> { new ExtensionConstantDeclaration(tok_constant, name, std::move(type)) };
            } else if (tokens[i].type == FUNCTION) {
                auto &tok_function = tokens[i];
                Token type;
                Token name;
                std::unique_ptr<Type> returntype;
                std::vector<std::unique_ptr<FunctionParameterGroup>> args;
                Token rparen;
                parseFunctionHeader(type, name, returntype, args, rparen);
                return std::unique_ptr<Declaration> { new ExtensionFunctionDeclaration(tok_function, std::move(type), name, std::move(returntype), std::move(args), rparen) };
            /*} else if (tokens[i].type == VAR) {
                auto &tok_var = tokens[i];
                ++i;
                Token name = tokens[i];
                if (name.type != IDENTIFIER) {
                    xrror(2128, "identifier expected");
                }
                ++i;
                if (tokens[i].type != COLON) {
                    xrror(2129, "colon expected");
                }
                ++i;
                std::unique_ptr<Type> type = parseType();
                return std::unique_ptr<Declaration> { new ExtensionVariableDeclaration(tok_var, name, std::move(type)) };*/
            }
            error(2125, tokens[i], "FUNCTION expected");
        }
        default:
            error(2058, tokens[i], "NATIVE expected");
    }
}

std::unique_ptr<Declaration> Parser::parseException()
{
    ++i;
    auto &tok_name = tokens[i];
    std::vector<Token> names;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2059, tokens[i], "identifier expected");
        }
        names.push_back(tokens[i]);
        ++i;
        if (tokens[i].type != DOT) {
            break;
        }
        ++i;
    }
    return std::unique_ptr<Declaration> { new ExceptionDeclaration(tok_name, names) };
}

std::unique_ptr<Declaration> Parser::parseInterface()
{
    i++;
    if (tokens[i].type != IDENTIFIER) {
        error(2128, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    i++;
    std::vector<std::pair<Token, std::unique_ptr<TypeFunctionPointer>>> methods;
    while (tokens[i].type != END) {
        if (tokens[i].type != FUNCTION) {
            error(2129, tokens[i], "FUNCTION expected");
        }
        auto &tok_function = tokens[i];
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2130, tokens[i], "identifier expected");
        }
        const Token &name = tokens[i];
        ++i;
        std::unique_ptr<Type> returntype;
        std::vector<std::unique_ptr<FunctionParameterGroup>> args;
        Token rparen;
        parseFunctionParameters(returntype, args, rparen);
        methods.push_back(std::make_pair(name, std::unique_ptr<TypeFunctionPointer> { new TypeFunctionPointer(tok_function, std::move(returntype), std::move(args)) }));
    }
    i++;
    if (tokens[i].type != INTERFACE) {
        error_a(2131, tokens[i-1], tokens[i], "'INTERFACE' expected");
    }
    i++;
    return std::unique_ptr<Declaration> { new InterfaceDeclaration(tok_name, tok_name, std::move(methods)) };
}

std::unique_ptr<Statement> Parser::parseExport()
{
    auto &tok_export = tokens[i];
    ++i;
    switch (tokens[i].type) {
        case TYPE: {
            std::unique_ptr<Declaration> type = parseTypeDefinition();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, {type->token}, std::move(type)) };
        }
        case CONSTANT: {
            std::unique_ptr<Declaration> constant = parseConstantDefinition();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, constant->names, std::move(constant)) };
        }
        case VAR: {
            std::unique_ptr<Declaration> var = parseVarStatement();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, var->names, std::move(var)) };
        }
        case LET: {
            std::unique_ptr<Declaration> let = parseLetStatement();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, let->names, std::move(let)) };
        }
        case FUNCTION: {
            std::unique_ptr<Declaration> function = parseFunctionDefinition(tok_export.column);
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, function->names, std::move(function)) };
        }
        case FOREIGN: {
            std::unique_ptr<Declaration> foreign = parseForeignDefinition();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, foreign->names, std::move(foreign)) };
        }
        case DECLARE: {
            std::unique_ptr<Declaration> declare = parseDeclaration();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, declare->names, std::move(declare)) };
        }
        case EXCEPTION: {
            std::unique_ptr<Declaration> exception = parseException();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, exception->names, std::move(exception)) };
        }
        case INTERFACE: {
            std::unique_ptr<Declaration> interface = parseInterface();
            return std::unique_ptr<Statement> { new ExportDeclaration(tok_export, interface->names, std::move(interface)) };
        }
        default:
            break;
    }
    if (tokens[i].type != IDENTIFIER) {
        error(2074, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    ++i;
    return std::unique_ptr<Statement> { new ExportDeclaration(tok_name, {tok_name}, nullptr) };
}

std::unique_ptr<Statement> Parser::parseIncrementStatement()
{
    auto &tok_op = tokens[i];
    ++i;
    int delta = tok_op.type == INC ? 1 : tok_op.type == DEC ? -1 : 0;
    std::unique_ptr<Expression> expr = parseExpression();
    return std::unique_ptr<Statement> { new IncrementStatement(tok_op, std::move(expr), delta) };
}

std::unique_ptr<Statement> Parser::parseIfStatement()
{
    auto &tok_if = tokens[i];
    std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::unique_ptr<Statement>>>> condition_statements;
    TemporaryMinimumIndent indent(this, tok_if.column + 1);
    std::vector<std::unique_ptr<Statement>> else_statements;
    do {
        ++i;
        std::unique_ptr<Expression> cond { nullptr };
        if (tokens[i].type == VALID) {
            auto &tok_valid = tokens[i];
            std::vector<std::unique_ptr<ValidPointerExpression::Clause>> tests;
            for (;;) {
                ++i;
                const Token &tok_expr = tokens[i];
                std::unique_ptr<Expression> ptr = parseExpression();
                Token name;
                bool shorthand = false;
                if (tokens[i].type == AS) {
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2064, tokens[i], "identifier expected");
                    }
                    name = tokens[i];
                    ++i;
                } else {
                    const IdentifierExpression *ident = dynamic_cast<const IdentifierExpression *>(ptr.get());
                    if (ident != nullptr) {
                        name = ident->token;
                        shorthand = true;
                    } else {
                        error(2065, tok_expr, "single identifier expected (otherwise use AS alias)");
                    }
                }
                tests.emplace_back(new ValidPointerExpression::Clause(std::move(ptr), name, shorthand));
                if (tokens[i].type != COMMA) {
                    break;
                }
            }
            cond.reset(new ValidPointerExpression(tok_valid, std::move(tests)));
        } else {
            cond = parseExpression();
        }
        if (tokens[i].type != THEN) {
            error(2026, tokens[i], "THEN expected");
        }
        ++i;
        std::vector<std::unique_ptr<Statement>> statements;
        while (tokens[i].type != ELSIF && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            std::unique_ptr<Statement> s = parseStatement();
            if (s != nullptr) {
                statements.push_back(std::move(s));
            }
        }
        condition_statements.push_back(std::make_pair(std::move(cond), std::move(statements)));
    } while (tokens[i].type == ELSIF);
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            std::unique_ptr<Statement> s = parseStatement();
            if (s != nullptr) {
                else_statements.push_back(std::move(s));
            }
        }
    }
    if (tokens[i].type != END) {
        error(2027, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != IF) {
        error_a(2037, tokens[i-1], tokens[i], "IF expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new IfStatement(tok_if, std::move(condition_statements), std::move(else_statements)) };
}

std::unique_ptr<Statement> Parser::parseCheckStatement()
{
    auto &tok_check = tokens[i];
    ++i;
    std::unique_ptr<Expression> cond = parseExpression();
    if (tokens[i].type != ELSE) {
        error_a(2103, tokens[i-1], tokens[i], "ELSE expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_check.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != END) {
        error(2104, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != CHECK) {
        error_a(2105, tokens[i-1], tokens[i], "CHECK expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new CheckStatement(tok_check, std::move(cond), std::move(statements)) };
}

std::unique_ptr<Statement> Parser::parseReturnStatement()
{
    auto &tok_return = tokens[i];
    ++i;
    std::unique_ptr<Expression> expr = parseExpression();
    return std::unique_ptr<Statement> { new ReturnStatement(tok_return, std::move(expr)) };
}

std::unique_ptr<Declaration> Parser::parseVarStatement()
{
    auto &tok_var = tokens[i];
    ++i;
    VariableInfo vars = parseVariableDeclaration();
    std::unique_ptr<Expression> expr = nullptr;
    if (tokens[i].type == ASSIGN) {
        ++i;
        expr = parseExpression();
    }
    return std::unique_ptr<Declaration> { new VariableDeclaration(tok_var, vars.first, std::move(vars.second), std::move(expr)) };
}

std::unique_ptr<Declaration> Parser::parseLetStatement()
{
    auto &tok_let = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2069, tokens[i], "identifier expected");
    }
    const Token &name = tokens[i];
    ++i;
    if (tokens[i].type != COLON) {
        error(2070, tokens[i], "':' expected");
    }
    ++i;
    std::unique_ptr<Type> type = parseType();
    if (tokens[i].type != ASSIGN) {
        error(2071, tokens[i], "':=' expected");
    }
    ++i;
    std::unique_ptr<Expression> expr = parseExpression();
    return std::unique_ptr<Declaration> { new LetDeclaration(tok_let, name, std::move(type), std::move(expr)) };
}

std::unique_ptr<Statement> Parser::parseWhileStatement()
{
    auto &tok_while = tokens[i];
    ++i;
    std::unique_ptr<Expression> cond { nullptr };
    if (tokens[i].type == VALID) {
        auto &tok_valid = tokens[i];
        std::vector<std::unique_ptr<ValidPointerExpression::Clause>> tests;
        for (;;) {
            ++i;
            const Token &tok_expr = tokens[i];
            std::unique_ptr<Expression> ptr = parseExpression();
            Token name;
            bool shorthand = false;
            if (tokens[i].type == AS) {
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2121, tokens[i], "identifier expected");
                }
                name = tokens[i];
                ++i;
            } else {
                const IdentifierExpression *ident = dynamic_cast<const IdentifierExpression *>(ptr.get());
                if (ident != nullptr) {
                    name = ident->token;
                    shorthand = true;
                } else {
                    error(2122, tok_expr, "single identifier expected (otherwise use AS alias)");
                }
            }
            tests.emplace_back(new ValidPointerExpression::Clause(std::move(ptr), name, shorthand));
            if (tokens[i].type != COMMA) {
                break;
            }
        }
        cond.reset(new ValidPointerExpression(tok_valid, std::move(tests)));
    } else {
        cond = parseExpression();
    }
    Token label {WHILE, "WHILE"};
    if (tokens[i].type == LABEL) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2116, tokens[i], "identifier expected");
        }
        label = tokens[i];
        ++i;
    }
    if (tokens[i].type != DO) {
        error_a(2028, tokens[i-1], tokens[i], "DO expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_while.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != END) {
        error(2029, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != WHILE) {
        error_a(2038, tokens[i-1], tokens[i], "WHILE expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new WhileStatement(tok_while, std::move(cond), label, std::move(statements)) };
}

std::unique_ptr<Statement> Parser::parseCaseStatement()
{
    auto &tok_case = tokens[i];
    ++i;
    std::unique_ptr<Expression> expr = parseExpression();
    TemporaryMinimumIndent indent(this, tok_case.column + 1);
    std::vector<std::pair<std::vector<std::unique_ptr<CaseStatement::WhenCondition>>, std::vector<std::unique_ptr<Statement>>>> clauses;
    while (tokens[i].type == WHEN && tokens[i+1].type != OTHERS) {
        auto &tok_when = tokens[i];
        std::vector<std::unique_ptr<CaseStatement::WhenCondition>> conditions;
        do {
            ++i;
            switch (tokens[i].type) {
                case EQUAL:
                case NOTEQUAL:
                case LESS:
                case GREATER:
                case LESSEQ:
                case GREATEREQ: {
                    auto op = tokens[i];
                    ++i;
                    std::unique_ptr<Expression> when = parseExpression();
                    std::unique_ptr<CaseStatement::WhenCondition> cond { new CaseStatement::ComparisonWhenCondition(tok_when, comparisonFromToken(op), std::move(when)) };
                    conditions.push_back(std::move(cond));
                    break;
                }
                case ISA: {
                    auto op = tokens[i];
                    ++i;
                    std::unique_ptr<Type> target = parseType();
                    std::unique_ptr<CaseStatement::WhenCondition> cond { new CaseStatement::TypeTestWhenCondition(tok_when, std::move(target)) };
                    conditions.push_back(std::move(cond));
                    break;
                }
                default: {
                    std::unique_ptr<Expression> when = parseExpression();
                    if (tokens[i].type == TO) {
                        ++i;
                        std::unique_ptr<Expression> when2 = parseExpression();
                        std::unique_ptr<CaseStatement::WhenCondition> cond { new CaseStatement::RangeWhenCondition(tok_when, std::move(when), std::move(when2)) };
                        conditions.push_back(std::move(cond));
                    } else {
                        std::unique_ptr<CaseStatement::WhenCondition> cond { new CaseStatement::ComparisonWhenCondition(tok_when, ComparisonExpression::Comparison::EQ, std::move(when)) };
                        conditions.push_back(std::move(cond));
                    }
                    break;
                }
            }
        } while (tokens[i].type == COMMA);
        if (tokens[i].type != DO) {
            error_a(2030, tokens[i-1], tokens[i], "'DO' expected");
        }
        ++i;
        std::vector<std::unique_ptr<Statement>> statements;
        while (tokens[i].type != WHEN && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            std::unique_ptr<Statement> stmt = parseStatement();
            if (stmt != nullptr) {
                statements.push_back(std::move(stmt));
            }
        }
        clauses.push_back(std::make_pair(std::move(conditions), std::move(statements)));
    }
    std::vector<std::unique_ptr<Statement>> others_statements;
    if (tokens[i].type == WHEN && tokens[i+1].type == OTHERS && tokens[i+2].type == DO) {
        i += 3;
        while (tokens[i].type != END) {
            std::unique_ptr<Statement> stmt = parseStatement();
            if (stmt != nullptr) {
                others_statements.push_back(std::move(stmt));
            }
        }
    }
    if (tokens[i].type != END) {
        error(2031, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != CASE) {
        error_a(2039, tokens[i-1], tokens[i], "CASE expected");
    }
    ++i;
    clauses.push_back(std::make_pair(std::vector<std::unique_ptr<CaseStatement::WhenCondition>>(), std::move(others_statements)));
    return std::unique_ptr<Statement> { new CaseStatement(tok_case, std::move(expr), std::move(clauses)) };
}

std::unique_ptr<Statement> Parser::parseForStatement()
{
    auto &tok_for = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2022, tokens[i], "identifier expected");
    }
    const Token &var = tokens[i];
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2044, tokens[i], "':=' expected");
    }
    ++i;
    std::unique_ptr<Expression> start = parseExpression();
    if (tokens[i].type != TO) {
        error(2040, tokens[i], "TO expected");
    }
    ++i;
    std::unique_ptr<Expression> end = parseExpression();
    std::unique_ptr<Expression> step = nullptr;
    if (tokens[i].type == STEP) {
        ++i;
        step = parseExpression();
    }
    Token label {FOR, "FOR"};
    if (tokens[i].type == LABEL) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2112, tokens[i], "identifier expected");
        }
        label = tokens[i];
        ++i;
    }
    if (tokens[i].type != DO) {
        error_a(2041, tokens[i-1], tokens[i], "'DO' expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_for.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != END) {
        error(2042, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FOR) {
        error_a(2043, tokens[i-1], tokens[i], "'END FOR' expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new ForStatement(tok_for, var, std::move(start), std::move(end), std::move(step), label, std::move(statements)) };
}

std::unique_ptr<Statement> Parser::parseForeachStatement()
{
    auto &tok_foreach = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2081, tokens[i], "identifier expected");
    }
    const Token &var = tokens[i];
    ++i;
    if (tokens[i].type != IN) {
        error(2082, tokens[i], "IN expected");
    }
    ++i;
    std::unique_ptr<Expression> array = parseExpression();
    Token index;
    if (tokens[i].type == INDEX) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2083, tokens[i], "identifier expected");
        }
        index = tokens[i];
        ++i;
    }
    Token label {FOREACH, "FOREACH"};
    if (tokens[i].type == LABEL) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2113, tokens[i], "identifier expected");
        }
        label = tokens[i];
        ++i;
    }
    if (tokens[i].type != DO) {
        error_a(2084, tokens[i-1], tokens[i], "'DO' expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_foreach.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != END) {
        error(2085, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FOREACH) {
        error_a(2086, tokens[i-1], tokens[i], "'END FOREACH' expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new ForeachStatement(tok_foreach, var, std::move(array), index, label, std::move(statements)) };
}

std::unique_ptr<Statement> Parser::parseLoopStatement()
{
    auto &tok_loop = tokens[i];
    ++i;
    Token label {LOOP, "LOOP"};
    if (tokens[i].type == LABEL) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2114, tokens[i], "identifier expected");
        }
        label = tokens[i];
        ++i;
    }
    TemporaryMinimumIndent indent(this, tok_loop.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != END) {
        error(2055, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != LOOP) {
        error_a(2056, tokens[i-1], tokens[i], "LOOP expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new LoopStatement(tok_loop, label, std::move(statements)) };
}

std::unique_ptr<Statement> Parser::parseRepeatStatement()
{
    auto &tok_repeat = tokens[i];
    ++i;
    Token label {REPEAT, "REPEAT"};
    if (tokens[i].type == LABEL) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2115, tokens[i], "identifier expected");
        }
        label = tokens[i];
        ++i;
    }
    TemporaryMinimumIndent indent(this, tok_repeat.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != UNTIL && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != UNTIL) {
        error(2057, tokens[i], "UNTIL expected");
    }
    ++i;
    std::unique_ptr<Expression> cond = parseExpression();
    return std::unique_ptr<Statement> { new RepeatStatement(tok_repeat, label, std::move(cond), std::move(statements)) };
}

std::unique_ptr<Statement> Parser::parseExitStatement()
{
    auto &tok_exit = tokens[i];
    ++i;
    auto &type = tokens[i];
    if (type.type != FUNCTION
     && type.type != WHILE
     && type.type != FOR
     && type.type != FOREACH
     && type.type != LOOP
     && type.type != REPEAT
     && type.type != IDENTIFIER) {
        error_a(2052, tokens[i-1], tokens[i], "loop type or label expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new ExitStatement(tok_exit, type) };
}

std::unique_ptr<Statement> Parser::parseNextStatement()
{
    auto &tok_next = tokens[i];
    ++i;
    auto &type = tokens[i];
    if (type.type != WHILE
     && type.type != FOR
     && type.type != FOREACH
     && type.type != LOOP
     && type.type != REPEAT
     && type.type != IDENTIFIER) {
        error_a(2054, tokens[i-1], tokens[i], "loop type or label expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new NextStatement(tok_next, type) };
}

std::unique_ptr<Statement> Parser::parseTryStatement()
{
    auto &tok_try = tokens[i];
    ++i;
    TemporaryMinimumIndent indent(this, tok_try.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != TRAP && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> stmt = parseStatement();
        if (stmt != nullptr) {
            statements.push_back(std::move(stmt));
        }
    }
    std::vector<std::unique_ptr<TryTrap>> catches;
    while (tokens[i].type == TRAP) {
        ++i;
        std::vector<Token> name;
        for (;;) {
            if (tokens[i].type != IDENTIFIER) {
                error(2060, tokens[i], "identifier expected");
            }
            name.push_back(tokens[i]);
            ++i;
            if (tokens[i].type != DOT) {
                break;
            }
            ++i;
        }
        std::vector<std::vector<Token>> exceptions;
        exceptions.push_back(name);
        Token infoname;
        if (tokens[i].type == AS) {
            ++i;
            if (tokens[i].type != IDENTIFIER) {
                error(2124, tokens[i], "identifier expected");
            }
            infoname = tokens[i];
            ++i;
        }
        if (tokens[i].type != DO) {
            error(2098, tokens[i], "DO expected");
        }
        auto &tok_do = tokens[i];
        ++i;
        std::vector<std::unique_ptr<Statement>> stmts;
        while (tokens[i].type != TRAP && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            std::unique_ptr<Statement> stmt = parseStatement();
            if (stmt != nullptr) {
                stmts.push_back(std::move(stmt));
            }
        }
        catches.emplace_back(new TryTrap(exceptions, infoname, std::unique_ptr<ParseTreeNode> { new TryHandlerStatement(tok_do, std::move(stmts)) }));
    }
    if (tokens[i].type != END) {
        error(2062, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != TRY) {
        error_a(2063, tokens[i-1], tokens[i], "TRY expected");
    }
    ++i;
    return std::unique_ptr<Statement> { new TryStatement(tok_try, std::move(statements), std::move(catches)) };
}

std::unique_ptr<Statement> Parser::parseRaiseStatement()
{
    auto &tok_raise = tokens[i];
    ++i;
    std::vector<Token> name;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2061, tokens[i], "identifier expected");
        }
        name.push_back(tokens[i]);
        ++i;
        if (tokens[i].type != DOT) {
            break;
        }
        ++i;
    }
    std::unique_ptr<Expression> info { nullptr };
    if (tokens[i].type == LPAREN) {
        std::unique_ptr<Expression> expr { new IdentifierExpression(Token(), "ExceptionInfo") };
        info = parseFunctionCall(std::move(expr));
    } else {
        info = nullptr;
    }
    return std::unique_ptr<Statement> { new RaiseStatement(tok_raise, name, std::move(info)) };
}

std::unique_ptr<Statement> Parser::parseExecStatement()
{
    auto &tok_exec = tokens[i];
    ++i;
    if (tokens[i].type != STRING) {
        internal_error("not a string");
    }
    auto &tok_text = tokens[i];
    std::string text = tokens[i].text;
    ++i;
    std::unique_ptr<SqlStatementInfo> info = parseSqlStatement(tok_text, text);
    return std::unique_ptr<Statement> { new ExecStatement(tok_exec, text, std::move(info)) };
}

std::unique_ptr<Statement> Parser::parseUnusedStatement()
{
    auto &tok_unused = tokens[i];
    ++i;
    std::vector<Token> vars;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2134, tokens[i], "identifier expected");
        }
        vars.push_back(tokens[i]);
        ++i;
        if (tokens[i].type != COMMA) {
            break;
        }
        ++i;
    }
    return std::unique_ptr<Statement> { new UnusedStatement(tok_unused, vars) };
}

std::unique_ptr<Statement> Parser::parseImport()
{
    auto &tok_import = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER && tokens[i].type != STRING) {
        error(2032, tokens[i], "identifier or string expected");
    }
    const Token &module = tokens[i];
    ++i;
    Token name;
    if (tokens[i].type == DOT) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2079, tokens[i], "identifier expected");
        }
        name = tokens[i];
        ++i;
    }
    Token alias;
    if (tokens[i].type == ALIAS) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2080, tokens[i], "identifier expected");
        }
        alias = tokens[i];
        ++i;
    } else if (module.type == STRING) {
        error(2087, module, "named import requires ALIAS");
    }
    return std::unique_ptr<Statement> { new ImportDeclaration(tok_import, module, name, alias) };
}

std::unique_ptr<Statement> Parser::parseAssert()
{
    auto &tok_assert = tokens[i];
    ++i;
    std::unique_ptr<Expression> e = parseExpression();
    std::vector<std::unique_ptr<Expression>> exprs;
    exprs.push_back(std::move(e));
    while (tokens[i].type == COMMA) {
        ++i;
        e = parseExpression();
        exprs.push_back(std::move(e));
    }
    return std::unique_ptr<Statement> { new AssertStatement(tok_assert, std::move(exprs), tok_assert.source_line()) };
}

std::unique_ptr<Statement> Parser::parseBegin()
{
    auto &tok_begin = tokens[i];
    ++i;
    if (tokens[i].type != MAIN) {
        error(2091, tokens[i], "'MAIN' expected");
    }
    auto &tok_main = tokens[i];
    ++i;
    TemporaryMinimumIndent indent(this, tok_begin.column + 1);
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    if (tokens[i].type != END) {
        error(2092, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != MAIN) {
        error_a(2093, tokens[i-1], tokens[i], "'MAIN' expected");
    }
    ++i;
    return std::unique_ptr<pt::FunctionDeclaration> { new FunctionDeclaration(tok_begin, Token(), tok_main, nullptr, {}, Token(), std::move(statements), Token()) };
}

std::unique_ptr<Statement> Parser::parseStatement()
{
    if (tokens[i].column < minimum_column) {
        error(2089, tokens[i], "indent must be at least column " + std::to_string(minimum_column));
    }
    if (tokens[i].type == IMPORT) {
        return parseImport();
    } else if (tokens[i].type == TYPE) {
        return parseTypeDefinition();
    } else if (tokens[i].type == CONSTANT) {
        return parseConstantDefinition();
    } else if (tokens[i].type == FUNCTION) {
        return parseFunctionDefinition(0);
    } else if (tokens[i].type == FOREIGN) {
        return parseForeignDefinition();
    } else if (tokens[i].type == DECLARE) {
        return parseDeclaration();
    } else if (tokens[i].type == EXCEPTION) {
        return parseException();
    } else if (tokens[i].type == INTERFACE) {
        return parseInterface();
    } else if (tokens[i].type == EXPORT) {
        return parseExport();
    } else if (tokens[i].type == INC || tokens[i].type == DEC) {
        return parseIncrementStatement();
    } else if (tokens[i].type == IF) {
        return parseIfStatement();
    } else if (tokens[i].type == RETURN) {
        return parseReturnStatement();
    } else if (tokens[i].type == VAR) {
        return parseVarStatement();
    } else if (tokens[i].type == LET) {
        return parseLetStatement();
    } else if (tokens[i].type == WHILE) {
        return parseWhileStatement();
    } else if (tokens[i].type == CASE) {
        return parseCaseStatement();
    } else if (tokens[i].type == FOR) {
        return parseForStatement();
    } else if (tokens[i].type == FOREACH) {
        return parseForeachStatement();
    } else if (tokens[i].type == LOOP) {
        return parseLoopStatement();
    } else if (tokens[i].type == REPEAT) {
        return parseRepeatStatement();
    } else if (tokens[i].type == EXIT) {
        return parseExitStatement();
    } else if (tokens[i].type == NEXT) {
        return parseNextStatement();
    } else if (tokens[i].type == TRY) {
        return parseTryStatement();
    } else if (tokens[i].type == RAISE) {
        return parseRaiseStatement();
    } else if (tokens[i].type == ASSERT) {
        return parseAssert();
    } else if (tokens[i].type == BEGIN) {
        return parseBegin();
    } else if (tokens[i].type == CHECK) {
        return parseCheckStatement();
    } else if (tokens[i].type == EXEC) {
        return parseExecStatement();
    } else if (tokens[i].type == UNUSED) {
        return parseUnusedStatement();
    } else if (tokens[i].type == IDENTIFIER) {
        const Token &start = tokens[i];
        if (tokens[i+1].type != ASSIGN
         && tokens[i+1].type != EQUAL // Note that this is an error, but error 3060 is better.
         && tokens[i+1].type != LBRACKET
         && tokens[i+1].type != LPAREN
         && tokens[i+1].type != DOT
         && tokens[i+1].type != ARROW) {
            error(2096, tokens[i], "plain identifier cannot be a statement");
        }
        std::unique_ptr<Expression> expr = parseExpression();
        if (tokens[i].type == ASSIGN) {
            auto &tok_assign = tokens[i];
            ++i;
            std::unique_ptr<Expression> rhs = parseExpression();
            std::vector<std::unique_ptr<Expression>> vars;
            vars.push_back(std::move(expr));
            return std::unique_ptr<Statement> { new AssignmentStatement(tok_assign, std::move(vars), std::move(rhs)) };
        } else {
            return std::unique_ptr<Statement> { new ExpressionStatement(start, std::move(expr)) };
        }
    } else if (tokens[i].type == UNDERSCORE) {
        auto &tok_underscore = tokens[i];
        ++i;
        if (tokens[i].type != ASSIGN) {
            error(2097, tokens[i], ":= expected");
        }
        auto &tok_assign = tokens[i];
        ++i;
        std::unique_ptr<Expression> rhs = parseExpression();
        std::vector<std::unique_ptr<Expression>> vars;
        vars.emplace_back(new DummyExpression(tok_underscore, tok_underscore.column, tok_underscore.column));
        return std::unique_ptr<Statement> { new AssignmentStatement(tok_assign, std::move(vars), std::move(rhs)) };
    } else {
        error(2033, tokens[i], "Identifier expected");
    }
}

std::unique_ptr<Program> Parser::parse()
{
    auto &tok_program = tokens[i];
    std::vector<std::unique_ptr<Statement>> statements;
    while (tokens[i].type != END_OF_FILE) {
        std::unique_ptr<Statement> s = parseStatement();
        if (s != nullptr) {
            statements.push_back(std::move(s));
        }
    }
    return std::unique_ptr<Program> { new Program(tok_program, std::move(statements), source.source_path, source.source_hash) };
}

std::unique_ptr<Program> parse(const TokenizedSource &tokens)
{
    return Parser(tokens).parse();
}

std::unique_ptr<pt::Expression> parseExpression(const TokenizedSource &tokens)
{
    return Parser(tokens).parseExpression();
}
