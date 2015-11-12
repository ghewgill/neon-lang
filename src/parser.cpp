#include "parser.h"

#include <iso646.h>
#include <list>
#include <set>
#include <stack>

#include "format.h"
#include "pt.h"
#include "util.h"

using namespace pt;

class Parser {
public:
    Parser(const TokenizedSource &tokens);

    TokenizedSource source;
    const std::vector<Token> tokens;
    std::vector<Token>::size_type i;
    int expression_depth;
    size_t minimum_column;

    typedef std::pair<std::vector<Token>, const Type *> VariableInfo;

    const Type *parseParameterisedType();
    const Type *parseRecordType();
    const Type *parseEnumType();
    const Type *parsePointerType();
    const Type *parseFunctionType();
    const Type *parseType();
    const Statement *parseTypeDefinition();
    const Statement *parseConstantDefinition();
    const FunctionCallExpression *parseFunctionCall(const Expression *func);
    const ArrayLiteralExpression *parseArrayLiteral();
    const DictionaryLiteralExpression *parseDictionaryLiteral();
    const Expression *parseInterpolatedStringExpression();
    const Expression *parseAtom();
    const Expression *parseExponentiation();
    const Expression *parseMultiplication();
    const Expression *parseAddition();
    const Expression *parseComparison();
    const Expression *parseMembership();
    const Expression *parseConjunction();
    const Expression *parseDisjunction();
    const Expression *parseConditional();
    const Expression *parseExpression();
    const VariableInfo parseVariableDeclaration();
    void parseFunctionParameters(const Type *&returntype, std::vector<const FunctionParameter *> &args, Token &rparen);
    void parseFunctionHeader(Token &type, Token &name, const Type *&returntype, std::vector<const FunctionParameter *> &args, Token &rparen);
    const Statement *parseFunctionDefinition();
    const Statement *parseExternalDefinition();
    const Statement *parseDeclaration();
    const Statement *parseExport();
    const Statement *parseIfStatement();
    const Statement *parseReturnStatement();
    const Statement *parseVarStatement();
    const Statement *parseLetStatement();
    const Statement *parseWhileStatement();
    const Statement *parseCaseStatement();
    const Statement *parseForStatement();
    const Statement *parseForeachStatement();
    const Statement *parseLoopStatement();
    const Statement *parseRepeatStatement();
    const Statement *parseExitStatement();
    const Statement *parseNextStatement();
    const Statement *parseTryStatement();
    const Statement *parseRaiseStatement();
    const Statement *parseImport();
    const Statement *parseAssert();
    const Statement *parseBegin();
    const Statement *parseStatement();
    const Program *parse();
private:
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
        case EQUAL:     return ComparisonExpression::EQ;
        case NOTEQUAL:  return ComparisonExpression::NE;
        case LESS:      return ComparisonExpression::LT;
        case GREATER:   return ComparisonExpression::GT;
        case LESSEQ:    return ComparisonExpression::LE;
        case GREATEREQ: return ComparisonExpression::GE;
        default:
            internal_error("invalid comparison type");
    }
}

const Type *Parser::parseParameterisedType()
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
    const Type *elementtype = parseType();
    if (tokens[i].type != GREATER) {
        error2(2003, tokens[i], "'>' expected", tok_less, "opening '<' here");
    }
    i++;
    return new TypeParameterised(tok_type, elementtype);
}

const Type *Parser::parseRecordType()
{
    if (tokens[i].type != RECORD) {
        internal_error("RECORD expected");
    }
    auto &tok_record = tokens[i];
    i++;
    std::vector<TypeRecord::Field> fields;
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
        const Type *t = parseType();
        fields.push_back(TypeRecord::Field(name, t, is_private));
    }
    i++;
    if (tokens[i].type != RECORD) {
        error_a(2034, tokens[i-1], tokens[i], "'RECORD' expected");
    }
    i++;
    return new TypeRecord(tok_record, fields);
}

const Type *Parser::parseEnumType()
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
    return new TypeEnum(tok_enum, names);
}

const Type *Parser::parsePointerType()
{
    if (tokens[i].type != POINTER) {
        internal_error("POINTER expected");
    }
    auto &tok_pointer = tokens[i];
    i++;
    if (tokens[i].type == TO) {
        i++;
        const Type *reftype = parseType();
        return new TypePointer(tok_pointer, reftype);
    } else {
        return new TypePointer(tok_pointer, nullptr);
    }
}

const Type *Parser::parseFunctionType()
{
    if (tokens[i].type != FUNCTION) {
        internal_error("FUNCTION expected");
    }
    auto &tok_function = tokens[i];
    i++;
    const Type *returntype;
    std::vector<const FunctionParameter *> args;
    Token rparen;
    parseFunctionParameters(returntype, args, rparen);
    return new TypeFunctionPointer(tok_function, returntype, args);
}

