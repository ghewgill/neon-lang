#include "analyzer.h"

#include <algorithm>
#include <fstream>
#include <iso646.h>
#include <list>
#include <sstream>
#include <stack>

#include "ast.h"
#include "bytecode.h"
#include "lexer.h"
#include "pt.h"
#include "rtl_compile.h"
#include "support.h"
#include "util.h"

static const ast::Expression *identity_conversion(Analyzer *, const ast::Expression *e) { return e; }

class Analyzer {
public:
    Analyzer(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ast::ExternalGlobalInfo> *external_globals);
    Analyzer(const Analyzer &) = delete;
    Analyzer &operator=(const Analyzer &) = delete;

    ICompilerSupport *support;
    const pt::Program *program;
    std::map<std::string, ast::ExternalGlobalInfo> *const external_globals;
    const std::string module_name;
    std::map<std::string, ast::Module *> modules;
    ast::Scope *global_scope;
    std::stack<ast::Frame *> frame;
    std::stack<ast::Scope *> scope;
    std::map<std::string, Token> exports;

    std::stack<std::pair<const ast::Type *, const ast::TypeFunction *>> functiontypes;
    std::stack<std::list<std::pair<std::string, unsigned int>>> loops;
    std::stack<std::set<std::string>> imported_checked_stack;

    const ast::Type *analyze(const pt::Type *type, const std::string &name = std::string());
    const ast::Type *analyze(const pt::TypeSimple *type, const std::string &name);
    const ast::Type *analyze_enum(const pt::TypeEnum *type, const std::string &name);
    const ast::Type *analyze_record(const pt::TypeRecord *type, const std::string &name);
    const ast::Type *analyze_class(const pt::TypeClass *type, const std::string &name);
    const ast::Type *analyze(const pt::TypePointer *type, const std::string &name);
    const ast::Type *analyze(const pt::TypeValidPointer *type, const std::string &name);
    const ast::Type *analyze(const pt::TypeFunctionPointer *type, const std::string &name);
    const ast::Type *analyze(const pt::TypeParameterised *type, const std::string &name);
    const ast::Type *analyze(const pt::TypeImport *type, const std::string &name);
    const ast::Expression *analyze(const pt::Expression *expr);
    const ast::Expression *analyze(const pt::DummyExpression *expr);
    const ast::Expression *analyze(const pt::IdentityExpression *expr);
    const ast::Expression *analyze(const pt::BooleanLiteralExpression *expr);
    const ast::Expression *analyze(const pt::NumberLiteralExpression *expr);
    const ast::Expression *analyze(const pt::StringLiteralExpression *expr);
    const ast::Expression *analyze(const pt::FileLiteralExpression *expr);
    const ast::Expression *analyze(const pt::BytesLiteralExpression *expr);
    const ast::Expression *analyze(const pt::ArrayLiteralExpression *expr);
    const ast::Expression *analyze(const pt::ArrayLiteralRangeExpression *expr);
    const ast::Expression *analyze(const pt::DictionaryLiteralExpression *expr);
    const ast::Expression *analyze(const pt::NilLiteralExpression *expr);
    const ast::Expression *analyze(const pt::NowhereLiteralExpression *expr);
    const ast::Expression *analyze(const pt::IdentifierExpression *expr);
    const ast::Name *analyze_qualified_name(const pt::Expression *expr);
    const ast::Expression *analyze(const pt::DotExpression *expr);
    const ast::Expression *analyze(const pt::ArrowExpression *expr);
    const ast::Expression *analyze(const pt::SubscriptExpression *expr);
    const ast::Expression *analyze(const pt::InterpolatedStringExpression *expr);
    const ast::Expression *analyze(const pt::FunctionCallExpression *expr);
    const ast::Expression *analyze(const pt::UnaryPlusExpression *expr);
    const ast::Expression *analyze(const pt::UnaryMinusExpression *expr);
    const ast::Expression *analyze(const pt::LogicalNotExpression *expr);
    const ast::Expression *analyze(const pt::ExponentiationExpression *expr);
    const ast::Expression *analyze(const pt::MultiplicationExpression *expr);
    const ast::Expression *analyze(const pt::DivisionExpression *expr);
    const ast::Expression *analyze(const pt::IntegerDivisionExpression *expr);
    const ast::Expression *analyze(const pt::ModuloExpression *expr);
    const ast::Expression *analyze(const pt::AdditionExpression *expr);
    const ast::Expression *analyze(const pt::SubtractionExpression *expr);
    const ast::Expression *analyze(const pt::ConcatenationExpression *expr);
    const ast::Expression *analyze(const pt::ComparisonExpression *expr);
    const ast::Expression *analyze(const pt::ChainedComparisonExpression *expr);
    const ast::Expression *analyze(const pt::TypeTestExpression *expr);
    const ast::Expression *analyze(const pt::MembershipExpression *expr);
    const ast::Expression *analyze(const pt::ConjunctionExpression *expr);
    const ast::Expression *analyze(const pt::DisjunctionExpression *expr);
    const ast::Expression *analyze(const pt::ConditionalExpression *expr);
    const ast::Expression *analyze(const pt::TryExpression *expr);
    const ast::Expression *analyze(const pt::NewClassExpression *expr);
    const ast::Expression *analyze(const pt::ValidPointerExpression *expr);
    const ast::Expression *analyze(const pt::ImportedModuleExpression *expr);
    const ast::Expression *analyze(const pt::RangeSubscriptExpression *expr);
    const ast::Statement *analyze(const pt::ImportDeclaration *declaration);
    const ast::Statement *analyze(const pt::TypeDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::ConstantDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::ConstantDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::NativeConstantDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::NativeConstantDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::ExtensionConstantDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::ExtensionConstantDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::VariableDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::VariableDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::NativeVariableDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::NativeVariableDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::LetDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::LetDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::FunctionDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::FunctionDeclaration *declaration);
    const ast::Statement *analyze(const pt::NativeFunctionDeclaration *declaration);
    const ast::Statement *analyze(const pt::ExtensionFunctionDeclaration *declaration);
    const ast::Statement *analyze(const pt::ExceptionDeclaration *declaration);
    const ast::Statement *analyze(const pt::InterfaceDeclaration *declaration);
    const ast::Statement *analyze_decl(const pt::ExportDeclaration *declaration);
    const ast::Statement *analyze_body(const pt::ExportDeclaration *declaration);
    std::vector<const ast::Statement *> analyze(const std::vector<std::unique_ptr<pt::Statement>> &statement);
    const ast::Statement *analyze(const pt::AssertStatement *statement);
    const ast::Statement *analyze(const pt::AssignmentStatement *statement);
    const ast::Statement *analyze(const pt::CaseStatement *statement);
    const ast::Statement *analyze(const pt::CheckStatement *statement);
    const ast::Statement *analyze(const pt::ExecStatement *statement);
    const ast::Statement *analyze(const pt::ExitStatement *statement);
    const ast::Statement *analyze(const pt::ExpressionStatement *statement);
    const ast::Statement *analyze(const pt::ForStatement *statement);
    const ast::Statement *analyze(const pt::ForeachStatement *statement);
    const ast::Statement *analyze(const pt::IfStatement *statement);
    const ast::Statement *analyze(const pt::IncrementStatement *statement);
    const ast::Statement *analyze(const pt::LoopStatement *statement);
    const ast::Statement *analyze(const pt::NextStatement *statement);
    const ast::Statement *analyze(const pt::RaiseStatement *statement);
    const ast::Statement *analyze(const pt::RepeatStatement *statement);
    const ast::Statement *analyze(const pt::ReturnStatement *statement);
    const ast::Statement *analyze(const pt::TestCaseStatement *statement);
    const ast::Statement *analyze(const pt::TryStatement *statement);
    const ast::Statement *analyze(const pt::TryHandlerStatement *statement);
    const ast::Statement *analyze(const pt::UnusedStatement *statement);
    const ast::Statement *analyze(const pt::WhileStatement *statement);
    const ast::Program *analyze();
private:
    static std::string extract_module_name(const pt::Program *program);
    ast::Module *import_module(const Token &token, const std::string &name, bool optional);
    ast::Type *deserialize_type(ast::Scope *s, const std::string &descriptor, std::string::size_type &i);
    ast::Type *deserialize_type(ast::Scope *s, const std::string &descriptor);
    const ast::Expression *convert(const ast::Type *target, const ast::Expression *e);
    bool convert2(const ast::Type *target, const ast::Expression *&left, const ast::Expression *&right);
    const ast::TypeFunction *analyze_function_type(const std::unique_ptr<pt::Type> &returntype, const std::vector<std::unique_ptr<pt::FunctionParameterGroup>> &args);
    const ast::Exception *resolve_exception(const std::vector<Token> &names);
    std::vector<ast::TryTrap> analyze_catches(const std::vector<std::unique_ptr<pt::TryTrap>> &catches, const ast::Type *expression_match_type);
    void process_into_results(const pt::ExecStatement *statement, const std::string &sql, const ast::Variable *function, std::vector<const ast::Expression *> args, std::vector<const ast::Statement *> &statements);
    std::vector<ast::TypeRecord::Field> analyze_fields(const pt::TypeRecord *type, bool for_class);
    ast::ComparisonExpression *analyze_comparison(const Token &token, const ast::Expression *left, ast::ComparisonExpression::Comparison comp, const ast::Expression *right);
};

class TypeAnalyzer: public pt::IParseTreeVisitor {
public:
    TypeAnalyzer(Analyzer *a, const std::string &name): type(nullptr), a(a), name(name) {}
    TypeAnalyzer(const TypeAnalyzer &) = delete;
    TypeAnalyzer &operator=(const TypeAnalyzer &) = delete;
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
    virtual void visit(const pt::TypeTestExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::TryExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewClassExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportedModuleExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExtensionConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeVariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExtensionFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::InterfaceDeclaration *) override { internal_error("pt::Declaration"); }
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
    virtual void visit(const pt::TestCaseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryHandlerStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::UnusedStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::WhileStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
    const ast::Type *type;
private:
    Analyzer *a;
    const std::string name;
};

class ExpressionAnalyzer: public pt::IParseTreeVisitor {
public:
    explicit ExpressionAnalyzer(Analyzer *a): expr(nullptr), a(a) {}
    ExpressionAnalyzer(const ExpressionAnalyzer &) = delete;
    ExpressionAnalyzer &operator=(const ExpressionAnalyzer &) = delete;
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
    virtual void visit(const pt::TypeTestExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::MembershipExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ConjunctionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::DisjunctionExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ConditionalExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::TryExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::NewClassExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ValidPointerExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ImportedModuleExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::RangeSubscriptExpression *p) override { expr = a->analyze(p); }
    virtual void visit(const pt::ImportDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExtensionConstantDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeVariableDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::LetDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::FunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExtensionFunctionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ExceptionDeclaration *) override { internal_error("pt::Declaration"); }
    virtual void visit(const pt::InterfaceDeclaration *) override { internal_error("pt::Declaration"); }
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
    virtual void visit(const pt::TestCaseStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::TryHandlerStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::UnusedStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::WhileStatement *) override { internal_error("pt::Statement"); }
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
    const ast::Expression *expr;
private:
    Analyzer *a;
};

class DeclarationAnalyzer: public pt::IParseTreeVisitor {
public:
    DeclarationAnalyzer(Analyzer *a, std::vector<const ast::Statement *> &v): a(a), v(v) {}
    DeclarationAnalyzer(const DeclarationAnalyzer &) = delete;
    DeclarationAnalyzer &operator=(const DeclarationAnalyzer &) = delete;
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
    virtual void visit(const pt::TypeTestExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::TryExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewClassExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportedModuleExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TypeDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ConstantDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeConstantDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::ExtensionConstantDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::VariableDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeVariableDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::LetDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) override { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::NativeFunctionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExtensionFunctionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ExceptionDeclaration *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::InterfaceDeclaration *p) override { v.push_back(a->analyze(p)); }
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
    virtual void visit(const pt::TestCaseStatement *) override {}
    virtual void visit(const pt::TryStatement *) override {}
    virtual void visit(const pt::TryHandlerStatement *) override {}
    virtual void visit(const pt::UnusedStatement *) override {}
    virtual void visit(const pt::WhileStatement *) override {}
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
private:
    Analyzer *a;
    std::vector<const ast::Statement *> &v;
};

class StatementAnalyzer: public pt::IParseTreeVisitor {
public:
    StatementAnalyzer(Analyzer *a, std::vector<const ast::Statement *> &v): a(a), v(v) {}
    StatementAnalyzer(const StatementAnalyzer &) = delete;
    StatementAnalyzer &operator=(const StatementAnalyzer &) = delete;
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
    virtual void visit(const pt::TypeTestExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::MembershipExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::DisjunctionExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ConditionalExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::TryExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::NewClassExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ValidPointerExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportedModuleExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::RangeSubscriptExpression *) override { internal_error("pt::Expression"); }
    virtual void visit(const pt::ImportDeclaration *) override {}
    virtual void visit(const pt::TypeDeclaration *) override {}
    virtual void visit(const pt::ConstantDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeConstantDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::ExtensionConstantDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::VariableDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeVariableDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::LetDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) override { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::NativeFunctionDeclaration *) override {}
    virtual void visit(const pt::ExtensionFunctionDeclaration *) override {}
    virtual void visit(const pt::ExceptionDeclaration *) override {}
    virtual void visit(const pt::InterfaceDeclaration *) override {}
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
    virtual void visit(const pt::TestCaseStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TryStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TryHandlerStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::UnusedStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::WhileStatement *p) override { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::Program *) override { internal_error("pt::Program"); }
private:
    Analyzer *a;
    std::vector<const ast::Statement *> &v;
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

static const ast::Expression *make_array_conversion(Analyzer *analyzer, const ast::TypeArray *from_type, const ast::Expression *from, const ast::TypeArray *to_type)
{
    if (from_type->elementtype == nullptr) {
        const ast::TypeArrayLiteral *tal = dynamic_cast<const ast::TypeArrayLiteral *>(from_type);
        if (tal == nullptr) {
            internal_error("TypeArrayLiteral expected");
        }
        if (not tal->elements.empty()) {
            internal_error("empty TypeArrayLiteral expected");
        }
        return from;
    }
    auto element_converter = to_type->elementtype->make_converter(from_type->elementtype);
    if (element_converter == nullptr) {
        internal_error("make_array_conversion: cannot convert " + from_type->elementtype->text() + " to " + to_type->elementtype->text());
    }
    ast::Variable *result = analyzer->scope.top()->makeTemporary(to_type);
    ast::Variable *array_copy = analyzer->scope.top()->makeTemporary(from_type);
    ast::Variable *index = analyzer->scope.top()->makeTemporary(ast::TYPE_NUMBER);
    ast::Variable *bound = analyzer->scope.top()->makeTemporary(ast::TYPE_NUMBER);
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(from));
    return new ast::StatementExpression(
        new ast::BaseLoopStatement(
            0,
            loop_id,
            // prologue
            {
                new ast::AssignmentStatement(0, { new ast::VariableExpression(result) }, new ast::ArrayLiteralExpression(nullptr, {}, {})),
                new ast::AssignmentStatement(0, { new ast::VariableExpression(index) }, new ast::ConstantNumberExpression(number_from_uint32(0))),
                new ast::AssignmentStatement(0, { new ast::VariableExpression(array_copy) }, from),
                new ast::AssignmentStatement(0, { new ast::VariableExpression(bound) }, new ast::FunctionCall(new ast::VariableExpression(ast::TYPE_ARRAY_STRING->methods.at("size")), { new ast::VariableExpression(array_copy) })),
            },
            // statements
            {
                new ast::IfStatement(
                    0,
                    {
                        std::make_pair(
                            new ast::NumericComparisonExpression(
                                new ast::VariableExpression(index),
                                new ast::VariableExpression(bound),
                                ast::ComparisonExpression::Comparison::GE
                            ),
                            std::vector<const ast::Statement *> { new ast::ExitStatement(0, loop_id) }
                        )
                    },
                    {}
                ),
                new ast::ExpressionStatement(
                    0,
                    new ast::FunctionCall(
                        new ast::VariableExpression(result->type->methods.at("append")),
                        {
                            new ast::VariableExpression(result),
                            element_converter(
                                analyzer,
                                new ast::ArrayValueIndexExpression(
                                    from_type->elementtype,
                                    new ast::VariableExpression(array_copy),
                                    new ast::VariableExpression(index),
                                    false
                                )
                            )
                        }
                    )
                )
            },
            // tail
            {
                new ast::IncrementStatement(0, new ast::VariableExpression(index), 1)
            },
            false
        ),
        new ast::VariableExpression(result)
    );
}

static const ast::Expression *make_dictionary_conversion(Analyzer *analyzer, const ast::TypeDictionary *from_type, const ast::Expression *from, const ast::TypeDictionary *to_type)
{
    if (from_type->elementtype == nullptr) {
        const ast::TypeDictionaryLiteral *tal = dynamic_cast<const ast::TypeDictionaryLiteral *>(from_type);
        if (tal == nullptr) {
            internal_error("TypeDictionaryLiteral expected");
        }
        if (not tal->elements.empty()) {
            internal_error("empty TypeDictionaryLiteral expected");
        }
        return from;
    }
    auto element_converter = to_type->elementtype->make_converter(from_type->elementtype);
    if (element_converter == nullptr) {
        internal_error("make_dictionary_conversion: cannot convert " + from_type->elementtype->text() + " to " + to_type->elementtype->text());
    }
    ast::Variable *result = analyzer->scope.top()->makeTemporary(to_type);
    ast::Variable *keys = analyzer->scope.top()->makeTemporary(ast::TYPE_ARRAY_STRING);
    ast::Variable *index = analyzer->scope.top()->makeTemporary(ast::TYPE_NUMBER);
    ast::Variable *bound = analyzer->scope.top()->makeTemporary(ast::TYPE_NUMBER);
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(from));
    return new ast::StatementExpression(
        new ast::BaseLoopStatement(
            0,
            loop_id,
            // prologue
            {
                new ast::AssignmentStatement(0, { new ast::VariableExpression(result) }, new ast::DictionaryLiteralExpression(nullptr, {}, {})),
                new ast::AssignmentStatement(0, { new ast::VariableExpression(index) }, new ast::ConstantNumberExpression(number_from_uint32(0))),
                new ast::AssignmentStatement(0, { new ast::VariableExpression(keys) }, new ast::FunctionCall(new ast::VariableExpression(from->type->methods.at("keys")), { from })),
                new ast::AssignmentStatement(0, { new ast::VariableExpression(bound) }, new ast::FunctionCall(new ast::VariableExpression(ast::TYPE_ARRAY_STRING->methods.at("size")), { new ast::VariableExpression(keys) })),
            },
            // statements
            {
                new ast::IfStatement(
                    0,
                    {
                        std::make_pair(
                            new ast::NumericComparisonExpression(
                                new ast::VariableExpression(index),
                                new ast::VariableExpression(bound),
                                ast::ComparisonExpression::Comparison::GE
                            ),
                            std::vector<const ast::Statement *> { new ast::ExitStatement(0, loop_id) }
                        )
                    },
                    {}
                ),
                new ast::AssignmentStatement(
                    0,
                    {new ast::DictionaryReferenceIndexExpression(
                        to_type->elementtype,
                        new ast::VariableExpression(result),
                        new ast::ArrayValueIndexExpression(
                            ast::TYPE_STRING,
                            new ast::VariableExpression(keys),
                            new ast::VariableExpression(index),
                            false
                        )
                    )},
                    element_converter(
                        analyzer,
                        new ast::DictionaryValueIndexExpression(
                            from_type->elementtype,
                            from,
                            new ast::ArrayValueIndexExpression(
                                ast::TYPE_STRING,
                                new ast::VariableExpression(keys),
                                new ast::VariableExpression(index),
                                false
                            )
                        )
                    )
                )
            },
            // tail
            {
                new ast::IncrementStatement(0, new ast::VariableExpression(index), 1)
            },
            false
        ),
        new ast::VariableExpression(result)
    );
}

static const ast::Expression *make_object_conversion_from_record(Analyzer *analyzer, const ast::TypeRecord *rtype, const ast::Expression *e)
{
    ast::Variable *result = analyzer->scope.top()->makeTemporary(ast::TYPE_DICTIONARY_OBJECT);
    std::vector<const ast::Statement *> field_statements;
    for (auto &f: rtype->fields) {
        auto converter = ast::TYPE_OBJECT->make_converter(f.type);
        field_statements.push_back(
            new ast::AssignmentStatement(
                0,
                {new ast::DictionaryReferenceIndexExpression(
                    ast::TYPE_OBJECT,
                    new ast::VariableExpression(result),
                    new ast::ConstantStringExpression(utf8string(f.name.text))
                )},
                converter(
                    analyzer,
                    new ast::RecordValueFieldExpression(
                        f.type,
                        e,
                        f.name.text,
                        false
                    )
                )
            )
        );
    }
    return new ast::StatementExpression(
        new ast::CompoundStatement(0, field_statements),
        new ast::FunctionCall(
            new ast::VariableExpression(dynamic_cast<const ast::Variable *>(analyzer->global_scope->lookupName("object__makeDictionary"))),
            {new ast::VariableExpression(result)}
        )
    );
}

