#include "parser.h"

#include <iso646.h>
#include <list>
#include <stack>

#include "ast.h"
#include "format.h"
#include "rtl_compile.h"
#include "util.h"

class Parser {
public:
    Parser(const std::vector<Token> &tokens);

    const std::vector<Token> tokens;
    static Scope *global_scope; // TODO: static is a hack, used by StringReference constructor
    std::vector<Token>::size_type i;
    int expression_depth;

    std::stack<const TypeFunction *> functiontypes;
    std::stack<std::list<std::pair<TokenType, unsigned int>>> loops;

    typedef std::pair<std::vector<std::string>, const Type *> VariableInfo;

    const Type *parseArrayType(Scope *scope);
    const Type *parseDictionaryType(Scope *scope);
    const Type *parseRecordType(Scope *scope);
    const Type *parseEnumType(Scope *scope);
    const Type *parsePointerType(Scope *scope);
    const Type *parseType(Scope *scope);
    const Statement *parseTypeDefinition(Scope *scope);
    const Statement *parseConstantDefinition(Scope *scope);
    const FunctionCall *parseFunctionCall(const Expression *func, const Expression *self, Scope *scope);
    const ArrayLiteralExpression *parseArrayLiteral(Scope *scope);
    const DictionaryLiteralExpression *parseDictionaryLiteral(Scope *scope);
    const Name *parseQualifiedName(Scope *scope, int &enclosing);
    const Expression *parseInterpolatedStringExpression(Scope *scope);
    const Expression *parseAtom(Scope *scope);
    const Expression *parseExponentiation(Scope *scope);
    const Expression *parseMultiplication(Scope *scope);
    const Expression *parseAddition(Scope *scope);
    const Expression *parseComparison(Scope *scope);
    const Expression *parseMembership(Scope *scope);
    const Expression *parseConjunction(Scope *scope);
    const Expression *parseDisjunction(Scope *scope);
    const Expression *parseConditional(Scope *scope);
    const Expression *parseExpression(Scope *scope, bool allow_nothing = false);
    const VariableInfo parseVariableDeclaration(Scope *scope);
    void parseFunctionHeader(Scope *scope, Type *&type, std::string &name, const Type *&returntype, Scope *&newscope, std::vector<FunctionParameter *> &args);
    const Statement *parseFunctionDefinition(Scope *scope);
    const Statement *parseExternalDefinition(Scope *scope);
    const Statement *parseDeclaration(Scope *scope);
    const Statement *parseIfStatement(Scope *scope, int line);
    const Statement *parseReturnStatement(Scope *scope, int line);
    const Statement *parseVarStatement(Scope *scope, int line);
    const Statement *parseWhileStatement(Scope *scope, int line);
    const Statement *parseCaseStatement(Scope *scope, int line);
    const Statement *parseForStatement(Scope *scope, int line);
    const Statement *parseLoopStatement(Scope *scope, int line);
    const Statement *parseRepeatStatement(Scope *scope, int line);
    const Statement *parseExitStatement(Scope *scope, int line);
    const Statement *parseNextStatement(Scope *scope, int line);
    const Statement *parseTryStatement(Scope *scope, int line);
    const Statement *parseRaiseStatement(Scope *scope, int line);
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
    expression_depth(0),
    functiontypes(),
    loops()
{
}

TypeEnum::TypeEnum(const std::map<std::string, int> &names)
  : TypeNumber(),
    names(names)
{
    {
        Scope *newscope = new Scope(Parser::global_scope);
        std::vector<FunctionParameter *> params;
        FunctionParameter *fp = new FunctionParameter("self", this, ParameterType::INOUT, newscope);
        params.push_back(fp);
        Function *f = new Function("enum.to_string", TYPE_STRING, newscope, params);
        f->scope->addName("self", fp, true);
        std::vector<const Expression *> values;
        for (auto n: names) {
            if (n.second < 0) {
                internal_error("TypeEnum");
            }
            if (values.size() < static_cast<size_t>(n.second)+1) {
                values.resize(n.second+1);
            }
            if (values[n.second] != nullptr) {
                internal_error("TypeEnum");
            }
            values[n.second] = new ConstantStringExpression(n.first);
        }
        f->statements.push_back(new ReturnStatement(0, new ArrayValueIndexExpression(TYPE_STRING, new ArrayLiteralExpression(TYPE_STRING, values), new VariableExpression(fp))));
        methods["to_string"] = f;
    }
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

StringReferenceIndexExpression::StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *index)
  : ReferenceExpression(ref->type, ref->is_readonly),
    ref(ref),
    index(index),
    load(nullptr),
    store(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Parser::global_scope->lookupName("substring"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Parser::global_scope->lookupName("splice"))), args);
    }
}

StringValueIndexExpression::StringValueIndexExpression(const Expression *str, const Expression *index)
  : Expression(str->type, str->is_readonly),
    str(str),
    index(index),
    load(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(str);
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Parser::global_scope->lookupName("substring"))), args);
    }
}

