#include "parser.h"

#include "ast.h"
#include "util.h"

static Scope *g_GlobalScope; // TODO: sort of a hack, all this should probably be in a class

static const Type *parseType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);
static const Expression *parseExpression(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);
static const VariableReference *parseVariableReference(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i);

static const Type *parseArrayType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != ARRAY) {
        error(tokens[i], "ARRAY expected");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(tokens[i], "'<' expected");
    }
    i++;
    const Type *elementtype = parseType(scope, tokens, i);
    if (tokens[i].type != GREATER) {
        error(tokens[i], "'>' expected");
    }
    i++;
    return new TypeArray(elementtype);
}

static const Type *parseDictionaryType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != DICTIONARY) {
        error(tokens[i], "DICTIONARY expected");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(tokens[i], "'<' expected");
    }
    i++;
    const Type *elementtype = parseType(scope, tokens, i);
    if (tokens[i].type != GREATER) {
        error(tokens[i], "'>' expected");
    }
    i++;
    return new TypeDictionary(elementtype);
}

static const Type *parseRecordType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != RECORD) {
        error(tokens[i], "RECORD expected");
    }
    i++;
    std::map<std::string, std::pair<int, const Type *> > fields;
    int index = 0;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (fields.find(name) != fields.end()) {
            error(tokens[i], "duplicate field: " + name);
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(tokens[i], "colon expected");
        }
        ++i;
        const Type *t = parseType(scope, tokens, i);
        fields[name] = std::make_pair(index, t);
        index++;
    }
    i++;
    return new TypeRecord(fields);
}

static const Type *parseEnumType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type != ENUM) {
        error(tokens[i], "ENUM expected");
    }
    i++;
    std::map<std::string, int> names;
    int index = 0;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (names.find(name) != names.end()) {
            error(tokens[i], "duplicate enum: " + name);
        }
        i++;
        names[name] = index;
        index++;
    }
    i++;
    return new TypeEnum(names);
}

static const Type *parseType(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == ARRAY) {
        return parseArrayType(scope, tokens, i);
    }
    if (tokens[i].type == DICTIONARY) {
        return parseDictionaryType(scope, tokens, i);
    }
    if (tokens[i].type == RECORD) {
        return parseRecordType(scope, tokens, i);
    }
    if (tokens[i].type == ENUM) {
        return parseEnumType(scope, tokens, i);
    }
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    const Name *name = scope->lookupName(tokens[i].text);
    if (name == nullptr) {
        error(tokens[i], "unknown type name");
    }
    const Type *type = dynamic_cast<const Type *>(name);
    if (type == nullptr) {
        error(tokens[i], "name is not a type");
    }
    i++;
    return type;
}

static const Statement *parseTypeDefinition(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(tokens[i], "':=' expected");
    }
    ++i;
    const Type *type = parseType(scope, tokens, i);
    scope->names[name] = const_cast<Type *>(type); // TODO clean up when 'referenced' is fixed
    return nullptr;
}

static const Statement *parseConstantDefinition(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    ++i;
    if (tokens[i].type != COLON) {
        error(tokens[i], "':' expected");
    }
    ++i;
    const Type *type = parseType(scope, tokens, i);
    if (tokens[i].type != ASSIGN) {
        error(tokens[i], "':=' expected");
    }
    ++i;
    const Expression *value = parseExpression(scope, tokens, i);
    if (value->type != type) {
        error(tokens[i], "type mismatch");
    }
    scope->names[name] = new Constant(name, value);
    return nullptr;
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
 *  ^        exponentiation                     parseExponentiation
 *  * / MOD  multiplication, division, modulo   parseMultiplication
 *  + -      addition, subtraction              parseAddition
 *  < = >    comparison                         parseComparison
 *  and      conjunction                        parseConjunction
 *  or       disjunction                        parseDisjunction
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
            const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(scope->lookupName(tokens[i].text));
            if (enumtype != nullptr) {
                ++i;
                if (tokens[i].type != DOT) {
                    error(tokens[i], "'.' expected");
                }
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(tokens[i], "identifier expected");
                }
                auto name = enumtype->names.find(tokens[i].text);
                if (name == enumtype->names.end()) {
                    error(tokens[i], "identifier not member of enum: " + tokens[i].text);
                }
                ++i;
                return new ConstantEnumExpression(enumtype, name->second);
            } else {
                const VariableReference *ref = parseVariableReference(scope, tokens, i);
                if (tokens[i].type == LPAREN) {
                    return parseFunctionCall(ref, scope, tokens, i);
                } else {
                    return new VariableExpression(ref);
                }
            }
        }
        default:
            error(tokens[i], "Expression expected");
    }
}