static const ast::Expression *make_record_conversion_from_object(Analyzer *analyzer, const ast::TypeRecord *rtype, const ast::Expression *e)
{
    ast::Variable *result = analyzer->scope.top()->makeTemporary(rtype);
    std::vector<const ast::Statement *> field_statements;
    for (auto &f: rtype->fields) {
        auto converter = f.type->make_converter(ast::TYPE_OBJECT);
        field_statements.push_back(
            new ast::TryStatement(
                0,
                {new ast::AssignmentStatement(
                    0,
                    {new ast::RecordReferenceFieldExpression(
                        f.type,
                        new ast::VariableExpression(result),
                        f.name.text,
                        true
                    )},
                    converter(
                        analyzer,
                        new ast::ObjectSubscriptExpression(
                            e,
                            ast::TYPE_OBJECT->make_converter(ast::TYPE_STRING)(analyzer, new ast::ConstantStringExpression(utf8string(f.name.text)))
                        )
                    )
                )},
                {
                    ast::TryTrap(
                        {dynamic_cast<const ast::Exception *>(analyzer->global_scope->lookupName("ObjectSubscriptException"))},
                        nullptr,
                        new ast::ExceptionHandlerStatement(0, {new ast::NullStatement(0)})
                    )
                }
            )
        );
    }
    return new ast::StatementExpression(
        new ast::CompoundStatement(0, field_statements),
        new ast::VariableExpression(result)
    );
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeBoolean::make_converter(const Type *from) const
{
    if (from == TYPE_BOOLEAN) {
        return identity_conversion;
    }
    if (from == TYPE_OBJECT) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__getBoolean"))),
                {e}
            );
        };
    }
    return nullptr;
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeNumber::make_converter(const Type *from) const
{
    if (from == TYPE_NUMBER) {
        return identity_conversion;
    }
    if (from == TYPE_OBJECT) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__getNumber"))),
                {e}
            );
        };
    }
    return nullptr;
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeString::make_converter(const Type *from) const
{
    if (from == TYPE_STRING) {
        return identity_conversion;
    }
    if (from == TYPE_OBJECT) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__getString"))),
                {e}
            );
        };
    }
    return nullptr;
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeBytes::make_converter(const Type *from) const
{
    if (from == TYPE_BYTES) {
        return identity_conversion;
    }
    if (from == TYPE_OBJECT) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__getBytes"))),
                {e}
            );
        };
    }
    return nullptr;
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeObject::make_converter(const Type *from) const
{
    if (dynamic_cast<const TypePointerNil *>(from) != nullptr) {
        return [](Analyzer *analyzer, const Expression *) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeNull"))),
                    {}
            );
        };
    }
    if (from == TYPE_BOOLEAN) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeBoolean"))),
                    {e}
            );
        };
    }
    if (from == TYPE_NUMBER) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeNumber"))),
                    {e}
            );
        };
    }
    if (from == TYPE_STRING) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeString"))),
                    {e}
            );
        };
    }
    if (from == TYPE_BYTES) {
        return [](Analyzer *analyzer, const Expression *e) {
            return new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeBytes"))),
                    {e}
            );
        };
    }
    if (from == TYPE_OBJECT) {
        return identity_conversion;
    }
    const TypeArray *atype = dynamic_cast<const TypeArray *>(from);
    if (atype != nullptr) {
        if (atype->elementtype == nullptr) {
            return [](Analyzer *analyzer, const Expression *e) {
                return new FunctionCall(
                    new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeArray"))),
                    {e}
                );
            };
        }
        if (make_converter(atype->elementtype) == nullptr) {
            return nullptr;
        }
        if (atype->elementtype != TYPE_OBJECT) {
            return [atype](Analyzer *analyzer, const Expression *e) {
                return new FunctionCall(
                    new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeArray"))),
                    // TODO: what happens if this conversion can't be done?
                    {make_array_conversion(analyzer, atype, e, TYPE_ARRAY_OBJECT)}
                );
            };
        } else {
            return [](Analyzer *analyzer, const Expression *e) {
                return new FunctionCall(
                    new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeArray"))),
                    {e}
                );
            };
        }
    }
    const TypeDictionary *dtype = dynamic_cast<const TypeDictionary *>(from);
    if (dtype != nullptr) {
        if (dtype->elementtype == nullptr) {
            return [](Analyzer *analyzer, const Expression *e) {
                return new FunctionCall(
                    new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeArray"))),
                    {e}
                );
            };
        }
        if (make_converter(dtype->elementtype) == nullptr) {
            return nullptr;
        }
        if (dtype->elementtype != TYPE_OBJECT) {
            return [dtype](Analyzer *analyzer, const Expression *e) {
                return new FunctionCall(
                    new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeDictionary"))),
                    {make_dictionary_conversion(analyzer, dtype, e, TYPE_DICTIONARY_OBJECT)}
                );
            };
        } else {
            return [](Analyzer *analyzer, const Expression *e) {
                return new FunctionCall(
                    new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__makeDictionary"))),
                    {e}
                );
            };
        }
    }
    const TypeRecord *rtype = dynamic_cast<const TypeRecord *>(from);
    if (rtype != nullptr) {
        for (auto &f: rtype->fields) {
            if (make_converter(f.type) == nullptr) {
                return nullptr;
            }
        }
        return [rtype](Analyzer *analyzer, const Expression *e) {
            return make_object_conversion_from_record(analyzer, rtype, e);
        };
    }
    return nullptr;
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeArray::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (is_structure_compatible(from)) {
        return identity_conversion;
    }
    const TypeArrayLiteral *tal = dynamic_cast<const TypeArrayLiteral *>(from);
    if (tal != nullptr && elementtype != nullptr) {
        if (tal->elements.empty()) {
            return identity_conversion;
        }
        int i = 0;
        for (auto e: tal->elements) {
            if (elementtype->make_converter(e->type) == nullptr) {
                return nullptr; // error(3079, tal->elementtokens[i], "type mismatch");
            }
            i++;
        }
    }
    if (from == TYPE_OBJECT) {
        if (elementtype->make_converter(ast::TYPE_OBJECT) == nullptr) {
            return nullptr;
        }
        return [this](Analyzer *analyzer, const Expression *e) {
            return make_array_conversion(analyzer, TYPE_ARRAY_OBJECT, new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__getArray"))),
                {e}
            ), this);
        };
    }
    const TypeArray *atype = dynamic_cast<const TypeArray *>(from);
    if (atype == nullptr) {
        return nullptr;
    }
    if (elementtype->make_converter(atype->elementtype) == nullptr) {
        return nullptr;
    }
    return [this, atype](Analyzer *analyzer, const Expression *e) {
        return make_array_conversion(analyzer, atype, e, this);
    };
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *e)> ast::TypeDictionary::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (is_structure_compatible(from)) {
        return identity_conversion;
    }
    const TypeDictionaryLiteral *tdl = dynamic_cast<const TypeDictionaryLiteral *>(from);
    if (tdl != nullptr && elementtype != nullptr) {
        if (tdl->elements.empty()) {
            return identity_conversion;
        }
        int i = 0;
        for (auto &e: tdl->elements) {
            if (elementtype->make_converter(e.second->type) == nullptr) {
                return nullptr; // error(3072, tdl->elementtokens[i], "type mismatch");
            }
            i++;
        }
    }
    if (from == TYPE_OBJECT) {
        if (elementtype->make_converter(ast::TYPE_OBJECT) == nullptr) {
            return nullptr;
        }
        return [this](Analyzer *analyzer, const Expression *e) {
            return make_dictionary_conversion(analyzer, TYPE_DICTIONARY_OBJECT, new FunctionCall(
                new VariableExpression(dynamic_cast<const Variable *>(analyzer->global_scope->lookupName("object__getDictionary"))),
                {e}
            ), this);
        };
    }
    const TypeDictionary *dtype = dynamic_cast<const TypeDictionary *>(from);
    if (dtype == nullptr) {
        return nullptr;
    }
    if (elementtype->make_converter(dtype->elementtype) == nullptr) {
        return nullptr;
    }
    return [this, dtype](Analyzer *analyzer, const Expression *e) {
        return make_dictionary_conversion(analyzer, dtype, e, this);
    };
}

std::function<const ast::Expression *(Analyzer *analyzer, const ast::Expression *from)> ast::TypeRecord::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (from == TYPE_OBJECT) {
        for (auto &f: fields) {
            auto conv = f.type->make_converter(ast::TYPE_OBJECT);
            if (conv == nullptr) {
                return nullptr;
            }
        }
        return [this](Analyzer *analyzer, const Expression *e) {
            return make_record_conversion_from_object(analyzer, this, e);
        };
    }
    return nullptr;
}

class GlobalScope: public ast::Scope {
public:
    GlobalScope(): Scope(nullptr, nullptr) {}
};

Analyzer::Analyzer(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ast::ExternalGlobalInfo> *external_globals)
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
    loops(),
    imported_checked_stack()
{
}

ast::TypeEnum::TypeEnum(const Token &declaration, const std::string &module, const std::string &name, const std::map<std::string, int> &names, Analyzer *analyzer)
  : Type(declaration, name),
    module(module),
    names(names)
{
    {
        std::vector<FunctionParameter *> params;
        FunctionParameter *fp = new FunctionParameter(Token(IDENTIFIER, "self"), "self", this, 1, ParameterType::Mode::IN, nullptr);
        params.push_back(fp);
        Function *f = new Function(Token(), "enum.toString", TYPE_STRING, analyzer->global_scope->frame, analyzer->global_scope, params, false, 1);
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
            values[n.second] = new ConstantStringExpression(utf8string(n.first));
        }
        f->statements.push_back(new ReturnStatement(0, new ArrayValueIndexExpression(TYPE_STRING, new ArrayLiteralExpression(TYPE_STRING, values, {}), new VariableExpression(fp), false)));
        methods["toString"] = f;
    }
}

ast::StringReferenceIndexExpression::StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
  : ReferenceExpression(TYPE_STRING, ref->is_readonly),
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

ast::StringValueIndexExpression::StringValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
  : Expression(TYPE_STRING, str->is_readonly),
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

ast::BytesReferenceIndexExpression::BytesReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
  : ReferenceExpression(TYPE_BYTES, ref->is_readonly),
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

ast::BytesValueIndexExpression::BytesValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
  : Expression(TYPE_BYTES, str->is_readonly),
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

ast::ArrayReferenceRangeExpression::ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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

ast::ArrayValueRangeExpression::ArrayValueRangeExpression(const Expression *array, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer)
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

ast::Module *Analyzer::import_module(const Token &token, const std::string &name, bool optional)
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
    try {
        support->loadBytecode(name, object);
    } catch (BytecodeException &e) {
        if (optional) {
            fprintf(stderr, "neonc: Note: Optional module %s not found at compile time\n", name.c_str());
            return ast::MODULE_MISSING;
        } else {
            error(3001, token, std::string("module not found: ") + e.what());
        }
    }
    ast::Module *module = new ast::Module(Token(), global_scope, name, optional);
    // Must do interfaces before types (so classes can refer to these).
    for (auto i: object.export_interfaces) {
        std::string interfacename = object.strtable[i.name];
        std::vector<std::pair<Token, const ast::TypeFunction *>> methods;
        for (auto method: i.method_descriptors) {
            const ast::TypeFunction *tf = dynamic_cast<const ast::TypeFunction *>(deserialize_type(module->scope, object.strtable[method.second]));
            if (tf == nullptr) {
                internal_error("deserialized type not function");
            }
            methods.push_back(std::make_pair(Token(IDENTIFIER, object.strtable[method.first]), tf));
        }
        module->scope->addName(Token(IDENTIFIER, ""), interfacename, new ast::Interface(Token(), interfacename, methods));
    }
    for (auto t: object.export_types) {
        if (object.strtable[t.descriptor][0] == 'R') {
            // Support recursive record type declarations.
            ast::TypeRecord *actual_record = new ast::TypeRecord(Token(), name, name + "." + object.strtable[t.name], std::vector<ast::TypeRecord::Field>());
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[t.name], actual_record);
            ast::Type *type = deserialize_type(module->scope, object.strtable[t.descriptor]);
            const ast::TypeRecord *rectype = dynamic_cast<const ast::TypeRecord *>(type);
            const_cast<std::vector<ast::TypeRecord::Field> &>(actual_record->fields) = rectype->fields;
            const_cast<std::map<std::string, size_t> &>(actual_record->field_names) = rectype->field_names;
        } else if (object.strtable[t.descriptor][0] == 'C') {
            // Support recursive class type declarations.
            ast::TypeClass *actual_class = new ast::TypeClass(Token(), name, name + "." + object.strtable[t.name], std::vector<ast::TypeRecord::Field>(), std::vector<const ast::Interface *>());
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[t.name], actual_class);
            ast::Type *type = deserialize_type(module->scope, object.strtable[t.descriptor]);
            const ast::TypeClass *classtype = dynamic_cast<const ast::TypeClass *>(type);
            const_cast<std::vector<ast::TypeRecord::Field> &>(actual_class->fields) = classtype->fields;
            const_cast<std::map<std::string, size_t> &>(actual_class->field_names) = classtype->field_names;
            const_cast<std::vector<const ast::Interface *> &>(actual_class->interfaces) = classtype->interfaces;
        } else {
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[t.name], deserialize_type(module->scope, object.strtable[t.descriptor]));
        }
    }
    for (auto c: object.export_constants) {
        const ast::Type *type = deserialize_type(module->scope, object.strtable[c.type]);
        int i = 0;
        module->scope->addName(Token(IDENTIFIER, ""), object.strtable[c.name], new ast::Constant(Token(), object.strtable[c.name], type->deserialize_value(c.value, i)));
    }
    for (auto v: object.export_variables) {
        module->scope->addName(Token(IDENTIFIER, ""), object.strtable[v.name], new ast::ModuleVariable(module, object.strtable[v.name], deserialize_type(module->scope, object.strtable[v.type])));
    }
    for (auto f: object.export_functions) {
        const ast::TypeFunction *ftype = dynamic_cast<const ast::TypeFunction *>(deserialize_type(module->scope, object.strtable[f.descriptor]));
        const std::string function_name = object.strtable[f.name];
        auto i = function_name.find('.');
        if (i != std::string::npos) {
            const std::string typestr = function_name.substr(0, i);
            const std::string method = function_name.substr(i+1);
            ast::Name *n = module->scope->lookupName(typestr);
            ast::Type *type = dynamic_cast<ast::Type *>(n);
            type->methods[method] = new ast::ModuleFunction(module, function_name, ftype, object.strtable[f.descriptor]);
        } else {
            module->scope->addName(Token(IDENTIFIER, ""), function_name, new ast::ModuleFunction(module, function_name, ftype, object.strtable[f.descriptor]));
        }
    }
    for (auto e: object.export_exceptions) {
        std::string exceptionname = object.strtable[e.name];
        std::string::size_type p = exceptionname.find('.');
        if (p != std::string::npos) {
            ast::Name *n = module->scope->lookupName(exceptionname.substr(0, p));
            if (n == nullptr) {
                internal_error("name not found in exception import");
            }
            ast::Exception *exc = dynamic_cast<ast::Exception *>(n);
            if (exc == nullptr) {
                internal_error("name not exception in exception import");
            }
            for (;;) {
                std::string::size_type q = exceptionname.find('.', p+1);
                if (q == std::string::npos) {
                    exc->subexceptions[exceptionname.substr(p+1, q)] = new ast::Exception(Token(), exceptionname.substr(0, q));
                    break;
                }
                auto s = exc->subexceptions.find(exceptionname.substr(p+1, q));
                if (s == exc->subexceptions.end()) {
                    internal_error("subexception not found in exception import");
                }
                p = q;
            }
        } else {
            module->scope->addName(Token(IDENTIFIER, ""), object.strtable[e.name], new ast::Exception(Token(), object.strtable[e.name]));
        }
    }
    s_importing.pop_back();
    rtl_import(name, module);
    modules[name] = module;
    return module;
}

const ast::Type *Analyzer::analyze(const pt::Type *type, const std::string &name)
{
    TypeAnalyzer ta(this, name);
    type->accept(&ta);
    return ta.type;
}

const ast::Type *Analyzer::analyze(const pt::TypeSimple *type, const std::string &)
{
    const ast::Name *name = scope.top()->lookupName(type->name);
    if (name == nullptr) {
        error(3011, type->token, "unknown type name");
    }
    const ast::Type *r = dynamic_cast<const ast::Type *>(name);
    if (r == nullptr) {
        error2(3012, type->token, "name is not a type", name->declaration, "name declared here");
    }
    return r;
}

const ast::Type *Analyzer::analyze_enum(const pt::TypeEnum *type, const std::string &name)
{
    std::map<std::string, int> names;
    int index = 0;
    for (auto x: type->names) {
        std::string enumname = x.first.text;
        auto t = names.find(enumname);
        if (t != names.end()) {
            error2(3010, x.first, "duplicate enum: " + enumname, type->names[t->second].first, "first declaration here");
        }
        names[enumname] = index;
        index++;
    }
    return new ast::TypeEnum(type->token, module_name, name, names, this);
}

std::vector<ast::TypeRecord::Field> Analyzer::analyze_fields(const pt::TypeRecord *type, bool for_class)
{
    std::vector<ast::TypeRecord::Field> fields;
    if (for_class) {
        fields.push_back(ast::TypeRecord::Field(Token("__classtype"), new ast::TypePointer(Token(), nullptr), true));
    }
    std::map<std::string, Token> field_names;
    for (auto &x: type->fields) {
        std::string name = x->name.text;
        auto prev = field_names.find(name);
        if (prev != field_names.end()) {
            error2(3009, x->name, "duplicate field: " + x->name.text, prev->second, "first declaration here");
        }
        const ast::Type *t = analyze(x->type.get());
        if (dynamic_cast<const ast::TypeClass *>(t) != nullptr) {
            error(3226, x->type->token, "class type not permitted as record member");
        }
        if (dynamic_cast<const ast::TypeValidPointer *>(t) != nullptr) {
            error(3223, x->type->token, "valid pointer type not permitted as record member");
        }
        fields.push_back(ast::TypeRecord::Field(x->name, t, x->is_private));
        field_names[name] = x->name;
    }
    return fields;
}

const ast::Type *Analyzer::analyze_record(const pt::TypeRecord *type, const std::string &name)
{
    std::vector<ast::TypeRecord::Field> fields = analyze_fields(type, false);
    return new ast::TypeRecord(type->token, module_name, name, fields);
}

const ast::Type *Analyzer::analyze_class(const pt::TypeClass *type, const std::string &name)
{
    std::vector<ast::TypeRecord::Field> fields = analyze_fields(type, true);
    std::vector<const ast::Interface *> interfaces;
    for (auto i: type->interfaces) {
        ast::Scope *s = scope.top();
        if (i.first.type != NONE) {
            const ast::Name *modname = scope.top()->lookupName(i.first.text);
            const ast::Module *mod = dynamic_cast<const ast::Module *>(modname);
            if (mod == nullptr) {
                error(3257, i.first, "module not found");
            }
            s = mod->scope;
        }
        const ast::Name *interfacename = s->lookupName(i.second.text);
        if (interfacename == nullptr) {
            error(3248, i.second, "interface name not found");
        }
        const ast::Interface *iface = dynamic_cast<const ast::Interface *>(interfacename);
        if (iface == nullptr) {
            error(3249, i.second, "interface name expected here");
        }
        interfaces.push_back(iface);
    }
    return new ast::TypeClass(type->token, module_name, name, fields, interfaces);
}

const ast::Type *Analyzer::analyze(const pt::TypePointer *type, const std::string &)
{
    if (type->reftype != nullptr) {
        const pt::TypeSimple *simple = dynamic_cast<const pt::TypeSimple *>(type->reftype.get());
        if (simple != nullptr && scope.top()->lookupName(simple->name) == nullptr) {
            const std::string name = simple->name;
            ast::TypePointer *ptrtype = new ast::TypePointer(type->token, new ast::TypeForwardClass(type->reftype->token));
            scope.top()->addForward(name, ptrtype);
            return ptrtype;
        } else {
            if (simple != nullptr) {
                const ast::Name *name = scope.top()->lookupName(simple->name);
                const ast::Interface *interface = dynamic_cast<const ast::Interface *>(name);
                if (interface != nullptr) {
                    return new ast::TypeInterfacePointer(type->token, interface);
                }
            }
            const pt::TypeImport *import = dynamic_cast<const pt::TypeImport *>(type->reftype.get());
            if (import != nullptr) {
                ast::Module *module = dynamic_cast<ast::Module *>(scope.top()->lookupName(import->modname.text));
                if (module != nullptr) {
                    ast::Interface *interface = dynamic_cast<ast::Interface *>(module->scope->lookupName(import->subname.text));
                    if (interface != nullptr) {
                        return new ast::TypeInterfacePointer(type->token, interface);
                    }
                }
            }
            const ast::Type *reftype = analyze(type->reftype.get());
            const ast::TypeClass *classtype = dynamic_cast<const ast::TypeClass *>(reftype);
            if (classtype == nullptr) {
                error(3098, type->reftype->token, "class or interface type expected");
            }
            return new ast::TypePointer(type->token, classtype);
        }
    } else {
        return new ast::TypePointer(type->token, nullptr);
    }
}

const ast::Type *Analyzer::analyze(const pt::TypeValidPointer *type, const std::string &)
{
    const pt::TypeSimple *simple = dynamic_cast<const pt::TypeSimple *>(type->reftype.get());
    if (simple != nullptr && scope.top()->lookupName(simple->name) == nullptr) {
        const std::string name = simple->name;
        ast::TypeValidPointer *ptrtype = new ast::TypeValidPointer(type->token, new ast::TypeForwardClass(type->reftype->token));
        scope.top()->addForward(name, ptrtype);
        return ptrtype;
    } else {
        if (simple != nullptr) {
            const ast::Name *name = scope.top()->lookupName(simple->name);
            const ast::Interface *interface = dynamic_cast<const ast::Interface *>(name);
            if (interface != nullptr) {
                return new ast::TypeValidInterfacePointer(type->token, interface);
            }
        }
        const ast::Type *reftype = analyze(type->reftype.get());
        const ast::TypeClass *classtype = dynamic_cast<const ast::TypeClass *>(reftype);
        if (classtype == nullptr) {
            error(3221, type->reftype->token, "class type expected");
        }
        return new ast::TypeValidPointer(type->token, classtype);
    }
}

const ast::TypeFunction *Analyzer::analyze_function_type(const std::unique_ptr<pt::Type> &returntype, const std::vector<std::unique_ptr<pt::FunctionParameterGroup>> &args)
{
    const ast::Type *rtype = returntype != nullptr ? analyze(returntype.get()) : ast::TYPE_NOTHING;
    std::vector<const ast::ParameterType *> params;
    bool variadic = false;
    bool in_default = false;
    for (auto &a: args) {
        if (variadic) {
            error(3270, a->token, "varargs function parameter must be last");
        }
        ast::ParameterType::Mode mode = ast::ParameterType::Mode::IN;
        switch (a->mode) {
            case pt::FunctionParameterGroup::Mode::IN:    mode = ast::ParameterType::Mode::IN;       break;
            case pt::FunctionParameterGroup::Mode::INOUT: mode = ast::ParameterType::Mode::INOUT;    break;
            case pt::FunctionParameterGroup::Mode::OUT:   mode = ast::ParameterType::Mode::OUT;      break;
        }
        const ast::Type *ptype = analyze(a->type.get());
        if (a->varargs) {
            if (a->names.size() > 1) {
                error(3271, a->names[1], "varargs must be single parameter name");
            }
            ptype = new ast::TypeArray(Token(), ptype);
            variadic = true;
        }
        const ast::Expression *def = nullptr;
        if (a->default_value != nullptr) {
            const bool is_dummy = dynamic_cast<pt::DummyExpression *>(a->default_value.get()) != nullptr;
            in_default = true;
            def = analyze(a->default_value.get());
            if (not is_dummy && not def->is_constant) {
                error(3149, a->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3151, a->token, "default value must be specified for this parameter");
        }
        for (auto name: a->names) {
            ast::ParameterType *pt = new ast::ParameterType(name, mode, ptype, def);
            params.push_back(pt);
        }
    }
    return new ast::TypeFunction(rtype, params, variadic);
}

const ast::Type *Analyzer::analyze(const pt::TypeFunctionPointer *type, const std::string &)
{
    const ast::TypeFunction *ftype = analyze_function_type(type->returntype, type->args);
    return new ast::TypeFunctionPointer(type->token, ftype);
}

const ast::Type *Analyzer::analyze(const pt::TypeParameterised *type, const std::string &)
{
    if (type->name.text == "Array") {
        const ast::Type *elementtype = analyze(type->elementtype.get());
        if (dynamic_cast<const ast::TypeClass *>(elementtype) != nullptr) {
            error(3227, type->elementtype->token, "class type not permitted as array element type");
        }
        if (dynamic_cast<const ast::TypeValidPointer *>(elementtype) != nullptr) {
            error(3222, type->elementtype->token, "valid pointer type not permitted as array element type");
        }
        return new ast::TypeArray(type->name, elementtype);
    }
    if (type->name.text == "Dictionary") {
        const ast::Type *elementtype = analyze(type->elementtype.get());
        if (dynamic_cast<const ast::TypeClass *>(elementtype) != nullptr) {
            error(3228, type->elementtype->token, "class type not permitted as dictionary element type");
        }
        return new ast::TypeDictionary(type->name, elementtype);
    }
    internal_error("Invalid parameterized type");
}

const ast::Type *Analyzer::analyze(const pt::TypeImport *type, const std::string &)
{
    ast::Name *modname = scope.top()->lookupName(type->modname.text);
    if (modname == nullptr) {
        error(3153, type->modname, "name not found");
    }
    ast::Module *module = dynamic_cast<ast::Module *>(modname);
    if (module == nullptr) {
        error(3154, type->modname, "module name expected");
    }
    ast::Name *name = module->scope->lookupName(type->subname.text);
    if (name == nullptr) {
        error(3155, type->subname, "name not found in module");
    }
    ast::Type *rtype = dynamic_cast<ast::Type *>(name);
    if (rtype == nullptr) {
        error(3156, type->subname, "name not a type");
    }
    return rtype;
}

const ast::Expression *Analyzer::analyze(const pt::Expression *expr)
{
    ExpressionAnalyzer ea(this);
    expr->accept(&ea);
    return ea.expr;
}

const ast::Expression *Analyzer::analyze(const pt::DummyExpression *)
{
    return new ast::DummyExpression();
}

const ast::Expression *Analyzer::analyze(const pt::IdentityExpression *expr)
{
    return analyze(expr->expr.get());
}

const ast::Expression *Analyzer::analyze(const pt::BooleanLiteralExpression *expr)
{
    return new ast::ConstantBooleanExpression(expr->value);
}

const ast::Expression *Analyzer::analyze(const pt::NumberLiteralExpression *expr)
{
    return new ast::ConstantNumberExpression(expr->value);
}

const ast::Expression *Analyzer::analyze(const pt::StringLiteralExpression *expr)
{
    return new ast::ConstantStringExpression(expr->value);
}

const ast::Expression *Analyzer::analyze(const pt::FileLiteralExpression *expr)
{
    std::ifstream f(expr->name, std::ios::binary);
    if (not f) {
        error(3182, expr->token, "could not open file");
    }
    std::stringstream buffer;
    buffer << f.rdbuf();
    return new ast::ConstantBytesExpression(expr->name, buffer.str());
}

const ast::Expression *Analyzer::analyze(const pt::BytesLiteralExpression *expr)
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
    return new ast::ConstantBytesExpression("HEXBYTES literal", bytes);
}

const ast::Expression *Analyzer::analyze(const pt::ArrayLiteralExpression *expr)
{
    std::vector<const ast::Expression *> elements;
    std::vector<Token> elementtokens;
    const ast::Type *elementtype = nullptr;
    for (auto &x: expr->elements) {
        const ast::Expression *element = analyze(x.get());
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (elementtype->make_converter(element->type) == nullptr) {
            elementtype = ast::TYPE_OBJECT;
            for (auto &y: elements) {
                y = convert(elementtype, y);
            }
        }
        element = convert(elementtype, element);
        elements.push_back(element);
        elementtokens.push_back(x->token);
    }
    return new ast::ArrayLiteralExpression(elementtype, elements, elementtokens);
}