const Type *Parser::parseArrayType(Scope *scope)
{
    if (tokens[i].type != ARRAY) {
        internal_error("Array expected");
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
        internal_error("Dictionary expected");
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
        internal_error("RECORD expected");
    }
    i++;
    std::vector<std::pair<std::string, const Type *>> fields;
    std::set<std::string> field_names;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2008, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (field_names.find(name) != field_names.end()) {
            error(2009, tokens[i], "duplicate field: " + name);
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(2010, tokens[i], "colon expected");
        }
        ++i;
        const Type *t = parseType(scope);
        fields.push_back(std::make_pair(name, t));
        field_names.insert(name);
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
        internal_error("ENUM expected");
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

const Type *Parser::parsePointerType(Scope *scope)
{
    if (tokens[i].type != POINTER) {
        internal_error("POINTER expected");
    }
    i++;
    if (tokens[i].type == TO) {
        i++;
        if (tokens[i].type == IDENTIFIER && scope->lookupName(tokens[i].text) == nullptr) {
            const std::string name = tokens[i].text;
            i++;
            TypePointer *ptrtype = new TypePointer(new TypeForwardRecord());
            scope->addForward(name, ptrtype);
            return ptrtype;
        } else {
            const Type *reftype = parseType(scope);
            const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(reftype);
            if (rectype == nullptr) {
                error(2171, tokens[i], "record type expected");
            }
            return new TypePointer(rectype);
        }
    } else {
        return new TypePointer(nullptr);
    }
}

const Type *Parser::parseType(Scope *scope)
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
    if (tokens[i].type == POINTER) {
        return parsePointerType(scope);
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
    const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
    if (rectype != nullptr) {
        scope->resolveForward(name, rectype);
    }
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

const FunctionCall *Parser::parseFunctionCall(const Expression *func, const Expression *self, Scope *scope)
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        error(2027, tokens[i-1], "not a function");
    }
    ++i;
    std::vector<const Type *>::size_type p = 0;
    std::vector<const Expression *> args;
    if (self != nullptr) {
        args.push_back(self);
        ++p;
    }
    if (tokens[i].type != RPAREN) {
        for (;;) {
            const Expression *e = parseExpression(scope);
            if (p >= ftype->params.size()) {
                error(2167, tokens[i], "too many parameters");
            }
            if (ftype->params[p]->mode != ParameterType::IN) {
                const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
                if (ref == nullptr) {
                    error(2028, tokens[i], "function call argument must be reference: " + e->text());
                }
                if (ref != nullptr && ref->is_readonly) {
                    error(2181, tokens[i], "readonly parameter to OUT");
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
    return new FunctionCall(func, args);
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

const Name *Parser::parseQualifiedName(Scope *scope, int &enclosing)
{
    const Name *name = scope->lookupName(tokens[i].text, enclosing);
    if (name == nullptr) {
        error(2059, tokens[i], "name not found: " + tokens[i].text);
    }
    const Module *module = dynamic_cast<const Module *>(name);
    if (module != nullptr) {
        ++i;
        if (tokens[i].type != DOT) {
            error(2060, tokens[i], "'.' expected");
        }
        ++i;
        return parseQualifiedName(module->scope, enclosing);
    }
    return name;
}

const Expression *Parser::parseInterpolatedStringExpression(Scope *scope)
{
    const VariableExpression *concat = new VariableExpression(dynamic_cast<const Variable *>(scope->lookupName("concat")));
    const VariableExpression *format = new VariableExpression(dynamic_cast<const Variable *>(scope->lookupName("format")));
    const Expression *expr = new ConstantStringExpression(tokens[i].text);
    for (;;) {
        ++i;
        if (tokens[i].type != SUBBEGIN) {
            break;
        }
        ++i;
        const Expression *e = parseExpression(scope);
        std::string fmt;
        if (tokens[i].type == SUBFMT) {
            ++i;
            if (tokens[i].type != STRING) {
                internal_error("parseInterpolatedStringExpression");
            }
            fmt = tokens[i].text;
            format::Spec spec;
            if (not format::parse(fmt, spec)) {
                error(2218, tokens[i], "invalid format specification");
            }
            ++i;
        }
        if (tokens[i].type != SUBEND) {
            internal_error("parseInterpolatedStringExpression");
        }
        ++i;
        const Expression *str;
        if (e->type->is_equivalent(TYPE_STRING)) {
            str = e;
        } else {
            auto to_string = e->type->methods.find("to_string");
            if (to_string == e->type->methods.end()) {
                error(2217, tokens[i], "no to_string() method found for type");
            }
            {
                std::vector<const Expression *> args;
                args.push_back(e);
                str = new FunctionCall(new VariableExpression(to_string->second), args);
            }
        }
        if (not fmt.empty()) {
            std::vector<const Expression *> args;
            args.push_back(str);
            args.push_back(new ConstantStringExpression(fmt));
            str = new FunctionCall(format, args);
        }
        {
            std::vector<const Expression *> args;
            args.push_back(expr);
            args.push_back(str);
            expr = new FunctionCall(concat, args);
        }
        if (tokens[i].type != STRING) {
            internal_error("parseInterpolatedStringExpression");
        }
        e = new ConstantStringExpression(tokens[i].text);
        {
            std::vector<const Expression *> args;
            args.push_back(expr);
            args.push_back(e);
            expr = new FunctionCall(concat, args);
        }
    }
    return expr;
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
            if (tokens[i+1].type == SUBBEGIN) {
                return parseInterpolatedStringExpression(scope);
            } else {
                return new ConstantStringExpression(tokens[i++].text);
            }
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
        case NEW: {
            ++i;
            const TypeRecord *type = dynamic_cast<const TypeRecord *>(parseType(scope));
            if (type == nullptr) {
                error(2172, tokens[i], "record type expected");
            }
            return new NewRecordExpression(type);
        }
        case NIL: {
            ++i;
            return new ConstantNilExpression();
        }
        case IDENTIFIER: {
            const Name *name = scope->lookupName(tokens[i].text);
            const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(name);
            const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(name);
            /* TODO: This allows referencing enum values for a variable
                     declared with an anonymous enum type. But it currently
                     conflicts with method call syntax like a.to_string().
                     See: t/enum2.neon
            if (name != nullptr && enumtype == nullptr && tokens[i+1].type == DOT) {
                enumtype = dynamic_cast<const TypeEnum *>(name->type);
            }*/
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
            } else if (recordtype != nullptr) {
                ++i;
                if (tokens[i].type != LPAREN) {
                    error(2213, tokens[i], "'(' expected");
                }
                ++i;
                std::vector<const Expression *> elements;
                auto f = recordtype->fields.begin();
                while (tokens[i].type != RPAREN) {
                    if (f == recordtype->fields.end()) {
                        error(2214, tokens[i], "too many fields");
                    }
                    const Expression *element = parseExpression(scope);
                    if (not element->type->is_equivalent(f->second)) {
                        error(2215, tokens[i], "type mismatch");
                    }
                    elements.push_back(element);
                    if (tokens[i].type == COMMA) {
                        ++i;
                        ++f;
                    } else if (tokens[i].type != RPAREN) {
                        error(2216, tokens[i], "',' or ']' expected");
                    }
                }
                ++i;
                return new RecordLiteralExpression(recordtype, elements);
            } else {
                int enclosing;
                const Name *name = parseQualifiedName(scope, enclosing);
                const Constant *cons = dynamic_cast<const Constant *>(name);
                // TODO: Need to look up methods on constants too (t/const-boolean.neon)
                if (cons != nullptr) {
                    ++i;
                    return cons->value;
                }
                const Variable *var = dynamic_cast<const Variable *>(name);
                if (var == nullptr) {
                    error(2061, tokens[i], "name is not a variable: " + tokens[i].text);
                }
                const Expression *expr = new VariableExpression(var);
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
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new ArrayReferenceIndexExpression(type, ref, index);
                            } else {
                                expr = new ArrayValueIndexExpression(type, expr, index);
                            }
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
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new DictionaryReferenceIndexExpression(type, ref, index);
                            } else {
                                expr = new DictionaryValueIndexExpression(type, expr, index);
                            }
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
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new StringReferenceIndexExpression(ref, index);
                            } else {
                                expr = new StringValueIndexExpression(expr, index);
                            }
                        } else {
                            error(2068, tokens[i], "not an array or dictionary");
                        }
                    } else if (tokens[i].type == LPAREN) {
                        expr = parseFunctionCall(expr, nullptr, scope);
                        type = expr->type;
                    } else if (tokens[i].type == DOT) {
                        ++i;
                        if (tokens[i].type != IDENTIFIER) {
                            error(2069, tokens[i], "identifier expected");
                        }
                        const std::string field = tokens[i].text;
                        auto m = type->methods.find(field);
                        const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(type);
                        if (m != type->methods.end()) {
                            ++i;
                            if (tokens[i].type != LPAREN) {
                                error(2196, tokens[i], "'(' expected");
                            }
                            expr = parseFunctionCall(new VariableExpression(m->second), expr, scope);
                        } else if (recordtype != nullptr) {
                            if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
                                internal_error("record not defined yet");
                            }
                            auto f = recordtype->field_names.find(field);
                            if (f == recordtype->field_names.end()) {
                                error(2070, tokens[i], "field not found");
                            }
                            ++i;
                            type = recordtype->fields[f->second].second;
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))));
                            } else {
                                expr = new ArrayValueIndexExpression(type, expr, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))));
                            }
                        } else {
                            error(2071, tokens[i], "no method found or not a record");
                        }
                    } else if (tokens[i].type == ARROW) {
                        const TypePointer *pointertype = dynamic_cast<const TypePointer *>(type);
                        if (pointertype != nullptr) {
                            if (dynamic_cast<const TypeValidPointer *>(pointertype) == nullptr) {
                                error(2178, tokens[i], "pointer must be a valid pointer");
                            }
                            const TypeRecord *recordtype = pointertype->reftype;
                            if (recordtype == nullptr) {
                                error(2194, tokens[i], "pointer must not be a generic pointer");
                            }
                            ++i;
                            if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
                                error(2179, tokens[i], "record not defined yet");
                            }
                            if (tokens[i].type != IDENTIFIER) {
                                error(2186, tokens[i], "identifier expected");
                            }
                            auto f = recordtype->field_names.find(tokens[i].text);
                            if (f == recordtype->field_names.end()) {
                                error(2187, tokens[i], "field not found");
                            }
                            ++i;
                            type = recordtype->fields[f->second].second;
                            const PointerDereferenceExpression *ref = new PointerDereferenceExpression(type, expr);
                            expr = new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))));
                        } else {
                            error(2188, tokens[i], "not a pointer");
                        }
                    } else {
                        break;
                    }
                }
                return expr;
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
                    error(2206, tokens[op], "type mismatch (use & to concatenate strings)");
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
            case CONCAT: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope);
                if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
                    std::vector<const Expression *> args;
                    args.push_back(left);
                    args.push_back(right);
                    left = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(scope->lookupName("concat"))), args);
                } else {
                    error(2193, tokens[op], "type mismatch");
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
    std::vector<const ComparisonExpression *> comps;
    while (tokens[i].type == EQUAL  || tokens[i].type == NOTEQUAL
        || tokens[i].type == LESS   || tokens[i].type == GREATER
        || tokens[i].type == LESSEQ || tokens[i].type == GREATEREQ) {
        ComparisonExpression::Comparison comp = comparisonFromToken(tokens[i]);
        auto op = i;
        ++i;
        const Expression *right = parseAddition(scope);
        if (not left->type->is_equivalent(right->type)) {
            error(2045, tokens[op], "type mismatch");
        }
        const ComparisonExpression *c;
        if (left->type->is_equivalent(TYPE_BOOLEAN)) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2046, tokens[op], "comparison not available for Boolean");
            }
            c = new BooleanComparisonExpression(left, right, comp);
        } else if (left->type->is_equivalent(TYPE_NUMBER)) {
            c = new NumericComparisonExpression(left, right, comp);
        } else if (left->type->is_equivalent(TYPE_STRING)) {
            c = new StringComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2047, tokens[op], "comparison not available for Array");
            }
            c = new ArrayComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeDictionary *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2048, tokens[op], "comparison not available for Dictionary");
            }
            c = new DictionaryComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeRecord *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2049, tokens[op], "comparison not available for RECORD");
            }
            c = new ArrayComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeEnum *>(left->type) != nullptr) {
            c = new NumericComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypePointer *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2173, tokens[op], "comparison not available for POINTER");
            }
            c = new PointerComparisonExpression(left, right, comp);
        } else {
            internal_error("unknown type in parseComparison");
        }
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

