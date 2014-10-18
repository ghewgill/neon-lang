#ifndef AST_H
#define AST_H

#include <assert.h>
#include <map>
#include <string>
#include <vector>

#include "number.h"

// Compiler
class Emitter;

class AstNode {
public:
    AstNode() {}
    void dump(std::ostream &out, int depth = 0) const;
    virtual std::string text() const = 0;
    virtual void dumpsubnodes(std::ostream &out, int depth) const {}
private:
    AstNode(const AstNode &);
    AstNode &operator=(const AstNode &);
};

class Name;
class Type;

class Scope {
public:
    Scope(Scope *parent): parent(parent) {}

    virtual void predeclare(Emitter &emitter) const;
    virtual void postdeclare(Emitter &emitter) const;

    const Name *lookupName(const std::string &name) const;

    Scope *const parent;
    std::map<std::string, Name *> names;
    int count;
};

class Name: public AstNode {
public:
    Name(const std::string &name, const Type *type): name(name), type(type), referenced(false) {}
    const std::string name;
    const Type *type;
    bool referenced;

    virtual void predeclare(Emitter &emitter) {}
    virtual void postdeclare(Emitter &emitter) {}
};

class Type: public Name {
public:
    Type(const std::string &name): Name(name, nullptr) {}
    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;
};

class TypeNone: public Type {
public:
    TypeNone(): Type("") {}
    virtual void generate_load(Emitter &emitter) const {}
    virtual void generate_store(Emitter &emitter) const {}
    virtual void generate_call(Emitter &emitter) const {}

    virtual std::string text() const { return "TypeNone"; }
};

extern TypeNone *TYPE_NONE;

class TypeBoolean: public Type {
public:
    TypeBoolean(): Type("boolean") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeBoolean"; }
};

extern TypeBoolean *TYPE_BOOLEAN;

class TypeNumber: public Type {
public:
    TypeNumber(): Type("number") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitterindex) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeNumber"; }
};

extern TypeNumber *TYPE_NUMBER;

class TypeString: public Type {
public:
    TypeString(): Type("string") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeString"; }
};

extern TypeString *TYPE_STRING;

class TypeFunction: public Type {
public:
    TypeFunction(const Type *returntype, const std::vector<const Type *> &args): Type("function"), returntype(returntype), args(args) {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    const Type *returntype;
    const std::vector<const Type *> args;

    virtual std::string text() const { return "TypeFunction(...)"; }
};

class TypeArray: public Type {
public:
    TypeArray(const Type *elementtype): Type("array"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const { assert(false); }

    virtual std::string text() const { return "TypeArray(" + elementtype->text() + ")"; }
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const Type *elementtype): Type("dictionary"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const { assert(false); }

    virtual std::string text() const { return "TypeDictionary(" + elementtype->text() + ")"; }
};

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
    GlobalVariable(const std::string &name, const Type *type): Variable(name, type), index() {}
    int index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "GlobalVariable(" + name + ", " + type->text() + ")"; }
};

class LocalVariable: public Variable {
public:
    LocalVariable(const std::string &name, const Type *type, Scope *scope): Variable(name, type), scope(scope), index() {}
    Scope *scope;
    int index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "LocalVariable(" + name + ", " + type->text() + ")"; }
};

class Expression: public AstNode {
public:
    Expression(const Type *type): type(type) {}

    virtual void generate(Emitter &emitter) const = 0;

    const Type *type;
};

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(bool value): Expression(TYPE_BOOLEAN), value(value) {}

    const bool value;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(Number value): Expression(TYPE_NUMBER), value(value) {}

    const Number value;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const std::string &value): Expression(TYPE_STRING), value(value) {}

    const std::string value;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const Expression *value): Expression(value->type), value(value) {
        assert(type == TYPE_NUMBER);
    }

    const Expression *const value;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "UnaryMinusExpression(" + value->text() + ")";
    }
};

class LogicalNotExpression: public Expression {
public:
    LogicalNotExpression(const Expression *value): Expression(value->type), value(value) {
        assert(type == TYPE_BOOLEAN);
    }

    const Expression *const value;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "LogicalNotExpression(" + value->text() + ")";
    }
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_BOOLEAN);
        assert(right->type == TYPE_BOOLEAN);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DisjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ConjunctionExpression: public Expression {
public:
    ConjunctionExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_BOOLEAN);
        assert(right->type == TYPE_BOOLEAN);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ConjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ComparisonExpression: public Expression {
public:
    enum Comparison {
        EQ, NE, LT, GT, LE, GE
    };
    ComparisonExpression(const Expression *left, const Expression *right, Comparison comp): Expression(TYPE_BOOLEAN), left(left), right(right), comp(comp) {}

    const Expression *const left;
    const Expression *const right;
    const Comparison comp;
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "NumericComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "StringComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_NUMBER);
        assert(right->type == TYPE_NUMBER);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "AdditionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_NUMBER);
        assert(right->type == TYPE_NUMBER);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "SubtractionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_NUMBER);
        assert(right->type == TYPE_NUMBER);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "MultiplicationExpression(" + left->text() + "," + right->text() + ")";
    }
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_NUMBER);
        assert(right->type == TYPE_NUMBER);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DivisionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ExponentiationExpression: public Expression {
public:
    ExponentiationExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_NUMBER);
        assert(right->type == TYPE_NUMBER);
    }

    const Expression *const left;
    const Expression *const right;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExponentiationExpression(" + left->text() + "," + right->text() + ")";
    }
};

