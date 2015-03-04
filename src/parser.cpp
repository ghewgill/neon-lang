#include "parser.h"

#include <iso646.h>
#include <list>
#include <stack>

#include "format.h"
#include "pt.h"
#include "util.h"

using namespace pt;

class Parser {
public:
    Parser(const std::vector<Token> &tokens);

    const std::vector<Token> tokens;
    std::vector<Token>::size_type i;
    int expression_depth;

    typedef std::pair<std::vector<std::string>, const Type *> VariableInfo;

    const Type *parseParameterisedType();
    const Type *parseRecordType();
    const Type *parseEnumType();
    const Type *parsePointerType();
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
    void parseFunctionHeader(std::string &type, std::string &name, const Type *&returntype, std::vector<const FunctionParameter *> &args);
    const Statement *parseFunctionDefinition();
    const Statement *parseExternalDefinition();
    const Statement *parseDeclaration();
    const Statement *parseIfStatement();
    const Statement *parseReturnStatement();
    const Statement *parseVarStatement();
    const Statement *parseWhileStatement();
    const Statement *parseCaseStatement();
    const Statement *parseForStatement();
    const Statement *parseLoopStatement();
    const Statement *parseRepeatStatement();
    const Statement *parseExitStatement();
    const Statement *parseNextStatement();
    const Statement *parseTryStatement();
    const Statement *parseRaiseStatement();
    const Statement *parseImport();
    const Statement *parseStatement();
    const Program *parse();
private:
    Parser(const Parser &);
    Parser &operator=(const Parser &);
};

Parser::Parser(const std::vector<Token> &tokens)
  : tokens(tokens),
    i(0),
    expression_depth(0)
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
    i++;
    const Type *elementtype = parseType();
    if (tokens[i].type != GREATER) {
        error(2003, tokens[i], "'>' expected");
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
    std::vector<std::pair<Token, const Type *>> fields;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2008, tokens[i], "identifier expected");
        }
        const Token &name = tokens[i];
        ++i;
        if (tokens[i].type != COLON) {
            error(2010, tokens[i], "colon expected");
        }
        ++i;
        const Type *t = parseType();
        fields.push_back(std::make_pair(name, t));
    }
    i++;
    if (tokens[i].type != RECORD) {
        error(2100, tokens[i], "'RECORD' expected");
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
            error(2012, tokens[i], "identifier expected");
        }
        const Token &name = tokens[i];
        i++;
        names.push_back(std::make_pair(name, index));
        index++;
    }
    i++;
    if (tokens[i].type != ENUM) {
        error(2101, tokens[i], "'ENUM' expected");
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
    if (tokens[i].type != IDENTIFIER) {
        error(2014, tokens[i], "identifier expected");
    }
    const Token &name = tokens[i];
    i++;
    return new TypeSimple(name, name.text);
}

const Statement *Parser::parseTypeDefinition()
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2018, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2020, tokens[i], "':=' expected");
    }
    ++i;
    const Type *type = parseType();
    return new TypeDeclaration(tok_name, type);
}

const Statement *Parser::parseConstantDefinition()
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2021, tokens[i], "identifier expected");
    }
    auto &tok_name = tokens[i];
    ++i;
    if (tokens[i].type != COLON) {
        error(2023, tokens[i], "':' expected");
    }
    ++i;
    const Type *type = parseType();
    if (tokens[i].type != ASSIGN) {
        error(2024, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *value = parseExpression();
    return new ConstantDeclaration(tok_name, tok_name.text, type, value);
}

const FunctionCallExpression *Parser::parseFunctionCall(const Expression *func)
{
    auto &tok_lparen = tokens[i];
    ++i;
    std::vector<const Expression *> args;
    if (tokens[i].type != RPAREN) {
        for (;;) {
            const Expression *e = parseExpression();
            args.push_back(e);
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2030, tokens[i], "')' or ',' expected");
        }
    }
    ++i;
    return new FunctionCallExpression(tok_lparen, func, args);
}

const ArrayLiteralExpression *Parser::parseArrayLiteral()
{
    auto &tok_lbracket = tokens[i];
    std::vector<const Expression *> elements;
    while (tokens[i].type != RBRACKET) {
        const Expression *element = parseExpression();
        elements.push_back(element);
        if (tokens[i].type == COMMA) {
            ++i;
        } else if (tokens[i].type != RBRACKET) {
            error(2139, tokens[i], "',' or ']' expected");
        }
    }
    ++i;
    return new ArrayLiteralExpression(tok_lbracket, elements);
}

