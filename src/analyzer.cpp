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
    Analyzer(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ExternalGlobalInfo> *external_globals);

    ICompilerSupport *support;
    const pt::Program *program;
    std::map<std::string, ExternalGlobalInfo> *const external_globals;
    const std::string module_name;
    std::map<std::string, Module *> modules;
    Scope *global_scope;
    std::stack<Frame *> frame;
    std::stack<Scope *> scope;
    std::map<std::string, Token> exports;

    std::stack<std::pair<const Type *, const TypeFunction *>> functiontypes;
    std::stack<std::list<std::pair<std::string, unsigned int>>> loops;

    const Type *analyze(const pt::Type *type, const std::string &name = std::string());
    const Type *analyze(const pt::TypeSimple *type, const std::string &name);
    const Type *analyze_enum(const pt::TypeEnum *type, const std::string &name);
    const Type *analyze_record(const pt::TypeRecord *type, const std::string &name);
    const Type *analyze_class(const pt::TypeClass *type, const std::string &name);
    const Type *analyze(const pt::TypePointer *type, const std::string &name);
    const Type *analyze(const pt::TypeValidPointer *type, const std::string &name);
    const Type *analyze(const pt::TypeFunctionPointer *type, const std::string &name);
    const Type *analyze(const pt::TypeParameterised *type, const std::string &name);
    const Type *analyze(const pt::TypeImport *type, const std::string &name);
    const Expression *analyze(const pt::Expression *expr);
    const Expression *analyze(const pt::DummyExpression *expr);
    const Expression *analyze(const pt::IdentityExpression *expr);
    const Expression *analyze(const pt::BooleanLiteralExpression *expr);
    const Expression *analyze(const pt::NumberLiteralExpression *expr);
    const Expression *analyze(const pt::StringLiteralExpression *expr);
    const Expression *analyze(const pt::FileLiteralExpression *expr);
    const Expression *analyze(const pt::BytesLiteralExpression *expr);
    const Expression *analyze(const pt::ArrayLiteralExpression *expr);
    const Expression *analyze(const pt::ArrayLiteralRangeExpression *expr);
    const Expression *analyze(const pt::DictionaryLiteralExpression *expr);
    const Expression *analyze(const pt::NilLiteralExpression *expr);
    const Expression *analyze(const pt::NowhereLiteralExpression *expr);
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
    const Expression *analyze(const pt::IntegerDivisionExpression *expr);
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
    const Expression *analyze(const pt::TryExpression *expr);
    const Expression *analyze(const pt::NewClassExpression *expr);
    const Expression *analyze(const pt::ValidPointerExpression *expr);
    const Expression *analyze(const pt::RangeSubscriptExpression *expr);
    const Statement *analyze(const pt::ImportDeclaration *declaration);
    const Statement *analyze(const pt::TypeDeclaration *declaration);
    const Statement *analyze_decl(const pt::ConstantDeclaration *declaration);
    const Statement *analyze_body(const pt::ConstantDeclaration *declaration);
    const Statement *analyze_decl(const pt::NativeConstantDeclaration *declaration);
    const Statement *analyze_body(const pt::NativeConstantDeclaration *declaration);
    const Statement *analyze_decl(const pt::VariableDeclaration *declaration);
    const Statement *analyze_body(const pt::VariableDeclaration *declaration);
    const Statement *analyze_decl(const pt::NativeVariableDeclaration *declaration);
    const Statement *analyze_body(const pt::NativeVariableDeclaration *declaration);
    const Statement *analyze_decl(const pt::LetDeclaration *declaration);
    const Statement *analyze_body(const pt::LetDeclaration *declaration);
    const Statement *analyze_decl(const pt::FunctionDeclaration *declaration);
    const Statement *analyze_body(const pt::FunctionDeclaration *declaration);
    const Statement *analyze_decl(const pt::ExternalFunctionDeclaration *declaration);
    const Statement *analyze_body(const pt::ExternalFunctionDeclaration *declaration);
    const Statement *analyze(const pt::NativeFunctionDeclaration *declaration);
    const Statement *analyze(const pt::ExceptionDeclaration *declaration);
    const Statement *analyze_decl(const pt::ExportDeclaration *declaration);
    const Statement *analyze_body(const pt::ExportDeclaration *declaration);
    std::vector<const Statement *> analyze(const std::vector<std::unique_ptr<pt::Statement>> &statement);
    const Statement *analyze(const pt::AssertStatement *statement);
    const Statement *analyze(const pt::AssignmentStatement *statement);
    const Statement *analyze(const pt::CaseStatement *statement);
    const Statement *analyze(const pt::CheckStatement *statement);
    const Statement *analyze(const pt::ExecStatement *statement);
    const Statement *analyze(const pt::ExitStatement *statement);
    const Statement *analyze(const pt::ExpressionStatement *statement);
    const Statement *analyze(const pt::ForStatement *statement);
    const Statement *analyze(const pt::ForeachStatement *statement);
    const Statement *analyze(const pt::IfStatement *statement);
    const Statement *analyze(const pt::IncrementStatement *statement);
    const Statement *analyze(const pt::LoopStatement *statement);
    const Statement *analyze(const pt::NextStatement *statement);
    const Statement *analyze(const pt::RaiseStatement *statement);
    const Statement *analyze(const pt::RepeatStatement *statement);
    const Statement *analyze(const pt::ReturnStatement *statement);
    const Statement *analyze(const pt::TryStatement *statement);
    const Statement *analyze(const pt::TryHandlerStatement *statement);
    const Statement *analyze(const pt::WhileStatement *statement);
    const Program *analyze();
private:
    static std::string extract_module_name(const pt::Program *program);
    Module *import_module(const Token &token, const std::string &name);
    Type *deserialize_type(Scope *scope, const std::string &descriptor, std::string::size_type &i);
    Type *deserialize_type(Scope *scope, const std::string &descriptor);
    std::vector<std::pair<std::vector<const Exception *>, const AstNode *>> analyze_catches(const std::vector<std::pair<std::vector<std::pair<Token, Token>>, std::unique_ptr<pt::ParseTreeNode>>> &catches);
    void process_into_results(const pt::ExecStatement *statement, const std::string &sql, const Variable *function, std::vector<const Expression *> args, std::vector<const Statement *> &statements);
    std::vector<TypeRecord::Field> analyze_fields(const pt::TypeRecord *type);
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
    virtual void visit(const pt::TypeClass *t) override { type = a->analyze_class(t, name); }
    virtual void visit(const pt::TypePointer *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeValidPointer *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeFunctionPointer *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeParameterised *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::TypeImport *t) override { type = a->analyze(t, name); }
    virtual void visit(const pt::DummyExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentityExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BooleanLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FileLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BytesLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralRangeExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NowhereLiteralExpression *) override { internal_error("pt::Expression"); }
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
    virtual void visit(const pt::IntegerDivisionExpression *) override { internal_error("pt::Expression"); }
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
    virtual void visit(const pt::TryExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewClassExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeVariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::AssertStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::AssignmentStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::CaseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::CheckStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExecStatement  *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExitStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExpressionStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForeachStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::IfStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::IncrementStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::LoopStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::NextStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RaiseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RepeatStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ReturnStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryHandlerStatement *) override { internal_error("pt::Statement"); }
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
    virtual void visit(const pt::TypeClass *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeValidPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeFunctionPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeImport *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::DummyExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::IdentityExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::BooleanLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NumberLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::StringLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::FileLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::BytesLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ArrayLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ArrayLiteralRangeExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::DictionaryLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NilLiteralExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NowhereLiteralExpression *p) override { expr = a->analyze(p); }
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
    virtual void visit(const pt::IntegerDivisionExpression *p) override { expr = a->analyze(p); }
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
    virtual void visit(const pt::TryExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NewClassExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ValidPointerExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::RangeSubscriptExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ImportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeVariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::AssertStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::AssignmentStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::CaseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::CheckStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExecStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExitStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ExpressionStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ForeachStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::IfStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::IncrementStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::LoopStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::NextStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RaiseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::RepeatStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::ReturnStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryHandlerStatement *) override { internal_error("pt::Statement"); }
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
    virtual void visit(const pt::TypeClass *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeValidPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeFunctionPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeImport *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::DummyExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentityExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BooleanLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FileLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BytesLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralRangeExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NowhereLiteralExpression *) override { internal_error("pt::Expression"); }
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
    virtual void visit(const pt::IntegerDivisionExpression *) override { internal_error("pt::Expression"); }
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
    virtual void visit(const pt::TryExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewClassExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TypeDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ConstantDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeConstantDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::VariableDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeVariableDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::LetDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeFunctionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExceptionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExportDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::AssertStatement *) override {}
    virtual void visit(const pt::AssignmentStatement *) override {}
    virtual void visit(const pt::CaseStatement *) override {}
    virtual void visit(const pt::CheckStatement *) override {}
    virtual void visit(const pt::ExecStatement *) override {}
    virtual void visit(const pt::ExitStatement *) override {}
    virtual void visit(const pt::ExpressionStatement *) override {}
    virtual void visit(const pt::ForStatement *) override {}
    virtual void visit(const pt::ForeachStatement *) override {}
    virtual void visit(const pt::IfStatement *) override {}
    virtual void visit(const pt::IncrementStatement *) override {}
    virtual void visit(const pt::LoopStatement *) override {}
    virtual void visit(const pt::NextStatement *) override {}
    virtual void visit(const pt::RaiseStatement *) override {}
    virtual void visit(const pt::RepeatStatement *) override {}
    virtual void visit(const pt::ReturnStatement *) override {}
    virtual void visit(const pt::TryStatement *) override {}
    virtual void visit(const pt::TryHandlerStatement *) override {}
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
    virtual void visit(const pt::TypeClass *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypePointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeValidPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeFunctionPointer *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeParameterised *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::TypeImport *) override { internal_error("pt::Type"); }
    virtual void visit(const pt::DummyExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::IdentityExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BooleanLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NumberLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::StringLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::FileLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::BytesLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ArrayLiteralRangeExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DictionaryLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NilLiteralExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NowhereLiteralExpression *) override { internal_error("pt::Expression"); }
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
    virtual void visit(const pt::IntegerDivisionExpression *) override { internal_error("pt::Expression"); }
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
    virtual void visit(const pt::TryExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewClassExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) override {}
    virtual void visit(const pt::TypeDeclaration *) override {}
    virtual void visit(const pt::ConstantDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeConstantDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::VariableDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeVariableDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::LetDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override {}
    virtual void visit(const pt::ExceptionDeclaration *) override {}
    virtual void visit(const pt::ExportDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::AssertStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::AssignmentStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::CaseStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::CheckStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExecStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExitStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExpressionStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ForStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ForeachStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::IfStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::IncrementStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::LoopStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::NextStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::RaiseStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::RepeatStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ReturnStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TryStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TryHandlerStatement *p) override { v.push_back(a->analyze(p)); }
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

class GlobalScope: public Scope {
public:
    GlobalScope(): Scope(nullptr, nullptr) {}
};

Analyzer::Analyzer(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ExternalGlobalInfo> *external_globals)
  : support(support),
    program(program),
    external_globals(external_globals),
    module_name(extract_module_name(program)),
    modules(),
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
        FunctionParameter *fp = new FunctionParameter(Token(IDENTIFIER, "self"), "self", this, 1, ParameterType::IN, nullptr);
        params.push_back(fp);
        Function *f = new Function(Token(), "enum.toString", TYPE_STRING, analyzer->global_scope->frame, analyzer->global_scope, params, 1);
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
        methods["toString"] = f;
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

BytesReferenceIndexExpression::BytesReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("bytes__range"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(first);
        args.push_back(new ConstantBooleanExpression(first_from_end));
        args.push_back(last);
        args.push_back(new ConstantBooleanExpression(last_from_end));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("bytes__splice"))), args);
    }
}

BytesValueIndexExpression::BytesValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("bytes__range"))), args);
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

