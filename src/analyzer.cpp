#include "parser.h"

#include <iso646.h>
#include <list>
#include <stack>

#include "ast.h"
#include "format.h"
#include "pt.h"
#include "rtl_compile.h"
#include "util.h"

class Analyzer {
public:
    Analyzer(const pt::Program *program);

    const pt::Program *program;
    static Frame *global_frame; // TODO
    static Scope *global_scope; // TODO: static is a hack, used by StringReference constructor
    std::stack<Frame *> frame;
    std::stack<Scope *> scope;

    std::stack<const TypeFunction *> functiontypes;
    std::stack<std::list<std::pair<TokenType, unsigned int>>> loops;

    const Type *analyze(const pt::Type *type);
    const Type *analyze(const pt::TypeSimple *type);
    const Type *analyze(const pt::TypeEnum *type);
    const Type *analyze(const pt::TypeRecord *type);
    const Type *analyze(const pt::TypePointer *type);
    const Type *analyze(const pt::TypeParameterised *type);
    const Expression *analyze(const pt::Expression *expr);
    const Expression *analyze(const pt::BooleanLiteralExpression *expr);
    const Expression *analyze(const pt::NumberLiteralExpression *expr);
    const Expression *analyze(const pt::StringLiteralExpression *expr);
    const Expression *analyze(const pt::ArrayLiteralExpression *expr);
    const Expression *analyze(const pt::DictionaryLiteralExpression *expr);
    const Expression *analyze(const pt::NilLiteralExpression *expr);
    const Expression *analyze(const pt::IdentifierExpression *expr);
    const Expression *analyze(const pt::DotExpression *expr);
    const Expression *analyze(const pt::ArrowExpression *expr);
    const Expression *analyze(const pt::SubscriptExpression *expr);
    const Expression *analyze(const pt::InterpolatedStringExpression *expr);
    const Expression *analyze(const pt::FunctionCallExpression *expr);
    const Expression *analyze(const pt::UnaryPlusExpression *expr);
    const Expression *analyze(const pt::UnaryMinusExpression *expr);
    const Expression *analyze(const pt::LogicalNotExpression *expr);
    const Expression *analyze(const pt::ExponentiationExpression *expr);
    const Expression *analyze(const pt::MultiplicationExpression *expr);
    const Expression *analyze(const pt::DivisionExpression *expr);
    const Expression *analyze(const pt::ModuloExpression *expr);
    const Expression *analyze(const pt::AdditionExpression *expr);
    const Expression *analyze(const pt::SubtractionExpression *expr);
    const Expression *analyze(const pt::ConcatenationExpression *expr);
    const Expression *analyze(const pt::ComparisonExpression *expr);
    const Expression *analyze(const pt::ChainedComparisonExpression *expr);
    const Expression *analyze(const pt::MembershipExpression *expr);
    const Expression *analyze(const pt::ConjunctionExpression *expr);
    const Expression *analyze(const pt::DisjunctionExpression *expr);
    const Expression *analyze(const pt::ConditionalExpression *expr);
    const Expression *analyze(const pt::NewRecordExpression *expr);
    const Expression *analyze(const pt::ValidPointerExpression *expr);
    const Expression *analyze(const pt::RangeSubscriptExpression *expr);
    const Statement *analyze(const pt::ImportDeclaration *declaration);
    const Statement *analyze(const pt::TypeDeclaration *declaration);
    const Statement *analyze_decl(const pt::ConstantDeclaration *declaration);
    const Statement *analyze_body(const pt::ConstantDeclaration *declaration);
    const Statement *analyze_decl(const pt::VariableDeclaration *declaration);
    const Statement *analyze_body(const pt::VariableDeclaration *declaration);
    const Statement *analyze_decl(const pt::LetDeclaration *declaration);
    const Statement *analyze_body(const pt::LetDeclaration *declaration);
    const Statement *analyze_decl(const pt::FunctionDeclaration *declaration);
    const Statement *analyze_body(const pt::FunctionDeclaration *declaration);
    const Statement *analyze_decl(const pt::ExternalFunctionDeclaration *declaration);
    const Statement *analyze_body(const pt::ExternalFunctionDeclaration *declaration);
    const Statement *analyze(const pt::ExceptionDeclaration *declaration);
    std::vector<const Statement *> analyze(const std::vector<const pt::Statement *> &statement);
    const Statement *analyze(const pt::AssignmentStatement *statement);
    const Statement *analyze(const pt::CaseStatement *statement);
    const Statement *analyze(const pt::ExitStatement *statement);
    const Statement *analyze(const pt::ExpressionStatement *statement);
    const Statement *analyze(const pt::ForStatement *statement);
    const Statement *analyze(const pt::IfStatement *statement);
    const Statement *analyze(const pt::LoopStatement *statement);
    const Statement *analyze(const pt::NextStatement *statement);
    const Statement *analyze(const pt::RaiseStatement *statement);
    const Statement *analyze(const pt::RepeatStatement *statement);
    const Statement *analyze(const pt::ReturnStatement *statement);
    const Statement *analyze(const pt::TryStatement *statement);
    const Statement *analyze(const pt::WhileStatement *statement);
    const Program *analyze();
private:
    Analyzer(const Analyzer &);
    Analyzer &operator=(const Analyzer &);
};

class TypeAnalyzer: public pt::IParseTreeVisitor {
public:
    TypeAnalyzer(Analyzer *a): type(nullptr), a(a) {}
    virtual void visit(const pt::TypeSimple *t) { type = a->analyze(t); }
    virtual void visit(const pt::TypeEnum *t) { type = a->analyze(t); }
    virtual void visit(const pt::TypeRecord *t) { type = a->analyze(t); }
    virtual void visit(const pt::TypePointer *t) { type = a->analyze(t); }
    virtual void visit(const pt::TypeParameterised *t) { type = a->analyze(t); }
    virtual void visit(const pt::BooleanLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentifierExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DotExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrowExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubscriptExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::InterpolatedStringExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::FunctionCallExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryPlusExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryMinusExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::LogicalNotExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ExponentiationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::MultiplicationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DivisionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ModuloExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::AdditionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubtractionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConcatenationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ComparisonExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ChainedComparisonExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewRecordExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::AssignmentStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::CaseStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExitStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExpressionStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::IfStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::LoopStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::NextStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::RaiseStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::RepeatStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ReturnStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::WhileStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::Program *) { internal_error("pt::Program"); }
    const Type *type;
private:
    Analyzer *a;
private:
    TypeAnalyzer(const TypeAnalyzer &);
    TypeAnalyzer &operator=(const TypeAnalyzer &);
};

class ExpressionAnalyzer: public pt::IParseTreeVisitor {
public:
    ExpressionAnalyzer(Analyzer *a): expr(nullptr), a(a) {}
    virtual void visit(const pt::TypeSimple *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeEnum *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeRecord *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) { internal_error("pt::Type"); }
    virtual void visit(const pt::BooleanLiteralExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::NumberLiteralExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::StringLiteralExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ArrayLiteralExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::DictionaryLiteralExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::NilLiteralExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::IdentifierExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::DotExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ArrowExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::SubscriptExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::InterpolatedStringExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::FunctionCallExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::UnaryPlusExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::UnaryMinusExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::LogicalNotExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ExponentiationExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::MultiplicationExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::DivisionExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ModuloExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::AdditionExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::SubtractionExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ConcatenationExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ComparisonExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ChainedComparisonExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::MembershipExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ConjunctionExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::DisjunctionExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ConditionalExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::NewRecordExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ValidPointerExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::RangeSubscriptExpression *p) { expr = a->analyze(p); }
    virtual void visit(const pt::ImportDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::AssignmentStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::CaseStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExitStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExpressionStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::IfStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::LoopStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::NextStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::RaiseStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::RepeatStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::ReturnStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::WhileStatement *) { internal_error("pt::Statement"); }
    virtual void visit(const pt::Program *) { internal_error("pt::Program"); }
    const Expression *expr;
private:
    Analyzer *a;
private:
    ExpressionAnalyzer(const ExpressionAnalyzer &);
    ExpressionAnalyzer &operator=(const ExpressionAnalyzer &);
};