const Expression *Parser::parseMembership(Scope *scope)
{
    const Expression *left = parseComparison(scope);
    if (tokens[i].type == IN) {
        ++i;
        const Expression *right = parseComparison(scope);
        const TypeArray *arraytype = dynamic_cast<const TypeArray *>(right->type);
        const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(right->type);
        if (arraytype != nullptr) {
            if (not left->type->is_equivalent(arraytype->elementtype)) {
                error(2142, tokens[i], "type mismatch");
            }
            return new ArrayInExpression(left, right);
        } else if (dicttype != nullptr) {
            if (not left->type->is_equivalent(TYPE_STRING)) {
                error(2143, tokens[i], "type mismatch");
            }
            return new DictionaryInExpression(left, right);
        } else {
            error(2141, tokens[i], "IN must be used with Array or Dictionary");
        }
    } else {
        return left;
    }
}

const Expression *Parser::parseConjunction(Scope *scope)
{
    const Expression *left = parseMembership(scope);
    for (;;) {
        if (tokens[i].type == AND) {
            auto op = i;
            ++i;
            const Expression *right = parseMembership(scope);
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

const Expression *Parser::parseExpression(Scope *scope, bool allow_nothing)
{
    expression_depth++;
    if (expression_depth > 100) {
        error(2197, tokens[i], "exceeded maximum nesting depth");
    }
    const Expression *r = parseConditional(scope);
    if (not allow_nothing && r->type == TYPE_NOTHING) {
        error(2192, tokens[i], "function does not return anything");
    }
    expression_depth--;
    return r;
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

void Parser::parseFunctionHeader(Scope *scope, Type *&type, std::string &name, const Type *&returntype, Scope *&newscope, std::vector<FunctionParameter *> &args)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2073, tokens[i], "identifier expected");
    }
    name = tokens[i].text;
    ++i;
    type = nullptr;
    if (tokens[i].type == DOT) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2208, tokens[i], "identifier expected");
        }
        Name *tname = scope->lookupName(name);
        if (tname == nullptr) {
            error(2209, tokens[i], "type name not found");
        }
        type = dynamic_cast<Type *>(tname);
        if (type == nullptr) {
            error(2210, tokens[i], "type name is not a type");
        }
        name = tokens[i].text;
        ++i;
    }
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
                if (type != nullptr && args.empty()) {
                    if (not vars.second->is_equivalent(type)) {
                        error(2211, tokens[i], "expected self parameter");
                    }
                }
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
    if (type != nullptr && args.empty()) {
        error(2212, tokens[i], "expected self parameter");
    }
    ++i;
    if (tokens[i].type == COLON) {
        ++i;
        returntype = parseType(newscope);
    } else {
        returntype = TYPE_NOTHING;
    }
}

