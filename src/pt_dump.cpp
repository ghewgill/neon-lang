#include "pt_dump.h"

#include <iostream>
#include <iso646.h>

#include "util.h"

using namespace pt;

static const std::string ComparisonName[] = {"eq", "ne", "lt", "gt", "le", "ge"};
static const std::string ArgumentModeName[] = {"in", "inout", "out"};

class Dumper: public IParseTreeVisitor {
public:
    Dumper(std::ostream &out): out(out), depth(0) {}

    virtual void visit(const TypeSimple *node) override {
        write("TypeSimple(" + node->name + ")");
    }
    virtual void visit(const TypeEnum *node) override {
        write("TypeEnum(" + join(node->names) + ")");
    }
    virtual void visit(const TypeRecord *node) override {
        write("TypeRecord(" + join(node->fields) + ")");
    }
    virtual void visit(const TypeClass *node) override {
        write("TypeClass(" + join(node->fields) + ")");
    }
    virtual void visit(const TypePointer *node) override {
        write("TypePointer");
        child(node->reftype.get());
    }
    virtual void visit(const TypeValidPointer *node) override {
        write("TypeValidPointer");
        child(node->reftype.get());
    }
    virtual void visit(const TypeFunctionPointer *node) override {
        write("TypeFunctionPointer");
        child(node->returntype.get());
    }
    virtual void visit(const TypeParameterised *node) override {
        write("TypeParameterised(" + node->name.text + ")");
        child(node->elementtype.get());
    }
    virtual void visit(const TypeImport *node) override {
        write("TypeImport(" + node->modname.text + "." + node->subname.text + ")");
    }

