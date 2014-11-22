#ifndef AST_H
#define AST_H

#include <iso646.h>
#include <limits.h>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "number.h"
#include "util.h"

// Compiler
class Emitter;

class AstNode {
public:
    AstNode() {}
    virtual ~AstNode() {}
    void dump(std::ostream &out, int depth = 0) const;
    virtual std::string text() const = 0;
    virtual void dumpsubnodes(std::ostream &/*out*/, int /*depth*/) const {}
private:
    AstNode(const AstNode &);
    AstNode &operator=(const AstNode &);
};

class Name;
class Type;
class FunctionCall;
class FunctionParameter;
class VariableReference;

class Scope {
public:
    Scope(Scope *parent): parent(parent), names(), referenced(), count(0) {}
    virtual ~Scope() {}

    virtual void predeclare(Emitter &emitter) const;
    virtual void postdeclare(Emitter &emitter) const;

    const Name *lookupName(const std::string &name);
    void addName(const std::string &name, Name *ref, bool init_referenced = false);
    int nextIndex();
    int getCount() const;

private:
    Scope *const parent;
    std::map<std::string, Name *> names;
    std::set<const Name *> referenced;
    int count;
private:
    Scope(const Scope &);
    Scope &operator=(const Scope &);
};

class Name: public AstNode {
public:
    Name(const std::string &name, const Type *type): name(name), type(type) {}
    const std::string name;
    const Type *type;

    virtual void predeclare(Emitter &) {}
    virtual void postdeclare(Emitter &) {}
private:
    Name(const Name &);
    Name &operator=(const Name &);
};

class Type: public Name {
public:
    Type(const std::string &name): Name(name, nullptr) {}
    virtual bool is_equivalent(const Type *rhs) const { return this == rhs; }
    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;
};

class TypeNothing: public Type {
public:
    TypeNothing(): Type("Nothing") {}
    virtual void generate_load(Emitter &) const {}
    virtual void generate_store(Emitter &) const {}
    virtual void generate_call(Emitter &) const {}

    virtual std::string text() const { return "TypeNothing"; }
};

extern TypeNothing *TYPE_NOTHING;

class TypeBoolean: public Type {
public:
    TypeBoolean(): Type("Boolean") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeBoolean"; }
};

extern TypeBoolean *TYPE_BOOLEAN;

class TypeNumber: public Type {
public:
    TypeNumber(): Type("Number") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeNumber"; }
};

extern TypeNumber *TYPE_NUMBER;

class TypeString: public Type {
public:
    TypeString(): Type("String") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeString"; }
};

extern TypeString *TYPE_STRING;

class ParameterType {
public:
    enum Mode {
        IN,
        INOUT,
        OUT
    };
    ParameterType(Mode mode, const Type *type): mode(mode), type(type) {}
    const Mode mode;
    const Type *type;
private:
    ParameterType(const ParameterType &);
    ParameterType &operator=(const ParameterType &);
};

class TypeFunction: public Type {
public:
    TypeFunction(const Type *returntype, const std::vector<const ParameterType *> &params): Type("function"), returntype(returntype), params(params) {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    const Type *returntype;
    const std::vector<const ParameterType *> params;

    virtual std::string text() const { return "TypeFunction(...)"; }
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const Type *elementtype): Type("array"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeArray(" + elementtype->text() + ")"; }
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const Type *elementtype): Type("dictionary"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeDictionary(" + elementtype->text() + ")"; }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

class TypeRecord: public Type {
public:
    TypeRecord(const std::map<std::string, std::pair<int, const Type *> > &fields): Type("record"), fields(fields) {}
    const std::map<std::string, std::pair<int, const Type *> > fields;

    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeRecord(...)"; }
};

class TypeEnum: public TypeNumber {
public:
    TypeEnum(const std::map<std::string, int> &names): TypeNumber(), names(names) {}
    const std::map<std::string, int> names;

    virtual std::string text() const { return "TypeEnum(...)"; }
};

class TypeModule: public Type {
public:
    TypeModule(): Type("module") {}

    virtual void generate_load(Emitter &) const { internal_error("TypeModule"); }
    virtual void generate_store(Emitter &) const { internal_error("TypeModule"); }
    virtual void generate_call(Emitter &) const { internal_error("TypeModule"); }

    virtual std::string text() const { return "TypeModule(...)"; }
};

extern TypeModule *TYPE_MODULE;

class Variable: public Name {
public:
    Variable(const std::string &name, const Type *type): Name(name, type) {}

    virtual void generate_address(Emitter &emitter) const = 0;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "Variable(" + name + ", " + type->text() + ")"; }
};

class GlobalVariable: public Variable {
public:
    GlobalVariable(const std::string &name, const Type *type): Variable(name, type), index(-1) {}
    int index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "GlobalVariable(" + name + ", " + type->text() + ")"; }
};