const Statement *Parser::parseFunctionDefinition(Scope *scope)
{
    Type *type;
    std::string name;
    const Type *returntype;
    Scope *newscope;
    std::vector<FunctionParameter *> args;
    parseFunctionHeader(scope, type, name, returntype, newscope, args);
    Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(name);
        if (f != type->methods.end()) {
            function = dynamic_cast<Function *>(f->second);
            newscope = function->scope;
        } else {
            function = new Function(name, returntype, newscope, args);
            type->methods[name] = function;
        }
    } else {
        Name *ident = scope->lookupName(name);
        function = dynamic_cast<Function *>(ident);
        if (function != nullptr) {
            newscope = function->scope;
        } else if (ident != nullptr) {
            error(2074, tokens[i], "name shadows outer");
        } else {
            function = new Function(name, returntype, newscope, args);
            scope->addName(name, function);
        }
    }
    functiontypes.push(dynamic_cast<const TypeFunction *>(function->type));
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    while (tokens[i].type != END) {
        const Statement *s = parseStatement(newscope);
        if (s != nullptr) {
            function->statements.push_back(s);
        }
    }
    if (returntype != TYPE_NOTHING) {
        if (function->statements.empty() || not function->statements.back()->always_returns()) {
            error(2146, tokens[i], "missing RETURN statement");
        }
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2102, tokens[i], "'FUNCTION' expected");
    }
    ++i;
    loops.pop();
    functiontypes.pop();
    newscope->checkForward();
    return nullptr;
}

