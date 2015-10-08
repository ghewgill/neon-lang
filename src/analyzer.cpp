#include "analyzer.h"

#include <algorithm>
#include <fstream>
#include <iso646.h>
#include <list>
#include <sstream>
#include <stack>

#include "ast.h"
#include "bytecode.h"
#include "format.h"
#include "pt.h"
#include "rtl_compile.h"
#include "support.h"
#include "util.h"

#include "constants_compile.inc"

class Analyzer {
public:
    Analyzer(ICompilerSupport *support, const pt::Program *program);

    ICompilerSupport *support;
    const pt::Program *program;
    std::map<std::string, Module *> modules;
    Frame *global_frame;
    Scope *global_scope;
    std::stack<Frame *> frame;
    std::stack<Scope *> scope;
    std::set<std::string> exports;

    std::stack<std::pair<const Type *, const TypeFunction *>> functiontypes;
    std::stack<std::list<std::pair<TokenType, unsigned int>>> loops;

    const Type *analyze(const pt::Type *type, const std::string &name = std::string());
    const Type *analyze(const pt::TypeSimple *type, const std::string &name);
    const Type *analyze_enum(const pt::TypeEnum *type, const std::string &name);
    const Type *analyze_record(const pt::TypeRecord *type, const std::string &name);
    const Type *analyze(const pt::TypePointer *type, const std::string &name);
    const Type *analyze(const pt::TypeFunctionPointer *type, const std::string &name);
    const Type *analyze(const pt::TypeParameterised *type, const std::string &name);
    const Type *analyze(const pt::TypeImport *type, const std::string &name);
    const Expression *analyze(const pt::Expression *expr);
    const Expression *analyze(const pt::IdentityExpression *expr);
    const Expression *analyze(const pt::BooleanLiteralExpression *expr);
    const Expression *analyze(const pt::NumberLiteralExpression *expr);
    const Expression *analyze(const pt::StringLiteralExpression *expr);
    const Expression *analyze(const pt::FileLiteralExpression *expr);
    const Expression *analyze(const pt::ArrayLiteralExpression *expr);
    const Expression *analyze(const pt::DictionaryLiteralExpression *expr);
    const Expression *analyze(const pt::NilLiteralExpression *expr);
    const Expression *analyze(const pt::IdentifierExpression *expr);
    const Name *analyze_qualified_name(const pt::Expression *expr);
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
    const Statement *analyze(const pt::NativeFunctionDeclaration *declaration);
    const Statement *analyze(const pt::ExceptionDeclaration *declaration);
    const Statement *analyze(const pt::ExportDeclaration *declaration);
    std::vector<const Statement *> analyze(const std::vector<const pt::Statement *> &statement);
    const Statement *analyze(const pt::AssertStatement *statement);
    const Statement *analyze(const pt::AssignmentStatement *statement);
    const Statement *analyze(const pt::CaseStatement *statement);
    const Statement *analyze(const pt::ExitStatement *statement);
    const Statement *analyze(const pt::ExpressionStatement *statement);
    const Statement *analyze(const pt::ForStatement *statement);
    const Statement *analyze(const pt::ForeachStatement *statement);
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
    Module *import_module(const Token &token, const std::string &name);
    Type *deserialize_type(Scope *scope, const std::string &descriptor, std::string::size_type &i);
    Type *deserialize_type(Scope *scope, const std::string &descriptor);
private:
    Analyzer(const Analyzer &);
    Analyzer &operator=(const Analyzer &);
};