const Type *Parser::parseType()
{
    if (tokens[i].type == ARRAY || tokens[i].type == DICTIONARY) {
        return parseParameterisedType();
    }
    if (tokens[i].type == RECORD) {
        return parseRecordType();
    }
    if (tokens[i].type == ENUM) {
        return parseEnumType();
    }
    if (tokens[i].type == POINTER) {
        return parsePointerType();
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
        return new TypeImport(name, name, subname);
    } else {
        return new TypeSimple(name, name.text);
    }
}

const Statement *Parser::parseTypeDefinition()
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
    const Type *type = parseType();
    return new TypeDeclaration(tok_name, type);
}

const Statement *Parser::parseConstantDefinition()
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
    const Type *type = parseType();
    if (tokens[i].type != ASSIGN) {
        error(2012, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *value = parseExpression();
    return new ConstantDeclaration(tok_name, tok_name, type, value);
}

const FunctionCallExpression *Parser::parseFunctionCall(const Expression *func)
{
    ++i;
    std::vector<FunctionCallExpression::Argument> args;
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
            if (tokens[i].type == IDENTIFIER && tokens[i+1].type == AS) {
                name = tokens[i];
                i += 2;
            }
            const Expression *e = parseExpression();
            args.push_back(FunctionCallExpression::Argument(mode, name, e));
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
    return new FunctionCallExpression(func->token, func, args, tok_rparen);
}

const ArrayLiteralExpression *Parser::parseArrayLiteral()
{
    auto &tok_lbracket = tokens[i];
    ++i;
    std::vector<const Expression *> elements;
    while (tokens[i].type != RBRACKET) {
        const Expression *element = parseExpression();
        elements.push_back(element);
        if (tokens[i].type == COMMA) {
            ++i;
        } else if (tokens[i].type != RBRACKET) {
            error2(2053, tokens[i], "',' or ']' expected", tok_lbracket, "opening '[' here");
        }
    }
    ++i;
    return new ArrayLiteralExpression(tok_lbracket, tokens[i-1].column+1, elements);
}

const DictionaryLiteralExpression *Parser::parseDictionaryLiteral()
{
    auto &tok_lbrace = tokens[i];
    ++i;
    std::vector<std::pair<Token, const Expression *>> elements;
    while (tokens[i].type == STRING) {
        auto &key = tokens[i];
        ++i;
        if (tokens[i].type != COLON) {
            error(2048, tokens[i], "':' expected");
        }
        ++i;
        const Expression *element = parseExpression();
        elements.push_back(std::make_pair(key, element));
        if (tokens[i].type == COMMA) {
            ++i;
        }
    }
    if (tokens[i].type != RBRACE) {
        error2(2049, tokens[i], "'}' expected", tok_lbrace, "opening '{' here");
    }
    ++i;
    return new DictionaryLiteralExpression(tok_lbrace, tokens[i-1].column+1, elements);
}

const Expression *Parser::parseInterpolatedStringExpression()
{
    std::vector<std::pair<const Expression *, Token>> parts;
    parts.push_back(std::make_pair(new StringLiteralExpression(tokens[i], tokens[i+1].column, tokens[i].text), Token()));
    for (;;) {
        ++i;
        if (tokens[i].type != SUBBEGIN) {
            break;
        }
        ++i;
        const Expression *e = parseExpression();
        Token fmt;
        if (tokens[i].type == SUBFMT) {
            ++i;
            if (tokens[i].type != STRING) {
                internal_error("parseInterpolatedStringExpression");
            }
            fmt = tokens[i];
            ++i;
        }
        parts.push_back(std::make_pair(e, fmt));
        if (tokens[i].type != SUBEND) {
            internal_error("parseInterpolatedStringExpression");
        }
        ++i;
        if (tokens[i].type != STRING) {
            internal_error("parseInterpolatedStringExpression");
        }
        e = new StringLiteralExpression(tokens[i], tokens[i+1].column, tokens[i].text);
        parts.push_back(std::make_pair(e, Token()));
    }
    return new InterpolatedStringExpression(parts[0].first->token, parts);
}

/*
 * Operator precedence:
 *
 *  ^        exponentiation                     parseExponentiation
 *  * / MOD  multiplication, division, modulo   parseMultiplication
 *  + -      addition, subtraction              parseAddition
 *  < = >    comparison                         parseComparison
 *  in       membership                         parseMembership
 *  and      conjunction                        parseConjunction
 *  or       disjunction                        parseDisjunction
 *  if       conditional                        parseConditional
 */

const Expression *Parser::parseAtom()
{
    switch (tokens[i].type) {
        case LPAREN: {
            auto &tok_lparen = tokens[i];
            ++i;
            const Expression *expr = parseExpression();
            if (tokens[i].type != RPAREN) {
                error2(2014, tokens[i], ") expected", tok_lparen, "opening '(' here");
            }
            ++i;
            return new IdentityExpression(tok_lparen, tok_lparen.column, tokens[i-1].column+1, expr);
        }
        case LBRACKET: {
            const Expression *array = parseArrayLiteral();
            return array;
        }
        case LBRACE: {
            const Expression *dict = parseDictionaryLiteral();
            return dict;
        }
        case FALSE: {
            auto &tok_false = tokens[i];
            ++i;
            return new BooleanLiteralExpression(tok_false, false);
        }
        case TRUE: {
            auto &tok_true = tokens[i];
            ++i;
            return new BooleanLiteralExpression(tok_true, true);
        }
        case NUMBER: {
            auto &tok_number = tokens[i];
            i++;
            return new NumberLiteralExpression(tok_number, tok_number.value);
        }
        case STRING: {
            if (tokens[i+1].type == SUBBEGIN) {
                return parseInterpolatedStringExpression();
            } else {
                auto &tok_string = tokens[i];
                i++;
                return new StringLiteralExpression(tok_string, tokens[i].column, tok_string.text);
            }
        }
        case EMBED: {
            ++i;
            auto &tok_file = tokens[i];
            ++i;
            if (tok_file.type != STRING) {
                error(2090, tok_file, "string literal expected");
            }
            return new FileLiteralExpression(tok_file, tokens[i].column, tok_file.text);
        }
        case HEXBYTES: {
            ++i;
            auto &tok_literal = tokens[i];
            ++i;
            if (tok_literal.type != STRING) {
                error(2094, tok_literal, "string literal expected");
            }
            return new BytesLiteralExpression(tok_literal, tokens[i].column, tok_literal.text);
        }
        case PLUS: {
            auto &tok_plus = tokens[i];
            ++i;
            const Expression *atom = parseAtom();
            return new UnaryPlusExpression(tok_plus, atom);
        }
        case MINUS: {
            auto &tok_minus = tokens[i];
            ++i;
            const Expression *atom = parseAtom();
            return new UnaryMinusExpression(tok_minus, atom);
        }
        case NOT: {
            auto &tok_not = tokens[i];
            ++i;
            const Expression *atom = parseAtom();
            return new LogicalNotExpression(tok_not, atom);
        }
        case NEW: {
            auto &tok_new = tokens[i];
            ++i;
            const Type *type = parseType();
            return new NewRecordExpression(tok_new, tokens[i].column, type);
        }
        case NIL: {
            auto &tok_nil = tokens[i];
            ++i;
            return new NilLiteralExpression(tok_nil);
        }
        case IDENTIFIER: {
            const Expression *expr = new IdentifierExpression(tokens[i], tokens[i].text);
            ++i;
            for (;;) {
                if (tokens[i].type == LBRACKET) {
                    auto &tok_lbracket = tokens[i];
                    ++i;
                    const Expression *index = nullptr;
                    bool first_from_end = false;
                    if (tokens[i].type == FIRST || tokens[i].type == LAST) {
                        first_from_end = tokens[i].type == LAST;
                        ++i;
                        if (tokens[i].type == RBRACKET || tokens[i].type == TO) {
                            index = new NumberLiteralExpression(Token(), number_from_uint32(0));
                        } else if (tokens[i].type != PLUS && tokens[i].type != MINUS) {
                            error(2072, tokens[i], "'+' or '-' expected");
                        }
                    }
                    if (index == nullptr) {
                        index = parseExpression();
                    }
                    const ArrayRange *range = nullptr;
                    const Expression *last = nullptr;
                    if (tokens[i].type == TO) {
                        ++i;
                        last = nullptr;
                        bool last_from_end = false;
                        if (tokens[i].type == FIRST || tokens[i].type == LAST) {
                            last_from_end = tokens[i].type == LAST;
                            ++i;
                            if (tokens[i].type == RBRACKET) {
                                last = new NumberLiteralExpression(Token(), number_from_uint32(0));
                            } else if (tokens[i].type != PLUS && tokens[i].type != MINUS) {
                                error(2073, tokens[i], "'+' or '-' expected");
                            }
                        }
                        if (last == nullptr) {
                            last = parseExpression();
                        }
                        range = new ArrayRange(tok_lbracket, index, first_from_end, last, last_from_end);
                    } else {
                        if (first_from_end) {
                            range = new ArrayRange(tok_lbracket, index, first_from_end, index, first_from_end);
                        }
                    }
                    if (tokens[i].type != RBRACKET) {
                        error(2020, tokens[i], "']' expected");
                    }
                    ++i;
                    if (range != nullptr) {
                        expr = new RangeSubscriptExpression(tok_lbracket, tokens[i].column, expr, range);
                        if (first_from_end && last == nullptr) {
                            expr = new SubscriptExpression(tok_lbracket, tokens[i].column, expr, new NumberLiteralExpression(Token(), number_from_uint32(0)));
                        }
                    } else {
                        expr = new SubscriptExpression(tok_lbracket, tokens[i].column, expr, index);
                    }
                } else if (tokens[i].type == LPAREN) {
                    expr = parseFunctionCall(expr);
                } else if (tokens[i].type == DOT) {
                    auto &tok_dot = tokens[i];
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2021, tokens[i], "identifier expected");
                    }
                    const Token &field = tokens[i];
                    ++i;
                    expr = new DotExpression(tok_dot, expr, field);
                } else if (tokens[i].type == ARROW) {
                    auto &tok_arrow = tokens[i];
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2066, tokens[i], "identifier expected");
                    }
                    const Token &field = tokens[i];
                    ++i;
                    expr = new ArrowExpression(tok_arrow, expr, field);
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

const Expression *Parser::parseExponentiation()
{
    const Expression *left = parseAtom();
    for (;;) {
        auto &tok_op = tokens[i];
        if (tokens[i].type == EXP) {
            ++i;
            const Expression *right = parseAtom();
            left = new ExponentiationExpression(tok_op, left, right);
        } else {
            return left;
        }
    }
}

const Expression *Parser::parseMultiplication()
{
    const Expression *left = parseExponentiation();
    for (;;) {
        auto &tok_op = tokens[i];
        switch (tokens[i].type) {
            case TIMES: {
                ++i;
                const Expression *right = parseExponentiation();
                left = new MultiplicationExpression(tok_op, left, right);
                break;
            }
            case DIVIDE: {
                ++i;
                const Expression *right = parseExponentiation();
                left = new DivisionExpression(tok_op, left, right);
                break;
            }
            case MOD: {
                ++i;
                const Expression *right = parseExponentiation();
                left = new ModuloExpression(tok_op, left, right);
                break;
            }
            default:
                return left;
        }
    }
}

const Expression *Parser::parseAddition()
{
    const Expression *left = parseMultiplication();
    for (;;) {
        auto &tok_op = tokens[i];
        switch (tokens[i].type) {
            case PLUS: {
                ++i;
                const Expression *right = parseMultiplication();
                left = new AdditionExpression(tok_op, left, right);
                break;
            }
            case MINUS: {
                ++i;
                const Expression *right = parseMultiplication();
                left = new SubtractionExpression(tok_op, left, right);
                break;
            }
            case CONCAT: {
                ++i;
                const Expression *right = parseMultiplication();
                left = new ConcatenationExpression(tok_op, left, right);
                break;
            }
            default:
                return left;
        }
    }
}

const Expression *Parser::parseComparison()
{
    const Expression *left = parseAddition();
    std::vector<const ComparisonExpression *> comps;
    while (tokens[i].type == EQUAL  || tokens[i].type == NOTEQUAL
        || tokens[i].type == LESS   || tokens[i].type == GREATER
        || tokens[i].type == LESSEQ || tokens[i].type == GREATEREQ) {
        auto &tok_comp = tokens[i];
        if (tok_comp.type == EQUAL && tokens[i+1].type == EQUAL) {
            error(2078, tok_comp, "'==' not expected, use '=' for comparison");
        }
        ComparisonExpression::Comparison comp = comparisonFromToken(tok_comp);
        ++i;
        const Expression *right = parseAddition();
        const ComparisonExpression *c = new ComparisonExpression(tok_comp, left, right, comp);
        comps.push_back(c);
        left = right;
    }
    if (comps.empty()) {
        return left;
    } else if (comps.size() == 1) {
        return comps[0];
    } else {
        return new ChainedComparisonExpression(comps);
    }
}

const Expression *Parser::parseMembership()
{
    const Expression *left = parseComparison();
    if (tokens[i].type == IN || (tokens[i].type == NOT && tokens[i+1].type == IN)) {
        auto &tok_op = tokens[i];
        bool notin = tokens[i].type == NOT;
        if (notin) {
            ++i;
        }
        ++i;
        const Expression *right = parseComparison();
        const Expression *r = new MembershipExpression(tok_op, left, right);
        if (notin) {
            r = new LogicalNotExpression(tok_op, r);
        }
        return r;
    } else {
        return left;
    }
}

const Expression *Parser::parseConjunction()
{
    const Expression *left = parseMembership();
    for (;;) {
        auto &tok_op = tokens[i];
        if (tokens[i].type == AND) {
            ++i;
            const Expression *right = parseMembership();
            left = new ConjunctionExpression(tok_op, left, right);
        } else {
            return left;
        }
    }
}

const Expression *Parser::parseDisjunction()
{
    const Expression *left = parseConjunction();
    for (;;) {
        auto &tok_op = tokens[i];
        if (tokens[i].type == OR) {
            ++i;
            const Expression *right = parseConjunction();
            left = new DisjunctionExpression(tok_op, left, right);
        } else {
            return left;
        }
    }
}

const Expression *Parser::parseConditional()
{
    if (tokens[i].type == IF) {
        auto &tok_if = tokens[i];
        ++i;
        const Expression *cond = parseExpression();
        if (tokens[i].type != THEN) {
            error(2016, tokens[i], "'THEN' expected");
        }
        ++i;
        const Expression *left = parseExpression();
        if (tokens[i].type != ELSE) {
            error(2017, tokens[i], "'ELSE' expected");
        }
        ++i;
        const Expression *right = parseExpression();
        return new ConditionalExpression(tok_if, cond, left, right);
    } else {
        return parseDisjunction();
    }
}

const Expression *Parser::parseExpression()
{
    expression_depth++;
    if (expression_depth > 100) {
        error(2067, tokens[i], "exceeded maximum nesting depth");
    }
    const Expression *r = parseConditional();
    expression_depth--;
    return r;
}

const Parser::VariableInfo Parser::parseVariableDeclaration()
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
    const Type *t = parseType();
    return make_pair(names, t);
}

void Parser::parseFunctionParameters(const Type *&returntype, std::vector<const FunctionParameter *> &args, Token &rparen)
{
    if (tokens[i].type != LPAREN) {
        error(2024, tokens[i], "'(' expected");
    }
    ++i;
    if (tokens[i].type != RPAREN) {
        for (;;) {
            FunctionParameter::Mode mode = FunctionParameter::IN;
            switch (tokens[i].type) {
                case IN:    mode = FunctionParameter::IN;       i++; break;
                case INOUT: mode = FunctionParameter::INOUT;    i++; break;
                case OUT:   mode = FunctionParameter::OUT;      i++; break;
                default:
                    break;
            }
            auto &tok_param = tokens[i];
            const VariableInfo vars = parseVariableDeclaration();
            const Expression *default_value = nullptr;
            if (tokens[i].type == DEFAULT) {
                ++i;
                default_value = parseExpression();
            }
            for (auto name: vars.first) {
                FunctionParameter *fp = new FunctionParameter(tok_param, name, vars.second, mode, default_value);
                args.push_back(fp);
            }
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

void Parser::parseFunctionHeader(Token &type, Token &name, const Type *&returntype, std::vector<const FunctionParameter *> &args, Token &rparen)
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

const Statement *Parser::parseFunctionDefinition()
{
    auto &tok_function = tokens[i];
    Token type;
    Token name;
    const Type *returntype;
    std::vector<const FunctionParameter *> args;
    Token rparen;
    parseFunctionHeader(type, name, returntype, args, rparen);
    TemporaryMinimumIndent indent(this, tok_function.column + 1);
    std::vector<const Statement *> body;
    while (tokens[i].type != END) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            body.push_back(s);
        }
    }
    auto &tok_end_function = tokens[i];
    ++i;
    if (tokens[i].type != FUNCTION) {
        error_a(2036, tokens[i-1], tokens[i], "'FUNCTION' expected");
    }
    ++i;
    return new FunctionDeclaration(tok_function, type, name, returntype, args, rparen, body, tok_end_function);
}

const Statement *Parser::parseExternalDefinition()
{
    auto &tok_external = tokens[i];
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2045, tokens[i], "FUNCTION expected");
    }
    Token type;
    Token name;
    const Type *returntype;
    std::vector<const FunctionParameter *> args;
    Token rparen;
    parseFunctionHeader(type, name, returntype, args, rparen);
    if (tokens[i].type != LBRACE) {
        error(2046, tokens[i], "{ expected");
    }
    const DictionaryLiteralExpression *dict = parseDictionaryLiteral();
    if (tokens[i].type != END) {
        error(2050, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error_a(2051, tokens[i-1], tokens[i], "'END FUNCTION' expected");
    }
    ++i;
    return new ExternalFunctionDeclaration(tok_external, type, name, returntype, args, rparen, dict);
}

const Statement *Parser::parseDeclaration()
{
    ++i;
    switch (tokens[i].type) {
        case EXCEPTION: {
            ++i;
            auto &tok_name = tokens[i];
            if (tokens[i].type != IDENTIFIER) {
                error(2059, tokens[i], "identifier expected");
            }
            ++i;
            return new ExceptionDeclaration(tok_name, tok_name);
        }
        case NATIVE: {
            ++i;
            if (tokens[i].type == CONSTANT) {
                ++i;
                // This just skips over the (assumed syntactically correct)
                // identifier, colon, and type declaration of a constant.
                ++i;
                ++i;
                parseType();
            } else if (tokens[i].type == FUNCTION) {
                auto &tok_function = tokens[i];
                Token type;
                Token name;
                const Type *returntype;
                std::vector<const FunctionParameter *> args;
                Token rparen;
                parseFunctionHeader(type, name, returntype, args, rparen);
                return new NativeFunctionDeclaration(tok_function, type, name, returntype, args, rparen);
            }
            return nullptr;
        }
        default:
            error(2058, tokens[i], "EXCEPTION expected");
    }
}

const Statement *Parser::parseExport()
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2074, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    ++i;
    return new ExportDeclaration(tok_name, tok_name);
}