const Statement *Parser::parseExternalDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2122, tokens[i], "FUNCTION expected");
    }
    Type *type;
    std::string name;
    const Type *returntype;
    Scope *newscope;
    std::vector<FunctionParameter *> args;
    parseFunctionHeader(scope, type, name, returntype, newscope, args);
    if (scope->lookupName(name) != nullptr) {
        error(2163, tokens[i], "name shadows outer");
    }
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
        auto *d = dynamic_cast<const DictionaryLiteralExpression *>(elem.second);
        if (not d->dict.empty() && not d->elementtype->is_equivalent(TYPE_STRING)) {
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
    for (auto a: args) {
        if (param_types.find(a->name) == param_types.end()) {
            error(2168, tokens[i], "parameter type missing: " + a->name);
        }
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

const Statement *Parser::parseDeclaration(Scope *scope)
{
    ++i;
    switch (tokens[i].type) {
        case EXCEPTION: {
            ++i;
            if (tokens[i].type != IDENTIFIER) {
                error(2152, tokens[i], "identifier expected");
            }
            std::string name = tokens[i].text;
            if (scope->lookupName(name) != nullptr) {
                error(2191, tokens[i], "name shadows outer");
            }
            ++i;
            scope->addName(name, new Exception(name));
            break;
        }
        case FUNCTION: {
            Type *type;
            std::string name;
            const Type *returntype;
            Scope *newscope;
            std::vector<FunctionParameter *> args;
            parseFunctionHeader(scope, type, name, returntype, newscope, args);
            if (scope->lookupName(name) != nullptr) {
                error(2162, tokens[i], "name shadows outer");
            }
            Function *function = new Function(name, returntype, newscope, args);
            scope->addName(name, function);
            break;
        }
        default:
            error(2151, tokens[i], "EXCEPTION or FUNCTION expected");
    }
    return nullptr;
}

const Statement *Parser::parseIfStatement(Scope *scope, int line)
{
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;
    do {
        ++i;
        const Expression *cond = nullptr;
        std::string name;
        if (tokens[i].type == VALID) {
            for (;;) {
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2174, tokens[i], "identifier expected");
                }
                name = tokens[i].text;
                if (scope->lookupName(name) != nullptr) {
                    error(2177, tokens[i], "name shadows outer");
                }
                ++i;
                if (tokens[i].type != ASSIGN) {
                    error(2175, tokens[i], "':=' expected");
                }
                ++i;
                const Expression *ptr = parseExpression(scope);
                const TypePointer *ptrtype = dynamic_cast<const TypePointer *>(ptr->type);
                if (ptrtype == nullptr) {
                    error(2176, tokens[i], "pointer type expression expected");
                }
                const TypeValidPointer *vtype = new TypeValidPointer(ptrtype);
                Variable *v;
                // TODO: Try to make this a local variable always (give the global scope a local space).
                if (functiontypes.empty()) {
                    v = new GlobalVariable(name, vtype, true);
                } else {
                    v = new LocalVariable(name, vtype, scope, true);
                }
                scope->addName(name, v, true);
                const Expression *valid = new ValidPointerExpression(v, ptr);
                if (cond == nullptr) {
                    cond = valid;
                } else {
                    cond = new ConjunctionExpression(cond, valid);
                }
                if (tokens[i].type != COMMA) {
                    break;
                }
            }
        } else {
            auto j = i;
            cond = parseExpression(scope);
            if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
                error(2078, tokens[j], "boolean value expected");
            }
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
        if (not name.empty()) {
            scope->scrubName(name);
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
    return new IfStatement(line, condition_statements, else_statements);
}

const Statement *Parser::parseReturnStatement(Scope *scope, int line)
{
    const auto token_return = tokens[i];
    ++i;
    const Expression *expr = parseExpression(scope);
    if (functiontypes.empty()) {
        error(2164, token_return, "RETURN not allowed outside function");
    } else if (functiontypes.top()->returntype == TYPE_NOTHING) {
        error(2165, token_return, "function does not return a value");
    } else if (not expr->type->is_equivalent(functiontypes.top()->returntype)) {
        error(2166, token_return, "type mismatch in RETURN");
    }
    return new ReturnStatement(line, expr);
}

const Statement *Parser::parseVarStatement(Scope *scope, int line)
{
    ++i;
    std::vector<Variable *> variables;
    const VariableInfo vars = parseVariableDeclaration(scope);
    for (auto name: vars.first) {
        Variable *v;
        if (scope == global_scope) {
            v = new GlobalVariable(name, vars.second, false);
        } else {
            v = new LocalVariable(name, vars.second, scope, false);
        }
        variables.push_back(v);
    }
    const Statement *r = nullptr;
    if (tokens[i].type == ASSIGN) {
        ++i;
        std::vector<const ReferenceExpression *> refs;
        const Expression *expr = parseExpression(scope);
        for (auto v: variables) {
            scope->addName(v->name, v, true);
            refs.push_back(new VariableExpression(v));
        }
        r =  new AssignmentStatement(line, refs, expr);
    } else {
        for (auto v: variables) {
            scope->addName(v->name, v);
        }
    }
    return r;
}

const Statement *Parser::parseWhileStatement(Scope *scope, int line)
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
    return new WhileStatement(line, loop_id, cond, statements);
}

const Statement *Parser::parseCaseStatement(Scope *scope, int line)
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
                    if (tokens[i].type == TO) {
                        ++i;
                        const Expression *when2 = parseExpression(scope);
                        if (not when2->type->is_equivalent(expr->type)) {
                            error(2089, tokens[i], "type mismatch");
                        }
                        if (not when2->is_constant) {
                            error(2090, tokens[i], "WHEN condition must be constant");
                        }
                        if (when->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(when->type) != nullptr) {
                            if (number_is_greater(when->eval_number(), when2->eval_number())) {
                                error(2184, tokens[i], "WHEN numeric range condition must be low..high");
                            }
                        } else if (when->type->is_equivalent(TYPE_STRING)) {
                            if (when->eval_string() > when2->eval_string()) {
                                error(2185, tokens[i], "WHEN string range condition must be low..high");
                            }
                        } else {
                            internal_error("range condition type");
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
        while (tokens[i].type != WHEN && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *stmt = parseStatement(scope);
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
            const Statement *stmt = parseStatement(scope);
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
    return new CaseStatement(line, expr, clauses);
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
                case ComparisonExpression::EQ: return value1 == value2;
                case ComparisonExpression::NE: return value1 != value2;
                case ComparisonExpression::LT: return value1 < value2;
                case ComparisonExpression::GT: return value1 > value2;
                case ComparisonExpression::LE: return value1 <= value2;
                case ComparisonExpression::GE: return value1 >= value2;
            }
            break;
        case ComparisonExpression::NE:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 != value2;
                default: return true;
            }
            break;
        case ComparisonExpression::LT:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 < value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return true;
                case ComparisonExpression::GT: return value1 > value2;
                case ComparisonExpression::LE: return true;
                case ComparisonExpression::GE: return value1 > value2;
            }
            break;
        case ComparisonExpression::GT:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 > value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return value1 < value2;
                case ComparisonExpression::GT: return true;
                case ComparisonExpression::LE: return value1 < value2;
                case ComparisonExpression::GE: return true;
            }
            break;
        case ComparisonExpression::LE:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 >= value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return true;
                case ComparisonExpression::GT: return value1 > value2;
                case ComparisonExpression::LE: return true;
                case ComparisonExpression::GE: return value1 >= value2;
            }
            break;
        case ComparisonExpression::GE:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 <= value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return value1 < value2;
                case ComparisonExpression::GT: return true;
                case ComparisonExpression::LE: return value1 <= value2;
                case ComparisonExpression::GE: return true;
            }
            break;
    }
    return false;
}