class TypeAnalyzer: public pt::IParseTreeVisitor {
public:
    TypeAnalyzer(Analyzer *a, const std::string &name): type(nullptr), a(a), name(name) {}
    virtual void visit(const pt::TypeSimple *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeEnum *t) override { type = a->analyze_enum(t, name); }
    virtual void visit(const pt::TypeRecord *t) override { type = a->analyze_record(t, name); }
    virtual void visit(const pt::TypePointer *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeFunctionPointer *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeParameterised *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeImport *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::IdentityExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BooleanLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FileLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentifierExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DotExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrowExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::InterpolatedStringExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FunctionCallExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryPlusExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryMinusExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::LogicalNotExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ExponentiationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MultiplicationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DivisionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ModuloExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::AdditionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubtractionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConcatenationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ComparisonExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ChainedComparisonExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewRecordExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::AssertStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::AssignmentStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::CaseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExitStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExpressionStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForeachStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::IfStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::LoopStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::NextStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RaiseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RepeatStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ReturnStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::WhileStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
    const Type *type;
private:
    Analyzer *a;
    const std::string name;
private:
    TypeAnalyzer(const TypeAnalyzer &);
    TypeAnalyzer &operator=(const TypeAnalyzer &);
};

class ExpressionAnalyzer: public pt::IParseTreeVisitor {
public:
    ExpressionAnalyzer(Analyzer *a): expr(nullptr), a(a) {}
    virtual void visit(const pt::TypeSimple *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeEnum *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeRecord *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeFunctionPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeImport *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::IdentityExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::BooleanLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NumberLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::StringLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::FileLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ArrayLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::DictionaryLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NilLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::IdentifierExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::DotExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ArrowExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::SubscriptExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::InterpolatedStringExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::FunctionCallExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::UnaryPlusExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::UnaryMinusExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::LogicalNotExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ExponentiationExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::MultiplicationExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::DivisionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ModuloExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::AdditionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::SubtractionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ConcatenationExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ComparisonExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ChainedComparisonExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::MembershipExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ConjunctionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::DisjunctionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ConditionalExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NewRecordExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ValidPointerExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::RangeSubscriptExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ImportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::AssertStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::AssignmentStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::CaseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExitStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExpressionStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForeachStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::IfStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::LoopStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::NextStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RaiseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RepeatStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ReturnStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::WhileStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
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
    virtual void visit(const pt::TypeSimple *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeEnum *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeRecord *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeFunctionPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeImport *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::IdentityExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BooleanLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FileLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentifierExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DotExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrowExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::InterpolatedStringExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FunctionCallExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryPlusExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryMinusExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::LogicalNotExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ExponentiationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MultiplicationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DivisionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ModuloExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::AdditionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubtractionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConcatenationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ComparisonExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ChainedComparisonExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewRecordExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TypeDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ConstantDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::VariableDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::LetDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeFunctionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExceptionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExportDeclaration *) override {}
    virtual void visit(const pt::AssertStatement *) override {}
    virtual void visit(const pt::AssignmentStatement *) override {}
    virtual void visit(const pt::CaseStatement *) override {}
    virtual void visit(const pt::ExitStatement *) override {}
    virtual void visit(const pt::ExpressionStatement *) override {}
    virtual void visit(const pt::ForStatement *) override {}
    virtual void visit(const pt::ForeachStatement *) override {}
    virtual void visit(const pt::IfStatement *) override {}
    virtual void visit(const pt::LoopStatement *) override {}
    virtual void visit(const pt::NextStatement *) override {}
    virtual void visit(const pt::RaiseStatement *) override {}
    virtual void visit(const pt::RepeatStatement *) override {}
    virtual void visit(const pt::ReturnStatement *) override {}
    virtual void visit(const pt::TryStatement *) override {}
    virtual void visit(const pt::WhileStatement *) override {}
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
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
    virtual void visit(const pt::TypeSimple *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeEnum *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeRecord *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeFunctionPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeImport *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::IdentityExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BooleanLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FileLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentifierExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DotExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrowExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::InterpolatedStringExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FunctionCallExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryPlusExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::UnaryMinusExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::LogicalNotExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ExponentiationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MultiplicationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DivisionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ModuloExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::AdditionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::SubtractionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConcatenationExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ComparisonExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ChainedComparisonExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewRecordExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) override {}
    virtual void visit(const pt::TypeDeclaration *) override {}
    virtual void visit(const pt::ConstantDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::VariableDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::LetDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override {}
    virtual void visit(const pt::ExceptionDeclaration *) override {}
    virtual void visit(const pt::ExportDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::AssertStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::AssignmentStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::CaseStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExitStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExpressionStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ForStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ForeachStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::IfStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::LoopStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::NextStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::RaiseStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::RepeatStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ReturnStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TryStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::WhileStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
private:
    Analyzer *a;
    std::vector<const Statement *> &v;
private:
    StatementAnalyzer(const StatementAnalyzer &);
    StatementAnalyzer &operator=(const StatementAnalyzer &);
};

static std::string path_basename(const std::string &path)
{
#ifdef _WIN32
    static const std::string delim = "/\\:";
#else
    static const std::string delim = "/";
#endif
    std::string::size_type i = path.find_last_of(delim);
    if (i == std::string::npos) {
        return path;
    }
    return path.substr(i + 1);
}

static std::string path_stripext(const std::string &name)
{
    std::string::size_type i = name.find_last_of('.');
    if (i == std::string::npos) {
        return name;
    }
    return name.substr(0, i);
}

Analyzer::Analyzer(ICompilerSupport *support, const pt::Program *program)
  : support(support),
    program(program),
    modules(),
    global_frame(nullptr),
    global_scope(nullptr),
    frame(),
    scope(),
    exports(),
    functiontypes(),
    loops()
{
}

TypeEnum::TypeEnum(const Token &declaration, const std::string &name, const std::map<std::string, int> &names, Analyzer *analyzer)
  : TypeNumber(declaration, name),
    names(names)
{
    {
        std::vector<FunctionParameter *> params;
        FunctionParameter *fp = new FunctionParameter(Token(), "self", this, ParameterType::IN, nullptr);
        params.push_back(fp);
        Function *f = new Function(Token(), "enum.to_string", TYPE_STRING, analyzer->global_frame, analyzer->global_scope, params);
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

StringReferenceIndexExpression::StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("string__substring"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("string__splice"))), args);
    }
}

StringValueIndexExpression::StringValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("string__substring"))), args);
    }
}

ArrayReferenceRangeExpression::ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("array__slice"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("array__splice"))), args);
    }
}

ArrayValueRangeExpression::ArrayValueRangeExpression(const Expression *array, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("array__slice"))), args);
    }
}

Module *Analyzer::import_module(const Token &token, const std::string &name)
{
    static std::vector<std::string> s_importing;

    auto m = modules.find(name);
    if (m != modules.end()) {
        return m->second;
    }
    if (std::find(s_importing.begin(), s_importing.end(), name) != s_importing.end()) {
        error(3181, token, "recursive import detected");
    }
    s_importing.push_back(name);
    Bytecode object;
    if (not support->loadBytecode(name, object)) {
        internal_error("TODO module not found: " + name);
    }
    Module *module = new Module(Token(), scope.top(), name);
    for (auto t: object.types) {
        if (object.strtable[t.descriptor][0] == 'R') {
            // Support recursive record type declarations.
            TypeRecord *actual_record = new TypeRecord(Token(), name + "." + object.strtable[t.name], std::vector<TypeRecord::Field>());
            module->scope->addName(Token(), object.strtable[t.name], actual_record);
            Type *type = deserialize_type(module->scope, object.strtable[t.descriptor]);
            const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
            const_cast<std::vector<TypeRecord::Field> &>(actual_record->fields) = rectype->fields;
            const_cast<std::map<std::string, size_t> &>(actual_record->field_names) = rectype->field_names;
        } else {
            module->scope->addName(Token(), object.strtable[t.name], deserialize_type(module->scope, object.strtable[t.descriptor]));
        }
    }
    for (auto c: object.constants) {
        const Type *type = deserialize_type(module->scope, object.strtable[c.type]);
        int i = 0;
        module->scope->addName(Token(), object.strtable[c.name], new Constant(Token(), object.strtable[c.name], type->deserialize_value(c.value, i)));
    }
    for (auto v: object.variables) {
        module->scope->addName(Token(), object.strtable[v.name], new ModuleVariable(name, object.strtable[v.name], deserialize_type(module->scope, object.strtable[v.type]), v.index));
    }
    for (auto f: object.functions) {
        const std::string function_name = object.strtable[f.name];
        auto i = function_name.find('.');
        if (i != std::string::npos) {
            const std::string typestr = function_name.substr(0, i);
            const std::string method = function_name.substr(i+1);
            Name *n = module->scope->lookupName(typestr);
            Type *type = dynamic_cast<Type *>(n);
            type->methods[method] = new ModuleFunction(name, function_name, deserialize_type(module->scope, object.strtable[f.descriptor]), f.entry);
        } else {
            module->scope->addName(Token(), function_name, new ModuleFunction(name, function_name, deserialize_type(module->scope, object.strtable[f.descriptor]), f.entry));
        }
    }
    for (auto e: object.exception_exports) {
        module->scope->addName(Token(), object.strtable[e.name], new Exception(Token(), object.strtable[e.name]));
    }
    s_importing.pop_back();
    modules[name] = module;
    return module;
}

const Type *Analyzer::analyze(const pt::Type *type, const std::string &name)
{
    TypeAnalyzer ta(this, name);
    type->accept(&ta);
    return ta.type;
}