static const Expression *parseExponentiation(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseAtom(scope, tokens, i);
    while (true) {
        if (tokens[i].type == EXP) {
            auto op = i;
            ++i;
            const Expression *right = parseAtom(scope, tokens, i);
            if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                left = new ExponentiationExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

static const Expression *parseMultiplication(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseExponentiation(scope, tokens, i);
    while (true) {
        switch (tokens[i].type) {
            case TIMES: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope, tokens, i);
                if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                    left = new MultiplicationExpression(left, right);
                } else {
                    error(tokens[op], "type mismatch");
                }
                break;
            }
            case DIVIDE: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope, tokens, i);
                if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                    left = new DivisionExpression(left, right);
                } else {
                    error(tokens[op], "type mismatch");
                }
                break;
            }
            case MOD: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope, tokens, i);
                if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                    left = new ModuloExpression(left, right);
                } else {
                    error(tokens[op], "type mismatch");
                }
                break;
            }
            default:
                return left;
        }
    }
}

static const Expression *parseAddition(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseMultiplication(scope, tokens, i);
    while (true) {
        switch (tokens[i].type) {
            case PLUS: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope, tokens, i);
                if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                    left = new AdditionExpression(left, right);
                } else if (left->type == TYPE_STRING && right->type == TYPE_STRING) {
                    std::vector<const Expression *> args;
                    args.push_back(left);
                    args.push_back(right);
                    left = new FunctionCall(new ScalarVariableReference(dynamic_cast<const Variable *>(scope->lookupName("concat"))), args);
                } else {
                    error(tokens[op], "type mismatch");
                }
                break;
            }
            case MINUS: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope, tokens, i);
                if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
                    left = new SubtractionExpression(left, right);
                } else {
                    error(tokens[op], "type mismatch");
                }
                break;
            }
            default:
                return left;
        }
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
    while (true) {
        if (tokens[i].type == AND) {
            auto op = i;
            ++i;
            const Expression *right = parseComparison(scope, tokens, i);
            if (left->type == TYPE_BOOLEAN && right->type == TYPE_BOOLEAN) {
                left = new ConjunctionExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

static const Expression *parseDisjunction(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    const Expression *left = parseConjunction(scope, tokens, i);
    while (true) {
        if (tokens[i].type == OR) {
            auto op = i;
            ++i;
            const Expression *right = parseConjunction(scope, tokens, i);
            if (left->type == TYPE_BOOLEAN && right->type == TYPE_BOOLEAN) {
                left = new DisjunctionExpression(left, right);
            } else {
                error(tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

static const Expression *parseExpression(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    return parseDisjunction(scope, tokens, i);
}

static const std::vector<Variable *> parseVariableDeclaration(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    std::vector<std::string> names;
    while (true) {
        if (tokens[i].type != IDENTIFIER) {
            error(tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (scope->names.find(name) != scope->names.end()) {
            error(tokens[i], "duplicate identifer: " + name);
        }
        names.push_back(name);
        ++i;
        if (tokens[i].type != COMMA) {
            break;
        }
        ++i;
    }
    if (tokens[i].type != COLON) {
        error(tokens[i], "colon expected");
    }
    ++i;
    const Type *t = parseType(scope, tokens, i);
    std::vector<Variable *> r;
    for (auto name: names) {
        Variable *v;
        if (scope == g_GlobalScope) {
            v = new GlobalVariable(name, t);
        } else {
            v = new LocalVariable(name, t, scope);
        }
        scope->names[name] = v;
        r.push_back(v);
    }
    return r;
}

static const VariableReference *parseVariableReference(Scope *scope, const std::vector<Token> &tokens, std::vector<Token>::size_type &i)
{
    if (tokens[i].type == IDENTIFIER) {
        const Name *name = scope->lookupName(tokens[i].text);
        if (name == nullptr) {
            error(tokens[i], "name not found: " + tokens[i].text);
        }
        const Constant *cons = dynamic_cast<const Constant *>(name);
        if (cons != nullptr) {
            ++i;
            return new ConstantReference(cons);
        }
        const Variable *var = dynamic_cast<const Variable *>(name);
        if (var == nullptr) {
            error(tokens[i], "name is not a variable: " + tokens[i].text);
        }
        const VariableReference *ref = new ScalarVariableReference(var);
        const Type *type = var->type;
        ++i;
        while (true) {
            if (tokens[i].type == LBRACKET) {
                const TypeArray *arraytype = dynamic_cast<const TypeArray *>(type);
                const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(type);
                if (arraytype != nullptr) {
                    ++i;
                    const Expression *index = parseExpression(scope, tokens, i);
                    if (index->type != TYPE_NUMBER) {
                        error(tokens[i], "index must be a number");
                    }
                    if (tokens[i].type != RBRACKET) {
                        error(tokens[i], "']' expected");
                    }
                    ++i;
                    type = arraytype->elementtype;
                    ref = new ArrayReference(type, ref, index);
                } else if (dicttype != nullptr) {
                    ++i;
                    const Expression *index = parseExpression(scope, tokens, i);
                    if (index->type != TYPE_STRING) {
                        error(tokens[i], "index must be a string");
                    }
                    if (tokens[i].type != RBRACKET) {
                        error(tokens[i], "']' expected");
                    }
                    ++i;
                    type = dicttype->elementtype;
                    ref = new DictionaryReference(type, ref, index);
                } else {
                    error(tokens[i], "not an array or dictionary");
                }
            } else if (tokens[i].type == DOT) {
                const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(type);
                if (recordtype != nullptr) {
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(tokens[i], "identifier expected");
                    }
                    std::map<std::string, std::pair<int, const Type *> >::const_iterator f = recordtype->fields.find(tokens[i].text);
                    if (f == recordtype->fields.end()) {
                        error(tokens[i], "field not found");
                    }
                    ++i;
                    type = f->second.second;
                    ref = new ArrayReference(type, ref, new ConstantNumberExpression(number_from_uint32(f->second.first)));
                } else {
                    error(tokens[i], "not a record");
                }
            } else {
                break;
            }
        }
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
    std::vector<Variable *> args;
    Scope *newscope = new Scope(scope);
    if (tokens[i].type != RPAREN) {
        while (true) {
            std::vector<Variable *> vars = parseVariableDeclaration(newscope, tokens, i);
            std::copy(vars.begin(), vars.end(), std::back_inserter(args));
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
    const Type *returntype = parseType(newscope, tokens, i);
    Function *function = new Function(name, returntype, newscope, args);
    while (tokens[i].type != END) {
        const Statement *s = parseStatement(newscope, tokens, i);
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
    auto j = i;
    const Expression *cond = parseExpression(scope, tokens, i);
    if (cond->type != TYPE_BOOLEAN) {
        error(tokens[j], "boolean value expected");
    }
    if (tokens[i].type != THEN) {
        error(tokens[i], "THEN expected");
    }
    ++i;
    std::vector<const Statement *> then_statements;
    std::vector<const Statement *> else_statements;
    while (tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope, tokens, i);
        if (s != nullptr) {
            then_statements.push_back(s);
        }
    }
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope, tokens, i);
            if (s != nullptr) {
                else_statements.push_back(s);
            }
        }
    }
    if (tokens[i].type != END) {
        error(tokens[i], "END expected");
    }
    ++i;
    return new IfStatement(cond, then_statements, else_statements);
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
    auto j = i;
    const Expression *cond = parseExpression(scope, tokens, i);
    if (cond->type != TYPE_BOOLEAN) {
        error(tokens[j], "boolean value expected");
    }
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
    if (tokens[i].type == TYPE) {
        return parseTypeDefinition(scope, tokens, i);
    } else if (tokens[i].type == CONST) {
        return parseConstantDefinition(scope, tokens, i);
    } else if (tokens[i].type == FUNCTION) {
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
            if (dynamic_cast<const ConstantReference *>(ref) != nullptr) {
                // TODO: there is probably a better way to detect this.
                error(tokens[op], "name is not a variable");
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
    g_GlobalScope = program->scope;
    std::vector<Token>::size_type i = 0;
    while (tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(program->scope, tokens, i);
        if (s != nullptr) {
            program->statements.push_back(s);
        }
    }
    return program;
}