const Statement *Parser::parseIfStatement()
{
    auto &tok_if = tokens[i];
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    TemporaryMinimumIndent indent(this, tok_if.column + 1);
    std::vector<const Statement *> else_statements;
    do {
        ++i;
        const Expression *cond = nullptr;
        if (tokens[i].type == VALID) {
            auto &tok_valid = tokens[i];
            std::vector<ValidPointerExpression::Clause> tests;
            for (;;) {
                ++i;
                const Token &tok_expr = tokens[i];
                const Expression *ptr = parseExpression();
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
                    const IdentifierExpression *ident = dynamic_cast<const IdentifierExpression *>(ptr);
                    if (ident != nullptr) {
                        name = ident->token;
                        shorthand = true;
                    } else {
                        error(2065, tok_expr, "single identifier expected (otherwise use AS alias)");
                    }
                }
                tests.push_back(ValidPointerExpression::Clause(ptr, name, shorthand));
                if (tokens[i].type != COMMA) {
                    break;
                }
            }
            cond = new ValidPointerExpression(tok_valid, tests);
        } else {
            cond = parseExpression();
        }
        if (tokens[i].type != THEN) {
            error(2026, tokens[i], "THEN expected");
        }
        ++i;
        TemporaryMinimumIndent indent(this, tok_if.column + 1);
        std::vector<const Statement *> statements;
        while (tokens[i].type != ELSIF && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement();
            if (s != nullptr) {
                statements.push_back(s);
            }
        }
        condition_statements.push_back(std::make_pair(cond, statements));
    } while (tokens[i].type == ELSIF);
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement();
            if (s != nullptr) {
                else_statements.push_back(s);
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
    return new IfStatement(tok_if, condition_statements, else_statements);
}