std::string Analyzer::extract_module_name(const pt::Program *program)
{
    std::string module_name = program->source_path;
    std::string::size_type i = module_name.find_last_of("/\\");
    if (i != std::string::npos) {
        module_name = module_name.substr(i+1);
    }
    if (module_name.size() >= 6 && module_name.substr(module_name.size() - 5) == ".neon") {
        module_name = module_name.substr(0, module_name.size() - 5);
    }
    return module_name;
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
    Module *module = new Module(Token(), global_scope, name);
    for (auto t: object.export_types) {
        if (object.strtable[t.descriptor][0] == 'R') {
            // Support recursive record type declarations.
            TypeRecord *actual_record = new TypeRecord(Token(), name + "." + object.strtable[t.name], std::vector<TypeRecord::Field>());
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[t.name], actual_record);
            Type *type = deserialize_type(module->scope, object.strtable[t.descriptor]);
            const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
            const_cast<std::vector<TypeRecord::Field> &>(actual_record->fields) = rectype->fields;
            const_cast<std::map<std::string, size_t> &>(actual_record->field_names) = rectype->field_names;
        } else if (object.strtable[t.descriptor][0] == 'C') {
            // Support recursive class type declarations.
            TypeClass *actual_class = new TypeClass(Token(), name + "." + object.strtable[t.name], std::vector<TypeRecord::Field>());
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[t.name], actual_class);
            Type *type = deserialize_type(module->scope, object.strtable[t.descriptor]);
            const TypeRecord *classtype = dynamic_cast<const TypeClass *>(type);
            const_cast<std::vector<TypeRecord::Field> &>(actual_class->fields) = classtype->fields;
            const_cast<std::map<std::string, size_t> &>(actual_class->field_names) = classtype->field_names;
        } else {
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[t.name], deserialize_type(module->scope, object.strtable[t.descriptor]));
        }
    }
    for (auto c: object.export_constants) {
        const Type *type = deserialize_type(module->scope, object.strtable[c.type]);
        int i = 0;
        module->scope->addName(Token(IDENTIFIER, ""), object.strtable[c.name], new Constant(Token(), object.strtable[c.name], type->deserialize_value(c.value, i)));
    }
    for (auto v: object.export_variables) {
        module->scope->addName(Token(IDENTIFIER, ""), object.strtable[v.name], new ModuleVariable(name, object.strtable[v.name], deserialize_type(module->scope, object.strtable[v.type]), v.index));
    }
    for (auto f: object.export_functions) {
        const std::string function_name = object.strtable[f.name];
        auto i = function_name.find('.');
        if (i != std::string::npos) {
            const std::string typestr = function_name.substr(0, i);
            const std::string method = function_name.substr(i+1);
            Name *n = module->scope->lookupName(typestr);
            Type *type = dynamic_cast<Type *>(n);
            type->methods[method] = new ModuleFunction(name, function_name, deserialize_type(module->scope, object.strtable[f.descriptor]), f.entry);
        } else {
            module->scope->addName(Token(IDENTIFIER, ""), function_name, new ModuleFunction(name, function_name, deserialize_type(module->scope, object.strtable[f.descriptor]), f.entry));
        }
    }
    for (auto e: object.export_exceptions) {
        module->scope->addName(Token(IDENTIFIER, ""), object.strtable[e.name], new Exception(Token(), object.strtable[e.name]));
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

std::vector<TypeRecord::Field> Analyzer::analyze_fields(const pt::TypeRecord *type)
{
    std::vector<TypeRecord::Field> fields;
    std::map<std::string, Token> field_names;
    for (auto &x: type->fields) {
        std::string name = x->name.text;
        auto prev = field_names.find(name);
        if (prev != field_names.end()) {
            error2(3009, x->name, "duplicate field: " + x->name.text, prev->second, "first declaration here");
        }
        const Type *t = analyze(x->type.get());
        if (dynamic_cast<const TypeValidPointer *>(t) != nullptr) {
            error(3223, x->type->token, "valid pointer type not permitted as record member");
        }
        fields.push_back(TypeRecord::Field(x->name, t, x->is_private));
        field_names[name] = x->name;
    }
    return fields;
}

const Type *Analyzer::analyze_record(const pt::TypeRecord *type, const std::string &name)
{
    std::vector<TypeRecord::Field> fields = analyze_fields(type);
    return new TypeRecord(type->token, name, fields);
}

const Type *Analyzer::analyze_class(const pt::TypeClass *type, const std::string &name)
{
    std::vector<TypeRecord::Field> fields = analyze_fields(type);
    return new TypeClass(type->token, name, fields);
}

const Type *Analyzer::analyze(const pt::TypePointer *type, const std::string &)
{
    if (type->reftype != nullptr) {
        const pt::TypeSimple *simple = dynamic_cast<const pt::TypeSimple *>(type->reftype.get());
        if (simple != nullptr && scope.top()->lookupName(simple->name) == nullptr) {
            const std::string name = simple->name;
            TypePointer *ptrtype = new TypePointer(type->token, new TypeForwardClass(type->reftype->token));
            scope.top()->addForward(name, ptrtype);
            return ptrtype;
        } else {
            const Type *reftype = analyze(type->reftype.get());
            const TypeClass *classtype = dynamic_cast<const TypeClass *>(reftype);
            if (classtype == nullptr) {
                error(3098, type->reftype->token, "class type expected");
            }
            return new TypePointer(type->token, classtype);
        }
    } else {
        return new TypePointer(type->token, nullptr);
    }
}

const Type *Analyzer::analyze(const pt::TypeValidPointer *type, const std::string &)
{
    const pt::TypeSimple *simple = dynamic_cast<const pt::TypeSimple *>(type->reftype.get());
    if (simple != nullptr && scope.top()->lookupName(simple->name) == nullptr) {
        const std::string name = simple->name;
        TypeValidPointer *ptrtype = new TypeValidPointer(type->token, new TypeForwardClass(type->reftype->token));
        scope.top()->addForward(name, ptrtype);
        return ptrtype;
    } else {
        const Type *reftype = analyze(type->reftype.get());
        const TypeClass *classtype = dynamic_cast<const TypeClass *>(reftype);
        if (classtype == nullptr) {
            error(3221, type->reftype->token, "class type expected");
        }
        return new TypeValidPointer(type->token, classtype);
    }
}

const Type *Analyzer::analyze(const pt::TypeFunctionPointer *type, const std::string &)
{
    const Type *returntype = type->returntype != nullptr ? analyze(type->returntype.get()) : TYPE_NOTHING;
    std::vector<const ParameterType *> params;
    bool in_default = false;
    for (auto &x: type->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameterGroup::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameterGroup::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameterGroup::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type.get());
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            in_default = true;
            def = analyze(x->default_value.get());
            if (not def->is_constant) {
                error(3177, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3178, x->token, "default value must be specified for this parameter");
        }
        for (auto name: x->names) {
            ParameterType *pt = new ParameterType(name, mode, ptype, def);
            params.push_back(pt);
        }
    }
    return new TypeFunctionPointer(type->token, new TypeFunction(returntype, params));
}

const Type *Analyzer::analyze(const pt::TypeParameterised *type, const std::string &)
{
    if (type->name.text == "Array") {
        const Type *elementtype = analyze(type->elementtype.get());
        if (dynamic_cast<const TypeValidPointer *>(elementtype) != nullptr) {
            error(3222, type->elementtype->token, "valid pointer type not permitted as array element type");
        }
        return new TypeArray(type->name, elementtype);
    }
    if (type->name.text == "Dictionary") {
        return new TypeDictionary(type->name, analyze(type->elementtype.get()));
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

const Expression *Analyzer::analyze(const pt::DummyExpression *)
{
    return new DummyExpression();
}

const Expression *Analyzer::analyze(const pt::IdentityExpression *expr)
{
    return analyze(expr->expr.get());
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
    return new ConstantBytesExpression(expr->name, buffer.str());
}

const Expression *Analyzer::analyze(const pt::BytesLiteralExpression *expr)
{
    std::string bytes;
    std::string::size_type i = 0;
    while (i < expr->data.length()) {
        if (isspace(expr->data[i])) {
            ++i;
            continue;
        }
        auto j = i;
        if (not isxdigit(expr->data[i])) {
            error(3183, expr->token, "invalid hex data");
        }
        j++;
        if (j < expr->data.length() && isxdigit(expr->data[j])) {
            j++;
        }
        bytes.push_back(static_cast<unsigned char>(std::stoul(expr->data.substr(i, j-i), nullptr, 16)));
        i = j;
    }
    return new ConstantBytesExpression("HEXBYTES literal", bytes);
}

const Expression *Analyzer::analyze(const pt::ArrayLiteralExpression *expr)
{
    std::vector<const Expression *> elements;
    const Type *elementtype = nullptr;
    for (auto &x: expr->elements) {
        const Expression *element = analyze(x.get());
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_assignment_compatible(elementtype)) {
            error(3079, x->token, "type mismatch");
        }
        elements.push_back(element);
    }
    return new ArrayLiteralExpression(elementtype, elements);
}

const Expression *Analyzer::analyze(const pt::ArrayLiteralRangeExpression *expr)
{
    const Expression *first = analyze(expr->first.get());
    if (not first->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(2100, expr->first->token, "numeric expression expected");
    }
    const Expression *last = analyze(expr->last.get());
    if (not last->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(2101, expr->last->token, "numeric expression expected");
    }
    const Expression *step = analyze(expr->step.get());
    if (not step->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(2102, expr->step->token, "numeric expression expected");
    }
    const VariableExpression *range = new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("array__range")));
    std::vector<const Expression *> args;
    args.push_back(first);
    args.push_back(last);
    args.push_back(step);
    return new FunctionCall(range, args);
}

const Expression *Analyzer::analyze(const pt::DictionaryLiteralExpression *expr)
{
    std::vector<std::pair<std::string, const Expression *>> elements;
    std::map<std::string, Token> keys;
    const Type *elementtype = nullptr;
    for (auto &x: expr->elements) {
        const Expression *k = analyze(x.first.get());
        if (not k->is_constant) {
            error(3212, x.first->token, "key value must be constant");
        }
        std::string key = k->eval_string(x.first->token);
        auto i = keys.find(key);
        if (i != keys.end()) {
            error2(3080, x.first->token, "duplicate key", i->second, "first key here");
        }
        keys[key] = x.first->token;
        const Expression *element = analyze(x.second.get());
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_assignment_compatible(elementtype)) {
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

const Expression *Analyzer::analyze(const pt::NowhereLiteralExpression *)
{
    return new ConstantNowhereExpression();
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
    const Name *base = analyze_qualified_name(dotted->base.get());
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
        internal_error("qualified name resolved but not matched: " + name->name);
    }
    name = analyze_qualified_name(expr->base.get());
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
    const Expression *base = analyze(expr->base.get());
    const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(base->type);
    if (recordtype == nullptr) {
        error(3046, expr->base->token, "not a record");
    }
    if (dynamic_cast<const TypeForwardClass *>(recordtype) != nullptr) {
        internal_error("class not defined yet");
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
    const Expression *base = analyze(expr->base.get());
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
    if (dynamic_cast<const TypeForwardClass *>(recordtype) != nullptr) {
        error2(3104, expr->base->token, "class not defined yet", recordtype->declaration, "forward declaration here");
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
    const Expression *base = analyze(expr->base.get());
    const Expression *index = analyze(expr->index.get());
    const Type *type = base->type;
    const TypeArray *arraytype = dynamic_cast<const TypeArray *>(type);
    const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(type);
    if (arraytype != nullptr) {
        if (not index->type->is_assignment_compatible(TYPE_NUMBER)) {
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
        if (not index->type->is_assignment_compatible(TYPE_STRING)) {
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
        if (not index->type->is_assignment_compatible(TYPE_NUMBER)) {
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
    for (auto &x: expr->parts) {
        const Expression *e = analyze(x.first.get());
        std::string fmt = x.second.text;
        if (not fmt.empty()) {
            format::Spec spec;
            if (not format::parse(fmt, spec)) {
                error(3133, x.second, "invalid format specification");
            }
        }
        const Expression *str;
        if (e->type->is_assignment_compatible(TYPE_STRING)) {
            str = e;
        } else {
            auto toString = e->type->methods.find("toString");
            if (toString == e->type->methods.end()) {
                error(3132, x.first->token, "no toString() method found for type");
            }
            {
                std::vector<const Expression *> args;
                args.push_back(e);
                str = new FunctionCall(new VariableExpression(toString->second), args);
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
    // TODO: This stuff works, but it's increasingly becoming a mess. There seem
    // to be quite a few cases of x.y(), and this function tries to handle them all
    // in a haphazard fashion.
    const TypeRecord *recordtype = nullptr;
    const pt::IdentifierExpression *fname = dynamic_cast<const pt::IdentifierExpression *>(expr->base.get());
    if (fname != nullptr) {
        if (fname->name == "valueCopy") {
            if (expr->args.size() != 2) {
                error(3136, expr->rparen, "two arguments expected");
            }
            const Expression *lhs_expr = analyze(expr->args[0]->expr.get());
            const ReferenceExpression *lhs = dynamic_cast<const ReferenceExpression *>(lhs_expr);
            if (lhs == nullptr) {
                error(3119, expr->args[0]->expr->token, "expression is not assignable");
            }
            if (lhs_expr->is_readonly && dynamic_cast<const TypePointer *>(lhs_expr->type) == nullptr) {
                error(3120, expr->args[0]->expr->token, "valueCopy to readonly expression");
            }
            const Expression *rhs = analyze(expr->args[1]->expr.get());
            const Type *ltype = lhs->type;
            const TypePointer *lptype = dynamic_cast<const TypePointer *>(ltype);
            if (lptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(lptype) == nullptr) {
                    error(3121, expr->args[0]->expr->token, "valid pointer type required");
                }
                ltype = lptype->reftype;
                lhs = new PointerDereferenceExpression(ltype, lhs);
            }
            const Type *rtype = rhs->type;
            const TypePointer *rptype = dynamic_cast<const TypePointer *>(rtype);
            if (rptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(rptype) == nullptr) {
                    error(3122, expr->args[1]->expr->token, "valid pointer type required");
                }
                rtype = rptype->reftype;
                rhs = new PointerDereferenceExpression(rtype, rhs);
            }
            if (not ltype->is_assignment_compatible(rtype)) {
                error(3123, expr->args[1]->expr->token, "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(lhs);
            return new StatementExpression(new AssignmentStatement(expr->token.line, vars, rhs));
        }
        recordtype = dynamic_cast<const TypeRecord *>(scope.top()->lookupName(fname->name));
    }
    const pt::DotExpression *dotmethod = dynamic_cast<const pt::DotExpression *>(expr->base.get());
    const pt::ArrowExpression *arrowmethod = dynamic_cast<const pt::ArrowExpression *>(expr->base.get());
    const Expression *self = nullptr;
    const Expression *func = nullptr;
    if (dotmethod != nullptr) {
        // This check avoids trying to evaluate foo.bar as an
        // expression in foo.bar() where foo is actually a module.
        bool is_module_call = false;
        const pt::IdentifierExpression *ident = dynamic_cast<const pt::IdentifierExpression *>(dotmethod->base.get());
        if (ident != nullptr) {
            const Name *name = scope.top()->lookupName(ident->name);
            is_module_call = dynamic_cast<const Module *>(name) != nullptr;
        }
        if (not is_module_call) {
            const Expression *base = analyze(dotmethod->base.get());
            auto m = base->type->methods.find(dotmethod->name.text);
            if (m == base->type->methods.end()) {
                error(3137, dotmethod->name, "method not found");
            } else {
                self = base;
            }
            func = new VariableExpression(m->second);
        } else {
            recordtype = dynamic_cast<const TypeRecord *>(analyze_qualified_name(expr->base.get()));
            if (recordtype == nullptr) {
                func = analyze(expr->base.get());
            }
        }
    } else if (arrowmethod != nullptr) {
        const Expression *base = analyze(arrowmethod->base.get());
        const TypePointer *ptype = dynamic_cast<const TypePointer *>(base->type);
        if (ptype == nullptr) {
            error(3218, arrowmethod->base->token, "pointer type expected");
        }
        if (dynamic_cast<const TypeValidPointer *>(ptype) == nullptr) {
            error(3219, arrowmethod->base->token, "valid pointer required");
        }
        auto m = ptype->reftype->methods.find(arrowmethod->name.text);
        if (m == ptype->reftype->methods.end()) {
            error(3220, arrowmethod->name, "method not found");
        }
        self = base;
        func = new VariableExpression(m->second);
    } else if (recordtype == nullptr) {
        func = analyze(expr->base.get());
    }
    if (recordtype != nullptr) {
        std::vector<const Expression *> elements(recordtype->fields.size());
        for (auto &x: expr->args) {
            if (x->name.text.empty()) {
                error(3208, x->expr->token, "field name must be specified using WITH");
            }
            auto f = recordtype->fields.begin();
            for (;;) {
                if (f == recordtype->fields.end()) {
                    error(3209, x->name, "field name not found");
                }
                if (x->name.text == f->name.text) {
                    break;
                }
                ++f;
            }
            auto p = std::distance(recordtype->fields.begin(), f);
            const Expression *element = analyze(x->expr.get());
            if (not f->type->is_assignment_compatible(element->type)) {
                error2(3131, x->expr->token, "type mismatch", f->name, "field declared here");
            }
            if (elements[p] != nullptr) {
                error(3210, x->name, "field name already specified");
            }
            elements[p] = element;
        }
        for (size_t p = 0; p < elements.size(); p++) {
            if (elements[p] == nullptr) {
                elements[p] = recordtype->fields[p].type->make_default_value();
            }
        }
        return new RecordLiteralExpression(recordtype, elements);
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
    for (auto &a: expr->args) {
        const Expression *e = analyze(a->expr.get());
        if (param_index >= static_cast<int>(ftype->params.size())) {
            error(3096, a->expr->token, "too many parameters");
        }
        int p;
        if (param_index >= 0 && a->name.text.empty()) {
            p = param_index;
            param_index++;
        } else {
            // Now in named argument mode.
            param_index = -1;
            if (a->name.text.empty()) {
                error(3145, a->expr->token, "parameter name must be specified");
            }
            auto fp = ftype->params.begin();
            for (;;) {
                if (a->name.text == (*fp)->declaration.text) {
                    break;
                }
                ++fp;
                if (fp == ftype->params.end()) {
                    error(3146, a->name, "parameter name not found");
                }
            }
            p = static_cast<int>(std::distance(ftype->params.begin(), fp));
            if (args[p] != nullptr) {
                error(3147, a->name, "parameter already specified");
            }
        }
        if (dynamic_cast<const DummyExpression *>(e) != nullptr && a->mode.type != OUT) {
            error2(3193, a->expr->token, "Underscore can only be used with OUT parameter", ftype->params[p]->declaration, "function argument here");
        }
        if (ftype->params[p]->mode == ParameterType::IN) {
            if (ftype->params[p]->type != nullptr) {
                // TODO: Above check works around problem in sdl.RenderDrawLines.
                // Something about a compound type in a predefined function parameter list.
                if (not ftype->params[p]->type->is_assignment_compatible(e->type)) {
                    error2(3019, a->expr->token, "type mismatch", ftype->params[p]->declaration, "function argument here");
                }
            }
        } else {
            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
            if (ref == nullptr) {
                error2(3018, a->expr->token, "function call argument must be reference", ftype->params[p]->declaration, "function argument here");
            }
            if (ref->is_readonly) {
                error(3106, a->expr->token, "readonly parameter to OUT");
            }
            if (not e->type->is_assignment_compatible(ftype->params[p]->type)) {
                error2(3194, a->expr->token, "type mismatch", ftype->params[p]->declaration, "function argument here");
            }
        }
        if (ftype->params[p]->mode == ParameterType::OUT && a->mode.type != OUT) {
            error(3184, a->expr->token, "OUT keyword required");
        } else if (ftype->params[p]->mode == ParameterType::INOUT && a->mode.type != INOUT) {
            error(3185, a->expr->token, "INOUT keyword required");
        } else if ((a->mode.type == IN && ftype->params[p]->mode != ParameterType::IN)
                || (a->mode.type == INOUT && ftype->params[p]->mode != ParameterType::INOUT)) {
            error(3186, a->mode, "parameter mode must match if specified");
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
    const Expression *atom = analyze(expr->expr.get());
    if (not atom->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3144, expr->expr->token, "number required");
    }
    return atom;
}

const Expression *Analyzer::analyze(const pt::UnaryMinusExpression *expr)
{
    const Expression *atom = analyze(expr->expr.get());
    if (not atom->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3021, expr->expr->token, "number required for negation");
    }
    return new UnaryMinusExpression(atom);
}

const Expression *Analyzer::analyze(const pt::LogicalNotExpression *expr)
{
    const Expression *atom = analyze(expr->expr.get());
    if (not atom->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        error(3022, expr->expr->token, "boolean required for logical not");
    }
    return new LogicalNotExpression(atom);
}

const Expression *Analyzer::analyze(const pt::ExponentiationExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new ExponentiationExpression(left, right);
    } else {
        error(3024, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::MultiplicationExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new MultiplicationExpression(left, right);
    } else {
        error(3025, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::DivisionExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new DivisionExpression(left, right);
    } else {
        error(3026, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::IntegerDivisionExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new FunctionCall(new VariableExpression(new PredefinedFunction("math$trunc", new TypeFunction(TYPE_NUMBER, {new ParameterType(Token(), ParameterType::IN, TYPE_NUMBER, nullptr)}))), {new DivisionExpression(left, right)});
    } else {
        error(3207, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ModuloExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new ModuloExpression(left, right);
    } else {
        error(3027, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::AdditionExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new AdditionExpression(left, right);
    } else if (left->type->is_assignment_compatible(TYPE_STRING) && right->type->is_assignment_compatible(TYPE_STRING)) {
        error(3124, expr->token, "type mismatch (use & to concatenate strings)");
    } else {
        error(3028, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::SubtractionExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_NUMBER) && right->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new SubtractionExpression(left, right);
    } else {
        error(3029, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ConcatenationExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_STRING) && right->type->is_assignment_compatible(TYPE_STRING)) {
        std::vector<const Expression *> args;
        args.push_back(left);
        args.push_back(right);
        return new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("concat"))), args);
    } else if (left->type->is_assignment_compatible(TYPE_BYTES) && right->type->is_assignment_compatible(TYPE_BYTES)) {
        std::vector<const Expression *> args;
        args.push_back(left);
        args.push_back(right);
        return new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("concatBytes"))), args);
    } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr
            && dynamic_cast<const TypeArray *>(right->type) != nullptr
            && dynamic_cast<const TypeArray *>(left->type)->elementtype == dynamic_cast<const TypeArray *>(right->type)->elementtype) {
        std::vector<const Expression *> args;
        args.push_back(left);
        args.push_back(right);
        VariableExpression *ve = new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName("array__concat")));
        // Since the array__concat function cannot be declared with its proper result type,
        // we have to create a new appropriate function type based on the desired result type
        // and the existing argument types.
        ve->type = new TypeFunction(left->type, dynamic_cast<const TypeFunction *>(ve->type)->params);
        return new FunctionCall(ve, args);
    } else {
        error(3116, expr->token, "type mismatch");
    }
}

static ComparisonExpression *analyze_comparison(const Token &token, const Expression *left, ComparisonExpression::Comparison comp, const Expression *right)
{
    if (not left->type->is_assignment_compatible(right->type)) {
        error(3030, token, "type mismatch");
    }
    if (left->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3031, token, "comparison not available for Boolean");
        }
        return new BooleanComparisonExpression(left, right, comp);
    } else if (left->type->is_assignment_compatible(TYPE_NUMBER)) {
        return new NumericComparisonExpression(left, right, comp);
    } else if (left->type->is_assignment_compatible(TYPE_STRING) || left->type->is_assignment_compatible(TYPE_BYTES)) {
        return new StringComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3032, token, "comparison not available for Array");
        }
        return new ArrayComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeDictionary *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3033, token, "comparison not available for Dictionary");
        }
        return new DictionaryComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeRecord *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3034, token, "comparison not available for RECORD");
        }
        return new ArrayComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeEnum *>(left->type) != nullptr) {
        return new NumericComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypePointer *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3100, token, "comparison not available for POINTER");
        }
        return new PointerComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeFunctionPointer *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(3180, token, "comparison not available for FUNCTION");
        }
        return new FunctionPointerComparisonExpression(left, right, comp);
    } else {
        internal_error("unknown type in parseComparison");
    }
}