const ast::Expression *Analyzer::analyze(const pt::ArrayLiteralRangeExpression *expr)
{
    const ast::Expression *first = analyze(expr->first.get());
    first = convert(ast::TYPE_NUMBER, first);
    if (first == nullptr) {
        error(2100, expr->first->token, "numeric expression expected");
    }
    const ast::Expression *last = analyze(expr->last.get());
    last = convert(ast::TYPE_NUMBER, last);
    if (last == nullptr) {
        error(2101, expr->last->token, "numeric expression expected");
    }
    const ast::Expression *step = analyze(expr->step.get());
    step = convert(ast::TYPE_NUMBER, step);
    if (step == nullptr) {
        error(2102, expr->step->token, "numeric expression expected");
    }
    const ast::VariableExpression *range = new ast::VariableExpression(dynamic_cast<const ast::Variable *>(scope.top()->lookupName("array__range")));
    std::vector<const ast::Expression *> args;
    args.push_back(first);
    args.push_back(last);
    args.push_back(step);
    return new ast::FunctionCall(range, args);
}

const ast::Expression *Analyzer::analyze(const pt::DictionaryLiteralExpression *expr)
{
    std::vector<std::pair<utf8string, const ast::Expression *>> elements;
    std::map<utf8string, Token> keys;
    std::vector<Token> elementtokens;
    const ast::Type *elementtype = nullptr;
    for (auto &x: expr->elements) {
        const ast::Expression *k = analyze(x.first.get());
        if (k->type != ast::TYPE_STRING) {
            error(3266, x.first->token, "key value must be string");
        }
        if (not k->is_constant) {
            error(3212, x.first->token, "key value must be constant");
        }
        utf8string key = k->eval_string(x.first->token);
        auto i = keys.find(key);
        if (i != keys.end()) {
            error2(3080, x.first->token, "duplicate key", i->second, "first key here");
        }
        keys[key] = x.first->token;
        const ast::Expression *element = analyze(x.second.get());
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (elementtype->make_converter(element->type) == nullptr) {
            elementtype = ast::TYPE_OBJECT;
            for (auto &y: elements) {
                y.second = convert(elementtype, y.second);
            }
        }
        element = convert(elementtype, element);
        elements.push_back(std::make_pair(key, element));
        elementtokens.push_back(x.second->token);
    }
    return new ast::DictionaryLiteralExpression(elementtype, elements, elementtokens);
}

const ast::Expression *Analyzer::analyze(const pt::NilLiteralExpression *)
{
    return new ast::ConstantNilExpression();
}

const ast::Expression *Analyzer::analyze(const pt::NowhereLiteralExpression *)
{
    return new ast::ConstantNowhereExpression();
}

const ast::Expression *Analyzer::analyze(const pt::IdentifierExpression *expr)
{
    const ast::Name *name = scope.top()->lookupName(expr->name);
    if (name == nullptr) {
        error(3039, expr->token, "name not found: " + expr->name);
    }
    const ast::Constant *constant = dynamic_cast<const ast::Constant *>(name);
    if (constant != nullptr) {
        return new ast::ConstantExpression(constant);
    }
    const ast::Variable *var = dynamic_cast<const ast::Variable *>(name);
    if (var != nullptr) {
        return new ast::VariableExpression(var);
    }
    error(3040, expr->token, "name is not a constant or variable: " + expr->name);
}

const ast::Name *Analyzer::analyze_qualified_name(const pt::Expression *expr)
{
    const pt::IdentifierExpression *ident = dynamic_cast<const pt::IdentifierExpression *>(expr);
    if (ident != nullptr) {
        const ast::Name *name = scope.top()->lookupName(ident->name);
        const ast::Module *module = dynamic_cast<const ast::Module *>(name);
        if (module != nullptr) {
            if (module == ast::MODULE_MISSING || (module->optional && (imported_checked_stack.empty() || imported_checked_stack.top().find(ident->name) == imported_checked_stack.top().end()))) {
                error(3281, ident->token, "optional module not checked with IF IMPORTED");
            }
        }
        return name;
    }
    const pt::DotExpression *dotted = dynamic_cast<const pt::DotExpression *>(expr);
    if (dotted == nullptr) {
        return nullptr;
    }
    const ast::Name *base = analyze_qualified_name(dotted->base.get());
    const ast::Module *module = dynamic_cast<const ast::Module *>(base);
    if (module != nullptr) {
        const ast::Name *name = module->scope->lookupName(dotted->name.text);
        if (name == nullptr) {
            error(3134, dotted->name, "name not found: " + dotted->name.text);
        }
        return name;
    }
    return nullptr;
}

const ast::Expression *Analyzer::analyze(const pt::DotExpression *expr)
{
    const ast::Name *name = analyze_qualified_name(expr);
    if (name != nullptr) {
        const ast::Constant *constant = dynamic_cast<const ast::Constant *>(name);
        if (constant != nullptr) {
            return new ast::ConstantExpression(constant);
        }
        const ast::Variable *var = dynamic_cast<const ast::Variable *>(name);
        if (var != nullptr) {
            return new ast::VariableExpression(var);
        }
        internal_error("qualified name resolved but not matched: " + name->name);
    }
    name = analyze_qualified_name(expr->base.get());
    if (name != nullptr) {
        const ast::TypeEnum *enumtype = dynamic_cast<const ast::TypeEnum *>(name);
        if (enumtype != nullptr) {
            auto enumname = enumtype->names.find(expr->name.text);
            if (enumname == enumtype->names.end()) {
                error2(3023, expr->name, "identifier not member of enum: " + expr->name.text, enumtype->declaration, "enum declared here");
            }
            return new ast::ConstantEnumExpression(enumtype, enumname->second);
        }
    }
    const ast::Expression *base = analyze(expr->base.get());
    if (base->type == ast::TYPE_OBJECT) {
        return new ast::ObjectSubscriptExpression(base, convert(ast::TYPE_OBJECT, new ast::ConstantStringExpression(utf8string(expr->name.text))));
    }
    const ast::TypeRecord *recordtype = dynamic_cast<const ast::TypeRecord *>(base->type);
    if (recordtype == nullptr) {
        error(3046, expr->base->token, "not a record");
    }
    if (dynamic_cast<const ast::TypeForwardClass *>(recordtype) != nullptr) {
        internal_error("class not defined yet");
    }
    auto f = recordtype->field_names.find(expr->name.text);
    if (f == recordtype->field_names.end()) {
        error2(3045, expr->name, "field not found", recordtype->declaration, "record declared here");
    }
    if (recordtype->fields[f->second].is_private && (functiontypes.empty() || functiontypes.top().first != recordtype)) {
        error(3162, expr->name, "field is private");
    }
    const ast::Type *type = recordtype->fields[f->second].type;
    const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
    if (ref != nullptr) {
        return new ast::RecordReferenceFieldExpression(type, ref, expr->name.text, true);
    } else {
        return new ast::RecordValueFieldExpression(type, base, expr->name.text, true);
    }
}

const ast::Expression *Analyzer::analyze(const pt::ArrowExpression *expr)
{
    const ast::Expression *base = analyze(expr->base.get());
    const ast::TypePointer *pointertype = dynamic_cast<const ast::TypePointer *>(base->type);
    if (pointertype == nullptr) {
        error(3112, expr->base->token, "not a pointer");
    }
    if (dynamic_cast<const ast::TypeValidPointer *>(pointertype) == nullptr) {
        error(3103, expr->base->token, "pointer must be a valid pointer");
    }
    const ast::TypeRecord *recordtype = pointertype->reftype;
    if (recordtype == nullptr) {
        error(3117, expr->base->token, "pointer must not be a generic pointer");
    }
    if (dynamic_cast<const ast::TypeForwardClass *>(recordtype) != nullptr) {
        error2(3104, expr->base->token, "class not defined yet", recordtype->declaration, "forward declaration here");
    }
    auto f = recordtype->field_names.find(expr->name.text);
    if (f == recordtype->field_names.end()) {
        error2(3111, expr->name, "field not found", recordtype->declaration, "record declared here");
    }
    if (recordtype->fields[f->second].is_private && (functiontypes.empty() || functiontypes.top().first != recordtype)) {
        error(3163, expr->name, "field is private");
    }
    const ast::Type *type = recordtype->fields[f->second].type;
    const ast::PointerDereferenceExpression *ref = new ast::PointerDereferenceExpression(recordtype, base);
    return new ast::RecordReferenceFieldExpression(type, ref, expr->name.text, false);
}

const ast::Expression *Analyzer::analyze(const pt::SubscriptExpression *expr)
{
    const ast::Expression *base = analyze(expr->base.get());
    const ast::Expression *index = analyze(expr->index.get());
    const ast::Type *type = base->type;
    const ast::TypeArray *arraytype = dynamic_cast<const ast::TypeArray *>(type);
    const ast::TypeDictionary *dicttype = dynamic_cast<const ast::TypeDictionary *>(type);
    if (arraytype != nullptr) {
        index = convert(ast::TYPE_NUMBER, index);
        if (index == nullptr) {
            error2(3041, expr->index->token, "index must be a number", arraytype->declaration, "array declared here");
        }
        type = arraytype->elementtype;
        const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
        if (ref != nullptr) {
            if (expr->from_last) {
                return new ast::ArrayReferenceIndexExpression(
                    type,
                    ref,
                    new ast::AdditionExpression(
                        new ast::SubtractionExpression(
                            new ast::FunctionCall(
                                new ast::VariableExpression(
                                    ref->type->methods.at("size")
                                ),
                                { ref }
                            ),
                            new ast::ConstantNumberExpression(number_from_sint32(1))
                        ),
                        index
                    ),
                    false
                );
            } else {
                return new ast::ArrayReferenceIndexExpression(type, ref, index, false);
            }
        } else {
            if (expr->from_last) {
                return new ast::ArrayValueIndexExpression(
                    type,
                    base,
                    new ast::AdditionExpression(
                        new ast::SubtractionExpression(
                            new ast::FunctionCall(
                                new ast::VariableExpression(
                                    base->type->methods.at("size")
                                ),
                                { base }
                            ),
                            new ast::ConstantNumberExpression(number_from_sint32(1))
                        ),
                        index
                    ),
                    false
                );
            } else {
                return new ast::ArrayValueIndexExpression(type, base, index, false);
            }
        }
    } else if (dicttype != nullptr) {
        index = convert(ast::TYPE_STRING, index);
        if (index == nullptr) {
            error2(3042, expr->index->token, "index must be a string", dicttype->declaration, "dictionary declared here");
        }
        type = dicttype->elementtype;
        const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ast::DictionaryReferenceIndexExpression(type, ref, index);
        } else {
            return new ast::DictionaryValueIndexExpression(type, base, index);
        }
    } else if (type == ast::TYPE_STRING) {
        index = convert(ast::TYPE_NUMBER, index);
        if (index == nullptr) {
            error(3043, expr->index->token, "index must be a number");
        }
        const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ast::StringReferenceIndexExpression(ref, index, expr->from_last, index, expr->from_last, this);
        } else {
            return new ast::StringValueIndexExpression(base, index, expr->from_last, index, expr->from_last, this);
        }
    } else if (type == ast::TYPE_OBJECT) {
        index = convert(ast::TYPE_OBJECT, index);
        if (index == nullptr) {
            error(3262, expr->index->token, "index must be an object");
        }
        return new ast::ObjectSubscriptExpression(base, index);
    } else {
        error2(3044, expr->token, "not an array or dictionary", type->declaration, "declaration here");
    }
}

const ast::Expression *Analyzer::analyze(const pt::InterpolatedStringExpression *expr)
{
    const ast::VariableExpression *concat = new ast::VariableExpression(dynamic_cast<const ast::Variable *>(scope.top()->lookupName("string__concat")));
    const ast::Module *string = import_module(Token(), "string", false);
    if (string == nullptr) {
        internal_error("need module string");
    }
    const ast::VariableExpression *format = new ast::VariableExpression(dynamic_cast<const ast::Variable *>(string->scope->lookupName("format")));
    const ast::Expression *r = nullptr;
    for (auto &x: expr->parts) {
        const ast::Expression *e = analyze(x.first.get());
        std::string fmt = x.second.text;
        const ast::Expression *str = convert(ast::TYPE_STRING, e);
        if (str != nullptr && e->type != ast::TYPE_OBJECT) {
            // No action required.
        } else {
            auto toString = e->type->methods.find("toString");
            if (toString == e->type->methods.end()) {
                error(3132, x.first->token, "no toString() method found for type");
            }
            {
                std::vector<const ast::Expression *> args;
                args.push_back(e);
                str = new ast::FunctionCall(new ast::VariableExpression(toString->second), args);
            }
        }
        if (not fmt.empty()) {
            std::vector<const ast::Expression *> args;
            args.push_back(str);
            args.push_back(new ast::ConstantStringExpression(utf8string(fmt)));
            str = new ast::FunctionCall(format, args);
        }
        if (r == nullptr) {
            r = str;
        } else {
            std::vector<const ast::Expression *> args;
            args.push_back(r);
            args.push_back(str);
            r = new ast::FunctionCall(concat, args);
        }
    }
    return r;
}

