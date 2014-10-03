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

class Type: public AstNode {
public:
    virtual int declare(const std::string &name, Emitter &emitter) const;
    virtual void generate_load(Emitter &emitter, int index) const = 0;
    virtual void generate_store(Emitter &emitter, int index) const = 0;
    virtual void generate_call(Emitter &emitter, int index) const = 0;
};

class TypeNone: public Type {
public:
    virtual void generate_load(Emitter &emitter, int index) const {}
    virtual void generate_store(Emitter &emitter, int index) const {}
    virtual void generate_call(Emitter &emitter, int index) const {}

    virtual std::string text() const { return "TypeNone"; }
};

extern TypeNone *TYPE_NONE;

class TypeBoolean: public Type {
public:
    virtual void generate_load(Emitter &emitter, int index) const;
    virtual void generate_store(Emitter &emitter, int index) const;
    virtual void generate_call(Emitter &emitter, int index) const;

    virtual std::string text() const { return "TypeBoolean"; }
};

extern TypeBoolean *TYPE_BOOLEAN;

class TypeNumber: public Type {
public:
    virtual void generate_load(Emitter &emitter, int index) const;
    virtual void generate_store(Emitter &emitter, int index) const;
    virtual void generate_call(Emitter &emitter, int index) const;

    virtual std::string text() const { return "TypeNumber"; }
};

extern TypeNumber *TYPE_NUMBER;

class TypeString: public Type {
public:
    virtual void generate_load(Emitter &emitter, int index) const;
    virtual void generate_store(Emitter &emitter, int index) const;
    virtual void generate_call(Emitter &emitter, int index) const;

    virtual std::string text() const { return "TypeString"; }
};

extern TypeString *TYPE_STRING;

class TypeFunction: public Type {
public:
    TypeFunction(const Type *returntype, const std::vector<const Type *> &args): returntype(returntype), args(args) {}
    virtual int declare(const std::string &name, Emitter &emitter) const { assert(false); }
    virtual void generate_load(Emitter &emitter, int index) const;
    virtual void generate_store(Emitter &emitter, int index) const;
    virtual void generate_call(Emitter &emitter, int index) const;

    const Type *returntype;
    const std::vector<const Type *> args;

    virtual std::string text() const { return "TypeFunction(...)"; }
};

class TypePredefinedFunction: public TypeFunction {
public:
    TypePredefinedFunction(const Type *returntype, const std::vector<const Type *> &args): TypeFunction(returntype, args) {}
    virtual int declare(const std::string &name, Emitter &emitter) const;
    virtual void generate_load(Emitter &emitter, int index) const;
    virtual void generate_store(Emitter &emitter, int index) const;
    virtual void generate_call(Emitter &emitter, int index) const;

    virtual std::string text() const { return "TypePredefinedFunction(...)"; }
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

class Variable: public Name {
public:
    Variable(const std::string &name, const Type *type): Name(name, type), index() {}
    int index;

    virtual void predeclare(Emitter &emitter);
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "Variable(" + name + ", " + type->text() + ")"; }
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

class VariableReference {
public:
    VariableReference(const Type *type): type(type) {}

    const Type *type;

    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;

    virtual std::string text() const = 0;
};

class ScalarVariableReference: public VariableReference {
public:
    ScalarVariableReference(const Variable *var): VariableReference(var->type), var(var) {}

    const Variable *var;

    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const {
        return "ScalarVariableReference(" + var->text() + ")";
    }
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

class IfStatement: public CompoundStatement {
public:
    IfStatement(const Expression *condition, const std::vector<const Statement *> &statements): CompoundStatement(statements), condition(condition) {}

    const Expression *condition;

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

class Scope {
public:
    Scope(Scope *parent): parent(parent) {}

    virtual void predeclare(Emitter &emitter) const;
    virtual void postdeclare(Emitter &emitter) const;

    const Type *lookupType(const std::string &name) const;
    const Name *lookupName(const std::string &name) const;

    Scope *const parent;
    std::map<std::string, Type *> types;
    std::map<std::string, Name *> names;
};

class Function: public Variable {
public:
    Function(const std::string &name, const Type *returntype, const std::vector<const Variable *> &args): Variable(name, makeFunctionType(returntype, args)), args(args) {}
    const std::vector<const Variable *> args;

    std::vector<const Statement *> statements;

    const Type *makeFunctionType(const Type *returntype, const std::vector<const Variable *> &args);

    virtual void predeclare(Emitter &emitter);
    virtual void postdeclare(Emitter &emitter);
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "Function(" + name + ", " + type->text() + ")"; }
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