const Expression *Analyzer::analyze(const pt::ComparisonExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    ComparisonExpression::Comparison comp = static_cast<ComparisonExpression::Comparison>(expr->comp); // TODO: remove cast
    const Expression *right = analyze(expr->right.get());
    return analyze_comparison(expr->token, left, comp, right);
}

const Expression *Analyzer::analyze(const pt::ChainedComparisonExpression *expr)
{
    std::vector<const ComparisonExpression *> comps;
    const Expression *left = analyze(expr->left.get());
    Token token = expr->left->token;
    for (auto &x: expr->comps) {
        ComparisonExpression::Comparison comp = static_cast<ComparisonExpression::Comparison>(x->comp); // TODO: remove cast
        const Expression *right = analyze(x->right.get());
        comps.push_back(analyze_comparison(token, left, comp, right));
        token = x->right->token;
    }
    return new ChainedComparisonExpression(comps);
}

const Expression *Analyzer::analyze(const pt::MembershipExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    const TypeArray *arraytype = dynamic_cast<const TypeArray *>(right->type);
    const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(right->type);
    if (arraytype != nullptr) {
        if (not left->type->is_assignment_compatible(arraytype->elementtype)) {
            error(3082, expr->left->token, "type mismatch");
        }
        return new ArrayInExpression(left, right);
    } else if (dicttype != nullptr) {
        if (not left->type->is_assignment_compatible(TYPE_STRING)) {
            error(3083, expr->left->token, "type mismatch");
        }
        return new DictionaryInExpression(left, right);
    } else {
        error(3081, expr->right->token, "IN must be used with Array or Dictionary");
    }
}