class DeclarationAnalyzer: public pt::IParseTreeVisitor {
public:
    DeclarationAnalyzer(Analyzer *a, std::vector<const Statement *> &v): a(a), v(v) {}
    virtual void visit(const pt::TypeSimple *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeEnum *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeRecord *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) { internal_error("pt::Type"); }
    virtual void visit(const pt::BooleanLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentifierExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DotExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrowExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubscriptExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::InterpolatedStringExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::FunctionCallExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryPlusExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryMinusExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::LogicalNotExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ExponentiationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::MultiplicationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DivisionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ModuloExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::AdditionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubtractionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConcatenationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ComparisonExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ChainedComparisonExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewRecordExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TypeDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ConstantDeclaration *p) { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::VariableDeclaration *p) { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::LetDeclaration *p) { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::ExceptionDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::AssignmentStatement *) {}
    virtual void visit(const pt::CaseStatement *) {}
    virtual void visit(const pt::ExitStatement *) {}
    virtual void visit(const pt::ExpressionStatement *) {}
    virtual void visit(const pt::ForStatement *) {}
    virtual void visit(const pt::IfStatement *) {}
    virtual void visit(const pt::LoopStatement *) {}
    virtual void visit(const pt::NextStatement *) {}
    virtual void visit(const pt::RaiseStatement *) {}
    virtual void visit(const pt::RepeatStatement *) {}
    virtual void visit(const pt::ReturnStatement *) {}
    virtual void visit(const pt::TryStatement *) {}
    virtual void visit(const pt::WhileStatement *) {}
    virtual void visit(const pt::Program *) { internal_error("pt::Program"); }
private:
    Analyzer *a;
    std::vector<const Statement *> &v;
private:
    DeclarationAnalyzer(const DeclarationAnalyzer &);
    DeclarationAnalyzer &operator=(const DeclarationAnalyzer &);
};

class StatementAnalyzer: public pt::IParseTreeVisitor {
public:
    StatementAnalyzer(Analyzer *a, std::vector<const Statement *> &v): a(a), v(v) {}
    virtual void visit(const pt::TypeSimple *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeEnum *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeRecord *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) { internal_error("pt::Type"); }
    virtual void visit(const pt::BooleanLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentifierExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DotExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrowExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubscriptExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::InterpolatedStringExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::FunctionCallExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryPlusExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryMinusExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::LogicalNotExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ExponentiationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::MultiplicationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DivisionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ModuloExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::AdditionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubtractionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConcatenationExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ComparisonExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ChainedComparisonExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewRecordExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) {}
    virtual void visit(const pt::TypeDeclaration *) {}
    virtual void visit(const pt::ConstantDeclaration *p) { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::VariableDeclaration *p) { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::LetDeclaration *p) { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::ExceptionDeclaration *) {}
    virtual void visit(const pt::AssignmentStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::CaseStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExitStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExpressionStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ForStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::IfStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::LoopStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::NextStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::RaiseStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::RepeatStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ReturnStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TryStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::WhileStatement *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::Program *) { internal_error("pt::Program"); }
private:
    Analyzer *a;
    std::vector<const Statement *> &v;
private:
    StatementAnalyzer(const StatementAnalyzer &);
    StatementAnalyzer &operator=(const StatementAnalyzer &);
};

Frame *Analyzer::global_frame;
Scope *Analyzer::global_scope;

Analyzer::Analyzer(const pt::Program *program)
  : program(program),
    frame(),
    scope(),
    functiontypes(),
    loops()
{
}

TypeEnum::TypeEnum(const Token &declaration, const std::map<std::string, int> &names)
  : TypeNumber(declaration),
    names(names)
{
    {
        std::vector<FunctionParameter *> params;
        FunctionParameter *fp = new FunctionParameter(Token(), "self", this, ParameterType::INOUT);
        params.push_back(fp);
        Function *f = new Function(Token(), "enum.to_string", TYPE_STRING, Analyzer::global_frame, Analyzer::global_scope, params);
        std::vector<const Expression *> values;
        for (auto n: names) {
            if (n.second < 0) {
                internal_error("TypeEnum");
            }
            if (values.size() < static_cast<size_t>(n.second)+1) {
                values.resize(n.second+1);
            }
            if (values[n.second] != nullptr) {
                internal_error("TypeEnum");
            }
            values[n.second] = new ConstantStringExpression(n.first);
        }
        f->statements.push_back(new ReturnStatement(0, new ArrayValueIndexExpression(TYPE_STRING, new ArrayLiteralExpression(TYPE_STRING, values), new VariableExpression(fp), false)));
        methods["to_string"] = f;
    }
}

StringReferenceIndexExpression::StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end)
  : ReferenceExpression(ref->type, ref->is_readonly),
    ref(ref),
    first(first),
    first_from_end(first_from_end),
    last(last),
    last_from_end(last_from_end),
    load(nullptr),
    store(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("string__substring"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("string__splice"))), args);
    }
}

StringValueIndexExpression::StringValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end)
  : Expression(str->type, str->is_readonly),
    str(str),
    first(first),
    first_from_end(first_from_end),
    last(last),
    last_from_end(last_from_end),
    load(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(str);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("string__substring"))), args);
    }
}

ArrayReferenceRangeExpression::ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end)
  : ReferenceExpression(ref->type, ref->is_readonly),
    ref(ref),
    first(first),
    first_from_end(first_from_end),
    last(last),
    last_from_end(last_from_end),
    load(nullptr),
    store(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("array__slice"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("array__splice"))), args);
    }
}

ArrayValueRangeExpression::ArrayValueRangeExpression(const Expression *array, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end)
  : Expression(array->type, false),
    array(array),
    first(first),
    first_from_end(first_from_end),
    last(last),
    last_from_end(last_from_end),
    load(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(array);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("array__slice"))), args);
    }
}

const Type *Analyzer::analyze(const pt::Type *type)
{
    TypeAnalyzer ta(this);
    type->accept(&ta);
    return ta.type;
}

const Type *Analyzer::analyze(const pt::TypeSimple *type)
{
    const Name *name = scope.top()->lookupName(type->name);
    if (name == nullptr) {
        error(3011, type->token, "unknown type name");
    }
    const Type *r = dynamic_cast<const Type *>(name);
    if (r == nullptr) {
        error2(3012, type->token, name->declaration, "name is not a type");
    }
    return r;
}

const Type *Analyzer::analyze(const pt::TypeEnum *type)
{
    std::map<std::string, int> names;
    int index = 0;
    for (auto x: type->names) {
        std::string name = x.first.text;
        auto t = names.find(name);
        if (t != names.end()) {
            error2(3010, x.first, type->names[t->second].first, "duplicate enum: " + name);
        }
        names[name] = index;
        index++;
    }
    return new TypeEnum(type->token, names);
}

const Type *Analyzer::analyze(const pt::TypeRecord *type)
{
    std::vector<std::pair<Token, const Type *>> fields;
    std::map<std::string, Token> field_names;
    for (auto x: type->fields) {
        std::string name = x.first.text;
        auto prev = field_names.find(name);
        if (prev != field_names.end()) {
            error2(3009, x.first, prev->second, "duplicate field: " + x.first.text);
        }
        const Type *t = analyze(x.second);
        fields.push_back(std::make_pair(x.first, t));
        field_names[name] = x.first;
    }
    return new TypeRecord(type->token, fields);
}