const ast::Expression *Analyzer::analyze(const pt::FunctionCallExpression *expr)
{
    // TODO: This stuff works, but it's increasingly becoming a mess. There seem
    // to be quite a few cases of x.y(), and this function tries to handle them all
    // in a haphazard fashion.
    const ast::TypeRecord *recordtype = nullptr;
    const pt::IdentifierExpression *fname = dynamic_cast<const pt::IdentifierExpression *>(expr->base.get());
    if (fname != nullptr) {
        recordtype = dynamic_cast<const ast::TypeRecord *>(scope.top()->lookupName(fname->name));
    }
    const pt::DotExpression *dotmethod = dynamic_cast<const pt::DotExpression *>(expr->base.get());
    const pt::ArrowExpression *arrowmethod = dynamic_cast<const pt::ArrowExpression *>(expr->base.get());
    const ast::Expression *self = nullptr;
    const ast::Expression *func = nullptr;
    const ast::Expression *dispatch = nullptr;
    const ast::TypeFunction *ftype = nullptr;
    if (dotmethod != nullptr) {
        // This check avoids trying to evaluate foo.bar as an
        // expression in foo.bar() where foo is actually a module.
        bool is_module_call = false;
        const pt::IdentifierExpression *ident = dynamic_cast<const pt::IdentifierExpression *>(dotmethod->base.get());
        if (ident != nullptr) {
            const ast::Name *name = scope.top()->lookupName(ident->name);
            is_module_call = dynamic_cast<const ast::Module *>(name) != nullptr;
        }
        if (not is_module_call) {
            const ast::Expression *base = analyze(dotmethod->base.get());
            auto m = base->type->methods.find(dotmethod->name.text);
            if (m == base->type->methods.end()) {
                error(3137, dotmethod->name, "method not found");
            } else {
                if (dynamic_cast<const ast::TypeClass *>(base->type) != nullptr) {
                    internal_error("class not expected here");
                }
                self = base;
            }
            func = new ast::VariableExpression(m->second);
        } else {
            recordtype = dynamic_cast<const ast::TypeRecord *>(analyze_qualified_name(expr->base.get()));
            if (recordtype == nullptr) {
                func = analyze(expr->base.get());
            }
        }
    } else if (arrowmethod != nullptr) {
        const ast::Expression *base = analyze(arrowmethod->base.get());
        const ast::TypePointer *ptype = dynamic_cast<const ast::TypePointer *>(base->type);
        const ast::TypeInterfacePointer *iptype = dynamic_cast<const ast::TypeInterfacePointer *>(base->type);
        if (ptype != nullptr) {
            if (dynamic_cast<const ast::TypeValidPointer *>(ptype) == nullptr) {
                error(3219, arrowmethod->base->token, "valid pointer required");
            }
            auto m = ptype->reftype->methods.find(arrowmethod->name.text);
            if (m == ptype->reftype->methods.end()) {
                error(3220, arrowmethod->name, "method not found");
            }
            self = base;
            func = new ast::VariableExpression(m->second);
        } else if (iptype != nullptr) {
            size_t i = 0;
            while (i < iptype->interface->methods.size()) {
                if (arrowmethod->name.text == iptype->interface->methods[i].first.text) {
                    break;
                }
                i++;
            }
            if (i >= iptype->interface->methods.size()) {
                error(3250, arrowmethod->name, "interface method not found");
            }
            self = new ast::InterfacePointerDeconstructor(base);
            func = new ast::InterfaceMethodExpression(iptype->interface->methods[i].second, i);
            dispatch = base;
            ftype = iptype->interface->methods[i].second;
        } else {
            error(3218, arrowmethod->base->token, "pointer type expected");
        }
    } else if (recordtype == nullptr) {
        func = analyze(expr->base.get());
    }
    if (recordtype != nullptr) {
        std::vector<const ast::Expression *> elements(recordtype->fields.size());
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
            const ast::Expression *element = analyze(x->expr.get());
            element = convert(f->type, element);
            if (element == nullptr) {
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
        const ast::TypeClass *classtype = dynamic_cast<const ast::TypeClass *>(recordtype);
        if (classtype != nullptr) {
            return new ast::ClassLiteralExpression(classtype, elements);
        } else {
            return new ast::RecordLiteralExpression(recordtype, elements);
        }
    }
    if (ftype == nullptr) {
        ftype = dynamic_cast<const ast::TypeFunction *>(func->type);
        if (ftype == nullptr) {
            const ast::TypeFunctionPointer *pf = dynamic_cast<const ast::TypeFunctionPointer *>(func->type);
            if (pf == nullptr) {
                error(3017, expr->base->token, "not a function");
            }
            ftype = pf->functype;
            dispatch = func;
        }
    }
    std::vector<const ast::Expression *> varargs_array;
    bool in_varargs = ftype->variadic && ftype->params.size() == 1;
    const ast::Type *varargs_type = in_varargs ? dynamic_cast<const ast::TypeArray *>(ftype->params[0]->type)->elementtype : nullptr;
    int param_index = 0;
    std::vector<const ast::Expression *> args(ftype->params.size());
    if (self != nullptr) {
        switch (ftype->params[0]->mode) {
            case ast::ParameterType::Mode::IN:
                break;
            case ast::ParameterType::Mode::INOUT: {
                const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(self);
                if (ref == nullptr) {
                    error(3277, dotmethod->base.get()->token, "first parameter must be a reference");
                }
                break;
            }
            case ast::ParameterType::Mode::OUT:
                internal_error("OUT parameter mode found for self parameter");
                break;
        }
        args[0] = self;
        ++param_index;
    }
    for (auto &a: expr->args) {
        const ast::Expression *e = analyze(a->expr.get());
        if (param_index >= static_cast<int>(ftype->params.size())) {
            error(3096, a->expr->token, "too many parameters");
        }
        int p;
        if (param_index >= 0 && a->name.text.empty()) {
            if (in_varargs) {
                p = param_index;
            } else {
                p = param_index;
                param_index++;
            }
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
        if (dynamic_cast<const ast::DummyExpression *>(e) != nullptr && a->mode.type != OUT) {
            error2(3193, a->expr->token, "Underscore can only be used with OUT parameter", ftype->params[p]->declaration, "function argument here");
        }
        if (ftype->params[p]->mode == ast::ParameterType::Mode::IN) {
            if (ftype->params[p]->type != nullptr) {
                // TODO: Above check works around problem in sdl.RenderDrawLines.
                // Something about a compound type in a predefined function parameter list.
                const ast::Type *ptype = ftype->params[p]->type;
                if (a->spread && in_varargs) {
                    if (not varargs_array.empty()) {
                        error(3272, a->expr->token, "spread argument must be only varargs argument");
                    }
                    in_varargs = false;
                    param_index++;
                }
                if (in_varargs) {
                    ptype = varargs_type;
                }
                e = convert(ptype, e);
                if (e == nullptr) {
                    error2(3019, a->expr->token, "type mismatch", ftype->params[p]->declaration, "function argument here");
                }
            }
        } else {
            const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(e);
            if (ref == nullptr) {
                error2(3018, a->expr->token, "function call argument must be reference", ftype->params[p]->declaration, "function argument here");
            }
            if (ref->is_readonly) {
                error(3106, a->expr->token, "readonly parameter to OUT");
            }
            if (e->type != ast::TYPE_DUMMY && not ftype->params[p]->type->is_structure_compatible(e->type)) {
                error2(3194, a->expr->token, "type mismatch", ftype->params[p]->declaration, "function argument here");
            }
            if (ftype->params[p]->mode == ast::ParameterType::Mode::INOUT && not ref->can_generate_address()) {
                error(3241, a->expr->token, "using this kind of expression with an INOUT parameter is currently not supported");
            }
        }
        if (ftype->params[p]->mode == ast::ParameterType::Mode::OUT && a->mode.type != OUT) {
            error(3184, a->expr->token, "OUT keyword required");
        } else if (ftype->params[p]->mode == ast::ParameterType::Mode::INOUT && a->mode.type != INOUT) {
            error(3185, a->expr->token, "INOUT keyword required");
        } else if ((a->mode.type == IN && ftype->params[p]->mode != ast::ParameterType::Mode::IN)
                || (a->mode.type == INOUT && ftype->params[p]->mode != ast::ParameterType::Mode::INOUT)
                || (a->mode.type == OUT && ftype->params[p]->mode != ast::ParameterType::Mode::OUT)) {
            error(3186, a->mode, "parameter mode must match if specified");
        }
        if (in_varargs) {
            varargs_array.push_back(e);
        } else {
            args[p] = e;
            if (ftype->variadic && param_index+1 == static_cast<int>(ftype->params.size())) {
                const ast::TypeArray *atype = dynamic_cast<const ast::TypeArray *>(ftype->params[param_index]->type);
                if (atype == nullptr) {
                    internal_error("varargs atype not null");
                }
                varargs_type = atype->elementtype;
                in_varargs = true;
            }
        }
    }
    if (in_varargs) {
        args[ftype->params.size()-1] = new ast::ArrayLiteralExpression(varargs_type, varargs_array, {});
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
    return new ast::FunctionCall(func, args, dispatch);
}

const ast::Expression *Analyzer::analyze(const pt::UnaryPlusExpression *expr)
{
    const ast::Expression *atom = analyze(expr->expr.get());
    atom = convert(ast::TYPE_NUMBER, atom);
    if (atom == nullptr) {
        error(3144, expr->expr->token, "number required");
    }
    return atom;
}

const ast::Expression *Analyzer::analyze(const pt::UnaryMinusExpression *expr)
{
    const ast::Expression *atom = analyze(expr->expr.get());
    atom = convert(ast::TYPE_NUMBER, atom);
    if (atom == nullptr) {
        error(3021, expr->expr->token, "number required for negation");
    }
    return new ast::UnaryMinusExpression(atom);
}

const ast::Expression *Analyzer::analyze(const pt::LogicalNotExpression *expr)
{
    const ast::Expression *atom = analyze(expr->expr.get());
    atom = convert(ast::TYPE_BOOLEAN, atom);
    if (atom == nullptr) {
        error(3022, expr->expr->token, "boolean required for logical not");
    }
    return new ast::LogicalNotExpression(atom);
}

const ast::Expression *Analyzer::analyze(const pt::ExponentiationExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::ExponentiationExpression(left, right);
    } else {
        error(3024, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::MultiplicationExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::MultiplicationExpression(left, right);
    } else {
        error(3025, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::DivisionExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::DivisionExpression(left, right);
    } else {
        error(3026, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::IntegerDivisionExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::FunctionCall(new ast::VariableExpression(new ast::PredefinedFunction(
            "math$intdiv",
            new ast::TypeFunction(
                ast::TYPE_NUMBER, {
                    new ast::ParameterType(Token(), ast::ParameterType::Mode::IN, ast::TYPE_NUMBER, nullptr),
                    new ast::ParameterType(Token(), ast::ParameterType::Mode::IN, ast::TYPE_NUMBER, nullptr)
                },
                false
            )
        )), {
            left,
            right
        });
    } else {
        error(3207, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::ModuloExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::ModuloExpression(left, right);
    } else {
        error(3027, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::AdditionExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (left->type == ast::TYPE_STRING && right->type == ast::TYPE_STRING) {
        error(3124, expr->token, "type mismatch (use & to concatenate strings)");
    }
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::AdditionExpression(left, right);
    } else {
        error(3028, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::SubtractionExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::SubtractionExpression(left, right);
    } else {
        error(3029, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::ConcatenationExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    // TODO: Handle ambiguous operands (such as concatenting two Object values).
    if (convert2(ast::TYPE_STRING, left, right)) {
        std::vector<const ast::Expression *> args;
        args.push_back(left);
        args.push_back(right);
        return new ast::FunctionCall(new ast::VariableExpression(dynamic_cast<const ast::Variable *>(scope.top()->lookupName("string__concat"))), args);
    }
    if (convert2(ast::TYPE_BYTES, left, right)) {
        std::vector<const ast::Expression *> args;
        args.push_back(left);
        args.push_back(right);
        return new ast::FunctionCall(new ast::VariableExpression(dynamic_cast<const ast::Variable *>(scope.top()->lookupName("bytes__concat"))), args);
    } else if (dynamic_cast<const ast::TypeArray *>(left->type) != nullptr
            && dynamic_cast<const ast::TypeArray *>(right->type) != nullptr
            && dynamic_cast<const ast::TypeArray *>(left->type)->elementtype == dynamic_cast<const ast::TypeArray *>(right->type)->elementtype) {
        std::vector<const ast::Expression *> args;
        args.push_back(left);
        args.push_back(right);
        ast::VariableExpression *ve = new ast::VariableExpression(dynamic_cast<const ast::Variable *>(scope.top()->lookupName("array__concat")));
        // Since the array__concat function cannot be declared with its proper result type,
        // we have to create a new appropriate function type based on the desired result type
        // and the existing argument types.
        ve->type = new ast::TypeFunction(left->type, dynamic_cast<const ast::TypeFunction *>(ve->type)->params, false);
        return new ast::FunctionCall(ve, args);
    } else {
        error(3116, expr->token, "type mismatch");
    }
}

ast::ComparisonExpression *Analyzer::analyze_comparison(const Token &token, const ast::Expression *left, ast::ComparisonExpression::Comparison comp, const ast::Expression *right)
{
    if (left->type == ast::TYPE_OBJECT && right->type == ast::TYPE_OBJECT) {
        error(3261, token, "cannot compare two values of type Object");
    }
    if (convert2(ast::TYPE_BOOLEAN, left, right)) {
        if (comp != ast::ComparisonExpression::Comparison::EQ && comp != ast::ComparisonExpression::Comparison::NE) {
            error(3031, token, "comparison not available for Boolean");
        }
        return new ast::BooleanComparisonExpression(left, right, comp);
    }
    if (convert2(ast::TYPE_NUMBER, left, right)) {
        return new ast::NumericComparisonExpression(left, right, comp);
    }
    if (convert2(ast::TYPE_STRING, left, right)) {
        return new ast::StringComparisonExpression(left, right, comp);
    }
    if (convert2(ast::TYPE_BYTES, left, right)) {
        return new ast::BytesComparisonExpression(left, right, comp);
    }
    {
        const ast::TypeArray *ta1 = dynamic_cast<const ast::TypeArray *>(left->type);
        const ast::TypeArray *ta2 = dynamic_cast<const ast::TypeArray *>(right->type);
        if (ta1 != nullptr && ta2 != nullptr) {
            if (comp != ast::ComparisonExpression::Comparison::EQ && comp != ast::ComparisonExpression::Comparison::NE) {
                error(3032, token, "comparison not available for Array");
            }
            if (ta1->is_structure_compatible(ta2)) {
                return new ast::ArrayComparisonExpression(left, right, comp);
            }
        }
    }
    {
        const ast::TypeDictionary *td1 = dynamic_cast<const ast::TypeDictionary *>(left->type);
        const ast::TypeDictionary *td2 = dynamic_cast<const ast::TypeDictionary *>(right->type);
        if (td1 != nullptr && td2 != nullptr) {
            if (comp != ast::ComparisonExpression::Comparison::EQ && comp != ast::ComparisonExpression::Comparison::NE) {
                error(3033, token, "comparison not available for Dictionary");
            }
            if (td1->is_structure_compatible(td2)) {
                return new ast::DictionaryComparisonExpression(left, right, comp);
            }
        }
    }
    {
        const ast::TypeRecord *tr1 = dynamic_cast<const ast::TypeRecord *>(left->type);
        const ast::TypeRecord *tr2 = dynamic_cast<const ast::TypeRecord *>(right->type);
        if (tr1 != nullptr && tr2 != nullptr) {
            if (comp != ast::ComparisonExpression::Comparison::EQ && comp != ast::ComparisonExpression::Comparison::NE) {
                error(3034, token, "comparison not available for RECORD");
            }
            if (tr1->is_structure_compatible(tr2)) {
                return new ast::RecordComparisonExpression(left, right, comp);
            }
        }
    }
    {
        const ast::TypeEnum *te1 = dynamic_cast<const ast::TypeEnum *>(left->type);
        const ast::TypeEnum *te2 = dynamic_cast<const ast::TypeEnum *>(right->type);
        if (te1 != nullptr && te2 != nullptr) {
            if (te1->is_structure_compatible(te2)) {
                return new ast::EnumComparisonExpression(left, right, comp);
            }
        }
    }
    {
        const ast::TypePointer *tp1 = dynamic_cast<const ast::TypePointer *>(left->type);
        const ast::TypePointer *tp2 = dynamic_cast<const ast::TypePointer *>(right->type);
        if (tp1 != nullptr && tp2 != nullptr) {
            if (comp != ast::ComparisonExpression::Comparison::EQ && comp != ast::ComparisonExpression::Comparison::NE) {
                error(3100, token, "comparison not available for POINTER");
            }
            if (tp1->is_structure_compatible(tp2)) {
                return new ast::PointerComparisonExpression(left, right, comp);
            }
            if (tp1->reftype == nullptr || tp2->reftype == nullptr) {
                return new ast::PointerComparisonExpression(left, right, comp);
            }
        }
    }
    {
        const ast::TypeFunctionPointer *tf1 = dynamic_cast<const ast::TypeFunctionPointer *>(left->type);
        const ast::TypeFunctionPointer *tf2 = dynamic_cast<const ast::TypeFunctionPointer *>(right->type);
        if (tf1 != nullptr && tf2 != nullptr) {
            if (comp != ast::ComparisonExpression::Comparison::EQ && comp != ast::ComparisonExpression::Comparison::NE) {
                error(3180, token, "comparison not available for FUNCTION");
            }
            if (tf1->is_structure_compatible(tf2)) {
                return new ast::FunctionPointerComparisonExpression(left, right, comp);
            }
            if (tf1->functype == nullptr || tf2->functype == nullptr) {
                return new ast::FunctionPointerComparisonExpression(left, right, comp);
            }
        }
    }
    error(3030, token, "type mismatch");
}

const ast::Expression *Analyzer::analyze(const pt::ComparisonExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    ast::ComparisonExpression::Comparison comp = static_cast<ast::ComparisonExpression::Comparison>(expr->comp); // TODO: remove cast
    const ast::Expression *right = analyze(expr->right.get());
    return analyze_comparison(expr->token, left, comp, right);
}

const ast::Expression *Analyzer::analyze(const pt::ChainedComparisonExpression *expr)
{
    std::vector<const ast::ComparisonExpression *> comps;
    const ast::Expression *left = analyze(expr->left.get());
    Token token = expr->left->token;
    for (auto &x: expr->comps) {
        ast::ComparisonExpression::Comparison comp = static_cast<ast::ComparisonExpression::Comparison>(x->comp); // TODO: remove cast
        const ast::Expression *right = analyze(x->right.get());
        comps.push_back(analyze_comparison(token, left, comp, right));
        token = x->right->token;
    }
    return new ast::ChainedComparisonExpression(comps);
}

const ast::Expression *Analyzer::analyze(const pt::TypeTestExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Type *target = analyze(expr->target.get());
    auto conv = target->make_converter(left->type);
    if (conv == nullptr) {
        return new ast::ConstantBooleanExpression(false);
    }
    if (left->type != ast::TYPE_OBJECT) {
        // TODO: Many kinds of type tests can be statically determined here.
        // However, this requires a bit more than just the simple test commented out here.
        //return new ast::ConstantBooleanExpression(target->is_assignment_compatible(left->type));
        // So for now, do it dynamically at runtime by going to Object and back.
        left = convert(ast::TYPE_OBJECT, left);
        if (left == nullptr) {
            error(3275, expr->left->token, "cannot convert to Object");
        }
    }
    return new ast::TypeTestExpression(left, convert(target, left));
}

const ast::Expression *Analyzer::analyze(const pt::MembershipExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    const ast::TypeArray *arraytype = dynamic_cast<const ast::TypeArray *>(right->type);
    const ast::TypeDictionary *dicttype = dynamic_cast<const ast::TypeDictionary *>(right->type);
    if (right->type == ast::TYPE_STRING) {
        if (left->type != ast::TYPE_STRING) {
            error(3284, expr->left->token, "type mismatch");
        }
        const ast::Module *string = import_module(Token(), "string", false);
        if (string == nullptr) {
            internal_error("need module string");
        }
        return new ast::TryExpression(
            new ast::NumericComparisonExpression(
                new ast::FunctionCall(
                    new ast::VariableExpression(
                        dynamic_cast<const ast::Variable *>(string->scope->lookupName("find"))
                    ),
                    {
                        right,
                        left
                    }
                ),
                new ast::ConstantNumberExpression(number_from_uint32(0)),
                ast::ComparisonExpression::Comparison::GE
            ),
            {
                ast::TryTrap(
                    {dynamic_cast<const ast::Exception *>(scope.top()->lookupName("ArrayIndexException"))},
                    nullptr,
                    new ast::ConstantBooleanExpression(false)
                )
            }
        );
    } else if (arraytype != nullptr) {
        // TODO: Allow more relaxed type checking.
        if (left->type != arraytype->elementtype) {
            error(3082, expr->left->token, "type mismatch");
        }
        return new ast::ArrayInExpression(left, right);
    } else if (dicttype != nullptr) {
        left = convert(ast::TYPE_STRING, left);
        if (left == nullptr) {
            error(3083, expr->left->token, "type mismatch");
        }
        return new ast::DictionaryInExpression(left, right);
    } else {
        error(3081, expr->right->token, "IN must be used with String or Array or Dictionary");
    }
}

const ast::Expression *Analyzer::analyze(const pt::ConjunctionExpression *expr)
{
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_BOOLEAN, left, right)) {
        return new ast::ConjunctionExpression(left, right);
    } else {
        error(3035, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::DisjunctionExpression *expr)
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
    if (lne != nullptr && rne != nullptr && lne->comp == pt::ComparisonExpression::Comparison::NE && rne->comp == pt::ComparisonExpression::Comparison::NE) {
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

    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    if (convert2(ast::TYPE_BOOLEAN, left, right)) {
        return new ast::DisjunctionExpression(left, right);
    } else {
        error(3036, expr->token, "type mismatch");
    }
}

const ast::Expression *Analyzer::analyze(const pt::ConditionalExpression *expr)
{
    const ast::Expression *cond = analyze(expr->cond.get());
    const ast::Expression *left = analyze(expr->left.get());
    const ast::Expression *right = analyze(expr->right.get());
    cond = convert(ast::TYPE_BOOLEAN, cond);
    if (cond == nullptr) {
        error(3265, expr->cond->token, "boolean expected");
    }
    if (left->type == ast::TYPE_NOTHING || right->type == ast::TYPE_NOTHING) {
        error(3267, left->type == ast::TYPE_NOTHING ? expr->left->token : expr->right->token, "branch of conditional must return value");
    }
    // TODO: Relax exact type check.
    if (left->type != right->type) {
        error(3037, expr->left->token, "type of THEN and ELSE must match");
    }
    return new ast::ConditionalExpression(cond, left, right);
}

const ast::Expression *Analyzer::analyze(const pt::TryExpression *expr)
{
    const ast::Expression *e = analyze(expr->expr.get());
    auto catches = analyze_catches(expr->catches, e->type);
    auto eci = expr->catches.begin();
    for (auto &c: catches) {
        const ast::ExceptionHandlerStatement *ehs = dynamic_cast<const ast::ExceptionHandlerStatement *>(c.handler);
        const ast::Expression *h = dynamic_cast<const ast::Expression *>(c.handler);
        if (ehs != nullptr) {
            if (ehs->statements.empty()) {
                error(3202, expr->expr->token, "body cannot be empty");
            }
            if (not ehs->statements.back()->is_scope_exit_statement()) {
                error(3203, dynamic_cast<const pt::TryHandlerStatement *>((*eci)->handler.get())->body.back()->token, "handler must end in EXIT, NEXT, RAISE, or RETURN");
            }
        } else if (h != nullptr) {
            // pass
        } else {
            internal_error("unexpected catch type");
        }
        ++eci;
    }
    return new ast::TryExpression(e, catches);
}

const ast::Expression *Analyzer::analyze(const pt::NewClassExpression *expr)
{
    const pt::Expression *e = expr->expr.get();
    const pt::Expression *type_expr = e;
    const ast::Expression *value = nullptr;
    const pt::FunctionCallExpression *fc = dynamic_cast<const pt::FunctionCallExpression *>(e);
    if (fc != nullptr) {
        type_expr = fc->base.get();
        value = analyze(fc);
    }
    const ast::Name *name = analyze_qualified_name(type_expr);
    const ast::TypeClass *type = dynamic_cast<const ast::TypeClass *>(name);
    if (type == nullptr) {
        error(3099, type_expr->token, "class type expected");
    }
    if (value == nullptr) {
        std::vector<const ast::Expression *> values;
        for (size_t i = 0; i < type->fields.size(); i++) {
            values.push_back(type->fields[i].type->make_default_value());
        }
        value = new ast::ClassLiteralExpression(type, values);
    }
    return new ast::NewClassExpression(type, value);
}

const ast::Expression *Analyzer::analyze(const pt::ValidPointerExpression * /*expr*/)
{
    // This should never happen because ValidPointerExpression is handled elsewhere.
    internal_error("TODO pt::Expression");
}

const ast::Expression *Analyzer::analyze(const pt::ImportedModuleExpression * /*expr*/)
{
    // This should never happen because ImportedModuleExpression is handled elsewhere.
    internal_error("TODO pt::Expression");
}

const ast::Expression *Analyzer::analyze(const pt::RangeSubscriptExpression *expr)
{
    const ast::Expression *base = analyze(expr->base.get());
    const ast::Expression *first = analyze(expr->range->first.get());
    const ast::Expression *last = analyze(expr->range->last.get());
    first = convert(ast::TYPE_NUMBER, first);
    if (first == nullptr) {
        error(3141, expr->range->first.get()->token, "range index must be a number");
    }
    last = convert(ast::TYPE_NUMBER, last);
    if (last == nullptr) {
        error(3142, expr->range->last.get()->token, "range index must be a number");
    }
    const ast::Type *type = base->type;
    const ast::TypeArray *arraytype = dynamic_cast<const ast::TypeArray *>(type);
    if (arraytype != nullptr) {
        const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ast::ArrayReferenceRangeExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        } else {
            return new ast::ArrayValueRangeExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        }
    } else if (type == ast::TYPE_STRING) {
        const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ast::StringReferenceIndexExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        } else {
            return new ast::StringValueIndexExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        }
    } else if (type == ast::TYPE_BYTES) {
        const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ast::BytesReferenceIndexExpression(ref, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        } else {
            return new ast::BytesValueIndexExpression(base, first, expr->range->first_from_end, last, expr->range->last_from_end, this);
        }
    } else {
        error2(3143, expr->base->token, "not an array or string", type->declaration, "declaration here");
    }
}

ast::Type *Analyzer::deserialize_type(ast::Scope *s, const std::string &descriptor, std::string::size_type &i)
{
    switch (descriptor.at(i)) {
        case 'Z': i++; return ast::TYPE_NOTHING;
        case 'B': i++; return ast::TYPE_BOOLEAN;
        case 'N': i++; return ast::TYPE_NUMBER;
        case 'S': i++; return ast::TYPE_STRING;
        case 'Y': i++; return ast::TYPE_BYTES;
        case 'O': i++; return ast::TYPE_OBJECT;
        case 'A': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            const ast::Type *type = deserialize_type(s, descriptor, i);
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new ast::TypeArray(Token(), type);
        }
        case 'D': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            const ast::Type *type = deserialize_type(s, descriptor, i);
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new ast::TypeDictionary(Token(), type);
        }
        case 'R': case 'C': {
            char kind = descriptor.at(i);
            i++;
            std::vector<const ast::Interface *> interfaces;
            std::vector<ast::TypeRecord::Field> fields;
            if (kind == 'C') {
                fields.push_back(ast::TypeRecord::Field(Token(), new ast::TypePointer(Token(), nullptr), true));
                if (descriptor.at(i) == '{') {
                    for (;;) {
                        i++;
                        std::string iname;
                        while (descriptor.at(i) != ',' && descriptor.at(i) != '}') {
                            iname.push_back(descriptor.at(i));
                            i++;
                        }
                        const ast::Interface *iface = dynamic_cast<const ast::Interface *>(s->lookupName(iname));
                        if (iface == nullptr) {
                            internal_error("interface not found on import");
                        }
                        interfaces.push_back(iface);
                        if (descriptor.at(i) == '}') {
                            break;
                        }
                    }
                    i++;
                }
            }
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
                const ast::Type *type = deserialize_type(s, descriptor, i);
                Token token;
                token.text = name;
                fields.push_back(ast::TypeRecord::Field(token, type, is_private));
                if (descriptor.at(i) == ',') {
                    i++;
                }
            }
            i++;
            if (kind == 'R') {
                return new ast::TypeRecord(Token(), "module", "record", fields);
            } else if (kind == 'C') {
                return new ast::TypeClass(Token(), "module", "class", fields, interfaces);
            } else {
                internal_error("what kind?");
            }
        }
        case 'I': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            std::string iname;
            while (descriptor.at(i) != '>') {
                iname.push_back(descriptor.at(i));
                i++;
            }
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            ast::Interface *iface = dynamic_cast<ast::Interface *>(s->lookupName(iname));
            if (iface == nullptr) {
                internal_error("deserialize_type");
            }
            return new ast::TypeInterfacePointer(Token(), iface);
        }
        case 'J': {
            i++;
            return ast::TYPE_INTERFACE;
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
            return new ast::TypeEnum(Token(), "module", "enum", names, this);
        }
        case 'F': {
            i++;
            std::vector<const ast::ParameterType *> params;
            if (descriptor.at(i) != '[') {
                internal_error("deserialize_type");
            }
            i++;
            while (descriptor.at(i) != ']') {
                ast::ParameterType::Mode mode = ast::ParameterType::Mode::IN;
                switch (descriptor.at(i)) {
                    case '>': mode = ast::ParameterType::Mode::IN;    break;
                    case '*': mode = ast::ParameterType::Mode::INOUT; break;
                    case '<': mode = ast::ParameterType::Mode::OUT;   break;
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
                const ast::Type *type = deserialize_type(s, descriptor, i);
                const ast::Expression *default_value = nullptr;
                if (descriptor.at(i) == '=') {
                    i++;
                    Bytecode::Bytes buf;
                    for (auto c: descriptor.substr(i)) {
                        buf.push_back(c);
                    }
                    int j = 0;
                    default_value = type->deserialize_value(buf, j);
                    i += j;
                }
                Token token;
                token.text = name;
                params.push_back(new ast::ParameterType(token, mode, type, default_value));
                if (descriptor.at(i) == ',') {
                    i++;
                }
            }
            i++;
            bool variadic = false;
            if (descriptor.at(i) == 'V') {
                variadic = true;
                i++;
            }
            if (descriptor.at(i) != ':') {
                internal_error("deserialize_type");
            }
            i++;
            const ast::Type *returntype = deserialize_type(s, descriptor, i);
            return new ast::TypeFunction(returntype, params, variadic);
        }
        case 'P': {
            i++;
            if (descriptor.at(i) != '<') {
                internal_error("deserialize_type");
            }
            i++;
            const ast::TypeClass *classtype = nullptr;
            if (descriptor.at(i) != '>') {
                const ast::Type *type = deserialize_type(s, descriptor, i);
                classtype = dynamic_cast<const ast::TypeClass *>(type);
                if (classtype == nullptr) {
                    internal_error("deserialize_type");
                }
            }
            if (descriptor.at(i) != '>') {
                internal_error("deserialize_type");
            }
            i++;
            return new ast::TypePointer(Token(), classtype);
        }
        case 'Q': {
            i++;
            ast::TypeFunction *f = dynamic_cast<ast::TypeFunction *>(deserialize_type(s, descriptor, i));
            return new ast::TypeFunctionPointer(Token(), f);
        }
        case '~': {
            i++;
            std::string name;
            while (descriptor.at(i) != ';') {
                name.push_back(descriptor.at(i));
                i++;
            }
            i++;
            ast::Scope *t = s;
            std::string localname = name;
            auto dot = name.find('.');
            if (dot != std::string::npos) {
                const ast::Module *module = import_module(Token(), name.substr(0, dot), false);
                t = module->scope;
                localname = name.substr(dot+1);
            }
            ast::Type *type = dynamic_cast<ast::Type *>(t->lookupName(localname));
            if (type == nullptr) {
                internal_error("reference to unknown type in exports: " + name);
            }
            return type;
        }
        default:
            internal_error("TODO unimplemented type in deserialize_type: " + descriptor);
    }
}

ast::Type *Analyzer::deserialize_type(ast::Scope *s, const std::string &descriptor)
{
    std::string::size_type i = 0;
    ast::Type *r = deserialize_type(s, descriptor, i);
    if (i != descriptor.length()) {
        internal_error("deserialize_type: " + descriptor + " " + std::to_string(i));
    }
    return r;
}

const ast::Expression *Analyzer::convert(const ast::Type *target, const ast::Expression *from)
{
    auto conv = target->make_converter(from->type);
    if (conv == nullptr) {
        return nullptr;
    }
    return conv(this, from);
}

bool Analyzer::convert2(const ast::Type *target, const ast::Expression *&left, const ast::Expression *&right)
{
    auto conv_left = target->make_converter(left->type);
    if (conv_left == nullptr) {
        return false;
    }
    auto conv_right = target->make_converter(right->type);
    if (conv_right == nullptr) {
        return false;
    }
    left = conv_left(this, left);
    right = conv_right(this, right);
    return true;
}

const ast::Statement *Analyzer::analyze(const pt::ImportDeclaration *declaration)
{
    if (declaration->alias.type == NONE && declaration->name.type == NONE) {
        if (scope.top()->lookupName(declaration->module.text) != nullptr && modules.find(declaration->module.text) != modules.end()) {
            return new ast::NullStatement(declaration->token.line);
        }
    }
    const Token &localname = declaration->alias.type != NONE ? declaration->alias : declaration->name.type != NONE ? declaration->name : declaration->module;
    if (not scope.top()->allocateName(localname, localname.text)) {
        error2(3114, localname, "duplicate definition of name", scope.top()->getDeclaration(localname.text), "first declaration here");
    }
    ast::Module *module = import_module(declaration->module, declaration->module.text, declaration->optional);
    if (declaration->name.type == NONE) {
        scope.top()->addName(declaration->token, localname.text, module);
    } else if (module != ast::MODULE_MISSING) {
        const ast::Name *name = module->scope->lookupName(declaration->name.text);
        if (name != nullptr) {
            scope.top()->addName(declaration->token, localname.text, module->scope->lookupName(declaration->name.text));
        } else {
            error(3176, declaration->name, "name not found in module");
        }
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze(const pt::TypeDeclaration *declaration)
{
    std::string name = declaration->token.text;
    if (not scope.top()->allocateName(declaration->token, name)) {
        error2(3013, declaration->token, "duplicate identifier", scope.top()->getDeclaration(name), "first declaration here");
    }
    ast::TypeClass *actual_class = nullptr;
    ast::TypeRecord *actual_record = nullptr;
    const pt::TypeRecord *classdecl = dynamic_cast<const pt::TypeClass *>(declaration->type.get());
    const pt::TypeRecord *recdecl = dynamic_cast<const pt::TypeRecord *>(declaration->type.get());
    if (classdecl != nullptr) {
        // Support recursive class type declarations.
        actual_class = new ast::TypeClass(classdecl->token, module_name, name, std::vector<ast::TypeRecord::Field>(), std::vector<const ast::Interface *>());
        scope.top()->addName(declaration->token, name, actual_class);
    } else if (recdecl != nullptr) {
        // Support recursive record type declarations.
        actual_record = new ast::TypeRecord(recdecl->token, module_name, name, std::vector<ast::TypeRecord::Field>());
        scope.top()->addName(declaration->token, name, actual_record);
    }
    const ast::Type *type = analyze(declaration->type.get(), name);
    if (actual_class != nullptr) {
        const ast::TypeClass *classtype = dynamic_cast<const ast::TypeClass *>(type);
        const_cast<std::vector<ast::TypeRecord::Field> &>(actual_class->fields) = classtype->fields;
        const_cast<std::map<std::string, size_t> &>(actual_class->field_names) = classtype->field_names;
        const_cast<std::vector<const ast::Interface *> &>(actual_class->interfaces) = classtype->interfaces;
        type = actual_class;
    } else if (actual_record != nullptr) {
        const ast::TypeRecord *rectype = dynamic_cast<const ast::TypeRecord *>(type);
        const_cast<std::vector<ast::TypeRecord::Field> &>(actual_record->fields) = rectype->fields;
        const_cast<std::map<std::string, size_t> &>(actual_record->field_names) = rectype->field_names;
        type = actual_record;
    } else {
        ast::Type *t = const_cast<ast::Type *>(type);
        if (type != ast::TYPE_BOOLEAN && type != ast::TYPE_NUMBER && type != ast::TYPE_STRING && type != ast::TYPE_BYTES) {
            const_cast<std::string &>(t->name) = name;
        }
        scope.top()->addName(declaration->token, name, t); // Still ugly.
    }
    const ast::TypeClass *classtype = dynamic_cast<const ast::TypeClass *>(type);
    if (classtype != nullptr) {
        scope.top()->resolveForward(name, classtype);
    }
    return new ast::TypeDeclarationStatement(declaration->token.line, name, type);
}

const ast::Statement *Analyzer::analyze_decl(const pt::ConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3014, declaration->token, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::ConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    const ast::Type *type = analyze(declaration->type.get());
    if (dynamic_cast<const ast::TypeClass *>(type) != nullptr) {
        error(3229, declaration->type->token, "class type not permitted as constant type");
    }
    const ast::Expression *value = analyze(declaration->value.get());
    value = convert(type, value);
    if (value == nullptr) {
        error(3015, declaration->value->token, "type mismatch");
    }
    if (not value->is_constant) {
        error(3016, declaration->value->token, "value must be constant");
    }
    if (type == ast::TYPE_BOOLEAN) {
        value = new ast::ConstantBooleanExpression(value->eval_boolean(declaration->value->token));
    } else if (type == ast::TYPE_NUMBER) {
        value = new ast::ConstantNumberExpression(value->eval_number(declaration->value->token));
    } else if (type == ast::TYPE_STRING) {
        value = new ast::ConstantStringExpression(value->eval_string(declaration->value->token));
    } else {
        ast::Variable *v = new ast::GlobalVariable(declaration->name, declaration->name.text, type, true);
        scope.top()->addName(v->declaration, v->name, v, true);
        return new ast::AssignmentStatement(declaration->token.line, {new ast::VariableExpression(v)}, value);
    }
    scope.top()->addName(declaration->name, name, new ast::Constant(declaration->name, name, value));
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_decl(const pt::NativeConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3206, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::NativeConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    const ast::Type *type = analyze(declaration->type.get());
    ast::Variable *v = frame.top()->createVariable(declaration->name, name, type, true);
    scope.top()->addName(v->declaration, v->name, v, true);
    return new ast::AssignmentStatement(
        declaration->token.line,
        {
            new ast::VariableExpression(v)
        },
        new ast::FunctionCall(
            new ast::VariableExpression(
                new ast::PredefinedFunction(
                    module_name + "$_CONSTANT_" + name,
                    new ast::TypeFunction(type, {}, false)
                )
            ),
            {}
        )
    );
}

const ast::Statement *Analyzer::analyze_decl(const pt::ExtensionConstantDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3245, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::ExtensionConstantDeclaration *declaration)
{
    const ast::Type *type = analyze(declaration->type.get());
    if (dynamic_cast<const ast::TypeClass *>(type) != nullptr) {
        error(3246, declaration->type->token, "class type not permitted as constant type");
    }
    ast::Variable *v = frame.top()->createVariable(declaration->name, declaration->name.text, type, true);
    scope.top()->addName(v->declaration, v->name, v, true);
    return new ast::AssignmentStatement(
        declaration->token.line,
        {
            new ast::VariableExpression(v)
        },
        new ast::FunctionCall(
            new ast::VariableExpression(
                new ast::ExtensionFunction(
                    Token(),
                    path_stripext(path_basename(program->source_path)),
                    declaration->name.text,
                    new ast::TypeFunction(type, {}, false)
                )
            ),
            {}
        )
    );
}

const ast::Statement *Analyzer::analyze_decl(const pt::VariableDeclaration *declaration)
{
    for (auto name: declaration->names) {
        if (not scope.top()->allocateName(name, name.text)) {
            error2(3038, name, "duplicate identifier", scope.top()->getDeclaration(name.text), "first declaration here");
        }
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::VariableDeclaration *declaration)
{
    const ast::Type *type = analyze(declaration->type.get());
    if (dynamic_cast<const ast::TypeClass *>(type) != nullptr) {
        error(3230, declaration->type->token, "class type not permitted as variable type (use POINTER TO)");
    }
    std::vector<ast::Variable *> variables;
    for (auto name: declaration->names) {
        ast::Variable *v = frame.top()->createVariable(name, name.text, type, false);
        variables.push_back(v);
    }
    std::vector<const ast::ReferenceExpression *> refs;
    const ast::Expression *expr = nullptr;
    if (declaration->value != nullptr) {
        expr = analyze(declaration->value.get());
        expr = convert(type, expr);
        if (expr == nullptr) {
            error(3113, declaration->value->token, "type mismatch");
        }
    }
    for (auto v: variables) {
        scope.top()->addName(v->declaration, v->name, v, true);
        refs.push_back(new ast::VariableExpression(v));
    }
    std::vector<const ast::Statement *> r;
    for (auto v: variables) {
        r.push_back(new ast::DeclarationStatement(declaration->token.line, v));
    }
    if (declaration->value != nullptr) {
        r.push_back(new ast::AssignmentStatement(declaration->token.line, refs, expr));
    } else {
        r.push_back(new ast::ResetStatement(declaration->token.line, refs));
    }
    return new ast::CompoundStatement(declaration->token.line, r);
}

const ast::Statement *Analyzer::analyze_decl(const pt::NativeVariableDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3225, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::NativeVariableDeclaration *declaration)
{
    //std::string name = declaration->name.text;
    //scope.top()->addName(declaration->name, name, new Constant(declaration->name, name, get_native_constant_value(module_name + "$" + name)));
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_decl(const pt::LetDeclaration *declaration)
{
    if (not scope.top()->allocateName(declaration->name, declaration->name.text)) {
        error2(3139, declaration->name, "duplicate identifier", scope.top()->getDeclaration(declaration->name.text), "first declaration here");
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::LetDeclaration *declaration)
{
    const ast::Type *type = analyze(declaration->type.get());
    if (dynamic_cast<const ast::TypeClass *>(type) != nullptr) {
        error(3231, declaration->type->token, "class type not permitted as variable type (use POINTER TO)");
    }
    const ast::Expression *expr = analyze(declaration->value.get());
    expr = convert(type, expr);
    if (expr == nullptr) {
        error(3140, declaration->value->token, "type mismatch");
    }
    const ast::TypePointer *ptype = dynamic_cast<const ast::TypePointer *>(type);
    if (ptype != nullptr && dynamic_cast<const ast::NewClassExpression *>(expr) != nullptr) {
        type = new ast::TypeValidPointer(Token(), ptype->reftype);
    }
    ast::Variable *v = frame.top()->createVariable(declaration->name, declaration->name.text, type, true);
    scope.top()->addName(v->declaration, v->name, v, true);
    std::vector<const ast::ReferenceExpression *> refs;
    refs.push_back(new ast::VariableExpression(v));
    return new ast::AssignmentStatement(declaration->token.line, refs, expr);
}

const ast::Statement *Analyzer::analyze_decl(const pt::FunctionDeclaration *declaration)
{
    const std::string classtype = declaration->type.text;
    ast::Type *type = nullptr;
    if (not classtype.empty()) {
        ast::Name *tname = scope.top()->lookupName(classtype);
        if (tname == nullptr) {
            auto decl = scope.top()->getDeclaration(classtype);
            if (decl.type == NONE) {
                error(3126, declaration->type, "type name not found");
            } else {
                error2(3127, declaration->type, "type name is not a type", decl, "declaration here");
            }
        }
        type = dynamic_cast<ast::Type *>(tname);
        if (type == nullptr) {
            error2(3138, declaration->type, "type name is not a type", tname->declaration, "declaration here");
        }
    }
    std::string name = declaration->name.text;
    if (type == nullptr && not scope.top()->allocateName(declaration->name, name)) {
        error2(3047, declaration->name, "duplicate definition of name", scope.top()->getDeclaration(name), "first declaration here");
    }
    const ast::Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype.get()) : ast::TYPE_NOTHING;
    std::vector<ast::FunctionParameter *> args;
    bool variadic = false;
    bool in_default = false;
    for (auto &x: declaration->args) {
        if (variadic) {
            error(3268, x->token, "varargs function parameter must be last");
        }
        ast::ParameterType::Mode mode = ast::ParameterType::Mode::IN;
        switch (x->mode) {
            case pt::FunctionParameterGroup::Mode::IN:    mode = ast::ParameterType::Mode::IN;       break;
            case pt::FunctionParameterGroup::Mode::INOUT: mode = ast::ParameterType::Mode::INOUT;    break;
            case pt::FunctionParameterGroup::Mode::OUT:   mode = ast::ParameterType::Mode::OUT;      break;
        }
        const ast::Type *ptype = analyze(x->type.get());
        if (dynamic_cast<const ast::TypeClass *>(ptype) != nullptr) {
            error(3232, x->type->token, "class type not permitted as function parameter (use pointers)");
        }
        if (type != nullptr && args.empty()) {
            const ast::TypeClass *ctype = dynamic_cast<const ast::TypeClass *>(type);
            if (ctype != nullptr) {
                const ast::TypeValidPointer *vptype = dynamic_cast<const ast::TypeValidPointer *>(ptype);
                if (vptype == nullptr || (vptype != nullptr && vptype->reftype != type)) {
                    error(3224, x->type->token, "valid pointer type expected");
                }
            } else {
                if (ptype != type) {
                    error(3128, x->type->token, "expected self parameter");
                }
            }
            if (mode == ast::ParameterType::Mode::OUT) {
                error(3278, x->token, "self parameter must be IN or INOUT");
            }
        }
        if (x->varargs) {
            if (x->names.size() > 1) {
                error(3269, x->names[1], "varargs must be single parameter name");
            }
            ptype = new ast::TypeArray(Token(), ptype);
            variadic = true;
        }
        const ast::Expression *def = nullptr;
        if (x->default_value != nullptr) {
            const bool is_dummy = dynamic_cast<pt::DummyExpression *>(x->default_value.get()) != nullptr;
            switch (mode) {
                case ast::ParameterType::Mode::IN:
                    break;
                case ast::ParameterType::Mode::INOUT:
                case ast::ParameterType::Mode::OUT:
                    if (not is_dummy) {
                        error(3175, x->default_value->token, "default value only available for IN parameters");
                    }
                    break;
            }
            in_default = true;
            def = analyze(x->default_value.get());
            if (not is_dummy && not def->is_constant) {
                error(3148, x->default_value->token, "default value not constant");
            }
        } else if (in_default) {
            error(3150, x->token, "default value must be specified for this parameter");
        }
        for (auto argname: x->names) {
            if (scope.top()->lookupName(argname.text)) {
                error(3174, argname, "duplicate identifier");
            }
            ast::FunctionParameter *fp = new ast::FunctionParameter(argname, argname.text, ptype, frame.top()->get_depth()+1, mode, def);
            args.push_back(fp);
        }
    }
    if (type != nullptr) {
        if (args.empty()) {
            error(3129, declaration->rparen, "expected self parameter");
        }
        const ast::TypeClass *ctype = dynamic_cast<const ast::TypeClass *>(type);
        if (ctype != nullptr) {
            for (auto i: ctype->interfaces) {
                for (auto m: i->methods) {
                    if (name == m.first.text) {
                        if (declaration->args.size() != m.second->params.size()) {
                            error(3255, declaration->rparen, "wrong number of arguments");
                        }
                        bool first = true;
                        for (size_t a = 0; a < args.size(); a++) {
                            if (first) {
                                first = false;
                                continue;
                            }
                            if (args[a]->type != m.second->params[a]->type) {
                                error(3256, args[a]->declaration, "parameter types must match interface");
                            }
                        }
                    }
                }
            }
        }
    }
    ast::Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(name);
        if (f != type->methods.end()) {
            function = dynamic_cast<ast::Function *>(f->second);
        } else {
            function = new ast::Function(declaration->name, type->name + "." + name, returntype, frame.top(), scope.top(), args, variadic, frame.top()->get_depth()+1);
            type->methods[name] = function;
        }
    } else {
        ast::Name *ident = scope.top()->lookupName(name);
        function = dynamic_cast<ast::Function *>(ident);
        if (function == nullptr) {
            function = new ast::Function(declaration->name, name, returntype, frame.top(), scope.top(), args, variadic, frame.top()->get_depth()+1);
            scope.top()->addName(declaration->name, name, function);
        }
    }
    return new ast::DeclarationStatement(declaration->token.line, function);
}

const ast::Statement *Analyzer::analyze_body(const pt::FunctionDeclaration *declaration)
{
    const std::string classtype = declaration->type.text;
    ast::Type *type = nullptr;
    if (not classtype.empty()) {
        ast::Name *tname = scope.top()->lookupName(classtype);
        if (tname == nullptr) {
            internal_error("type name not found");
        }
        type = dynamic_cast<ast::Type *>(tname);
        if (type == nullptr) {
            internal_error("type name is not a type");
        }
    }
    ast::Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(declaration->name.text);
        function = dynamic_cast<ast::Function *>(f->second);
    } else {
        function = dynamic_cast<ast::Function *>(scope.top()->lookupName(declaration->name.text));
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
    functiontypes.push(std::make_pair(type, dynamic_cast<const ast::TypeFunction *>(function->type)));
    loops.push(std::list<std::pair<std::string, unsigned int>>());
    function->statements = analyze(declaration->body);
    const ast::Type *returntype = dynamic_cast<const ast::TypeFunction *>(function->type)->returntype;
    if (returntype != ast::TYPE_NOTHING) {
        if (function->statements.empty() || not function->statements.back()->always_returns()) {
            error(3085, declaration->end_function, "missing RETURN statement");
        }
    }
    loops.pop();
    functiontypes.pop();
    scope.top()->checkForward();
    scope.pop();
    frame.pop();
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze(const pt::NativeFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3166, declaration->name, "duplicate identifier", scope.top()->getDeclaration(name), "first declaration here");
    }
    const ast::TypeFunction *ftype = analyze_function_type(declaration->returntype, declaration->args);
    ast::PredefinedFunction *function = new ast::PredefinedFunction(path_stripext(path_basename(program->source_path))+"$"+name, ftype);
    scope.top()->addName(declaration->name, name, function);
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze(const pt::ExtensionFunctionDeclaration *declaration)
{
    std::string name = declaration->name.text;
    if (not scope.top()->allocateName(declaration->name, name)) {
        error2(3242, declaration->name, "duplicate identifier", scope.top()->getDeclaration(name), "first declaration here");
    }
    const ast::TypeFunction *ftype = analyze_function_type(declaration->returntype, declaration->args);
    ast::ExtensionFunction *function = new ast::ExtensionFunction(declaration->name, path_stripext(path_basename(program->source_path)), name, ftype);
    scope.top()->addName(declaration->name, name, function);
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze(const pt::ExceptionDeclaration *declaration)
{
    std::string name = declaration->name[0].text;
    if (declaration->name.size() == 1) {
        if (not scope.top()->allocateName(declaration->token, name)) {
            error2(3115, declaration->token, "duplicate definition of name", scope.top()->getDeclaration(name), "first declaration here");
        }
        if (name.length() < 9 || name.substr(name.length()-9) != "Exception") {
            error(3198, declaration->token, "Exception name must end in 'Exception'");
        }
        scope.top()->addName(declaration->token, name, new ast::Exception(declaration->token, name));
    } else {
        ast::Name *n = scope.top()->lookupName(name);
        if (n == nullptr) {
            error(3235, declaration->token, "name not found");
        }
        ast::Exception *e = dynamic_cast<ast::Exception *>(n);
        if (e == nullptr) {
            error(3236, declaration->token, "name is not an exception");
        }
        std::string fullname = name;
        ast::Exception *s = e;
        for (size_t i = 1; i < declaration->name.size()-1; i++) {
            auto t = s->subexceptions.find(declaration->name[i].text);
            if (t == s->subexceptions.end()) {
                error(3237, declaration->name[i], "subexception not found");
            }
            fullname += "." + declaration->name[i].text;
            s = t->second;
        }
        std::string lastname = declaration->name[declaration->name.size()-1].text;
        auto t = s->subexceptions.find(lastname);
        if (t != s->subexceptions.end()) {
            error(3238, declaration->name[declaration->name.size()-1], "subexception already declared");
        }
        fullname += "." + declaration->name[declaration->name.size()-1].text;
        ast::Exception *se = new ast::Exception(declaration->token, fullname);
        s->subexceptions[lastname] = se;
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze(const pt::InterfaceDeclaration *declaration)
{
    std::string name = declaration->names[0].text;
    if (not scope.top()->allocateName(declaration->token, name)) {
        error2(3251, declaration->token, "duplicate definition of name", scope.top()->getDeclaration(name), "first declaration here");
    }
    scope.push(new ast::Scope(scope.top(), frame.top()));
    scope.top()->addName(declaration->token, name, ast::TYPE_INTERFACE);
    std::vector<std::pair<Token, const ast::TypeFunction *>> methods;
    std::map<std::string, Token> method_names;
    for (auto &x: declaration->methods) {
        std::string methodname = x.first.text;
        auto prev = method_names.find(methodname);
        if (prev != method_names.end()) {
            error2(3247, x.first, "duplicate method: " + x.first.text, prev->second, "first declaration here");
        }
        const ast::Type *t = analyze(x.second.get());
        const ast::TypeFunctionPointer *fp = dynamic_cast<const ast::TypeFunctionPointer *>(t);
        if (fp == nullptr) {
            internal_error("expected function type");
        }
        if (fp->functype->params.size() == 0) {
            error(3253, x.first, "interface function must have at least one parameter");
        }
        if (fp->functype->params[0]->type != ast::TYPE_INTERFACE) {
            error(3254, fp->functype->params[0]->declaration, "first parameter must be interface type");
        }
        methods.push_back(std::make_pair(x.first, fp->functype));
        method_names[methodname] = x.first;
    }
    scope.pop();
    scope.top()->addName(declaration->token, name, new ast::Interface(declaration->token, name, methods));
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_decl(const pt::ExportDeclaration *declaration)
{
    if (declaration->declaration != nullptr) {
        std::vector<const ast::Statement *> statements;
        DeclarationAnalyzer da(this, statements);
        declaration->declaration->accept(&da);
        if (not statements.empty()) {
            return statements[0];
        }
    }
    return new ast::NullStatement(declaration->token.line);
}

const ast::Statement *Analyzer::analyze_body(const pt::ExportDeclaration *declaration)
{
    for (auto &name: declaration->names) {
        if (scope.top()->getDeclaration(name.text).type == NONE) {
            error(3152, name, "export name not declared");
        }
        exports[name.text] = name;
    }
    if (declaration->declaration != nullptr) {
        std::vector<const ast::Statement *> statements;
        StatementAnalyzer sa(this, statements);
        declaration->declaration->accept(&sa);
        if (not statements.empty()) {
            return statements[0];
        }
    }
    return new ast::NullStatement(declaration->token.line);
}

std::vector<const ast::Statement *> Analyzer::analyze(const std::vector<std::unique_ptr<pt::Statement>> &statement)
{
    std::vector<const ast::Statement *> statements;
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
        deconstruct(re->range->first.get(), parts);
        deconstruct(re->range->last.get(), parts);
    } else if (dynamic_cast<const pt::BooleanLiteralExpression *>(expr) != nullptr
            || dynamic_cast<const pt::NumberLiteralExpression *>(expr) != nullptr
            || dynamic_cast<const pt::StringLiteralExpression *>(expr) != nullptr) {
        return;
    }
    parts.push_back(expr);
}

const ast::Statement *Analyzer::analyze(const pt::AssertStatement *statement)
{
    const pt::Expression *e = statement->exprs[0].get();
    const ast::Expression *expr = analyze(e);
    expr = convert(ast::TYPE_BOOLEAN, expr);
    if (expr == nullptr) {
        error(3173, statement->exprs[0]->token, "boolean value expected");
    }
    std::vector<const pt::Expression *> parts;
    deconstruct(e, parts);
    for (auto x = statement->exprs.begin()+1; x != statement->exprs.end(); ++x) {
        parts.push_back(x->get());
    }
    const ast::Module *textio = import_module(Token(), "textio", false);
    if (textio == nullptr) {
        internal_error("need module textio");
    }
    std::vector<const ast::Statement *> statements;
    const ast::Variable *textio_stderr = dynamic_cast<const ast::Variable *>(textio->scope->lookupName("stderr"));
    if (textio_stderr == nullptr) {
        internal_error("need textio.stderr");
    }
    const ast::PredefinedFunction *writeLine = dynamic_cast<const ast::PredefinedFunction *>(textio->scope->lookupName("writeLine"));
    if (writeLine == nullptr) {
        internal_error("need textio.writeLine");
    }
    statements.push_back(
        new ast::ExpressionStatement(
            statement->token.line,
            new ast::FunctionCall(
                new ast::VariableExpression(writeLine),
                {
                    new ast::VariableExpression(textio_stderr),
                    new ast::ConstantStringExpression(utf8string("Assert failed (" + statement->token.file() + " line " + std::to_string(statement->token.line) + "):"))
                }
            )
        )
    );
    for (int line = e->get_start_token().line; line <= e->get_end_token().line; line++) {
        statements.push_back(
            new ast::ExpressionStatement(
                statement->token.line,
                new ast::FunctionCall(
                    new ast::VariableExpression(writeLine),
                    {
                        new ast::VariableExpression(textio_stderr),
                        new ast::ConstantStringExpression(utf8string(statement->token.source->source_line(line)))
                    }
                )
            )
        );
    }
    statements.push_back(
        new ast::ExpressionStatement(
            statement->token.line,
            new ast::FunctionCall(
                new ast::VariableExpression(writeLine),
                {
                    new ast::VariableExpression(textio_stderr),
                    new ast::ConstantStringExpression(utf8string("Assert expression dump:"))
                }
            )
        )
    );
    std::set<std::string> seen;
    for (auto part: parts) {
        const Token &start = part->get_start_token();
        const Token &end = part->get_end_token();
        std::string str;
        if (start.line == end.line) {
            str = start.source_line().substr(start.column-1, end.column + end.text.length() - start.column);
        } else {
            str = start.source_line().substr(start.column-1);
            for (int line = start.line + 1; line < end.line; line++) {
                str += start.source->source_line(line);
            }
            str += end.source_line().substr(0, end.column + end.text.length());
        }
        if (seen.find(str) != seen.end()) {
            continue;
        }
        seen.insert(str);

        // Instead of directly constructing an AST fragment here, construct a temporary
        // parse tree so we can leverage the analyze step for InterpolatedStringExpression
        // (this takes care of the call to .toString()).
        std::vector<std::pair<std::unique_ptr<pt::Expression>, Token>> iparts;
        iparts.push_back(std::make_pair(std::unique_ptr<pt::Expression> { new pt::StringLiteralExpression(Token(), Token(), utf8string("  " + str + " is ")) }, Token()));
        iparts.push_back(std::make_pair(std::unique_ptr<pt::Expression> { const_cast<pt::Expression *>(part) }, Token()));
        std::unique_ptr<pt::InterpolatedStringExpression> ie { new pt::InterpolatedStringExpression(Token(), std::move(iparts)) };
        try {
            statements.push_back(
                new ast::ExpressionStatement(
                    statement->token.line,
                    new ast::FunctionCall(
                        new ast::VariableExpression(writeLine),
                        {
                            new ast::VariableExpression(textio_stderr),
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
    return new ast::AssertStatement(statement->token.line, statements, expr, statement->source);
}

const ast::Statement *Analyzer::analyze(const pt::AssignmentStatement *statement)
{
    if (statement->variables.size() != 1) {
        internal_error("unexpected multiple assign statement");
    }
    const ast::Expression *expr = analyze(statement->variables[0].get());
    const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(expr);
    if (ref == nullptr) {
        error(3058, statement->variables[0]->token, "expression is not assignable");
    }
    if (expr->is_readonly) {
        error(3105, statement->variables[0]->token, "assignment to readonly expression");
    }
    const ast::Expression *rhs = analyze(statement->expr.get());
    rhs = convert(expr->type, rhs);
    if (rhs == nullptr) {
        error(3057, statement->expr->token, "type mismatch");
    }
    std::vector<const ast::ReferenceExpression *> vars;
    vars.push_back(ref);
    return new ast::AssignmentStatement(statement->token.line, vars, rhs);
}

const ast::Statement *Analyzer::analyze(const pt::CaseStatement *statement)
{
    const ast::Expression *expr = analyze(statement->expr.get());
    if (expr->type != ast::TYPE_NUMBER && expr->type != ast::TYPE_STRING && dynamic_cast<const ast::TypeEnum *>(expr->type) == nullptr && expr->type != ast::TYPE_OBJECT) {
        error(3050, statement->expr->token, "CASE expression must be Number, String, or ENUM");
    }
    std::vector<std::pair<std::vector<const ast::CaseStatement::WhenCondition *>, std::vector<const ast::Statement *>>> clauses;
    for (auto &x: statement->clauses) {
        std::vector<const ast::CaseStatement::WhenCondition *> conditions;
        for (auto &c: x.first) {
            const pt::CaseStatement::ComparisonWhenCondition *cwc = dynamic_cast<const pt::CaseStatement::ComparisonWhenCondition *>(c.get());
            const pt::CaseStatement::RangeWhenCondition *rwc = dynamic_cast<const pt::CaseStatement::RangeWhenCondition *>(c.get());
            const pt::CaseStatement::TypeTestWhenCondition *twc = dynamic_cast<const pt::CaseStatement::TypeTestWhenCondition *>(c.get());
            if (cwc != nullptr) {
                const ast::Expression *when = analyze(cwc->expr.get());
                if (when->type != expr->type) {
                    error(3051, cwc->expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(3052, cwc->expr->token, "WHEN condition must be constant");
                }
                ast::ComparisonExpression::Comparison comp = static_cast<ast::ComparisonExpression::Comparison>(cwc->comp); // TODO: remove cast
                const ast::CaseStatement::WhenCondition *cond = new ast::CaseStatement::ComparisonWhenCondition(cwc->expr->token, comp, when);
                for (auto clause: clauses) {
                    for (auto cc: clause.first) {
                        if (cond->overlaps(cc)) {
                            error2(3062, cwc->expr->token, "overlapping case condition", cc->token, "overlaps here");
                        }
                    }
                }
                for (auto cc: conditions) {
                    if (cond->overlaps(cc)) {
                        error2(3063, cwc->expr->token, "overlapping case condition", cc->token, "overlaps here");
                    }
                }
                conditions.push_back(cond);
            } else if (rwc != nullptr) {
                const ast::Expression *when = analyze(rwc->low_expr.get());
                if (when->type != expr->type) {
                    error(3053, rwc->low_expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(3054, rwc->low_expr->token, "WHEN condition must be constant");
                }
                const ast::Expression *when2 = analyze(rwc->high_expr.get());
                if (when2->type != expr->type) {
                    error(3055, rwc->high_expr->token, "type mismatch");
                }
                if (not when2->is_constant) {
                    error(3056, rwc->high_expr->token, "WHEN condition must be constant");
                }
                if (when->type == ast::TYPE_NUMBER || dynamic_cast<const ast::TypeEnum *>(when->type) != nullptr) {
                    if (number_is_greater(when->eval_number(rwc->low_expr->token), when2->eval_number(rwc->high_expr->token))) {
                        error(3109, rwc->high_expr->token, "WHEN numeric range condition must be low..high");
                    }
                } else if (when->type == ast::TYPE_STRING) {
                    if (when->eval_string(rwc->low_expr->token) > when2->eval_string(rwc->high_expr->token)) {
                        error(3110, rwc->high_expr->token, "WHEN string range condition must be low..high");
                    }
                } else {
                    internal_error("range condition type");
                }
                const ast::CaseStatement::WhenCondition *cond = new ast::CaseStatement::RangeWhenCondition(rwc->low_expr->token, when, when2);
                for (auto clause: clauses) {
                    for (auto cc: clause.first) {
                        if (cond->overlaps(cc)) {
                            error2(3064, rwc->low_expr->token, "overlapping case condition", cc->token, "overlaps here");
                        }
                    }
                }
                for (auto cc: conditions) {
                    if (cond->overlaps(cc)) {
                        error2(3065, rwc->low_expr->token, "overlapping case condition", cc->token, "overlaps here");
                    }
                }
                conditions.push_back(cond);
            } else if (twc != nullptr) {
                if (expr->type != ast::TYPE_OBJECT) {
                    error2(3263, statement->expr->token, "WHEN ISA requires Object type", twc->token, "used here");
                }
                const ast::Type *target = analyze(twc->target.get());
                const ast::CaseStatement::WhenCondition *cond = new ast::CaseStatement::TypeTestWhenCondition(twc->target->token, convert(target, expr), target);
                conditions.push_back(cond);
            } else {
                internal_error("unknown case when condition");
            }
        }
        scope.push(new ast::Scope(scope.top(), frame.top()));
        std::vector<const ast::Statement *> statements = analyze(x.second);
        scope.pop();
        clauses.emplace_back(std::make_pair(conditions, statements));
    }
    const ast::TypeEnum *te = dynamic_cast<const ast::TypeEnum *>(expr->type);
    if (te != nullptr) {
        std::set<int> covered;
        for (auto &clause: clauses) {
            for (auto &cond: clause.first) {
                const ast::CaseStatement::ComparisonWhenCondition *cwc = dynamic_cast<const ast::CaseStatement::ComparisonWhenCondition *>(cond);
                const ast::CaseStatement::RangeWhenCondition *rwc = dynamic_cast<const ast::CaseStatement::RangeWhenCondition *>(cond);
                const ast::CaseStatement::TypeTestWhenCondition *twc = dynamic_cast<const ast::CaseStatement::TypeTestWhenCondition *>(cond);
                if (cwc != nullptr) {
                    const ast::ConstantEnumExpression *ce = dynamic_cast<const ast::ConstantEnumExpression *>(cwc->expr);
                    if (ce == nullptr) {
                        internal_error("expected ConstantEnumExpression");
                    }
                    switch (cwc->comp) {
                        case ast::ComparisonExpression::Comparison::EQ:
                            covered.insert(ce->value);
                            break;
                        case ast::ComparisonExpression::Comparison::NE:
                            for (int i = 0; i < static_cast<int>(te->names.size()); i++) {
                                if (i != ce->value) {
                                    covered.insert(i);
                                }
                            }
                            break;
                        case ast::ComparisonExpression::Comparison::LT:
                            for (int i = 0; i < ce->value; i++) {
                                covered.insert(i);
                            }
                            break;
                        case ast::ComparisonExpression::Comparison::GT:
                            for (int i = ce->value+1; i < static_cast<int>(te->names.size()); i++) {
                                covered.insert(i);
                            }
                            break;
                        case ast::ComparisonExpression::Comparison::LE:
                            for (int i = 0; i <= ce->value; i++) {
                                covered.insert(i);
                            }
                            break;
                        case ast::ComparisonExpression::Comparison::GE:
                            for (int i = ce->value; i < static_cast<int>(te->names.size()); i++) {
                                covered.insert(i);
                            }
                            break;
                        default:
                            internal_error("unimplemented comparison in case when condition");
                    }
                } else if (rwc != nullptr) {
                    const ast::ConstantEnumExpression *le = dynamic_cast<const ast::ConstantEnumExpression *>(rwc->low_expr);
                    if (le == nullptr) {
                        internal_error("expected ConstantEnumExpression");
                    }
                    const ast::ConstantEnumExpression *he = dynamic_cast<const ast::ConstantEnumExpression *>(rwc->high_expr);
                    if (he == nullptr) {
                        internal_error("expected ConstantEnumExpression");
                    }
                    for (int i = le->value; i <= he->value; i++) {
                        covered.insert(i);
                    }
                } else if (twc != nullptr) {
                    internal_error("unexpected case when condition");
                } else {
                    internal_error("unknown case when condition");
                }
            }
            if (clause.first.empty()) {
                for (size_t i = 0; i < te->names.size(); i++) {
                    covered.insert(i);
                }
            }
        }
        if (covered.size() < te->names.size()) {
            error(3283, statement->expr->token, "cases do not cover all enum values");
        }
    }
    return new ast::CaseStatement(statement->token.line, expr, clauses);
}

namespace overlap {

static bool operator==(const Number &x, const Number &y) { return number_is_equal(x, y); }
static bool operator!=(const Number &x, const Number &y) { return number_is_not_equal(x, y); }
static bool operator<(const Number &x, const Number &y) { return number_is_less(x, y); }
static bool operator>(const Number &x, const Number &y) { return number_is_greater(x, y); }
static bool operator<=(const Number &x, const Number &y) { return number_is_less_equal(x, y); }
static bool operator>=(const Number &x, const Number &y) { return number_is_greater_equal(x, y); }

template <typename T> bool check(ast::ComparisonExpression::Comparison comp1, const T &value1, ast::ComparisonExpression::Comparison comp2, const T &value2)
{
    switch (comp1) {
        case ast::ComparisonExpression::Comparison::EQ:
            switch (comp2) {
                case ast::ComparisonExpression::Comparison::EQ: return value1 == value2;
                case ast::ComparisonExpression::Comparison::NE: return value1 != value2;
                case ast::ComparisonExpression::Comparison::LT: return value1 < value2;
                case ast::ComparisonExpression::Comparison::GT: return value1 > value2;
                case ast::ComparisonExpression::Comparison::LE: return value1 <= value2;
                case ast::ComparisonExpression::Comparison::GE: return value1 >= value2;
            }
            break;
        case ast::ComparisonExpression::Comparison::NE:
            switch (comp2) {
                case ast::ComparisonExpression::Comparison::EQ: return value1 != value2;
                default: return true;
            }
            break;
        case ast::ComparisonExpression::Comparison::LT:
            switch (comp2) {
                case ast::ComparisonExpression::Comparison::EQ: return value1 < value2;
                case ast::ComparisonExpression::Comparison::NE: return true;
                case ast::ComparisonExpression::Comparison::LT: return true;
                case ast::ComparisonExpression::Comparison::GT: return value1 > value2;
                case ast::ComparisonExpression::Comparison::LE: return true;
                case ast::ComparisonExpression::Comparison::GE: return value1 > value2;
            }
            break;
        case ast::ComparisonExpression::Comparison::GT:
            switch (comp2) {
                case ast::ComparisonExpression::Comparison::EQ: return value1 > value2;
                case ast::ComparisonExpression::Comparison::NE: return true;
                case ast::ComparisonExpression::Comparison::LT: return value1 < value2;
                case ast::ComparisonExpression::Comparison::GT: return true;
                case ast::ComparisonExpression::Comparison::LE: return value1 < value2;
                case ast::ComparisonExpression::Comparison::GE: return true;
            }
            break;
        case ast::ComparisonExpression::Comparison::LE:
            switch (comp2) {
                case ast::ComparisonExpression::Comparison::EQ: return value1 >= value2;
                case ast::ComparisonExpression::Comparison::NE: return true;
                case ast::ComparisonExpression::Comparison::LT: return true;
                case ast::ComparisonExpression::Comparison::GT: return value1 > value2;
                case ast::ComparisonExpression::Comparison::LE: return true;
                case ast::ComparisonExpression::Comparison::GE: return value1 >= value2;
            }
            break;
        case ast::ComparisonExpression::Comparison::GE:
            switch (comp2) {
                case ast::ComparisonExpression::Comparison::EQ: return value1 <= value2;
                case ast::ComparisonExpression::Comparison::NE: return true;
                case ast::ComparisonExpression::Comparison::LT: return value1 < value2;
                case ast::ComparisonExpression::Comparison::GT: return true;
                case ast::ComparisonExpression::Comparison::LE: return value1 <= value2;
                case ast::ComparisonExpression::Comparison::GE: return true;
            }
            break;
    }
    return false;
}

template <typename T> bool check(ast::ComparisonExpression::Comparison comp1, const T &value1, const T &value2low, const T &value2high)
{
    switch (comp1) {
        case ast::ComparisonExpression::Comparison::EQ: return value1 >= value2low && value1 <= value2high;
        case ast::ComparisonExpression::Comparison::NE: return value1 != value2low || value1 != value2high;
        case ast::ComparisonExpression::Comparison::LT: return value1 > value2low;
        case ast::ComparisonExpression::Comparison::GT: return value1 > value2high;
        case ast::ComparisonExpression::Comparison::LE: return value1 >= value2low;
        case ast::ComparisonExpression::Comparison::GE: return value1 <= value2high;
    }
    return false;
}

template <typename T> bool check(const T &value1low, const T &value1high, const T &value2low, const T &value2high)
{
    return value1high >= value2low && value1low <= value2high;
}

} // namespace overlap

bool ast::CaseStatement::ComparisonWhenCondition::overlaps(const WhenCondition *cond) const
{
    const ComparisonWhenCondition *cwhen = dynamic_cast<const ComparisonWhenCondition *>(cond);
    const RangeWhenCondition *rwhen = dynamic_cast<const RangeWhenCondition *>(cond);
    if (cwhen != nullptr) {
        if (expr->type == ast::TYPE_NUMBER || dynamic_cast<const ast::TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(cond->token), cwhen->comp, cwhen->expr->eval_number(cond->token));
        } else if (expr->type == ast::TYPE_STRING) {
            return overlap::check(comp, expr->eval_string(cond->token), cwhen->comp, cwhen->expr->eval_string(cond->token));
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (expr->type == ast::TYPE_NUMBER || dynamic_cast<const ast::TypeEnum *>(expr->type) != nullptr) {
            return overlap::check(comp, expr->eval_number(cond->token), rwhen->low_expr->eval_number(cond->token), rwhen->high_expr->eval_number(cond->token));
        } else if (expr->type == ast::TYPE_STRING) {
            return overlap::check(comp, expr->eval_string(cond->token), rwhen->low_expr->eval_string(cond->token), rwhen->high_expr->eval_string(cond->token));
        } else {
            internal_error("ComparisonWhenCondition");
        }
    } else {
        internal_error("ComparisonWhenCondition");
    }
}

bool ast::CaseStatement::RangeWhenCondition::overlaps(const WhenCondition *cond) const
{
    const ComparisonWhenCondition *cwhen = dynamic_cast<const ComparisonWhenCondition *>(cond);
    const RangeWhenCondition *rwhen = dynamic_cast<const RangeWhenCondition *>(cond);
    if (cwhen != nullptr) {
        if (low_expr->type == ast::TYPE_NUMBER || dynamic_cast<const ast::TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_number(cwhen->token), low_expr->eval_number(cwhen->token), high_expr->eval_number(cwhen->token));
        } else if (low_expr->type == ast::TYPE_STRING) {
            return overlap::check(cwhen->comp, cwhen->expr->eval_string(cwhen->token), low_expr->eval_string(cwhen->token), high_expr->eval_string(cwhen->token));
        } else {
            internal_error("RangeWhenCondition");
        }
    } else if (rwhen != nullptr) {
        if (low_expr->type == ast::TYPE_NUMBER || dynamic_cast<const ast::TypeEnum *>(low_expr->type) != nullptr) {
            return overlap::check(low_expr->eval_number(rwhen->token), high_expr->eval_number(rwhen->token), rwhen->low_expr->eval_number(rwhen->token), rwhen->high_expr->eval_number(rwhen->token));
        } else if (low_expr->type == ast::TYPE_STRING) {
            return overlap::check(low_expr->eval_string(rwhen->token), high_expr->eval_string(rwhen->token), rwhen->low_expr->eval_string(rwhen->token), rwhen->high_expr->eval_string(rwhen->token));
        } else {
            internal_error("RangeWhenCondition");
        }
    } else {
        internal_error("RangeWhenCondition");
    }
}

bool ast::CaseStatement::TypeTestWhenCondition::overlaps(const WhenCondition *cond) const
{
    const TypeTestWhenCondition *twhen = dynamic_cast<const TypeTestWhenCondition *>(cond);
    if (twhen != nullptr) {
        return target == twhen->target;
    } else {
        internal_error("TypeTestWhenCondition");
    }
}

const ast::Statement *Analyzer::analyze(const pt::CheckStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> condition_statements;
    const ast::Expression *cond { nullptr };
    const pt::ValidPointerExpression *valid = dynamic_cast<const pt::ValidPointerExpression *>(statement->cond.get());
    if (valid != nullptr) {
        for (auto &v: valid->tests) {
            if (not v->shorthand and scope.top()->lookupName(v->name.text) != nullptr) {
                error2(3274, v->name, "duplicate identifier", scope.top()->getDeclaration(v->name.text), "first declaration here");
            }
            const ast::Expression *ptr = analyze(v->expr.get());
            const ast::TypePointer *ptrtype = dynamic_cast<const ast::TypePointer *>(ptr->type);
            if (ptrtype == nullptr) {
                error(3273, v->expr->token, "pointer type expression expected");
            }
            const ast::TypeValidPointer *vtype = new ast::TypeValidPointer(Token(), ptrtype->reftype);
            ast::Variable *var;
            // TODO: Try to make this a local variable always (give the global scope a local space).
            if (functiontypes.empty()) {
                var = new ast::GlobalVariable(v->name, v->name.text, vtype, true);
            } else {
                // TODO: probably use frame.top()->get_depth() (add IF VALID to repl tests)
                var = new ast::LocalVariable(v->name, v->name.text, vtype, frame.size()-1, true);
            }
            scope.top()->parent->replaceName(v->name, v->name.text, var);
            const ast::Expression *ve = new ast::ValidPointerExpression(var, ptr);
            if (cond == nullptr) {
                cond = ve;
            } else {
                cond = new ast::ConjunctionExpression(cond, ve);
            }
        }
    } else {
        cond = analyze(statement->cond.get());
        cond = convert(ast::TYPE_BOOLEAN, cond);
        if (cond == nullptr) {
            error(3199, statement->cond->token, "boolean value expected");
        }
    }
    condition_statements.push_back(std::make_pair(cond, std::vector<const ast::Statement *>()));
    std::vector<const ast::Statement *> else_statements = analyze(statement->body);
    if (else_statements.empty()) {
        error(3200, statement->token, "body cannot be empty");
    }
    if (not else_statements.back()->is_scope_exit_statement()) {
        error(3201, statement->body.back()->token, "CHECK body must end in EXIT, NEXT, RAISE, or RETURN");
    }
    scope.pop();
    return new ast::IfStatement(statement->token.line, condition_statements, else_statements);
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

void Analyzer::process_into_results(const pt::ExecStatement *statement, const std::string &sql, const ast::Variable *function, std::vector<const ast::Expression *> args, std::vector<const ast::Statement *> &statements)
{
    const ast::PredefinedFunction *print = dynamic_cast<const ast::PredefinedFunction *>(scope.top()->lookupName("print"));
    if (print == nullptr) {
        internal_error("where's the print function");
    }
    const ast::Module *sys = import_module(Token(), "sys", false);
    if (sys == nullptr) {
        internal_error("need module sys");
    }
    const ast::PredefinedFunction *fexit = dynamic_cast<const ast::PredefinedFunction *>(sys->scope->lookupName("exit"));
    if (fexit == nullptr) {
        internal_error("where's the exit function");
    }
    const ast::Module *sqlite = import_module(Token(), "sqlite", false);
    if (sqlite == nullptr) {
        internal_error("need module sqlite");
    }
    const ast::Type *type_row = dynamic_cast<const ast::Type *>(sqlite->scope->lookupName("Row"));
    if (type_row == nullptr) {
        internal_error("need a Row type");
    }
    std::vector<const ast::VariableExpression *> out_bindings;
    for (auto name: statement->info->assignments) {
        const ast::Variable *var = dynamic_cast<const ast::Variable *>(scope.top()->lookupName(name.text));
        if (var == nullptr) {
            error(4306, name, "variable not found");
        }
        if (var->type != ast::TYPE_STRING) {
            error(4307, name, "INTO variable must be of type String (current implementation limitation)");
        }
        out_bindings.push_back(new ast::VariableExpression(var));
    }
    ast::Variable *result = scope.top()->makeTemporary(type_row);
    ast::Variable *found = scope.top()->makeTemporary(ast::TYPE_BOOLEAN);
    args.push_back(new ast::VariableExpression(result));
    statements.push_back(
        new ast::AssignmentStatement(
            statement->token.line,
            {new ast::VariableExpression(found)},
            new ast::FunctionCall(
                new ast::VariableExpression(function),
                args
            )
        )
    );
    std::vector<const ast::Statement *> then_statements;
    std::vector<const ast::Statement *> else_statements;
    uint32_t column = 0;
    for (auto b: out_bindings) {
        then_statements.push_back(
            new ast::AssignmentStatement(
                statement->token.line,
                {b},
                new ast::ArrayReferenceIndexExpression(
                    ast::TYPE_STRING,
                    new ast::VariableExpression(result),
                    new ast::ConstantNumberExpression(number_from_uint32(column)),
                    false
                )
            )
        );
        column++;
    }
    switch (scope.top()->sql_whenever[NotFound].type) {
        case SqlWheneverActionType::Continue:
            break;
        case SqlWheneverActionType::SqlPrint:
            else_statements.push_back(
                new ast::ExpressionStatement(
                    statement->token.line,
                    new ast::FunctionCall(
                        new ast::VariableExpression(print),
                        {new ast::ConstantStringExpression(utf8string(sql))}
                    )
                )
            );
            break;
        case SqlWheneverActionType::Stop:
            else_statements.push_back(
                new ast::ExpressionStatement(
                    statement->token.line,
                    new ast::FunctionCall(
                        new ast::VariableExpression(fexit),
                        {new ast::ConstantNumberExpression(number_from_uint32(1))}
                    )
                )
            );
            break;
        case SqlWheneverActionType::DoExitLoop:
            else_statements.push_back(
                new ast::ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "LOOP")
                )
            );
            break;
        case SqlWheneverActionType::DoExitFor:
            else_statements.push_back(
                new ast::ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOR")
                )
            );
            break;
        case SqlWheneverActionType::DoExitForeach:
            else_statements.push_back(
                new ast::ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOREACH")
                )
            );
            break;
        case SqlWheneverActionType::DoExitRepeat:
            else_statements.push_back(
                new ast::ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "REPEAT")
                )
            );
            break;
        case SqlWheneverActionType::DoExitWhile:
            else_statements.push_back(
                new ast::ExitStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "WHILE")
                )
            );
            break;
        case SqlWheneverActionType::DoNextLoop:
            else_statements.push_back(
                new ast::NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "LOOP")
                )
            );
            break;
        case SqlWheneverActionType::DoNextFor:
            else_statements.push_back(
                new ast::NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOR")
                )
            );
            break;
        case SqlWheneverActionType::DoNextForeach:
            else_statements.push_back(
                new ast::NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "FOREACH")
                )
            );
            break;
        case SqlWheneverActionType::DoNextRepeat:
            else_statements.push_back(
                new ast::NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "REPEAT")
                )
            );
            break;
        case SqlWheneverActionType::DoNextWhile:
            else_statements.push_back(
                new ast::NextStatement(
                    statement->token.line,
                    get_loop_id(statement->token, loops, "WHILE")
                )
            );
            break;
        case SqlWheneverActionType::DoRaiseException: {
            auto &info = scope.top()->sql_whenever[NotFound].info;
            Token token = info[0];
            const ast::Exception *exception = dynamic_cast<const ast::Exception *>(scope.top()->lookupName(info[0].text));
            if (exception != nullptr) {
                for (size_t i = 1; i < info.size(); i++) {
                    auto s = exception->subexceptions.find(info[i].text);
                    if (s == exception->subexceptions.end()) {
                        token = info[i];
                        exception = nullptr;
                        break;
                    }
                    exception = s->second;
                }
            }
            if (exception == nullptr) {
                error(4309, token, "exception not found");
            }
            else_statements.push_back(
                new ast::RaiseStatement(
                    statement->token.line,
                    exception,
                    new ast::RecordLiteralExpression(dynamic_cast<const ast::TypeRecord *>(scope.top()->lookupName("ExceptionInfo")), {
                        new ast::ConstantStringExpression(utf8string("No records found"))
                    })
                )
            );
            break;
        }
        default:
            internal_error("unexpected whenever condition");
    }
    statements.push_back(
        new ast::IfStatement(
            statement->token.line,
            {std::make_pair(new ast::VariableExpression(found), then_statements)},
            else_statements
        )
    );
}

const ast::Statement *Analyzer::analyze(const pt::ExecStatement *statement)
{
    const ast::Module *sqlite = import_module(Token(), "sqlite", false);
    if (sqlite == nullptr) {
        internal_error("need module sqlite");
    }
    const ast::PredefinedFunction *exec = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("exec"));
    if (exec == nullptr) {
        internal_error("where's the exec function");
    }
    const ast::PredefinedFunction *execOne = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("execOne"));
    if (execOne == nullptr) {
        internal_error("where's the execOne function");
    }
    const ast::Type *type_rows = dynamic_cast<const ast::Type *>(sqlite->scope->lookupName("Rows"));
    if (type_rows == nullptr) {
        internal_error("need a Rows type");
    }
    std::vector<const ast::Statement *> statements;
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
        const ast::PredefinedFunction *cursorClose = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("cursorClose"));
        statements.push_back(new ast::ExpressionStatement(
            statement->token.line,
            new ast::FunctionCall(
                new ast::VariableExpression(cursorClose),
                {
                    new ast::ConstantStringExpression(utf8string(close->name->text()))
                }
            )
        ));
    } else if (connect != nullptr) {
        const ast::PredefinedFunction *openfunc = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("open"));
        const SqlValueLiteral *target_literal = dynamic_cast<const SqlValueLiteral *>(connect->target.get());
        const SqlValueVariable *target_variable = dynamic_cast<const SqlValueVariable *>(connect->target.get());
        const ast::Expression *target;
        if (target_literal != nullptr) {
            target = new ast::ConstantStringExpression(utf8string(target_literal->value));
        } else if (target_variable != nullptr) {
            const ast::Variable *var = dynamic_cast<ast::Variable *>(scope.top()->lookupName(target_variable->variable.text));
            if (var == nullptr) {
                error(4302, target_variable->variable, "variable not found");
            }
            target = new ast::VariableExpression(var);
        } else {
            internal_error("unexpected target type");
        }
        const SqlIdentifierSymbol *name_symbol = dynamic_cast<const SqlIdentifierSymbol *>(connect->name.get());
        const SqlIdentifierVariable *name_variable = dynamic_cast<const SqlIdentifierVariable *>(connect->name.get());
        if (name_symbol != nullptr) {
            internal_error("todo");
        } else if (name_variable != nullptr) {
            const ast::Variable *name = dynamic_cast<const ast::Variable *>(scope.top()->lookupName(name_variable->variable.text));
            if (name == nullptr) {
                error(4304, name_variable->variable, "variable not found");
            }
            statements.push_back(new ast::AssignmentStatement(
                statement->token.line,
                {new ast::VariableExpression(name)},
                new ast::FunctionCall(
                    new ast::VariableExpression(openfunc),
                    {target}
                )
            ));
        } else {
            // TODO: handle default connection
            const ast::Variable *name = dynamic_cast<const ast::Variable *>(sqlite->scope->lookupName("db"));
            statements.push_back(new ast::AssignmentStatement(
                statement->token.line,
                {new ast::VariableExpression(name)},
                new ast::FunctionCall(
                    new ast::VariableExpression(openfunc),
                    {target}
                )
            ));
        }
    } else if (declare != nullptr) {
        const SqlDeclareQueryStatement *declare_query = dynamic_cast<const SqlDeclareQueryStatement *>(declare);
        if (declare_query != nullptr) {
            const ast::PredefinedFunction *cursorDeclare = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("cursorDeclare"));
            statements.push_back(new ast::AssignmentStatement(
                statement->token.line,
                {new ast::DummyExpression()},
                new ast::FunctionCall(
                    new ast::VariableExpression(cursorDeclare),
                    {
                        new ast::VariableExpression(dynamic_cast<const ast::Variable *>(sqlite->scope->lookupName("db"))),
                        new ast::ConstantStringExpression(utf8string(declare_query->cursor->text())),
                        new ast::ConstantStringExpression(utf8string(declare_query->query))
                    }
                )
            ));
        } else {
            internal_error("unexpected declare type");
        }
    } else if (disconnect != nullptr) {
        const ast::PredefinedFunction *closefunc = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("close"));
        const SqlIdentifierSymbol *name_symbol = dynamic_cast<const SqlIdentifierSymbol *>(disconnect->name.get());
        const SqlIdentifierVariable *name_variable = dynamic_cast<const SqlIdentifierVariable *>(disconnect->name.get());
        if (name_symbol != nullptr) {
            internal_error("todo");
        } else if (name_variable != nullptr) {
            const ast::Variable *name = dynamic_cast<const ast::Variable *>(scope.top()->lookupName(name_variable->variable.text));
            statements.push_back(new ast::ExpressionStatement(
                statement->token.line,
                new ast::FunctionCall(
                    new ast::VariableExpression(closefunc),
                    {new ast::VariableExpression(name)}
                )
            ));
        } else if (disconnect->default_) {
            internal_error("todo");
        } else if (disconnect->all) {
            internal_error("todo");
        } else {
            // TODO: handle default connection
            const ast::Variable *db = dynamic_cast<const ast::Variable *>(sqlite->scope->lookupName("db"));
            statements.push_back(new ast::ExpressionStatement(
                statement->token.line,
                new ast::FunctionCall(
                    new ast::VariableExpression(closefunc),
                    {new ast::VariableExpression(db)}
                )
            ));
        }
    } else if (execute != nullptr) {
        const ast::PredefinedFunction *execRaw = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("execRaw"));
        const SqlValueLiteral *statement_literal = dynamic_cast<const SqlValueLiteral *>(execute->statement.get());
        const SqlValueVariable *statement_variable = dynamic_cast<const SqlValueVariable *>(execute->statement.get());
        const ast::Expression *sqlstatement;
        if (statement_literal != nullptr) {
            sqlstatement = new ast::ConstantStringExpression(utf8string(statement_literal->value));
        } else if (statement_variable != nullptr) {
            const ast::Variable *var = dynamic_cast<ast::Variable *>(scope.top()->lookupName(statement_variable->variable.text));
            if (var == nullptr) {
                error(4303, statement_variable->variable, "variable not found");
            }
            sqlstatement = new ast::VariableExpression(var);
        } else {
            internal_error("unexpected execute type");
        }
        statements.push_back(new ast::AssignmentStatement(
            statement->token.line,
            {new ast::DummyExpression()},
            new ast::FunctionCall(
                new ast::VariableExpression(execRaw),
                {
                    new ast::VariableExpression(dynamic_cast<const ast::Variable *>(sqlite->scope->lookupName("db"))),
                    sqlstatement
                }
            )
        ));
    } else if (fetch != nullptr) {
        const ast::PredefinedFunction *cursorFetch = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("cursorFetch"));
        process_into_results(
            statement,
            "FETCH",
            cursorFetch,
            {
                new ast::ConstantStringExpression(utf8string(fetch->name->text())),
            },
            statements
        );
    } else if (open != nullptr) {
        const ast::PredefinedFunction *cursorOpen = dynamic_cast<const ast::PredefinedFunction *>(sqlite->scope->lookupName("cursorOpen"));
        statements.push_back(new ast::ExpressionStatement(
            statement->token.line,
            new ast::FunctionCall(
                new ast::VariableExpression(cursorOpen),
                {
                    new ast::ConstantStringExpression(utf8string(open->name->text()))
                }
            )
        ));
    } else if (query != nullptr) {
        std::vector<std::pair<utf8string, const ast::Expression *>> binding_vars;
        for (auto p: statement->info->parameters) {
            const ast::Variable *var = dynamic_cast<const ast::Variable *>(scope.top()->lookupName(p.text.substr(1)));
            if (var == nullptr) {
                error(4305, p, "variable not found");
            }
            // TODO: Call toString() on the parameter to convert it into a string.
            if (var->type != ast::TYPE_STRING) {
                error(4308, p, "query parameter "+p.text+" must be of type String (current implementation limitation)");
            }
            binding_vars.push_back(std::make_pair(utf8string(p.text), new ast::VariableExpression(var)));
        }
        process_into_results(
            statement,
            query->query,
            execOne,
            {
                new ast::VariableExpression(dynamic_cast<const ast::Variable *>(sqlite->scope->lookupName("db"))),
                new ast::ConstantStringExpression(utf8string(query->query)),
                new ast::DictionaryLiteralExpression(ast::TYPE_STRING, binding_vars, {})
            },
            statements
        );
    } else if (whenever != nullptr) {
        scope.top()->sql_whenever[whenever->condition] = whenever->action;
    } else {
        internal_error("unexpected statement type");
    }
    return new ast::CompoundStatement(statement->token.line, statements);
}