const Type *Analyzer::analyze(const pt::TypeSimple *type, const std::string &)
{
    const Name *name = scope.top()->lookupName(type->name);
    if (name == nullptr) {
        error(3011, type->token, "unknown type name");
    }
    const Type *r = dynamic_cast<const Type *>(name);
    if (r == nullptr) {
        error2(3012, type->token, "name is not a type", name->declaration, "name declared here");
    }
    return r;
}

const Type *Analyzer::analyze_enum(const pt::TypeEnum *type, const std::string &name)
{
    std::map<std::string, int> names;
    int index = 0;
    for (auto x: type->names) {
        std::string name = x.first.text;
        auto t = names.find(name);
        if (t != names.end()) {
            error2(3010, x.first, "duplicate enum: " + name, type->names[t->second].first, "first declaration here");
        }
        names[name] = index;
        index++;
    }
    return new TypeEnum(type->token, name, names, this);
}

const Type *Analyzer::analyze_record(const pt::TypeRecord *type, const std::string &name)
{
    std::vector<TypeRecord::Field> fields;
    std::map<std::string, Token> field_names;
    for (auto x: type->fields) {
        std::string name = x.name.text;
        auto prev = field_names.find(name);
        if (prev != field_names.end()) {
            error2(3009, x.name, "duplicate field: " + x.name.text, prev->second, "first declaration here");
        }
        const Type *t = analyze(x.type);
        fields.push_back(TypeRecord::Field(x.name, t, x.is_private));
        field_names[name] = x.name;
    }
    return new TypeRecord(type->token, name, fields);
}

const Type *Analyzer::analyze(const pt::TypePointer *type, const std::string &)
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

const Type *Analyzer::analyze(const pt::TypeFunctionPointer *type, const std::string &)
{
    const Type *returntype = type->returntype != nullptr ? analyze(type->returntype) : TYPE_NOTHING;
    std::vector<const ParameterType *> params;
    bool in_default = false;
    for (auto x: type->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameter::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameter::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameter::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type);
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            in_default = true;
            def = analyze(x->default_value);
            if (not def->is_constant) {
                error(3177, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3178, x->token, "default value must be specified for this parameter");
        }
        ParameterType *pt = new ParameterType(x->name, mode, ptype, def);
        params.push_back(pt);
    }
    return new TypeFunctionPointer(type->token, new TypeFunction(returntype, params));
}

const Type *Analyzer::analyze(const pt::TypeParameterised *type, const std::string &)
{
    if (type->name.text == "Array") {
        return new TypeArray(type->name, analyze(type->elementtype));
    }
    if (type->name.text == "Dictionary") {
        return new TypeDictionary(type->name, analyze(type->elementtype));
    }
    internal_error("Invalid parameterized type");
}

const Type *Analyzer::analyze(const pt::TypeImport *type, const std::string &)
{
    Name *modname = scope.top()->lookupName(type->modname.text);
    if (modname == nullptr) {
        error(3153, type->modname, "name not found");
    }
    Module *module = dynamic_cast<Module *>(modname);
    if (module == nullptr) {
        error(3154, type->modname, "module name expected");
    }
    Name *name = module->scope->lookupName(type->subname.text);
    if (name == nullptr) {
        error(3155, type->subname, "name not found in module");
    }
    Type *rtype = dynamic_cast<Type *>(name);
    if (rtype == nullptr) {
        error(3156, type->subname, "name not a type");
    }
    return rtype;
}

const Expression *Analyzer::analyze(const pt::Expression *expr)
{
    ExpressionAnalyzer ea(this);
    expr->accept(&ea);
    return ea.expr;
}

const Expression *Analyzer::analyze(const pt::IdentityExpression *expr)
{
    return analyze(expr->expr);
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

const Expression *Analyzer::analyze(const pt::FileLiteralExpression *expr)
{
    std::ifstream f(expr->name, std::ios::binary);
    if (not f) {
        error(3182, expr->token, "could not open file");
    }
    std::stringstream buffer;
    buffer << f.rdbuf();
    return new ConstantFileExpression(expr->name, buffer.str());
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
            error2(3080, x.first, "duplicate key", i->second, "first key here");
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

const Name *Analyzer::analyze_qualified_name(const pt::Expression *expr)
{
    const pt::IdentifierExpression *ident = dynamic_cast<const pt::IdentifierExpression *>(expr);
    if (ident != nullptr) {
        return scope.top()->lookupName(ident->name);
    }
    const pt::DotExpression *dotted = dynamic_cast<const pt::DotExpression *>(expr);
    if (dotted == nullptr) {
        return nullptr;
    }
    const Name *base = analyze_qualified_name(dotted->base);
    const Module *module = dynamic_cast<const Module *>(base);
    if (module != nullptr) {
        const Name *name = module->scope->lookupName(dotted->name.text);
        if (name == nullptr) {
            error(3134, dotted->name, "name not found: " + dotted->name.text);
        }
        return name;
    }
    return nullptr;
}

const Expression *Analyzer::analyze(const pt::DotExpression *expr)
{
    const Name *name = analyze_qualified_name(expr);
    if (name != nullptr) {
        const Constant *constant = dynamic_cast<const Constant *>(name);
        if (constant != nullptr) {
            return new ConstantExpression(constant);
        }
        const Variable *var = dynamic_cast<const Variable *>(name);
        if (var != nullptr) {
            return new VariableExpression(var);
        }
    }
    name = analyze_qualified_name(expr->base);
    if (name != nullptr) {
        const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(name);
        if (enumtype != nullptr) {
            auto name = enumtype->names.find(expr->name.text);
            if (name == enumtype->names.end()) {
                error2(3023, expr->name, "identifier not member of enum: " + expr->name.text, enumtype->declaration, "enum declared here");
            }
            return new ConstantEnumExpression(enumtype, name->second);
        }
    }
    const Expression *base = analyze(expr->base);
    const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(base->type);
    if (recordtype == nullptr) {
        error(3046, expr->base->token, "not a record");
    }
    if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
        internal_error("record not defined yet");
    }
    auto f = recordtype->field_names.find(expr->name.text);
    if (f == recordtype->field_names.end()) {
        error2(3045, expr->name, "field not found", recordtype->declaration, "record declared here");
    }
    if (recordtype->fields[f->second].is_private && (functiontypes.empty() || functiontypes.top().first != recordtype)) {
        error(3162, expr->name, "field is private");
    }
    const Type *type = recordtype->fields[f->second].type;
    const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
    if (ref != nullptr) {
        return new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
    } else {
        return new ArrayValueIndexExpression(type, base, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
    }
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
        error2(3104, expr->base->token, "record not defined yet", recordtype->declaration, "forward declaration here");
    }
    auto f = recordtype->field_names.find(expr->name.text);
    if (f == recordtype->field_names.end()) {
        error2(3111, expr->name, "field not found", recordtype->declaration, "record declared here");
    }
    if (recordtype->fields[f->second].is_private && (functiontypes.empty() || functiontypes.top().first != recordtype)) {
        error(3163, expr->name, "field is private");
    }
    const Type *type = recordtype->fields[f->second].type;
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
            error2(3041, expr->index->token, "index must be a number", arraytype->declaration, "array declared here");
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
            error2(3042, expr->index->token, "index must be a string", dicttype->declaration, "dictionary declared here");
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
            return new StringReferenceIndexExpression(ref, index, false, index, false, this);
        } else {
            return new StringValueIndexExpression(base, index, false, index, false, this);
        }
    } else {
        error2(3044, expr->token, "not an array or dictionary", type->declaration, "declaration here");
    }
}