const DictionaryLiteralExpression *Parser::parseDictionaryLiteral()
{
    auto &tok_lbrace = tokens[i];
    std::vector<std::pair<Token, const Expression *>> elements;
    while (tokens[i].type == STRING) {
        auto &key = tokens[i];
        ++i;
        if (tokens[i].type != COLON) {
            error(2126, tokens[i], "':' expected");
        }
        ++i;
        const Expression *element = parseExpression();
        elements.push_back(std::make_pair(key, element));
        if (tokens[i].type == COMMA) {
            ++i;
        }
    }
    return new DictionaryLiteralExpression(tok_lbrace, elements);
}

const Expression *Parser::parseInterpolatedStringExpression()
{
    std::vector<std::pair<const Expression *, std::string>> parts;
    parts.push_back(std::make_pair(new StringLiteralExpression(tokens[i], tokens[i].text), ""));
    for (;;) {
        ++i;
        if (tokens[i].type != SUBBEGIN) {
            break;
        }
        ++i;
        const Expression *e = parseExpression();
        std::string fmt;
        if (tokens[i].type == SUBFMT) {
            ++i;
            if (tokens[i].type != STRING) {
                internal_error("parseInterpolatedStringExpression");
            }
            fmt = tokens[i].text;
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
        e = new StringLiteralExpression(tokens[i], tokens[i].text);
        parts.push_back(std::make_pair(e, ""));
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
            ++i;
            const Expression *expr = parseExpression();
            if (tokens[i].type != RPAREN) {
                error(2032, tokens[i], ") expected");
            }
            ++i;
            return expr;
        }
        case LBRACKET: {
            ++i;
            const Expression *array = parseArrayLiteral();
            return array;
        }
        case LBRACE: {
            ++i;
            const Expression *dict = parseDictionaryLiteral();
            if (tokens[i].type != RBRACE) {
                error(2128, tokens[i], "'}' expected");
            }
            ++i;
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
                return new StringLiteralExpression(tok_string, tok_string.text);
            }
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
            return new NewRecordExpression(tok_new, type);
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
                    const Expression *index = parseExpression();
                    if (tokens[i].type != RBRACKET) {
                        error(2063, tokens[i], "']' expected");
                    }
                    ++i;
                    expr = new SubscriptExpression(tok_lbracket, expr, index);
                } else if (tokens[i].type == LPAREN) {
                    expr = parseFunctionCall(expr);
                } else if (tokens[i].type == DOT) {
                    auto &tok_dot = tokens[i];
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2069, tokens[i], "identifier expected");
                    }
                    const std::string field = tokens[i].text;
                    ++i;
                    expr = new DotExpression(tok_dot, expr, field);
                } else if (tokens[i].type == ARROW) {
                    auto &tok_arrow = tokens[i];
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2186, tokens[i], "identifier expected");
                    }
                    const std::string field = tokens[i].text;
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
            error(2038, tokens[i], "Expression expected");
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
            error(2053, tokens[i], "'THEN' expected");
        }
        ++i;
        const Expression *left = parseExpression();
        if (tokens[i].type != ELSE) {
            error(2054, tokens[i], "'ELSE' expected");
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
        error(2197, tokens[i], "exceeded maximum nesting depth");
    }
    const Expression *r = parseConditional();
    expression_depth--;
    return r;
}

const Parser::VariableInfo Parser::parseVariableDeclaration()
{
    std::vector<std::string> names;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2056, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        ++i;
        names.push_back(name);
        if (tokens[i].type != COMMA) {
            break;
        }
        ++i;
    }
    if (tokens[i].type != COLON) {
        error(2058, tokens[i], "colon expected");
    }
    ++i;
    const Type *t = parseType();
    return make_pair(names, t);
}

void Parser::parseFunctionHeader(std::string &type, std::string &name, const Type *&returntype, std::vector<const FunctionParameter *> &args)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2073, tokens[i], "identifier expected");
    }
    name = tokens[i].text;
    ++i;
    if (tokens[i].type == DOT) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2208, tokens[i], "identifier expected");
        }
        type = name;
        name = tokens[i].text;
        ++i;
    }
    if (tokens[i].type != LPAREN) {
        error(2075, tokens[i], "'(' expected");
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
            for (auto name: vars.first) {
                FunctionParameter *fp = new FunctionParameter(tok_param, name, vars.second, mode);
                args.push_back(fp);
            }
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2076, tokens[i], "')' or ',' expected");
        }
    }
    ++i;
    if (tokens[i].type == COLON) {
        ++i;
        returntype = parseType();
    } else {
        returntype = nullptr;
    }
}