class LocalVariable: public Variable {
public:
    LocalVariable(const std::string &name, const Type *type, Scope *scope): Variable(name, type), scope(scope), index(-1) {}
    Scope *scope;
    int index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "LocalVariable(" + name + ", " + type->text() + ")"; }
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class FunctionParameter: public LocalVariable {
public:
    FunctionParameter(const std::string &name, const Type *type, ParameterType::Mode mode, Scope *scope): LocalVariable(name, type, scope), mode(mode) {}
    ParameterType::Mode mode;

    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "FunctionParameter(" + name + ", " + type->text() + ")"; }
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class Expression: public AstNode {
public:
    Expression(const Type *type, bool is_constant): type(type), is_constant(is_constant) {}

    virtual Number eval_number() const = 0;
    virtual std::string eval_string() const = 0;
    virtual const VariableReference *get_reference() const { return nullptr; }
    virtual void generate(Emitter &emitter) const = 0;

    const Type *type;
    const bool is_constant;
private:
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

class Constant: public Name {
public:
    Constant(const std::string &name, const Expression *value): Name(name, value->type), value(value) {}

    const Expression *value;

    virtual std::string text() const { return "Constant(" + name + ", " + value->text() + ")"; }
private:
    Constant(const Constant &);
    Constant &operator=(const Constant &);
};

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(bool value): Expression(TYPE_BOOLEAN, true), value(value) {}

    const bool value;

    virtual Number eval_number() const { internal_error("ConstantBooleanExpression"); }
    virtual std::string eval_string() const { internal_error("ConstantBooleanExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(Number value): Expression(TYPE_NUMBER, true), value(value) {}

    const Number value;

    virtual Number eval_number() const { return value; }
    virtual std::string eval_string() const { internal_error("ConstantNumberExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const std::string &value): Expression(TYPE_STRING, true), value(value) {}

    const std::string value;

    virtual Number eval_number() const { internal_error("ConstantStringExpression"); }
    virtual std::string eval_string() const { return value; }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const TypeEnum *type, int value): Expression(type, true), value(value) {}

    const int value;

    virtual Number eval_number() const { return number_from_uint32(value); }
    virtual std::string eval_string() const { internal_error("ConstantEnumExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Type *elementtype, const std::vector<const Expression *> &elements): Expression(new TypeArray(elementtype), all_constant(elements)), elementtype(elementtype), elements(elements) {}

    const Type *elementtype;
    const std::vector<const Expression *> elements;

    virtual Number eval_number() const { internal_error("ArrayLiteralExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayLiteralExpression"); }
    virtual void generate(Emitter &) const;

    virtual std::string text() const { return "ArrayLiteralExpression(...)"; }
private:
    ArrayLiteralExpression(const ArrayLiteralExpression &);
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &);

    static bool all_constant(const std::vector<const Expression *> &elements);
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const Type *elementtype, const std::vector<std::pair<std::string, const Expression *>> &elements): Expression(new TypeDictionary(elementtype), all_constant(elements)), elementtype(elementtype), dict(make_dictionary(elements)) {}

    const Type *elementtype;
    const std::map<std::string, const Expression *> dict;

    virtual Number eval_number() const { internal_error("DictionaryLiteralExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryLiteralExpression"); }
    virtual void generate(Emitter &) const;

    virtual std::string text() const { return "DictionaryLiteralExpression(...)"; }
private:
    DictionaryLiteralExpression(const DictionaryLiteralExpression &);
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &);

    static bool all_constant(const std::vector<std::pair<std::string, const Expression *>> &elements);
    static std::map<std::string, const Expression *> make_dictionary(const std::vector<std::pair<std::string, const Expression *>> &elements);
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const Expression *value): Expression(value->type, value->is_constant), value(value) {
        if (not type->is_equivalent(TYPE_NUMBER)) {
            internal_error("UnaryMinusExpression");
        }
    }

    const Expression *const value;

    virtual Number eval_number() const { return number_negate(value->eval_number()); }
    virtual std::string eval_string() const { internal_error("UnaryMinusExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "UnaryMinusExpression(" + value->text() + ")";
    }
private:
    UnaryMinusExpression(const UnaryMinusExpression &);
    UnaryMinusExpression &operator=(const UnaryMinusExpression &);
};

class LogicalNotExpression: public Expression {
public:
    LogicalNotExpression(const Expression *value): Expression(value->type, value->is_constant), value(value) {
        if (not type->is_equivalent(TYPE_BOOLEAN)) {
            internal_error("LogicalNotExpression");
        }
    }

    const Expression *const value;

    virtual Number eval_number() const { internal_error("LogicalNotExpression"); }
    virtual std::string eval_string() const { internal_error("LogicalNotExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "LogicalNotExpression(" + value->text() + ")";
    }
private:
    LogicalNotExpression(const LogicalNotExpression &);
    LogicalNotExpression &operator=(const LogicalNotExpression &);
};

class ConditionalExpression: public Expression {
public:
    ConditionalExpression(const Expression *condition, const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), condition(condition), left(left), right(right) {
        if (not left->type->is_equivalent(right->type)) {
            internal_error("ConditionalExpression");
        }
    }

    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual Number eval_number() const { internal_error("ConditionalExpression"); }
    virtual std::string eval_string() const { internal_error("ConditionalExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ConditionalExpression(" + condition->text() + "," + left->text() + "," + right->text() + ")";
    }
private:
    ConditionalExpression(const ConditionalExpression &);
    ConditionalExpression &operator=(const ConditionalExpression &);
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_BOOLEAN) || not right->type->is_equivalent(TYPE_BOOLEAN)) {
            internal_error("DisjunctionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { internal_error("DisjunctionExpression"); }
    virtual std::string eval_string() const { internal_error("DisjunctionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DisjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    DisjunctionExpression(const DisjunctionExpression &);
    DisjunctionExpression &operator=(const DisjunctionExpression &);
};

class ConjunctionExpression: public Expression {
public:
    ConjunctionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_BOOLEAN) || not right->type->is_equivalent(TYPE_BOOLEAN)) {
            internal_error("ConjunctionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { internal_error("ConjunctionExpression"); }
    virtual std::string eval_string() const { internal_error("ConjunctionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ConjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ConjunctionExpression(const ConjunctionExpression &);
    ConjunctionExpression &operator=(const ConjunctionExpression &);
};

class ArrayInExpression: public Expression {
public:
    ArrayInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}

    const Expression *left;
    const Expression *right;

    virtual Number eval_number() const { internal_error("ArrayInExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayInExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ArrayInExpression(" + left->text() + ", " + right->text() + ")"; }
private:
    ArrayInExpression(const ArrayInExpression &);
    ArrayInExpression &operator=(const ArrayInExpression &);
};

class DictionaryInExpression: public Expression {
public:
    DictionaryInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}

    const Expression *left;
    const Expression *right;

    virtual Number eval_number() const { internal_error("DictionaryInExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryInExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "DictionaryInExpression(" + left->text() + ", " + right->text() + ")"; }
private:
    DictionaryInExpression(const DictionaryInExpression &);
    DictionaryInExpression &operator=(const DictionaryInExpression &);
};

class ComparisonExpression: public Expression {
public:
    enum Comparison {
        EQ, NE, LT, GT, LE, GE
    };
    ComparisonExpression(const Expression *left, const Expression *right, Comparison comp): Expression(TYPE_BOOLEAN, left->is_constant && right->is_constant), left(left), right(right), comp(comp) {}

    const Expression *const left;
    const Expression *const right;
    const Comparison comp;
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    BooleanComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("BooleanComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("BooleanComparisonExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "BooleanComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("NumericComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("NumericComparisonExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "NumericComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("StringComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("StringComparisonExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "StringComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("ArrayComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayComparisonExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ArrayComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("DictionaryComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryComparisonExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DictionaryComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("AdditionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_add(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("AdditionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "AdditionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    AdditionExpression(const AdditionExpression &);
    AdditionExpression &operator=(const AdditionExpression &);
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("SubtractionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_subtract(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("SubtractionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "SubtractionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    SubtractionExpression(const SubtractionExpression &);
    SubtractionExpression &operator=(const SubtractionExpression &);
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("MultiplicationExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_multiply(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("MultiplicationExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "MultiplicationExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    MultiplicationExpression(const MultiplicationExpression &);
    MultiplicationExpression &operator=(const MultiplicationExpression &);
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("DivisionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_divide(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("DivisionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DivisionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    DivisionExpression(const DivisionExpression &);
    DivisionExpression &operator=(const DivisionExpression &);
};

class ModuloExpression: public Expression {
public:
    ModuloExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("ModuloExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_modulo(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("ModuloExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ModuloExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ModuloExpression(const ModuloExpression &);
    ModuloExpression &operator=(const ModuloExpression &);
};

class ExponentiationExpression: public Expression {
public:
    ExponentiationExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("ExponentiationExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_pow(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("ExponentiationExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExponentiationExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class VariableReference: public AstNode {
public:
    VariableReference(const Type *type, bool is_constant): type(type), is_constant(is_constant) {}

    const Type *type;
    const bool is_constant;

    virtual void generate_address(Emitter &emitter) const = 0;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const = 0;
private:
    VariableReference(const VariableReference &);
    VariableReference &operator=(const VariableReference &);
};

class ConstantReference: public VariableReference {
public:
    ConstantReference(const Constant *cons): VariableReference(cons->type, true), cons(cons) {}

    const Constant *cons;

    virtual void generate_address(Emitter &) const {}
    virtual void generate_load(Emitter &emitter) const { cons->value->generate(emitter); }

    virtual std::string text() const {
        return "ConstantReference(" + cons->text() + ")";
    }
private:
    ConstantReference(const ConstantReference &);
    ConstantReference &operator=(const ConstantReference &);
};

class ScalarVariableReference: public VariableReference {
public:
    ScalarVariableReference(const Variable *var): VariableReference(var->type, false), var(var) {}

    const Variable *var;

    virtual void generate_address(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const {
        return "ScalarVariableReference(" + var->text() + ")";
    }
private:
    ScalarVariableReference(const ScalarVariableReference &);
    ScalarVariableReference &operator=(const ScalarVariableReference &);
};

class StringReference: public VariableReference {
public:
    StringReference(const VariableReference *str, const Expression *index);

    const VariableReference *str;
    const Expression *index;

    const FunctionCall *load;
    const FunctionCall *store;

    virtual void generate_address(Emitter &) const { internal_error("StringReference"); }
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;

    virtual std::string text() const { return "StringReference(" + str->text() + ", " + index->text() + ")"; }
private:
    StringReference(const StringReference &);
    StringReference &operator=(const StringReference &);
};

class ArrayReference: public VariableReference {
public:
    ArrayReference(const Type *type, const VariableReference *array, const Expression *index): VariableReference(type, array->is_constant), array(array), index(index) {}

    const VariableReference *array;
    const Expression *index;

    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "ArrayReference(" + array->text() + ", " + index->text() + ")"; }
private:
    ArrayReference(const ArrayReference &);
    ArrayReference &operator=(const ArrayReference &);
};

class DictionaryReference: public VariableReference {
public:
    DictionaryReference(const Type *type, const VariableReference *dict, const Expression *index): VariableReference(type, dict->is_constant), dict(dict), index(index) {}

    const VariableReference *dict;
    const Expression *index;

    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "DictionaryReference(" + dict->text() + ", " + index->text() + ")"; }
private:
    DictionaryReference(const DictionaryReference &);
    DictionaryReference &operator=(const DictionaryReference &);
};

class VariableExpression: public Expression {
public:
    VariableExpression(const VariableReference *var): Expression(var->type, var->is_constant), var(var) {}

    const VariableReference *var;

    virtual Number eval_number() const { internal_error("VariableExpression"); }
    virtual std::string eval_string() const { internal_error("VariableExpression"); }
    virtual const VariableReference *get_reference() const { return var; }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "VariableExpression(" + var->text() + ")";
    }
private:
    VariableExpression(const VariableExpression &);
    VariableExpression &operator=(const VariableExpression &);
};

class FunctionCall: public Expression {
public:
    FunctionCall(const VariableReference *func, const std::vector<const Expression *> &args): Expression(dynamic_cast<const TypeFunction *>(func->type)->returntype, false), func(func), args(args) {}

    const VariableReference *const func;
    const std::vector<const Expression *> args;

    virtual Number eval_number() const { internal_error("FunctionCall"); }
    virtual std::string eval_string() const { internal_error("FunctionCall"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
};

class Statement: public AstNode {
public:
    Statement(int line): line(line) {}
    const int line;

    void generate(Emitter &emitter) const;
    virtual void generate_code(Emitter &emitter) const = 0;
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(int line, const VariableReference *variable, const Expression *expr): Statement(line), variable(variable), expr(expr) {
        if (not variable->type->is_equivalent(expr->type)) {
            internal_error("AssignmentStatement");
        }
    }

    const VariableReference *const variable;
    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "AssignmentStatement(" + variable->text() + ", " + expr->text() + ")";
    }
private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(int line, const Expression *expr): Statement(line), expr(expr) {}

    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExpressionStatement(" + expr->text() + ")";
    }
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(int line, const Expression *expr): Statement(line), expr(expr) {}

    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "ReturnStatement(" + expr->text() + ")"; }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(int line, const std::vector<const Statement *> &statements): Statement(line), statements(statements) {}

    const std::vector<const Statement *> statements;

    virtual void dumpsubnodes(std::ostream &out, int depth) const;
};

class IfStatement: public Statement {
public:
    IfStatement(int line, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(line), condition_statements(condition_statements), else_statements(else_statements) {}

    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "IfStatement(" + condition_statements[0].first->text() + ")";
    }
private:
    IfStatement(const IfStatement &);
    IfStatement &operator=(const IfStatement &);
};

class LoopStatement: public CompoundStatement {
public:
    LoopStatement(int line, unsigned int loop_id, const std::vector<const Statement *> &statements): CompoundStatement(line, statements), loop_id(loop_id) {}

    const unsigned int loop_id;
};

class WhileStatement: public LoopStatement {
public:
    WhileStatement(int line, unsigned int loop_id, const Expression *condition, const std::vector<const Statement *> &statements): LoopStatement(line, loop_id, statements), condition(condition) {}

    const Expression *condition;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "WhileStatement(" + condition->text() + ")";
    }
private:
    WhileStatement(const WhileStatement &);
    WhileStatement &operator=(const WhileStatement &);
};

class ForStatement: public LoopStatement {
public:
    ForStatement(int line, unsigned int loop_id, const VariableReference *var, const Expression *start, const Expression *end, const std::vector<const Statement *> &statements): LoopStatement(line, loop_id, statements), var(var), start(start), end(end) {
    }

    const VariableReference *var;
    const Expression *start;
    const Expression *end;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "ForStatement(" + var->text() + " = (" + start->text() + ".." + end->text() + ")";
    }
private:
    ForStatement(const ForStatement &);
    ForStatement &operator=(const ForStatement &);
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        virtual ~WhenCondition() {}
        virtual bool overlaps(const WhenCondition *cond) const = 0;
        virtual void generate(Emitter &emitter) const = 0;
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(ComparisonExpression::Comparison comp, const Expression *expr): comp(comp), expr(expr) {}
        ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual bool overlaps(const WhenCondition *cond) const;
        virtual void generate(Emitter &emitter) const;
    private:
        ComparisonWhenCondition(const ComparisonWhenCondition &);
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &);
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Expression *low_expr, const Expression *high_expr): low_expr(low_expr), high_expr(high_expr) {}
        const Expression *low_expr;
        const Expression *high_expr;
        virtual bool overlaps(const WhenCondition *cond) const;
        virtual void generate(Emitter &emitter) const;
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(int line, const Expression *expr, const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> &clauses): Statement(line), expr(expr), clauses(clauses) {}

    const Expression *expr;
    const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "CaseStatement(" + expr->text() + ")";
    }
private:
    CaseStatement(const CaseStatement &);
    CaseStatement &operator=(const CaseStatement &);
};

class ExitStatement: public Statement {
public:
    ExitStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}

    const unsigned int loop_id;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "ExitStatement(...)"; }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class Function: public Variable {
public:
    Function(const std::string &name, const Type *returntype, Scope *scope, const std::vector<FunctionParameter *> &params): Variable(name, makeFunctionType(returntype, params)), scope(scope), params(params), entry_label(UINT_MAX), statements() {}

    Scope *scope;
    const std::vector<FunctionParameter *> params;
    unsigned int entry_label;

    std::vector<const Statement *> statements;

    static const Type *makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params);

    virtual void predeclare(Emitter &emitter);
    virtual void postdeclare(Emitter &emitter);
    virtual void generate_address(Emitter &) const { internal_error("Function"); }
    virtual void generate_load(Emitter &) const { internal_error("Function"); }
    virtual void generate_store(Emitter &) const { internal_error("Function"); }
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "Function(" + name + ", " + type->text() + ")"; }
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const std::string &name, const Type *type): Variable(name, type), name_index(-1) {}
    int name_index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_address(Emitter &) const { internal_error("PredefinedFunction"); }
    virtual void generate_load(Emitter &) const { internal_error("PredefinedFunction"); }
    virtual void generate_store(Emitter &) const { internal_error("PredefinedFunction"); }
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "PredefinedFunction(" + name + ", " + type->text() + ")"; }
};

class ExternalFunction: public Function {
public:
    ExternalFunction(const std::string &name, const Type *returntype, Scope *scope, const std::vector<FunctionParameter *> &params, const std::string &library_name, const std::map<std::string, std::string> &param_types): Function(name, returntype, scope, params), library_name(library_name), param_types(param_types), external_index(-1) {}

    const std::string library_name;
    const std::map<std::string, std::string> param_types;
    int external_index;

    virtual void predeclare(Emitter &);
    virtual void postdeclare(Emitter &) {} // TODO: Stub this out so we don't inherit the one from Function. Fix Function hierarchy.
    virtual void generate_call(Emitter &emitter) const;

private:
    ExternalFunction(const ExternalFunction &);
    ExternalFunction &operator=(const ExternalFunction &);
};

class Module: public Name {
public:
    Module(Scope *scope, const std::string &name): Name(name, TYPE_MODULE), scope(new Scope(scope)) {}

    Scope *scope;

    virtual void predeclare(Emitter &emitter);

    virtual std::string text() const { return "Module"; }
private:
    Module(const Module &);
    Module &operator=(const Module &);
};

class Program: public AstNode {
public:
    Program();

    Scope *scope;
    std::vector<const Statement *> statements;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "Program"; }
    virtual void dumpsubnodes(std::ostream &out, int depth) const;
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

#endif