    virtual void visit(const DummyExpression *) override {
        write("DummyExpression");
    }
    virtual void visit(const IdentityExpression *node) override {
        write("IdentityExpression");
        child(node->expr.get());
    }
    virtual void visit(const BooleanLiteralExpression *node) override {
        write("BooleanLiteralExpression(" + std::string(node->value ? "true" : "false") + ")");
    }
    virtual void visit(const NumberLiteralExpression *node) override {
        write("NumberLiteralExpression(" + number_to_string(node->value) + ")");
    }
    virtual void visit(const StringLiteralExpression *node) override {
        write("StringLiteralExpression(" + node->value + ")");
    }
    virtual void visit(const FileLiteralExpression *node) override {
        write("FileLiteralExpression(" + node->name + ")");
    }
    virtual void visit(const BytesLiteralExpression *) override {
        write("BytesLiteralExpression");
    }
    virtual void visit(const ArrayLiteralExpression *node) override {
        write("ArrayLiteralExpression");
        for (auto &x: node->elements) {
            child(x.get());
        }
    }
    virtual void visit(const ArrayLiteralRangeExpression *node) override {
        write("ArrayLiteralRangeExpression");
        child(node->first.get());
        child(node->last.get());
        child(node->step.get());
    }
    virtual void visit(const DictionaryLiteralExpression *node) override {
        write("DictionaryLiteralExpression");
        depth++;
        for (auto &x: node->elements) {
            child(x.first.get());
            child(x.second.get());
        }
        depth--;
    }
    virtual void visit(const NilLiteralExpression *) override {
        write("NilLiteralExpression");
    }
    virtual void visit(const NowhereLiteralExpression *) override {
        write("NowhereLiteralExpression");
    }
    virtual void visit(const IdentifierExpression *node) override {
        write("IdentifierExpression(" + node->name + ")");
    }
    virtual void visit(const DotExpression *node) override {
        write("DotExpression(" + node->name.text + ")");
        child(node->base.get());
    }
    virtual void visit(const ArrowExpression *node) override {
        write("ArrowExpression(" + node->name.text + ")");
        child(node->base.get());
    }
    virtual void visit(const SubscriptExpression *node) override {
        write("SubscriptExpression");
        child(node->base.get());
        child(node->index.get());
    }
    virtual void visit(const InterpolatedStringExpression *node) override {
        write("InterpolatedStringExpression");
        depth++;
        for (auto &x: node->parts) {
            write("FormatString(" + x.second.text + ")");
            child(x.first.get());
        }
        depth--;
    }
    virtual void visit(const FunctionCallExpression *node) override {
        write("FunctionCallExpression");
        child(node->base.get());
        for (auto &x: node->args) {
            child(x->expr.get());
        }
    }
    virtual void visit(const UnaryPlusExpression *node) override {
        write("UnaryPlusExpression");
        child(node->expr.get());
    }
    virtual void visit(const UnaryMinusExpression *node) override {
        write("UnaryMinusExpression");
        child(node->expr.get());
    }
    virtual void visit(const LogicalNotExpression *node) override {
        write("LogicalNotExpression");
        child(node->expr.get());
    }
    virtual void visit(const ExponentiationExpression *node) override {
        write("ExponentiationExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const MultiplicationExpression *node) override {
        write("MultiplicationExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const DivisionExpression *node) override {
        write("DivisionExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const IntegerDivisionExpression *node) override {
        write("IntegerDivisionExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const ModuloExpression *node) override {
        write("ModuloExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const AdditionExpression *node) override {
        write("AdditionExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const SubtractionExpression *node) override {
        write("SubtractionExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const ConcatenationExpression *node) override {
        write("ConcatenationExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const ComparisonExpression *node) override {
        write("ComparisonExpression(" + ComparisonName[node->comp] + ")");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const ChainedComparisonExpression *node) override {
        write("ChainedComparisonExpression");
        child(node->left.get());
        for (auto &x: node->comps) {
            child(x->right.get());
        }
    }
    virtual void visit(const MembershipExpression *node) override {
        write("MembershipExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const ConjunctionExpression *node) override {
        write("ConjunctionExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const DisjunctionExpression *node) override {
        write("DisjunctionExpression");
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const ConditionalExpression *node) override {
        write("ConditionalExpression");
        child(node->cond.get());
        child(node->left.get());
        child(node->right.get());
    }
    virtual void visit(const TryExpression *node) override {
        write("TryExpression");
        child(node->expr.get());
    }
    virtual void visit(const NewClassExpression *node) override {
        write("NewClassExpression");
        child(node->expr.get());
    }
    virtual void visit(const ValidPointerExpression *node) override {
        write("ValidPointerExpression");
        for (auto &x: node->tests) {
            child(x->expr.get());
        }
    }
    virtual void visit(const RangeSubscriptExpression *node) override {
        write("RangeSubscriptExpression");
        child(node->base.get());
        child(node->range->first.get());
        child(node->range->last.get());
    }

    virtual void visit(const ImportDeclaration *node) override {
        write("ImportDeclaration(" + node->module.text + "." + node->name.text + ", " + node->alias.text + ")");
    }
    virtual void visit(const TypeDeclaration *node) override {
        write("TypeDeclaration");
        child(node->type.get());
    }
    virtual void visit(const ConstantDeclaration *node) override {
        write("ConstantDeclaration(" + node->name.text + ")");
        child(node->type.get());
        child(node->value.get());
    }
    virtual void visit(const NativeConstantDeclaration *node) override {
        write("NativeConstantDeclaration(" + node->name.text + ")");
        child(node->type.get());
    }
    virtual void visit(const ExtensionConstantDeclaration *node) override {
        write("ExtensionConstantDeclaration(" + node->name.text + ")");
        child(node->type.get());
    }
    virtual void visit(const VariableDeclaration *node) override {
        write("VariableDeclaration(" + join(node->names) + ")");
        child(node->type.get());
        child(node->value.get());
    }
    virtual void visit(const NativeVariableDeclaration *node) override {
        write("NativeVariableDeclaration(" + node->name.text + ")");
        child(node->type.get());
    }
    virtual void visit(const LetDeclaration *node) override {
        write("LetDeclaration(" + node->name.text + ")");
        child(node->type.get());
        child(node->value.get());
    }
    virtual void visit(const FunctionDeclaration *node) override {
        write("FunctionDeclaration(" + node->name.text + ")");
        child(node->returntype.get());
        depth++;
        for (auto &x: node->args) {
            for (auto name: x->names) {
                write(ArgumentModeName[x->mode] + " " + name.text);
                child(x->type.get());
            }
        }
        depth--;
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const ForeignFunctionDeclaration *node) override {
        write("ForeignFunctionDeclaration(" + node->name.text + ")");
        child(node->returntype.get());
        depth++;
        for (auto &x: node->args) {
            for (auto name: x->names) {
                write(ArgumentModeName[x->mode] + " " + name.text);
                child(x->type.get());
            }
        }
        depth--;
        child(node->dict.get());
    }
    virtual void visit(const NativeFunctionDeclaration *node) override {
        write("NativeFunctionDeclaration(" + node->name.text + ")");
        child(node->returntype.get());
        depth++;
        for (auto &x: node->args) {
            for (auto name: x->names) {
                write(ArgumentModeName[x->mode] + " " + name.text);
                child(x->type.get());
            }
        }
        depth--;
    }
    virtual void visit(const ExtensionFunctionDeclaration *node) override {
        write("ExtensionFunctionDeclaration(" + node->name.text + ")");
        child(node->returntype.get());
        depth++;
        for (auto &x: node->args) {
            for (auto name: x->names) {
                write(ArgumentModeName[x->mode] + " " + name.text);
                child(x->type.get());
            }
        }
        depth--;
    }
    virtual void visit(const ExceptionDeclaration *node) override {
        write("ExceptionDeclaration(" + join(node->name) + ")");
    }
    virtual void visit(const InterfaceDeclaration *node) override {
        write("InterfaceDeclaration(" + join(node->methods) + ")");
    }
    virtual void visit(const ExportDeclaration *node) override {
        for (auto &name: node->names) {
            write("ExportDeclaration(" + name.text + ")");
        }
    }

    virtual void visit(const AssertStatement *node) override {
        write("AssertStatement");
        for (auto &e: node->exprs) {
            child(e.get());
        }
    }
    virtual void visit(const AssignmentStatement *node) override {
        write("AssignmentStatement");
        for (auto &x: node->variables) {
            child(x.get());
        }
        child(node->expr.get());
    }
    virtual void visit(const CaseStatement *node) override {
        write("CaseStatement");
        child(node->expr.get());
        depth++;
        for (auto &x: node->clauses) {
            for (auto &when: x.first) {
                const CaseStatement::ComparisonWhenCondition *cw = dynamic_cast<const CaseStatement::ComparisonWhenCondition *>(when.get());
                const CaseStatement::RangeWhenCondition *rw = dynamic_cast<const CaseStatement::RangeWhenCondition *>(when.get());
                if (cw != nullptr) {
                    write("ComparisonWhenCondition(" + ComparisonName[cw->comp] + ")");
                    child(cw->expr.get());
                }
                if (rw != nullptr) {
                    write("RangeWhenCondition");
                    child(rw->low_expr.get());
                    child(rw->high_expr.get());
                }
            }
            for (auto &s: x.second) {
                child(s.get());
            }
        }
        depth--;
    }
    virtual void visit(const CheckStatement *node) override {
        write("CheckStatement");
        child(node->cond.get());
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const ExecStatement *node) override {
        write("ExecStatement(" + node->text + ")");
    }
    virtual void visit(const ExitStatement *node) override {
        write("ExitStatement(" + node->type.text + ")");
    }
    virtual void visit(const ExpressionStatement *node) override {
        write("ExpressionStatement");
        child(node->expr.get());
    }
    virtual void visit(const ForStatement *node) override {
        write("ForStatement(" + node->var.text + ")");
        child(node->start.get());
        child(node->end.get());
        child(node->step.get());
        write("  " + node->label.text);
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const ForeachStatement *node) override {
        write("ForeachStatement(" + node->var.text + ")");
        child(node->array.get());
        write("  " + node->label.text);
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const IfStatement *node) override {
        write("IfStatement");
        for (auto &x: node->condition_statements) {
            child(x.first.get());
            for (auto &y: x.second) {
                child(y.get());
            }
        }
        for (auto &x: node->else_statements) {
            child(x.get());
        }
    }
    virtual void visit(const IncrementStatement *node) override {
        write("IncrementStatement(" + std::to_string(node->delta) + ")");
        child(node->expr.get());
    }
    virtual void visit(const LoopStatement *node) override {
        write("LoopStatement");
        write("  " + node->label.text);
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const NextStatement *node) override {
        write("NextStatement(" + node->type.text + ")");
    }
    virtual void visit(const RaiseStatement *node) override {
        write("RaiseStatement(" + join(node->name) + ")");
        child(node->info.get());
    }
    virtual void visit(const RepeatStatement *node) override {
        write("RepeatStatement");
        write("  " + node->label.text);
        child(node->cond.get());
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const ReturnStatement *node) override {
        write("ReturnStatement");
        child(node->expr.get());
    }
    virtual void visit(const TryStatement *node) override {
        write("TryStatement");
        for (auto &x: node->body) {
            child(x.get());
        }
        // TODO: internal_error("TODO: TryStatement");
    }
    virtual void visit(const TryHandlerStatement *node) override {
        write("TryHandlerStatement");
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const WhileStatement *node) override {
        write("WhileStatement");
        child(node->cond.get());
        write("  " + node->label.text);
        for (auto &x: node->body) {
            child(x.get());
        }
    }
    virtual void visit(const Program *node) override {
        write("Program");
        for (auto &s: node->body) {
            child(s.get());
        }
    }
private:
    std::ostream &out;
    int depth;

    void write(const std::string &s) {
        out << std::string(depth*2, ' ') << s << "\n";
    }

    void child(const ParseTreeNode *node) {
        depth++;
        if (node != nullptr) {
            node->accept(this);
        } else {
            write("nullptr");
        }
        depth--;
    }

    static std::string join(const std::vector<std::string> &a) {
        std::string r;
        for (auto x: a) {
            if (not r.empty()) {
                r += ",";
            }
            r += x;
        }
        return r;
    }

    static std::string join(const std::vector<Token> &a) {
        std::string r;
        for (auto x: a) {
            if (not r.empty()) {
                r += ",";
            }
            r += x.text;
        }
        return r;
    }

    static std::string join(const std::vector<std::pair<Token, int>> &a) {
        std::vector<std::string> b;
        for (auto x: a) {
            b.push_back(x.first.text);
        }
        return join(b);
    }

    static std::string join(const std::vector<std::pair<Token, const Type *>> &a) {
        std::vector<std::string> b;
        for (auto x: a) {
            b.push_back(x.first.text);
        }
        return join(b);
    }

    static std::string join(const std::vector<std::unique_ptr<TypeRecord::Field>> &a) {
        std::string r;
        for (auto &x: a) {
            if (not r.empty()) {
                r += ",";
            }
            r += x->name.text;
        }
        return r;
    }

    static std::string join(const std::vector<std::pair<Token, std::unique_ptr<TypeFunctionPointer>>> &a) {
        std::string r;
        for (auto &x: a) {
            if (not r.empty()) {
                r += ",";
            }
            r += x.first.text;
        }
        return r;
    }

private:
    Dumper(const Dumper &);
    Dumper &operator=(const Dumper &);
};

void pt::dump(std::ostream &out, const ParseTreeNode *node)
{
    Dumper d(out);
    node->accept(&d);
}