const Statement *Parser::parseFunctionDefinition()
{
    auto &tok_function = tokens[i];
    std::string type;
    std::string name;
    const Type *returntype;
    std::vector<const FunctionParameter *> args;
    parseFunctionHeader(type, name, returntype, args);
    std::vector<const Statement *> body;
    while (tokens[i].type != END) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            body.push_back(s);
        }
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2102, tokens[i], "'FUNCTION' expected");
    }
    ++i;
    return new FunctionDeclaration(tok_function, type, name, returntype, args, body);
}

const Statement *Parser::parseExternalDefinition()
{
    auto &tok_external = tokens[i];
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2122, tokens[i], "FUNCTION expected");
    }
    std::string type;
    std::string name;
    const Type *returntype;
    std::vector<const FunctionParameter *> args;
    parseFunctionHeader(type, name, returntype, args);
    if (tokens[i].type != LBRACE) {
        error(2123, tokens[i], "{ expected");
    }
    ++i;
    const DictionaryLiteralExpression *dict = parseDictionaryLiteral();
    if (tokens[i].type != RBRACE) {
        error(2125, tokens[i], "} expected");
    }
    ++i;
    if (tokens[i].type != END) {
        error(2129, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2130, tokens[i], "'END FUNCTION' expected");
    }
    ++i;
    return new ExternalFunctionDeclaration(tok_external, type, name, returntype, args, dict);
}

const Statement *Parser::parseDeclaration()
{
    ++i;
    switch (tokens[i].type) {
        case EXCEPTION: {
            ++i;
            auto &tok_name = tokens[i];
            if (tokens[i].type != IDENTIFIER) {
                error(2152, tokens[i], "identifier expected");
            }
            std::string name = tokens[i].text;
            ++i;
            return new ExceptionDeclaration(tok_name, name);
        }
        default:
            error(2151, tokens[i], "EXCEPTION expected");
    }
}

const Statement *Parser::parseIfStatement()
{
    auto &tok_if = tokens[i];
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;
    do {
        ++i;
        const Expression *cond = nullptr;
        std::string name;
        if (tokens[i].type == VALID) {
            for (;;) {
                auto &tok_valid = tokens[i];
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2174, tokens[i], "identifier expected");
                }
                name = tokens[i].text;
                ++i;
                if (tokens[i].type != ASSIGN) {
                    error(2175, tokens[i], "':=' expected");
                }
                ++i;
                const Expression *ptr = parseExpression();
                const Expression *valid = new ValidPointerExpression(tok_valid, name, ptr);
                if (cond == nullptr) {
                    cond = valid;
                } else {
                    cond = new ConjunctionExpression(tokens[i], cond, valid);
                }
                if (tokens[i].type != COMMA) {
                    break;
                }
                // TODO: check that next token is VALID
            }
        } else {
            cond = parseExpression();
        }
        if (tokens[i].type != THEN) {
            error(2079, tokens[i], "THEN expected");
        }
        ++i;
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
        error(2080, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != IF) {
        error(2103, tokens[i], "IF expected");
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

const Statement *Parser::parseWhileStatement()
{
    auto &tok_while = tokens[i];
    ++i;
    const Expression *cond = parseExpression();
    if (tokens[i].type != DO) {
        error(2082, tokens[i], "DO expected");
    }
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != END) {
        error(2083, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != WHILE) {
        error(2104, tokens[i], "WHILE expected");
    }
    ++i;
    return new WhileStatement(tok_while, cond, statements);
}

const Statement *Parser::parseCaseStatement()
{
    auto &tok_case = tokens[i];
    ++i;
    const Expression *expr = parseExpression();
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
            error(2091, tokens[i], "'DO' expected");
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
        error(2092, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != CASE) {
        error(2105, tokens[i], "CASE expected");
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
        error(2072, tokens[i], "identifier expected");
    }
    const std::string var = tokens[i].text;
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2121, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *start = parseExpression();
    if (tokens[i].type != TO) {
        error(2114, tokens[i], "TO expected");
    }
    ++i;
    const Expression *end = parseExpression();
    const Expression *step = nullptr;
    if (tokens[i].type == STEP) {
        ++i;
        step = parseExpression();
    }
    if (tokens[i].type != DO) {
        error(2118, tokens[i], "'DO' expected");
    }
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != END) {
        error(2119, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FOR) {
        error(2120, tokens[i], "'END FOR' expected");
    }
    ++i;
    return new ForStatement(tok_for, var, start, end, step, statements);
}

const Statement *Parser::parseLoopStatement()
{
    auto &tok_loop = tokens[i];
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != END) {
        error(2147, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != LOOP) {
        error(2148, tokens[i], "LOOP expected");
    }
    ++i;
    return new LoopStatement(tok_loop, statements);
}

const Statement *Parser::parseRepeatStatement()
{
    auto &tok_repeat = tokens[i];
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != UNTIL && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement();
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != UNTIL) {
        error(2149, tokens[i], "UNTIL expected");
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
     && type != LOOP
     && type != REPEAT) {
        error(2136, tokens[i], "loop type expected");
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
     && type != LOOP
     && type != REPEAT) {
        error(2144, tokens[i], "loop type expected");
    }
    ++i;
    return new NextStatement(tok_next, type);
}