const Expression *Analyzer::analyze(const pt::InterpolatedStringExpression *expr)
{
    const VariableExpression *concat = new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("concat")));
    const VariableExpression *format = new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("format")));
    const Expression *r = nullptr;
    for (auto x: expr->parts) {
        const Expression *e = analyze(x.first);
        std::string fmt = x.second.text;
        if (not fmt.empty()) {
            format::Spec spec;
            if (not format::parse(fmt, spec)) {
                error(3133, x.second, "invalid format specification");
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
                error(3136, expr->rparen, "two arguments expected");
            }
            const Expression *lhs_expr = analyze(expr->args[0].second);
            const ReferenceExpression *lhs = dynamic_cast<const ReferenceExpression *>(lhs_expr);
            if (lhs == nullptr) {
                error(3119, expr->args[0].second->token, "expression is not assignable");
            }
            if (lhs_expr->is_readonly && dynamic_cast<const TypePointer *>(lhs_expr->type) == nullptr) {
                error(3120, expr->args[0].second->token, "value_copy to readonly expression");
            }
            const Expression *rhs = analyze(expr->args[1].second);
            const Type *ltype = lhs->type;
            const TypePointer *lptype = dynamic_cast<const TypePointer *>(ltype);
            if (lptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(lptype) == nullptr) {
                    error(3121, expr->args[0].second->token, "valid pointer type required");
                }
                ltype = lptype->reftype;
                lhs = new PointerDereferenceExpression(ltype, lhs);
            }
            const Type *rtype = rhs->type;
            const TypePointer *rptype = dynamic_cast<const TypePointer *>(rtype);
            if (rptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(rptype) == nullptr) {
                    error(3122, expr->args[1].second->token, "valid pointer type required");
                }
                rtype = rptype->reftype;
                rhs = new PointerDereferenceExpression(rtype, rhs);
            }
            if (not ltype->is_equivalent(rtype)) {
                error(3123, expr->args[1].second->token, "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(lhs);
            return new StatementExpression(new AssignmentStatement(expr->token.line, vars, rhs));
        }
        const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(scope.top()->lookupName(fname->name));
        if (recordtype != nullptr) {
            if (expr->args.size() > recordtype->fields.size()) {
                error2(3130, expr->args[recordtype->fields.size()].second->token, "wrong number of fields", recordtype->declaration, "record declared here");
            }
            std::vector<const Expression *> elements;
            auto f = recordtype->fields.begin();
            for (auto x: expr->args) {
                const Expression *element = analyze(x.second);
                if (not element->type->is_equivalent(f->type)) {
                    error2(3131, x.second->token, "type mismatch", f->name, "field declared here");
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
        const TypeFunctionPointer *pf = dynamic_cast<const TypeFunctionPointer *>(func->type);
        if (pf == nullptr) {
            error(3017, expr->base->token, "not a function");
        }
        ftype = pf->functype;
    }
    int param_index = 0;
    std::vector<const Expression *> args(ftype->params.size());
    if (self != nullptr) {
        args[0] = self;
        ++param_index;
    }
    for (auto a: expr->args) {
        const Expression *e = analyze(a.second);
        if (param_index >= static_cast<int>(ftype->params.size())) {
            error(3096, a.second->token, "too many parameters");
        }
        int p;
        if (param_index >= 0 && a.first.text.empty()) {
            p = param_index;
            param_index++;
        } else {
            // Now in named argument mode.
            param_index = -1;
            if (a.first.text.empty()) {
                error(3145, a.second->token, "parameter name must be specified");
            }
            auto fp = ftype->params.begin();
            for (;;) {
                if (a.first.text == (*fp)->declaration.text) {
                    break;
                }
                ++fp;
                if (fp == ftype->params.end()) {
                    error(3146, a.first, "parameter name not found");
                }
            }
            p = static_cast<int>(std::distance(ftype->params.begin(), fp));
            if (args[p] != nullptr) {
                error(3147, a.first, "parameter already specified");
            }
        }
        if (ftype->params[p]->mode != ParameterType::IN) {
            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
            if (ref == nullptr) {
                error2(3018, a.second->token, "function call argument must be reference", ftype->params[p]->declaration, "function argument here");
            }
            if (ref->is_readonly) {
                error(3106, a.second->token, "readonly parameter to OUT");
            }
        }
        if (not ftype->params[p]->type->is_equivalent(e->type)) {
            error2(3019, a.second->token, "type mismatch", ftype->params[p]->declaration, "function argument here");
        }
        args[p] = e;
    }
    int p = 0;
    for (auto a: args) {
        if (a == nullptr) {
            if (ftype->params[p]->default_value != nullptr) {
                args[p] = ftype->params[p]->default_value;
            } else {
                error(3020, expr->rparen, "argument not specified for: " + ftype->params[p]->declaration.text);
            }
        }
        p++;
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
    ComparisonExpression::Comparison comp = static_cast<ComparisonExpression::Comparison>(expr->comp); // TODO: remove cast
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
    } else if (dynamic_cast<const TypeFunctionPointer *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3180, expr->token, "comparison not available for POINTER");
        }
        return new FunctionPointerComparisonExpression(left, right, comp);
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
    // This is a pretty long-winded way of identifying common tautological
    // expressions of the form:
    //  a # "x" OR a # "y"
    // The following only works for expressions like the above where:
    //  - the identifier ("a") is a simple identifier
    //  - both # expressions are in the above order
    //  - it's actually an OR expression
    // A more general purpose and cleaner implementation would be welcome.
    const pt::ComparisonExpression *lne = dynamic_cast<const pt::ComparisonExpression *>(expr->left);
    const pt::ComparisonExpression *rne = dynamic_cast<const pt::ComparisonExpression *>(expr->right);
    if (lne != nullptr && rne != nullptr && lne->comp == pt::ComparisonExpression::NE && rne->comp == pt::ComparisonExpression::NE) {
        const pt::IdentifierExpression *lid = dynamic_cast<const pt::IdentifierExpression *>(lne->left);
        const pt::IdentifierExpression *rid = dynamic_cast<const pt::IdentifierExpression *>(rne->left);
        if (lid != nullptr && rid != nullptr && lid->name == rid->name) {
            const pt::BooleanLiteralExpression *lble = dynamic_cast<const pt::BooleanLiteralExpression *>(lne->right);
            const pt::BooleanLiteralExpression *rble = dynamic_cast<const pt::BooleanLiteralExpression *>(rne->right);
            const pt::NumberLiteralExpression *lnle = dynamic_cast<const pt::NumberLiteralExpression *>(lne->right);
            const pt::NumberLiteralExpression *rnle = dynamic_cast<const pt::NumberLiteralExpression *>(rne->right);
            const pt::StringLiteralExpression *lsle = dynamic_cast<const pt::StringLiteralExpression *>(lne->right);
            const pt::StringLiteralExpression *rsle = dynamic_cast<const pt::StringLiteralExpression *>(rne->right);
            if ((lble != nullptr && rble != nullptr && lble->value != rble->value)
             || (lnle != nullptr && rnle != nullptr && number_is_not_equal(lnle->value, rnle->value))
             || (lsle != nullptr && rsle != nullptr && lsle->value != rsle->value)) {
                error(3172, expr->token, "boolean expression is always false");
            }
        }
    }

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
    // This should never happen because ValidPointerExpression is handled elsewhere.
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
            return new ArrayReferenceRangeExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        } else {
            return new ArrayValueRangeExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        }
    } else if (type == TYPE_STRING) {
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new StringReferenceIndexExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        } else {
            return new StringValueIndexExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        }
    } else {
        error2(3143, expr->base->token, "not an array or string", type->declaration, "declaration here");
    }
}