const ast::Statement *Analyzer::analyze(const pt::ExitStatement *statement)
{
    if (statement->type.type == FUNCTION) {
        if (functiontypes.empty()) {
            error(3107, statement->token, "EXIT FUNCTION not allowed outside function");
        } else if (functiontypes.top().second->returntype != ast::TYPE_NOTHING) {
            error(3108, statement->token, "function must return a value");
        }
        return new ast::ReturnStatement(statement->token.line, nullptr);
    }
    std::string type = statement->type.text;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new ast::ExitStatement(statement->token.line, j->second);
            }
        }
    }
    error(3078, statement->token, "no matching loop found in current scope");
}

const ast::Statement *Analyzer::analyze(const pt::ExpressionStatement *statement)
{
    const ast::Expression *expr = analyze(statement->expr.get());
    if (expr->type == ast::TYPE_NOTHING) {
        return new ast::ExpressionStatement(statement->token.line, analyze(statement->expr.get()));
    }
    if (dynamic_cast<const ast::ComparisonExpression *>(expr) != nullptr) {
        error(3060, statement->expr->token, "':=' expected");
    }
    if (dynamic_cast<const ast::FunctionCall *>(expr) != nullptr) {
        error(3059, statement->token, "return value unused");
    }
    error(3061, statement->token, "Unexpected");
}

