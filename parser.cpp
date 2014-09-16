#include "parser.h"

#include "ast.h"
#include "util.h"

static const Expression *parseExpression(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);
static const VariableReference *parseVariableReference(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);

static const Type *parseType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    const Type *type = scope->lookupType(tokens[i].text);
    if (type == nullptr) {
        error(tokens[i], "unknown type name");
    }
    i++;
    return type;
}

static const FunctionCall *parseFunctionCall(const VariableReference *ref, Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(ref->type);
    if (ftype == nullptr) {
        error(tokens[i], "not a function");
    }
    ++i;
    std::vector<const Type *>::size_type a = 0;
    std::vector<const Expression *> args;
    while (true) {
        const Expression *e = parseExpression(scope, tokens, i);
        if (e->type != ftype->args[a]) {
            error(tokens[i], "type mismatch");
        }
        args.push_back(e);
        ++a;
        if (tokens[i].type != COMMA) {
            break;
        }
        ++i;
    }
    if (a < ftype->args.size()) {
        error(tokens[i], "not enough arguments");
    }
    if (tokens[i].type != RPAREN) {
        error(tokens[i], "')' or ',' expected");
    }
    ++i;
    return new FunctionCall(ref, args);
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
        case STRING: {
            return new ConstantStringExpression(tokens[i++].text);
        }
        case MINUS: {
            auto op = i;
            ++i;
            const Expression *factor = parseFactor(scope, tokens, i);
            if (factor->type != TYPE_NUMBER) {
                error(tokens[op], "number required for negation");
            }
            return new UnaryMinusExpression(factor);
        }
        case IDENTIFIER: {
            const VariableReference *ref = parseVariableReference(scope, tokens, i);
            if (tokens[i].type == LPAREN) {
                return parseFunctionCall(ref, scope, tokens, i);
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
            auto op = i;
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new MultiplicationExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
        }
        case DIVIDE: {
            auto op = i;
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new DivisionExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
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
            auto op = i;
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new AdditionExpression(left, right);
            } else if (left->type == TYPE_STRING && right->type == TYPE_STRING) {
                std::vector<const Expression *> args;
                args.push_back(left);
                args.push_back(right);
                return new FunctionCall(new ScalarVariableReference(scope->lookupVariable("concat")), args);
            } else {
                error(tokens[op], "type mismatch");
            }
        }
        case MINUS: {
            auto op = i;
            ++i;
            const Expression *right = parseFactor(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new SubtractionExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
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
        const Type *t = parseType(scope, tokens, i);
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
            auto op = i;
            ++i;
            const Expression *expr = parseExpression(scope, tokens, i);
            if (expr->type != ref->type) {
                error(tokens[op], "type mismatch");
            }
            return new AssignmentStatement(ref, expr);
        } else if (tokens[i].type == LPAREN) {
            const FunctionCall *fc = parseFunctionCall(ref, scope, tokens, i);
            return new ExpressionStatement(fc);
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
