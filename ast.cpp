#include "ast.h"

#include <iostream>
#include <sstream>

TypeNone *TYPE_NONE = new TypeNone();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber();
TypeString *TYPE_STRING = new TypeString();

void AstNode::dump(std::ostream &out, int depth) const
{
    out << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(out, depth);
}

std::string ConstantBooleanExpression::text() const
{
    std::stringstream s;
    s << "ConstantBooleanExpression(" << value << ")";
    return s.str();
}

std::string ConstantNumberExpression::text() const
{
    std::stringstream s;
    s << "ConstantNumberExpression(" << number_to_string(value) << ")";
    return s.str();
}

std::string ConstantStringExpression::text() const
{
    std::stringstream s;
    s << "ConstantStringExpression(" << value << ")";
    return s.str();
}

std::string FunctionCall::text() const
{
    std::stringstream s;
    s << "FunctionCall(" << func->text() << ", [";
    for (std::vector<const Expression *>::const_iterator i = args.begin(); i != args.end(); ++i) {
        s << (*i)->text();
        if (i+1 != args.end()) {
            s << ", ";
        }
    }
    s << "])";
    return s.str();
}

void CompoundStatement::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}

const Name *Scope::lookupName(const std::string &name) const
{
    const Scope *s = this;
    while (s != nullptr) {
        auto n = s->names.find(name);
        if (n != s->names.end()) {
            n->second->referenced = true;
            return n->second;
        }
        s = s->parent;
    }
    return nullptr;
}

const Type *Function::makeFunctionType(const Type *returntype, const std::vector<const Variable *> &args)
{
    std::vector<const Type *> argtypes;
    for (auto a: args) {
        argtypes.push_back(a->type);
    }
    return new TypeFunction(returntype, argtypes);
}

Program::Program()
  : scope(new Scope(nullptr))
{
    scope->names["boolean"] = TYPE_BOOLEAN;
    scope->names["number"] = TYPE_NUMBER;
    scope->names["string"] = TYPE_STRING;

    static struct {
        const char *name;
        const Type *returntype;
        const Type *args[10];
    } BuiltinFunctions[] = {
        {"abs",    TYPE_NUMBER, {TYPE_NUMBER}},
        {"concat", TYPE_STRING, {TYPE_STRING, TYPE_STRING}},
        {"print",  TYPE_NONE,   {TYPE_STRING}},
        {"str",    TYPE_STRING, {TYPE_NUMBER}},
        {"strb",   TYPE_STRING, {TYPE_BOOLEAN}},
    };
    for (auto f: BuiltinFunctions) {
        std::vector<const Type *> args;
        for (auto a: f.args) {
            if (a == nullptr) {
                break;
            }
            args.push_back(a);
        }
        scope->names[f.name] = new PredefinedFunction(f.name, new TypeFunction(f.returntype, args));
    }
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}