const ast::Statement *Analyzer::analyze(const pt::ForStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    Token name = statement->var;
    if (scope.top()->lookupName(name.text) != nullptr) {
        error2(3118, name, "duplicate identifier", scope.top()->getDeclaration(name.text), "first declaration here");
    }
    ast::Variable *var = frame.top()->createVariable(name, name.text, ast::TYPE_NUMBER, false);
    scope.top()->addName(var->declaration, var->name, var, true);
    var->is_readonly = true;
    ast::Variable *bound = scope.top()->makeTemporary(ast::TYPE_NUMBER);
    const ast::Expression *start = analyze(statement->start.get());
    start = convert(ast::TYPE_NUMBER, start);
    if (start == nullptr) {
        error(3067, statement->start->token, "numeric expression expected");
    }
    const ast::Expression *end = analyze(statement->end.get());
    end = convert(ast::TYPE_NUMBER, end);
    if (end == nullptr) {
        error(3068, statement->end->token, "numeric expression expected");
    }
    const ast::Expression *step = nullptr;
    if (statement->step != nullptr) {
        step = analyze(statement->step.get());
        if (step->type != ast::TYPE_NUMBER) {
            error(3069, statement->step->token, "numeric expression expected");
        }
        if (not step->is_constant) {
            error(3070, statement->step->token, "numeric expression must be constant");
        }
        if (number_is_zero(step->eval_number(statement->step->token))) {
            error(3091, statement->step->token, "STEP value cannot be zero");
        }
    } else {
        step = new ast::ConstantNumberExpression(number_from_uint32(1));
    }
    // TODO: make loop_id a void*
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3213, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new ast::LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const ast::ReferenceExpression *> vars { new ast::VariableExpression(var) };
    std::vector<const ast::Statement *> init_statements {
        new ast::AssignmentStatement(statement->token.line, vars, start),
        new ast::AssignmentStatement(statement->token.line, { new ast::VariableExpression(bound) }, end),
    };
    std::vector<const ast::Statement *> statements {
        new ast::IfStatement(
            statement->token.line,
            std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> {
                std::make_pair(
                    new ast::NumericComparisonExpression(
                        new ast::VariableExpression(var),
                        new ast::VariableExpression(bound),
                        number_is_negative(step->eval_number(statement->step ? statement->step->token : Token())) ? ast::ComparisonExpression::Comparison::LT : ast::ComparisonExpression::Comparison::GT
                    ),
                    std::vector<const ast::Statement *> { new ast::ExitStatement(statement->token.line, loop_id) }
                )
            },
            std::vector<const ast::Statement *>()
        ),
    };
    std::vector<const ast::Statement *> body = analyze(statement->body);
    std::copy(body.begin(), body.end(), std::back_inserter(statements));
    std::vector<const ast::Statement *> tail_statements {
        new ast::AssignmentStatement(statement->token.line, vars, new ast::AdditionExpression(new ast::VariableExpression(var), step)),
    };
    scope.pop();
    loops.top().pop_back();
    var->is_readonly = false;
    return new ast::BaseLoopStatement(statement->token.line, loop_id, init_statements, statements, tail_statements, false);
}