const Statement *Parser::parseReturnStatement()
{
    auto &tok_return = tokens[i];
    ++i;
    const Expression *expr = parseExpression();
    return new ReturnStatement(tok_return, expr);
}

const Statement *Parser::parseVarStatement()
{
    auto &tok_var = tokens[i];
    ++i;
    const VariableInfo vars = parseVariableDeclaration();
    const Expression *expr = nullptr;
    if (tokens[i].type == ASSIGN) {
        ++i;
        expr = parseExpression();
    }
    return new VariableDeclaration(tok_var, vars.first, vars.second, expr);
}

const Statement *Parser::parseLetStatement()
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
    const Type *type = parseType();
    if (tokens[i].type != ASSIGN) {
        error(2071, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *expr = parseExpression();
    return new LetDeclaration(tok_let, name, type, expr);
}

const Statement *Parser::parseWhileStatement()
{
    auto &tok_while = tokens[i];
    ++i;
    const Expression *cond = parseExpression();
    if (tokens[i].type != DO) {
        error_a(2028, tokens[i-1], tokens[i], "DO expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_while.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
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
    return new WhileStatement(tok_while, cond, statements);
}

const Statement *Parser::parseCaseStatement()
{
    auto &tok_case = tokens[i];
    ++i;
    const Expression *expr = parseExpression();
    TemporaryMinimumIndent indent(this, tok_case.column + 1);
    std::vector<std::pair<std::vector<const CaseStatement::WhenCondition *>, std::vector<const Statement *>>> clauses;
    while (tokens[i].type == WHEN) {
        auto &tok_when = tokens[i];
        std::vector<const CaseStatement::WhenCondition *> conditions;
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
                    const Expression *when = parseExpression();
                    const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(tok_when, comparisonFromToken(op), when);
                    conditions.push_back(cond);
                    break;
                }
                default: {
                    const Expression *when = parseExpression();
                    if (tokens[i].type == TO) {
                        ++i;
                        const Expression *when2 = parseExpression();
                        const CaseStatement::WhenCondition *cond = new CaseStatement::RangeWhenCondition(tok_when, when, when2);
                        conditions.push_back(cond);
                    } else {
                        const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(tok_when, ComparisonExpression::EQ, when);
                        conditions.push_back(cond);
                    }
                    break;
                }
            }
        } while (tokens[i].type == COMMA);
        if (tokens[i].type != DO) {
            error_a(2030, tokens[i-1], tokens[i], "'DO' expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != WHEN && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *stmt = parseStatement();
            if (stmt != nullptr) {
                statements.push_back(stmt);
            }
        }
        clauses.push_back(std::make_pair(conditions, statements));
    }
    std::vector<const Statement *> else_statements;
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END) {
            const Statement *stmt = parseStatement();
            if (stmt != nullptr) {
                else_statements.push_back(stmt);
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
    clauses.push_back(std::make_pair(std::vector<const CaseStatement::WhenCondition *>(), else_statements));
    return new CaseStatement(tok_case, expr, clauses);
}

const Statement *Parser::parseForStatement()
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
    const Expression *start = parseExpression();
    if (tokens[i].type != TO) {
        error(2040, tokens[i], "TO expected");
    }
    ++i;
    const Expression *end = parseExpression();
    const Expression *step = nullptr;
    if (tokens[i].type == STEP) {
        ++i;
        step = parseExpression();
    }
    if (tokens[i].type != DO) {
        error_a(2041, tokens[i-1], tokens[i], "'DO' expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_for.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
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
    return new ForStatement(tok_for, var, start, end, step, statements);
}

const Statement *Parser::parseForeachStatement()
{
    auto &tok_foreach = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2081, tokens[i], "identifier expected");
    }
    const Token &var = tokens[i];
    ++i;
    if (tokens[i].type != OF) {
        error(2082, tokens[i], "OF expected");
    }
    ++i;
    const Expression *array = parseExpression();
    Token index;
    if (tokens[i].type == INDEX) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2083, tokens[i], "identifier expected");
        }
        index = tokens[i];
        ++i;
    }
    if (tokens[i].type != DO) {
        error_a(2084, tokens[i-1], tokens[i], "'DO' expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_foreach.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
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
    return new ForeachStatement(tok_foreach, var, array, index, statements);
}

const Statement *Parser::parseLoopStatement()
{
    auto &tok_loop = tokens[i];
    ++i;
    TemporaryMinimumIndent indent(this, tok_loop.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
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
    return new LoopStatement(tok_loop, statements);
}

const Statement *Parser::parseRepeatStatement()
{
    auto &tok_repeat = tokens[i];
    ++i;
    TemporaryMinimumIndent indent(this, tok_repeat.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != UNTIL && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != UNTIL) {
        error(2057, tokens[i], "UNTIL expected");
    }
    ++i;
    const Expression *cond = parseExpression();
    return new RepeatStatement(tok_repeat, cond, statements);
}

const Statement *Parser::parseExitStatement()
{
    auto &tok_exit = tokens[i];
    ++i;
    TokenType type = tokens[i].type;
    if (type != FUNCTION
     && type != WHILE
     && type != FOR
     && type != FOREACH
     && type != LOOP
     && type != REPEAT) {
        error_a(2052, tokens[i-1], tokens[i], "loop type expected");
    }
    ++i;
    return new ExitStatement(tok_exit, type);
}

const Statement *Parser::parseNextStatement()
{
    auto &tok_next = tokens[i];
    ++i;
    TokenType type = tokens[i].type;
    if (type != WHILE
     && type != FOR
     && type != FOREACH
     && type != LOOP
     && type != REPEAT) {
        error_a(2054, tokens[i-1], tokens[i], "loop type expected");
    }
    ++i;
    return new NextStatement(tok_next, type);
}

const Statement *Parser::parseTryStatement()
{
    auto &tok_try = tokens[i];
    ++i;
    TemporaryMinimumIndent indent(this, tok_try.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *stmt = parseStatement();
        if (stmt != nullptr) {
            statements.push_back(stmt);
        }
    }
    std::vector<std::pair<std::vector<std::pair<Token, Token>>, std::vector<const Statement *>>> catches;
    while (tokens[i].type == EXCEPTION) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2060, tokens[i], "identifier expected");
        }
        std::pair<Token, Token> name;
        name.second = tokens[i];
        ++i;
        if (tokens[i].type == DOT) {
            ++i;
            if (tokens[i].type != IDENTIFIER) {
                error(2077, tokens[i], "identifier expected");
            }
            name.first = name.second;
            name.second = tokens[i];
            ++i;
        }
        std::vector<std::pair<Token, Token>> exceptions;
        exceptions.push_back(name);
        std::vector<const Statement *> statements;
        while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *stmt = parseStatement();
            if (stmt != nullptr) {
                statements.push_back(stmt);
            }
        }
        catches.push_back(std::make_pair(exceptions, statements));
    }
    if (tokens[i].type != END) {
        error(2062, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != TRY) {
        error_a(2063, tokens[i-1], tokens[i], "TRY expected");
    }
    ++i;
    return new TryStatement(tok_try, statements, catches);
}

