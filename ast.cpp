#include "ast.h"

#include <iostream>
#include <sstream>

#include "rtl.h"

TypeNothing *TYPE_NOTHING = new TypeNothing();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber();
TypeString *TYPE_STRING = new TypeString();
TypeModule *TYPE_MODULE = new TypeModule();

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

std::string ConstantEnumExpression::text() const
{
    std::stringstream s;
    s << "ConstantEnumExpression(" << value << ")";
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

const Type *Function::makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params)
{
    std::vector<const ParameterType *> paramtypes;
    for (auto p: params) {
        paramtypes.push_back(new ParameterType(p->mode, p->type));
    }
    return new TypeFunction(returntype, paramtypes);
}

Program::Program()
  : scope(new Scope(nullptr)),
    statements()
{
    scope->names["Nothing"] = TYPE_NOTHING;
    scope->names["Boolean"] = TYPE_BOOLEAN;
    scope->names["Number"] = TYPE_NUMBER;
    scope->names["String"] = TYPE_STRING;

    rtl_init(scope);
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}
