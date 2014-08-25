#include "interpreter.h"

#include <map>
#include <stdlib.h>

#include "ast.h"

enum ValueType {
    NONE,
    INTEGER,
    FUNCTION,
};

class Value {
public:
    Value(ValueType type): type(type) {}
    const ValueType type;
};

class IntegerValue: public Value {
public:
    IntegerValue(int value): Value(INTEGER), value(value) {}
    const int value;
};

typedef Value *(*BuiltinFunction)(Environment &env, const std::vector<Value *> &args);

class FunctionValue: public Value {
public:
    FunctionValue(BuiltinFunction function): Value(FUNCTION), function(function) {}
    const BuiltinFunction function;
};

class Environment {
public:
    Environment(std::ostream &stdout): stdout(stdout), parent(NULL) {}

    std::ostream &stdout;

    Value *getVariable(const std::string &name);
    void setVariable(const std::string &name, Value *value);

    Environment *parent;
    std::map<std::string, Value *> variables;
};

Value *Environment::getVariable(const std::string &name)
{
    Environment *e = this;
    while (e != NULL) {
        std::map<std::string, Value *>::iterator i = variables.find(name);
        if (i != variables.end()) {
            return i->second;
        }
        e = e->parent;
    }
    printf("Undefined variable: %s\n", name.c_str());
    abort();
}

void Environment::setVariable(const std::string &name, Value *value)
{
    variables[name] = value;
}

int ScalarVariableReference::eval(Environment &env) const
{
    Value *value = env.getVariable(name);
    if (value->type == INTEGER) {
        return static_cast<IntegerValue *>(value)->value;
    } else {
        printf("Type mismatch: %s\n", name.c_str());
        abort();
    }
}

Value *ScalarVariableReference::get(Environment &env) const
{
    return env.getVariable(name);
}

void ScalarVariableReference::set(Environment &env, Value *value) const
{
    env.setVariable(name, value);
}

Value *FunctionReference::get(Environment &env) const
{
    return env.getVariable(name);
}

void FunctionReference::set(Environment &env, Value *value) const
{
    env.setVariable(name, value);
}

int VariableExpression::eval(Environment &env) const
{
    return static_cast<IntegerValue *>(var->get(env))->value;
}

int FunctionCall::eval(Environment &env) const
{
    Value *v = func->get(env);
    if (v != NULL) {
        if (v->type == FUNCTION) {
            FunctionValue *f = static_cast<FunctionValue *>(v);
            std::vector<Value *> values;
            for (std::vector<const Expression *>::const_iterator i = args.begin(); i != args.end(); ++i) {
                values.push_back(new IntegerValue((*i)->eval(env)));
            }
            Value *r = f->function(env, values);
            if (r == NULL) {
                return 0;
            } else if (r->type == INTEGER) {
                return static_cast<IntegerValue *>(r)->value;
            } else {
                printf("Function return nonscalar value");
                abort();
            }
        } else {
            printf("Not a callable value");
            abort();
        }
    } else {
        printf("Unknown function: %s\n", func->text().c_str());
        abort();
    }
}

void AssignmentStatement::interpret(Environment &env) const
{
    variable->set(env, new IntegerValue(expr->eval(env)));
}

void ExpressionStatement::interpret(Environment &env) const
{
    expr->eval(env);
}

void CompoundStatement::interpret(Environment &env) const
{
    for (auto s: statements) {
        s->interpret(env);
    }
}

void IfStatement::interpret(Environment &env) const
{
    if (condition->eval(env)) {
        CompoundStatement::interpret(env);
    }
}

void WhileStatement::interpret(Environment &env) const
{
    while (condition->eval(env)) {
        CompoundStatement::interpret(env);
    }
}

void Program::interpret(Environment &env) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->interpret(env);
    }
}

Value *f_abs(Environment &env, const std::vector<Value *> &args)
{
    switch (args[0]->type) {
        case INTEGER: {
            IntegerValue *v = static_cast<IntegerValue *>(args[0]);
            return new IntegerValue(abs(v->value));
        }
        default:
            printf("Integer expected in abs()\n");
            abort();
    }
}

Value *f_print(Environment &env, const std::vector<Value *> &args)
{
    switch (args[0]->type) {
        case INTEGER: {
            IntegerValue *v = static_cast<IntegerValue *>(args[0]);
            env.stdout << v->value << "\n";
            break;
        }
        default:
            printf("Unknown type in print()\n");
            abort();
    }
    return NULL;
}

void interpret(const Program *program, std::ostream &stdout)
{
    Environment env(stdout);
    env.setVariable("abs", new FunctionValue(f_abs));
    env.setVariable("print", new FunctionValue(f_print));
    program->interpret(env);
}
