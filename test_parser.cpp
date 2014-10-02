#include <assert.h>
#include <iostream>
#include <typeinfo>

#include "ast.h"
#include "parser.h"

static const Program *dump(const Program *program)
{
    program->dump(std::cout);
    return program;
}

int main()
{
    {
        auto *program = dump(parse(tokenize("VAR a: number a := 1")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->var->name == "a");
        auto *ce = dynamic_cast<const ConstantNumberExpression *>(as->expr);
        assert(number_is_equal(ce->value, number_from_uint32(1)));
    }

    {
        auto *program = dump(parse(tokenize("VAR a: number a := -1")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->var->name == "a");
        auto *ume = dynamic_cast<const UnaryMinusExpression *>(as->expr);
        auto *ce = dynamic_cast<const ConstantNumberExpression *>(ume->value);
        assert(number_is_equal(ce->value, number_from_uint32(1)));
    }

    {
        auto *program = dump(parse(tokenize("VAR a: number a := 1 + 2")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->var->name == "a");
        auto *ae = dynamic_cast<const AdditionExpression *>(as->expr);
        auto *ce = dynamic_cast<const ConstantNumberExpression *>(ae->left);
        assert(number_is_equal(ce->value, number_from_uint32(1)));
        ce = dynamic_cast<const ConstantNumberExpression *>(ae->right);
        assert(number_is_equal(ce->value, number_from_uint32(2)));
    }

    {
        auto *program = dump(parse(tokenize("VAR a: number a := abs(1)")));
        assert(program->statements.size() == 1);
        auto *as = dynamic_cast<const AssignmentStatement *>(program->statements[0]);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(as->variable);
        assert(svr->var->name == "a");
        auto *fc = dynamic_cast<const FunctionCall *>(as->expr);
        svr = dynamic_cast<const ScalarVariableReference *>(fc->func);
        assert(svr->var->name == "abs");
        assert(fc->args.size() == 1);
        auto *ce = dynamic_cast<const ConstantNumberExpression *>(fc->args[0]);
        assert(number_is_equal(ce->value, number_from_uint32(1)));
    }

    {
        auto *program = dump(parse(tokenize("print(\"1\")")));
        assert(program->statements.size() == 1);
        auto *es = dynamic_cast<const ExpressionStatement *>(program->statements[0]);
        auto *fc = dynamic_cast<const FunctionCall *>(es->expr);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(fc->func);
        assert(svr->var->name == "print");
        assert(fc->args.size() == 1);
        auto *ce = dynamic_cast<const ConstantStringExpression *>(fc->args[0]);
        assert(ce->value == "1");
    }

    {
        auto *program = dump(parse(tokenize("VAR a: number IF a THEN print(str(a)) END")));
        assert(program->statements.size() == 1);
        auto *is = dynamic_cast<const IfStatement *>(program->statements[0]);
        auto *ve = dynamic_cast<const VariableExpression *>(is->condition);
        auto *svr = dynamic_cast<const ScalarVariableReference *>(ve->var);
        assert(svr->var->name == "a");
        assert(is->statements.size() == 1);
        auto *es = dynamic_cast<const ExpressionStatement *>(is->statements[0]);
        assert(es != NULL);
    }
}