Type *Analyzer::deserialize_type(Scope *scope, const std::string &descriptor, std::string::size_type &i)
{
    switch (descriptor.at(i)) {
        case 'Z': i++; return TYPE_NOTHING;
        case 'B': i++; return TYPE_BOOLEAN;
        case 'N': i++; return TYPE_NUMBER;
        case 'S': i++; return TYPE_STRING;
        case 'Y': i++; return TYPE_BYTES;
        case 'A': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            const Type *type = deserialize_type(scope, descriptor, i);
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new TypeArray(Token(), type);
        }
        case 'D': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            const Type *type = deserialize_type(scope, descriptor, i);
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new TypeDictionary(Token(), type);
        }
        case 'R': {
            i++;
            std::vector<TypeRecord::Field> fields;
            if (descriptor.at(i) != '[') {
                internal_error("deserialize_type");
            }
            i++;
            while (descriptor.at(i) != ']') {
                bool is_private = false;
                if (descriptor.at(i) == '!') {
                    is_private = true;
                    i++;
                }
                std::string name;
                while (descriptor.at(i) != ':') {
                    name.push_back(descriptor.at(i));
                    i++;
                }
                i++;
                const Type *type = deserialize_type(scope, descriptor, i);
                Token token;
                token.text = name;
                fields.push_back(TypeRecord::Field(token, type, is_private));
                if (descriptor.at(i) == ',') {
                    i++;
                }
            }
            i++;
            return new TypeRecord(Token(), "record", fields);
        }
        case 'E': {
            i++;
            std::map<std::string, int> names;
            if (descriptor.at(i) != '[') {
                internal_error("deserialize_type");
            }
            i++;
            int value = 0;
            for (;;) {
                std::string name;
                while (descriptor.at(i) != ',' && descriptor.at(i) != ']') {
                    name.push_back(descriptor.at(i));
                    i++;
                }
                names[name] = value;
                value++;
                if (descriptor.at(i) == ']') {
                    break;
                }
                i++;
            }
            i++;
            return new TypeEnum(Token(), "enum", names, this);
        }
        case 'F': {
            i++;
            std::vector<const ParameterType *> params;
            if (descriptor.at(i) != '[') {
                internal_error("deserialize_type");
            }
            i++;
            while (descriptor.at(i) != ']') {
                ParameterType::Mode mode = ParameterType::IN;
                switch (descriptor.at(i)) {
                    case '>': mode = ParameterType::IN;    break;
                    case '*': mode = ParameterType::INOUT; break;
                    case '<': mode = ParameterType::OUT;   break;
                    default:
                        internal_error("unexpected mode indicator");
                }
                i++;
                std::string name;
                while (descriptor.at(i) != ':') {
                    name.push_back(descriptor.at(i));
                    i++;
                }
                i++;
                const Type *type = deserialize_type(scope, descriptor, i);
                Token token;
                token.text = name;
                // TODO: default value
                params.push_back(new ParameterType(token, mode, type, nullptr));
                if (descriptor.at(i) == ',') {
                    i++;
                }
            }
            i++;
            if (descriptor.at(i) != ':') {
                internal_error("deserialize_type");
            }
            i++;
            const Type *returntype = deserialize_type(scope, descriptor, i);
            return new TypeFunction(returntype, params);
        }
        case 'P': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            const TypeRecord *rectype = nullptr;
            if (descriptor.at(i) != '>') {
                const Type *type = deserialize_type(scope, descriptor, i);
                rectype = dynamic_cast<const TypeRecord *>(type);
                if (rectype == nullptr) {
                    internal_error("deserialize_type");
                }
            }
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new TypePointer(Token(), rectype);
        }
        case 'Q': {
            i++;
            TypeFunction *f = dynamic_cast<TypeFunction *>(deserialize_type(scope, descriptor, i));
            return new TypeFunctionPointer(Token(), f);
        }
        case '~': {
            i++;
            std::string name;
            while (descriptor.at(i) != ';') {
                name.push_back(descriptor.at(i));
                i++;
            }
            i++;
            Scope *s = scope;
            std::string localname = name;
            auto dot = name.find('.');
            if (dot != std::string::npos) {
                const Module *module = import_module(Token(), name.substr(0, dot));
                s = module->scope;
                localname = name.substr(dot+1);
            }
            Type *type = dynamic_cast<Type *>(s->lookupName(localname));
            if (type == nullptr) {
                internal_error("reference to unknown type in exports: " + name);
            }
            return type;
        }
        default:
            internal_error("TODO unimplemented type in deserialize_type: " + descriptor);
    }
}

Type *Analyzer::deserialize_type(Scope *scope, const std::string &descriptor)
{
    std::string::size_type i = 0;
    Type *r = deserialize_type(scope, descriptor, i);
    if (i != descriptor.length()) {
        internal_error("deserialize_type: " + descriptor + " " + std::to_string(i));
    }
    return r;
}

