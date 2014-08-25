#include "parser.h"

#include "ast.h"
#include "util.h"

static const Expression *parseExpression(const std::vector<Token> &tokens, std::vector<Token>::size_type &i);
static const VariableReference *parseVariableReference(const std::vector<Token> &tokens, std::vector<Token>::size_type &i);

static const Expression *parseFactor(const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    switch (tokens[i].type) {
        case LPAREN: {
            ++i;
            const Expression *expr = parseExpression(tokens, i);
            if (tokens[i].type != RPAREN) {
                error(tokens[i], ") expected");
            }
            ++i;
            return expr;
        }
        case NUMBER: {
            return new ConstantExpression(tokens[i++].value);
        }
        case MINUS: {
            ++i;
            return new UnaryMinusExpression(parseFactor(tokens, i));
        }
        case IDENTIFIER: {
            const VariableReference *ref = parseVariableReference(tokens, i);
            if (tokens[i].type == LPAREN) {
                ++i;
                std::vector<const Expression *> args;
                while (true) {
                    const Expression *e = parseExpression(tokens, i);
                    args.push_back(e);
                    if (tokens[i].type != COMMA) {
                        break;
                    }
                    ++i;
                }
                if (tokens[i].type != RPAREN) {
                    error(tokens[i], "')' or ',' expected");
                }
                ++i;
                return new FunctionCall(ref, args);
            } else {
                return new VariableExpression(ref);
            }
        }
        default:
            error(tokens[i], "Expression expected");
    }
}

static const Expression *parseTerm(const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseFactor(tokens, i);
    switch (tokens[i].type) {
        case TIMES: {
            ++i;
            const Expression *right = parseFactor(tokens, i);
            return new MultiplicationExpression(left, right);
        }
        case DIVIDE: {
            ++i;
            const Expression *right = parseFactor(tokens, i);
            return new DivisionExpression(left, right);
        }
        default:
            return left;
    }
}

static const Expression *parseExpression(const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseTerm(tokens, i);
    switch (tokens[i].type) {
        case PLUS: {
            ++i;
            const Expression *right = parseFactor(tokens, i);
            return new AdditionExpression(left, right);
        }
        case MINUS: {
            ++i;
            const Expression *right = parseFactor(tokens, i);
            return new SubtractionExpression(left, right);
        }
        default:
            return left;
    }
}

static const VariableReference *parseVariableReference(const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == IDENTIFIER) {
        VariableReference *ref;
        if (tokens[i].text == "abs" || tokens[i].text == "print") {
            ref = new FunctionReference(tokens[i].text);
        } else {
            ref = new ScalarVariableReference(tokens[i].text);
        }
        ++i;
        return ref;
    } else {
        error(tokens[i], "Identifier expected");
    }
}

static const Statement *parseStatement(const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == IF) {
        ++i;
        const Expression *cond = parseExpression(tokens, i);
        if (tokens[i].type != THEN) {
            error(tokens[i], "THEN expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(tokens, i);
            statements.push_back(s);
        }
        if (tokens[i].type != END) {
            error(tokens[i], "END expected");
        }
        ++i;
        return new IfStatement(cond, statements);
    } else if (tokens[i].type == WHILE) {
        ++i;
        const Expression *cond = parseExpression(tokens, i);
        if (tokens[i].type != DO) {
            error(tokens[i], "DO expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(tokens, i);
            statements.push_back(s);
        }
        if (tokens[i].type != END) {
            error(tokens[i], "END expected");
        }
        ++i;
        return new WhileStatement(cond, statements);
    } else if (tokens[i].type == IDENTIFIER) {
        const VariableReference *ref = parseVariableReference(tokens, i);
        if (tokens[i].type == ASSIGN) {
            ++i;
            const Expression *expr = parseExpression(tokens, i);
            return new AssignmentStatement(ref, expr);
        } else if (tokens[i].type == LPAREN) {
            ++i;
            std::vector<const Expression *> args;
            while (true) {
                const Expression *e = parseExpression(tokens, i);
                args.push_back(e);
                if (tokens[i].type != COMMA) {
                    break;
                }
                ++i;
            }
            if (tokens[i].type != RPAREN) {
                error(tokens[i], ") or , expected");
            }
            ++i;
            return new ExpressionStatement(new FunctionCall(ref, args));
        } else {
            error(tokens[i], "Unexpected");
        }
    } else {
        error(tokens[i], "Identifier expected");
    }
}

const Program *parse(const std::vector<Token> &tokens)
{
    std::vector<const Statement *> statements;
    std::vector<Token>::size_type i = 0;
    while (tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(tokens, i);
        statements.push_back(s);
    }
    return new Program(statements);
}
