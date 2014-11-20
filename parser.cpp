#include "parser.h"

#include <iso646.h>
#include <list>

#include "ast.h"
#include "rtl.h"
#include "util.h"

class Parser {
public:
    Parser(const std::vector<Token> &tokens);

    const std::vector<Token> tokens;
    static Scope *global_scope; // TODO: static is a hack, used by StringReference constructor
    std::vector<Token>::size_type i;

    std::stack<std::list<std::pair<TokenType, unsigned int>>> loops;

    typedef std::pair<std::vector<std::string>, const Type *> VariableInfo;

    const Type *parseArrayType(Scope *scope);
    const Type *parseDictionaryType(Scope *scope);
    const Type *parseRecordType(Scope *scope);
    const Type *parseEnumType(Scope *scope);
    const Type *parseType(Scope *scope, bool allow_nothing = false);
    const Statement *parseTypeDefinition(Scope *scope);
    const Statement *parseConstantDefinition(Scope *scope);
    const FunctionCall *parseFunctionCall(const VariableReference *ref, Scope *scope);
    const ArrayLiteralExpression *parseArrayLiteral(Scope *scope);
    const DictionaryLiteralExpression *parseDictionaryLiteral(Scope *scope);
    const Expression *parseAtom(Scope *scope);
    const Expression *parseExponentiation(Scope *scope);
    const Expression *parseMultiplication(Scope *scope);
    const Expression *parseAddition(Scope *scope);
    const Expression *parseComparison(Scope *scope);
    const Expression *parseConjunction(Scope *scope);
    const Expression *parseDisjunction(Scope *scope);
    const Expression *parseConditional(Scope *scope);
    const Expression *parseExpression(Scope *scope);
    const VariableInfo parseVariableDeclaration(Scope *scope);
    const VariableReference *parseVariableReference(Scope *scope);
    void parseFunctionHeader(Scope *scope, std::string &name, const Type *&returntype, Scope *&newscope, std::vector<FunctionParameter *> &args);
    const Statement *parseFunctionDefinition(Scope *scope);
    const Statement *parseExternalDefinition(Scope *scope);
    const Statement *parseIfStatement(Scope *scope);
    const Statement *parseReturnStatement(Scope *scope);
    const Statement *parseVarStatement(Scope *scope);
    const Statement *parseWhileStatement(Scope *scope);
    const Statement *parseCaseStatement(Scope *scope);
    const Statement *parseForStatement(Scope *scope);
    const Statement *parseExitStatement(Scope *scope);
    const Statement *parseImport(Scope *scope);
    const Statement *parseStatement(Scope *scope);
    const Program *parse();
private:
    Parser(const Parser &);
    Parser &operator=(const Parser &);
};

Scope *Parser::global_scope;

Parser::Parser(const std::vector<Token> &tokens)
  : tokens(tokens),
    i(0),
    loops()
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
            error(2000, token, "internal error");
    }
}