const Statement *Analyzer::analyze(const pt::ImportDeclaration *declaration)
{
    const Token &localname = declaration->alias.type != NONE ? declaration->alias : declaration->name.type != NONE ? declaration->name : declaration->module;
    if (not scope.top()->allocateName(localname, localname.text)) {
        error2(3114, localname, "duplicate definition of name", scope.top()->getDeclaration(localname.text), "first declaration here");
    }
    Module *module = import_module(declaration->module, declaration->module.text);
    rtl_import(declaration->module.text, module);
    if (declaration->name.type == NONE) {
        scope.top()->addName(declaration->token, localname.text, module);
    } else {
        const Name *name = module->scope->lookupName(declaration->name.text);
        if (name != nullptr) {
            scope.top()->addName(declaration->token, localname.text, module->scope->lookupName(declaration->name.text));
        } else {
            error(3176, declaration->name, "name not found in module");
        }
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::TypeDeclaration *declaration)
{
    std::string name = declaration->token.text;
    if (not scope.top()->allocateName(declaration->token, name)) {
        error2(3013, declaration->token, "duplicate identifier", scope.top()->getDeclaration(name), "first declaration here");
    }
    TypeRecord *actual_record = nullptr;
    const pt::TypeRecord *recdecl = dynamic_cast<const pt::TypeRecord *>(declaration->type);
    if (recdecl != nullptr) {
        // Support recursive record type declarations.
        actual_record = new TypeRecord(recdecl->token, name, std::vector<TypeRecord::Field>());
        scope.top()->addName(declaration->token, name, actual_record);
    }
    const Type *type = analyze(declaration->type, name);
    if (actual_record != nullptr) {
        const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
        const_cast<std::vector<TypeRecord::Field> &>(actual_record->fields) = rectype->fields;
        const_cast<std::map<std::string, size_t> &>(actual_record->field_names) = rectype->field_names;
        type = actual_record;
    } else {
        Type *t = const_cast<Type *>(type);
        if (type != TYPE_BOOLEAN && type != TYPE_NUMBER && type != TYPE_STRING && type != TYPE_BYTES) {
            const_cast<std::string &>(t->name) = name;
        }
        scope.top()->addName(declaration->token, name, t); // Still ugly.
    }
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
        error2(3014, declaration->token, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
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
            error2(3038, name, "duplicate identifier", scope.top()->getDeclaration(name.text), "first declaration here");
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
        if (not type->is_equivalent(expr->type)) {
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
        error2(3139, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::LetDeclaration *declaration)
{
    const Type *type = analyze(declaration->type);
    const Expression *expr = analyze(declaration->value);
    if (not type->is_equivalent(expr->type)) {
        error(3140, declaration->value->token, "type mismatch");
    }
    const TypePointer *ptype = dynamic_cast<const TypePointer *>(type);
    if (ptype != nullptr && dynamic_cast<const NewRecordExpression *>(expr) != nullptr) {
        type = new TypeValidPointer(ptype);
    }
    Variable *v;
    if (frame.top() == global_frame) {
        v = new GlobalVariable(declaration->name, declaration->name.text, type, true);
    } else {
        v = new LocalVariable(declaration->name, declaration->name.text, type, true);
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
                error2(3127, declaration->type, "type name is not a type", decl, "declaration here");
            }
        }
        type = dynamic_cast<Type *>(tname);
        if (type == nullptr) {
            error2(3138, declaration->type, "type name is not a type", tname->declaration, "declaration here");
        }
    }
    std::string name = declaration->name.text;
    if (type == nullptr && not scope.top()->allocateName(declaration->name, name)) {
        error2(3047, declaration->name, "duplicate definition of name", scope.top()->getDeclaration(name), "first declaration here");
    }
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    std::vector<FunctionParameter *> args;
    bool in_default = false;
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
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            if (mode != ParameterType::IN) {
                error(3175, x->default_value->token, "default value only available for IN parameters");
            }
            in_default = true;
            def = analyze(x->default_value);
            if (not def->is_constant) {
                error(3148, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3150, x->token, "default value must be specified for this parameter");
        }
        if (scope.top()->lookupName(x->name.text)) {
            error(3174, x->name, "duplicate identifier");
        }
        FunctionParameter *fp = new FunctionParameter(x->name, x->name.text, ptype, mode, def);
        args.push_back(fp);
    }
    if (type != nullptr && args.empty()) {
        error(3129, declaration->rparen, "expected self parameter");
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
    for (auto x: declaration->args) {
        Token decl = scope.top()->getDeclaration(x->name.text);
        if (decl.type != NONE) {
            error2(3179, x->name, "duplicate identifier", decl, "first declaration here");
        }
    }
    frame.push(function->frame);
    scope.push(function->scope);
    functiontypes.push(std::make_pair(type, dynamic_cast<const TypeFunction *>(function->type)));
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    function->statements = analyze(declaration->body);
    const Type *returntype = dynamic_cast<const TypeFunction *>(function->type)->returntype;
    if (returntype != TYPE_NOTHING) {
        if (function->statements.empty() || not function->statements.back()->always_returns()) {
            error(3085, declaration->end_function, "missing RETURN statement");
        }
    }
    loops.pop();
    functiontypes.pop();
    scope.top()->checkForward();
    scope.pop();
    frame.pop();
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_decl(const pt::ExternalFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3092, declaration->name, "duplicate identifier", scope.top()->getDeclaration(name), "first declaration here");
    }
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    std::vector<FunctionParameter *> args;
    bool in_default = false;
    for (auto x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameter::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameter::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameter::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type);
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            in_default = true;
            def = analyze(x->default_value);
            if (not def->is_constant) {
                error(3149, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3151, x->token, "default value must be specified for this parameter");
        }
        FunctionParameter *fp = new FunctionParameter(x->name, x->name.text, ptype, mode, def);
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
    for (auto p: function->params) {
        if (p->mode == ParameterType::OUT) {
            error(3164, p->declaration, "OUT parameter mode not supported (use INOUT): " + p->name);
        }
        if (param_types.find(p->name) == param_types.end()) {
            error(3097, declaration->dict->token, "parameter type missing: " + p->name);
        }
    }

    function->library_name = library_name;
    function->param_types = param_types;
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::NativeFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3166, declaration->name, "duplicate identifier", scope.top()->getDeclaration(name), "first declaration here");
    }
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    std::vector<const ParameterType *> params;
    bool in_default = false;
    for (auto x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameter::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameter::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameter::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type);
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            in_default = true;
            def = analyze(x->default_value);
            if (not def->is_constant) {
                error(3167, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3168, x->token, "default value must be specified for this parameter");
        }
        ParameterType *pt = new ParameterType(x->name, mode, ptype, def);
        params.push_back(pt);
    }
    PredefinedFunction *function = new PredefinedFunction(path_stripext(path_basename(program->source_path))+"$"+name, new TypeFunction(returntype, params));
    scope.top()->addName(declaration->name, name, function);
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::ExceptionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3115, declaration->token, "duplicate definition of name", scope.top()->getDeclaration(name), "first declaration here");
    }
    scope.top()->addName(declaration->name, name, new Exception(declaration->name, name));
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::ExportDeclaration *declaration)
{
    if (scope.top()->getDeclaration(declaration->name.text).type == NONE) {
        error(3152, declaration->name, "export name not declared");
    }
    exports.insert(declaration->name.text);
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
    bool lastexit = false;
    for (auto s: statement) {
        s->accept(&sa);
        if (dynamic_cast<const pt::ExitStatement *>(s) != nullptr
         || dynamic_cast<const pt::NextStatement *>(s) != nullptr
         || dynamic_cast<const pt::RaiseStatement *>(s) != nullptr
         || dynamic_cast<const pt::ReturnStatement *>(s) != nullptr) {
            lastexit = true;
        } else if (lastexit) {
            error(3165, s->token, "unreachable code");
        }
    }
    return statements;
}