const Statement *Parser::parseTryStatement()
{
    auto &tok_try = tokens[i];
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *stmt = parseStatement();
        if (stmt != nullptr) {
            statements.push_back(stmt);
        }
    }
    std::vector<std::pair<std::vector<std::string>, std::vector<const Statement *>>> catches;
    while (tokens[i].type == EXCEPTION) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2153, tokens[i], "identifier expected");
        }
        const std::string name = tokens[i].text;
        std::vector<std::string> exceptions;
        exceptions.push_back(name);
        ++i;
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
        error(2159, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != TRY) {
        error(2160, tokens[i], "TRY expected");
    }
    ++i;
    return new TryStatement(tok_try, statements, catches);
}

const Statement *Parser::parseRaiseStatement()
{
    auto &tok_raise = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2156, tokens[i], "identifier expected");
    }
    const std::string name = tokens[i].text;
    ++i;
    const Expression *info = nullptr;
    if (tokens[i].type == LPAREN) {
        info = parseExpression();
    } else {
        info = nullptr;
    }
    return new RaiseStatement(tok_raise, name, info);
}

const Statement *Parser::parseImport()
{
    auto &tok_import = tokens[i];
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2093, tokens[i], "identifier expected");
    }
    const std::string name = tokens[i].text;
    ++i;
    return new ImportDeclaration(tok_import, name);
}

const Statement *Parser::parseStatement()
{
    if (tokens[i].type == IMPORT) {
        return parseImport();
    } else if (tokens[i].type == TYPE) {
        return parseTypeDefinition();
    } else if (tokens[i].type == CONST) {
        return parseConstantDefinition();
    } else if (tokens[i].type == FUNCTION) {
        return parseFunctionDefinition();
    } else if (tokens[i].type == EXTERNAL) {
        return parseExternalDefinition();
    } else if (tokens[i].type == DECLARE) {
        return parseDeclaration();
    } else if (tokens[i].type == IF) {
        return parseIfStatement();
    } else if (tokens[i].type == RETURN) {
        return parseReturnStatement();
    } else if (tokens[i].type == VAR) {
        return parseVarStatement();
    } else if (tokens[i].type == WHILE) {
        return parseWhileStatement();
    } else if (tokens[i].type == CASE) {
        return parseCaseStatement();
    } else if (tokens[i].type == FOR) {
        return parseForStatement();
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
    } else if (tokens[i].type == IDENTIFIER) {
        const Expression *expr = parseExpression();
        if (tokens[i].type == ASSIGN) {
            auto &tok_assign = tokens[i];
            ++i;
            const Expression *rhs = parseExpression();
            std::vector<const Expression *> vars;
            vars.push_back(expr);
            return new AssignmentStatement(tok_assign, vars, rhs);
        } else {
            return new ExpressionStatement(expr);
        }
    } else {
        error(2099, tokens[i], "Identifier expected");
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
    return new Program(tok_program, statements);
}

const Program *parse(const std::vector<Token> &tokens)
{
    return Parser(tokens).parse();
}