StringReference::StringReference(const VariableReference *str, const Expression *index)
  : VariableReference(TYPE_STRING, false),
    str(str),
    index(index),
    load(nullptr),
    store(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(new VariableExpression(str));
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        load = new FunctionCall(new ScalarVariableReference(dynamic_cast<const Variable *>(Parser::global_scope->lookupName("substring"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(new VariableExpression(str));
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        store = new FunctionCall(new ScalarVariableReference(dynamic_cast<const Variable *>(Parser::global_scope->lookupName("splice"))), args);
    }
}

const Type *Parser::parseArrayType(Scope *scope)
{
    if (tokens[i].type != ARRAY) {
        error(2001, tokens[i], "Array expected");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(2002, tokens[i], "'<' expected");
    }
    i++;
    const Type *elementtype = parseType(scope);
    if (tokens[i].type != GREATER) {
        error(2003, tokens[i], "'>' expected");
    }
    i++;
    return new TypeArray(elementtype);
}

const Type *Parser::parseDictionaryType(Scope *scope)
{
    if (tokens[i].type != DICTIONARY) {
        error(2004, tokens[i], "Dictionary expected");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(2005, tokens[i], "'<' expected");
    }
    i++;
    const Type *elementtype = parseType(scope);
    if (tokens[i].type != GREATER) {
        error(2006, tokens[i], "'>' expected");
    }
    i++;
    return new TypeDictionary(elementtype);
}

const Type *Parser::parseRecordType(Scope *scope)
{
    if (tokens[i].type != RECORD) {
        error(2007, tokens[i], "RECORD expected");
    }
    i++;
    std::map<std::string, std::pair<int, const Type *> > fields;
    int index = 0;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2008, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (fields.find(name) != fields.end()) {
            error(2009, tokens[i], "duplicate field: " + name);
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(2010, tokens[i], "colon expected");
        }
        ++i;
        const Type *t = parseType(scope);
        fields[name] = std::make_pair(index, t);
        index++;
    }
    i++;
    if (tokens[i].type != RECORD) {
        error(2100, tokens[i], "'RECORD' expected");
    }
    i++;
    return new TypeRecord(fields);
}

const Type *Parser::parseEnumType(Scope *)
{
    if (tokens[i].type != ENUM) {
        error(2011, tokens[i], "ENUM expected");
    }
    i++;
    std::map<std::string, int> names;
    int index = 0;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2012, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (names.find(name) != names.end()) {
            error(2013, tokens[i], "duplicate enum: " + name);
        }
        i++;
        names[name] = index;
        index++;
    }
    i++;
    if (tokens[i].type != ENUM) {
        error(2101, tokens[i], "'ENUM' expected");
    }
    i++;
    return new TypeEnum(names);
}

const Type *Parser::parseType(Scope *scope, bool allow_nothing)
{
    if (tokens[i].type == ARRAY) {
        return parseArrayType(scope);
    }
    if (tokens[i].type == DICTIONARY) {
        return parseDictionaryType(scope);
    }
    if (tokens[i].type == RECORD) {
        return parseRecordType(scope);
    }
    if (tokens[i].type == ENUM) {
        return parseEnumType(scope);
    }
    if (tokens[i].type != IDENTIFIER) {
        error(2014, tokens[i], "identifier expected");
    }
    const Name *name = scope->lookupName(tokens[i].text);
    if (name == nullptr) {
        error(2015, tokens[i], "unknown type name");
    }
    const Type *type = dynamic_cast<const Type *>(name);
    if (type == nullptr) {
        error(2016, tokens[i], "name is not a type");
    }
    if (not allow_nothing && type == TYPE_NOTHING) {
        error(2017, tokens[i], "'Nothing' type is not allowed here");
    }
    i++;
    return type;
}

const Statement *Parser::parseTypeDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2018, tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    if (scope->lookupName(name) != nullptr) {
        error(2019, tokens[i], "name shadows outer");
    }
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2020, tokens[i], "':=' expected");
    }
    ++i;
    const Type *type = parseType(scope);
    scope->addName(name, const_cast<Type *>(type)); // Still ugly.
    return nullptr;
}

const Statement *Parser::parseConstantDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2021, tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    if (scope->lookupName(name) != nullptr) {
        error(2022, tokens[i], "name shadows outer");
    }
    ++i;
    if (tokens[i].type != COLON) {
        error(2023, tokens[i], "':' expected");
    }
    ++i;
    const Type *type = parseType(scope);
    if (tokens[i].type != ASSIGN) {
        error(2024, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *value = parseExpression(scope);
    if (not value->type->is_equivalent(type)) {
        error(2025, tokens[i], "type mismatch");
    }
    if (not value->is_constant) {
        error(2026, tokens[i], "value must be constant");
    }
    scope->addName(name, new Constant(name, value));
    return nullptr;
}

const FunctionCall *Parser::parseFunctionCall(const VariableReference *ref, Scope *scope)
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(ref->type);
    if (ftype == nullptr) {
        error(2027, tokens[i-1], "not a function");
    }
    ++i;
    std::vector<const Type *>::size_type p = 0;
    std::vector<const Expression *> args;
    if (tokens[i].type != RPAREN) {
        for (;;) {
            const Expression *e = parseExpression(scope);
            if (ftype->params[p]->mode != ParameterType::IN) {
                const VariableReference *ref = e->get_reference();
                if (ref == nullptr) {
                    error(2028, tokens[i], "function call argument must be reference: " + e->text());
                }
            }
            if (not e->type->is_equivalent(ftype->params[p]->type)) {
                error(2029, tokens[i], "type mismatch");
            }
            args.push_back(e);
            ++p;
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2030, tokens[i], "')' or ',' expected");
        }
    }
    if (p < ftype->params.size()) {
        error(2031, tokens[i], "not enough arguments");
    }
    ++i;
    return new FunctionCall(ref, args);
}

const ArrayLiteralExpression *Parser::parseArrayLiteral(Scope *scope)
{
    std::vector<const Expression *> elements;
    const Type *elementtype = nullptr;
    while (tokens[i].type != RBRACKET) {
        const Expression *element = parseExpression(scope);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(2138, tokens[i], "type mismatch");
        }
        elements.push_back(element);
        if (tokens[i].type == COMMA) {
            ++i;
        } else if (tokens[i].type != RBRACKET) {
            error(2139, tokens[i], "',' or ']' expected");
        }
    }
    ++i;
    return new ArrayLiteralExpression(elementtype, elements);
}

