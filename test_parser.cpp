#include <assert.h>
#include <typeinfo>

#include "ast.h"
#include "parser.h"

static const Program *dump(const Program *program)
{
    program->dump();
    return program;
}

int main()
{
    {
        auto *program = dump(parse(tokenize("a := 1")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->name == "a");
        auto *ce = dynamic_cast<const ConstantExpression *>(as->expr);
        assert(ce->value == 1);
    }

    {
        auto *program = dump(parse(tokenize("a := -1")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->name == "a");
        auto *ume = dynamic_cast<const UnaryMinusExpression *>(as->expr);
        auto *ce = dynamic_cast<const ConstantExpression *>(ume->value);
        assert(ce->value == 1);
    }

    {
        auto *program = dump(parse(tokenize("a := 1 + 2")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->name == "a");
        auto *ae = dynamic_cast<const AdditionExpression *>(as->expr);
        auto *ce = dynamic_cast<const ConstantExpression *>(ae->left);
        assert(ce->value == 1);
        ce = dynamic_cast<const ConstantExpression *>(ae->right);
        assert(ce->value == 2);
    }

    {
        auto *program = dump(parse(tokenize("a := abs(1)")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->name == "a");
        auto *fc = dynamic_cast<const FunctionCall *>(as->expr);
        auto *fr = dynamic_cast<const FunctionReference *>(fc->func);
        assert(fr->name == "abs");
        assert(fc->args.size() == 1);
        auto *ce = dynamic_cast<const ConstantExpression *>(fc->args[0]);
        assert(ce->value == 1);
    }

    {
        auto *program = dump(parse(tokenize("print(1)")));
        assert(program->statements.size() == 1);
        auto *es = dynamic_cast<const ExpressionStatement *>(program->statements[0]);
        auto *fc = dynamic_cast<const FunctionCall *>(es->expr);
        auto *fr = dynamic_cast<const FunctionReference *>(fc->func);
        assert(fr->name == "print");
        assert(fc->args.size() == 1);
        auto *ce = dynamic_cast<const ConstantExpression *>(fc->args[0]);
        assert(ce->value == 1);
    }
}
