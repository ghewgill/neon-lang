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
    virtual ~AstNode() {}
    void dump(std::ostream &out, int depth = 0) const;
    virtual std::string text() const = 0;
    virtual void dumpsubnodes(std::ostream &out, int depth) const {}
private:
    AstNode(const AstNode &);
    AstNode &operator=(const AstNode &);
};

class Name;
class Type;
class FunctionCall;

class Scope {
public:
    Scope(Scope *parent): parent(parent), names(), count(0) {}
    virtual ~Scope() {}

    virtual void predeclare(Emitter &emitter) const;
    virtual void postdeclare(Emitter &emitter) const;

    const Name *lookupName(const std::string &name) const;

    Scope *const parent;
    std::map<std::string, Name *> names;
    int count;
private:
    Scope(const Scope &);
    Scope &operator=(const Scope &);
};

class Name: public AstNode {
public:
    Name(const std::string &name, const Type *type): name(name), type(type), referenced(false) {}
    const std::string name;
    const Type *type;
    bool referenced;

    virtual void predeclare(Emitter &emitter) {}
    virtual void postdeclare(Emitter &emitter) {}
private:
    Name(const Name &);
    Name &operator=(const Name &);
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
    virtual void generate_store(Emitter &emitter) const;
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
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const Type *elementtype): Type("array"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const { assert(false); }

    virtual std::string text() const { return "TypeArray(" + elementtype->text() + ")"; }
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const Type *elementtype): Type("dictionary"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const { assert(false); }

    virtual std::string text() const { return "TypeDictionary(" + elementtype->text() + ")"; }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

class TypeRecord: public Type {
public:
    TypeRecord(const std::map<std::string, std::pair<int, const Type *> > &fields): Type("record"), fields(fields) {}
    const std::map<std::string, std::pair<int, const Type *> > fields;

    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const { assert(false); }

    virtual std::string text() const { return "TypeRecord(...)"; }
};

class TypeEnum: public Type {
public:
    TypeEnum(const std::map<std::string, int> &names): Type("enum"), names(names) {}
    const std::map<std::string, int> names;

    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
    virtual void generate_call(Emitter &emitter) const { assert(false); }

    virtual std::string text() const { return "TypeEnum(...)"; }
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
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class Expression: public AstNode {
public:
    Expression(const Type *type): type(type) {}

    virtual void generate(Emitter &emitter) const = 0;

    const Type *type;
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

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const TypeEnum *type, int value): Expression(type), value(value) {}

    const int value;

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
private:
    UnaryMinusExpression(const UnaryMinusExpression &);
    UnaryMinusExpression &operator=(const UnaryMinusExpression &);
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
private:
    LogicalNotExpression(const LogicalNotExpression &);
    LogicalNotExpression &operator=(const LogicalNotExpression &);
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
private:
    DisjunctionExpression(const DisjunctionExpression &);
    DisjunctionExpression &operator=(const DisjunctionExpression &);
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
private:
    ConjunctionExpression(const ConjunctionExpression &);
    ConjunctionExpression &operator=(const ConjunctionExpression &);
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
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
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
private:
    AdditionExpression(const AdditionExpression &);
    AdditionExpression &operator=(const AdditionExpression &);
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
private:
    SubtractionExpression(const SubtractionExpression &);
    SubtractionExpression &operator=(const SubtractionExpression &);
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
private:
    MultiplicationExpression(const MultiplicationExpression &);
    MultiplicationExpression &operator=(const MultiplicationExpression &);
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
private:
    DivisionExpression(const DivisionExpression &);
    DivisionExpression &operator=(const DivisionExpression &);
};

class ModuloExpression: public Expression {
public:
    ModuloExpression(const Expression *left, const Expression *right): Expression(left->type), left(left), right(right) {
        assert(left->type == TYPE_NUMBER);
        assert(right->type == TYPE_NUMBER);
    }

    const Expression *const left;
    const Expression *const right;

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
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class VariableReference: public AstNode {
public:
    VariableReference(const Type *type): type(type) {}

    const Type *type;

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
    ConstantReference(const Constant *cons): VariableReference(cons->type), cons(cons) {}

    const Constant *cons;

    virtual void generate_address(Emitter &emitter) const {}
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
    ScalarVariableReference(const Variable *var): VariableReference(var->type), var(var) {}

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

    virtual void generate_address(Emitter &emitter) const { assert(false); }
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;

    virtual std::string text() const { return "StringReference(" + str->text() + ", " + index->text() + ")"; }
private:
    StringReference(const StringReference &);
    StringReference &operator=(const StringReference &);
};

class ArrayReference: public VariableReference {
public:
    ArrayReference(const Type *type, const VariableReference *array, const Expression *index): VariableReference(type), array(array), index(index) {}

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
    DictionaryReference(const Type *type, const VariableReference *dict, const Expression *index): VariableReference(type), dict(dict), index(index) {}

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
    VariableExpression(const VariableReference *var): Expression(var->type), var(var) {}

    const VariableReference *var;

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
    FunctionCall(const VariableReference *func, const std::vector<const Expression *> &args): Expression(dynamic_cast<const TypeFunction *>(func->type)->returntype), func(func), args(args) {}

    const VariableReference *const func;
    const std::vector<const Expression *> args;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
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
private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const Expression *expr): expr(expr) {}

    const Expression *const expr;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExpressionStatement(" + expr->text() + ")";
    }
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const Expression *expr): expr(expr) {}

    const Expression *const expr;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ReturnStatement(" + expr->text() + ")"; }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
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
private:
    IfStatement(const IfStatement &);
    IfStatement &operator=(const IfStatement &);
};

class WhileStatement: public CompoundStatement {
public:
    WhileStatement(const Expression *condition, const std::vector<const Statement *> &statements): CompoundStatement(statements), condition(condition) {}

    const Expression *condition;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "WhileStatement(" + condition->text() + ")";
    }
private:
    WhileStatement(const WhileStatement &);
    WhileStatement &operator=(const WhileStatement &);
};

class Function: public Variable {
public:
    Function(const std::string &name, const Type *returntype, Scope *scope, const std::vector<Variable *> &args): Variable(name, makeFunctionType(returntype, args)), scope(scope), args(args), entry_label(-1), statements() {
        for (auto v: args) {
            scope->names[v->name] = v;
        }
    }
    Scope *scope;
    const std::vector<Variable *> args;
    int entry_label;

    std::vector<const Statement *> statements;

    static const Type *makeFunctionType(const Type *returntype, const std::vector<Variable *> &args);

    virtual void predeclare(Emitter &emitter);
    virtual void postdeclare(Emitter &emitter);
    virtual void generate_address(Emitter &emitter) const { assert(false); }
    virtual void generate_load(Emitter &emitter) const { assert(false); }
    virtual void generate_store(Emitter &emitter) const { assert(false); }
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
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

#endif
