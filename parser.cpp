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
        error(tokens[i-1], "not a function");
    }
    ++i;
    std::vector<const Type *>::size_type a = 0;
    std::vector<const Expression *> args;
    if (tokens[i].type != RPAREN) {
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
        if (tokens[i].type != RPAREN) {
            error(tokens[i], "')' or ',' expected");
        }
    }
    if (a < ftype->args.size()) {
        error(tokens[i], "not enough arguments");
    }
    ++i;
    return new FunctionCall(ref, args);
}

/*
 * Operator precedence:
 *
 *  ^        exponentiation             parseExponentiation
 *  * /      multiplication, division   parseMultiplication
 *  + -      addition, subtraction      parseAddition
 *  < = >    comparison                 parseComparison
 *  and      conjunction                parseConjunction
 *  or       disjunction                parseDisjunction
 */

static const Expression *parseAtom(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
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
        case FALSE: {
            ++i;
            return new ConstantBooleanExpression(false);
        }
        case TRUE: {
            ++i;
            return new ConstantBooleanExpression(true);
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
            const Expression *factor = parseAtom(scope, tokens, i);
            if (factor->type != TYPE_NUMBER) {
                error(tokens[op], "number required for negation");
            }
            return new UnaryMinusExpression(factor);
        }
        case NOT: {
            auto op = i;
            ++i;
            const Expression *atom = parseAtom(scope, tokens, i);
            if (atom->type != TYPE_BOOLEAN) {
                error(tokens[op], "boolean required for logical not");
            }
            return new LogicalNotExpression(atom);
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

static const Expression *parseExponentiation(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseAtom(scope, tokens, i);
    if (tokens[i].type == EXP) {
        auto op = i;
        ++i;
        const Expression *right = parseAtom(scope, tokens, i);
        if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
            return new ExponentiationExpression(left, right);
        } else {
            error(tokens[op], "type mismatch");
        }
    } else {
        return left;
    }
}

static const Expression *parseMultiplication(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseExponentiation(scope, tokens, i);
    switch (tokens[i].type) {
        case TIMES: {
            auto op = i;
            ++i;
            const Expression *right = parseExponentiation(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new MultiplicationExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
        }
        case DIVIDE: {
            auto op = i;
            ++i;
            const Expression *right = parseExponentiation(scope, tokens, i);
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

static const Expression *parseAddition(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseMultiplication(scope, tokens, i);
    switch (tokens[i].type) {
        case PLUS: {
            auto op = i;
            ++i;
            const Expression *right = parseMultiplication(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new AdditionExpression(left, right);
            } else if (left->type == TYPE_STRING && right->type == TYPE_STRING) {
                std::vector<const Expression *> args;
                args.push_back(left);
                args.push_back(right);
                return new FunctionCall(new ScalarVariableReference(dynamic_cast<const Variable *>(scope->lookupName("concat"))), args);
            } else {
                error(tokens[op], "type mismatch");
            }
        }
        case MINUS: {
            auto op = i;
            ++i;
            const Expression *right = parseMultiplication(scope, tokens, i);
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

static const Expression *parseComparison(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseAddition(scope, tokens, i);
    switch (tokens[i].type) {
        case EQUAL:
        case NOTEQUAL:
        case LESS:
        case GREATER:
        case LESSEQ:
        case GREATEREQ: {
            ComparisonExpression::Comparison comp;
            switch (tokens[i].type) {
                case EQUAL:     comp = ComparisonExpression::EQ; break;
                case NOTEQUAL:  comp = ComparisonExpression::NE; break;
                case LESS:      comp = ComparisonExpression::LT; break;
                case GREATER:   comp = ComparisonExpression::GT; break;
                case LESSEQ:    comp = ComparisonExpression::LE; break;
                case GREATEREQ: comp = ComparisonExpression::GE; break;
                default:
                    error(tokens[i], "internal error");
            }
            auto op = i;
            ++i;
            const Expression *right = parseAddition(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                return new NumericComparisonExpression(left, right, comp);
            } else if (left->type == TYPE_STRING && right->type == TYPE_STRING) {
                return new StringComparisonExpression(left, right, comp);
            } else {
                error(tokens[op], "type mismatch");
            }
        }
        default:
            return left;
    }
}

static const Expression *parseConjunction(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseComparison(scope, tokens, i);
    if (tokens[i].type == AND) {
        auto op = i;
        ++i;
        const Expression *right = parseComparison(scope, tokens, i);
        if (left->type == TYPE_BOOLEAN && right->type == TYPE_BOOLEAN) {
            return new ConjunctionExpression(left, right);
        } else {
            error(tokens[op], "type mismatch");
        }
    } else {
        return left;
    }
}

static const Expression *parseDisjunction(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseConjunction(scope, tokens, i);
    if (tokens[i].type == OR) {
        auto op = i;
        ++i;
        const Expression *right = parseConjunction(scope, tokens, i);
        if (left->type == TYPE_BOOLEAN && right->type == TYPE_BOOLEAN) {
            return new DisjunctionExpression(left, right);
        } else {
            error(tokens[op], "type mismatch");
        }
    } else {
        return left;
    }
}

static const Expression *parseExpression(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    return parseDisjunction(scope, tokens, i);
}

static const Variable *parseVariableDeclaration(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    if (scope->names.find(name) != scope->names.end()) {
        error(tokens[i], "duplicate identifer: " + name);
    }
    ++i;
    if (tokens[i].type != COLON) {
        error(tokens[i], "colon expected");
    }
    ++i;
    const Type *t = parseType(scope, tokens, i);
    Variable *v = new Variable(name, t);
    scope->names[name] = v;
    return v;
}

static const VariableReference *parseVariableReference(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == IDENTIFIER) {
        const Name *name = scope->lookupName(tokens[i].text);
        if (name == nullptr) {
            error(tokens[i], "variable not found: " + tokens[i].text);
        }
        const Variable *var = dynamic_cast<const Variable *>(name);
        if (var == nullptr) {
            error(tokens[i], "name is not a variable: " + tokens[i].text);
        }
        const VariableReference *ref = new ScalarVariableReference(var);
        ++i;
        // TODO: [ for array and . for struct
        return ref;
    } else {
        error(tokens[i], "Identifier expected");
    }
}

static const Statement *parseStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);
static const Statement *parseVarStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);

static const Statement *parseFunctionDefinition(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    ++i;
    if (tokens[i].type != LPAREN) {
        error(tokens[i], "'(' expected");
    }
    ++i;
    std::vector<const Variable *> args;
    if (tokens[i].type != RPAREN) {
        while (true) {
            const Variable *v = parseVariableDeclaration(scope, tokens, i);
            args.push_back(v);
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(tokens[i], "')' or ',' expected");
        }
    }
    ++i;
    if (tokens[i].type != COLON) {
        error(tokens[i], "':' expected");
    }
    ++i;
    const Type *returntype = parseType(scope, tokens, i);
    Function *function = new Function(name, returntype, args);
    Scope *bodyscope = new Scope(scope);
    while (tokens[i].type != END) {
        const Statement *s = parseStatement(bodyscope, tokens, i);
        if (s != nullptr) {
            function->statements.push_back(s);
        }
    }
    ++i;
    scope->names[name] = function;
    return nullptr;
}

static const Statement *parseIfStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
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
}

static const Statement *parseReturnStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    ++i;
    const Expression *expr = parseExpression(scope, tokens, i);
    // TODO: check return type
    return new ReturnStatement(expr);
}

static const Statement *parseVarStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    ++i;
    parseVariableDeclaration(scope, tokens, i);
    return nullptr;
}

static const Statement *parseWhileStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
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
}

static const Statement *parseStatement(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == FUNCTION) {
        return parseFunctionDefinition(scope, tokens, i);
    } else if (tokens[i].type == IF) {
        return parseIfStatement(scope, tokens, i);
    } else if (tokens[i].type == RETURN) {
        return parseReturnStatement(scope, tokens, i);
    } else if (tokens[i].type == VAR) {
        return parseVarStatement(scope, tokens, i);
    } else if (tokens[i].type == WHILE) {
        return parseWhileStatement(scope, tokens, i);
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
        const Statement *s = parseStatement(program->scope, tokens, i);
        if (s != nullptr) {
            program->statements.push_back(s);
        }
    }
    return program;
}
