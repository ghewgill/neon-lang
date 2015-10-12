#include "pt_dump.h"

#include <iostream>
#include <iso646.h>

#include "util.h"

using namespace pt;

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
    virtual void visit(const TypePointer *node) override {
        write("TypePointer");
        child(node->reftype);
    }
    virtual void visit(const TypeFunctionPointer *node) override {
        write("TypeFunctionPointer");
        child(node->returntype);
    }
    virtual void visit(const TypeParameterised *node) override {
        write("TypeParameterised(" + node->name.text + ")");
        child(node->elementtype);
    }
    virtual void visit(const TypeImport *node) override {
        write("TypeImport(" + node->modname.text + "." + node->subname.text + ")");
    }

    virtual void visit(const IdentityExpression *node) override {
        write("IdentityExpression");
        child(node->expr);
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
    virtual void visit(const ArrayLiteralExpression *node) override {
        write("ArrayLiteralExpression");
        for (auto x: node->elements) {
            child(x);
        }
    }
    virtual void visit(const DictionaryLiteralExpression *node) override {
        write("DictionaryLiteralExpression");
        depth++;
        for (auto x: node->elements) {
            write(x.first.text);
            child(x.second);
        }
        depth--;
    }
    virtual void visit(const NilLiteralExpression *) override {
        write("NilLiteralExpression");
    }
    virtual void visit(const IdentifierExpression *node) override {
        write("IdentifierExpression(" + node->name + ")");
    }
    virtual void visit(const DotExpression *node) override {
        write("DotExpression(" + node->name.text + ")");
        child(node->base);
    }
    virtual void visit(const ArrowExpression *node) override {
        write("ArrowExpression(" + node->name.text + ")");
        child(node->base);
    }
    virtual void visit(const SubscriptExpression *node) override {
        write("SubscriptExpression");
        child(node->base);
        child(node->index);
    }
    virtual void visit(const InterpolatedStringExpression *node) override {
        write("InterpolatedStringExpression");
        depth++;
        for (auto x: node->parts) {
            write("FormatString(" + x.second.text + ")");
            child(x.first);
        }
        depth--;
    }
    virtual void visit(const FunctionCallExpression *node) override {
        write("FunctionCallExpression");
        child(node->base);
        for (auto x: node->args) {
            child(x.second);
        }
    }
    virtual void visit(const UnaryPlusExpression *node) override {
        write("UnaryPlusExpression");
        child(node->expr);
    }
    virtual void visit(const UnaryMinusExpression *node) override {
        write("UnaryMinusExpression");
        child(node->expr);
    }
    virtual void visit(const LogicalNotExpression *node) override {
        write("LogicalNotExpression");
        child(node->expr);
    }
    virtual void visit(const ExponentiationExpression *node) override {
        write("ExponentiationExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const MultiplicationExpression *node) override {
        write("MultiplicationExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const DivisionExpression *node) override {
        write("DivisionExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const ModuloExpression *node) override {
        write("ModuloExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const AdditionExpression *node) override {
        write("AdditionExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const SubtractionExpression *node) override {
        write("SubtractionExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const ConcatenationExpression *node) override {
        write("ConcatenationExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const ComparisonExpression *node) override {
        write("ComparisonExpression(" + std::to_string(node->comp) + ")");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const ChainedComparisonExpression *node) override {
        write("ChainedComparisonExpression");
        for (auto x: node->comps) {
            child(x);
        }
    }
    virtual void visit(const MembershipExpression *node) override {
        write("MembershipExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const ConjunctionExpression *node) override {
        write("ConjunctionExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const DisjunctionExpression *node) override {
        write("DisjunctionExpression");
        child(node->left);
        child(node->right);
    }
    virtual void visit(const ConditionalExpression *node) override {
        write("ConditionalExpression");
        child(node->cond);
        child(node->left);
        child(node->right);
    }
    virtual void visit(const NewRecordExpression *node) override {
        write("NewRecordExpression");
        child(node->type);
    }
    virtual void visit(const ValidPointerExpression *node) override {
        write("ValidPointerExpression");
        for (auto x: node->tests) {
            child(x.expr);
        }
    }
    virtual void visit(const RangeSubscriptExpression *node) override {
        write("RangeSubscriptExpression");
        child(node->base);
        child(node->range->first);
        child(node->range->last);
    }

    virtual void visit(const ImportDeclaration *node) override {
        write("ImportDeclaration(" + node->module.text + "." + node->name.text + ", " + node->alias.text + ")");
    }
    virtual void visit(const TypeDeclaration *node) override {
        write("TypeDeclaration");
        child(node->type);
    }
    virtual void visit(const ConstantDeclaration *node) override {
        write("ConstantDeclaration(" + node->name.text + ")");
        child(node->type);
        child(node->value);
    }
    virtual void visit(const VariableDeclaration *node) override {
        write("VariableDeclaration(" + join(node->names) + ")");
        child(node->type);
        child(node->value);
    }
    virtual void visit(const LetDeclaration *node) override {
        write("LetDeclaration(" + node->name.text + ")");
        child(node->type);
        child(node->value);
    }
    virtual void visit(const FunctionDeclaration *node) override {
        write("FunctionDeclaration(" + node->name.text + ")");
        child(node->returntype);
        depth++;
        for (auto x: node->args) {
            write(std::to_string(x->mode) + " " + x->name.text);
            child(x->type);
        }
        depth--;
        for (auto x: node->body) {
            child(x);
        }
    }
    virtual void visit(const ExternalFunctionDeclaration *node) override {
        write("ExternalFunctionDeclaration(" + node->name.text + ")");
        child(node->returntype);
        depth++;
        for (auto x: node->args) {
            write(std::to_string(x->mode) + " " + x->name.text);
            child(x->type);
        }
        depth--;
        child(node->dict);
    }
    virtual void visit(const NativeFunctionDeclaration *node) override {
        write("NativeFunctionDeclaration(" + node->name.text + ")");
        child(node->returntype);
        depth++;
        for (auto x: node->args) {
            write(std::to_string(x->mode) + " " + x->name.text);
            child(x->type);
        }
        depth--;
    }
    virtual void visit(const ExceptionDeclaration *node) override {
        write("ExceptionDeclaration(" + node->name.text + ")");
    }
    virtual void visit(const ExportDeclaration *node) override {
        write("ExportDeclaration(" + node->name.text + ")");
    }
    virtual void visit(const MainBlock *node) override {
        write("MainBlock");
        for (auto x: node->body) {
            child(x);
        }
    }

    virtual void visit(const AssertStatement *node) override {
        write("AssertStatement");
        child(node->expr);
    }
    virtual void visit(const AssignmentStatement *node) override {
        write("AssignmentStatement");
        for (auto x: node->variables) {
            child(x);
        }
        child(node->expr);
    }
    virtual void visit(const CaseStatement *node) override {
        write("CaseStatement");
        child(node->expr);
        depth++;
        for (auto x: node->clauses) {
            for (const CaseStatement::WhenCondition *when: x.first) {
                const CaseStatement::ComparisonWhenCondition *cw = dynamic_cast<const CaseStatement::ComparisonWhenCondition *>(when);
                const CaseStatement::RangeWhenCondition *rw = dynamic_cast<const CaseStatement::RangeWhenCondition *>(when);
                if (cw != nullptr) {
                    write("ComparisonWhenCondition(" + std::to_string(cw->comp) + ")");
                    child(cw->expr);
                }
                if (rw != nullptr) {
                    write("RangeWhenCondition");
                    child(rw->low_expr);
                    child(rw->high_expr);
                }
            }
            for (auto s: x.second) {
                child(s);
            }
        }
        depth--;
    }
    virtual void visit(const ExitStatement *node) override {
        write("ExitStatement(" + std::to_string(node->type) + ")");
    }
    virtual void visit(const ExpressionStatement *node) override {
        write("ExpressionStatement");
        child(node->expr);
    }
    virtual void visit(const ForStatement *node) override {
        write("ForStatement(" + node->var.text + ")");
        child(node->start);
        child(node->end);
        child(node->step);
        for (auto x: node->body) {
            child(x);
        }
    }
    virtual void visit(const ForeachStatement *node) override {
        write("ForeachStatement(" + node->var.text + ")");
        child(node->array);
        for (auto x: node->body) {
            child(x);
        }
    }
    virtual void visit(const IfStatement *node) override {
        write("IfStatement");
        for (auto x: node->condition_statements) {
            child(x.first);
            for (auto y: x.second) {
                child(y);
            }
        }
        for (auto x: node->else_statements) {
            child(x);
        }
    }
    virtual void visit(const LoopStatement *node) override {
        write("LoopStatement");
        for (auto x: node->body) {
            child(x);
        }
    }
    virtual void visit(const NextStatement *node) override {
        write("NextStatement(" + std::to_string(node->type) + ")");
    }
    virtual void visit(const RaiseStatement *node) override {
        write("RaiseStatement(" + node->name.first.text + "." + node->name.second.text + ")");
        child(node->info);
    }
    virtual void visit(const RepeatStatement *node) override {
        write("RepeatStatement");
        child(node->cond);
        for (auto x: node->body) {
            child(x);
        }
    }
    virtual void visit(const ReturnStatement *node) override {
        write("ReturnStatement");
        child(node->expr);
    }
    virtual void visit(const TryStatement *node) override {
        write("TryStatement");
        for (auto x: node->body) {
            child(x);
        }
        // TODO: internal_error("TODO: TryStatement");
    }
    virtual void visit(const WhileStatement *node) override {
        write("WhileStatement");
        child(node->cond);
        for (auto x: node->body) {
            child(x);
        }
    }
    virtual void visit(const Program *node) override {
        write("Program");
        for (auto s: node->body) {
            child(s);
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

    static std::string join(const std::vector<TypeRecord::Field> &a) {
        std::string r;
        for (auto x: a) {
            if (not r.empty()) {
                r += ",";
            }
            r += x.name.text;
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