template <typename T> bool check(ComparisonExpression::Comparison comp1, const T &value1, const T &value2low, const T &value2high)
{
    switch (comp1) {
        case ComparisonExpression::EQ: return value1 >= value2low && value1 <= value2high;
        case ComparisonExpression::NE: return value1 != value2low || value1 != value2high;
        case ComparisonExpression::LT: return value1 > value2low;
        case ComparisonExpression::GT: return value1 > value2high;
        case ComparisonExpression::LE: return value1 >= value2low;
        case ComparisonExpression::GE: return value1 <= value2high;
    }
    return false;
}

template <typename T> bool check(const T &value1low, const T &value1high, const T &value2low, const T &value2high)
{
    return value1high >= value2low && value1low <= value2high;
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

const Statement *Parser::parseForStatement(Scope *scope, int line)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2072, tokens[i], "identifier expected");
    }
    Name *name = scope->lookupName(tokens[i].text);
    Variable *var = dynamic_cast<Variable *>(name);
    if (var == nullptr) {
        error(2195, tokens[i], "name not a variable: " + tokens[i].text);
    }
    if (not var->type->is_equivalent(TYPE_NUMBER)) {
        error(2112, tokens[i], "type mismatch");
    }
    if (var->is_readonly) {
        error(2207, tokens[i], "cannot use readonly variable in FOR loop");
    }
    var->is_readonly = true;
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2121, tokens[i], "':=' expected");
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

    const Expression *step = nullptr;
    if (tokens[i].type == STEP) {
        ++i;
        step = parseExpression(scope);
        if (step->type != TYPE_NUMBER) {
            error(2116, tokens[i], "numeric expression expected");
        }
        if (not step->is_constant) {
            error(2117, tokens[i], "numeric expression must be constant");
        }
        if (number_is_zero(step->eval_number())) {
            error(2161, tokens[i], "STEP value cannot be zero");
        }
    } else {
        step = new ConstantNumberExpression(number_from_uint32(1));
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
    var->is_readonly = false;
    return new ForStatement(line, loop_id, new VariableExpression(var), start, end, step, statements);
}

