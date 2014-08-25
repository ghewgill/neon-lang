#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

// Interpreter
class Environment;
class Value;

// Compiler
class Emitter;

class AstNode {
public:
    void dump(int depth = 0) const;
    virtual std::string text() const = 0;
    virtual void dumpsubnodes(int depth) const {}
};

class Expression: public AstNode {
public:
    virtual int eval(Environment &env) const = 0;

    virtual void generate(Emitter &emitter) const = 0;
};

class ConstantExpression: public Expression {
public:
    ConstantExpression(int value): value(value) {}

    const int value;

    virtual int eval(Environment &env) const { return value; }

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const Expression *value): value(value) {}

    const Expression *const value;

    virtual int eval(Environment &env) const { return -value->eval(env); }

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "UnaryMinusExpression(" + value->text() + ")";
    }
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const Expression *left, const Expression *right): left(left), right(right) {}

    const Expression *const left;
    const Expression *const right;

    virtual int eval(Environment &env) const { return left->eval(env) + right->eval(env); }

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "AdditionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const Expression *left, const Expression *right): left(left), right(right) {}
    const Expression *const left;
    const Expression *const right;

    virtual int eval(Environment &env) const { return left->eval(env) - right->eval(env); }

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "SubtractionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const Expression *left, const Expression *right): left(left), right(right) {}
    const Expression *const left;
    const Expression *const right;

    virtual int eval(Environment &env) const { return left->eval(env) * right->eval(env); }

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "MultiplicationExpression(" + left->text() + "," + right->text() + ")";
    }
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const Expression *left, const Expression *right): left(left), right(right) {}
    const Expression *const left;
    const Expression *const right;

    virtual int eval(Environment &env) const { return left->eval(env) / right->eval(env); }

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DivisionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class VariableReference {
public:
    virtual Value *get(Environment &env) const = 0;
    virtual void set(Environment &env, Value *value) const = 0;

    virtual void generate(Emitter &emitter) const = 0;

    virtual std::string text() const = 0;
};

class ScalarVariableReference: public VariableReference {
public:
    ScalarVariableReference(const std::string &name): name(name) {}

    const std::string name;

    virtual int eval(Environment &env) const;
    virtual Value *get(Environment &env) const;
    virtual void set(Environment &env, Value *value) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ScalarVariableReference(" + name + ")";
    }
};

class FunctionReference: public VariableReference {
public:
    FunctionReference(const std::string &name): name(name) {}

    const std::string name;

    virtual Value *get(Environment &env) const;
    virtual void set(Environment &env, Value *value) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "FunctionReference(" + name + ")";
    }
};

class VariableExpression: public Expression {
public:
    VariableExpression(const VariableReference *var): var(var) {}

    const VariableReference *var;

    virtual int eval(Environment &env) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "VariableExpression(" + var->text() + ")";
    }
};

class FunctionCall: public Expression {
public:
    FunctionCall(const VariableReference *func, const std::vector<const Expression *> &args): func(func), args(args) {}

    const VariableReference *const func;
    const std::vector<const Expression *> args;

    virtual int eval(Environment &env) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class Statement: public AstNode {
public:
    virtual void interpret(Environment &env) const = 0;

    virtual void generate(Emitter &emitter) const = 0;
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(const VariableReference *variable, const Expression *expr): variable(variable), expr(expr) {}

    const VariableReference *const variable;
    const Expression *const expr;

    virtual void interpret(Environment &env) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "AssignmentStatement(" + variable->text() + ", " + expr->text() + ")";
    }
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const Expression *expr): expr(expr) {}

    const Expression *const expr;

    virtual void interpret(Environment &env) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExpressionStatement(" + expr->text() + ")";
    }
};

class IfStatement: public Statement {
public:
    IfStatement(const Expression *condition, const std::vector<const Statement *> &statements): condition(condition), statements(statements) {}

    const Expression *condition;
    const std::vector<const Statement *> statements;

    virtual void interpret(Environment &env) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "IfStatement(" + condition->text() + ")";
    }
    virtual void dumpsubnodes(int depth) const;
};

class Program: public AstNode {
public:
    Program(const std::vector<const Statement *> &statements): statements(statements) {}

    const std::vector<const Statement *> statements;

    virtual void interpret(Environment &env) const;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "Program"; }
    virtual void dumpsubnodes(int depth) const;
};

#endif