const Type *Analyzer::analyze(const pt::TypePointer *type)
{
    if (type->reftype != nullptr) {
        const pt::TypeSimple *simple = dynamic_cast<const pt::TypeSimple *>(type->reftype);
        if (simple != nullptr && scope.top()->lookupName(simple->name) == nullptr) {
            const std::string name = simple->name;
            TypePointer *ptrtype = new TypePointer(type->token, new TypeForwardRecord(type->reftype->token));
            scope.top()->addForward(name, ptrtype);
            return ptrtype;
        } else {
            const Type *reftype = analyze(type->reftype);
            const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(reftype);
            if (rectype == nullptr) {
                error(3098, type->reftype->token, "record type expected");
            }
            return new TypePointer(type->token, rectype);
        }
    } else {
        return new TypePointer(type->token, nullptr);
    }
}

const Type *Analyzer::analyze(const pt::TypeParameterised *type)
{
    if (type->name.text == "Array") {
        return new TypeArray(type->name, analyze(type->elementtype));
    }
    if (type->name.text == "Dictionary") {
        return new TypeDictionary(type->name, analyze(type->elementtype));
    }
    internal_error("Invalid parameterized type");
}

const Expression *Analyzer::analyze(const pt::Expression *expr)
{
    ExpressionAnalyzer ea(this);
    expr->accept(&ea);
    return ea.expr;
}

const Expression *Analyzer::analyze(const pt::BooleanLiteralExpression *expr)
{
    return new ConstantBooleanExpression(expr->value);
}

const Expression *Analyzer::analyze(const pt::NumberLiteralExpression *expr)
{
    return new ConstantNumberExpression(expr->value);
}

const Expression *Analyzer::analyze(const pt::StringLiteralExpression *expr)
{
    return new ConstantStringExpression(expr->value);
}

const Expression *Analyzer::analyze(const pt::ArrayLiteralExpression *expr)
{
    std::vector<const Expression *> elements;
    const Type *elementtype = nullptr;
    for (auto x: expr->elements) {
        const Expression *element = analyze(x);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(3079, x->token, "type mismatch");
        }
        elements.push_back(element);
    }
    return new ArrayLiteralExpression(elementtype, elements);
}

const Expression *Analyzer::analyze(const pt::DictionaryLiteralExpression *expr)
{
    std::vector<std::pair<std::string, const Expression *>> elements;
    std::map<std::string, Token> keys;
    const Type *elementtype = nullptr;
    for (auto x: expr->elements) {
        std::string key = x.first.text;
        auto i = keys.find(key);
        if (i != keys.end()) {
            error2(3080, x.first, i->second, "duplicate key");
        }
        keys[key] = x.first;
        const Expression *element = analyze(x.second);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(3072, x.second->token, "type mismatch");
        }
        elements.push_back(std::make_pair(key, element));
    }
    return new DictionaryLiteralExpression(elementtype, elements);
}

const Expression *Analyzer::analyze(const pt::NilLiteralExpression *)
{
    return new ConstantNilExpression();
}

const Expression *Analyzer::analyze(const pt::IdentifierExpression *expr)
{
    const Name *name = scope.top()->lookupName(expr->name);
    if (name == nullptr) {
        error(3039, expr->token, "name not found: " + expr->name);
    }
    const Constant *constant = dynamic_cast<const Constant *>(name);
    if (constant != nullptr) {
        return new ConstantExpression(constant);
    }
    const Variable *var = dynamic_cast<const Variable *>(name);
    if (var != nullptr) {
        return new VariableExpression(var);
    }
    error(3040, expr->token, "name is not a constant or variable: " + expr->name);
}

const Expression *Analyzer::analyze(const pt::DotExpression *expr)
{
    const pt::IdentifierExpression *scopename = dynamic_cast<const pt::IdentifierExpression *>(expr->base);
    if (scopename != nullptr) {
        const Name *name = scope.top()->lookupName(scopename->name);
        const Module *module = dynamic_cast<const Module *>(name);
        if (module != nullptr) {
            const Name *name = module->scope->lookupName(expr->name.text);
            if (name == nullptr) {
                error(3134, expr->name, "name not found: " + expr->name.text);
            }
            const Constant *constant = dynamic_cast<const Constant *>(name);
            if (constant != nullptr) {
                return new ConstantExpression(constant);
            }
            const Variable *var = dynamic_cast<const Variable *>(name);
            if (var != nullptr) {
                return new VariableExpression(var);
            }
            error(3135, expr->token, "name is not a variable: " + expr->name.text);
        }
        const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(name);
        if (enumtype != nullptr) {
            auto name = enumtype->names.find(expr->name.text);
            if (name == enumtype->names.end()) {
                error2(3023, expr->name, enumtype->declaration, "identifier not member of enum: " + expr->name.text);
            }
            return new ConstantEnumExpression(enumtype, name->second);
        }
    }
    const Expression *base = analyze(expr->base);
    const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(base->type);
    if (recordtype != nullptr) {
        if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
            internal_error("record not defined yet");
        }
        auto f = recordtype->field_names.find(expr->name.text);
        if (f == recordtype->field_names.end()) {
            error2(3045, expr->name, recordtype->declaration, "field not found");
        }
        const Type *type = recordtype->fields[f->second].second;
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
        } else {
            return new ArrayValueIndexExpression(type, base, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
        }
    }
    error(3046, expr->base->token, "no method found or not a record");
}

const Expression *Analyzer::analyze(const pt::ArrowExpression *expr)
{
    const Expression *base = analyze(expr->base);
    const TypePointer *pointertype = dynamic_cast<const TypePointer *>(base->type);
    if (pointertype == nullptr) {
        error(3112, expr->base->token, "not a pointer");
    }
    if (dynamic_cast<const TypeValidPointer *>(pointertype) == nullptr) {
        error(3103, expr->base->token, "pointer must be a valid pointer");
    }
    const TypeRecord *recordtype = pointertype->reftype;
    if (recordtype == nullptr) {
        error(3117, expr->base->token, "pointer must not be a generic pointer");
    }
    if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
        error2(3104, expr->base->token, recordtype->declaration, "record not defined yet");
    }
    auto f = recordtype->field_names.find(expr->name.text);
    if (f == recordtype->field_names.end()) {
        error2(3111, expr->name, recordtype->declaration, "field not found");
    }
    const Type *type = recordtype->fields[f->second].second;
    const PointerDereferenceExpression *ref = new PointerDereferenceExpression(type, base);
    return new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), false);
}

const Expression *Analyzer::analyze(const pt::SubscriptExpression *expr)
{
    const Expression *base = analyze(expr->base);
    const Expression *index = analyze(expr->index);
    const Type *type = base->type;
    const TypeArray *arraytype = dynamic_cast<const TypeArray *>(type);
    const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(type);
    if (arraytype != nullptr) {
        if (not index->type->is_equivalent(TYPE_NUMBER)) {
            error2(3041, expr->index->token, arraytype->declaration, "index must be a number");
        }
        type = arraytype->elementtype;
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        // This check for ArrayReferenceRangeExpression is a stupendous hack that
        // allows expressions like a[LAST] (which is compiled as a[LAST TO LAST][0])
        // to work. Without this, the subscript [0] on the range expression tries
        // to generate the address of the base so it can apply an INDEXAR opcode.
        // Since the range expression is really a function call, it has no address.
        if (ref != nullptr && dynamic_cast<const ArrayReferenceRangeExpression *>(ref) == nullptr) {
            return new ArrayReferenceIndexExpression(type, ref, index, false);
        } else {
            return new ArrayValueIndexExpression(type, base, index, false);
        }
    } else if (dicttype != nullptr) {
        if (not index->type->is_equivalent(TYPE_STRING)) {
            error2(3042, expr->index->token, dicttype->declaration, "index must be a string");
        }
        type = dicttype->elementtype;
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new DictionaryReferenceIndexExpression(type, ref, index);
        } else {
            return new DictionaryValueIndexExpression(type, base, index);
        }
    } else if (type == TYPE_STRING) {
        if (not index->type->is_equivalent(TYPE_NUMBER)) {
            error(3043, expr->index->token, "index must be a number");
        }
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new StringReferenceIndexExpression(ref, index, false, index, false);
        } else {
            return new StringValueIndexExpression(base, index, false, index, false);
        }
    } else {
        error2(3044, expr->token, type->declaration, "not an array or dictionary");
    }
}

