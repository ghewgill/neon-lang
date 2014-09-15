#include "ast.h"

#include <iostream>
#include <sstream>

void AstNode::dump(int depth) const
{
    std::cout << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(depth);
}

std::string ConstantNumberExpression::text() const
{
    std::stringstream s;
    s << "ConstantNumberExpression(" << value << ")";
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

void CompoundStatement::dumpsubnodes(int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(depth+1);
    }
}

const Variable *Scope::lookupVariable(const std::string &name) const
{
    const Scope *s = this;
    while (s != nullptr) {
        auto v = vars.find(name);
        if (v != vars.end()) {
            v->second->referenced = true;
            return v->second;
        }
        s = s->parent;
    }
    return nullptr;
}

Program::Program()
  : Scope(nullptr)
{
    static const char *Builtins[] = {
        "abs",
        "print",
    };
    for (auto f: Builtins) {
        vars[f] = new Variable(f, new TypeFunction());
    }
}

void Program::dumpsubnodes(int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(depth+1);
    }
}