const Statement *Analyzer::analyze(const pt::AssertStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (not expr->type->is_equivalent(TYPE_BOOLEAN)) {
        error(3173, statement->expr->token, "boolean value expected");
    }
    std::vector<const Statement *> statements = analyze(statement->body);
    return new AssertStatement(statement->token.line, statements, expr, statement->source);
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
    if (not expr->type->is_equivalent(rhs->type)) {
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
                ComparisonExpression::Comparison comp = static_cast<ComparisonExpression::Comparison>(cwc->comp); // TODO: remove cast
                const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(cwc->expr->token, comp, when);
                for (auto clause: clauses) {
                    for (auto c: clause.first) {
                        if (cond->overlaps(c)) {
                            error2(3062, cwc->expr->token, "overlapping case condition", c->token, "overlaps here");
                        }
                    }
                }
                for (auto c: conditions) {
                    if (cond->overlaps(c)) {
                        error2(3063, cwc->expr->token, "overlapping case condition", c->token, "overlaps here");
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
                            error2(3064, rwc->low_expr->token, "overlapping case condition", c->token, "overlaps here");
                        }
                    }
                }
                for (auto c: conditions) {
                    if (cond->overlaps(c)) {
                        error2(3065, rwc->low_expr->token, "overlapping case condition", c->token, "overlaps here");
                    }
                }
                conditions.push_back(cond);
            }
        }
        scope.push(new Scope(scope.top(), frame.top()));
        std::vector<const Statement *> statements = analyze(x.second);
        scope.pop();
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
        } else if (functiontypes.top().second->returntype != TYPE_NOTHING) {
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
    scope.push(new Scope(scope.top(), frame.top()));
    Token name = statement->var;
    if (scope.top()->lookupName(name.text) != nullptr) {
        error2(3118, name, "duplicate identifier", scope.top()->getDeclaration(name.text), "first declaration here");
    }
    Variable *var;
    if (frame.top() == global_frame) {
        var = new GlobalVariable(name, name.text, TYPE_NUMBER, false);
    } else {
        var = new LocalVariable(name, name.text, TYPE_NUMBER, false);
    }
    scope.top()->addName(var->declaration, var->name, var, true);
    var->is_readonly = true;
    Variable *bound;
    if (frame.top() == global_frame) {
        bound = new GlobalVariable(Token(), std::to_string(reinterpret_cast<intptr_t>(statement)), TYPE_NUMBER, false);
    } else {
        bound = new LocalVariable(Token(), std::to_string(reinterpret_cast<intptr_t>(statement)), TYPE_NUMBER, false);
    }
    // TODO: Need better way of declaring unnamed local variable.
    scope.top()->addName(Token(), std::to_string(reinterpret_cast<intptr_t>(statement)), bound, true);
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
    scope.pop();
    loops.top().pop_back();
    var->is_readonly = false;
    return new ForStatement(statement->token.line, loop_id, new VariableExpression(var), start, end, step, new VariableExpression(bound), statements);
}

const Statement *Analyzer::analyze(const pt::ForeachStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    Token var_name = statement->var;
    if (scope.top()->lookupName(var_name.text) != nullptr) {
        error2(3169, var_name, "duplicate identifier", scope.top()->getDeclaration(var_name.text), "first declaration here");
    }
    const Expression *array = analyze(statement->array);
    const TypeArray *atype = dynamic_cast<const TypeArray *>(array->type);
    if (atype == nullptr) {
        error(3170, statement->array->token, "array expected");
    }
    Variable *var;
    if (frame.top() == global_frame) {
        var = new GlobalVariable(var_name, var_name.text, atype->elementtype, false);
    } else {
        var = new LocalVariable(var_name, var_name.text, atype->elementtype, false);
    }
    scope.top()->addName(var->declaration, var->name, var, true);
    var->is_readonly = true;

    Token index_name = statement->index;
    Variable *index;
    if (index_name.type == IDENTIFIER) {
        if (scope.top()->lookupName(index_name.text) != nullptr) {
            error2(3171, index_name, "duplicate identifier", scope.top()->getDeclaration(index_name.text), "first declaration here");
        }
        if (frame.top() == global_frame) {
            index = new GlobalVariable(index_name, index_name.text, TYPE_NUMBER, false);
        } else {
            index = new LocalVariable(index_name, index_name.text, TYPE_NUMBER, false);
        }
        scope.top()->addName(index->declaration, index->name, index, true);
    } else {
        // TODO: Need better way of declaring unnamed local variable.
        index_name.text = std::to_string(reinterpret_cast<intptr_t>(statement)+1);
        if (frame.top() == global_frame) {
            index = new GlobalVariable(Token(), index_name.text, TYPE_NUMBER, false);
        } else {
            index = new LocalVariable(Token(), index_name.text, TYPE_NUMBER, false);
        }
        scope.top()->addName(Token(), index_name.text, index, true);
    }
    index->is_readonly = true;

    Variable *bound;
    // TODO: Need better way of declaring unnamed local variable.
    std::string bound_name = std::to_string(reinterpret_cast<intptr_t>(statement));
    if (frame.top() == global_frame) {
        bound = new GlobalVariable(Token(), bound_name, TYPE_NUMBER, false);
    } else {
        bound = new LocalVariable(Token(), bound_name, TYPE_NUMBER, false);
    }
    scope.top()->addName(Token(), bound_name, bound, true);
    // TODO: make loop_id a void*
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(FOREACH, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
    loops.top().pop_back();
    var->is_readonly = false;
    return new ForeachStatement(statement->token.line, loop_id, new VariableExpression(var), array, new VariableExpression(index), new VariableExpression(bound), statements);
}

const Statement *Analyzer::analyze(const pt::IfStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    for (auto c: statement->condition_statements) {
        const Expression *cond = nullptr;
        const pt::ValidPointerExpression *valid = dynamic_cast<const pt::ValidPointerExpression *>(c.first);
        if (valid != nullptr) {
            for (auto v: valid->tests) {
                if (not v.shorthand and scope.top()->lookupName(v.name.text) != nullptr) {
                    error2(3102, v.name, "duplicate identifier", scope.top()->getDeclaration(v.name.text), "first declaration here");
                }
                const Expression *ptr = analyze(v.expr);
                const TypePointer *ptrtype = dynamic_cast<const TypePointer *>(ptr->type);
                if (ptrtype == nullptr) {
                    error(3101, v.expr->token, "pointer type expression expected");
                }
                const TypeValidPointer *vtype = new TypeValidPointer(ptrtype);
                Variable *var;
                // TODO: Try to make this a local variable always (give the global scope a local space).
                if (functiontypes.empty()) {
                    var = new GlobalVariable(v.name, v.name.text, vtype, true);
                } else {
                    var = new LocalVariable(v.name, v.name.text, vtype, true);
                }
                scope.top()->addName(v.name, v.name.text, var, true, v.shorthand);
                const Expression *ve = new ValidPointerExpression(var, ptr);
                if (cond == nullptr) {
                    cond = ve;
                } else {
                    cond = new ConjunctionExpression(cond, ve);
                }
            }
        } else {
            cond = analyze(c.first);
            if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
                error(3048, c.first->token, "boolean value expected");
            }
        }
        scope.push(new Scope(scope.top(), frame.top()));
        condition_statements.push_back(std::make_pair(cond, analyze(c.second)));
        scope.pop();
    }
    std::vector<const Statement *> else_statements = analyze(statement->else_statements);
    scope.pop();
    return new IfStatement(statement->token.line, condition_statements, else_statements);
}