const DictionaryLiteralExpression *Parser::parseDictionaryLiteral(Scope *scope)
{
    std::vector<std::pair<std::string, const Expression *>> elements;
    std::set<std::string> keys;
    const Type *elementtype = nullptr;
    while (tokens[i].type == STRING) {
        std::string key = tokens[i].text;
        if (not keys.insert(key).second) {
            error(2140, tokens[i], "duplicate key");
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(2126, tokens[i], "':' expected");
        }
        ++i;
        const Expression *element = parseExpression(scope);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(2127, tokens[i], "type mismatch");
        }
        elements.push_back(std::make_pair(key, element));
        if (tokens[i].type == COMMA) {
            ++i;
        }
    }
    return new DictionaryLiteralExpression(elementtype, elements);
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
 *  if       conditional                        parseConditional
 */

const Expression *Parser::parseAtom(Scope *scope)
{
    switch (tokens[i].type) {
        case LPAREN: {
            ++i;
            const Expression *expr = parseExpression(scope);
            if (tokens[i].type != RPAREN) {
                error(2032, tokens[i], ") expected");
            }
            ++i;
            return expr;
        }
        case LBRACKET: {
            ++i;
            const Expression *array = parseArrayLiteral(scope);
            return array;
        }
        case LBRACE: {
            ++i;
            const Expression *dict = parseDictionaryLiteral(scope);
            if (tokens[i].type != RBRACE) {
                error(2128, tokens[i], "'}' expected");
            }
            ++i;
            return dict;
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
            const Expression *factor = parseAtom(scope);
            if (not factor->type->is_equivalent(TYPE_NUMBER)) {
                error(2033, tokens[op], "number required for negation");
            }
            return new UnaryMinusExpression(factor);
        }
        case NOT: {
            auto op = i;
            ++i;
            const Expression *atom = parseAtom(scope);
            if (not atom->type->is_equivalent(TYPE_BOOLEAN)) {
                error(2034, tokens[op], "boolean required for logical not");
            }
            return new LogicalNotExpression(atom);
        }
        case IDENTIFIER: {
            const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(scope->lookupName(tokens[i].text));
            if (enumtype != nullptr) {
                ++i;
                if (tokens[i].type != DOT) {
                    error(2035, tokens[i], "'.' expected");
                }
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2036, tokens[i], "identifier expected");
                }
                auto name = enumtype->names.find(tokens[i].text);
                if (name == enumtype->names.end()) {
                    error(2037, tokens[i], "identifier not member of enum: " + tokens[i].text);
                }
                ++i;
                return new ConstantEnumExpression(enumtype, name->second);
            } else {
                const VariableReference *ref = parseVariableReference(scope);
                if (tokens[i].type == LPAREN) {
                    return parseFunctionCall(ref, scope);
                } else {
                    return new VariableExpression(ref);
                }
            }
        }
        default:
            error(2038, tokens[i], "Expression expected");
    }
}

const Expression *Parser::parseExponentiation(Scope *scope)
{
    const Expression *left = parseAtom(scope);
    for (;;) {
        if (tokens[i].type == EXP) {
            auto op = i;
            ++i;
            const Expression *right = parseAtom(scope);
            if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                left = new ExponentiationExpression(left, right);
            } else {
                error(2039, tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

const Expression *Parser::parseMultiplication(Scope *scope)
{
    const Expression *left = parseExponentiation(scope);
    for (;;) {
        switch (tokens[i].type) {
            case TIMES: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new MultiplicationExpression(left, right);
                } else {
                    error(2040, tokens[op], "type mismatch");
                }
                break;
            }
            case DIVIDE: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new DivisionExpression(left, right);
                } else {
                    error(2041, tokens[op], "type mismatch");
                }
                break;
            }
            case MOD: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new ModuloExpression(left, right);
                } else {
                    error(2042, tokens[op], "type mismatch");
                }
                break;
            }
            default:
                return left;
        }
    }
}

const Expression *Parser::parseAddition(Scope *scope)
{
    const Expression *left = parseMultiplication(scope);
    for (;;) {
        switch (tokens[i].type) {
            case PLUS: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new AdditionExpression(left, right);
                } else if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
                    std::vector<const Expression *> args;
                    args.push_back(left);
                    args.push_back(right);
                    left = new FunctionCall(new ScalarVariableReference(dynamic_cast<const Variable *>(scope->lookupName("concat"))), args);
                } else {
                    error(2043, tokens[op], "type mismatch");
                }
                break;
            }
            case MINUS: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new SubtractionExpression(left, right);
                } else {
                    error(2044, tokens[op], "type mismatch");
                }
                break;
            }
            default:
                return left;
        }
    }
}