const Expression *Analyzer::analyze(const pt::ConjunctionExpression *expr)
{
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_BOOLEAN) && right->type->is_assignment_compatible(TYPE_BOOLEAN)) {
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
    const pt::ComparisonExpression *lne = dynamic_cast<const pt::ComparisonExpression *>(expr->left.get());
    const pt::ComparisonExpression *rne = dynamic_cast<const pt::ComparisonExpression *>(expr->right.get());
    if (lne != nullptr && rne != nullptr && lne->comp == pt::ComparisonExpression::NE && rne->comp == pt::ComparisonExpression::NE) {
        const pt::IdentifierExpression *lid = dynamic_cast<const pt::IdentifierExpression *>(lne->left.get());
        const pt::IdentifierExpression *rid = dynamic_cast<const pt::IdentifierExpression *>(rne->left.get());
        if (lid != nullptr && rid != nullptr && lid->name == rid->name) {
            const pt::BooleanLiteralExpression *lble = dynamic_cast<const pt::BooleanLiteralExpression *>(lne->right.get());
            const pt::BooleanLiteralExpression *rble = dynamic_cast<const pt::BooleanLiteralExpression *>(rne->right.get());
            const pt::NumberLiteralExpression *lnle = dynamic_cast<const pt::NumberLiteralExpression *>(lne->right.get());
            const pt::NumberLiteralExpression *rnle = dynamic_cast<const pt::NumberLiteralExpression *>(rne->right.get());
            const pt::StringLiteralExpression *lsle = dynamic_cast<const pt::StringLiteralExpression *>(lne->right.get());
            const pt::StringLiteralExpression *rsle = dynamic_cast<const pt::StringLiteralExpression *>(rne->right.get());
            if ((lble != nullptr && rble != nullptr && lble->value != rble->value)
             || (lnle != nullptr && rnle != nullptr && number_is_not_equal(lnle->value, rnle->value))
             || (lsle != nullptr && rsle != nullptr && lsle->value != rsle->value)) {
                error(3172, expr->token, "boolean expression is always false");
            }
        }
    }

    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (left->type->is_assignment_compatible(TYPE_BOOLEAN) && right->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        return new DisjunctionExpression(left, right);
    } else {
        error(3036, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ConditionalExpression *expr)
{
    const Expression *cond = analyze(expr->cond.get());
    const Expression *left = analyze(expr->left.get());
    const Expression *right = analyze(expr->right.get());
    if (not left->type->is_assignment_compatible(right->type)) {
        error(3037, expr->left->token, "type of THEN and ELSE must match");
    }
    return new ConditionalExpression(cond, left, right);
}

const Expression *Analyzer::analyze(const pt::TryExpression *expr)
{
    const Expression *e = analyze(expr->expr.get());
    auto catches = analyze_catches(expr->catches);
    auto eci = expr->catches.begin();
    for (auto c: catches) {
        const ExceptionHandlerStatement *ehs = dynamic_cast<const ExceptionHandlerStatement *>(c.second);
        const Expression *e = dynamic_cast<const Expression *>(c.second);
        if (ehs != nullptr) {
            if (ehs->statements.empty()) {
                error(3202, expr->expr->token, "body cannot be empty");
            }
            if (not ehs->statements.back()->is_scope_exit_statement()) {
                error(3203, dynamic_cast<const pt::TryHandlerStatement *>(eci->second.get())->body.back()->token, "handler must end in EXIT, NEXT, RAISE, or RETURN");
            }
        } else if (e != nullptr) {
            // pass
        } else {
            internal_error("unexpected catch type");
        }
        ++eci;
    }
    return new TryExpression(e, catches);
}

const Expression *Analyzer::analyze(const pt::NewClassExpression *expr)
{
    const pt::Expression *e = expr->expr.get();
    const pt::Expression *type_expr = e;
    const Expression *value = nullptr;
    const pt::FunctionCallExpression *fc = dynamic_cast<const pt::FunctionCallExpression *>(e);
    if (fc != nullptr) {
        type_expr = fc->base.get();
        value = analyze(fc);
    }
    const Name *name = analyze_qualified_name(type_expr);
    const TypeClass *type = dynamic_cast<const TypeClass *>(name);
    if (type == nullptr) {
        error(3099, type_expr->token, "class type expected");
    }
    return new NewClassExpression(type, value);
}

const Expression *Analyzer::analyze(const pt::ValidPointerExpression * /*expr*/)
{
    // This should never happen because ValidPointerExpression is handled elsewhere.
    internal_error("TODO pt::Expression");
}

const Expression *Analyzer::analyze(const pt::RangeSubscriptExpression *expr)
{
    const Expression *base = analyze(expr->base.get());
    const Expression *first = analyze(expr->range->get_first());
    const Expression *last = analyze(expr->range->get_last());
    if (not first->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3141, expr->range->get_first()->token, "range index must be a number");
    }
    if (not last->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3142, expr->range->get_last()->token, "range index must be a number");
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
    } else if (type == TYPE_BYTES) {
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new BytesReferenceIndexExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        } else {
            return new BytesValueIndexExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
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
        case 'R': case 'C': {
            char kind = descriptor.at(i);
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
            if (kind == 'R') {
                return new TypeRecord(Token(), "record", fields);
            } else if (kind == 'C') {
                return new TypeClass(Token(), "class", fields);
            } else {
                internal_error("what kind?");
            }
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
            const TypeClass *classtype = nullptr;
            if (descriptor.at(i) != '>') {
                const Type *type = deserialize_type(scope, descriptor, i);
                classtype = dynamic_cast<const TypeClass *>(type);
                if (classtype == nullptr) {
                    internal_error("deserialize_type");
                }
            }
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new TypePointer(Token(), classtype);
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
    if (declaration->alias.type == NONE && declaration->name.type == NONE) {
        if (scope.top()->lookupName(declaration->module.text) != nullptr && modules.find(declaration->module.text) != modules.end()) {
            return new NullStatement(declaration->token.line);
        }
    }
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
    TypeClass *actual_class = nullptr;
    TypeRecord *actual_record = nullptr;
    const pt::TypeRecord *classdecl = dynamic_cast<const pt::TypeClass *>(declaration->type.get());
    const pt::TypeRecord *recdecl = dynamic_cast<const pt::TypeRecord *>(declaration->type.get());
    if (classdecl != nullptr) {
        // Support recursive class type declarations.
        actual_class = new TypeClass(classdecl->token, name, std::vector<TypeRecord::Field>());
        scope.top()->addName(declaration->token, name, actual_class);
    } else if (recdecl != nullptr) {
        // Support recursive record type declarations.
        actual_record = new TypeRecord(recdecl->token, name, std::vector<TypeRecord::Field>());
        scope.top()->addName(declaration->token, name, actual_record);
    }
    const Type *type = analyze(declaration->type.get(), name);
    if (actual_class != nullptr) {
        const TypeClass *classtype = dynamic_cast<const TypeClass *>(type);
        const_cast<std::vector<TypeRecord::Field> &>(actual_class->fields) = classtype->fields;
        const_cast<std::map<std::string, size_t> &>(actual_class->field_names) = classtype->field_names;
        type = actual_class;
    } else if (actual_record != nullptr) {
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
    const TypeClass *classtype = dynamic_cast<const TypeClass *>(type);
    if (classtype != nullptr) {
        scope.top()->resolveForward(name, classtype);
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
    const Type *type = analyze(declaration->type.get());
    const Expression *value = analyze(declaration->value.get());
    if (not type->is_assignment_compatible(value->type)) {
        error(3015, declaration->value->token, "type mismatch");
    }
    if (not value->is_constant) {
        error(3016, declaration->value->token, "value must be constant");
    }
    if (type == TYPE_BOOLEAN) {
        value = new ConstantBooleanExpression(value->eval_boolean(declaration->value->token));
    } else if (type == TYPE_NUMBER) {
        value = new ConstantNumberExpression(value->eval_number(declaration->value->token));
    } else if (type == TYPE_STRING) {
        value = new ConstantStringExpression(value->eval_string(declaration->value->token));
    } else {
        Variable *v = new GlobalVariable(declaration->name, declaration->name.text, type, true);
        scope.top()->addName(v->declaration, v->name, v, true);
        return new AssignmentStatement(declaration->token.line, {new VariableExpression(v)}, value);
    }
    scope.top()->addName(declaration->name, name, new Constant(declaration->name, name, value));
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_decl(const pt::NativeConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3206, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::NativeConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    scope.top()->addName(declaration->name, name, new Constant(declaration->name, name, get_native_constant_value(module_name + "$" + name)));
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
    const Type *type = analyze(declaration->type.get());
    std::vector<Variable *> variables;
    for (auto name: declaration->names) {
        Variable *v = frame.top()->createVariable(name, name.text, type, false);
        variables.push_back(v);
    }
    std::vector<const ReferenceExpression *> refs;
    const Expression *expr = nullptr;
    if (declaration->value != nullptr) {
        expr = analyze(declaration->value.get());
        if (not type->is_assignment_compatible(expr->type)) {
            error(3113, declaration->value->token, "type mismatch");
        }
    }
    for (auto v: variables) {
        scope.top()->addName(v->declaration, v->name, v, true);
        refs.push_back(new VariableExpression(v));
    }
    if (declaration->value != nullptr) {
        return new AssignmentStatement(declaration->token.line, refs, expr);
    } else {
        return new ResetStatement(declaration->token.line, refs);
    }
}

const Statement *Analyzer::analyze_decl(const pt::NativeVariableDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3225, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::NativeVariableDeclaration *declaration)
{
    //std::string name = declaration->name.text;
    //scope.top()->addName(declaration->name, name, new Constant(declaration->name, name, get_native_constant_value(module_name + "$" + name)));
    return new NullStatement(declaration->token.line);
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
    const Type *type = analyze(declaration->type.get());
    const Expression *expr = analyze(declaration->value.get());
    if (not type->is_assignment_compatible(expr->type)) {
        error(3140, declaration->value->token, "type mismatch");
    }
    const TypePointer *ptype = dynamic_cast<const TypePointer *>(type);
    if (ptype != nullptr && dynamic_cast<const NewClassExpression *>(expr) != nullptr) {
        type = new TypeValidPointer(Token(), ptype->reftype);
    }
    Variable *v = frame.top()->createVariable(declaration->name, declaration->name.text, type, true);
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
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype.get()) : TYPE_NOTHING;
    std::vector<FunctionParameter *> args;
    bool in_default = false;
    for (auto &x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameterGroup::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameterGroup::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameterGroup::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type.get());
        if (type != nullptr && args.empty()) {
            const TypeClass *ctype = dynamic_cast<const TypeClass *>(type);
            if (ctype != nullptr) {
                const TypeValidPointer *vptype = dynamic_cast<const TypeValidPointer *>(ptype);
                if (vptype == nullptr || not vptype->reftype->is_assignment_compatible(type)) {
                    error(3224, x->type->token, "valid pointer type expected");
                }
            } else {
                if (not ptype->is_assignment_compatible(type)) {
                    error(3128, x->type->token, "expected self parameter");
                }
            }
        }
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            if (mode != ParameterType::IN) {
                error(3175, x->default_value->token, "default value only available for IN parameters");
            }
            in_default = true;
            def = analyze(x->default_value.get());
            if (not def->is_constant) {
                error(3148, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3150, x->token, "default value must be specified for this parameter");
        }
        for (auto name: x->names) {
            if (scope.top()->lookupName(name.text)) {
                error(3174, name, "duplicate identifier");
            }
            FunctionParameter *fp = new FunctionParameter(name, name.text, ptype, frame.top()->get_depth()+1, mode, def);
            args.push_back(fp);
        }
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
            function = new Function(declaration->name, type->name + "." + name, returntype, frame.top(), scope.top(), args, frame.top()->get_depth()+1);
            type->methods[name] = function;
        }
    } else {
        Name *ident = scope.top()->lookupName(name);
        function = dynamic_cast<Function *>(ident);
        if (function == nullptr) {
            function = new Function(declaration->name, name, returntype, frame.top(), scope.top(), args, frame.top()->get_depth()+1);
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
    for (auto &x: declaration->args) {
        for (auto name: x->names) {
            Token decl = scope.top()->getDeclaration(name.text);
            if (decl.type != NONE) {
                error2(3179, name, "duplicate identifier", decl, "first declaration here");
            }
        }
    }
    frame.push(function->frame);
    scope.push(function->scope);
    functiontypes.push(std::make_pair(type, dynamic_cast<const TypeFunction *>(function->type)));
    loops.push(std::list<std::pair<std::string, unsigned int>>());
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
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype.get()) : TYPE_NOTHING;
    std::vector<FunctionParameter *> args;
    bool in_default = false;
    for (auto &x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameterGroup::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameterGroup::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameterGroup::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type.get());
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            in_default = true;
            def = analyze(x->default_value.get());
            if (not def->is_constant) {
                error(3149, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3151, x->token, "default value must be specified for this parameter");
        }
        for (auto name: x->names) {
            FunctionParameter *fp = new FunctionParameter(name, name.text, ptype, frame.size()-1, mode, def);
            args.push_back(fp);
        }
    }
    ExternalFunction *function = new ExternalFunction(declaration->name, name, returntype, frame.top(), scope.top(), args);
    scope.top()->addName(declaration->name, name, function);
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::ExternalFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    ExternalFunction *function = dynamic_cast<ExternalFunction *>(scope.top()->lookupName(name));

    const DictionaryLiteralExpression *dict = dynamic_cast<const DictionaryLiteralExpression *>(analyze(declaration->dict.get()));
    if (not dict->is_constant) {
        error(3071, declaration->dict->token, "constant dictionary expected");
    }
    if (dynamic_cast<const TypeDictionary *>(dict->elementtype) == nullptr) {
        error(3073, declaration->dict->token, "top level dictionary element not dictionary");
    }
    for (auto elem: dict->dict) {
        auto *d = dynamic_cast<const DictionaryLiteralExpression *>(elem.second);
        if (not d->dict.empty() && not d->elementtype->is_assignment_compatible(TYPE_STRING)) {
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
    std::string library_name = kname->second->eval_string(declaration->dict->token);

    auto ktypes = dict->dict.find("types");
    if (ktypes == dict->dict.end()) {
        error(3077, declaration->dict->token, "\"types\" key not found");
    }
    auto &types_dict = dynamic_cast<const DictionaryLiteralExpression *>(ktypes->second)->dict;
    std::map<std::string, std::string> param_types;
    for (auto paramtype: types_dict) {
        param_types[paramtype.first] = paramtype.second->eval_string(declaration->dict->token);
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
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype.get()) : TYPE_NOTHING;
    std::vector<const ParameterType *> params;
    bool in_default = false;
    for (auto &x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameterGroup::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameterGroup::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameterGroup::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type.get());
        const Expression *def = nullptr;
        if (x->default_value != nullptr) {
            in_default = true;
            def = analyze(x->default_value.get());
            if (not def->is_constant) {
                error(3167, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3168, x->token, "default value must be specified for this parameter");
        }
        for (auto name: x->names) {
            ParameterType *pt = new ParameterType(name, mode, ptype, def);
            params.push_back(pt);
        }
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
    if (name.length() < 9 || name.substr(name.length()-9) != "Exception") {
        error(3198, declaration->name, "Exception name must end in 'Exception'");
    }
    scope.top()->addName(declaration->name, name, new Exception(declaration->name, name));
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_decl(const pt::ExportDeclaration *declaration)
{
    if (declaration->declaration != nullptr) {
        std::vector<const Statement *> statements;
        DeclarationAnalyzer da(this, statements);
        declaration->declaration->accept(&da);
        if (not statements.empty()) {
            return statements[0];
        }
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::ExportDeclaration *declaration)
{
    for (auto &name: declaration->names) {
        if (scope.top()->getDeclaration(name.text).type == NONE) {
            error(3152, name, "export name not declared");
        }
        exports[name.text] = name;
    }
    if (declaration->declaration != nullptr) {
        std::vector<const Statement *> statements;
        StatementAnalyzer sa(this, statements);
        declaration->declaration->accept(&sa);
        if (not statements.empty()) {
            return statements[0];
        }
    }
    return new NullStatement(declaration->token.line);
}

std::vector<const Statement *> Analyzer::analyze(const std::vector<std::unique_ptr<pt::Statement>> &statement)
{
    std::vector<const Statement *> statements;
    DeclarationAnalyzer da(this, statements);
    for (auto &d: statement) {
        d->accept(&da);
    }
    StatementAnalyzer sa(this, statements);
    bool lastexit = false;
    for (auto &s: statement) {
        s->accept(&sa);
        if (dynamic_cast<const pt::ExitStatement *>(s.get()) != nullptr
         || dynamic_cast<const pt::NextStatement *>(s.get()) != nullptr
         || dynamic_cast<const pt::RaiseStatement *>(s.get()) != nullptr
         || dynamic_cast<const pt::ReturnStatement *>(s.get()) != nullptr) {
            lastexit = true;
        } else if (lastexit) {
            error(3165, s->token, "unreachable code");
        }
    }
    return statements;
}

static void deconstruct(const pt::Expression *expr, std::vector<const pt::Expression *> &parts)
{
    const pt::IdentityExpression *ie = dynamic_cast<const pt::IdentityExpression *>(expr);
    const pt::UnaryExpression *ue = dynamic_cast<const pt::UnaryExpression *>(expr);
    const pt::BinaryExpression *be = dynamic_cast<const pt::BinaryExpression *>(expr);
    // TODO: Most arrays don't have a toString() method, so this fails for some test code.
    // Handle this somehow.
    //const pt::SubscriptExpression *se = dynamic_cast<const pt::SubscriptExpression *>(expr);
    const pt::ChainedComparisonExpression *che = dynamic_cast<const pt::ChainedComparisonExpression *>(expr);
    const pt::ConditionalExpression *ce = dynamic_cast<const pt::ConditionalExpression *>(expr);
    const pt::RangeSubscriptExpression *re = dynamic_cast<const pt::RangeSubscriptExpression *>(expr);
    if (ie != nullptr) {
        deconstruct(ie->expr.get(), parts);
        return;
    } else if (ue != nullptr) {
        deconstruct(ue->expr.get(), parts);
    } else if (be != nullptr) {
        deconstruct(be->left.get(), parts);
        deconstruct(be->right.get(), parts);
    //} else if (se != nullptr) {
    //    deconstruct(se->base.get(), parts);
    //    deconstruct(se->index.get(), parts);
    } else if (che != nullptr) {
        deconstruct(che->left.get(), parts);
        for (auto &c: che->comps) {
            deconstruct(c->right.get(), parts);
        }
    } else if (ce != nullptr) {
        deconstruct(ce->cond.get(), parts);
        deconstruct(ce->left.get(), parts);
        deconstruct(ce->right.get(), parts);
    } else if (re != nullptr) {
        deconstruct(re->base.get(), parts);
        deconstruct(re->range->get_first(), parts);
        deconstruct(re->range->get_last(), parts);
    } else if (dynamic_cast<const pt::BooleanLiteralExpression *>(expr) != nullptr
            || dynamic_cast<const pt::NumberLiteralExpression *>(expr) != nullptr
            || dynamic_cast<const pt::StringLiteralExpression *>(expr) != nullptr) {
        return;
    }
    parts.push_back(expr);
}

const Statement *Analyzer::analyze(const pt::AssertStatement *statement)
{
    const pt::Expression *e = statement->exprs[0].get();
    const Expression *expr = analyze(e);
    if (not expr->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        error(3173, statement->exprs[0]->token, "boolean value expected");
    }
    std::vector<const pt::Expression *> parts;
    deconstruct(e, parts);
    for (auto x = statement->exprs.begin()+1; x != statement->exprs.end(); ++x) {
        parts.push_back(x->get());
    }
    const Module *io = dynamic_cast<const Module *>(scope.top()->lookupName("io"));
    if (io == nullptr) {
        Module *module = import_module(Token(), "io");
        rtl_import("io", module);
        global_scope->addName(Token(IDENTIFIER, "io"), "io", module, true);
        io = module;
    }
    std::vector<const Statement *> statements;
    const Variable *io_stderr = dynamic_cast<const Variable *>(io->scope->lookupName("stderr"));
    if (io_stderr == nullptr) {
        internal_error("need io.stderr");
    }
    const PredefinedFunction *fprint = dynamic_cast<const PredefinedFunction *>(io->scope->lookupName("fprint"));
    if (fprint == nullptr) {
        internal_error("need io.fprint");
    }
    statements.push_back(
        new ExpressionStatement(
            statement->token.line,
            new FunctionCall(
                new VariableExpression(fprint),
                {
                    new VariableExpression(io_stderr),
                    new ConstantStringExpression("Assert failed (" + statement->token.file() + " line " + std::to_string(statement->token.line) + "):")
                }
            )
        )
    );
    statements.push_back(
        new ExpressionStatement(
            statement->token.line,
            new FunctionCall(
                new VariableExpression(fprint),
                {
                    new VariableExpression(io_stderr),
                    new ConstantStringExpression(statement->token.source_line())
                }
            )
        )
    );
    statements.push_back(
        new ExpressionStatement(
            statement->token.line,
            new FunctionCall(
                new VariableExpression(fprint),
                {
                    new VariableExpression(io_stderr),
                    new ConstantStringExpression("Assert expression dump:")
                }
            )
        )
    );
    std::set<std::string> seen;
    for (auto e: parts) {
        const std::string str = statement->token.source_line().substr(e->get_start_column()-1, e->get_end_column()-e->get_start_column());
        if (seen.find(str) != seen.end()) {
            continue;
        }
        seen.insert(str);

        // Instead of directly constructing an AST fragment here, construct a temporary
        // parse tree so we can leverage the analyze step for InterpolatedStringExpression
        // (this takes care of the call to .toString()).
        std::vector<std::pair<std::unique_ptr<pt::Expression>, Token>> iparts;
        iparts.push_back(std::make_pair(std::unique_ptr<pt::Expression> { new pt::StringLiteralExpression(Token(), 0, "  " + str + " is ") }, Token()));
        iparts.push_back(std::make_pair(std::unique_ptr<pt::Expression> { const_cast<pt::Expression *>(e) }, Token()));
        std::unique_ptr<pt::InterpolatedStringExpression> ie { new pt::InterpolatedStringExpression(Token(), std::move(iparts)) };
        try {
            statements.push_back(
                new ExpressionStatement(
                    statement->token.line,
                    new FunctionCall(
                        new VariableExpression(fprint),
                        {
                            new VariableExpression(io_stderr),
                            analyze(ie.get())
                        }
                    )
                )
            );
            // These pointers are borrowed from the real parse tree,
            // so release them here before the above temporary tree
            // fragment tries to delete them itself.
            ie->parts[1].first.release();
        } catch (...) {
            // And also do this if the above call to analyze() throws
            // any exception.
            ie->parts[1].first.release();
            throw;
        }
    }
    return new AssertStatement(statement->token.line, statements, expr, statement->source);
}

const Statement *Analyzer::analyze(const pt::AssignmentStatement *statement)
{
    if (statement->variables.size() != 1) {
        internal_error("unexpected multiple assign statement");
    }
    const Expression *expr = analyze(statement->variables[0].get());
    const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
    if (ref == nullptr) {
        error(3058, statement->variables[0]->token, "expression is not assignable");
    }
    if (expr->is_readonly) {
        error(3105, statement->variables[0]->token, "assignment to readonly expression");
    }
    const Expression *rhs = analyze(statement->expr.get());
    if (not expr->type->is_assignment_compatible(rhs->type)) {
        error(3057, statement->expr->token, "type mismatch");
    }
    std::vector<const ReferenceExpression *> vars;
    vars.push_back(ref);
    return new AssignmentStatement(statement->token.line, vars, rhs);
}

const Statement *Analyzer::analyze(const pt::CaseStatement *statement)
{
    const Expression *expr = analyze(statement->expr.get());
    if (not expr->type->is_assignment_compatible(TYPE_NUMBER) && not expr->type->is_assignment_compatible(TYPE_STRING) && dynamic_cast<const TypeEnum *>(expr->type) == nullptr) {
        error(3050, statement->expr->token, "CASE expression must be Number, String, or ENUM");
    }
    std::vector<std::pair<std::vector<const CaseStatement::WhenCondition *>, std::vector<const Statement *>>> clauses;
    for (auto &x: statement->clauses) {
        std::vector<const CaseStatement::WhenCondition *> conditions;
        for (auto &c: x.first) {
            const pt::CaseStatement::ComparisonWhenCondition *cwc = dynamic_cast<const pt::CaseStatement::ComparisonWhenCondition *>(c.get());
            const pt::CaseStatement::RangeWhenCondition *rwc = dynamic_cast<const pt::CaseStatement::RangeWhenCondition *>(c.get());
            if (cwc != nullptr) {
                const Expression *when = analyze(cwc->expr.get());
                if (not when->type->is_assignment_compatible(expr->type)) {
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
                const Expression *when = analyze(rwc->low_expr.get());
                if (not when->type->is_assignment_compatible(expr->type)) {
                    error(3053, rwc->low_expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(3054, rwc->low_expr->token, "WHEN condition must be constant");
                }
                const Expression *when2 = analyze(rwc->high_expr.get());
                if (not when2->type->is_assignment_compatible(expr->type)) {
                    error(3055, rwc->high_expr->token, "type mismatch");
                }
                if (not when2->is_constant) {
                    error(3056, rwc->high_expr->token, "WHEN condition must be constant");
                }
                if (when->type->is_assignment_compatible(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(when->type) != nullptr) {
                    if (number_is_greater(when->eval_number(rwc->low_expr->token), when2->eval_number(rwc->high_expr->token))) {
                        error(3109, rwc->high_expr->token, "WHEN numeric range condition must be low..high");
                    }
                } else if (when->type->is_assignment_compatible(TYPE_STRING)) {
                    if (when->eval_string(rwc->low_expr->token) > when2->eval_string(rwc->high_expr->token)) {
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
        clauses.emplace_back(std::make_pair(conditions, statements));
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
        if (expr->type->is_assignment_compatible(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(cond->token), cwhen->comp, cwhen->expr->eval_number(cond->token));
        } else if (expr->type->is_assignment_compatible(TYPE_STRING)) {
            return overlap::check(comp, expr->eval_string(cond->token), cwhen->comp, cwhen->expr->eval_string(cond->token));
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (expr->type->is_assignment_compatible(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(cond->token), rwhen->low_expr->eval_number(cond->token), rwhen->high_expr->eval_number(cond->token));
        } else if (expr->type->is_assignment_compatible(TYPE_STRING)) {
            return overlap::check(comp, expr->eval_string(cond->token), rwhen->low_expr->eval_string(cond->token), rwhen->high_expr->eval_string(cond->token));
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
        if (low_expr->type->is_assignment_compatible(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_number(cwhen->token), low_expr->eval_number(cwhen->token), high_expr->eval_number(cwhen->token));
        } else if (low_expr->type->is_assignment_compatible(TYPE_STRING)) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_string(cwhen->token), low_expr->eval_string(cwhen->token), high_expr->eval_string(cwhen->token));
        } else {
            internal_error("RangeWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (low_expr->type->is_assignment_compatible(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(low_expr->eval_number(cwhen->token), high_expr->eval_number(cwhen->token), rwhen->low_expr->eval_number(cwhen->token), rwhen->high_expr->eval_number(cwhen->token));
        } else if (low_expr->type->is_assignment_compatible(TYPE_STRING)) {
            return overlap::check(low_expr->eval_string(cwhen->token), high_expr->eval_string(cwhen->token), rwhen->low_expr->eval_string(cwhen->token), rwhen->high_expr->eval_string(cwhen->token));
        } else {
            internal_error("RangeWhenCondition");
        }
    } else {
        internal_error("RangeWhenCondition");
    }
}

const Statement *Analyzer::analyze(const pt::CheckStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const Expression *cond = analyze(statement->cond.get());
    if (not cond->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        error(3199, statement->cond->token, "boolean value expected");
    }
    condition_statements.push_back(std::make_pair(cond, std::vector<const Statement *>()));
    std::vector<const Statement *> else_statements = analyze(statement->body);
    if (else_statements.empty()) {
        error(3200, statement->token, "body cannot be empty");
    }
    if (not else_statements.back()->is_scope_exit_statement()) {
        error(3201, statement->body.back()->token, "CHECK body must end in EXIT, NEXT, RAISE, or RETURN");
    }
    scope.pop();
    return new IfStatement(statement->token.line, condition_statements, else_statements);
}

static unsigned int get_loop_id(const Token &token, const std::stack<std::list<std::pair<std::string, unsigned int>>> &loops, const std::string &type)
{
    for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
        if (j->first == type) {
            return j->second;
        }
    }
    error(4301, token, "statement not within loop type while WHENEVER condition in effect");
}

void Analyzer::process_into_results(const pt::ExecStatement *statement, const std::string &sql, const Variable *function, std::vector<const Expression *> args, std::vector<const Statement *> &statements)
{
    const Exception *sql_exception = dynamic_cast<const Exception *>(scope.top()->lookupName("SqlException"));
    if (sql_exception == nullptr) {
        internal_error("need exception SqlException");
    }
    const PredefinedFunction *print = dynamic_cast<const PredefinedFunction *>(scope.top()->lookupName("print"));
    if (print == nullptr) {
        internal_error("where's the print function");
    }
    const Module *sys = dynamic_cast<const Module *>(scope.top()->lookupName("sys"));
    if (sys == nullptr) {
        Module *module = import_module(Token(), "sys");
        rtl_import("sys", module);
        global_scope->addName(Token(IDENTIFIER, "sys"), "sys", module, true);
        sys = module;
    }
    const PredefinedFunction *fexit = dynamic_cast<const PredefinedFunction *>(sys->scope->lookupName("exit"));
    if (fexit == nullptr) {
        internal_error("where's the exit function");
    }
    const Module *sqlite = dynamic_cast<const Module *>(scope.top()->lookupName("sqlite"));
    if (sqlite == nullptr) {
        internal_error("need module sqlite");
    }
    const Type *type_row = dynamic_cast<const Type *>(sqlite->scope->lookupName("Row"));
    if (type_row == nullptr) {
        internal_error("need a Row type");
    }
    std::vector<const VariableExpression *> out_bindings;
    for (auto name: statement->info->assignments) {
        out_bindings.push_back(new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName(name))));
    }
    Variable *result = frame.top()->createVariable(Token(), std::to_string(reinterpret_cast<intptr_t>(statement)), type_row, false);
    Variable *found = frame.top()->createVariable(Token(), std::to_string(reinterpret_cast<intptr_t>(statement)+1), TYPE_BOOLEAN, false);
    scope.top()->addName(Token(IDENTIFIER, ""), std::to_string(reinterpret_cast<intptr_t>(statement)), result, true);
    scope.top()->addName(Token(IDENTIFIER, ""), std::to_string(reinterpret_cast<intptr_t>(statement)+1), found, true);
    args.push_back(new VariableExpression(result));
    statements.push_back(
        new AssignmentStatement(
            statement->token.line,
            {new VariableExpression(found)},
            new FunctionCall(
                new VariableExpression(function),
                args
            )
        )
    );
    std::vector<const Statement *> then_statements;
    std::vector<const Statement *> else_statements;
    uint32_t column = 0;
    for (auto b: out_bindings) {
        then_statements.push_back(
            new AssignmentStatement(
                statement->token.line,
                {b},
                new ArrayReferenceIndexExpression(
                    TYPE_STRING,
                    new VariableExpression(result),
                    new ConstantNumberExpression(number_from_uint32(column)),
                    false
                )
            )
        );
        column++;
    }
    switch (scope.top()->sql_whenever[NotFound]) {
        case Continue:
            break;
        case SqlPrint:
            else_statements.push_back(
                new ExpressionStatement(
                    statement->token.line,
                    new FunctionCall(
                        new VariableExpression(print),
                        {new ConstantStringExpression(sql)}
                    )
                )
            );
            break;
        case Stop:
            else_statements.push_back(
                new ExpressionStatement(
                    statement->token.line,
                    new FunctionCall(
                        new VariableExpression(fexit),
                        {new ConstantNumberExpression(number_from_uint32(1))}
                    )
                )
            );
            break;
        case DoExitLoop:
            else_statements.push_back(
                new ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "LOOP")
                )
            );
            break;
        case DoExitFor:
            else_statements.push_back(
                new ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOR")
                )
            );
            break;
        case DoExitForeach:
            else_statements.push_back(
                new ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOREACH")
                )
            );
            break;
        case DoExitRepeat:
            else_statements.push_back(
                new ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "REPEAT")
                )
            );
            break;
        case DoExitWhile:
            else_statements.push_back(
                new ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "WHILE")
                )
            );
            break;
        case DoNextLoop:
            else_statements.push_back(
                new NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "LOOP")
                )
            );
            break;
        case DoNextFor:
            else_statements.push_back(
                new NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOR")
                )
            );
            break;
        case DoNextForeach:
            else_statements.push_back(
                new NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOREACH")
                )
            );
            break;
        case DoNextRepeat:
            else_statements.push_back(
                new NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "REPEAT")
                )
            );
            break;
        case DoNextWhile:
            else_statements.push_back(
                new NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "WHILE")
                )
            );
            break;
        case DoRaiseException:
            else_statements.push_back(
                new RaiseStatement(
                    statement->token.line,
                    sql_exception,
                    new RecordLiteralExpression(dynamic_cast<const TypeRecord *>(scope.top()->lookupName("ExceptionInfo")->type), {
                        new ConstantStringExpression("No records found")
                    })
                )
            );
            break;
        default:
            internal_error("unexpected whenever condition");
    }
    statements.push_back(
        new IfStatement(
            statement->token.line,
            {std::make_pair(new VariableExpression(found), then_statements)},
            else_statements
        )
    );
}