const Statement *Parser::parseLoopStatement(Scope *scope, int line)
{
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(LOOP, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
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
    loops.top().pop_back();
    return new LoopStatement(line, loop_id, statements);
}

const Statement *Parser::parseRepeatStatement(Scope *scope, int line)
{
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(REPEAT, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != UNTIL && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != UNTIL) {
        error(2149, tokens[i], "UNTIL expected");
    }
    ++i;
    const Expression *cond = parseExpression(scope);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(2150, tokens[i], "boolean value expected");
    }
    loops.top().pop_back();
    return new RepeatStatement(line, loop_id, cond, statements);
}

const Statement *Parser::parseExitStatement(Scope *, int line)
{
    ++i;
    if (tokens[i].type == FUNCTION) {
        if (functiontypes.empty()) {
            error(2182, tokens[i], "EXIT FUNCTION not allowed outside function");
        } else if (functiontypes.top()->returntype != TYPE_NOTHING) {
            error(2183, tokens[i], "function must return a value");
        }
        i++;
        return new ReturnStatement(line, nullptr);
    }
    if (tokens[i].type != WHILE
     && tokens[i].type != FOR
     && tokens[i].type != LOOP
     && tokens[i].type != REPEAT) {
        error(2136, tokens[i], "loop type expected");
    }
    TokenType type = tokens[i].type;
    ++i;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new ExitStatement(line, j->second);
            }
        }
    }
    error(2137, tokens[i-1], "no matching loop found in current scope");
}

const Statement *Parser::parseNextStatement(Scope *, int line)
{
    ++i;
    if (tokens[i].type != WHILE
     && tokens[i].type != FOR
     && tokens[i].type != LOOP
     && tokens[i].type != REPEAT) {
        error(2144, tokens[i], "loop type expected");
    }
    TokenType type = tokens[i].type;
    ++i;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new NextStatement(line, j->second);
            }
        }
    }
    error(2145, tokens[i-1], "no matching loop found in current scope");
}

const Statement *Parser::parseTryStatement(Scope *scope, int line)
{
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *stmt = parseStatement(scope);
        if (stmt != nullptr) {
            statements.push_back(stmt);
        }
    }
    std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;
    while (tokens[i].type == EXCEPTION) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2153, tokens[i], "identifier expected");
        }
        const Name *name = scope->lookupName(tokens[i].text);
        if (name == nullptr) {
            error(2154, tokens[i], "exception not found: " + tokens[i].text);
        }
        const Exception *exception = dynamic_cast<const Exception *>(name);
        if (exception == nullptr) {
            error(2155, tokens[i], "name not an exception");
        }
        std::vector<const Exception *> exceptions;
        exceptions.push_back(exception);
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *stmt = parseStatement(scope);
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
    return new TryStatement(line, statements, catches);
}