const Expression *Analyzer::analyze(const pt::InterpolatedStringExpression *expr)
{
    const VariableExpression *concat = new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("concat")));
    const VariableExpression *format = new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("format")));
    const Expression *r = nullptr;
    for (auto x: expr->parts) {
        const Expression *e = analyze(x.first);
        std::string fmt = x.second;
        if (not fmt.empty()) {
            format::Spec spec;
            if (not format::parse(fmt, spec)) {
                error(3133, x.first->token, "invalid format specification");
            }
        }
        const Expression *str;
        if (e->type->is_equivalent(TYPE_STRING)) {
            str = e;
        } else {
            auto to_string = e->type->methods.find("to_string");
            if (to_string == e->type->methods.end()) {
                error(3132, x.first->token, "no to_string() method found for type");
            }
            {
                std::vector<const Expression *> args;
                args.push_back(e);
                str = new FunctionCall(new VariableExpression(to_string->second), args);
            }
        }
        if (not fmt.empty()) {
            std::vector<const Expression *> args;
            args.push_back(str);
            args.push_back(new ConstantStringExpression(fmt));
            str = new FunctionCall(format, args);
        }
        if (r == nullptr) {
            r = str;
        } else {
            std::vector<const Expression *> args;
            args.push_back(r);
            args.push_back(str);
            r = new FunctionCall(concat, args);
        }
    }
    return r;
}

const Expression *Analyzer::analyze(const pt::FunctionCallExpression *expr)
{
    const pt::IdentifierExpression *fname = dynamic_cast<const pt::IdentifierExpression *>(expr->base);
    if (fname != nullptr) {
        if (fname->name == "value_copy") {
            if (expr->args.size() != 2) {
                error(3136, expr->token, "two arguments expected");
            }
            const Expression *lhs_expr = analyze(expr->args[0]);
            const ReferenceExpression *lhs = dynamic_cast<const ReferenceExpression *>(lhs_expr);
            if (lhs == nullptr) {
                error(3119, expr->args[0]->token, "expression is not assignable");
            }
            if (lhs_expr->is_readonly && dynamic_cast<const TypePointer *>(lhs_expr->type) == nullptr) {
                error(3120, expr->args[0]->token, "value_copy to readonly expression");
            }
            const Expression *rhs = analyze(expr->args[1]);
            const Type *ltype = lhs->type;
            const TypePointer *lptype = dynamic_cast<const TypePointer *>(ltype);
            if (lptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(lptype) == nullptr) {
                    error(3121, expr->args[0]->token, "valid pointer type required");
                }
                ltype = lptype->reftype;
                lhs = new PointerDereferenceExpression(ltype, lhs);
            }
            const Type *rtype = rhs->type;
            const TypePointer *rptype = dynamic_cast<const TypePointer *>(rtype);
            if (rptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(rptype) == nullptr) {
                    error(3122, expr->args[1]->token, "valid pointer type required");
                }
                rtype = rptype->reftype;
                rhs = new PointerDereferenceExpression(rtype, rhs);
            }
            if (not ltype->is_equivalent(rtype)) {
                error(3123, expr->args[1]->token, "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(lhs);
            return new StatementExpression(new AssignmentStatement(expr->token.line, vars, rhs));
        }
        const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(scope.top()->lookupName(fname->name));
        if (recordtype != nullptr) {
            if (expr->args.size() > recordtype->fields.size()) {
                error2(3130, expr->args[recordtype->fields.size()]->token, recordtype->declaration, "wrong number of fields");
            }
            std::vector<const Expression *> elements;
            auto f = recordtype->fields.begin();
            for (auto x: expr->args) {
                const Expression *element = analyze(x);
                if (not element->type->is_equivalent(f->second)) {
                    error2(3131, x->token, f->first, "type mismatch");
                }
                elements.push_back(element);
                ++f;
            }
            return new RecordLiteralExpression(recordtype, elements);
        }
    }
    const pt::DotExpression *dotmethod = dynamic_cast<const pt::DotExpression *>(expr->base);
    const Expression *self = nullptr;
    const Expression *func = nullptr;
    if (dotmethod != nullptr) {
        // This check avoids trying to evaluate foo.bar as an
        // expression in foo.bar() where foo is actually a module.
        bool is_module_call = false;
        const pt::IdentifierExpression *ident = dynamic_cast<const pt::IdentifierExpression *>(dotmethod->base);
        if (ident != nullptr) {
            const Name *name = scope.top()->lookupName(ident->name);
            is_module_call = dynamic_cast<const Module *>(name) != nullptr;
        }
        if (not is_module_call) {
            const Expression *base = analyze(dotmethod->base);
            auto m = base->type->methods.find(dotmethod->name.text);
            if (m == base->type->methods.end()) {
                error(3137, dotmethod->name, "method not found");
            } else {
                self = base;
            }
            func = new VariableExpression(m->second);
        } else {
            func = analyze(expr->base);
        }
    } else {
        func = analyze(expr->base);
    }
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        error(3017, expr->base->token, "not a function");
    }
    std::vector<const Type *>::size_type p = 0;
    std::vector<const Expression *> args;
    if (self != nullptr) {
        args.push_back(self);
        ++p;
    }
    for (const pt::Expression *a: expr->args) {
        const Expression *e = analyze(a);
        if (p >= ftype->params.size()) {
            error(3096, a->token, "too many parameters");
        }
        if (ftype->params[p]->mode != ParameterType::IN) {
            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
            if (ref == nullptr) {
                error2(3018, a->token, ftype->params[p]->declaration, "function call argument must be reference: " + e->text());
            }
            if (ref->is_readonly) {
                error(3106, a->token, "readonly parameter to OUT");
            }
        }
        if (not e->type->is_equivalent(ftype->params[p]->type)) {
            error2(3019, a->token, ftype->params[p]->declaration, "type mismatch");
        }
        args.push_back(e);
        ++p;
    }
    if (p < ftype->params.size()) {
        error(3020, expr->token, "not enough arguments (got " + std::to_string(p) + ", expected " + std::to_string(ftype->params.size()));
    }
    return new FunctionCall(func, args);
}

const Expression *Analyzer::analyze(const pt::UnaryPlusExpression *expr)
{
    const Expression *atom = analyze(expr->expr);
    if (not atom->type->is_equivalent(TYPE_NUMBER)) {
        error(3144, expr->expr->token, "number required");
    }
    return atom;
}

const Expression *Analyzer::analyze(const pt::UnaryMinusExpression *expr)
{
    const Expression *atom = analyze(expr->expr);
    if (not atom->type->is_equivalent(TYPE_NUMBER)) {
        error(3021, expr->expr->token, "number required for negation");
    }
    return new UnaryMinusExpression(atom);
}

const Expression *Analyzer::analyze(const pt::LogicalNotExpression *expr)
{
    const Expression *atom = analyze(expr->expr);
    if (not atom->type->is_equivalent(TYPE_BOOLEAN)) {
        error(3022, expr->expr->token, "boolean required for logical not");
    }
    return new LogicalNotExpression(atom);
}

