#include "ast.h"

#include <iostream>
#include <sstream>

void AstNode::dump(int depth) const
{
    std::cout << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(depth);
}

std::string ConstantExpression::text() const
{
    std::stringstream s;
    s << "ConstantExpression(" << value << ")";
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

void IfStatement::dumpsubnodes(int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(depth+1);
    }
}

void Program::dumpsubnodes(int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(depth+1);
    }
}