const ast::Statement *Analyzer::analyze(const pt::ForeachStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    Token var_name = statement->var;
    if (scope.top()->lookupName(var_name.text) != nullptr) {
        error2(3169, var_name, "duplicate identifier", scope.top()->getDeclaration(var_name.text), "first declaration here");
    }
    const ast::Expression *array = analyze(statement->array.get());
    const ast::TypeArray *arrtype = dynamic_cast<const ast::TypeArray *>(array->type);
    const ast::Type *strtype = dynamic_cast<const ast::TypeString *>(array->type);
    const ast::Type *atype;
    const ast::Type *elementtype;
    std::string len_method;
    if (arrtype != nullptr) {
        atype = arrtype;
        elementtype = arrtype->elementtype;
        len_method = "size";
        if (elementtype == nullptr) {
            // Case of an empty literal array. There is no elementtype,
            // so two things: (1) We won't be able to create temporary
            // variables with an actual type, and (2) The body of the loop
            // will never be executed anyway.
            // Note that this skips over all semantic checking of the rest
            // of the loop declaration as well as the entire loop body.
            // Not sure it's a good idea to allow semantically invalid code
            // through.
            return new ast::NullStatement(statement->token.line);
        }
    } else if (strtype != nullptr) {
        atype = strtype;
        elementtype = ast::TYPE_STRING;
        len_method = "length";
    } else {
        error(3170, statement->array->token, "array or string expected");
    }
    ast::Variable *array_copy = scope.top()->makeTemporary(atype);

    ast::Variable *var = frame.top()->createVariable(var_name, var_name.text, elementtype, false);
    scope.top()->addName(var->declaration, var->name, var, true);
    var->is_readonly = true;

    Token index_name = statement->index;
    ast::Variable *index;
    if (index_name.type == IDENTIFIER) {
        if (scope.top()->lookupName(index_name.text) != nullptr) {
            error2(3171, index_name, "duplicate identifier", scope.top()->getDeclaration(index_name.text), "first declaration here");
        }
        index = frame.top()->createVariable(index_name, index_name.text, ast::TYPE_NUMBER, false);
        scope.top()->addName(index->declaration, index->name, index, true);
    } else {
        index = scope.top()->makeTemporary(ast::TYPE_NUMBER);
    }
    index->is_readonly = true;

    ast::Variable *bound = scope.top()->makeTemporary(ast::TYPE_NUMBER);
    // TODO: make loop_id a void*
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3214, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new ast::LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const ast::Statement *> init_statements {
        new ast::AssignmentStatement(statement->token.line, { new ast::VariableExpression(index) }, new ast::ConstantNumberExpression(number_from_uint32(0))),
        new ast::AssignmentStatement(statement->token.line, { new ast::VariableExpression(array_copy) }, array),
        new ast::AssignmentStatement(statement->token.line, { new ast::VariableExpression(bound) }, new ast::FunctionCall(new ast::VariableExpression(atype->methods.at(len_method)), { new ast::VariableExpression(array_copy) })),
    };
    std::vector<const ast::Statement *> statements {
        new ast::IfStatement(
            statement->token.line,
            std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> {
                std::make_pair(
                    new ast::NumericComparisonExpression(
                        new ast::VariableExpression(index),
                        new ast::VariableExpression(bound),
                        ast::ComparisonExpression::Comparison::GE
                    ),
                    std::vector<const ast::Statement *> { new ast::ExitStatement(statement->token.line, loop_id) }
                ),
            },
            std::vector<const ast::Statement *>()
        ),
        new ast::AssignmentStatement(statement->token.line, { new ast::VariableExpression(var) },
            arrtype ? static_cast<ast::Expression *>(new ast::ArrayValueIndexExpression(var->type, new ast::VariableExpression(array_copy), new ast::VariableExpression(index), false)) :
            strtype ? static_cast<ast::Expression *>(new ast::StringValueIndexExpression(new ast::VariableExpression(array_copy), new ast::VariableExpression(index), false, new ast::VariableExpression(index), false, this)) :
            nullptr
        ),
    };
    std::vector<const ast::Statement *> body = analyze(statement->body);
    std::copy(body.begin(), body.end(), std::back_inserter(statements));
    std::vector<const ast::Statement *> tail_statements {
        new ast::IncrementStatement(statement->token.line, new ast::VariableExpression(index), 1),
    };
    scope.pop();
    loops.top().pop_back();
    var->is_readonly = false;
    return new ast::BaseLoopStatement(statement->token.line, loop_id, init_statements, statements, tail_statements, false);
}

const ast::Statement *Analyzer::analyze(const pt::IfStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> condition_statements;
    for (auto &c: statement->condition_statements) {
        const ast::Expression *cond = nullptr;
        bool skip_statements = false;
        bool imported_checked = false;
        const pt::ValidPointerExpression *valid = dynamic_cast<const pt::ValidPointerExpression *>(c.first.get());
        const pt::ImportedModuleExpression *imported = dynamic_cast<const pt::ImportedModuleExpression *>(c.first.get());
        if (valid != nullptr) {
            for (auto &v: valid->tests) {
                if (not v->shorthand and scope.top()->lookupName(v->name.text) != nullptr) {
                    error2(3102, v->name, "duplicate identifier", scope.top()->getDeclaration(v->name.text), "first declaration here");
                }
                const ast::Expression *ptr = analyze(v->expr.get());
                const ast::TypePointer *ptrtype = dynamic_cast<const ast::TypePointer *>(ptr->type);
                if (ptrtype == nullptr) {
                    error(3101, v->expr->token, "pointer type expression expected");
                }
                const ast::TypeValidPointer *vtype = new ast::TypeValidPointer(Token(), ptrtype->reftype);
                ast::Variable *var;
                // TODO: Try to make this a local variable always (give the global scope a local space).
                if (functiontypes.empty()) {
                    var = new ast::GlobalVariable(v->name, v->name.text, vtype, true);
                } else {
                    // TODO: probably use frame.top()->get_depth() (add IF VALID to repl tests)
                    var = new ast::LocalVariable(v->name, v->name.text, vtype, frame.size()-1, true);
                }
                scope.top()->addName(v->name, v->name.text, var, true, v->shorthand);
                const ast::Expression *ve = new ast::ValidPointerExpression(var, ptr);
                if (cond == nullptr) {
                    cond = ve;
                } else {
                    cond = new ast::ConjunctionExpression(cond, ve);
                }
            }
        } else if (imported != nullptr) {
            const ast::Name *name = scope.top()->lookupName(imported->module.text);
            if (name == nullptr) {
                error(3279, imported->module, "unknown identifier");
            }
            const ast::Module *mod = dynamic_cast<const ast::Module *>(name);
            if (mod == nullptr) {
                error(3280, imported->module, "identifier is not a module");
            }
            if (mod != ast::MODULE_MISSING) {
                const ast::Module *runtime = import_module(Token(), "runtime", false);
                if (runtime == nullptr) {
                    internal_error("need module runtime");
                }
                cond = new ast::FunctionCall(
                    new ast::VariableExpression(dynamic_cast<const ast::PredefinedFunction *>(runtime->scope->lookupName("isModuleImported"))),
                    { new ast::ConstantStringExpression(utf8string(imported->module.text)) }
                );
                if (imported_checked_stack.empty()) {
                    imported_checked_stack.push({});
                } else {
                    imported_checked_stack.push(imported_checked_stack.top());
                }
                imported_checked_stack.top().insert(imported->module.text);
                imported_checked = true;
            } else {
                cond = new ast::ConstantBooleanExpression(false);
                skip_statements = true;
            }
        } else {
            cond = analyze(c.first.get());
            cond = convert(ast::TYPE_BOOLEAN, cond);
            if (cond == nullptr) {
                error(3048, c.first->token, "boolean value expected");
            }
        }
        scope.push(new ast::Scope(scope.top(), frame.top()));
        if (not skip_statements) {
            condition_statements.push_back(std::make_pair(cond, analyze(c.second)));
        }
        scope.pop();
        if (imported_checked) {
            imported_checked_stack.pop();
        }
    }
    std::vector<const ast::Statement *> else_statements = analyze(statement->else_statements);
    scope.pop();
    return new ast::IfStatement(statement->token.line, condition_statements, else_statements);
}

const ast::Statement *Analyzer::analyze(const pt::IncrementStatement *statement)
{
    const ast::Expression *e = analyze(statement->expr.get());
    if (e->type != ast::TYPE_NUMBER) {
        error(3187, statement->expr->token, "INC and DEC parameter must be Number");
    }
    const ast::ReferenceExpression *ref = dynamic_cast<const ast::ReferenceExpression *>(e);
    if (ref == nullptr) {
        error(3188, statement->expr->token, "INC and DEC call argument must be reference");
    }
    if (ref->is_readonly) {
        error(3189, statement->expr->token, "readonly parameter to INC or DEC");
    }
    return new ast::IncrementStatement(statement->token.line, ref, statement->delta);
}

const ast::Statement *Analyzer::analyze(const pt::LoopStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3215, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new ast::LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const ast::Statement *> statements = analyze(statement->body);
    scope.pop();
    loops.top().pop_back();
    return new ast::BaseLoopStatement(statement->token.line, loop_id, {}, statements, {}, true);
}

const ast::Statement *Analyzer::analyze(const pt::NextStatement *statement)
{
    std::string type = statement->type.text;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new ast::NextStatement(statement->token.line, j->second);
            }
        }
    }
    error(3084, statement->token, "no matching loop found in current scope");
}

const ast::Statement *Analyzer::analyze(const pt::RaiseStatement *statement)
{
    ast::Scope *s = scope.top();
    size_t i = 0;
    const ast::Name *modname = scope.top()->lookupName(statement->name[i].text);
    const ast::Module *mod = dynamic_cast<const ast::Module *>(modname);
    if (mod != nullptr) {
        s = mod->scope;
        i++;
    }
    const ast::Name *name = s->lookupName(statement->name[i].text);
    if (name == nullptr) {
        error(3089, statement->name[i], "exception not found: " + statement->name[i].text);
    }
    const ast::Exception *exception = dynamic_cast<const ast::Exception *>(name);
    if (exception == nullptr) {
        error2(3090, statement->name[i], "name not an exception", name->declaration, "declaration here");
    }
    i++;
    const ast::Exception *sn = exception;
    while (i < statement->name.size()) {
        auto t = sn->subexceptions.find(statement->name[i].text);
        if (t == sn->subexceptions.end()) {
            error(3239, statement->name[i], "exception subexception not found");
        }
        sn = t->second;
        i++;
    }
    const ast::Expression *info;
    if (statement->info != nullptr) {
        info = analyze(statement->info.get());
    } else {
        std::vector<const ast::Expression *> values;
        info = new ast::RecordLiteralExpression(dynamic_cast<const ast::TypeRecord *>(s->lookupName("ExceptionInfo")), values);
    }
    return new ast::RaiseStatement(statement->token.line, sn, info);
}

const ast::Statement *Analyzer::analyze(const pt::RepeatStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3216, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new ast::LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const ast::Statement *> statements = analyze(statement->body);
    const ast::Expression *cond = analyze(statement->cond.get());
    cond = convert(ast::TYPE_BOOLEAN, cond);
    if (cond == nullptr) {
        error(3086, statement->cond->token, "boolean value expected");
    }
    statements.push_back(
        new ast::IfStatement(
            statement->cond->token.line,
            std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> {
                std::make_pair(
                    cond,
                    std::vector<const ast::Statement *> { new ast::ExitStatement(statement->cond->token.line, loop_id) }
                )
            },
            std::vector<const ast::Statement *>()
        )
    );
    scope.pop();
    loops.top().pop_back();
    return new ast::BaseLoopStatement(statement->token.line, loop_id, {}, statements, {}, cond->is_constant && cond->eval_boolean(statement->cond->token) == false);
}

const ast::Statement *Analyzer::analyze(const pt::ReturnStatement *statement)
{
    const ast::Expression *expr = analyze(statement->expr.get());
    if (functiontypes.empty()) {
        error(3093, statement->token, "RETURN not allowed outside function");
    }
    const ast::Type *returntype = functiontypes.top().second->returntype;
    if (returntype == ast::TYPE_NOTHING) {
        error(3094, statement->token, "function does not return a value");
    }
    expr = convert(returntype, expr);
    if (expr == nullptr) {
        error(3095, statement->expr->token, "type mismatch in RETURN");
    }
    if (returntype != expr->type) {
        return new ast::ReturnStatement(statement->token.line, new ast::TypeConversionExpression(functiontypes.top().second->returntype, expr));
    }
    return new ast::ReturnStatement(statement->token.line, expr);
}