const Expression *Parser::parseComparison(Scope *scope)
{
    const Expression *left = parseAddition(scope);
    switch (tokens[i].type) {
        case EQUAL:
        case NOTEQUAL:
        case LESS:
        case GREATER:
        case LESSEQ:
        case GREATEREQ: {
            ComparisonExpression::Comparison comp = comparisonFromToken(tokens[i]);
            auto op = i;
            ++i;
            const Expression *right = parseAddition(scope);
            if (not left->type->is_equivalent(right->type)) {
                error(2045, tokens[op], "type mismatch");
            }
            if (left->type->is_equivalent(TYPE_BOOLEAN)) {
                if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                    error(2046, tokens[op], "comparison not available for Boolean");
                }
                return new BooleanComparisonExpression(left, right, comp);
            } else if (left->type->is_equivalent(TYPE_NUMBER)) {
                return new NumericComparisonExpression(left, right, comp);
            } else if (left->type->is_equivalent(TYPE_STRING)) {
                return new StringComparisonExpression(left, right, comp);
            } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr) {
                if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                    error(2047, tokens[op], "comparison not available for Array");
                }
                return new ArrayComparisonExpression(left, right, comp);
            } else if (dynamic_cast<const TypeDictionary *>(left->type) != nullptr) {
                if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                    error(2048, tokens[op], "comparison not available for Dictionary");
                }
                return new DictionaryComparisonExpression(left, right, comp);
            } else if (dynamic_cast<const TypeRecord *>(left->type) != nullptr) {
                if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                    error(2049, tokens[op], "comparison not available for RECORD");
                }
                return new ArrayComparisonExpression(left, right, comp);
            } else if (dynamic_cast<const TypeEnum *>(left->type) != nullptr) {
                return new NumericComparisonExpression(left, right, comp);
            } else {
                error(2050, tokens[op], "type mismatch");
            }
        }
        default:
            return left;
    }
}