const Statement *Analyzer::analyze(const pt::ExecStatement *statement)
{
    const Module *sqlite = dynamic_cast<const Module *>(scope.top()->lookupName("sqlite"));
    if (sqlite == nullptr) {
        Module *module = import_module(Token(), "sqlite");
        rtl_import("sqlite", module);
        global_scope->addName(Token(IDENTIFIER, "sqlite"), "sqlite", module, true);
        sqlite = module;
    }
    const PredefinedFunction *exec = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("exec"));
    if (exec == nullptr) {
        internal_error("where's the exec function");
    }
    const PredefinedFunction *execOne = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("execOne"));
    if (execOne == nullptr) {
        internal_error("where's the execOne function");
    }
    const Type *type_rows = dynamic_cast<const Type *>(sqlite->scope->lookupName("Rows"));
    if (type_rows == nullptr) {
        internal_error("need a Rows type");
    }
    std::vector<const Statement *> statements;
    SqlCloseStatement *close = dynamic_cast<SqlCloseStatement *>(statement->info->sql.get());
    SqlConnectStatement *connect = dynamic_cast<SqlConnectStatement *>(statement->info->sql.get());
    SqlDeclareStatement *declare = dynamic_cast<SqlDeclareStatement *>(statement->info->sql.get());
    SqlDisconnectStatement *disconnect = dynamic_cast<SqlDisconnectStatement *>(statement->info->sql.get());
    SqlExecuteStatement *execute = dynamic_cast<SqlExecuteStatement *>(statement->info->sql.get());
    SqlFetchStatement *fetch = dynamic_cast<SqlFetchStatement *>(statement->info->sql.get());
    SqlOpenStatement *open = dynamic_cast<SqlOpenStatement *>(statement->info->sql.get());
    SqlQueryStatement *query = dynamic_cast<SqlQueryStatement *>(statement->info->sql.get());
    SqlWheneverStatement *whenever = dynamic_cast<SqlWheneverStatement *>(statement->info->sql.get());
    if (close != nullptr) {
        const PredefinedFunction *cursorClose = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("cursorClose"));
        statements.push_back(new ExpressionStatement(
            statement->token.line,
            new FunctionCall(
                new VariableExpression(cursorClose),
                {
                    new ConstantStringExpression(close->name->text())
                }
            )
        ));
    } else if (connect != nullptr) {
        const PredefinedFunction *open = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("open"));
        const SqlValueLiteral *target_literal = dynamic_cast<const SqlValueLiteral *>(connect->target.get());
        const SqlValueVariable *target_variable = dynamic_cast<const SqlValueVariable *>(connect->target.get());
        const Expression *target;
        if (target_literal != nullptr) {
            target = new ConstantStringExpression(target_literal->value);
        } else if (target_variable != nullptr) {
            const Variable *var = dynamic_cast<Variable *>(scope.top()->lookupName(target_variable->variable.text));
            if (var == nullptr) {
                error(4302, target_variable->variable, "variable not found");
            }
            target = new VariableExpression(var);
        } else {
            internal_error("unexpected target type");
        }
        const SqlIdentifierSymbol *name_symbol = dynamic_cast<const SqlIdentifierSymbol *>(connect->name.get());
        const SqlIdentifierVariable *name_variable = dynamic_cast<const SqlIdentifierVariable *>(connect->name.get());
        if (name_symbol != nullptr) {
            internal_error("todo");
        } else if (name_variable != nullptr) {
            const Variable *name = dynamic_cast<const Variable *>(scope.top()->lookupName(name_variable->variable.text));
            if (name == nullptr) {
                error(4304, name_variable->variable, "variable not found");
            }
            statements.push_back(new AssignmentStatement(
                statement->token.line,
                {new VariableExpression(name)},
                new FunctionCall(
                    new VariableExpression(open),
                    {target}
                )
            ));
        } else {
            // TODO: handle default connection
            const Variable *name = dynamic_cast<const Variable *>(sqlite->scope->lookupName("db"));
            statements.push_back(new AssignmentStatement(
                statement->token.line,
                {new VariableExpression(name)},
                new FunctionCall(
                    new VariableExpression(open),
                    {target}
                )
            ));
        }
    } else if (declare != nullptr) {
        const SqlDeclareQueryStatement *declare_query = dynamic_cast<const SqlDeclareQueryStatement *>(declare);
        if (declare_query != nullptr) {
            const PredefinedFunction *cursorDeclare = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("cursorDeclare"));
            statements.push_back(new AssignmentStatement(
                statement->token.line,
                {new DummyExpression()},
                new FunctionCall(
                    new VariableExpression(cursorDeclare),
                    {
                        new VariableExpression(dynamic_cast<const Variable *>(sqlite->scope->lookupName("db"))),
                        new ConstantStringExpression(declare_query->cursor->text()),
                        new ConstantStringExpression(declare_query->query)
                    }
                )
            ));
        } else {
            internal_error("unexpected declare type");
        }
    } else if (disconnect != nullptr) {
        const PredefinedFunction *close = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("close"));
        const SqlIdentifierSymbol *name_symbol = dynamic_cast<const SqlIdentifierSymbol *>(disconnect->name.get());
        const SqlIdentifierVariable *name_variable = dynamic_cast<const SqlIdentifierVariable *>(disconnect->name.get());
        if (name_symbol != nullptr) {
            internal_error("todo");
        } else if (name_variable != nullptr) {
            const Variable *name = dynamic_cast<const Variable *>(scope.top()->lookupName(name_variable->variable.text));
            statements.push_back(new ExpressionStatement(
                statement->token.line,
                new FunctionCall(
                    new VariableExpression(close),
                    {new VariableExpression(name)}
                )
            ));
        } else if (disconnect->default_) {
            internal_error("todo");
        } else if (disconnect->all) {
            internal_error("todo");
        } else {
            // TODO: handle default connection
            const Variable *db = dynamic_cast<const Variable *>(sqlite->scope->lookupName("db"));
            statements.push_back(new ExpressionStatement(
                statement->token.line,
                new FunctionCall(
                    new VariableExpression(close),
                    {new VariableExpression(db)}
                )
            ));
        }
    } else if (execute != nullptr) {
        const PredefinedFunction *execRaw = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("execRaw"));
        const SqlValueLiteral *statement_literal = dynamic_cast<const SqlValueLiteral *>(execute->statement.get());
        const SqlValueVariable *statement_variable = dynamic_cast<const SqlValueVariable *>(execute->statement.get());
        const Expression *sqlstatement;
        if (statement_literal != nullptr) {
            sqlstatement = new ConstantStringExpression(statement_literal->value);
        } else if (statement_variable != nullptr) {
            const Variable *var = dynamic_cast<Variable *>(scope.top()->lookupName(statement_variable->variable.text));
            if (var == nullptr) {
                error(4303, statement_variable->variable, "variable not found");
            }
            sqlstatement = new VariableExpression(var);
        } else {
            internal_error("unexpected execute type");
        }
        statements.push_back(new AssignmentStatement(
            statement->token.line,
            {new DummyExpression()},
            new FunctionCall(
                new VariableExpression(execRaw),
                {
                    new VariableExpression(dynamic_cast<const Variable *>(sqlite->scope->lookupName("db"))),
                    sqlstatement
                }
            )
        ));
    } else if (fetch != nullptr) {
        const PredefinedFunction *cursorFetch = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("cursorFetch"));
        process_into_results(
            statement,
            "FETCH",
            cursorFetch,
            {
                new ConstantStringExpression(fetch->name->text()),
            },
            statements
        );
    } else if (open != nullptr) {
        const PredefinedFunction *cursorOpen = dynamic_cast<const PredefinedFunction *>(sqlite->scope->lookupName("cursorOpen"));
        statements.push_back(new ExpressionStatement(
            statement->token.line,
            new FunctionCall(
                new VariableExpression(cursorOpen),
                {
                    new ConstantStringExpression(open->name->text())
                }
            )
        ));
    } else if (query != nullptr) {
        std::vector<std::pair<std::string, const Expression *>> binding_vars;
        for (auto p: statement->info->parameters) {
            binding_vars.push_back(std::make_pair(p, new VariableExpression(dynamic_cast<const Variable *>(scope.top()->lookupName(p.substr(1))))));
        }
        process_into_results(
            statement,
            query->query,
            execOne,
            {
                new VariableExpression(dynamic_cast<const Variable *>(sqlite->scope->lookupName("db"))),
                new ConstantStringExpression(query->query),
                new DictionaryLiteralExpression(TYPE_STRING, binding_vars)
            },
            statements
        );
    } else if (whenever != nullptr) {
        scope.top()->sql_whenever[whenever->condition] = whenever->action;
    } else {
        internal_error("unexpected statement type");
    }
    return new CompoundStatement(statement->token.line, statements);
}