const Statement *Analyzer::analyze(const pt::LoopStatement *statement)
{
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(LOOP, loop_id));
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
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
    Scope *s = scope.top();
    if (statement->name.first.type != NONE) {
        const Name *modname = scope.top()->lookupName(statement->name.first.text);
        if (modname == nullptr) {
            error(3157, statement->name.first, "module name not found: " + statement->name.first.text);
        }
        const Module *mod = dynamic_cast<const Module *>(modname);
        if (mod == nullptr) {
            error(3158, statement->name.first, "module name expected");
        }
        s = mod->scope;
    }
    const Name *name = s->lookupName(statement->name.second.text);
    if (name == nullptr) {
        error(3089, statement->name.second, "exception not found: " + statement->name.second.text);
    }
    const Exception *exception = dynamic_cast<const Exception *>(name);
    if (exception == nullptr) {
        error2(3090, statement->name.second, "name not an exception", name->declaration, "declaration here");
    }
    const Expression *info;
    if (statement->info != nullptr) {
        info = analyze(statement->info);
    } else {
        std::vector<const Expression *> values;
        info = new RecordLiteralExpression(dynamic_cast<const TypeRecord *>(s->lookupName("ExceptionInfo")->type), values);
    }
    return new RaiseStatement(statement->token.line, exception, info);
}

const Statement *Analyzer::analyze(const pt::RepeatStatement *statement)
{
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    loops.top().push_back(std::make_pair(REPEAT, loop_id));
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<const Statement *> statements = analyze(statement->body);
    const Expression *cond = analyze(statement->cond);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(3086, statement->cond->token, "boolean value expected");
    }
    scope.pop();
    loops.top().pop_back();
    return new RepeatStatement(statement->token.line, loop_id, cond, statements);
}

const Statement *Analyzer::analyze(const pt::ReturnStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (functiontypes.empty()) {
        error(3093, statement->token, "RETURN not allowed outside function");
    } else if (functiontypes.top().second->returntype == TYPE_NOTHING) {
        error(3094, statement->token, "function does not return a value");
    } else if (not expr->type->is_equivalent(functiontypes.top().second->returntype)) {
        error(3095, statement->expr->token, "type mismatch in RETURN");
    }
    return new ReturnStatement(statement->token.line, expr);
}

const Statement *Analyzer::analyze(const pt::TryStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
    std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;
    for (auto x: statement->catches) {
        Scope *s = scope.top();
        if (x.first.at(0).first.type != NONE) {
            const Name *modname = scope.top()->lookupName(x.first.at(0).first.text);
            if (modname == nullptr) {
                error(3159, x.first.at(0).first, "module name not found: " + x.first.at(0).first.text);
            }
            const Module *mod = dynamic_cast<const Module *>(modname);
            if (mod == nullptr) {
                error(3160, x.first.at(0).first, "module name expected");
            }
            s = mod->scope;
        }
        const Name *name = s->lookupName(x.first.at(0).second.text);
        if (name == nullptr) {
            error(3087, x.first.at(0).second, "exception not found: " + x.first.at(0).second.text);
        }
        const Exception *exception = dynamic_cast<const Exception *>(name);
        if (exception == nullptr) {
            error2(3088, x.first.at(0).second, "name not an exception", name->declaration, "declaration here");
        }
        std::vector<const Exception *> exceptions;
        exceptions.push_back(exception);
        scope.push(new Scope(scope.top(), frame.top()));
        std::vector<const Statement *> statements = analyze(x.second);
        scope.pop();
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
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
    loops.top().pop_back();
    return new WhileStatement(statement->token.line, loop_id, cond, statements);
}

const Program *Analyzer::analyze()
{
    Program *r = new Program(program->source_path, program->source_hash);
    global_frame = r->frame;
    global_scope = r->scope;
    frame.push(global_frame);
    scope.push(global_scope);

    // TODO: This bit makes sure that the native constants are
    // actually available in the module where they are declared.
    // There's certainly a better way that doesn't involve this
    // roundabout way.
    init_builtin_constants(global_scope);
    std::string module_name = program->source_path;
    std::string::size_type i = module_name.find_last_of("/\\");
    if (i != std::string::npos) {
        module_name = module_name.substr(i+1);
    }
    if (module_name.size() >= 6 && module_name.substr(module_name.size() - 5) == ".neon") {
        module_name = module_name.substr(0, module_name.size() - 5);
    }
    init_builtin_constants(module_name, global_scope);

    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    r->statements = analyze(program->body);
    loops.pop();
    r->scope->checkForward();
    for (auto n: exports) {
        const Name *name = scope.top()->lookupName(n);
        if (name == nullptr) {
            internal_error("export name not found");
        }
        if (r->exports.find(n) != r->exports.end()) {
            internal_error("export name already exported");
        }
        r->exports[n] = name;
    }
    scope.pop();
    return r;
}

const Program *analyze(ICompilerSupport *support, const pt::Program *program)
{
    return Analyzer(support, program).analyze();
}