const Statement *Parser::parseRaiseStatement()
{
    auto &tok_raise = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2061, tokens[i], "identifier expected");
    }
    std::pair<Token, Token> name;
    name.second = tokens[i];
    ++i;
    if (tokens[i].type == DOT) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2076, tokens[i], "identifier expected");
        }
        name.first = name.second;
        name.second = tokens[i];
        ++i;
    }
    const Expression *info = nullptr;
    if (tokens[i].type == LPAREN) {
        info = parseFunctionCall(new IdentifierExpression(Token(), "ExceptionInfo"));
    } else {
        info = nullptr;
    }
    return new RaiseStatement(tok_raise, name, info);
}

const Statement *Parser::parseImport()
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
    return new ImportDeclaration(tok_import, module, name, alias);
}

static void deconstruct(const Expression *expr, std::vector<const Expression *> &parts)
{
    const IdentityExpression *ie = dynamic_cast<const IdentityExpression *>(expr);
    const UnaryExpression *ue = dynamic_cast<const UnaryExpression *>(expr);
    const BinaryExpression *be = dynamic_cast<const BinaryExpression *>(expr);
    if (ie != nullptr) {
        deconstruct(ie->expr, parts);
        return;
    } else if (ue != nullptr) {
        deconstruct(ue->expr, parts);
    } else if (be != nullptr) {
        deconstruct(be->left, parts);
        deconstruct(be->right, parts);
    } else if (dynamic_cast<const BooleanLiteralExpression *>(expr) != nullptr
            || dynamic_cast<const NumberLiteralExpression *>(expr) != nullptr
            || dynamic_cast<const StringLiteralExpression *>(expr) != nullptr) {
        return;
    }
    parts.push_back(expr);
}

