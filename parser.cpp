#include "parser.h"

#include "ast.h"
#include "util.h"

static const Expression *parseExpression(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);
static const VariableReference *parseVariableReference(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);

static const Type *parseType(const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    if (tokens[i].text == "number") {
        ++i;
        return new TypeNumber();
    } else if (tokens[i].text == "string") {
        ++i;
        return new TypeString();
    } else {
        error(tokens[i], "unknown type name");
    }
}

static const Expression *parseFactor(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    switch (tokens[i].type) {
        case LPAREN: {
            ++i;
            const Expression *expr = parseExpression(scope, tokens, i);
            if (tokens[i].type != RPAREN) {
                error(tokens[i], ") expected");
            }
            ++i;
            return expr;
        }
        case NUMBER: {
            return new ConstantNumberExpression(tokens[i++].value);
        }
        case MINUS: {
            ++i;
            return new UnaryMinusExpression(parseFactor(scope, tokens, i));
        }
        case IDENTIFIER: {
            const VariableReference *ref = parseVariableReference(scope, tokens, i);
            if (tokens[i].type == LPAREN) {
                ++i;
                std::vector<const Expression *> args;
                while (true) {
                    const Expression *e = parseExpression(scope, tokens, i);
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

static const Expression *parseTerm(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseFactor(scope, tokens, i);
    switch (tokens[i].type) {
        case TIMES: {
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            return new MultiplicationExpression(left, right);
        }
        case DIVIDE: {
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            return new DivisionExpression(left, right);
        }
        default:
            return left;
    }
}

static const Expression *parseExpression(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseTerm(scope, tokens, i);
    switch (tokens[i].type) {
        case PLUS: {
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            return new AdditionExpression(left, right);
        }
        case MINUS: {
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            return new SubtractionExpression(left, right);
        }
        default:
            return left;
    }
}

static const VariableReference *parseVariableReference(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == IDENTIFIER) {
        const Variable *var = scope->lookupVariable(tokens[i].text);
        if (var == nullptr) {
            error(tokens[i], "variable not found: " + tokens[i].text);
        }
        const VariableReference *ref = new ScalarVariableReference(var);
        ++i;
        // TODO: [ for array and . for struct
        return ref;
    } else {
        error(tokens[i], "Identifier expected");
    }
}

static const Statement *parseStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == IF) {
        ++i;
        const Expression *cond = parseExpression(scope, tokens, i);
        if (tokens[i].type != THEN) {
            error(tokens[i], "THEN expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope, tokens, i);
            if (s != nullptr) {
                statements.push_back(s);
            }
        }
        if (tokens[i].type != END) {
            error(tokens[i], "END expected");
        }
        ++i;
        return new IfStatement(cond, statements);
    } else if (tokens[i].type == VAR) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (scope->vars.find(name) != scope->vars.end()) {
            error(tokens[i], "duplicate identifer: " + name);
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(tokens[i], "colon expected");
        }
        ++i;
        const Type *t = parseType(tokens, i);
        scope->vars[name] = new Variable(name, t);
        return nullptr;
    } else if (tokens[i].type == WHILE) {
        ++i;
        const Expression *cond = parseExpression(scope, tokens, i);
        if (tokens[i].type != DO) {
            error(tokens[i], "DO expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope, tokens, i);
            if (s != nullptr) {
                statements.push_back(s);
            }
        }
        if (tokens[i].type != END) {
            error(tokens[i], "END expected");
        }
        ++i;
        return new WhileStatement(cond, statements);
    } else if (tokens[i].type == IDENTIFIER) {
        const VariableReference *ref = parseVariableReference(scope, tokens, i);
        if (tokens[i].type == ASSIGN) {
            ++i;
            const Expression *expr = parseExpression(scope, tokens, i);
            return new AssignmentStatement(ref, expr);
        } else if (tokens[i].type == LPAREN) {
            ++i;
            std::vector<const Expression *> args;
            while (true) {
                const Expression *e = parseExpression(scope, tokens, i);
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
    Program *program = new Program();
    std::vector<Token>::size_type i = 0;
    while (tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(program, tokens, i);
        if (s != nullptr) {
            program->statements.push_back(s);
        }
    }
    return program;
}