const Statement *Parser::parseRaiseStatement(Scope *scope, int line)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2156, tokens[i], "identifier expected");
    }
    const Name *name = scope->lookupName(tokens[i].text);
    if (name == nullptr) {
        error(2157, tokens[i], "exception not found: " + tokens[i].text);
    }
    const Exception *exception = dynamic_cast<const Exception *>(name);
    if (exception == nullptr) {
        error(2158, tokens[i], "name not an exception");
    }
    ++i;
    const Expression *info = nullptr;
    if (tokens[i].type == LPAREN) {
        info = parseExpression(scope);
    } else {
        info = new ConstantStringExpression("");
    }
    return new RaiseStatement(line, exception, info);
}

const Statement *Parser::parseImport(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2093, tokens[i], "identifier expected");
    }
    if (scope->lookupName(tokens[i].text) != nullptr) {
        error(2190, tokens[i], "name shadows outer");
    }
    rtl_import(scope, tokens[i].text);
    ++i;
    return nullptr;
}

const Statement *Parser::parseStatement(Scope *scope)
{
    const int line = tokens[i].line;
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
    } else if (tokens[i].type == DECLARE) {
        return parseDeclaration(scope);
    } else if (tokens[i].type == IF) {
        return parseIfStatement(scope, line);
    } else if (tokens[i].type == RETURN) {
        return parseReturnStatement(scope, line);
    } else if (tokens[i].type == VAR) {
        return parseVarStatement(scope, line);
    } else if (tokens[i].type == WHILE) {
        return parseWhileStatement(scope, line);
    } else if (tokens[i].type == CASE) {
        return parseCaseStatement(scope, line);
    } else if (tokens[i].type == FOR) {
        return parseForStatement(scope, line);
    } else if (tokens[i].type == LOOP) {
        return parseLoopStatement(scope, line);
    } else if (tokens[i].type == REPEAT) {
        return parseRepeatStatement(scope, line);
    } else if (tokens[i].type == EXIT) {
        return parseExitStatement(scope, line);
    } else if (tokens[i].type == NEXT) {
        return parseNextStatement(scope, line);
    } else if (tokens[i].type == TRY) {
        return parseTryStatement(scope, line);
    } else if (tokens[i].type == RAISE) {
        return parseRaiseStatement(scope, line);
    } else if (tokens[i].type == IDENTIFIER) {
        if (tokens[i].text == "value_copy") {
            ++i;
            if (tokens[i].type != LPAREN) {
                error(2198, tokens[i], "'(' expected");
            }
            ++i;
            auto tok_lhs = tokens[i];
            const Expression *expr = parseExpression(scope);
            const ReferenceExpression *lhs = dynamic_cast<const ReferenceExpression *>(expr);
            if (lhs == nullptr) {
                error(2199, tok_lhs, "expression is not assignable");
            }
            if (expr->is_readonly && dynamic_cast<const TypePointer *>(expr->type) == nullptr) {
                error(2200, tok_lhs, "value_copy to readonly expression");
            }

            if (tokens[i].type != COMMA) {
                error(2201, tokens[i], "',' expected");
            }
            ++i;
            auto tok_rhs = tokens[i];
            const Expression *rhs = parseExpression(scope);
            if (tokens[i].type != RPAREN) {
                error(2202, tokens[i], "')' expected");
            }
            ++i;
            const Type *ltype = lhs->type;
            const TypePointer *lptype = dynamic_cast<const TypePointer *>(ltype);
            if (lptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(lptype) == nullptr) {
                    error(2203, tok_lhs, "valid pointer type required");
                }
                ltype = lptype->reftype;
                lhs = new PointerDereferenceExpression(ltype, lhs);
            }
            const Type *rtype = rhs->type;
            const TypePointer *rptype = dynamic_cast<const TypePointer *>(rtype);
            if (rptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(rptype) == nullptr) {
                    error(2204, tok_rhs, "valid pointer type required");
                }
                rtype = rptype->reftype;
                rhs = new PointerDereferenceExpression(rtype, rhs);
            }
            if (not ltype->is_equivalent(rtype)) {
                error(2205, tok_rhs, "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(lhs);
            return new AssignmentStatement(line, vars, rhs);
        }
        const Expression *expr = parseExpression(scope, true);
        if (expr->type == TYPE_NOTHING) {
            return new ExpressionStatement(line, expr);
        } else if (tokens[i].type == ASSIGN) {
            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
            if (ref == nullptr) {
                error(2095, tokens[i], "expression is not assignable");
            }
            if (expr->is_readonly) {
                error(2180, tokens[i], "assignment to readonly expression");
            }
            auto op = i;
            ++i;
            const Expression *rhs = parseExpression(scope);
            if (not rhs->type->is_equivalent(expr->type)) {
                error(2094, tokens[op], "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(ref);
            return new AssignmentStatement(line, vars, rhs);
        } else if (dynamic_cast<const ComparisonExpression *>(expr) != nullptr) {
            error(2097, tokens[i], "':=' expected");
        } else if (dynamic_cast<const FunctionCall *>(expr) != nullptr) {
            error(2096, tokens[i], "return value unused");
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
    program->scope->checkForward();
    return program;
}

const Program *parse(const std::vector<Token> &tokens)
{
    return Parser(tokens).parse();
}