const Expression *Analyzer::analyze(const pt::ExponentiationExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new ExponentiationExpression(left, right);
    } else {
        error(3024, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::MultiplicationExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new MultiplicationExpression(left, right);
    } else {
        error(3025, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::DivisionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new DivisionExpression(left, right);
    } else {
        error(3026, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ModuloExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new ModuloExpression(left, right);
    } else {
        error(3027, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::AdditionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new AdditionExpression(left, right);
    } else if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
        error(3124, expr->token, "type mismatch (use & to concatenate strings)");
    } else {
        error(3028, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::SubtractionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new SubtractionExpression(left, right);
    } else {
        error(3029, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ConcatenationExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
        std::vector<const Expression *> args;
        args.push_back(left);
        args.push_back(right);
        return new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("concat"))), args);
    } else {
        error(3116, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ComparisonExpression *expr)
{
    const Expression *left = analyze(expr->left);
    ComparisonExpression::Comparison comp = (ComparisonExpression::Comparison)expr->comp; // TODO: remove cast
    const Expression *right = analyze(expr->right);
    if (not left->type->is_equivalent(right->type)) {
        error(3030, expr->token, "type mismatch");
    }
    if (left->type->is_equivalent(TYPE_BOOLEAN)) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3031, expr->token, "comparison not available for Boolean");
        }
        return new BooleanComparisonExpression(left, right, comp);
    } else if (left->type->is_equivalent(TYPE_NUMBER)) {
        return new NumericComparisonExpression(left, right, comp);
    } else if (left->type->is_equivalent(TYPE_STRING)) {
        return new StringComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3032, expr->token, "comparison not available for Array");
        }
        return new ArrayComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeDictionary *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3033, expr->token, "comparison not available for Dictionary");
        }
        return new DictionaryComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeRecord *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3034, expr->token, "comparison not available for RECORD");
        }
        return new ArrayComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeEnum *>(left->type) != nullptr) {
        return new NumericComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypePointer *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3100, expr->token, "comparison not available for POINTER");
        }
        return new PointerComparisonExpression(left, right, comp);
    } else {
        internal_error("unknown type in parseComparison");
    }
}

const Expression *Analyzer::analyze(const pt::ChainedComparisonExpression *expr)
{
    std::vector<const ComparisonExpression *> comps;
    for (auto x: expr->comps) {
        const Expression *expr = analyze(x);
        const ComparisonExpression *comp = dynamic_cast<const ComparisonExpression *>(expr);
        if (comp == nullptr) {
            internal_error("ChainedComparisonExpression");
        }
        comps.push_back(comp);
    }
    return new ChainedComparisonExpression(comps);
}

const Expression *Analyzer::analyze(const pt::MembershipExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    const TypeArray *arraytype = dynamic_cast<const TypeArray *>(right->type);
    const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(right->type);
    if (arraytype != nullptr) {
        if (not left->type->is_equivalent(arraytype->elementtype)) {
            error(3082, expr->left->token, "type mismatch");
        }
        return new ArrayInExpression(left, right);
    } else if (dicttype != nullptr) {
        if (not left->type->is_equivalent(TYPE_STRING)) {
            error(3083, expr->left->token, "type mismatch");
        }
        return new DictionaryInExpression(left, right);
    } else {
        error(3081, expr->right->token, "IN must be used with Array or Dictionary");
    }
}

const Expression *Analyzer::analyze(const pt::ConjunctionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
        return new ConjunctionExpression(left, right);
    } else {
        error(3035, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::DisjunctionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
        return new DisjunctionExpression(left, right);
    } else {
        error(3036, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ConditionalExpression *expr)
{
    const Expression *cond = analyze(expr->cond);
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (not left->type->is_equivalent(right->type)) {
        error(3037, expr->left->token, "type of THEN and ELSE must match");
    }
    return new ConditionalExpression(cond, left, right);
}

const Expression *Analyzer::analyze(const pt::NewRecordExpression *expr)
{
    const TypeRecord *type = dynamic_cast<const TypeRecord *>(analyze(expr->type));
    if (type == nullptr) {
        error(3099, expr->type->token, "record type expected");
    }
    return new NewRecordExpression(type);
}

const Expression *Analyzer::analyze(const pt::ValidPointerExpression * /*expr*/)
{
    // TODO: This happens with multiple pointers in an IF VALID statement.
    internal_error("TODO pt::Expression");
}

const Expression *Analyzer::analyze(const pt::RangeSubscriptExpression *expr)
{
    const Expression *base = analyze(expr->base);
    const Expression *first = analyze(expr->range->first);
    const Expression *last = analyze(expr->range->last);
    if (not first->type->is_equivalent(TYPE_NUMBER)) {
        error(3141, expr->range->first->token, "range index must be a number");
    }
    if (not last->type->is_equivalent(TYPE_NUMBER)) {
        error(3142, expr->range->last->token, "range index must be a number");
    }
    const Type *type = base->type;
    const TypeArray *arraytype = dynamic_cast<const TypeArray *>(type);
    if (arraytype != nullptr) {
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ArrayReferenceRangeExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end);
        } else {
            return new ArrayValueRangeExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end);
        }
    } else if (type == TYPE_STRING) {
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new StringReferenceIndexExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end);
        } else {
            return new StringValueIndexExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end);
        }
    } else {
        error2(3143, expr->base->token, type->declaration, "not an array or string");
    }
}