const Expression *Parser::parseConjunction(Scope *scope)
{
    const Expression *left = parseComparison(scope);
    for (;;) {
        if (tokens[i].type == AND) {
            auto op = i;
            ++i;
            const Expression *right = parseComparison(scope);
            if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
                left = new ConjunctionExpression(left, right);
            } else {
                error(2051, tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

const Expression *Parser::parseDisjunction(Scope *scope)
{
    const Expression *left = parseConjunction(scope);
    for (;;) {
        if (tokens[i].type == OR) {
            auto op = i;
            ++i;
            const Expression *right = parseConjunction(scope);
            if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
                left = new DisjunctionExpression(left, right);
            } else {
                error(2052, tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

const Expression *Parser::parseConditional(Scope *scope)
{
    if (tokens[i].type == IF) {
        ++i;
        const Expression *cond = parseExpression(scope);
        if (tokens[i].type != THEN) {
            error(2053, tokens[i], "'THEN' expected");
        }
        ++i;
        const Expression *left = parseExpression(scope);
        if (tokens[i].type != ELSE) {
            error(2054, tokens[i], "'ELSE' expected");
        }
        ++i;
        const Expression *right = parseExpression(scope);
        if (not left->type->is_equivalent(right->type)) {
            error(2055, tokens[i], "type of THEN and ELSE must match");
        }
        return new ConditionalExpression(cond, left, right);
    } else {
        return parseDisjunction(scope);
    }
}

const Expression *Parser::parseExpression(Scope *scope)
{
    return parseConditional(scope);
}

typedef std::pair<std::vector<std::string>, const Type *> VariableInfo;

const VariableInfo Parser::parseVariableDeclaration(Scope *scope)
{
    std::vector<std::string> names;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2056, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (scope->lookupName(name) != nullptr) {
            error(2057, tokens[i], "name shadows outer");
        }
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
    const Type *t = parseType(scope);
    return make_pair(names, t);
}

const VariableReference *Parser::parseVariableReference(Scope *scope)
{
    if (tokens[i].type == IDENTIFIER) {
        const Name *name = scope->lookupName(tokens[i].text);
        if (name == nullptr) {
            error(2059, tokens[i], "name not found: " + tokens[i].text);
        }
        const Constant *cons = dynamic_cast<const Constant *>(name);
        if (cons != nullptr) {
            ++i;
            return new ConstantReference(cons);
        }
        const Module *module = dynamic_cast<const Module *>(name);
        if (module != nullptr) {
            ++i;
            if (tokens[i].type != DOT) {
                error(2060, tokens[i], "'.' expected");
            }
            ++i;
            return parseVariableReference(module->scope);
        }
        const Variable *var = dynamic_cast<const Variable *>(name);
        if (var == nullptr) {
            error(2061, tokens[i], "name is not a variable: " + tokens[i].text);
        }
        const VariableReference *ref = new ScalarVariableReference(var);
        const Type *type = var->type;
        ++i;
        for (;;) {
            if (tokens[i].type == LBRACKET) {
                const TypeArray *arraytype = dynamic_cast<const TypeArray *>(type);
                const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(type);
                if (arraytype != nullptr) {
                    ++i;
                    const Expression *index = parseExpression(scope);
                    if (not index->type->is_equivalent(TYPE_NUMBER)) {
                        error(2062, tokens[i], "index must be a number");
                    }
                    if (tokens[i].type != RBRACKET) {
                        error(2063, tokens[i], "']' expected");
                    }
                    ++i;
                    type = arraytype->elementtype;
                    ref = new ArrayReference(type, ref, index);
                } else if (dicttype != nullptr) {
                    ++i;
                    const Expression *index = parseExpression(scope);
                    if (not index->type->is_equivalent(TYPE_STRING)) {
                        error(2064, tokens[i], "index must be a string");
                    }
                    if (tokens[i].type != RBRACKET) {
                        error(2065, tokens[i], "']' expected");
                    }
                    ++i;
                    type = dicttype->elementtype;
                    ref = new DictionaryReference(type, ref, index);
                } else if (type == TYPE_STRING) {
                    ++i;
                    const Expression *index = parseExpression(scope);
                    if (not index->type->is_equivalent(TYPE_NUMBER)) {
                        error(2066, tokens[i], "index must be a number");
                    }
                    if (tokens[i].type != RBRACKET) {
                        error(2067, tokens[i], "']' expected");
                    }
                    ++i;
                    return new StringReference(ref, index);
                } else {
                    error(2068, tokens[i], "not an array or dictionary");
                }
            } else if (tokens[i].type == DOT) {
                const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(type);
                if (recordtype != nullptr) {
                    ++i;
                    if (tokens[i].type != IDENTIFIER) {
                        error(2069, tokens[i], "identifier expected");
                    }
                    std::map<std::string, std::pair<int, const Type *> >::const_iterator f = recordtype->fields.find(tokens[i].text);
                    if (f == recordtype->fields.end()) {
                        error(2070, tokens[i], "field not found");
                    }
                    ++i;
                    type = f->second.second;
                    ref = new ArrayReference(type, ref, new ConstantNumberExpression(number_from_uint32(f->second.first)));
                } else {
                    error(2071, tokens[i], "not a record");
                }
            } else {
                break;
            }
        }
        return ref;
    } else {
        error(2072, tokens[i], "Identifier expected");
    }
}

void Parser::parseFunctionHeader(Scope *scope, std::string &name, const Type *&returntype, Scope *&newscope, std::vector<FunctionParameter *> &args)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2073, tokens[i], "identifier expected");
    }
    name = tokens[i].text;
    if (scope->lookupName(name) != nullptr) {
        error(2074, tokens[i], "name shadows outer");
    }
    ++i;
    if (tokens[i].type != LPAREN) {
        error(2075, tokens[i], "'(' expected");
    }
    ++i;
    newscope = new Scope(scope);
    if (tokens[i].type != RPAREN) {
        for (;;) {
            ParameterType::Mode mode = ParameterType::IN;
            switch (tokens[i].type) {
                case IN:    mode = ParameterType::IN;       i++; break;
                case INOUT: mode = ParameterType::INOUT;    i++; break;
                case OUT:   mode = ParameterType::OUT;      i++; break;
                default:
                    break;
            }
            const VariableInfo vars = parseVariableDeclaration(newscope);
            for (auto name: vars.first) {
                FunctionParameter *fp = new FunctionParameter(name, vars.second, mode, newscope);
                args.push_back(fp);
                newscope->addName(name, fp, true);
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
    if (tokens[i].type != COLON) {
        error(2077, tokens[i], "':' expected");
    }
    ++i;
    returntype = parseType(newscope, true);
}

const Statement *Parser::parseFunctionDefinition(Scope *scope)
{
    std::string name;
    const Type *returntype;
    Scope *newscope;
    std::vector<FunctionParameter *> args;
    parseFunctionHeader(scope, name, returntype, newscope, args);
    Function *function = new Function(name, returntype, newscope, args);
    scope->addName(name, function);
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    while (tokens[i].type != END) {
        const Statement *s = parseStatement(newscope);
        if (s != nullptr) {
            function->statements.push_back(s);
        }
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2102, tokens[i], "'FUNCTION' expected");
    }
    ++i;
    loops.pop();
    return nullptr;
}

const Statement *Parser::parseExternalDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2122, tokens[i], "FUNCTION expected");
    }
    std::string name;
    const Type *returntype;
    Scope *newscope;
    std::vector<FunctionParameter *> args;
    parseFunctionHeader(scope, name, returntype, newscope, args);
    if (tokens[i].type != LBRACE) {
        error(2123, tokens[i], "{ expected");
    }
    ++i;
    const DictionaryLiteralExpression *dict = parseDictionaryLiteral(scope);
    if (not dict->is_constant) {
        error(2124, tokens[i], "constant dictionary expected");
    }
    if (dynamic_cast<const TypeDictionary *>(dict->elementtype) == nullptr) {
        error(2131, tokens[i], "top level dictionary element not dictionary");
    }
    for (auto elem: dict->dict) {
        if (not dynamic_cast<const DictionaryLiteralExpression *>(elem.second)->elementtype->is_equivalent(TYPE_STRING)) {
            error(2132, tokens[i], "sub level dictionary must have string elements");
        }
    }

    auto klibrary = dict->dict.find("library");
    if (klibrary == dict->dict.end()) {
        error(2133, tokens[i], "\"library\" key not found");
    }
    auto &library_dict = dynamic_cast<const DictionaryLiteralExpression *>(klibrary->second)->dict;
    auto kname = library_dict.find("name");
    if (kname == library_dict.end()) {
        error(2134, tokens[i], "\"name\" key not found in library");
    }
    std::string library_name = kname->second->eval_string();

    auto ktypes = dict->dict.find("types");
    if (ktypes == dict->dict.end()) {
        error(2135, tokens[i], "\"types\" key not found");
    }
    auto &types_dict = dynamic_cast<const DictionaryLiteralExpression *>(ktypes->second)->dict;
    std::map<std::string, std::string> param_types;
    for (auto paramtype: types_dict) {
        param_types[paramtype.first] = paramtype.second->eval_string();
    }

    ExternalFunction *function = new ExternalFunction(name, returntype, newscope, args, library_name, param_types);
    scope->addName(name, function);

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
    return nullptr;
}

const Statement *Parser::parseIfStatement(Scope *scope)
{
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;
    do {
        ++i;
        auto j = i;
        const Expression *cond = parseExpression(scope);
        if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
            error(2078, tokens[j], "boolean value expected");
        }
        if (tokens[i].type != THEN) {
            error(2079, tokens[i], "THEN expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != ELSIF && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope);
            if (s != nullptr) {
                statements.push_back(s);
            }
        }
        condition_statements.push_back(std::make_pair(cond, statements));
    } while (tokens[i].type == ELSIF);
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope);
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
    return new IfStatement(condition_statements, else_statements);
}

const Statement *Parser::parseReturnStatement(Scope *scope)
{
    ++i;
    const Expression *expr = parseExpression(scope);
    // TODO: check return type
    return new ReturnStatement(expr);
}

const Statement *Parser::parseVarStatement(Scope *scope)
{
    ++i;
    const VariableInfo vars = parseVariableDeclaration(scope);
    for (auto name: vars.first) {
        Variable *v;
        if (scope == global_scope) {
            v = new GlobalVariable(name, vars.second);
        } else {
            v = new LocalVariable(name, vars.second, scope);
        }
        scope->addName(name, v);
    }
    return nullptr;
}

const Statement *Parser::parseWhileStatement(Scope *scope)
{
    ++i;
    auto j = i;
    const Expression *cond = parseExpression(scope);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(2081, tokens[j], "boolean value expected");
    }
    if (tokens[i].type != DO) {
        error(2082, tokens[i], "DO expected");
    }
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(WHILE, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
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
    loops.top().pop_back();
    return new WhileStatement(loop_id, cond, statements);
}

const Statement *Parser::parseCaseStatement(Scope *scope)
{
    ++i;
    const Expression *expr = parseExpression(scope);
    if (not expr->type->is_equivalent(TYPE_NUMBER) && not expr->type->is_equivalent(TYPE_STRING) && dynamic_cast<const TypeEnum *>(expr->type) == nullptr) {
        error(2084, tokens[i], "CASE expression must be Number, String, or ENUM");
    }
    std::vector<std::pair<std::vector<const CaseStatement::WhenCondition *>, std::vector<const Statement *>>> clauses;
    while (tokens[i].type == WHEN) {
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
                    const Expression *when = parseExpression(scope);
                    if (not when->type->is_equivalent(expr->type)) {
                        error(2085, tokens[i], "type mismatch");
                    }
                    if (not when->is_constant) {
                        error(2086, tokens[i], "WHEN condition must be constant");
                    }
                    const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(comparisonFromToken(op), when);
                    for (auto clause: clauses) {
                        for (auto c: clause.first) {
                            if (cond->overlaps(c)) {
                                error(2106, tokens[i], "overlapping case condition");
                            }
                        }
                    }
                    for (auto c: conditions) {
                        if (cond->overlaps(c)) {
                            error(2107, tokens[i], "overlapping case condition");
                        }
                    }
                    conditions.push_back(cond);
                    break;
                }
                default: {
                    const Expression *when = parseExpression(scope);
                    if (not when->type->is_equivalent(expr->type)) {
                        error(2087, tokens[i], "type mismatch");
                    }
                    if (not when->is_constant) {
                        error(2088, tokens[i], "WHEN condition must be constant");
                    }
                    if (tokens[i].type == DOTDOT) {
                        ++i;
                        const Expression *when2 = parseExpression(scope);
                        if (not when2->type->is_equivalent(expr->type)) {
                            error(2089, tokens[i], "type mismatch");
                        }
                        if (not when2->is_constant) {
                            error(2090, tokens[i], "WHEN condition must be constant");
                        }
                        const CaseStatement::WhenCondition *cond = new CaseStatement::RangeWhenCondition(when, when2);
                        for (auto clause: clauses) {
                            for (auto c: clause.first) {
                                if (cond->overlaps(c)) {
                                    error(2108, tokens[i], "overlapping case condition");
                                }
                            }
                        }
                        for (auto c: conditions) {
                            if (cond->overlaps(c)) {
                                error(2109, tokens[i], "overlapping case condition");
                            }
                        }
                        conditions.push_back(cond);
                    } else {
                        const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(ComparisonExpression::EQ, when);
                        for (auto clause: clauses) {
                            for (auto c: clause.first) {
                                if (cond->overlaps(c)) {
                                    error(2110, tokens[i], "overlapping case condition");
                                }
                            }
                        }
                        for (auto c: conditions) {
                            if (cond->overlaps(c)) {
                                error(2111, tokens[i], "overlapping case condition");
                            }
                        }
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
        while (tokens[i].type != WHEN && tokens[i].type != ELSE && tokens[i].type != END) {
            const Statement *stmt = parseStatement(scope);
            statements.push_back(stmt);
        }
        clauses.push_back(std::make_pair(conditions, statements));
    }
    std::vector<const Statement *> else_statements;
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END) {
            const Statement *stmt = parseStatement(scope);
            else_statements.push_back(stmt);
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
    return new CaseStatement(expr, clauses);
}

namespace overlap {

static bool operator==(const Number &x, const Number &y) { return number_is_equal(x, y); }
static bool operator!=(const Number &x, const Number &y) { return number_is_not_equal(x, y); }
static bool operator<(const Number &x, const Number &y) { return number_is_less(x, y); }
static bool operator>(const Number &x, const Number &y) { return number_is_greater(x, y); }
static bool operator<=(const Number &x, const Number &y) { return number_is_less_equal(x, y); }
static bool operator>=(const Number &x, const Number &y) { return number_is_greater_equal(x, y); }

template <typename T> bool check(ComparisonExpression::Comparison comp1, const T &value1, ComparisonExpression::Comparison comp2, const T &value2)
{
    switch (comp1) {
        case ComparisonExpression::EQ:
            switch (comp2) {
                case ComparisonExpression::EQ:
                    return value1 == value2;
                case ComparisonExpression::NE:
                    return value1 != value2;
                case ComparisonExpression::LT:
                    return value1 < value2;
                case ComparisonExpression::GT:
                    return value1 > value2;
                case ComparisonExpression::LE:
                    return value1 <= value2;
                case ComparisonExpression::GE:
                    return value1 >= value2;
            }
            break;
        case ComparisonExpression::NE:
            return false; // TODO
        case ComparisonExpression::LT:
            return false; // TODO
        case ComparisonExpression::GT:
            return false; // TODO
        case ComparisonExpression::LE:
            return false; // TODO
        case ComparisonExpression::GE:
            return false; // TODO
    }
    return false;
}

template <typename T> bool check(ComparisonExpression::Comparison /*comp1*/, const T &/*value1*/, const T &/*value2low*/, const T &/*value2high*/)
{
    return false; // TODO
}

template <typename T> bool check(const T &/*value1low*/, const T &/*value1high*/, const T &/*value2low*/, const T &/*value2high*/)
{
    return false; // TODO
}

} // namespace overlap

bool CaseStatement::ComparisonWhenCondition::overlaps(const WhenCondition *cond) const
{
    const ComparisonWhenCondition *cwhen = dynamic_cast<const ComparisonWhenCondition *>(cond);
    const RangeWhenCondition *rwhen = dynamic_cast<const RangeWhenCondition *>(cond);
    if (cwhen != nullptr) {
        if (expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(), cwhen->comp, cwhen->expr->eval_number());
        } else if (expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(comp, expr->eval_string(), cwhen->comp, cwhen->expr->eval_string());
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(), rwhen->low_expr->eval_number(), rwhen->high_expr->eval_number());
        } else if (expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(comp, expr->eval_string(), rwhen->low_expr->eval_string(), rwhen->high_expr->eval_string());
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else {
        internal_error("ComparisonWhenCondition");
    }
}

bool CaseStatement::RangeWhenCondition::overlaps(const WhenCondition *cond) const
{
    const ComparisonWhenCondition *cwhen = dynamic_cast<const ComparisonWhenCondition *>(cond);
    const RangeWhenCondition *rwhen = dynamic_cast<const RangeWhenCondition *>(cond);
    if (cwhen != nullptr) {
        if (low_expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_number(), low_expr->eval_number(), high_expr->eval_number());
        } else if (low_expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_string(), low_expr->eval_string(), high_expr->eval_string());
        } else {
            internal_error("RangeWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (low_expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(low_expr->eval_number(), high_expr->eval_number(), rwhen->low_expr->eval_number(), rwhen->high_expr->eval_number());
        } else if (low_expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(low_expr->eval_string(), high_expr->eval_string(), rwhen->low_expr->eval_string(), rwhen->high_expr->eval_string());
        } else {
            internal_error("RangeWhenCondition");
        }
    } else {
        internal_error("RangeWhenCondition");
    }
}

const Statement *Parser::parseForStatement(Scope *scope)
{
    ++i;
    const VariableReference *var = parseVariableReference(scope);
    if (not var->type->is_equivalent(TYPE_NUMBER)) {
        error(2112, tokens[i], "type mismatch");
    }
    if (tokens[i].type != IN) {
        error(2121, tokens[i], "'IN' expected");
    }
    ++i;
    const Expression *start = parseExpression(scope);
    if (not start->type->is_equivalent(TYPE_NUMBER)) {
        error(2113, tokens[i], "numeric expression expected");
    }
    if (tokens[i].type != TO) {
        error(2114, tokens[i], "TO expected");
    }
    ++i;
    const Expression *end = parseExpression(scope);
    if (not end->type->is_equivalent(TYPE_NUMBER)) {
        error(2115, tokens[i], "numeric expression expected");
    }
    if (tokens[i].type != DO) {
        error(2118, tokens[i], "'DO' expected");
    }
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(FOR, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
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
    loops.top().pop_back();
    return new ForStatement(loop_id, var, start, end, statements);
}

const Statement *Parser::parseExitStatement(Scope *)
{
    ++i;
    if (tokens[i].type != WHILE
     && tokens[i].type != FOR) {
        error(2136, tokens[i], "loop type expected");
    }
    TokenType type = tokens[i].type;
    ++i;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new ExitStatement(j->second);
            }
        }
    }
    error(2137, tokens[i-1], "no matching loop found in current scope");
}

const Statement *Parser::parseImport(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2093, tokens[i], "identifier expected");
    }
    rtl_import(scope, tokens[i].text);
    ++i;
    return nullptr;
}