const Statement *Analyzer::analyze(const pt::ExitStatement *statement)
{
    if (statement->type.type == FUNCTION) {
        if (functiontypes.empty()) {
            error(3107, statement->token, "EXIT FUNCTION not allowed outside function");
        } else if (functiontypes.top().second->returntype != TYPE_NOTHING) {
            error(3108, statement->token, "function must return a value");
        }
        return new ReturnStatement(statement->token.line, nullptr);
    }
    std::string type = statement->type.text;
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
    const Expression *expr = analyze(statement->expr.get());
    if (expr->type == TYPE_NOTHING) {
        return new ExpressionStatement(statement->token.line, analyze(statement->expr.get()));
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
    Variable *var = frame.top()->createVariable(name, name.text, TYPE_NUMBER, false);
    scope.top()->addName(var->declaration, var->name, var, true);
    var->is_readonly = true;
    Variable *bound = frame.top()->createVariable(Token(), std::to_string(reinterpret_cast<intptr_t>(statement)), TYPE_NUMBER, false);
    // TODO: Need better way of declaring unnamed local variable.
    scope.top()->addName(Token(IDENTIFIER, ""), std::to_string(reinterpret_cast<intptr_t>(statement)), bound, true);
    const Expression *start = analyze(statement->start.get());
    if (not start->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3067, statement->start->token, "numeric expression expected");
    }
    const Expression *end = analyze(statement->end.get());
    if (not end->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3068, statement->end->token, "numeric expression expected");
    }
    const Expression *step = nullptr;
    if (statement->step != nullptr) {
        step = analyze(statement->step.get());
        if (step->type != TYPE_NUMBER) {
            error(3069, statement->step->token, "numeric expression expected");
        }
        if (not step->is_constant) {
            error(3070, statement->step->token, "numeric expression must be constant");
        }
        if (number_is_zero(step->eval_number(statement->step->token))) {
            error(3091, statement->step->token, "STEP value cannot be zero");
        }
    } else {
        step = new ConstantNumberExpression(number_from_uint32(1));
    }
    // TODO: make loop_id a void*
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3213, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const ReferenceExpression *> vars { new VariableExpression(var) };
    std::vector<const Statement *> init_statements {
        new AssignmentStatement(statement->token.line, vars, start),
        new AssignmentStatement(statement->token.line, { new VariableExpression(bound) }, end),
    };
    std::vector<const Statement *> statements {
        new IfStatement(
            statement->token.line,
            std::vector<std::pair<const Expression *, std::vector<const Statement *>>> {
                std::make_pair(
                    new NumericComparisonExpression(
                        new VariableExpression(var),
                        new VariableExpression(bound),
                        number_is_negative(step->eval_number(statement->step->token)) ? ComparisonExpression::LT : ComparisonExpression::GT
                    ),
                    std::vector<const Statement *> { new ExitStatement(statement->token.line, loop_id) }
                )
            },
            std::vector<const Statement *>()
        ),
    };
    std::vector<const Statement *> body = analyze(statement->body);
    std::copy(body.begin(), body.end(), std::back_inserter(statements));
    std::vector<const Statement *> tail_statements {
        new AssignmentStatement(statement->token.line, vars, new AdditionExpression(new VariableExpression(var), step)),
    };
    scope.pop();
    loops.top().pop_back();
    var->is_readonly = false;
    return new BaseLoopStatement(statement->token.line, loop_id, init_statements, statements, tail_statements, false);
}

const Statement *Analyzer::analyze(const pt::ForeachStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    Token var_name = statement->var;
    if (scope.top()->lookupName(var_name.text) != nullptr) {
        error2(3169, var_name, "duplicate identifier", scope.top()->getDeclaration(var_name.text), "first declaration here");
    }
    const Expression *array = analyze(statement->array.get());
    const TypeArray *arrtype = dynamic_cast<const TypeArray *>(array->type);
    const Type *strtype = dynamic_cast<const TypeString *>(array->type);
    const Type *atype;
    const Type *elementtype;
    std::string len_method;
    if (arrtype != nullptr) {
        atype = arrtype;
        elementtype = arrtype->elementtype;
        len_method = "size";
    } else if (strtype != nullptr) {
        atype = strtype;
        elementtype = TYPE_STRING;
        len_method = "length";
    } else {
        error(3170, statement->array->token, "array or string expected");
    }
    std::string array_copy_name = std::to_string(reinterpret_cast<intptr_t>(statement)+2);
    Variable *array_copy = frame.top()->createVariable(Token(), array_copy_name, atype, false);
    scope.top()->addName(Token(IDENTIFIER, ""), array_copy_name, array_copy, true);

    Variable *var = frame.top()->createVariable(var_name, var_name.text, elementtype, false);
    scope.top()->addName(var->declaration, var->name, var, true);
    var->is_readonly = true;

    Token index_name = statement->index;
    Variable *index;
    if (index_name.type == IDENTIFIER) {
        if (scope.top()->lookupName(index_name.text) != nullptr) {
            error2(3171, index_name, "duplicate identifier", scope.top()->getDeclaration(index_name.text), "first declaration here");
        }
        index = frame.top()->createVariable(index_name, index_name.text, TYPE_NUMBER, false);
        scope.top()->addName(index->declaration, index->name, index, true);
    } else {
        // TODO: Need better way of declaring unnamed local variable.
        index_name.text = std::to_string(reinterpret_cast<intptr_t>(statement)+1);
        index = frame.top()->createVariable(Token(), index_name.text, TYPE_NUMBER, false);
        scope.top()->addName(Token(IDENTIFIER, ""), index_name.text, index, true);
    }
    index->is_readonly = true;

    // TODO: Need better way of declaring unnamed local variable.
    std::string bound_name = std::to_string(reinterpret_cast<intptr_t>(statement));
    Variable *bound = frame.top()->createVariable(Token(), bound_name, TYPE_NUMBER, false);
    scope.top()->addName(Token(IDENTIFIER, ""), bound_name, bound, true);
    // TODO: make loop_id a void*
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3214, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const Statement *> init_statements {
        new AssignmentStatement(statement->token.line, { new VariableExpression(index) }, new ConstantNumberExpression(number_from_uint32(0))),
        new AssignmentStatement(statement->token.line, { new VariableExpression(array_copy) }, array),
        new AssignmentStatement(statement->token.line, { new VariableExpression(bound) }, new FunctionCall(new VariableExpression(atype->methods.at(len_method)), { new VariableExpression(array_copy) })),
    };
    std::vector<const Statement *> statements {
        new IfStatement(
            statement->token.line,
            std::vector<std::pair<const Expression *, std::vector<const Statement *>>> {
                std::make_pair(
                    new NumericComparisonExpression(
                        new VariableExpression(index),
                        new VariableExpression(bound),
                        ComparisonExpression::GE
                    ),
                    std::vector<const Statement *> { new ExitStatement(statement->token.line, loop_id) }
                ),
            },
            std::vector<const Statement *>()
        ),
        new AssignmentStatement(statement->token.line, { new VariableExpression(var) },
            arrtype ? static_cast<Expression *>(new ArrayValueIndexExpression(var->type, new VariableExpression(array_copy), new VariableExpression(index), false)) :
            strtype ? static_cast<Expression *>(new StringValueIndexExpression(new VariableExpression(array_copy), new VariableExpression(index), false, new VariableExpression(index), false, this)) :
            nullptr
        ),
    };
    std::vector<const Statement *> body = analyze(statement->body);
    std::copy(body.begin(), body.end(), std::back_inserter(statements));
    std::vector<const Statement *> tail_statements {
        new IncrementStatement(statement->token.line, new VariableExpression(index), 1),
    };
    scope.pop();
    loops.top().pop_back();
    var->is_readonly = false;
    return new BaseLoopStatement(statement->token.line, loop_id, init_statements, statements, tail_statements, false);
}