const Statement *Analyzer::analyze(const pt::ImportDeclaration *declaration)
{
    if (not scope.top()->allocateName(declaration->name, declaration->name.text)) {
        error2(3114, declaration->name, scope.top()->getDeclaration(declaration->name.text), "duplicate definition of name");
    }
    rtl_import(declaration->name, scope.top(), declaration->name.text);
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::TypeDeclaration *declaration)
{
    std::string name = declaration->token.text;
    if (not scope.top()->allocateName(declaration->token, name)) {
        error2(3013, declaration->token, scope.top()->getDeclaration(name), "name shadows outer");
    }
    const Type *type = analyze(declaration->type);
    scope.top()->addName(declaration->token, name, const_cast<Type *>(type)); // Still ugly.
    const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
    if (rectype != nullptr) {
        scope.top()->resolveForward(name, rectype);
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_decl(const pt::ConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3014, declaration->token, scope.top()->getDeclaration(declaration->name.text), "name shadows outer");
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::ConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    const Type *type = analyze(declaration->type);
    const Expression *value = analyze(declaration->value);
    if (not value->type->is_equivalent(type)) {
        error(3015, declaration->value->token, "type mismatch");
    }
    if (not value->is_constant) {
        error(3016, declaration->value->token, "value must be constant");
    }
    scope.top()->addName(declaration->name, name, new Constant(declaration->name, name, value));
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_decl(const pt::VariableDeclaration *declaration)
{
    for (auto name: declaration->names) {
        if (not scope.top()->allocateName(name, name.text)) {
            error2(3038, name, scope.top()->getDeclaration(name.text), "name shadows outer");
        }
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::VariableDeclaration *declaration)
{
    const Type *type = analyze(declaration->type);
    std::vector<Variable *> variables;
    for (auto name: declaration->names) {
        Variable *v;
        if (frame.top() == global_frame) {
            v = new GlobalVariable(name, name.text, type, false);
        } else {
            v = new LocalVariable(name, name.text, type, false);
        }
        variables.push_back(v);
    }
    if (declaration->value != nullptr) {
        std::vector<const ReferenceExpression *> refs;
        const Expression *expr = analyze(declaration->value);
        if (not expr->type->is_equivalent(type)) {
            error(3113, declaration->value->token, "type mismatch");
        }
        for (auto v: variables) {
            scope.top()->addName(v->declaration, v->name, v, true);
            refs.push_back(new VariableExpression(v));
        }
        return new AssignmentStatement(declaration->token.line, refs, expr);
    } else {
        for (auto v: variables) {
            scope.top()->addName(v->declaration, v->name, v);
        }
        return new NullStatement(declaration->token.line);
    }
}

const Statement *Analyzer::analyze_decl(const pt::LetDeclaration *declaration)
{
    if (not scope.top()->allocateName(declaration->name, declaration->name.text)) {
        error2(3139, declaration->name, scope.top()->getDeclaration(declaration->name.text), "name shadows outer");
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::LetDeclaration *declaration)
{
    const Type *type = analyze(declaration->type);
    Variable *v;
    if (frame.top() == global_frame) {
        v = new GlobalVariable(declaration->name, declaration->name.text, type, true);
    } else {
        v = new LocalVariable(declaration->name, declaration->name.text, type, true);
    }
    const Expression *expr = analyze(declaration->value);
    if (not expr->type->is_equivalent(type)) {
        error(3140, declaration->value->token, "type mismatch");
    }
    scope.top()->addName(v->declaration, v->name, v, true);
    std::vector<const ReferenceExpression *> refs;
    refs.push_back(new VariableExpression(v));
    return new AssignmentStatement(declaration->token.line, refs, expr);
}

const Statement *Analyzer::analyze_decl(const pt::FunctionDeclaration *declaration)
{
    const std::string classtype = declaration->type.text;
    Type *type = nullptr;
    if (not classtype.empty()) {
        Name *tname = scope.top()->lookupName(classtype);
        if (tname == nullptr) {
            auto decl = scope.top()->getDeclaration(classtype);
            if (decl.type == NONE) {
                error(3126, declaration->type, "type name not found");
            } else {
                error2(3127, declaration->type, decl, "type name is not a type");
            }
        }
        type = dynamic_cast<Type *>(tname);
        if (type == nullptr) {
            error2(3138, declaration->type, tname->declaration, "type name is not a type");
        }
    }
    std::string name = declaration->name.text;
    if (type == nullptr && not scope.top()->allocateName(declaration->name, name)) {
        error2(3047, declaration->name, scope.top()->getDeclaration(name), "duplicate definition of name");
    }
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    std::vector<FunctionParameter *> args;
    for (auto x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameter::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameter::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameter::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type);
        if (type != nullptr && args.empty()) {
            if (not ptype->is_equivalent(type)) {
                error(3128, x->type->token, "expected self parameter");
            }
        }
        FunctionParameter *fp = new FunctionParameter(x->name, x->name.text, ptype, mode);
        args.push_back(fp);
    }
    if (type != nullptr && args.empty()) {
        error(3129, declaration->token, "expected self parameter");
    }
    Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(name);
        if (f != type->methods.end()) {
            function = dynamic_cast<Function *>(f->second);
        } else {
            function = new Function(declaration->name, name, returntype, frame.top(), scope.top(), args);
            type->methods[name] = function;
        }
    } else {
        Name *ident = scope.top()->lookupName(name);
        function = dynamic_cast<Function *>(ident);
        if (function == nullptr) {
            function = new Function(declaration->name, name, returntype, frame.top(), scope.top(), args);
            scope.top()->addName(declaration->name, name, function);
        }
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::FunctionDeclaration *declaration)
{
    const std::string classtype = declaration->type.text;
    Type *type = nullptr;
    if (not classtype.empty()) {
        Name *tname = scope.top()->lookupName(classtype);
        if (tname == nullptr) {
            internal_error("type name not found");
        }
        type = dynamic_cast<Type *>(tname);
        if (type == nullptr) {
            internal_error("type name is not a type");
        }
    }
    Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(declaration->name.text);
        function = dynamic_cast<Function *>(f->second);
    } else {
        function = dynamic_cast<Function *>(scope.top()->lookupName(declaration->name.text));
    }
    scope.push(function->scope);
    functiontypes.push(dynamic_cast<const TypeFunction *>(function->type));
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    function->statements = analyze(declaration->body);
    const Type *returntype = dynamic_cast<const TypeFunction *>(function->type)->returntype;
    if (returntype != TYPE_NOTHING) {
        if (function->statements.empty() || not function->statements.back()->always_returns()) {
            error(3085, declaration->token, "missing RETURN statement");
        }
    }
    loops.pop();
    functiontypes.pop();
    scope.top()->checkForward();
    scope.pop();
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_decl(const pt::ExternalFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3092, declaration->name, scope.top()->getDeclaration(name), "name shadows outer");
    }
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    std::vector<FunctionParameter *> args;
    for (auto x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameter::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameter::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameter::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type);
        FunctionParameter *fp = new FunctionParameter(x->name, x->name.text, ptype, mode);
        args.push_back(fp);
    }
    ExternalFunction *function = new ExternalFunction(declaration->name, name, returntype, frame.top(), scope.top(), args);
    scope.top()->addName(declaration->name, name, function);
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::ExternalFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    ExternalFunction *function = dynamic_cast<ExternalFunction *>(scope.top()->lookupName(name));

    const DictionaryLiteralExpression *dict = dynamic_cast<const DictionaryLiteralExpression *>(analyze(declaration->dict));
    if (not dict->is_constant) {
        error(3071, declaration->dict->token, "constant dictionary expected");
    }
    if (dynamic_cast<const TypeDictionary *>(dict->elementtype) == nullptr) {
        error(3073, declaration->dict->token, "top level dictionary element not dictionary");
    }
    for (auto elem: dict->dict) {
        auto *d = dynamic_cast<const DictionaryLiteralExpression *>(elem.second);
        if (not d->dict.empty() && not d->elementtype->is_equivalent(TYPE_STRING)) {
            error(3074, declaration->dict->token, "sub level dictionary must have string elements");
        }
    }

    auto klibrary = dict->dict.find("library");
    if (klibrary == dict->dict.end()) {
        error(3075, declaration->dict->token, "\"library\" key not found");
    }
    auto &library_dict = dynamic_cast<const DictionaryLiteralExpression *>(klibrary->second)->dict;
    auto kname = library_dict.find("name");
    if (kname == library_dict.end()) {
        error(3076, declaration->dict->token, "\"name\" key not found in library");
    }
    std::string library_name = kname->second->eval_string();

    auto ktypes = dict->dict.find("types");
    if (ktypes == dict->dict.end()) {
        error(3077, declaration->dict->token, "\"types\" key not found");
    }
    auto &types_dict = dynamic_cast<const DictionaryLiteralExpression *>(ktypes->second)->dict;
    std::map<std::string, std::string> param_types;
    for (auto paramtype: types_dict) {
        param_types[paramtype.first] = paramtype.second->eval_string();
    }
    for (auto a: function->params) {
        if (param_types.find(a->name) == param_types.end()) {
            error(3097, declaration->dict->token, "parameter type missing: " + a->name);
        }
    }

    function->library_name = library_name;
    function->param_types = param_types;
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::ExceptionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3115, declaration->token, scope.top()->getDeclaration(name), "duplicate definition of name");
    }
    scope.top()->addName(declaration->name, name, new Exception(declaration->name, name));
    return new NullStatement(declaration->token.line);
}

std::vector<const Statement *> Analyzer::analyze(const std::vector<const pt::Statement *> &statement)
{
    std::vector<const Statement *> statements;
    DeclarationAnalyzer da(this, statements);
    for (auto d: statement) {
        d->accept(&da);
    }
    StatementAnalyzer sa(this, statements);
    for (auto s: statement) {
        s->accept(&sa);
    }
    return statements;
}

const Statement *Analyzer::analyze(const pt::AssignmentStatement *statement)
{
    if (statement->variables.size() != 1) {
        internal_error("unexpected multiple assign statement");
    }
    const Expression *expr = analyze(statement->variables[0]);
    const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
    if (ref == nullptr) {
        error(3058, statement->variables[0]->token, "expression is not assignable");
    }
    if (expr->is_readonly) {
        error(3105, statement->variables[0]->token, "assignment to readonly expression");
    }
    const Expression *rhs = analyze(statement->expr);
    if (not rhs->type->is_equivalent(expr->type)) {
        error(3057, statement->expr->token, "type mismatch");
    }
    std::vector<const ReferenceExpression *> vars;
    vars.push_back(ref);
    return new AssignmentStatement(statement->token.line, vars, rhs);
}