const Statement *Parser::parseAssert()
{
    auto &tok_assert = tokens[i];
    ++i;
    const Expression *expr = parseExpression();
    std::vector<const Expression *> parts;
    deconstruct(expr, parts);
    while (tokens[i].type == COMMA) {
        ++i;
        const Expression *e = parseExpression();
        parts.push_back(e);
    }
    std::vector<const Statement *> body;
    {
        std::vector<FunctionCallExpression::Argument> args;
        args.push_back(FunctionCallExpression::Argument(Token(), Token(), new StringLiteralExpression(Token(), 0, "Assert failed (" + source.source_path + " line " + std::to_string(tok_assert.line) + "):")));
        const FunctionCallExpression *p = new FunctionCallExpression(Token(), new IdentifierExpression(Token(), "print"), args, Token());
        const ExpressionStatement *s = new ExpressionStatement(tok_assert, p);
        body.push_back(s);
    }
    {
        std::vector<FunctionCallExpression::Argument> args;
        args.push_back(FunctionCallExpression::Argument(Token(), Token(), new StringLiteralExpression(Token(), 0, tok_assert.source)));
        const FunctionCallExpression *p = new FunctionCallExpression(Token(), new IdentifierExpression(Token(), "print"), args, Token());
        const ExpressionStatement *s = new ExpressionStatement(tok_assert, p);
        body.push_back(s);
    }
    {
        std::vector<FunctionCallExpression::Argument> args;
        args.push_back(FunctionCallExpression::Argument(Token(), Token(), new StringLiteralExpression(Token(), 0, "Assert expression dump:")));
        const FunctionCallExpression *p = new FunctionCallExpression(Token(), new IdentifierExpression(Token(), "print"), args, Token());
        const ExpressionStatement *s = new ExpressionStatement(tok_assert, p);
        body.push_back(s);
    }
    std::set<std::string> seen;
    for (auto e: parts) {
        const std::string str = tok_assert.source.substr(e->get_start_column()-1, e->get_end_column()-e->get_start_column());
        if (seen.find(str) != seen.end()) {
            continue;
        }
        seen.insert(str);

        std::vector<std::pair<const Expression *, Token>> iparts;
        iparts.push_back(std::make_pair(new StringLiteralExpression(Token(), 0, "  " + str + " is "), Token()));
        iparts.push_back(std::make_pair(e, Token()));
        const InterpolatedStringExpression *is = new InterpolatedStringExpression(Token(), iparts);
        std::vector<FunctionCallExpression::Argument> args;
        args.push_back(FunctionCallExpression::Argument(Token(), Token(), is));
        const FunctionCallExpression *p = new FunctionCallExpression(Token(), new IdentifierExpression(Token(), "print"), args, Token());
        const ExpressionStatement *s = new ExpressionStatement(e->token, p);
        body.push_back(s);
    }
    return new AssertStatement(tok_assert, body, expr, tok_assert.source);
}