const Statement *Analyzer::analyze(const pt::IfStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    for (auto &c: statement->condition_statements) {
        const Expression *cond = nullptr;
        const pt::ValidPointerExpression *valid = dynamic_cast<const pt::ValidPointerExpression *>(c.first.get());
        if (valid != nullptr) {
            for (auto &v: valid->tests) {
                if (not v->shorthand and scope.top()->lookupName(v->name.text) != nullptr) {
                    error2(3102, v->name, "duplicate identifier", scope.top()->getDeclaration(v->name.text), "first declaration here");
                }
                const Expression *ptr = analyze(v->expr.get());
                const TypePointer *ptrtype = dynamic_cast<const TypePointer *>(ptr->type);
                if (ptrtype == nullptr) {
                    error(3101, v->expr->token, "pointer type expression expected");
                }
                const TypeValidPointer *vtype = new TypeValidPointer(Token(), ptrtype->reftype);
                Variable *var;
                // TODO: Try to make this a local variable always (give the global scope a local space).
                if (functiontypes.empty()) {
                    var = new GlobalVariable(v->name, v->name.text, vtype, true);
                } else {
                    // TODO: probably use frame.top()->get_depth() (add IF VALID to repl tests)
                    var = new LocalVariable(v->name, v->name.text, vtype, frame.size()-1, true);
                }
                scope.top()->addName(v->name, v->name.text, var, true, v->shorthand);
                const Expression *ve = new ValidPointerExpression(var, ptr);
                if (cond == nullptr) {
                    cond = ve;
                } else {
                    cond = new ConjunctionExpression(cond, ve);
                }
            }
        } else {
            cond = analyze(c.first.get());
            if (not cond->type->is_assignment_compatible(TYPE_BOOLEAN)) {
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

const Statement *Analyzer::analyze(const pt::IncrementStatement *statement)
{
    const Expression *e = analyze(statement->expr.get());
    if (not e->type->is_assignment_compatible(TYPE_NUMBER)) {
        error(3187, statement->expr->token, "INC and DEC parameter must be Number");
    }
    const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
    if (ref == nullptr) {
        error(3188, statement->expr->token, "INC and DEC call argument must be reference");
    }
    if (ref->is_readonly) {
        error(3189, statement->expr->token, "readonly parameter to INC or DEC");
    }
    return new IncrementStatement(statement->token.line, ref, statement->delta);
}

const Statement *Analyzer::analyze(const pt::LoopStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3215, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
    loops.top().pop_back();
    return new BaseLoopStatement(statement->token.line, loop_id, {}, statements, {}, true);
}

const Statement *Analyzer::analyze(const pt::NextStatement *statement)
{
    std::string type = statement->type.text;
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
        info = analyze(statement->info.get());
    } else {
        std::vector<const Expression *> values;
        info = new RecordLiteralExpression(dynamic_cast<const TypeRecord *>(s->lookupName("ExceptionInfo")->type), values);
    }
    return new RaiseStatement(statement->token.line, exception, info);
}

const Statement *Analyzer::analyze(const pt::RepeatStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3216, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const Statement *> statements = analyze(statement->body);
    const Expression *cond = analyze(statement->cond.get());
    if (not cond->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        error(3086, statement->cond->token, "boolean value expected");
    }
    statements.push_back(
        new IfStatement(
            statement->cond->token.line,
            std::vector<std::pair<const Expression *, std::vector<const Statement *>>> {
                std::make_pair(
                    cond,
                    std::vector<const Statement *> { new ExitStatement(statement->cond->token.line, loop_id) }
                )
            },
            std::vector<const Statement *>()
        )
    );
    scope.pop();
    loops.top().pop_back();
    return new BaseLoopStatement(statement->token.line, loop_id, {}, statements, {}, cond->is_constant && cond->eval_boolean(statement->cond->token) == false);
}

const Statement *Analyzer::analyze(const pt::ReturnStatement *statement)
{
    const Expression *expr = analyze(statement->expr.get());
    if (functiontypes.empty()) {
        error(3093, statement->token, "RETURN not allowed outside function");
    } else if (functiontypes.top().second->returntype == TYPE_NOTHING) {
        error(3094, statement->token, "function does not return a value");
    } else if (not functiontypes.top().second->returntype->is_assignment_compatible(expr->type)) {
        error(3095, statement->expr->token, "type mismatch in RETURN");
    }
    return new ReturnStatement(statement->token.line, expr);
}

std::vector<std::pair<std::vector<const Exception *>, const AstNode *>> Analyzer::analyze_catches(const std::vector<std::pair<std::vector<std::pair<Token, Token>>, std::unique_ptr<pt::ParseTreeNode>>> &catches)
{
    std::vector<std::pair<std::vector<const Exception *>, const AstNode *>> r;
    for (auto &x: catches) {
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
        const pt::TryHandlerStatement *ths = dynamic_cast<const pt::TryHandlerStatement *>(x.second.get());
        const pt::Expression *e = dynamic_cast<const pt::Expression *>(x.second.get());
        if (ths != nullptr) {
            std::vector<const Statement *> statements = analyze(ths->body);
            r.push_back(std::make_pair(exceptions, new ExceptionHandlerStatement(0 /*TODO*/, statements)));
        } else if (e != nullptr) {
            const Expression *g = analyze(e);
            r.push_back(std::make_pair(exceptions, g));
        } else {
            internal_error("unexpected catch type");
        }
        scope.pop();
    }
    return r;
}

const Statement *Analyzer::analyze(const pt::TryStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
    auto catches = analyze_catches(statement->catches);
    return new TryStatement(statement->token.line, statements, catches);
}

const Statement *Analyzer::analyze(const pt::TryHandlerStatement *statement)
{
    scope.push(new Scope(scope.top(), frame.top()));
    std::vector<const Statement *> statements = analyze(statement->body);
    scope.pop();
    return new ExceptionHandlerStatement(statement->token.line, statements);
}

const Statement *Analyzer::analyze(const pt::WhileStatement *statement)
{
    const Expression *cond = analyze(statement->cond.get());
    if (not cond->type->is_assignment_compatible(TYPE_BOOLEAN)) {
        error(3049, statement->cond->token, "boolean value expected");
    }
    scope.push(new Scope(scope.top(), frame.top()));
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3217, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const Statement *> statements {
        new IfStatement(
            statement->token.line,
            std::vector<std::pair<const Expression *, std::vector<const Statement *>>> {
                std::make_pair(
                    new LogicalNotExpression(cond),
                    std::vector<const Statement *> { new ExitStatement(statement->token.line, loop_id) }
                )
            },
            std::vector<const Statement *>()
        )
    };
    std::vector<const Statement *> body = analyze(statement->body);
    std::copy(body.begin(), body.end(), std::back_inserter(statements));
    scope.pop();
    loops.top().pop_back();
    return new BaseLoopStatement(statement->token.line, loop_id, {}, statements, {}, cond->is_constant && cond->eval_boolean(statement->cond->token) == true);
}

// This code attempts to check for every type that is used in an exported
// declaration, and has a name, that it is also exported. There seem to be
// a bunch of corner cases, such as types declared inline (without their own
// name), and types imported from other modules that have no name, that need
// to be handled here. Also, recursive types need to be handled (kept track
// of by the seen set).
//
// All this to try to prevent going recursive in Emitter::get_type_reference
// way over in the compiler.

class ExportedTypeChecker {
public:
    ExportedTypeChecker(const std::set<const Name *> &exported): exported(exported), seen() {}
    void check(const Type *type) {
        if (seen.find(type) != seen.end()) {
            return;
        }
        seen.insert(type);
        if (type == TYPE_NOTHING
         || type == TYPE_BOOLEAN
         || type == TYPE_NUMBER
         || type == TYPE_STRING
         || type == TYPE_BYTES) {
            return;
        }
        const TypeArray *ta = dynamic_cast<const TypeArray *>(type);
        const TypeDictionary *td = dynamic_cast<const TypeDictionary *>(type);
        const TypePointer *tp = dynamic_cast<const TypePointer *>(type);
        if (ta != nullptr) {
            check(ta->elementtype);
            return;
        } else if (td != nullptr) {
            check(td->elementtype);
            return;
        } else if (tp != nullptr) {
            if (tp->reftype != nullptr) {
                check(tp->reftype);
            }
            return;
        }
        if (exported.find(type) == exported.end() && not type->name.empty() && type->declaration.line > 0) {
            error(3211, type->declaration, "dependent type must be exported: " + type->name + ", " + type->text());
        }
        const TypeRecord *tr = dynamic_cast<const TypeRecord *>(type);
        if (tr != nullptr) {
            for (auto &f: tr->fields) {
                check(f.type);
            }
        }
    }
private:
    const std::set<const Name *> &exported;
    std::set<const Name *> seen;
    ExportedTypeChecker &operator=(const ExportedTypeChecker &) = delete;
};

const Program *Analyzer::analyze()
{
    Program *r = new Program(program->source_path, program->source_hash);
    global_scope = r->scope;
    frame.push(r->frame);
    scope.push(global_scope);
    // Create a new scope for the global things in the main program,
    // so that modules can use the real global scope as their parent
    // scope to avoid accidentally linking them up together (issue #30).
    if (external_globals != nullptr) {
        frame.push(new ExternalGlobalFrame(frame.top(), *external_globals));
        scope.push(new ExternalGlobalScope(scope.top(), frame.top(), *external_globals));
        r->frame = frame.top();
        r->scope = scope.top();
    } else {
        scope.push(new Scope(scope.top(), frame.top()));
        r->scope = scope.top();
    }

    //init_builtin_constants(global_scope);

    loops.push(std::list<std::pair<std::string, unsigned int>>());
    r->statements = analyze(program->body);
    loops.pop();
    r->scope->checkForward();
    std::set<const Name *> exported;
    for (auto nt: exports) {
        const Name *name = scope.top()->lookupName(nt.first);
        if (name == nullptr) {
            internal_error("export name not found: " + nt.first);
        }
        if (r->exports.find(nt.first) != r->exports.end()) {
            internal_error("export name already exported");
        }
        if (dynamic_cast<const Type *>(name) == nullptr
         && dynamic_cast<const Exception *>(name) == nullptr
         && dynamic_cast<const GlobalVariable *>(name) == nullptr
         && dynamic_cast<const Constant *>(name) == nullptr
         && dynamic_cast<const Function *>(name) == nullptr
         && dynamic_cast<const PredefinedFunction *>(name) == nullptr) {
            internal_error("Attempt to export something that can't be exported: " + nt.first);
        }
        r->exports[nt.first] = name;
        exported.insert(name);
    }
    ExportedTypeChecker etc(exported);
    for (auto nt: exports) {
        const Name *name = scope.top()->lookupName(nt.first);
        if (dynamic_cast<const Type *>(name) != nullptr) {
            // pass
        } else if (dynamic_cast<const Exception *>(name) != nullptr) {
            // pass
        } else if (dynamic_cast<const GlobalVariable *>(name) != nullptr) {
            etc.check(dynamic_cast<const GlobalVariable *>(name)->type);
        } else if (dynamic_cast<const Constant *>(name) != nullptr) {
            etc.check(dynamic_cast<const Constant *>(name)->type);
        } else if (dynamic_cast<const Function *>(name) != nullptr) {
            const Function *f = dynamic_cast<const Function *>(name);
            for (auto p: f->params) {
                etc.check(p->type);
            }
            etc.check(dynamic_cast<const TypeFunction *>(f->type)->returntype);
        } else if (dynamic_cast<const PredefinedFunction *>(name) != nullptr) {
            // pass
        }
    }
    scope.pop();
    return r;
}

class UninitialisedFinder: public pt::IParseTreeVisitor {
public:
    UninitialisedFinder(): variables(), assigned(), out_parameters(), outer_scope(nullptr) {
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
    }
    UninitialisedFinder(const UninitialisedFinder *outer): variables(), assigned(), out_parameters(), outer_scope(outer) {
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
    }
    virtual void visit(const pt::TypeSimple *) {}
    virtual void visit(const pt::TypeEnum *) {}
    virtual void visit(const pt::TypeRecord *) {}
    virtual void visit(const pt::TypeClass *) {}
    virtual void visit(const pt::TypePointer *) {}
    virtual void visit(const pt::TypeValidPointer *) {}
    virtual void visit(const pt::TypeFunctionPointer *) {}
    virtual void visit(const pt::TypeParameterised *) {}
    virtual void visit(const pt::TypeImport *) {}

    virtual void visit(const pt::DummyExpression *) {}
    virtual void visit(const pt::IdentityExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::BooleanLiteralExpression *) {}
    virtual void visit(const pt::NumberLiteralExpression *) {}
    virtual void visit(const pt::StringLiteralExpression *) {}
    virtual void visit(const pt::FileLiteralExpression *) {}
    virtual void visit(const pt::BytesLiteralExpression *) {}
    virtual void visit(const pt::ArrayLiteralExpression *node) { for (auto &x: node->elements) x->accept(this); }
    virtual void visit(const pt::ArrayLiteralRangeExpression *node) { node->first->accept(this); node->last->accept(this); node->step->accept(this); }
    virtual void visit(const pt::DictionaryLiteralExpression *node) { for (auto &x: node->elements) x.second->accept(this); }
    virtual void visit(const pt::NilLiteralExpression *) {}
    virtual void visit(const pt::NowhereLiteralExpression *) {}
    virtual void visit(const pt::IdentifierExpression *node) {
        for (auto v = variables.rbegin(); v != variables.rend(); ++v) {
            auto i = v->find(node->name);
            if (i != v->end() && not i->second.second) {
                error2(3190, node->token, "Uninitialised variable: " + node->name, i->second.first, "Variable declared here");
            }
        }
    }
    virtual void visit(const pt::DotExpression *node) { node->base->accept(this); }
    virtual void visit(const pt::ArrowExpression *node) { node->base->accept(this); }
    virtual void visit(const pt::SubscriptExpression *node) { node->base->accept(this); node->index->accept(this); }
    virtual void visit(const pt::InterpolatedStringExpression *node) { for (auto &x: node->parts) x.first->accept(this); }
    virtual void visit(const pt::FunctionCallExpression *node) {
        node->base->accept(this);
        for (auto &x: node->args) {
            if (x->mode.type == OUT) {
                const pt::IdentifierExpression *expr = dynamic_cast<const pt::IdentifierExpression *>(x->expr.get());
                if (expr != nullptr) {
                    mark_assigned(expr->name);
                }
            } else {
                x->expr->accept(this);
            }
        }
    }
    virtual void visit(const pt::UnaryPlusExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::UnaryMinusExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::LogicalNotExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::ExponentiationExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::MultiplicationExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::DivisionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::IntegerDivisionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ModuloExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::AdditionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::SubtractionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConcatenationExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ComparisonExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ChainedComparisonExpression *node) { node->left->accept(this); for (auto &x: node->comps) x->right->accept(this); }
    virtual void visit(const pt::MembershipExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConjunctionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::DisjunctionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConditionalExpression *node) { node->cond->accept(this); node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::TryExpression *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::NewClassExpression *) {}
    virtual void visit(const pt::ValidPointerExpression *node) { for (auto &x: node->tests) x->expr->accept(this); }
    virtual void visit(const pt::RangeSubscriptExpression *node) { node->base->accept(this); node->range->get_first()->accept(this); node->range->get_last()->accept(this); }

    virtual void visit(const pt::ImportDeclaration *) {}
    virtual void visit(const pt::TypeDeclaration *) {}
    virtual void visit(const pt::ConstantDeclaration *) {}
    virtual void visit(const pt::NativeConstantDeclaration *) {}
    virtual void visit(const pt::VariableDeclaration *node) {
        if (node->value != nullptr) {
            node->value->accept(this);
        } else {
            for (auto name: node->names) {
                variables.back()[name.text] = std::make_pair(name, false);
            }
        }
    }
    virtual void visit(const pt::NativeVariableDeclaration *) {}
    virtual void visit(const pt::LetDeclaration *node) {
        node->value->accept(this);
    }
    virtual void visit(const pt::FunctionDeclaration *node) {
        UninitialisedFinder uf;
        for (auto &a: node->args) {
            if (a->mode == pt::FunctionParameterGroup::OUT) {
                for (auto name: a->names) {
                    uf.variables.back()[name.text] = std::make_pair(name, false);
                    uf.out_parameters.push_back(name.text);
                }
            }
        }
        for (auto &s: node->body) {
            s->accept(&uf);
        }
        uf.check_out_parameters(node->end_function);
    }
    virtual void visit(const pt::ExternalFunctionDeclaration *) {}
    virtual void visit(const pt::NativeFunctionDeclaration *) {}
    virtual void visit(const pt::ExceptionDeclaration *) {}
    virtual void visit(const pt::ExportDeclaration *node) {
        if (node->declaration != nullptr) {
            node->declaration->accept(this);
        }
    }

    virtual void visit(const pt::AssertStatement *node) { node->exprs[0]->accept(this); }
    virtual void visit(const pt::AssignmentStatement *node) {
        node->expr->accept(this);
        for (auto &x: node->variables) {
            const pt::IdentifierExpression *expr = dynamic_cast<const pt::IdentifierExpression *>(x.get());
            if (expr != nullptr) {
                mark_assigned(expr->name);
            }
        }
    }
    virtual void visit(const pt::CaseStatement *node) {
        node->expr->accept(this);
        std::set<std::string> assigned;
        bool first = true;
        for (auto &c: node->clauses) {
            for (auto &w: c.first) {
                auto *cwc = dynamic_cast<const pt::CaseStatement::ComparisonWhenCondition *>(w.get());
                auto *rwc = dynamic_cast<const pt::CaseStatement::RangeWhenCondition *>(w.get());
                if (cwc != nullptr) {
                    cwc->expr->accept(this);
                } else if (rwc != nullptr) {
                    rwc->low_expr->accept(this);
                    rwc->high_expr->accept(this);
                }
            }
            UninitialisedFinder uf(this);
            for (auto &s: c.second) {
                s->accept(&uf);
            }
            if (first) {
                assigned = uf.assigned;
                first = false;
            } else {
                intersect(assigned, uf.assigned);
            }
        }
        for (auto a: assigned) {
            mark_assigned(a);
        }
    }
    virtual void visit(const pt::CheckStatement *node) {
        node->cond->accept(this);
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::ExecStatement *node) {
        for (auto &var: node->info->assignments) {
            mark_assigned(var);
        }
    }
    virtual void visit(const pt::ExitStatement *node) {
        if (node->type.type == FUNCTION) {
            check_out_parameters(node->token);
        }
    }
    virtual void visit(const pt::ExpressionStatement *node) { node->expr->accept(this); }
    virtual void visit(const pt::ForStatement *node) {
        node->start->accept(this);
        node->end->accept(this);
        if (node->step != nullptr) {
            node->step->accept(this);
        }
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::ForeachStatement *node) {
        node->array->accept(this);
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::IfStatement *node) {
        std::set<std::string> assigned;
        bool first = true;
        for (auto &x: node->condition_statements) {
            x.first->accept(this);
            UninitialisedFinder uf(this);
            for (auto &s: x.second) {
                s->accept(&uf);
            }
            if (first) {
                assigned = uf.assigned;
                first = false;
            } else {
                intersect(assigned, uf.assigned);
            }
        }
        UninitialisedFinder uf(this);
        for (auto &s: node->else_statements) {
            s->accept(&uf);
        }
        intersect(assigned, uf.assigned);
        for (auto a: assigned) {
            mark_assigned(a);
        }
    }
    virtual void visit(const pt::IncrementStatement *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::LoopStatement *node) {
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::NextStatement *) {}
    virtual void visit(const pt::RaiseStatement *) {}
    virtual void visit(const pt::RepeatStatement *node) {
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::ReturnStatement *node) {
        node->expr->accept(this);
        check_out_parameters(node->token);
    }
    virtual void visit(const pt::TryStatement *node) {
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::TryHandlerStatement *node) {
        for (auto &s: node->body) {
            s->accept(this);
        }
    }
    virtual void visit(const pt::WhileStatement *node) {
        node->cond->accept(this);
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
        for (auto &s: node->body) {
            s->accept(this);
        }
        variables.pop_back();
    }
    virtual void visit(const pt::Program *node) {
        for (auto &s: node->body) {
            s->accept(this);
        }
    }
private:
    std::list<std::map<std::string, std::pair<Token, bool>>> variables;
    std::set<std::string> assigned;
    std::vector<std::string> out_parameters;
    const UninitialisedFinder *outer_scope;

    void mark_assigned(const std::string &name) {
        for (auto v = variables.rbegin(); v != variables.rend(); ++v) {
            auto i = v->find(name);
            if (i != v->end()) {
                i->second.second = true;
                break;
            }
        }
        assigned.insert(name);
    }

    void check_out_parameters(const Token &token)
    {
        const UninitialisedFinder *top_finder = this;
        while (top_finder->outer_scope != nullptr) {
            top_finder = top_finder->outer_scope;
        }
        for (auto p: top_finder->out_parameters) {
            for (const UninitialisedFinder *uf = this; uf != nullptr; uf = uf->outer_scope) {
                if (uf->assigned.find(p) != uf->assigned.end()) {
                    break;
                }
                for (auto v = uf->variables.rbegin(); v != uf->variables.rend(); ++v) {
                    auto i = v->find(p);
                    if (i != v->end()) {
                        if (i->second.second) {
                            uf = nullptr;
                            break;
                        } else {
                            error2(3191, token, "Uninitialised OUT parameter: " + p, i->second.first, "Variable declared here");
                        }
                    }
                }
            }
        }
    }

    template <typename T> void intersect(std::set<T> &lhs, const std::set<T> &rhs) {
        for (auto i = lhs.begin(); i != lhs.end(); ) {
            if (rhs.find(*i) == rhs.end()) {
                auto tmp = i;
                ++i;
                lhs.erase(tmp);
            } else {
                ++i;
            }
        }
    }

private:
    UninitialisedFinder(const UninitialisedFinder &);
    UninitialisedFinder &operator=(const UninitialisedFinder &);
};

class UnusedFinder: public pt::IParseTreeVisitor {
public:
    UnusedFinder(): variables() {
        enter();
    }
    virtual void visit(const pt::TypeSimple *) {}
    virtual void visit(const pt::TypeEnum *) {}
    virtual void visit(const pt::TypeRecord *) {}
    virtual void visit(const pt::TypeClass *) {}
    virtual void visit(const pt::TypePointer *) {}
    virtual void visit(const pt::TypeValidPointer *) {}
    virtual void visit(const pt::TypeFunctionPointer *) {}
    virtual void visit(const pt::TypeParameterised *) {}
    virtual void visit(const pt::TypeImport *) {}

    virtual void visit(const pt::DummyExpression *) {}
    virtual void visit(const pt::IdentityExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::BooleanLiteralExpression *) {}
    virtual void visit(const pt::NumberLiteralExpression *) {}
    virtual void visit(const pt::StringLiteralExpression *) {}
    virtual void visit(const pt::FileLiteralExpression *) {}
    virtual void visit(const pt::BytesLiteralExpression *) {}
    virtual void visit(const pt::ArrayLiteralExpression *node) { for (auto &x: node->elements) x->accept(this); }
    virtual void visit(const pt::ArrayLiteralRangeExpression *node) { node->first->accept(this); node->last->accept(this); node->step->accept(this); }
    virtual void visit(const pt::DictionaryLiteralExpression *node) { for (auto &x: node->elements) x.second->accept(this); }
    virtual void visit(const pt::NilLiteralExpression *) {}
    virtual void visit(const pt::NowhereLiteralExpression *) {}
    virtual void visit(const pt::IdentifierExpression *node) {
        for (auto v = variables.rbegin(); v != variables.rend(); ++v) {
            auto i = v->find(node->name);
            if (i != v->end()) {
                i->second.second = true;
                break;
            }
        }
    }
    virtual void visit(const pt::DotExpression *node) { node->base->accept(this); }
    virtual void visit(const pt::ArrowExpression *node) { node->base->accept(this); }
    virtual void visit(const pt::SubscriptExpression *node) { node->base->accept(this); node->index->accept(this); }
    virtual void visit(const pt::InterpolatedStringExpression *node) { for (auto &x: node->parts) x.first->accept(this); }
    virtual void visit(const pt::FunctionCallExpression *node) {
        node->base->accept(this);
        for (auto &x: node->args) {
            if (x->mode.type != OUT) {
                x->expr->accept(this);
            }
        }
    }
    virtual void visit(const pt::UnaryPlusExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::UnaryMinusExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::LogicalNotExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::ExponentiationExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::MultiplicationExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::DivisionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::IntegerDivisionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ModuloExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::AdditionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::SubtractionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConcatenationExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ComparisonExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ChainedComparisonExpression *node) { node->left->accept(this); for (auto &x: node->comps) x->right->accept(this); }
    virtual void visit(const pt::MembershipExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConjunctionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::DisjunctionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConditionalExpression *node) { node->cond->accept(this); node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::TryExpression *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::NewClassExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::ValidPointerExpression *node) { for (auto &x: node->tests) x->expr->accept(this); }
    virtual void visit(const pt::RangeSubscriptExpression *node) { node->base->accept(this); node->range->get_first()->accept(this); node->range->get_last()->accept(this); }

    virtual void visit(const pt::ImportDeclaration *) {}
    virtual void visit(const pt::TypeDeclaration *) {}
    virtual void visit(const pt::ConstantDeclaration *) {}
    virtual void visit(const pt::NativeConstantDeclaration *) {}
    virtual void visit(const pt::VariableDeclaration *node) {
        if (node->value != nullptr) {
            node->value->accept(this);
        }
        for (auto name: node->names) {
            variables.back()[name.text] = std::make_pair(name, false);
        }
    }
    virtual void visit(const pt::NativeVariableDeclaration *) {}
    virtual void visit(const pt::LetDeclaration *node) {
        node->value->accept(this);
        variables.back()[node->name.text] = std::make_pair(node->name, false);
    }
    virtual void visit(const pt::FunctionDeclaration *node) {
        UnusedFinder uf;
        for (auto &s: node->body) {
            s->accept(&uf);
        }
        uf.check();
    }
    virtual void visit(const pt::ExternalFunctionDeclaration *) {}
    virtual void visit(const pt::NativeFunctionDeclaration *) {}
    virtual void visit(const pt::ExceptionDeclaration *) {}
    virtual void visit(const pt::ExportDeclaration *node) {
        if (node->declaration != nullptr) {
            node->declaration->accept(this);
        }
    }

    virtual void visit(const pt::AssertStatement *node) { node->exprs[0]->accept(this); }
    virtual void visit(const pt::AssignmentStatement *node) {
        node->expr->accept(this);
        for (auto &x: node->variables) {
            const pt::IdentifierExpression *expr = dynamic_cast<const pt::IdentifierExpression *>(x.get());
            if (expr != nullptr) {
                for (auto v = variables.rbegin(); v != variables.rend(); ++v) {
                    auto i = v->find(expr->name);
                    if (i != v->end()) {
                        i->second.second = true;
                        break;
                    }
                }
            } else {
                x->accept(this);
            }
        }
    }
    virtual void visit(const pt::CaseStatement *node) {
        node->expr->accept(this);
        for (auto &c: node->clauses) {
            for (auto &w: c.first) {
                auto *cwc = dynamic_cast<const pt::CaseStatement::ComparisonWhenCondition *>(w.get());
                auto *rwc = dynamic_cast<const pt::CaseStatement::RangeWhenCondition *>(w.get());
                if (cwc != nullptr) {
                    cwc->expr->accept(this);
                } else if (rwc != nullptr) {
                    rwc->low_expr->accept(this);
                    rwc->high_expr->accept(this);
                }
            }
            enter();
            for (auto &s: c.second) {
                s->accept(this);
            }
            leave();
        }
    }
    virtual void visit(const pt::CheckStatement *node) {
        node->cond->accept(this);
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::ExecStatement *node) {
        for (auto p: node->info->parameters) {
            for (auto v = variables.rbegin(); v != variables.rend(); ++v) {
                auto i = v->find(p.substr(1));
                if (i != v->end()) {
                    i->second.second = true;
                    break;
                }
            }
        }
    }
    virtual void visit(const pt::ExitStatement *) { }
    virtual void visit(const pt::ExpressionStatement *node) { node->expr->accept(this); }
    virtual void visit(const pt::ForStatement *node) {
        node->start->accept(this);
        node->end->accept(this);
        if (node->step != nullptr) {
            node->step->accept(this);
        }
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::ForeachStatement *node) {
        node->array->accept(this);
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::IfStatement *node) {
        for (auto &x: node->condition_statements) {
            x.first->accept(this);
            enter();
            for (auto &s: x.second) {
                s->accept(this);
            }
            leave();
        }
        enter();
        for (auto &s: node->else_statements) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::IncrementStatement *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::LoopStatement *node) {
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::NextStatement *) {}
    virtual void visit(const pt::RaiseStatement *) {}
    virtual void visit(const pt::RepeatStatement *node) {
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        node->cond->accept(this);
        leave();
    }
    virtual void visit(const pt::ReturnStatement *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::TryStatement *node) {
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::TryHandlerStatement *node) {
        for (auto &s: node->body) {
            s->accept(this);
        }
    }
    virtual void visit(const pt::WhileStatement *node) {
        node->cond->accept(this);
        enter();
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::Program *node) {
        for (auto &s: node->body) {
            s->accept(this);
        }
    }
private:
    std::list<std::map<std::string, std::pair<Token, bool>>> variables;

    void enter() {
        variables.push_back(std::map<std::string, std::pair<Token, bool>>());
    }
    void leave() {
        check();
        variables.pop_back();
    }
    void check() {
        for (auto &name: variables.back()) {
            if (not name.second.second) {
                error(3205, name.second.first, "Unused variable");
            }
        }
    }

private:
    UnusedFinder(const UnusedFinder &);
    UnusedFinder &operator=(const UnusedFinder &);
};

const Program *analyze(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ExternalGlobalInfo> *external_globals)
{
    const Program *r = Analyzer(support, program, external_globals).analyze();

    // Find uninitalised variables.
    UninitialisedFinder uf;
    program->accept(&uf);

    // Find unused variables.
    UnusedFinder unf;
    program->accept(&unf);

    // Skip this if we're in the repl.
    if (external_globals == nullptr) {
        // Find unused imports.
        for (size_t i = 0; i < r->frame->getCount(); i++) {
            Frame::Slot s = r->frame->getSlot(i);
            if (dynamic_cast<Module *>(s.ref) != nullptr && not s.referenced) {
                error(3192, s.token, "Unused import");
            }
        }
    }

    return r;
}