const ast::Statement *Analyzer::analyze(const pt::TestCaseStatement *statement)
{
    const ast::Expression *expr = analyze(statement->expr.get());
    const ast::Module *textio = import_module(Token(), "textio", false);
    if (textio == nullptr) {
        internal_error("need module textio");
    }
    const ast::Variable *textio_stderr = dynamic_cast<const ast::Variable *>(textio->scope->lookupName("stderr"));
    if (textio_stderr == nullptr) {
        internal_error("need textio.stderr");
    }
    const ast::PredefinedFunction *writeLine = dynamic_cast<const ast::PredefinedFunction *>(textio->scope->lookupName("writeLine"));
    if (writeLine == nullptr) {
        internal_error("need textio.writeLine");
    }
    const ast::Module *sys = import_module(Token(), "sys", false);
    if (sys == nullptr) {
        internal_error("need module sys");
    }
    const ast::PredefinedFunction *sys_exit = dynamic_cast<const ast::PredefinedFunction *>(sys->scope->lookupName("exit"));
    if (sys_exit == nullptr) {
        internal_error("need sys.exit");
    }
    std::vector<const ast::Statement *> fail_statements {
        new ast::ExpressionStatement(
            statement->token.line,
            new ast::FunctionCall(
                new ast::VariableExpression(writeLine),
                {
                    new ast::VariableExpression(textio_stderr),
                    new ast::ConstantStringExpression(utf8string("TESTCASE failed (" + statement->token.file() + " line " + std::to_string(statement->token.line) + "): " + statement->token.source->source_line(statement->token.line)))
                }
            )
        ),
        new ast::ExpressionStatement(
            statement->token.line,
            new ast::FunctionCall(
                new ast::VariableExpression(sys_exit),
                {
                    new ast::ConstantNumberExpression(number_from_uint32(1))
                }
            )
        )
    };
    if (statement->expected_exception.empty()) {
        expr = convert(ast::TYPE_BOOLEAN, expr);
        if (expr == nullptr) {
            error(3285, statement->expr->token, "boolean value expected");
        }
        return new ast::IfStatement(
            statement->token.line,
            std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> {
                std::make_pair(
                    new ast::LogicalNotExpression(expr),
                    fail_statements
                )
            },
            {}
        );
    } else {
        const ast::Exception *exception = resolve_exception(statement->expected_exception);
        std::vector<const ast::Statement *> statements;
        if (expr->type == ast::TYPE_NOTHING) {
            statements.push_back(
                new ast::ExpressionStatement(
                    statement->token.line,
                    expr
                )
            );
        } else {
            statements.push_back(
                new ast::AssignmentStatement(
                    statement->token.line,
                    {new ast::DummyExpression()},
                    expr
                )
            );
        }
        std::copy(fail_statements.begin(), fail_statements.end(), std::back_inserter(statements));
        return new ast::TryStatement(
            statement->token.line,
            statements,
            {ast::TryTrap({exception}, nullptr, new ast::ExceptionHandlerStatement(statement->token.line, {}))}
        );
    }
}

const ast::Exception *Analyzer::resolve_exception(const std::vector<Token> &names)
{
    ast::Scope *s = scope.top();
    size_t i = 0;
    const ast::Name *modname = scope.top()->lookupName(names[i].text);
    const ast::Module *mod = dynamic_cast<const ast::Module *>(modname);
    if (mod != nullptr) {
        s = mod->scope;
        i++;
    }
    const ast::Name *name = s->lookupName(names[i].text);
    if (name == nullptr) {
        error(3087, names[i], "exception not found: " + names[i].text);
    }
    const ast::Exception *exception = dynamic_cast<const ast::Exception *>(name);
    if (exception == nullptr) {
        error2(3088, names[i], "name not an exception", name->declaration, "declaration here");
    }
    i++;
    const ast::Exception *sn = exception;
    while (i < names.size()) {
        auto t = sn->subexceptions.find(names[i].text);
        if (t == sn->subexceptions.end()) {
            error(3240, names[i], "exception subexception not found");
        }
        sn = t->second;
        i++;
    }
    return sn;
}

std::vector<ast::TryTrap> Analyzer::analyze_catches(const std::vector<std::unique_ptr<pt::TryTrap>> &catches, const ast::Type *expression_match_type)
{
    std::vector<ast::TryTrap> r;
    for (auto &x: catches) {
        const ast::Exception *sn = resolve_exception(x->exceptions.at(0));
        std::vector<const ast::Exception *> exceptions;
        exceptions.push_back(sn);
        scope.push(new ast::Scope(scope.top(), frame.top()));
        ast::Variable *var = nullptr;
        if (x->name.type != NONE) {
            const ast::TypeRecord *vtype = dynamic_cast<const ast::TypeRecord *>(scope.top()->lookupName("ExceptionType"));
            if (vtype == nullptr) {
                internal_error("could not find ExceptionType");
            }
            if (scope.top()->lookupName(x->name.text) != nullptr) {
                error2(3276, x->name, "duplicate identifier", scope.top()->getDeclaration(x->name.text), "first declaration here");
            }
            // TODO: Try to make this a local variable always (give the global scope a local space).
            if (functiontypes.empty()) {
                var = new ast::GlobalVariable(x->name, x->name.text, vtype, true);
            } else {
                var = new ast::LocalVariable(x->name, x->name.text, vtype, frame.size()-1, true);
            }
            scope.top()->addName(x->name, x->name.text, var, true);
        }
        const pt::TryHandlerStatement *ths = dynamic_cast<const pt::TryHandlerStatement *>(x->handler.get());
        const pt::Expression *e = dynamic_cast<const pt::Expression *>(x->handler.get());
        if (ths != nullptr) {
            std::vector<const ast::Statement *> statements = analyze(ths->body);
            r.push_back(ast::TryTrap(exceptions, var, new ast::ExceptionHandlerStatement(0 /*TODO*/, statements)));
        } else if (e != nullptr) {
            const ast::Expression *g = analyze(e);
            if (g->type != expression_match_type) {
                error(3282, e->token, "type mismatch");
            }
            r.push_back(ast::TryTrap(exceptions, var, g));
        } else {
            internal_error("unexpected catch type");
        }
        scope.pop();
    }
    return r;
}

const ast::Statement *Analyzer::analyze(const pt::TryStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    std::vector<const ast::Statement *> statements = analyze(statement->body);
    scope.pop();
    auto catches = analyze_catches(statement->catches, nullptr);
    return new ast::TryStatement(statement->token.line, statements, catches);
}

const ast::Statement *Analyzer::analyze(const pt::TryHandlerStatement *statement)
{
    scope.push(new ast::Scope(scope.top(), frame.top()));
    std::vector<const ast::Statement *> statements = analyze(statement->body);
    scope.pop();
    return new ast::ExceptionHandlerStatement(statement->token.line, statements);
}

const ast::Statement *Analyzer::analyze(const pt::UnusedStatement *statement)
{
    return new ast::NullStatement(statement->token.line);
}

const ast::Statement *Analyzer::analyze(const pt::WhileStatement *statement)
{
    const ast::Expression *cond = nullptr;
    const pt::ValidPointerExpression *valid = dynamic_cast<const pt::ValidPointerExpression *>(statement->cond.get());
    if (valid != nullptr) {
        for (auto &v: valid->tests) {
            if (not v->shorthand and scope.top()->lookupName(v->name.text) != nullptr) {
                error2(3234, v->name, "duplicate identifier", scope.top()->getDeclaration(v->name.text), "first declaration here");
            }
            const ast::Expression *ptr = analyze(v->expr.get());
            const ast::TypePointer *ptrtype = dynamic_cast<const ast::TypePointer *>(ptr->type);
            if (ptrtype == nullptr) {
                error(3233, v->expr->token, "pointer type expression expected");
            }
            const ast::TypeValidPointer *vtype = new ast::TypeValidPointer(Token(), ptrtype->reftype);
            ast::Variable *var;
            // TODO: Try to make this a local variable always (give the global scope a local space).
            if (functiontypes.empty()) {
                var = new ast::GlobalVariable(v->name, v->name.text, vtype, true);
            } else {
                // TODO: probably use frame.top()->get_depth() (add IF VALID to repl tests)
                var = new ast::LocalVariable(v->name, v->name.text, vtype, frame.size()-1, true);
            }
            scope.top()->addName(v->name, v->name.text, var, true, v->shorthand);
            const ast::Expression *ve = new ast::ValidPointerExpression(var, ptr);
            if (cond == nullptr) {
                cond = ve;
            } else {
                cond = new ast::ConjunctionExpression(cond, ve);
            }
        }
    } else {
        cond = analyze(statement->cond.get());
        cond = convert(ast::TYPE_BOOLEAN, cond);
        if (cond == nullptr) {
            error(3049, statement->cond->token, "boolean value expected");
        }
    }
    scope.push(new ast::Scope(scope.top(), frame.top()));
    unsigned int loop_id = static_cast<unsigned int>(reinterpret_cast<intptr_t>(statement));
    if (statement->label.type == IDENTIFIER) {
        Token label = scope.top()->getDeclaration(statement->label.text);
        if (label.type != NONE) {
            error2(3217, statement->label, "loop label already defined", label, "declaration here");
        }
        scope.top()->addName(statement->label, statement->label.text, new ast::LoopLabel(statement->label));
    }
    loops.top().push_back(std::make_pair(statement->label.text, loop_id));
    std::vector<const ast::Statement *> statements {
        new ast::IfStatement(
            statement->token.line,
            std::vector<std::pair<const ast::Expression *, std::vector<const ast::Statement *>>> {
                std::make_pair(
                    new ast::LogicalNotExpression(cond),
                    std::vector<const ast::Statement *> { new ast::ExitStatement(statement->token.line, loop_id) }
                )
            },
            std::vector<const ast::Statement *>()
        )
    };
    std::vector<const ast::Statement *> body = analyze(statement->body);
    std::copy(body.begin(), body.end(), std::back_inserter(statements));
    scope.pop();
    loops.top().pop_back();
    return new ast::BaseLoopStatement(statement->token.line, loop_id, {}, statements, {}, cond->is_constant && cond->eval_boolean(statement->cond->token) == true);
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
    explicit ExportedTypeChecker(const std::set<const ast::Name *> &exported): exported(exported), seen() {}
    ExportedTypeChecker &operator=(const ExportedTypeChecker &) = delete;
    void check(const ast::Type *type) {
        if (seen.find(type) != seen.end()) {
            return;
        }
        seen.insert(type);
        if (type == ast::TYPE_NOTHING
         || type == ast::TYPE_BOOLEAN
         || type == ast::TYPE_NUMBER
         || type == ast::TYPE_STRING
         || type == ast::TYPE_BYTES) {
            return;
        }
        const ast::TypeArray *ta = dynamic_cast<const ast::TypeArray *>(type);
        const ast::TypeDictionary *td = dynamic_cast<const ast::TypeDictionary *>(type);
        const ast::TypePointer *tp = dynamic_cast<const ast::TypePointer *>(type);
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
        const ast::TypeRecord *tr = dynamic_cast<const ast::TypeRecord *>(type);
        if (tr != nullptr) {
            for (auto &f: tr->fields) {
                check(f.type);
            }
        }
    }
private:
    const std::set<const ast::Name *> &exported;
    std::set<const ast::Name *> seen;
};

const ast::Program *Analyzer::analyze()
{
    ast::Program *r = new ast::Program(program->source_path, program->source_hash, module_name);
    global_scope = r->scope;
    frame.push(r->frame);
    scope.push(global_scope);
    // Create a new scope for the global things in the main program,
    // so that modules can use the real global scope as their parent
    // scope to avoid accidentally linking them up together (issue #30).
    if (external_globals != nullptr) {
        frame.push(new ast::ExternalGlobalFrame(frame.top(), *external_globals));
        scope.push(new ast::ExternalGlobalScope(scope.top(), frame.top(), *external_globals));
        r->frame = frame.top();
        r->scope = scope.top();
    } else {
        scope.push(new ast::Scope(scope.top(), frame.top()));
        r->scope = scope.top();
    }

    //init_builtin_constants(global_scope);

    loops.push(std::list<std::pair<std::string, unsigned int>>());
    r->statements = analyze(program->body);
    loops.pop();
    r->scope->checkForward();

    // Check for unimplemented interface methods.
    for (size_t i = 0; i < r->frame->getCount(); i++) {
        ast::Frame::Slot s = r->frame->getSlot(i);
        ast::TypeClass *c = dynamic_cast<ast::TypeClass *>(s.ref);
        if (c != nullptr) {
            for (auto iface: c->interfaces) {
                for (auto m: iface->methods) {
                    if (c->methods.find(m.first.text) == c->methods.end()) {
                        error2(3252, c->declaration, "method missing", m.first, "declared here");
                    }
                }
            }
        }
    }

    std::set<const ast::Name *> exported;
    for (auto nt: exports) {
        const ast::Name *name = scope.top()->lookupName(nt.first);
        if (name == nullptr) {
            internal_error("export name not found: " + nt.first);
        }
        if (r->exports.find(nt.first) != r->exports.end()) {
            internal_error("export name already exported");
        }
        if (dynamic_cast<const ast::Type *>(name) == nullptr
         && dynamic_cast<const ast::Exception *>(name) == nullptr
         && dynamic_cast<const ast::Interface *>(name) == nullptr
         && dynamic_cast<const ast::GlobalVariable *>(name) == nullptr
         && dynamic_cast<const ast::Constant *>(name) == nullptr
         && dynamic_cast<const ast::BaseFunction *>(name) == nullptr) {
            internal_error("Attempt to export something that can't be exported: " + nt.first);
        }
        r->exports[nt.first] = name;
        exported.insert(name);
    }
    ExportedTypeChecker etc(exported);
    for (auto nt: exports) {
        const ast::Name *name = scope.top()->lookupName(nt.first);
        if (dynamic_cast<const ast::Type *>(name) != nullptr) {
            // pass
        } else if (dynamic_cast<const ast::Exception *>(name) != nullptr) {
            // pass
        } else if (dynamic_cast<const ast::GlobalVariable *>(name) != nullptr) {
            etc.check(dynamic_cast<const ast::GlobalVariable *>(name)->type);
        } else if (dynamic_cast<const ast::Constant *>(name) != nullptr) {
            etc.check(dynamic_cast<const ast::Constant *>(name)->type);
        } else if (dynamic_cast<const ast::Function *>(name) != nullptr) {
            const ast::Function *f = dynamic_cast<const ast::Function *>(name);
            for (auto p: f->params) {
                etc.check(p->type);
            }
            etc.check(dynamic_cast<const ast::TypeFunction *>(f->type)->returntype);
        } else if (dynamic_cast<const ast::PredefinedFunction *>(name) != nullptr) {
            // pass
        }
    }
    scope.pop();
    return r;
}

class VariableChecker: public pt::IParseTreeVisitor {
public:
    VariableChecker(): scopes(), out_parameters() {
        scopes.push_back(ScopeInfo());
    }
    VariableChecker(const VariableChecker &) = delete;
    VariableChecker &operator=(const VariableChecker &) = delete;
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
        const bool uninitialised = scopes.back().assigned.find(node->name) == scopes.back().assigned.end();
        for (auto s = scopes.rbegin(); s != scopes.rend(); ++s) {
            auto i = s->variables.find(node->name);
            if (i != s->variables.end()) {
                if (uninitialised) {
                    error2(3190, node->token, "Uninitialised variable: " + node->name, i->second.token, "Variable declared here");
                }
                i->second.mark_used(node->token);
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
            if (x->mode.type == OUT) {
                const pt::IdentifierExpression *expr = dynamic_cast<const pt::IdentifierExpression *>(x->expr.get());
                if (expr != nullptr) {
                    mark_assigned(expr->name, expr->token);
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
    virtual void visit(const pt::TypeTestExpression *node) { node->left->accept(this); }
    virtual void visit(const pt::MembershipExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConjunctionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::DisjunctionExpression *node) { node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::ConditionalExpression *node) { node->cond->accept(this); node->left->accept(this); node->right->accept(this); }
    virtual void visit(const pt::TryExpression *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::NewClassExpression *node) { node->expr->accept(this); }
    virtual void visit(const pt::ValidPointerExpression *node) { for (auto &x: node->tests) x->expr->accept(this); }
    virtual void visit(const pt::ImportedModuleExpression *) {}
    virtual void visit(const pt::RangeSubscriptExpression *node) { node->base->accept(this); node->range->first.get()->accept(this); node->range->last.get()->accept(this); }

    virtual void visit(const pt::ImportDeclaration *) {}
    virtual void visit(const pt::TypeDeclaration *) {}
    virtual void visit(const pt::ConstantDeclaration *) {}
    virtual void visit(const pt::NativeConstantDeclaration *) {}
    virtual void visit(const pt::ExtensionConstantDeclaration *) {}
    virtual void visit(const pt::VariableDeclaration *node) {
        if (node->value != nullptr) {
            node->value->accept(this);
            for (auto name: node->names) {
                add_variable(name, true);
            }
        } else {
            for (auto name: node->names) {
                add_variable(name, false);
            }
        }
    }
    virtual void visit(const pt::NativeVariableDeclaration *) {}
    virtual void visit(const pt::LetDeclaration *node) {
        node->value->accept(this);
        add_variable(node->name, true);
    }
    virtual void visit(const pt::FunctionDeclaration *node) {
        VariableChecker vc;
        for (auto &a: node->args) {
            if (a->mode == pt::FunctionParameterGroup::Mode::OUT) {
                for (auto name: a->names) {
                    vc.add_variable(name, false);
                    vc.out_parameters.push_back(name.text);
                }
            }
        }
        for (auto &s: node->body) {
            s->accept(&vc);
        }
        vc.check_out_parameters(node->end_function);
        vc.check_unused();
    }
    virtual void visit(const pt::NativeFunctionDeclaration *) {}
    virtual void visit(const pt::ExtensionFunctionDeclaration *) {}
    virtual void visit(const pt::ExceptionDeclaration *) {}
    virtual void visit(const pt::InterfaceDeclaration *) {}
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
                mark_assigned(expr->name, expr->token);
                for (auto s = scopes.rbegin(); s != scopes.rend(); ++s) {
                    auto i = s->variables.find(expr->name);
                    if (i != s->variables.end()) {
                        i->second.mark_used(expr->token);
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
        std::set<std::string> assigned;
        bool found_others = false;
        bool first = true;
        for (auto &c: node->clauses) {
            if (not c.first.empty()) {
                for (auto &w: c.first) {
                    auto *cwc = dynamic_cast<const pt::CaseStatement::ComparisonWhenCondition *>(w.get());
                    auto *rwc = dynamic_cast<const pt::CaseStatement::RangeWhenCondition *>(w.get());
                    auto *twc = dynamic_cast<const pt::CaseStatement::TypeTestWhenCondition *>(w.get());
                    if (cwc != nullptr) {
                        cwc->expr->accept(this);
                    } else if (rwc != nullptr) {
                        rwc->low_expr->accept(this);
                        rwc->high_expr->accept(this);
                    } else if (twc != nullptr) {
                         // no action
                    } else {
                        internal_error("unknown case when condition");
                    }
                }
            } else {
                found_others = true;
            }
            enter(true);
            for (auto &s: c.second) {
                s->accept(this);
            }
            if (first) {
                assigned = scopes.back().assigned;
                first = false;
            } else {
                intersect(assigned, scopes.back().assigned);
            }
            leave();
        }
        if (not found_others) {
            // If there is no WHEN OTHERS clause, then nothing at all could happen.
            assigned.clear();
        }
        for (auto a: assigned) {
            mark_assigned(a, Token());
        }
    }
    virtual void visit(const pt::CheckStatement *node) {
        node->cond->accept(this);
        enter(true);
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::ExecStatement *node) {
        for (auto &var: node->info->assignments) {
            mark_assigned(var.text, var);
        }
        for (auto p: node->info->parameters) {
            for (auto s = scopes.rbegin(); s != scopes.rend(); ++s) {
                auto i = s->variables.find(p.text.substr(1));
                if (i != s->variables.end()) {
                    i->second.mark_used(p);
                    break;
                }
            }
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
        enter(true);
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::ForeachStatement *node) {
        node->array->accept(this);
        enter(true);
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::IfStatement *node) {
        std::set<std::string> assigned;
        bool first = true;
        for (auto &x: node->condition_statements) {
            x.first->accept(this);
            enter(true);
            for (auto &s: x.second) {
                s->accept(this);
            }
            if (first) {
                assigned = scopes.back().assigned;
                first = false;
            } else {
                intersect(assigned, scopes.back().assigned);
            }
            leave();
        }
        enter(true);
        for (auto &s: node->else_statements) {
            s->accept(this);
        }
        intersect(assigned, scopes.back().assigned);
        leave();
        for (auto a: assigned) {
            mark_assigned(a, Token());
        }
    }
    virtual void visit(const pt::IncrementStatement *node) {
        node->expr->accept(this);
    }
    virtual void visit(const pt::LoopStatement *node) {
        enter(false);
        for (auto &s: node->body) {
            s->accept(this);
        }
        leave();
    }
    virtual void visit(const pt::NextStatement *) {}
    virtual void visit(const pt::RaiseStatement *) {}
    virtual void visit(const pt::RepeatStatement *node) {
        enter(false);
        for (auto &s: node->body) {
            s->accept(this);
        }
        node->cond->accept(this);
        leave();
    }
    virtual void visit(const pt::ReturnStatement *node) {
        node->expr->accept(this);
        check_out_parameters(node->token);
    }
    virtual void visit(const pt::TestCaseStatement *node) { node->expr->accept(this); }
    virtual void visit(const pt::TryStatement *node) {
        enter(false);
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
    virtual void visit(const pt::UnusedStatement *node) {
        for (auto &v: node->vars) {
            bool found = false;
            for (auto s = scopes.rbegin(); s != scopes.rend(); ++s) {
                auto i = s->variables.find(v.text);
                if (i != s->variables.end()) {
                    i->second.mark_declared_unused(v);
                    found = true;
                    break;
                }
            }
            if (not found) {
                error(3258, v, "variable not found");
            }
        }
    }
    virtual void visit(const pt::WhileStatement *node) {
        node->cond->accept(this);
        enter(true);
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
    struct VariableInfo {
        VariableInfo(): token(), usage(Usage::UNUSED) {}
        explicit VariableInfo(const Token &token): token(token), usage(Usage::UNUSED) {}
        Token token;
        enum class Usage {
            UNUSED,
            DECLARED_UNUSED,
            USED
        };
        Usage usage;
        void mark_used(const Token &t) {
            if (usage == Usage::DECLARED_UNUSED) {
                error(3260, t, "variable already declared unused");
            }
            usage = Usage::USED;
        }
        void mark_declared_unused(const Token &t) {
            if (usage == VariableInfo::Usage::USED) {
                error(3259, t, "variable already used");
            }
            usage = Usage::DECLARED_UNUSED;
        }
    };
    struct ScopeInfo {
        ScopeInfo(): conditional(false), variables(), assigned() {}
        ScopeInfo(const std::set<std::string> &outer_assigned, bool conditional): conditional(conditional), variables(), assigned(outer_assigned) {}
        bool conditional;
        std::map<std::string, VariableInfo> variables;
        std::set<std::string> assigned;
    };
    std::list<ScopeInfo> scopes;
    std::vector<std::string> out_parameters;

    void enter(bool conditional) {
        scopes.push_back(ScopeInfo(scopes.back().assigned, conditional));
    }
    void leave() {
        check_unused();
        scopes.pop_back();
    }
    void add_variable(const Token &name, bool initialised)
    {
        scopes.back().variables[name.text] = VariableInfo(name);
        if (initialised) {
            scopes.back().assigned.insert(name.text);
        }
    }
    void check_unused() {
        for (auto &name: scopes.back().variables) {
            if (name.second.usage == VariableInfo::Usage::UNUSED) {
                error(3205, name.second.token, "Unused variable");
            }
        }
    }
    void mark_assigned(const std::string &name, const Token &t) {
        bool unconditional = true;
        for (auto s = scopes.rbegin(); s != scopes.rend(); ++s) {
            if (unconditional) {
                s->assigned.insert(name);
            }
            auto i = s->variables.find(name);
            if (i != s->variables.end()) {
                i->second.mark_used(t);
                break;
            }
            if (s->conditional) {
                unconditional = false;
            }
        }
    }

    void check_out_parameters(const Token &token)
    {
        for (auto p: out_parameters) {
            if (scopes.back().assigned.find(p) != scopes.back().assigned.end()) {
                break;
            }
            for (auto s = scopes.rbegin(); s != scopes.rend(); ++s) {
                auto i = s->variables.find(p);
                if (i != s->variables.end()) {
                    if (s->assigned.find(i->second.token.text) != s->assigned.end()) {
                        break;
                    } else {
                        error2(3191, token, "Uninitialised OUT parameter: " + p, i->second.token, "Variable declared here");
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
};

const ast::Program *analyze(ICompilerSupport *support, const pt::Program *program, std::map<std::string, ast::ExternalGlobalInfo> *external_globals)
{
    const ast::Program *r = Analyzer(support, program, external_globals).analyze();

    // Check variable use (uninitialised, unused).
    VariableChecker vc;
    program->accept(&vc);

    // Skip this if we're in the repl.
    if (external_globals == nullptr) {
        // Find unused imports.
        for (size_t i = 0; i < r->frame->getCount(); i++) {
            ast::Frame::Slot s = r->frame->getSlot(i);
            if (dynamic_cast<ast::Module *>(s.ref) != nullptr && not s.referenced) {
                error(3192, s.token, "Unused import");
            }
        }
    }

    return r;
}