const Statement *Parser::parseBegin()
{
    auto &tok_begin = tokens[i];
    ++i;
    if (tokens[i].type != MAIN) {
        error(2091, tokens[i], "'MAIN' expected");
    }
    ++i;
    TemporaryMinimumIndent indent(this, tok_begin.column + 1);
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
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
    return new MainBlock(tok_begin, statements);
}

const Statement *Parser::parseStatement()
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
        return parseFunctionDefinition();
    } else if (tokens[i].type == EXTERNAL) {
        return parseExternalDefinition();
    } else if (tokens[i].type == DECLARE) {
        return parseDeclaration();
    } else if (tokens[i].type == EXPORT) {
        return parseExport();
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
    } else if (tokens[i].type == IDENTIFIER) {
        const Token &start = tokens[i];
        const Expression *expr = parseExpression();
        if (tokens[i].type == ASSIGN) {
            auto &tok_assign = tokens[i];
            ++i;
            const Expression *rhs = parseExpression();
            std::vector<const Expression *> vars;
            vars.push_back(expr);
            return new AssignmentStatement(tok_assign, vars, rhs);
        } else {
            return new ExpressionStatement(start, expr);
        }
    } else {
        error(2033, tokens[i], "Identifier expected");
    }
}

const Program *Parser::parse()
{
    auto &tok_program = tokens[i];
    std::vector<const Statement *> statements;
    while (tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    return new Program(tok_program, statements, source.source_path, source.source_hash);
}

const Program *parse(const TokenizedSource &tokens)
{
    return Parser(tokens).parse();
}