const Statement *Parser::parseStatement(Scope *scope)
{
    if (tokens[i].type == IMPORT) {
        return parseImport(scope);
    } else if (tokens[i].type == TYPE) {
        return parseTypeDefinition(scope);
    } else if (tokens[i].type == CONST) {
        return parseConstantDefinition(scope);
    } else if (tokens[i].type == FUNCTION) {
        return parseFunctionDefinition(scope);
    } else if (tokens[i].type == EXTERNAL) {
        return parseExternalDefinition(scope);
    } else if (tokens[i].type == IF) {
        return parseIfStatement(scope);
    } else if (tokens[i].type == RETURN) {
        return parseReturnStatement(scope);
    } else if (tokens[i].type == VAR) {
        return parseVarStatement(scope);
    } else if (tokens[i].type == WHILE) {
        return parseWhileStatement(scope);
    } else if (tokens[i].type == CASE) {
        return parseCaseStatement(scope);
    } else if (tokens[i].type == FOR) {
        return parseForStatement(scope);
    } else if (tokens[i].type == EXIT) {
        return parseExitStatement(scope);
    } else if (tokens[i].type == IDENTIFIER) {
        const VariableReference *ref = parseVariableReference(scope);
        if (tokens[i].type == ASSIGN) {
            auto op = i;
            ++i;
            const Expression *expr = parseExpression(scope);
            if (not expr->type->is_equivalent(ref->type)) {
                error(2094, tokens[op], "type mismatch");
            }
            if (dynamic_cast<const ConstantReference *>(ref) != nullptr) {
                // TODO: there is probably a better way to detect this.
                error(2095, tokens[op], "name is not a variable");
            }
            return new AssignmentStatement(ref, expr);
        } else if (tokens[i].type == LPAREN) {
            const FunctionCall *fc = parseFunctionCall(ref, scope);
            if (fc->type != TYPE_NOTHING) {
                error(2096, tokens[i], "return value unused");
            }
            return new ExpressionStatement(fc);
        } else if (tokens[i].type == EQUAL) {
            error(2097, tokens[i], "':=' expected");
        } else {
            error(2098, tokens[i], "Unexpected");
        }
    } else {
        error(2099, tokens[i], "Identifier expected");
    }
}

const Program *Parser::parse()
{
    Program *program = new Program();
    global_scope = program->scope;
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    while (tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(program->scope);
        if (s != nullptr) {
            program->statements.push_back(s);
        }
    }
    loops.pop();
    return program;
}

const Program *parse(const std::vector<Token> &tokens)
{
    return Parser(tokens).parse();
}