const Statement *Analyzer::analyze(const pt::CaseStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (not expr->type->is_equivalent(TYPE_NUMBER) && not expr->type->is_equivalent(TYPE_STRING) && dynamic_cast<const TypeEnum *>(expr->type) == nullptr) {
        error(3050, statement->expr->token, "CASE expression must be Number, String, or ENUM");
    }
    std::vector<std::pair<std::vector<const CaseStatement::WhenCondition *>, std::vector<const Statement *>>> clauses;
    for (auto x: statement->clauses) {
        std::vector<const CaseStatement::WhenCondition *> conditions;
        for (auto c: x.first) {
            const pt::CaseStatement::ComparisonWhenCondition *cwc = dynamic_cast<const pt::CaseStatement::ComparisonWhenCondition *>(c);
            const pt::CaseStatement::RangeWhenCondition *rwc = dynamic_cast<const pt::CaseStatement::RangeWhenCondition *>(c);
            if (cwc != nullptr) {
                const Expression *when = analyze(cwc->expr);
                if (not when->type->is_equivalent(expr->type)) {
                    error(3051, cwc->expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(3052, cwc->expr->token, "WHEN condition must be constant");
                }
                ComparisonExpression::Comparison comp = (ComparisonExpression::Comparison)cwc->comp; // TODO: remove cast
                const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(cwc->expr->token, comp, when);
                for (auto clause: clauses) {
                    for (auto c: clause.first) {
                        if (cond->overlaps(c)) {
                            error2(3062, cwc->expr->token, c->token, "overlapping case condition");
                        }
                    }
                }
                for (auto c: conditions) {
                    if (cond->overlaps(c)) {
                        error2(3063, cwc->expr->token, c->token, "overlapping case condition");
                    }
                }
                conditions.push_back(cond);
            }
            if (rwc != nullptr) {
                const Expression *when = analyze(rwc->low_expr);
                if (not when->type->is_equivalent(expr->type)) {
                    error(3053, rwc->low_expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(3054, rwc->low_expr->token, "WHEN condition must be constant");
                }
                const Expression *when2 = analyze(rwc->high_expr);
                if (not when2->type->is_equivalent(expr->type)) {
                    error(3055, rwc->high_expr->token, "type mismatch");
                }
                if (not when2->is_constant) {
                    error(3056, rwc->high_expr->token, "WHEN condition must be constant");
                }
                if (when->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(when->type) != nullptr) {
                    if (number_is_greater(when->eval_number(), when2->eval_number())) {
                        error(3109, rwc->high_expr->token, "WHEN numeric range condition must be low..high");
                    }
                } else if (when->type->is_equivalent(TYPE_STRING)) {
                    if (when->eval_string() > when2->eval_string()) {
                        error(3110, rwc->high_expr->token, "WHEN string range condition must be low..high");
                    }
                } else {
                    internal_error("range condition type");
                }
                const CaseStatement::WhenCondition *cond = new CaseStatement::RangeWhenCondition(rwc->low_expr->token, when, when2);
                for (auto clause: clauses) {
                    for (auto c: clause.first) {
                        if (cond->overlaps(c)) {
                            error2(3064, rwc->low_expr->token, c->token, "overlapping case condition");
                        }
                    }
                }
                for (auto c: conditions) {
                    if (cond->overlaps(c)) {
                        error2(3065, rwc->low_expr->token, c->token, "overlapping case condition");
                    }
                }
                conditions.push_back(cond);
            }
        }
        std::vector<const Statement *> statements = analyze(x.second);
        clauses.push_back(std::make_pair(conditions, statements));
    }
    return new CaseStatement(statement->token.line, expr, clauses);
}

namespace overlap {

static bool operator==(const Number &x, const Number &y) { return number_is_equal(x, y); }
static bool operator!=(const Number &x, const Number &y) { return number_is_not_equal(x, y); }
static bool operator<(const Number &x, const Number &y) { return number_is_less(x, y); }
static bool operator>(const Number &x, const Number &y) { return number_is_greater(x, y); }
static bool operator<=(const Number &x, const Number &y) { return number_is_less_equal(x, y); }
static bool operator>=(const Number &x, const Number &y) { return number_is_greater_equal(x, y); }

template <typename T> bool check(ComparisonExpression::Comparison comp1, const T &value1, ComparisonExpression::Comparison comp2, const T &value2)
{
    switch (comp1) {
        case ComparisonExpression::EQ:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 == value2;
                case ComparisonExpression::NE: return value1 != value2;
                case ComparisonExpression::LT: return value1 < value2;
                case ComparisonExpression::GT: return value1 > value2;
                case ComparisonExpression::LE: return value1 <= value2;
                case ComparisonExpression::GE: return value1 >= value2;
            }
            break;
        case ComparisonExpression::NE:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 != value2;
                default: return true;
            }
            break;
        case ComparisonExpression::LT:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 < value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return true;
                case ComparisonExpression::GT: return value1 > value2;
                case ComparisonExpression::LE: return true;
                case ComparisonExpression::GE: return value1 > value2;
            }
            break;
        case ComparisonExpression::GT:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 > value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return value1 < value2;
                case ComparisonExpression::GT: return true;
                case ComparisonExpression::LE: return value1 < value2;
                case ComparisonExpression::GE: return true;
            }
            break;
        case ComparisonExpression::LE:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 >= value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return true;
                case ComparisonExpression::GT: return value1 > value2;
                case ComparisonExpression::LE: return true;
                case ComparisonExpression::GE: return value1 >= value2;
            }
            break;
        case ComparisonExpression::GE:
            switch (comp2) {
                case ComparisonExpression::EQ: return value1 <= value2;
                case ComparisonExpression::NE: return true;
                case ComparisonExpression::LT: return value1 < value2;
                case ComparisonExpression::GT: return true;
                case ComparisonExpression::LE: return value1 <= value2;
                case ComparisonExpression::GE: return true;
            }
            break;
    }
    return false;
}

template <typename T> bool check(ComparisonExpression::Comparison comp1, const T &value1, const T &value2low, const T &value2high)
{
    switch (comp1) {
        case ComparisonExpression::EQ: return value1 >= value2low && value1 <= value2high;
        case ComparisonExpression::NE: return value1 != value2low || value1 != value2high;
        case ComparisonExpression::LT: return value1 > value2low;
        case ComparisonExpression::GT: return value1 > value2high;
        case ComparisonExpression::LE: return value1 >= value2low;
        case ComparisonExpression::GE: return value1 <= value2high;
    }
    return false;
}

template <typename T> bool check(const T &value1low, const T &value1high, const T &value2low, const T &value2high)
{
    return value1high >= value2low && value1low <= value2high;
}

} // namespace overlap

bool CaseStatement::ComparisonWhenCondition::overlaps(const WhenCondition *cond) const
{
    const ComparisonWhenCondition *cwhen = dynamic_cast<const ComparisonWhenCondition *>(cond);
    const RangeWhenCondition *rwhen = dynamic_cast<const RangeWhenCondition *>(cond);
    if (cwhen != nullptr) {
        if (expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(), cwhen->comp, cwhen->expr->eval_number());
        } else if (expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(comp, expr->eval_string(), cwhen->comp, cwhen->expr->eval_string());
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(), rwhen->low_expr->eval_number(), rwhen->high_expr->eval_number());
        } else if (expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(comp, expr->eval_string(), rwhen->low_expr->eval_string(), rwhen->high_expr->eval_string());
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else {
        internal_error("ComparisonWhenCondition");
    }
}

bool CaseStatement::RangeWhenCondition::overlaps(const WhenCondition *cond) const
{
    const ComparisonWhenCondition *cwhen = dynamic_cast<const ComparisonWhenCondition *>(cond);
    const RangeWhenCondition *rwhen = dynamic_cast<const RangeWhenCondition *>(cond);
    if (cwhen != nullptr) {
        if (low_expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_number(), low_expr->eval_number(), high_expr->eval_number());
        } else if (low_expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_string(), low_expr->eval_string(), high_expr->eval_string());
        } else {
            internal_error("RangeWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (low_expr->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(low_expr->eval_number(), high_expr->eval_number(), rwhen->low_expr->eval_number(), rwhen->high_expr->eval_number());
        } else if (low_expr->type->is_equivalent(TYPE_STRING)) {
            return overlap::check(low_expr->eval_string(), high_expr->eval_string(), rwhen->low_expr->eval_string(), rwhen->high_expr->eval_string());
        } else {
            internal_error("RangeWhenCondition");
        }
    } else {
        internal_error("RangeWhenCondition");
    }
}