class VariableReference {
public:
    VariableReference(const Type *type): type(type) {}

    const Type *type;

    virtual void generate_address(Emitter &emitter) const = 0;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const = 0;
};

class ScalarVariableReference: public VariableReference {
public:
    ScalarVariableReference(const Variable *var): VariableReference(var->type), var(var) {}

    const Variable *var;

    virtual void generate_address(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const {
        return "ScalarVariableReference(" + var->text() + ")";
    }
};

class ArrayReference: public VariableReference {
public:
    ArrayReference(const Type *type, const VariableReference *array, const Expression *index): VariableReference(type), array(array), index(index) {}

    const VariableReference *array;
    const Expression *index;

    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "ArrayReference(" + array->text() + ", " + index->text() + ")"; }
};

class DictionaryReference: public VariableReference {
public:
    DictionaryReference(const Type *type, const VariableReference *dict, const Expression *index): VariableReference(type), dict(dict), index(index) {}

    const VariableReference *dict;
    const Expression *index;

    virtual void generate_address(Emitter &emitter) const;

    virtual std::string text() const { return "DictionaryReference(" + dict->text() + ", " + index->text() + ")"; }
};

class VariableExpression: public Expression {
public:
    VariableExpression(const VariableReference *var): Expression(var->type), var(var) {}

    const VariableReference *var;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "VariableExpression(" + var->text() + ")";
    }
};

class FunctionCall: public Expression {
public:
    FunctionCall(const VariableReference *func, const std::vector<const Expression *> &args): Expression(dynamic_cast<const TypeFunction *>(func->type)->returntype), func(func), args(args) {}

    const VariableReference *const func;
    const std::vector<const Expression *> args;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class Statement: public AstNode {
public:
    virtual void generate(Emitter &emitter) const = 0;
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(const VariableReference *variable, const Expression *expr): variable(variable), expr(expr) {
        assert(variable->type == expr->type);
    }

    const VariableReference *const variable;
    const Expression *const expr;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "AssignmentStatement(" + variable->text() + ", " + expr->text() + ")";
    }
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const Expression *expr): expr(expr) {}

    const Expression *const expr;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExpressionStatement(" + expr->text() + ")";
    }
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const Expression *expr): expr(expr) {}

    const Expression *const expr;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ReturnStatement(" + expr->text() + ")"; }
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(const std::vector<const Statement *> &statements): statements(statements) {}

    const std::vector<const Statement *> statements;

    virtual void dumpsubnodes(std::ostream &out, int depth) const;
};

class IfStatement: public Statement {
public:
    IfStatement(const Expression *condition, const std::vector<const Statement *> &then_statements, const std::vector<const Statement *> &else_statements): condition(condition), then_statements(then_statements), else_statements(else_statements) {}

    const Expression *condition;
    const std::vector<const Statement *> then_statements;
    const std::vector<const Statement *> else_statements;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "IfStatement(" + condition->text() + ")";
    }
};

class WhileStatement: public CompoundStatement {
public:
    WhileStatement(const Expression *condition, const std::vector<const Statement *> &statements): CompoundStatement(statements), condition(condition) {}

    const Expression *condition;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "WhileStatement(" + condition->text() + ")";
    }
};

class Function: public Variable {
public:
    Function(const std::string &name, const Type *returntype, Scope *parentscope, const std::vector<const Variable *> &args): Variable(name, makeFunctionType(returntype, args)), scope(new Scope(parentscope)), args(args) {}
    Scope *scope;
    const std::vector<const Variable *> args;
    int entry_label;

    std::vector<const Statement *> statements;

    static const Type *makeFunctionType(const Type *returntype, const std::vector<const Variable *> &args);

    virtual void predeclare(Emitter &emitter);
    virtual void postdeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const { assert(false); }
    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "Function(" + name + ", " + type->text() + ")"; }
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const std::string &name, const Type *type): Variable(name, type) {}
    int name_index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const { assert(false); }
    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "PredefinedFunction(" + name + ", " + type->text() + ")"; }
};

class Program: public AstNode {
public:
    Program();

    Scope *scope;
    std::vector<const Statement *> statements;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "Program"; }
    virtual void dumpsubnodes(std::ostream &out, int depth) const;
};

#endif