const Statement *Analyzer::analyze(const pt::ExitStatement *statement)
{
    if (statement->type == FUNCTION) {
        if (functiontypes.empty()) {
            error(3107, statement->token, "EXIT FUNCTION not allowed outside function");
        } else if (functiontypes.top()->returntype != TYPE_NOTHING) {
            error(3108, statement->token, "function must return a value");
        }
        return new ReturnStatement(statement->token.line, nullptr);
    }
    TokenType type = statement->type;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new ExitStatement(statement->token.line, j->second);
            }
        }
    }
    error(3078, statement->token, "no matching loop found in current scope");
}

const Statement *Analyzer::analyze(const pt::ExpressionStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (expr->type == TYPE_NOTHING) {
        return new ExpressionStatement(statement->token.line, analyze(statement->expr));
    }
    if (dynamic_cast<const ComparisonExpression *>(expr) != nullptr) {
        error(3060, statement->expr->token, "':=' expected");
    }
    if (dynamic_cast<const FunctionCall *>(expr) != nullptr) {
        error(3059, statement->token, "return value unused");
    }
    error(3061, statement->token, "Unexpected");
}

const Statement *Analyzer::analyze(const pt::ForStatement *statement)
{
    Name *name = scope.top()->lookupName(statement->var.text);
    Variable *var = dynamic_cast<Variable *>(name);
    if (var == nullptr) {
        error2(3118, statement->var, scope.top()->getDeclaration(statement->var.text), "name not a variable: " + statement->var.text);
    }
    if (not var->type->is_equivalent(TYPE_NUMBER)) {
        error2(3066, statement->var, scope.top()->getDeclaration(statement->var.text), "type mismatch");
    }
    if (var->is_readonly) {
        error2(3125, statement->var, scope.top()->getDeclaration(statement->var.text), "cannot use readonly variable in FOR loop");
    }
    var->is_readonly = true;
    const Expression *start = analyze(statement->start);
    if (not start->type->is_equivalent(TYPE_NUMBER)) {
        error(3067, statement->start->token, "numeric expression expected");
    }
    const Expression *end = analyze(statement->end);
    if (not end->type->is_equivalent(TYPE_NUMBER)) {
        error(3068, statement->end->token, "numeric expression expected");
    }
    const Expression *step = nullptr;
    if (statement->step != nullptr) {
        step = analyze(statement->step);
        if (step->type != TYPE_NUMBER) {
            error(3069, statement->step->token, "numeric expression expected");
        }
        if (not step->is_constant) {
            error(3070, statement->step->token, "numeric expression must be constant");
        }
        if (number_is_zero(step->eval_number())) {
            error(3091, statement->step->token, "STEP value cannot be zero");
        }
    } else {
        step = new ConstantNumberExpression(number_from_uint32(1));
    }
    // TODO: make loop_id a void*
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(FOR, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    loops.top().pop_back();
    var->is_readonly = false;
    return new ForStatement(statement->token.line, loop_id, new VariableExpression(var), start, end, step, statements);
}

const Statement *Analyzer::analyze(const pt::IfStatement *statement)
{
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    for (auto c: statement->condition_statements) {
        const Expression *cond;
        const pt::ValidPointerExpression *valid = dynamic_cast<const pt::ValidPointerExpression *>(c.first);
        if (valid != nullptr) {
            if (scope.top()->lookupName(valid->name.text) != nullptr) {
                error2(3102, valid->name, scope.top()->getDeclaration(valid->name.text), "name shadows outer");
            }
            const Expression *ptr = analyze(valid->ptr);
            const TypePointer *ptrtype = dynamic_cast<const TypePointer *>(ptr->type);
            if (ptrtype == nullptr) {
                error(3101, valid->ptr->token, "pointer type expression expected");
            }
            const TypeValidPointer *vtype = new TypeValidPointer(ptrtype);
            Variable *v;
            // TODO: Try to make this a local variable always (give the global scope a local space).
            if (functiontypes.empty()) {
                v = new GlobalVariable(valid->name, valid->name.text, vtype, true);
            } else {
                v = new LocalVariable(valid->name, valid->name.text, vtype, true);
            }
            scope.top()->addName(valid->name, valid->name.text, v, true);
            cond = new ValidPointerExpression(v, ptr);
        } else {
            cond = analyze(c.first);
            if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
                error(3048, c.first->token, "boolean value expected");
            }
        }
        condition_statements.push_back(std::make_pair(cond, analyze(c.second)));
    }
    std::vector<const Statement *> else_statements = analyze(statement->else_statements);
    return new IfStatement(statement->token.line, condition_statements, else_statements);
}

const Statement *Analyzer::analyze(const pt::LoopStatement *statement)
{
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(LOOP, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    loops.top().pop_back();
    return new LoopStatement(statement->token.line, loop_id, statements);
}

const Statement *Analyzer::analyze(const pt::NextStatement *statement)
{
    TokenType type = statement->type;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new NextStatement(statement->token.line, j->second);
            }
        }
    }
    error(3084, statement->token, "no matching loop found in current scope");
}

const Statement *Analyzer::analyze(const pt::RaiseStatement *statement)
{
    const Name *name = scope.top()->lookupName(statement->name.text);
    if (name == nullptr) {
        error(3089, statement->name, "exception not found: " + statement->name.text);
    }
    const Exception *exception = dynamic_cast<const Exception *>(name);
    if (exception == nullptr) {
        error2(3090, statement->name, name->declaration, "name not an exception");
    }
    const Expression *info = statement->info != nullptr ? analyze(statement->info) : new ConstantStringExpression("");
    return new RaiseStatement(statement->token.line, exception, info);
}

const Statement *Analyzer::analyze(const pt::RepeatStatement *statement)
{
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(REPEAT, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    const Expression *cond = analyze(statement->cond);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(3086, statement->cond->token, "boolean value expected");
    }
    loops.top().pop_back();
    return new RepeatStatement(statement->token.line, loop_id, cond, statements);
}

const Statement *Analyzer::analyze(const pt::ReturnStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (functiontypes.empty()) {
        error(3093, statement->token, "RETURN not allowed outside function");
    } else if (functiontypes.top()->returntype == TYPE_NOTHING) {
        error(3094, statement->token, "function does not return a value");
    } else if (not expr->type->is_equivalent(functiontypes.top()->returntype)) {
        error(3095, statement->expr->token, "type mismatch in RETURN");
    }
    return new ReturnStatement(statement->token.line, expr);
}

const Statement *Analyzer::analyze(const pt::TryStatement *statement)
{
    std::vector<const Statement *> statements = analyze(statement->body);
    std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;
    for (auto x: statement->catches) {
        const Name *name = scope.top()->lookupName(x.first.at(0).text);
        if (name == nullptr) {
            error(3087, x.first.at(0), "exception not found: " + x.first.at(0).text);
        }
        const Exception *exception = dynamic_cast<const Exception *>(name);
        if (exception == nullptr) {
            error2(3088, x.first.at(0), name->declaration, "name not an exception");
        }
        std::vector<const Exception *> exceptions;
        exceptions.push_back(exception);
        std::vector<const Statement *> statements = analyze(x.second);
        catches.push_back(std::make_pair(exceptions, statements));
    }
    return new TryStatement(statement->token.line, statements, catches);
}

const Statement *Analyzer::analyze(const pt::WhileStatement *statement)
{
    const Expression *cond = analyze(statement->cond);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(3049, statement->cond->token, "boolean value expected");
    }
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(WHILE, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    loops.top().pop_back();
    return new WhileStatement(statement->token.line, loop_id, cond, statements);
}

const Program *Analyzer::analyze()
{
    Program *r = new Program();
    global_frame = r->frame;
    global_scope = r->scope;
    frame.push(global_frame);
    scope.push(global_scope);
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    r->statements = analyze(program->body);
    loops.pop();
    r->scope->checkForward();
    scope.pop();
    return r;
}

const Program *analyze(const pt::Program *program)
{
    return Analyzer(program).analyze();
}
