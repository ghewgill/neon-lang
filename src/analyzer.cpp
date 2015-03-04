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
    static Scope *global_scope; // TODO: static is a hack, used by StringReference constructor
    std::stack<Scope *> scope;

    std::stack<const TypeFunction *> functiontypes;
    std::stack<std::list<std::pair<TokenType, unsigned int>>> loops;

#if 0
    const Type *parseArrayType(Scope *scope);
    const Type *parseDictionaryType(Scope *scope);
    const Type *parseRecordType(Scope *scope);
    const Type *parseEnumType(Scope *scope);
    const Type *parsePointerType(Scope *scope);
    const Type *parseType(Scope *scope);
    const Statement *parseTypeDefinition(Scope *scope);
    const Statement *parseConstantDefinition(Scope *scope);
    const FunctionCall *parseFunctionCall(const Expression *func, const Expression *self, Scope *scope);
    const ArrayLiteralExpression *parseArrayLiteral(Scope *scope);
    const DictionaryLiteralExpression *parseDictionaryLiteral(Scope *scope);
    const Name *parseQualifiedName(Scope *scope, int &enclosing);
    const Expression *parseInterpolatedStringExpression(Scope *scope);
    const Expression *parseAtom(Scope *scope);
    const Expression *parseExponentiation(Scope *scope);
    const Expression *parseMultiplication(Scope *scope);
    const Expression *parseAddition(Scope *scope);
    const Expression *parseComparison(Scope *scope);
    const Expression *parseMembership(Scope *scope);
    const Expression *parseConjunction(Scope *scope);
    const Expression *parseDisjunction(Scope *scope);
    const Expression *parseConditional(Scope *scope);
    const Expression *parseExpression(Scope *scope, bool allow_nothing = false);
    const VariableInfo parseVariableDeclaration(Scope *scope);
    void parseFunctionHeader(Scope *scope, Type *&type, std::string &name, const Type *&returntype, Scope *&newscope, std::vector<FunctionParameter *> &args);
    const Statement *parseFunctionDefinition(Scope *scope);
    const Statement *parseExternalDefinition(Scope *scope);
    const Statement *parseDeclaration(Scope *scope);
    const Statement *parseIfStatement(Scope *scope, int line);
    const Statement *parseReturnStatement(Scope *scope, int line);
    const Statement *parseVarStatement(Scope *scope, int line);
    const Statement *parseWhileStatement(Scope *scope, int line);
    const Statement *parseCaseStatement(Scope *scope, int line);
    const Statement *parseForStatement(Scope *scope, int line);
    const Statement *parseLoopStatement(Scope *scope, int line);
    const Statement *parseRepeatStatement(Scope *scope, int line);
    const Statement *parseExitStatement(Scope *scope, int line);
    const Statement *parseNextStatement(Scope *scope, int line);
    const Statement *parseTryStatement(Scope *scope, int line);
    const Statement *parseRaiseStatement(Scope *scope, int line);
    const Statement *parseImport(Scope *scope);
    const Statement *parseStatement(Scope *scope);
#endif
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
    const Statement *analyze(const pt::ImportDeclaration *declaration);
    const Statement *analyze(const pt::TypeDeclaration *declaration);
    const Statement *analyze(const pt::ConstantDeclaration *declaration);
    const Statement *analyze(const pt::VariableDeclaration *declaration);
    const Statement *analyze_decl(const pt::FunctionDeclaration *declaration);
    const Statement *analyze_body(const pt::FunctionDeclaration *declaration);
    const Statement *analyze(const pt::ExternalFunctionDeclaration *declaration);
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
    virtual void visit(const pt::ImportDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) { internal_error("pt::Declaration"); }
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
    virtual void visit(const pt::ImportDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::TypeDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::ConstantDeclaration *) { internal_error("pt::Declaration"); }
    virtual void visit(const pt::VariableDeclaration *) { internal_error("pt::Declaration"); }
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
    virtual void visit(const pt::ImportDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::TypeDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::ConstantDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::VariableDeclaration *p) { v.push_back(a->analyze(p)); }
    virtual void visit(const pt::FunctionDeclaration *p) { v.push_back(a->analyze_decl(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *p) { v.push_back(a->analyze(p)); }
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
    virtual void visit(const pt::ImportDeclaration *) {}
    virtual void visit(const pt::TypeDeclaration *) {}
    virtual void visit(const pt::ConstantDeclaration *) {}
    virtual void visit(const pt::VariableDeclaration *) {}
    virtual void visit(const pt::FunctionDeclaration *p) { v.push_back(a->analyze_body(p)); }
    virtual void visit(const pt::ExternalFunctionDeclaration *) {}
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

Scope *Analyzer::global_scope;

Analyzer::Analyzer(const pt::Program *program)
  : program(program),
    scope(),
    functiontypes(),
    loops()
{
}

TypeEnum::TypeEnum(const std::map<std::string, int> &names)
  : TypeNumber(),
    names(names)
{
    {
        Scope *newscope = new Scope(Analyzer::global_scope);
        std::vector<FunctionParameter *> params;
        FunctionParameter *fp = new FunctionParameter("self", this, ParameterType::INOUT, newscope);
        params.push_back(fp);
        Function *f = new Function("enum.to_string", TYPE_STRING, newscope, params);
        f->scope->addName("self", fp, true);
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

StringReferenceIndexExpression::StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *index)
  : ReferenceExpression(ref->type, ref->is_readonly),
    ref(ref),
    index(index),
    load(nullptr),
    store(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("substring"))), args);
    }
    {
        std::vector<const Expression *> args;
        args.push_back(ref);
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        store = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("splice"))), args);
    }
}

StringValueIndexExpression::StringValueIndexExpression(const Expression *str, const Expression *index)
  : Expression(str->type, str->is_readonly),
    str(str),
    index(index),
    load(nullptr)
{
    {
        std::vector<const Expression *> args;
        args.push_back(str);
        args.push_back(index);
        args.push_back(new ConstantNumberExpression(number_from_uint32(1)));
        load = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(Analyzer::global_scope->lookupName("substring"))), args);
    }
}

#if 0

const Type *Analyzer::parseArrayType(Scope *scope)
{
    if (tokens[i].type != ARRAY) {
        internal_error("Array expected");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(2002, tokens[i], "'<' expected");
    }
    i++;
    const Type *elementtype = parseType(scope);
    if (tokens[i].type != GREATER) {
        error(2003, tokens[i], "'>' expected");
    }
    i++;
    return new TypeArray(elementtype);
}

const Type *Analyzer::parseDictionaryType(Scope *scope)
{
    if (tokens[i].type != DICTIONARY) {
        internal_error("Dictionary expected");
    }
    i++;
    if (tokens[i].type != LESS) {
        error(2005, tokens[i], "'<' expected");
    }
    i++;
    const Type *elementtype = parseType(scope);
    if (tokens[i].type != GREATER) {
        error(2006, tokens[i], "'>' expected");
    }
    i++;
    return new TypeDictionary(elementtype);
}

const Type *Analyzer::parseRecordType(Scope *scope)
{
    if (tokens[i].type != RECORD) {
        internal_error("RECORD expected");
    }
    i++;
    std::vector<std::pair<std::string, const Type *>> fields;
    std::set<std::string> field_names;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2008, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (field_names.find(name) != field_names.end()) {
            error(2009, tokens[i], "duplicate field: " + name);
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(2010, tokens[i], "colon expected");
        }
        ++i;
        const Type *t = parseType(scope);
        fields.push_back(std::make_pair(name, t));
        field_names.insert(name);
    }
    i++;
    if (tokens[i].type != RECORD) {
        error(2100, tokens[i], "'RECORD' expected");
    }
    i++;
    return new TypeRecord(fields);
}

const Type *Analyzer::parseEnumType(Scope *)
{
    if (tokens[i].type != ENUM) {
        internal_error("ENUM expected");
    }
    i++;
    std::map<std::string, int> names;
    int index = 0;
    while (tokens[i].type != END) {
        if (tokens[i].type != IDENTIFIER) {
            error(2012, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (names.find(name) != names.end()) {
            error(2013, tokens[i], "duplicate enum: " + name);
        }
        i++;
        names[name] = index;
        index++;
    }
    i++;
    if (tokens[i].type != ENUM) {
        error(2101, tokens[i], "'ENUM' expected");
    }
    i++;
    return new TypeEnum(names);
}

const Type *Analyzer::parsePointerType(Scope *scope)
{
    if (tokens[i].type != POINTER) {
        internal_error("POINTER expected");
    }
    i++;
    if (tokens[i].type == TO) {
        i++;
        if (tokens[i].type == IDENTIFIER && scope->lookupName(tokens[i].text) == nullptr) {
            const std::string name = tokens[i].text;
            i++;
            TypePointer *ptrtype = new TypePointer(new TypeForwardRecord());
            scope->addForward(name, ptrtype);
            return ptrtype;
        } else {
            const Type *reftype = parseType(scope);
            const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(reftype);
            if (rectype == nullptr) {
                error(2171, tokens[i], "record type expected");
            }
            return new TypePointer(rectype);
        }
    } else {
        return new TypePointer(nullptr);
    }
}

const Type *Analyzer::parseType(Scope *scope)
{
    if (tokens[i].type == ARRAY) {
        return parseArrayType(scope);
    }
    if (tokens[i].type == DICTIONARY) {
        return parseDictionaryType(scope);
    }
    if (tokens[i].type == RECORD) {
        return parseRecordType(scope);
    }
    if (tokens[i].type == ENUM) {
        return parseEnumType(scope);
    }
    if (tokens[i].type == POINTER) {
        return parsePointerType(scope);
    }
    if (tokens[i].type != IDENTIFIER) {
        error(2014, tokens[i], "identifier expected");
    }
    const Name *name = scope->lookupName(tokens[i].text);
    if (name == nullptr) {
        error(2015, tokens[i], "unknown type name");
    }
    const Type *type = dynamic_cast<const Type *>(name);
    if (type == nullptr) {
        error(2016, tokens[i], "name is not a type");
    }
    i++;
    return type;
}

const Statement *Analyzer::parseTypeDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2018, tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    if (scope->lookupName(name) != nullptr) {
        error(2019, tokens[i], "name shadows outer");
    }
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2020, tokens[i], "':=' expected");
    }
    ++i;
    const Type *type = parseType(scope);
    scope->addName(name, const_cast<Type *>(type)); // Still ugly.
    const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
    if (rectype != nullptr) {
        scope->resolveForward(name, rectype);
    }
    return nullptr;
}

const Statement *Analyzer::parseConstantDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2021, tokens[i], "identifier expected");
    }
    std::string name = tokens[i].text;
    if (scope->lookupName(name) != nullptr) {
        error(2022, tokens[i], "name shadows outer");
    }
    ++i;
    if (tokens[i].type != COLON) {
        error(2023, tokens[i], "':' expected");
    }
    ++i;
    const Type *type = parseType(scope);
    if (tokens[i].type != ASSIGN) {
        error(2024, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *value = parseExpression(scope);
    if (not value->type->is_equivalent(type)) {
        error(2025, tokens[i], "type mismatch");
    }
    if (not value->is_constant) {
        error(2026, tokens[i], "value must be constant");
    }
    scope->addName(name, new Constant(name, value));
    return nullptr;
}

const FunctionCall *Analyzer::parseFunctionCall(const Expression *func, const Expression *self, Scope *scope)
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        error(2027, tokens[i-1], "not a function");
    }
    ++i;
    std::vector<const Type *>::size_type p = 0;
    std::vector<const Expression *> args;
    if (self != nullptr) {
        args.push_back(self);
        ++p;
    }
    if (tokens[i].type != RPAREN) {
        for (;;) {
            const Expression *e = parseExpression(scope);
            if (p >= ftype->params.size()) {
                error(2167, tokens[i], "too many parameters");
            }
            if (ftype->params[p]->mode != ParameterType::IN) {
                const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
                if (ref == nullptr) {
                    error(2028, tokens[i], "function call argument must be reference: " + e->text());
                }
                if (ref != nullptr && ref->is_readonly) {
                    error(2181, tokens[i], "readonly parameter to OUT");
                }
            }
            if (not e->type->is_equivalent(ftype->params[p]->type)) {
                error(2029, tokens[i], "type mismatch");
            }
            args.push_back(e);
            ++p;
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2030, tokens[i], "')' or ',' expected");
        }
    }
    if (p < ftype->params.size()) {
        error(2031, tokens[i], "not enough arguments");
    }
    ++i;
    return new FunctionCall(func, args);
}

const ArrayLiteralExpression *Analyzer::parseArrayLiteral(Scope *scope)
{
    std::vector<const Expression *> elements;
    const Type *elementtype = nullptr;
    while (tokens[i].type != RBRACKET) {
        const Expression *element = parseExpression(scope);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(2138, tokens[i], "type mismatch");
        }
        elements.push_back(element);
        if (tokens[i].type == COMMA) {
            ++i;
        } else if (tokens[i].type != RBRACKET) {
            error(2139, tokens[i], "',' or ']' expected");
        }
    }
    ++i;
    return new ArrayLiteralExpression(elementtype, elements);
}

const DictionaryLiteralExpression *Analyzer::parseDictionaryLiteral(Scope *scope)
{
    std::vector<std::pair<std::string, const Expression *>> elements;
    std::set<std::string> keys;
    const Type *elementtype = nullptr;
    while (tokens[i].type == STRING) {
        std::string key = tokens[i].text;
        if (not keys.insert(key).second) {
            error(2140, tokens[i], "duplicate key");
        }
        ++i;
        if (tokens[i].type != COLON) {
            error(2126, tokens[i], "':' expected");
        }
        ++i;
        const Expression *element = parseExpression(scope);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(2127, tokens[i], "type mismatch");
        }
        elements.push_back(std::make_pair(key, element));
        if (tokens[i].type == COMMA) {
            ++i;
        }
    }
    return new DictionaryLiteralExpression(elementtype, elements);
}

const Name *Analyzer::parseQualifiedName(Scope *scope, int &enclosing)
{
    const Name *name = scope->lookupName(tokens[i].text, enclosing);
    if (name == nullptr) {
        error(2059, tokens[i], "name not found: " + tokens[i].text);
    }
    const Module *module = dynamic_cast<const Module *>(name);
    if (module != nullptr) {
        ++i;
        if (tokens[i].type != DOT) {
            error(2060, tokens[i], "'.' expected");
        }
        ++i;
        return parseQualifiedName(module->scope, enclosing);
    }
    return name;
}

const Expression *Analyzer::parseInterpolatedStringExpression(Scope *scope)
{
    const VariableExpression *concat = new VariableExpression(dynamic_cast<const Variable *>(scope->lookupName("concat")));
    const VariableExpression *format = new VariableExpression(dynamic_cast<const Variable *>(scope->lookupName("format")));
    const Expression *expr = new ConstantStringExpression(tokens[i].text);
    for (;;) {
        ++i;
        if (tokens[i].type != SUBBEGIN) {
            break;
        }
        ++i;
        const Expression *e = parseExpression(scope);
        std::string fmt;
        if (tokens[i].type == SUBFMT) {
            ++i;
            if (tokens[i].type != STRING) {
                internal_error("parseInterpolatedStringExpression");
            }
            fmt = tokens[i].text;
            format::Spec spec;
            if (not format::parse(fmt, spec)) {
                error(2218, tokens[i], "invalid format specification");
            }
            ++i;
        }
        if (tokens[i].type != SUBEND) {
            internal_error("parseInterpolatedStringExpression");
        }
        ++i;
        const Expression *str;
        if (e->type->is_equivalent(TYPE_STRING)) {
            str = e;
        } else {
            auto to_string = e->type->methods.find("to_string");
            if (to_string == e->type->methods.end()) {
                error(2217, tokens[i], "no to_string() method found for type");
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
        {
            std::vector<const Expression *> args;
            args.push_back(expr);
            args.push_back(str);
            expr = new FunctionCall(concat, args);
        }
        if (tokens[i].type != STRING) {
            internal_error("parseInterpolatedStringExpression");
        }
        e = new ConstantStringExpression(tokens[i].text);
        {
            std::vector<const Expression *> args;
            args.push_back(expr);
            args.push_back(e);
            expr = new FunctionCall(concat, args);
        }
    }
    return expr;
}

/*
 * Operator precedence:
 *
 *  ^        exponentiation                     parseExponentiation
 *  * / MOD  multiplication, division, modulo   parseMultiplication
 *  + -      addition, subtraction              parseAddition
 *  < = >    comparison                         parseComparison
 *  in       membership                         parseMembership
 *  and      conjunction                        parseConjunction
 *  or       disjunction                        parseDisjunction
 *  if       conditional                        parseConditional
 */

const Expression *Analyzer::parseAtom(Scope *scope)
{
    switch (tokens[i].type) {
        case LPAREN: {
            ++i;
            const Expression *expr = parseExpression(scope);
            if (tokens[i].type != RPAREN) {
                error(2032, tokens[i], ") expected");
            }
            ++i;
            return expr;
        }
        case LBRACKET: {
            ++i;
            const Expression *array = parseArrayLiteral(scope);
            return array;
        }
        case LBRACE: {
            ++i;
            const Expression *dict = parseDictionaryLiteral(scope);
            if (tokens[i].type != RBRACE) {
                error(2128, tokens[i], "'}' expected");
            }
            ++i;
            return dict;
        }
        case FALSE: {
            ++i;
            return new ConstantBooleanExpression(false);
        }
        case TRUE: {
            ++i;
            return new ConstantBooleanExpression(true);
        }
        case NUMBER: {
            return new ConstantNumberExpression(tokens[i++].value);
        }
        case STRING: {
            if (tokens[i+1].type == SUBBEGIN) {
                return parseInterpolatedStringExpression(scope);
            } else {
                return new ConstantStringExpression(tokens[i++].text);
            }
        }
        case MINUS: {
            auto op = i;
            ++i;
            const Expression *factor = parseAtom(scope);
            if (not factor->type->is_equivalent(TYPE_NUMBER)) {
                error(2033, tokens[op], "number required for negation");
            }
            return new UnaryMinusExpression(factor);
        }
        case NOT: {
            auto op = i;
            ++i;
            const Expression *atom = parseAtom(scope);
            if (not atom->type->is_equivalent(TYPE_BOOLEAN)) {
                error(2034, tokens[op], "boolean required for logical not");
            }
            return new LogicalNotExpression(atom);
        }
        case NEW: {
            ++i;
            const TypeRecord *type = dynamic_cast<const TypeRecord *>(parseType(scope));
            if (type == nullptr) {
                error(2172, tokens[i], "record type expected");
            }
            return new NewRecordExpression(type);
        }
        case NIL: {
            ++i;
            return new ConstantNilExpression();
        }
        case IDENTIFIER: {
            const Name *name = scope->lookupName(tokens[i].text);
            const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(name);
            const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(name);
            /* TODO: This allows referencing enum values for a variable
                     declared with an anonymous enum type. But it currently
                     conflicts with method call syntax like a.to_string().
                     See: t/enum2.neon
            if (name != nullptr && enumtype == nullptr && tokens[i+1].type == DOT) {
                enumtype = dynamic_cast<const TypeEnum *>(name->type);
            }*/
            if (enumtype != nullptr) {
                ++i;
                if (tokens[i].type != DOT) {
                    error(2035, tokens[i], "'.' expected");
                }
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2036, tokens[i], "identifier expected");
                }
                auto name = enumtype->names.find(tokens[i].text);
                if (name == enumtype->names.end()) {
                    error(2037, tokens[i], "identifier not member of enum: " + tokens[i].text);
                }
                ++i;
                return new ConstantEnumExpression(enumtype, name->second);
            } else if (recordtype != nullptr) {
                ++i;
                if (tokens[i].type != LPAREN) {
                    error(2213, tokens[i], "'(' expected");
                }
                ++i;
                std::vector<const Expression *> elements;
                auto f = recordtype->fields.begin();
                while (tokens[i].type != RPAREN) {
                    if (f == recordtype->fields.end()) {
                        error(2214, tokens[i], "too many fields");
                    }
                    const Expression *element = parseExpression(scope);
                    if (not element->type->is_equivalent(f->second)) {
                        error(2215, tokens[i], "type mismatch");
                    }
                    elements.push_back(element);
                    if (tokens[i].type == COMMA) {
                        ++i;
                        ++f;
                    } else if (tokens[i].type != RPAREN) {
                        error(2216, tokens[i], "',' or ']' expected");
                    }
                }
                ++i;
                return new RecordLiteralExpression(recordtype, elements);
            } else {
                int enclosing;
                const Name *name = parseQualifiedName(scope, enclosing);
                const Constant *cons = dynamic_cast<const Constant *>(name);
                // TODO: Need to look up methods on constants too (t/const-boolean.neon)
                if (cons != nullptr) {
                    ++i;
                    return cons->value;
                }
                const Variable *var = dynamic_cast<const Variable *>(name);
                if (var == nullptr) {
                    error(2061, tokens[i], "name is not a variable: " + tokens[i].text);
                }
                const Expression *expr = new VariableExpression(var);
                const Type *type = var->type;
                ++i;
                for (;;) {
                    if (tokens[i].type == LBRACKET) {
                        const TypeArray *arraytype = dynamic_cast<const TypeArray *>(type);
                        const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(type);
                        if (arraytype != nullptr) {
                            ++i;
                            const Expression *index = parseExpression(scope);
                            if (not index->type->is_equivalent(TYPE_NUMBER)) {
                                error(2062, tokens[i], "index must be a number");
                            }
                            if (tokens[i].type != RBRACKET) {
                                error(2063, tokens[i], "']' expected");
                            }
                            ++i;
                            type = arraytype->elementtype;
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new ArrayReferenceIndexExpression(type, ref, index, false);
                            } else {
                                expr = new ArrayValueIndexExpression(type, expr, index, false);
                            }
                        } else if (dicttype != nullptr) {
                            ++i;
                            const Expression *index = parseExpression(scope);
                            if (not index->type->is_equivalent(TYPE_STRING)) {
                                error(2064, tokens[i], "index must be a string");
                            }
                            if (tokens[i].type != RBRACKET) {
                                error(2065, tokens[i], "']' expected");
                            }
                            ++i;
                            type = dicttype->elementtype;
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new DictionaryReferenceIndexExpression(type, ref, index);
                            } else {
                                expr = new DictionaryValueIndexExpression(type, expr, index);
                            }
                        } else if (type == TYPE_STRING) {
                            ++i;
                            const Expression *index = parseExpression(scope);
                            if (not index->type->is_equivalent(TYPE_NUMBER)) {
                                error(2066, tokens[i], "index must be a number");
                            }
                            if (tokens[i].type != RBRACKET) {
                                error(2067, tokens[i], "']' expected");
                            }
                            ++i;
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new StringReferenceIndexExpression(ref, index);
                            } else {
                                expr = new StringValueIndexExpression(expr, index);
                            }
                        } else {
                            error(2068, tokens[i], "not an array or dictionary");
                        }
                    } else if (tokens[i].type == LPAREN) {
                        expr = parseFunctionCall(expr, nullptr, scope);
                        type = expr->type;
                    } else if (tokens[i].type == DOT) {
                        ++i;
                        if (tokens[i].type != IDENTIFIER) {
                            error(2069, tokens[i], "identifier expected");
                        }
                        const std::string field = tokens[i].text;
                        auto m = type->methods.find(field);
                        const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(type);
                        if (m != type->methods.end()) {
                            ++i;
                            if (tokens[i].type != LPAREN) {
                                error(2196, tokens[i], "'(' expected");
                            }
                            expr = parseFunctionCall(new VariableExpression(m->second), expr, scope);
                        } else if (recordtype != nullptr) {
                            if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
                                internal_error("record not defined yet");
                            }
                            auto f = recordtype->field_names.find(field);
                            if (f == recordtype->field_names.end()) {
                                error(2070, tokens[i], "field not found");
                            }
                            ++i;
                            type = recordtype->fields[f->second].second;
                            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
                            if (ref != nullptr) {
                                expr = new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
                            } else {
                                expr = new ArrayValueIndexExpression(type, expr, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
                            }
                        } else {
                            error(2071, tokens[i], "no method found or not a record");
                        }
                    } else if (tokens[i].type == ARROW) {
                        const TypePointer *pointertype = dynamic_cast<const TypePointer *>(type);
                        if (pointertype != nullptr) {
                            if (dynamic_cast<const TypeValidPointer *>(pointertype) == nullptr) {
                                error(2178, tokens[i], "pointer must be a valid pointer");
                            }
                            const TypeRecord *recordtype = pointertype->reftype;
                            if (recordtype == nullptr) {
                                error(2194, tokens[i], "pointer must not be a generic pointer");
                            }
                            ++i;
                            if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
                                error(2179, tokens[i], "record not defined yet");
                            }
                            if (tokens[i].type != IDENTIFIER) {
                                error(2186, tokens[i], "identifier expected");
                            }
                            auto f = recordtype->field_names.find(tokens[i].text);
                            if (f == recordtype->field_names.end()) {
                                error(2187, tokens[i], "field not found");
                            }
                            ++i;
                            type = recordtype->fields[f->second].second;
                            const PointerDereferenceExpression *ref = new PointerDereferenceExpression(type, expr);
                            expr = new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), false);
                        } else {
                            error(2188, tokens[i], "not a pointer");
                        }
                    } else {
                        break;
                    }
                }
                return expr;
            }
        }
        default:
            error(2038, tokens[i], "Expression expected");
    }
}

const Expression *Analyzer::parseExponentiation(Scope *scope)
{
    const Expression *left = parseAtom(scope);
    for (;;) {
        if (tokens[i].type == EXP) {
            auto op = i;
            ++i;
            const Expression *right = parseAtom(scope);
            if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                left = new ExponentiationExpression(left, right);
            } else {
                error(2039, tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

const Expression *Analyzer::parseMultiplication(Scope *scope)
{
    const Expression *left = parseExponentiation(scope);
    for (;;) {
        switch (tokens[i].type) {
            case TIMES: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new MultiplicationExpression(left, right);
                } else {
                    error(2040, tokens[op], "type mismatch");
                }
                break;
            }
            case DIVIDE: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new DivisionExpression(left, right);
                } else {
                    error(2041, tokens[op], "type mismatch");
                }
                break;
            }
            case MOD: {
                auto op = i;
                ++i;
                const Expression *right = parseExponentiation(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new ModuloExpression(left, right);
                } else {
                    error(2042, tokens[op], "type mismatch");
                }
                break;
            }
            default:
                return left;
        }
    }
}

const Expression *Analyzer::parseAddition(Scope *scope)
{
    const Expression *left = parseMultiplication(scope);
    for (;;) {
        switch (tokens[i].type) {
            case PLUS: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new AdditionExpression(left, right);
                } else if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
                    error(2206, tokens[op], "type mismatch (use & to concatenate strings)");
                } else {
                    error(2043, tokens[op], "type mismatch");
                }
                break;
            }
            case MINUS: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope);
                if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
                    left = new SubtractionExpression(left, right);
                } else {
                    error(2044, tokens[op], "type mismatch");
                }
                break;
            }
            case CONCAT: {
                auto op = i;
                ++i;
                const Expression *right = parseMultiplication(scope);
                if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
                    std::vector<const Expression *> args;
                    args.push_back(left);
                    args.push_back(right);
                    left = new FunctionCall(new VariableExpression(dynamic_cast<const Variable *>(scope->lookupName("concat"))), args);
                } else {
                    error(2193, tokens[op], "type mismatch");
                }
                break;
            }
            default:
                return left;
        }
    }
}

const Expression *Analyzer::parseComparison(Scope *scope)
{
    const Expression *left = parseAddition(scope);
    std::vector<const ComparisonExpression *> comps;
    while (tokens[i].type == EQUAL  || tokens[i].type == NOTEQUAL
        || tokens[i].type == LESS   || tokens[i].type == GREATER
        || tokens[i].type == LESSEQ || tokens[i].type == GREATEREQ) {
        ComparisonExpression::Comparison comp = comparisonFromToken(tokens[i]);
        auto op = i;
        ++i;
        const Expression *right = parseAddition(scope);
        if (not left->type->is_equivalent(right->type)) {
            error(2045, tokens[op], "type mismatch");
        }
        const ComparisonExpression *c;
        if (left->type->is_equivalent(TYPE_BOOLEAN)) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2046, tokens[op], "comparison not available for Boolean");
            }
            c = new BooleanComparisonExpression(left, right, comp);
        } else if (left->type->is_equivalent(TYPE_NUMBER)) {
            c = new NumericComparisonExpression(left, right, comp);
        } else if (left->type->is_equivalent(TYPE_STRING)) {
            c = new StringComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2047, tokens[op], "comparison not available for Array");
            }
            c = new ArrayComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeDictionary *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2048, tokens[op], "comparison not available for Dictionary");
            }
            c = new DictionaryComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeRecord *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2049, tokens[op], "comparison not available for RECORD");
            }
            c = new ArrayComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypeEnum *>(left->type) != nullptr) {
            c = new NumericComparisonExpression(left, right, comp);
        } else if (dynamic_cast<const TypePointer *>(left->type) != nullptr) {
            if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
                error(2173, tokens[op], "comparison not available for POINTER");
            }
            c = new PointerComparisonExpression(left, right, comp);
        } else {
            internal_error("unknown type in parseComparison");
        }
        comps.push_back(c);
        left = right;
    }
    if (comps.empty()) {
        return left;
    } else if (comps.size() == 1) {
        return comps[0];
    } else {
        return new ChainedComparisonExpression(comps);
    }
}

const Expression *Analyzer::parseMembership(Scope *scope)
{
    const Expression *left = parseComparison(scope);
    if (tokens[i].type == IN || (tokens[i].type == NOT && tokens[i+1].type == IN)) {
        bool notin = tokens[i].type == NOT;
        if (notin) {
            ++i;
        }
        ++i;
        const Expression *right = parseComparison(scope);
        const TypeArray *arraytype = dynamic_cast<const TypeArray *>(right->type);
        const TypeDictionary *dicttype = dynamic_cast<const TypeDictionary *>(right->type);
        const Expression *r;
        if (arraytype != nullptr) {
            if (not left->type->is_equivalent(arraytype->elementtype)) {
                error(2142, tokens[i], "type mismatch");
            }
            r = new ArrayInExpression(left, right);
        } else if (dicttype != nullptr) {
            if (not left->type->is_equivalent(TYPE_STRING)) {
                error(2143, tokens[i], "type mismatch");
            }
            r = new DictionaryInExpression(left, right);
        } else {
            error(2141, tokens[i], "IN must be used with Array or Dictionary");
        }
        if (notin) {
            r = new LogicalNotExpression(r);
        }
        return r;
    } else {
        return left;
    }
}

const Expression *Analyzer::parseConjunction(Scope *scope)
{
    const Expression *left = parseMembership(scope);
    for (;;) {
        if (tokens[i].type == AND) {
            auto op = i;
            ++i;
            const Expression *right = parseMembership(scope);
            if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
                left = new ConjunctionExpression(left, right);
            } else {
                error(2051, tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

const Expression *Analyzer::parseDisjunction(Scope *scope)
{
    const Expression *left = parseConjunction(scope);
    for (;;) {
        if (tokens[i].type == OR) {
            auto op = i;
            ++i;
            const Expression *right = parseConjunction(scope);
            if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
                left = new DisjunctionExpression(left, right);
            } else {
                error(2052, tokens[op], "type mismatch");
            }
        } else {
            return left;
        }
    }
}

const Expression *Analyzer::parseConditional(Scope *scope)
{
    if (tokens[i].type == IF) {
        ++i;
        const Expression *cond = parseExpression(scope);
        if (tokens[i].type != THEN) {
            error(2053, tokens[i], "'THEN' expected");
        }
        ++i;
        const Expression *left = parseExpression(scope);
        if (tokens[i].type != ELSE) {
            error(2054, tokens[i], "'ELSE' expected");
        }
        ++i;
        const Expression *right = parseExpression(scope);
        if (not left->type->is_equivalent(right->type)) {
            error(2055, tokens[i], "type of THEN and ELSE must match");
        }
        return new ConditionalExpression(cond, left, right);
    } else {
        return parseDisjunction(scope);
    }
}

const Expression *Analyzer::parseExpression(Scope *scope, bool allow_nothing)
{
    expression_depth++;
    if (expression_depth > 100) {
        error(2197, tokens[i], "exceeded maximum nesting depth");
    }
    const Expression *r = parseConditional(scope);
    if (not allow_nothing && r->type == TYPE_NOTHING) {
        error(2192, tokens[i], "function does not return anything");
    }
    expression_depth--;
    return r;
}

typedef std::pair<std::vector<std::string>, const Type *> VariableInfo;

const VariableInfo Analyzer::parseVariableDeclaration(Scope *scope)
{
    std::vector<std::string> names;
    for (;;) {
        if (tokens[i].type != IDENTIFIER) {
            error(2056, tokens[i], "identifier expected");
        }
        std::string name = tokens[i].text;
        if (scope->lookupName(name) != nullptr) {
            error(2057, tokens[i], "name shadows outer");
        }
        ++i;
        names.push_back(name);
        if (tokens[i].type != COMMA) {
            break;
        }
        ++i;
    }
    if (tokens[i].type != COLON) {
        error(2058, tokens[i], "colon expected");
    }
    ++i;
    const Type *t = parseType(scope);
    return make_pair(names, t);
}

void Analyzer::parseFunctionHeader(Scope *scope, Type *&type, std::string &name, const Type *&returntype, Scope *&newscope, std::vector<FunctionParameter *> &args)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2073, tokens[i], "identifier expected");
    }
    name = tokens[i].text;
    ++i;
    type = nullptr;
    if (tokens[i].type == DOT) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2208, tokens[i], "identifier expected");
        }
        Name *tname = scope->lookupName(name);
        if (tname == nullptr) {
            error(2209, tokens[i], "type name not found");
        }
        type = dynamic_cast<Type *>(tname);
        if (type == nullptr) {
            error(2210, tokens[i], "type name is not a type");
        }
        name = tokens[i].text;
        ++i;
    }
    if (tokens[i].type != LPAREN) {
        error(2075, tokens[i], "'(' expected");
    }
    ++i;
    newscope = new Scope(scope);
    if (tokens[i].type != RPAREN) {
        for (;;) {
            ParameterType::Mode mode = ParameterType::IN;
            switch (tokens[i].type) {
                case IN:    mode = ParameterType::IN;       i++; break;
                case INOUT: mode = ParameterType::INOUT;    i++; break;
                case OUT:   mode = ParameterType::OUT;      i++; break;
                default:
                    break;
            }
            const VariableInfo vars = parseVariableDeclaration(newscope);
            for (auto name: vars.first) {
                if (type != nullptr && args.empty()) {
                    if (not vars.second->is_equivalent(type)) {
                        error(2211, tokens[i], "expected self parameter");
                    }
                }
                FunctionParameter *fp = new FunctionParameter(name, vars.second, mode, newscope);
                args.push_back(fp);
                newscope->addName(name, fp, true);
            }
            if (tokens[i].type != COMMA) {
                break;
            }
            ++i;
        }
        if (tokens[i].type != RPAREN) {
            error(2076, tokens[i], "')' or ',' expected");
        }
    }
    if (type != nullptr && args.empty()) {
        error(2212, tokens[i], "expected self parameter");
    }
    ++i;
    if (tokens[i].type == COLON) {
        ++i;
        returntype = parseType(newscope);
    } else {
        returntype = TYPE_NOTHING;
    }
}

const Statement *Analyzer::parseFunctionDefinition(Scope *scope)
{
    Type *type;
    std::string name;
    const Type *returntype;
    Scope *newscope;
    std::vector<FunctionParameter *> args;
    parseFunctionHeader(scope, type, name, returntype, newscope, args);
    Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(name);
        if (f != type->methods.end()) {
            function = dynamic_cast<Function *>(f->second);
            newscope = function->scope;
        } else {
            function = new Function(name, returntype, newscope, args);
            type->methods[name] = function;
        }
    } else {
        Name *ident = scope->lookupName(name);
        function = dynamic_cast<Function *>(ident);
        if (function != nullptr) {
            newscope = function->scope;
        } else if (ident != nullptr) {
            error(2074, tokens[i], "name shadows outer");
        } else {
            function = new Function(name, returntype, newscope, args);
            scope->addName(name, function);
        }
    }
    functiontypes.push(dynamic_cast<const TypeFunction *>(function->type));
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    while (tokens[i].type != END) {
        const Statement *s = parseStatement(newscope);
        if (s != nullptr) {
            function->statements.push_back(s);
        }
    }
    if (returntype != TYPE_NOTHING) {
        if (function->statements.empty() || not function->statements.back()->always_returns()) {
            error(2146, tokens[i], "missing RETURN statement");
        }
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2102, tokens[i], "'FUNCTION' expected");
    }
    ++i;
    loops.pop();
    functiontypes.pop();
    newscope->checkForward();
    return nullptr;
}

const Statement *Analyzer::parseExternalDefinition(Scope *scope)
{
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2122, tokens[i], "FUNCTION expected");
    }
    Type *type;
    std::string name;
    const Type *returntype;
    Scope *newscope;
    std::vector<FunctionParameter *> args;
    parseFunctionHeader(scope, type, name, returntype, newscope, args);
    if (scope->lookupName(name) != nullptr) {
        error(2163, tokens[i], "name shadows outer");
    }
    if (tokens[i].type != LBRACE) {
        error(2123, tokens[i], "{ expected");
    }
    ++i;
    const DictionaryLiteralExpression *dict = parseDictionaryLiteral(scope);
    if (not dict->is_constant) {
        error(2124, tokens[i], "constant dictionary expected");
    }
    if (dynamic_cast<const TypeDictionary *>(dict->elementtype) == nullptr) {
        error(2131, tokens[i], "top level dictionary element not dictionary");
    }
    for (auto elem: dict->dict) {
        auto *d = dynamic_cast<const DictionaryLiteralExpression *>(elem.second);
        if (not d->dict.empty() && not d->elementtype->is_equivalent(TYPE_STRING)) {
            error(2132, tokens[i], "sub level dictionary must have string elements");
        }
    }

    auto klibrary = dict->dict.find("library");
    if (klibrary == dict->dict.end()) {
        error(2133, tokens[i], "\"library\" key not found");
    }
    auto &library_dict = dynamic_cast<const DictionaryLiteralExpression *>(klibrary->second)->dict;
    auto kname = library_dict.find("name");
    if (kname == library_dict.end()) {
        error(2134, tokens[i], "\"name\" key not found in library");
    }
    std::string library_name = kname->second->eval_string();

    auto ktypes = dict->dict.find("types");
    if (ktypes == dict->dict.end()) {
        error(2135, tokens[i], "\"types\" key not found");
    }
    auto &types_dict = dynamic_cast<const DictionaryLiteralExpression *>(ktypes->second)->dict;
    std::map<std::string, std::string> param_types;
    for (auto paramtype: types_dict) {
        param_types[paramtype.first] = paramtype.second->eval_string();
    }
    for (auto a: args) {
        if (param_types.find(a->name) == param_types.end()) {
            error(2168, tokens[i], "parameter type missing: " + a->name);
        }
    }

    ExternalFunction *function = new ExternalFunction(name, returntype, newscope, args, library_name, param_types);
    scope->addName(name, function);

    if (tokens[i].type != RBRACE) {
        error(2125, tokens[i], "} expected");
    }
    ++i;
    if (tokens[i].type != END) {
        error(2129, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FUNCTION) {
        error(2130, tokens[i], "'END FUNCTION' expected");
    }
    ++i;
    return nullptr;
}

const Statement *Analyzer::parseDeclaration(Scope *scope)
{
    ++i;
    switch (tokens[i].type) {
        case EXCEPTION: {
            ++i;
            if (tokens[i].type != IDENTIFIER) {
                error(2152, tokens[i], "identifier expected");
            }
            std::string name = tokens[i].text;
            if (scope->lookupName(name) != nullptr) {
                error(2191, tokens[i], "name shadows outer");
            }
            ++i;
            scope->addName(name, new Exception(name));
            break;
        }
        case FUNCTION: {
            Type *type;
            std::string name;
            const Type *returntype;
            Scope *newscope;
            std::vector<FunctionParameter *> args;
            parseFunctionHeader(scope, type, name, returntype, newscope, args);
            if (scope->lookupName(name) != nullptr) {
                error(2162, tokens[i], "name shadows outer");
            }
            Function *function = new Function(name, returntype, newscope, args);
            scope->addName(name, function);
            break;
        }
        default:
            error(2151, tokens[i], "EXCEPTION or FUNCTION expected");
    }
    return nullptr;
}

const Statement *Analyzer::parseIfStatement(Scope *scope, int line)
{
    std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    std::vector<const Statement *> else_statements;
    do {
        ++i;
        const Expression *cond = nullptr;
        std::string name;
        if (tokens[i].type == VALID) {
            for (;;) {
                ++i;
                if (tokens[i].type != IDENTIFIER) {
                    error(2174, tokens[i], "identifier expected");
                }
                name = tokens[i].text;
                if (scope->lookupName(name) != nullptr) {
                    error(2177, tokens[i], "name shadows outer");
                }
                ++i;
                if (tokens[i].type != ASSIGN) {
                    error(2175, tokens[i], "':=' expected");
                }
                ++i;
                const Expression *ptr = parseExpression(scope);
                const TypePointer *ptrtype = dynamic_cast<const TypePointer *>(ptr->type);
                if (ptrtype == nullptr) {
                    error(2176, tokens[i], "pointer type expression expected");
                }
                const TypeValidPointer *vtype = new TypeValidPointer(ptrtype);
                Variable *v;
                // TODO: Try to make this a local variable always (give the global scope a local space).
                if (functiontypes.empty()) {
                    v = new GlobalVariable(name, vtype, true);
                } else {
                    v = new LocalVariable(name, vtype, scope, true);
                }
                scope->addName(name, v, true);
                const Expression *valid = new ValidPointerExpression(v, ptr);
                if (cond == nullptr) {
                    cond = valid;
                } else {
                    cond = new ConjunctionExpression(cond, valid);
                }
                if (tokens[i].type != COMMA) {
                    break;
                }
            }
        } else {
            auto j = i;
            cond = parseExpression(scope);
            if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
                error(2078, tokens[j], "boolean value expected");
            }
        }
        if (tokens[i].type != THEN) {
            error(2079, tokens[i], "THEN expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != ELSIF && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope);
            if (s != nullptr) {
                statements.push_back(s);
            }
        }
        if (not name.empty()) {
            scope->scrubName(name);
        }
        condition_statements.push_back(std::make_pair(cond, statements));
    } while (tokens[i].type == ELSIF);
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *s = parseStatement(scope);
            if (s != nullptr) {
                else_statements.push_back(s);
            }
        }
    }
    if (tokens[i].type != END) {
        error(2080, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != IF) {
        error(2103, tokens[i], "IF expected");
    }
    ++i;
    return new IfStatement(line, condition_statements, else_statements);
}

const Statement *Analyzer::parseReturnStatement(Scope *scope, int line)
{
    const auto token_return = tokens[i];
    ++i;
    const Expression *expr = parseExpression(scope);
    if (functiontypes.empty()) {
        error(2164, token_return, "RETURN not allowed outside function");
    } else if (functiontypes.top()->returntype == TYPE_NOTHING) {
        error(2165, token_return, "function does not return a value");
    } else if (not expr->type->is_equivalent(functiontypes.top()->returntype)) {
        error(2166, token_return, "type mismatch in RETURN");
    }
    return new ReturnStatement(line, expr);
}

const Statement *Analyzer::parseVarStatement(Scope *scope, int line)
{
    ++i;
    std::vector<Variable *> variables;
    const VariableInfo vars = parseVariableDeclaration(scope);
    for (auto name: vars.first) {
        Variable *v;
        if (scope == global_scope) {
            v = new GlobalVariable(name, vars.second, false);
        } else {
            v = new LocalVariable(name, vars.second, scope, false);
        }
        variables.push_back(v);
    }
    const Statement *r = nullptr;
    if (tokens[i].type == ASSIGN) {
        ++i;
        std::vector<const ReferenceExpression *> refs;
        const Expression *expr = parseExpression(scope);
        if (not expr->type->is_equivalent(vars.second)) {
            error(2189, tokens[i], "type mismatch");
        }
        for (auto v: variables) {
            scope->addName(v->name, v, true);
            refs.push_back(new VariableExpression(v));
        }
        r =  new AssignmentStatement(line, refs, expr);
    } else {
        for (auto v: variables) {
            scope->addName(v->name, v);
        }
    }
    return r;
}

const Statement *Analyzer::parseWhileStatement(Scope *scope, int line)
{
    ++i;
    auto j = i;
    const Expression *cond = parseExpression(scope);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(2081, tokens[j], "boolean value expected");
    }
    if (tokens[i].type != DO) {
        error(2082, tokens[i], "DO expected");
    }
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(WHILE, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != END) {
        error(2083, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != WHILE) {
        error(2104, tokens[i], "WHILE expected");
    }
    ++i;
    loops.top().pop_back();
    return new WhileStatement(line, loop_id, cond, statements);
}

const Statement *Analyzer::parseCaseStatement(Scope *scope, int line)
{
    ++i;
    const Expression *expr = parseExpression(scope);
    if (not expr->type->is_equivalent(TYPE_NUMBER) && not expr->type->is_equivalent(TYPE_STRING) && dynamic_cast<const TypeEnum *>(expr->type) == nullptr) {
        error(2084, tokens[i], "CASE expression must be Number, String, or ENUM");
    }
    std::vector<std::pair<std::vector<const CaseStatement::WhenCondition *>, std::vector<const Statement *>>> clauses;
    while (tokens[i].type == WHEN) {
        std::vector<const CaseStatement::WhenCondition *> conditions;
        do {
            ++i;
            switch (tokens[i].type) {
                case EQUAL:
                case NOTEQUAL:
                case LESS:
                case GREATER:
                case LESSEQ:
                case GREATEREQ: {
                    auto op = tokens[i];
                    ++i;
                    const Expression *when = parseExpression(scope);
                    if (not when->type->is_equivalent(expr->type)) {
                        error(2085, tokens[i], "type mismatch");
                    }
                    if (not when->is_constant) {
                        error(2086, tokens[i], "WHEN condition must be constant");
                    }
                    const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(comparisonFromToken(op), when);
                    for (auto clause: clauses) {
                        for (auto c: clause.first) {
                            if (cond->overlaps(c)) {
                                error(2106, tokens[i], "overlapping case condition");
                            }
                        }
                    }
                    for (auto c: conditions) {
                        if (cond->overlaps(c)) {
                            error(2107, tokens[i], "overlapping case condition");
                        }
                    }
                    conditions.push_back(cond);
                    break;
                }
                default: {
                    const Expression *when = parseExpression(scope);
                    if (not when->type->is_equivalent(expr->type)) {
                        error(2087, tokens[i], "type mismatch");
                    }
                    if (not when->is_constant) {
                        error(2088, tokens[i], "WHEN condition must be constant");
                    }
                    if (tokens[i].type == TO) {
                        ++i;
                        const Expression *when2 = parseExpression(scope);
                        if (not when2->type->is_equivalent(expr->type)) {
                            error(2089, tokens[i], "type mismatch");
                        }
                        if (not when2->is_constant) {
                            error(2090, tokens[i], "WHEN condition must be constant");
                        }
                        if (when->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(when->type) != nullptr) {
                            if (number_is_greater(when->eval_number(), when2->eval_number())) {
                                error(2184, tokens[i], "WHEN numeric range condition must be low..high");
                            }
                        } else if (when->type->is_equivalent(TYPE_STRING)) {
                            if (when->eval_string() > when2->eval_string()) {
                                error(2185, tokens[i], "WHEN string range condition must be low..high");
                            }
                        } else {
                            internal_error("range condition type");
                        }
                        const CaseStatement::WhenCondition *cond = new CaseStatement::RangeWhenCondition(when, when2);
                        for (auto clause: clauses) {
                            for (auto c: clause.first) {
                                if (cond->overlaps(c)) {
                                    error(2108, tokens[i], "overlapping case condition");
                                }
                            }
                        }
                        for (auto c: conditions) {
                            if (cond->overlaps(c)) {
                                error(2109, tokens[i], "overlapping case condition");
                            }
                        }
                        conditions.push_back(cond);
                    } else {
                        const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(ComparisonExpression::EQ, when);
                        for (auto clause: clauses) {
                            for (auto c: clause.first) {
                                if (cond->overlaps(c)) {
                                    error(2110, tokens[i], "overlapping case condition");
                                }
                            }
                        }
                        for (auto c: conditions) {
                            if (cond->overlaps(c)) {
                                error(2111, tokens[i], "overlapping case condition");
                            }
                        }
                        conditions.push_back(cond);
                    }
                    break;
                }
            }
        } while (tokens[i].type == COMMA);
        if (tokens[i].type != DO) {
            error(2091, tokens[i], "'DO' expected");
        }
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != WHEN && tokens[i].type != ELSE && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *stmt = parseStatement(scope);
            if (stmt != nullptr) {
                statements.push_back(stmt);
            }
        }
        clauses.push_back(std::make_pair(conditions, statements));
    }
    std::vector<const Statement *> else_statements;
    if (tokens[i].type == ELSE) {
        ++i;
        while (tokens[i].type != END) {
            const Statement *stmt = parseStatement(scope);
            if (stmt != nullptr) {
                else_statements.push_back(stmt);
            }
        }
    }
    if (tokens[i].type != END) {
        error(2092, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != CASE) {
        error(2105, tokens[i], "CASE expected");
    }
    ++i;
    clauses.push_back(std::make_pair(std::vector<const CaseStatement::WhenCondition *>(), else_statements));
    return new CaseStatement(line, expr, clauses);
}

const Statement *Analyzer::parseForStatement(Scope *scope, int line)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2072, tokens[i], "identifier expected");
    }
    Name *name = scope->lookupName(tokens[i].text);
    Variable *var = dynamic_cast<Variable *>(name);
    if (var == nullptr) {
        error(2195, tokens[i], "name not a variable: " + tokens[i].text);
    }
    if (not var->type->is_equivalent(TYPE_NUMBER)) {
        error(2112, tokens[i], "type mismatch");
    }
    if (var->is_readonly) {
        error(2207, tokens[i], "cannot use readonly variable in FOR loop");
    }
    var->is_readonly = true;
    ++i;
    if (tokens[i].type != ASSIGN) {
        error(2121, tokens[i], "':=' expected");
    }
    ++i;
    const Expression *start = parseExpression(scope);
    if (not start->type->is_equivalent(TYPE_NUMBER)) {
        error(2113, tokens[i], "numeric expression expected");
    }
    if (tokens[i].type != TO) {
        error(2114, tokens[i], "TO expected");
    }
    ++i;
    const Expression *end = parseExpression(scope);
    if (not end->type->is_equivalent(TYPE_NUMBER)) {
        error(2115, tokens[i], "numeric expression expected");
    }

    const Expression *step = nullptr;
    if (tokens[i].type == STEP) {
        ++i;
        step = parseExpression(scope);
        if (step->type != TYPE_NUMBER) {
            error(2116, tokens[i], "numeric expression expected");
        }
        if (not step->is_constant) {
            error(2117, tokens[i], "numeric expression must be constant");
        }
        if (number_is_zero(step->eval_number())) {
            error(2161, tokens[i], "STEP value cannot be zero");
        }
    } else {
        step = new ConstantNumberExpression(number_from_uint32(1));
    }
    if (tokens[i].type != DO) {
        error(2118, tokens[i], "'DO' expected");
    }
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(FOR, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != END) {
        error(2119, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != FOR) {
        error(2120, tokens[i], "'END FOR' expected");
    }
    ++i;
    loops.top().pop_back();
    var->is_readonly = false;
    return new ForStatement(line, loop_id, new VariableExpression(var), start, end, step, statements);
}

const Statement *Analyzer::parseLoopStatement(Scope *scope, int line)
{
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(LOOP, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != END) {
        error(2147, tokens[i], "END expected");
    }
    ++i;
    if (tokens[i].type != LOOP) {
        error(2148, tokens[i], "LOOP expected");
    }
    ++i;
    loops.top().pop_back();
    return new LoopStatement(line, loop_id, statements);
}

const Statement *Analyzer::parseRepeatStatement(Scope *scope, int line)
{
    ++i;
    unsigned int loop_id = static_cast<unsigned int>(i);
    loops.top().push_back(std::make_pair(REPEAT, loop_id));
    std::vector<const Statement *> statements;
    while (tokens[i].type != UNTIL && tokens[i].type != END_OF_FILE) {
        const Statement *s = parseStatement(scope);
        if (s != nullptr) {
            statements.push_back(s);
        }
    }
    if (tokens[i].type != UNTIL) {
        error(2149, tokens[i], "UNTIL expected");
    }
    ++i;
    const Expression *cond = parseExpression(scope);
    if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
        error(2150, tokens[i], "boolean value expected");
    }
    loops.top().pop_back();
    return new RepeatStatement(line, loop_id, cond, statements);
}

const Statement *Analyzer::parseExitStatement(Scope *, int line)
{
    ++i;
    if (tokens[i].type == FUNCTION) {
        if (functiontypes.empty()) {
            error(2182, tokens[i], "EXIT FUNCTION not allowed outside function");
        } else if (functiontypes.top()->returntype != TYPE_NOTHING) {
            error(2183, tokens[i], "function must return a value");
        }
        i++;
        return new ReturnStatement(line, nullptr);
    }
    if (tokens[i].type != WHILE
     && tokens[i].type != FOR
     && tokens[i].type != LOOP
     && tokens[i].type != REPEAT) {
        error(2136, tokens[i], "loop type expected");
    }
    TokenType type = tokens[i].type;
    ++i;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new ExitStatement(line, j->second);
            }
        }
    }
    error(2137, tokens[i-1], "no matching loop found in current scope");
}

const Statement *Analyzer::parseNextStatement(Scope *, int line)
{
    ++i;
    if (tokens[i].type != WHILE
     && tokens[i].type != FOR
     && tokens[i].type != LOOP
     && tokens[i].type != REPEAT) {
        error(2144, tokens[i], "loop type expected");
    }
    TokenType type = tokens[i].type;
    ++i;
    if (not loops.empty()) {
        for (auto j = loops.top().rbegin(); j != loops.top().rend(); ++j) {
            if (j->first == type) {
                return new NextStatement(line, j->second);
            }
        }
    }
    error(2145, tokens[i-1], "no matching loop found in current scope");
}

const Statement *Analyzer::parseTryStatement(Scope *scope, int line)
{
    ++i;
    std::vector<const Statement *> statements;
    while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
        const Statement *stmt = parseStatement(scope);
        if (stmt != nullptr) {
            statements.push_back(stmt);
        }
    }
    std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;
    while (tokens[i].type == EXCEPTION) {
        ++i;
        if (tokens[i].type != IDENTIFIER) {
            error(2153, tokens[i], "identifier expected");
        }
        const Name *name = scope->lookupName(tokens[i].text);
        if (name == nullptr) {
            error(2154, tokens[i], "exception not found: " + tokens[i].text);
        }
        const Exception *exception = dynamic_cast<const Exception *>(name);
        if (exception == nullptr) {
            error(2155, tokens[i], "name not an exception");
        }
        std::vector<const Exception *> exceptions;
        exceptions.push_back(exception);
        ++i;
        std::vector<const Statement *> statements;
        while (tokens[i].type != EXCEPTION && tokens[i].type != END && tokens[i].type != END_OF_FILE) {
            const Statement *stmt = parseStatement(scope);
            if (stmt != nullptr) {
                statements.push_back(stmt);
            }
        }
        catches.push_back(std::make_pair(exceptions, statements));
    }
    if (tokens[i].type != END) {
        error(2159, tokens[i], "'END' expected");
    }
    ++i;
    if (tokens[i].type != TRY) {
        error(2160, tokens[i], "TRY expected");
    }
    ++i;
    return new TryStatement(line, statements, catches);
}

const Statement *Analyzer::parseRaiseStatement(Scope *scope, int line)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2156, tokens[i], "identifier expected");
    }
    const Name *name = scope->lookupName(tokens[i].text);
    if (name == nullptr) {
        error(2157, tokens[i], "exception not found: " + tokens[i].text);
    }
    const Exception *exception = dynamic_cast<const Exception *>(name);
    if (exception == nullptr) {
        error(2158, tokens[i], "name not an exception");
    }
    ++i;
    const Expression *info = nullptr;
    if (tokens[i].type == LPAREN) {
        info = parseExpression(scope);
    } else {
        info = new ConstantStringExpression("");
    }
    return new RaiseStatement(line, exception, info);
}

const Statement *Analyzer::parseImport(Scope *scope)
{
    ++i;
    if (tokens[i].type != IDENTIFIER) {
        error(2093, tokens[i], "identifier expected");
    }
    if (scope->lookupName(tokens[i].text) != nullptr) {
        error(2190, tokens[i], "name shadows outer");
    }
    rtl_import(scope, tokens[i].text);
    ++i;
    return nullptr;
}

const Statement *Analyzer::parseStatement(Scope *scope)
{
    const int line = tokens[i].line;
    if (tokens[i].type == IMPORT) {
        return parseImport(scope);
    } else if (tokens[i].type == TYPE) {
        return parseTypeDefinition(scope);
    } else if (tokens[i].type == CONST) {
        return parseConstantDefinition(scope);
    } else if (tokens[i].type == FUNCTION) {
        return parseFunctionDefinition(scope);
    } else if (tokens[i].type == EXTERNAL) {
        return parseExternalDefinition(scope);
    } else if (tokens[i].type == DECLARE) {
        return parseDeclaration(scope);
    } else if (tokens[i].type == IF) {
        return parseIfStatement(scope, line);
    } else if (tokens[i].type == RETURN) {
        return parseReturnStatement(scope, line);
    } else if (tokens[i].type == VAR) {
        return parseVarStatement(scope, line);
    } else if (tokens[i].type == WHILE) {
        return parseWhileStatement(scope, line);
    } else if (tokens[i].type == CASE) {
        return parseCaseStatement(scope, line);
    } else if (tokens[i].type == FOR) {
        return parseForStatement(scope, line);
    } else if (tokens[i].type == LOOP) {
        return parseLoopStatement(scope, line);
    } else if (tokens[i].type == REPEAT) {
        return parseRepeatStatement(scope, line);
    } else if (tokens[i].type == EXIT) {
        return parseExitStatement(scope, line);
    } else if (tokens[i].type == NEXT) {
        return parseNextStatement(scope, line);
    } else if (tokens[i].type == TRY) {
        return parseTryStatement(scope, line);
    } else if (tokens[i].type == RAISE) {
        return parseRaiseStatement(scope, line);
    } else if (tokens[i].type == IDENTIFIER) {
        if (tokens[i].text == "value_copy") {
            ++i;
            if (tokens[i].type != LPAREN) {
                error(2198, tokens[i], "'(' expected");
            }
            ++i;
            auto tok_lhs = tokens[i];
            const Expression *expr = parseExpression(scope);
            const ReferenceExpression *lhs = dynamic_cast<const ReferenceExpression *>(expr);
            if (lhs == nullptr) {
                error(2199, tok_lhs, "expression is not assignable");
            }
            if (expr->is_readonly && dynamic_cast<const TypePointer *>(expr->type) == nullptr) {
                error(2200, tok_lhs, "value_copy to readonly expression");
            }

            if (tokens[i].type != COMMA) {
                error(2201, tokens[i], "',' expected");
            }
            ++i;
            auto tok_rhs = tokens[i];
            const Expression *rhs = parseExpression(scope);
            if (tokens[i].type != RPAREN) {
                error(2202, tokens[i], "')' expected");
            }
            ++i;
            const Type *ltype = lhs->type;
            const TypePointer *lptype = dynamic_cast<const TypePointer *>(ltype);
            if (lptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(lptype) == nullptr) {
                    error(2203, tok_lhs, "valid pointer type required");
                }
                ltype = lptype->reftype;
                lhs = new PointerDereferenceExpression(ltype, lhs);
            }
            const Type *rtype = rhs->type;
            const TypePointer *rptype = dynamic_cast<const TypePointer *>(rtype);
            if (rptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(rptype) == nullptr) {
                    error(2204, tok_rhs, "valid pointer type required");
                }
                rtype = rptype->reftype;
                rhs = new PointerDereferenceExpression(rtype, rhs);
            }
            if (not ltype->is_equivalent(rtype)) {
                error(2205, tok_rhs, "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(lhs);
            return new AssignmentStatement(line, vars, rhs);
        }
        const Expression *expr = parseExpression(scope, true);
        if (expr->type == TYPE_NOTHING) {
            return new ExpressionStatement(line, expr);
        } else if (tokens[i].type == ASSIGN) {
            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(expr);
            if (ref == nullptr) {
                error(2095, tokens[i], "expression is not assignable");
            }
            if (expr->is_readonly) {
                error(2180, tokens[i], "assignment to readonly expression");
            }
            auto op = i;
            ++i;
            const Expression *rhs = parseExpression(scope);
            if (not rhs->type->is_equivalent(expr->type)) {
                error(2094, tokens[op], "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(ref);
            return new AssignmentStatement(line, vars, rhs);
        } else if (dynamic_cast<const ComparisonExpression *>(expr) != nullptr) {
            error(2097, tokens[i], "':=' expected");
        } else if (dynamic_cast<const FunctionCall *>(expr) != nullptr) {
            error(2096, tokens[i], "return value unused");
        } else {
            error(2098, tokens[i], "Unexpected");
        }
    } else {
        error(2099, tokens[i], "Identifier expected");
    }
}

#endif

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
        error(2015, type->token, "unknown type name");
    }
    const Type *r = dynamic_cast<const Type *>(name);
    if (r == nullptr) {
        error(2016, type->token, "name is not a type");
    }
    return r;
}

const Type *Analyzer::analyze(const pt::TypeEnum *type)
{
    std::map<std::string, int> names;
    int index = 0;
    for (auto x: type->names) {
        std::string name = x.first.text;
        if (names.find(name) != names.end()) {
            error(2013, x.first, "duplicate enum: " + name);
        }
        names[name] = index;
        index++;
    }
    return new TypeEnum(names);
}

const Type *Analyzer::analyze(const pt::TypeRecord *type)
{
    std::vector<std::pair<std::string, const Type *>> fields;
    std::set<std::string> field_names;
    for (auto x: type->fields) {
        std::string name = x.first.text;
        if (field_names.find(name) != field_names.end()) {
            error(2009, x.first, "duplicate field: " + x.first.text);
        }
        const Type *t = analyze(x.second);
        fields.push_back(std::make_pair(name, t));
        field_names.insert(name);
    }
    return new TypeRecord(fields);
}

const Type *Analyzer::analyze(const pt::TypePointer *type)
{
    if (type->reftype != nullptr) {
        const pt::TypeSimple *simple = dynamic_cast<const pt::TypeSimple *>(type->reftype);
        if (simple != nullptr && scope.top()->lookupName(simple->name) == nullptr) {
            const std::string name = simple->name;
            TypePointer *ptrtype = new TypePointer(new TypeForwardRecord());
            scope.top()->addForward(name, ptrtype);
            return ptrtype;
        } else {
            const Type *reftype = analyze(type->reftype);
            const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(reftype);
            if (rectype == nullptr) {
                error(2171, type->reftype->token, "record type expected");
            }
            return new TypePointer(rectype);
        }
    } else {
        return new TypePointer(nullptr);
    }
}

const Type *Analyzer::analyze(const pt::TypeParameterised *type)
{
    if (type->name == "Array") {
        return new TypeArray(analyze(type->elementtype));
    }
    if (type->name == "Dictionary") {
        return new TypeDictionary(analyze(type->elementtype));
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
            error(2138, x->token, "type mismatch");
        }
        elements.push_back(element);
    }
    return new ArrayLiteralExpression(elementtype, elements);
}

const Expression *Analyzer::analyze(const pt::DictionaryLiteralExpression *expr)
{
    std::vector<std::pair<std::string, const Expression *>> elements;
    std::set<std::string> keys;
    const Type *elementtype = nullptr;
    for (auto x: expr->elements) {
        std::string key = x.first.text;
        if (not keys.insert(key).second) {
            error(2140, x.first, "duplicate key");
        }
        const Expression *element = analyze(x.second);
        if (elementtype == nullptr) {
            elementtype = element->type;
        } else if (not element->type->is_equivalent(elementtype)) {
            error(2127, x.second->token, "type mismatch");
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
        error(2059, expr->token, "name not found: " + expr->name);
    }
    const Constant *constant = dynamic_cast<const Constant *>(name);
    if (constant != nullptr) {
        return new ConstantExpression(constant);
    }
    const Variable *var = dynamic_cast<const Variable *>(name);
    if (var != nullptr) {
        return new VariableExpression(var);
    }
    error(2061, expr->token, "name is not a constant or variable: " + expr->name);
}

const Expression *Analyzer::analyze(const pt::DotExpression *expr)
{
    const pt::IdentifierExpression *scopename = dynamic_cast<const pt::IdentifierExpression *>(expr->base);
    if (scopename != nullptr) {
        const Name *name = scope.top()->lookupName(scopename->name);
        const Module *module = dynamic_cast<const Module *>(name);
        if (module != nullptr) {
            const Name *name = module->scope->lookupName(expr->name);
            if (name == nullptr) {
                error(2259, expr->token, "name not found: " + expr->name);
            }
            const Constant *constant = dynamic_cast<const Constant *>(name);
            if (constant != nullptr) {
                return new ConstantExpression(constant);
            }
            const Variable *var = dynamic_cast<const Variable *>(name);
            if (var != nullptr) {
                return new VariableExpression(var);
            }
            error(2261, expr->token, "name is not a variable: " + expr->name);
        }
        const TypeEnum *enumtype = dynamic_cast<const TypeEnum *>(name);
        if (enumtype != nullptr) {
            auto name = enumtype->names.find(expr->name);
            if (name == enumtype->names.end()) {
                error(2037, expr->token, "identifier not member of enum: " + expr->name);
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
        auto f = recordtype->field_names.find(expr->name);
        if (f == recordtype->field_names.end()) {
            error(2070, expr->token, "field not found");
        }
        const Type *type = recordtype->fields[f->second].second;
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ArrayReferenceIndexExpression(type, ref, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
        } else {
            return new ArrayValueIndexExpression(type, base, new ConstantNumberExpression(number_from_uint32(static_cast<uint32_t>(f->second))), true);
        }
    }
    error(2071, expr->token, "no method found or not a record");
}

const Expression *Analyzer::analyze(const pt::ArrowExpression *expr)
{
    const Expression *base = analyze(expr->base);
    const TypePointer *pointertype = dynamic_cast<const TypePointer *>(base->type);
    if (pointertype == nullptr) {
        error(2188, expr->token, "not a pointer");
    }
    if (dynamic_cast<const TypeValidPointer *>(pointertype) == nullptr) {
        error(2178, expr->token, "pointer must be a valid pointer");
    }
    const TypeRecord *recordtype = pointertype->reftype;
    if (recordtype == nullptr) {
        error(2194, expr->token, "pointer must not be a generic pointer");
    }
    if (dynamic_cast<const TypeForwardRecord *>(recordtype) != nullptr) {
        error(2179, expr->token, "record not defined yet");
    }
    auto f = recordtype->field_names.find(expr->name);
    if (f == recordtype->field_names.end()) {
        error(2187, expr->token, "field not found");
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
            error(2062, expr->index->token, "index must be a number");
        }
        type = arraytype->elementtype;
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new ArrayReferenceIndexExpression(type, ref, index, false);
        } else {
            return new ArrayValueIndexExpression(type, base, index, false);
        }
    } else if (dicttype != nullptr) {
        if (not index->type->is_equivalent(TYPE_STRING)) {
            error(2064, expr->index->token, "index must be a string");
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
            error(2066, expr->index->token, "index must be a number");
        }
        const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(base);
        if (ref != nullptr) {
            return new StringReferenceIndexExpression(ref, index);
        } else {
            return new StringValueIndexExpression(base, index);
        }
    } else {
        error(2068, expr->token, "not an array or dictionary");
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
                error(2218, x.first->token, "invalid format specification");
            }
        }
        const Expression *str;
        if (e->type->is_equivalent(TYPE_STRING)) {
            str = e;
        } else {
            auto to_string = e->type->methods.find("to_string");
            if (to_string == e->type->methods.end()) {
                error(2217, x.first->token, "no to_string() method found for type");
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
                error(2777, expr->token, "two arguments expected");
            }
            const Expression *lhs_expr = analyze(expr->args[0]);
            const ReferenceExpression *lhs = dynamic_cast<const ReferenceExpression *>(lhs_expr);
            if (lhs == nullptr) {
                error(2199, expr->args[0]->token, "expression is not assignable");
            }
            if (lhs_expr->is_readonly && dynamic_cast<const TypePointer *>(lhs_expr->type) == nullptr) {
                error(2200, expr->args[0]->token, "value_copy to readonly expression");
            }
            const Expression *rhs = analyze(expr->args[1]);
            const Type *ltype = lhs->type;
            const TypePointer *lptype = dynamic_cast<const TypePointer *>(ltype);
            if (lptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(lptype) == nullptr) {
                    error(2203, expr->args[0]->token, "valid pointer type required");
                }
                ltype = lptype->reftype;
                lhs = new PointerDereferenceExpression(ltype, lhs);
            }
            const Type *rtype = rhs->type;
            const TypePointer *rptype = dynamic_cast<const TypePointer *>(rtype);
            if (rptype != nullptr) {
                if (dynamic_cast<const TypeValidPointer *>(rptype) == nullptr) {
                    error(2204, expr->args[1]->token, "valid pointer type required");
                }
                rtype = rptype->reftype;
                rhs = new PointerDereferenceExpression(rtype, rhs);
            }
            if (not ltype->is_equivalent(rtype)) {
                error(2205, expr->args[1]->token, "type mismatch");
            }
            std::vector<const ReferenceExpression *> vars;
            vars.push_back(lhs);
            return new StatementExpression(new AssignmentStatement(expr->token.line, vars, rhs));
        }
        const TypeRecord *recordtype = dynamic_cast<const TypeRecord *>(scope.top()->lookupName(fname->name));
        if (recordtype != nullptr) {
            if (expr->args.size() != recordtype->fields.size()) {
                error(2214, expr->token, "wrong number of fields");
            }
            std::vector<const Expression *> elements;
            auto f = recordtype->fields.begin();
            for (auto x: expr->args) {
                const Expression *element = analyze(x);
                if (not element->type->is_equivalent(f->second)) {
                    error(2215, x->token, "type mismatch");
                }
                elements.push_back(element);
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
            auto m = base->type->methods.find(dotmethod->name);
            if (m == base->type->methods.end()) {
                error(2778, dotmethod->token, "method not found");
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
        error(2027, expr->base->token, "not a function");
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
            error(2167, a->token, "too many parameters");
        }
        if (ftype->params[p]->mode != ParameterType::IN) {
            const ReferenceExpression *ref = dynamic_cast<const ReferenceExpression *>(e);
            if (ref == nullptr) {
                error(2028, a->token, "function call argument must be reference: " + e->text());
            }
            if (ref->is_readonly) {
                error(2181, a->token, "readonly parameter to OUT");
            }
        }
        if (not e->type->is_equivalent(ftype->params[p]->type)) {
            error(2029, a->token, "type mismatch");
        }
        args.push_back(e);
        ++p;
    }
    if (p < ftype->params.size()) {
        error(2031, expr->token, "not enough arguments (got " + std::to_string(p) + ", expected " + std::to_string(ftype->params.size()));
    }
    return new FunctionCall(func, args);
}

const Expression *Analyzer::analyze(const pt::UnaryMinusExpression *expr)
{
    const Expression *atom = analyze(expr->expr);
    if (not atom->type->is_equivalent(TYPE_NUMBER)) {
        error(2033, expr->token, "number required for negation");
    }
    return new UnaryMinusExpression(atom);
}

const Expression *Analyzer::analyze(const pt::LogicalNotExpression *expr)
{
    const Expression *atom = analyze(expr->expr);
    if (not atom->type->is_equivalent(TYPE_BOOLEAN)) {
        error(2034, expr->token, "boolean required for logical not");
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
        error(2039, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::MultiplicationExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new MultiplicationExpression(left, right);
    } else {
        error(2040, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::DivisionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new DivisionExpression(left, right);
    } else {
        error(2041, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ModuloExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new ModuloExpression(left, right);
    } else {
        error(2042, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::AdditionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new AdditionExpression(left, right);
    } else if (left->type->is_equivalent(TYPE_STRING) && right->type->is_equivalent(TYPE_STRING)) {
        error(2206, expr->token, "type mismatch (use & to concatenate strings)");
    } else {
        error(2043, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::SubtractionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_NUMBER) && right->type->is_equivalent(TYPE_NUMBER)) {
        return new SubtractionExpression(left, right);
    } else {
        error(2044, expr->token, "type mismatch");
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
        error(2193, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ComparisonExpression *expr)
{
    const Expression *left = analyze(expr->left);
    ComparisonExpression::Comparison comp = (ComparisonExpression::Comparison)expr->comp; // TODO: remove cast
    const Expression *right = analyze(expr->right);
    if (not left->type->is_equivalent(right->type)) {
        error(2045, expr->token, "type mismatch");
    }
    if (left->type->is_equivalent(TYPE_BOOLEAN)) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(2046, expr->token, "comparison not available for Boolean");
        }
        return new BooleanComparisonExpression(left, right, comp);
    } else if (left->type->is_equivalent(TYPE_NUMBER)) {
        return new NumericComparisonExpression(left, right, comp);
    } else if (left->type->is_equivalent(TYPE_STRING)) {
        return new StringComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeArray *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(2047, expr->token, "comparison not available for Array");
        }
        return new ArrayComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeDictionary *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(2048, expr->token, "comparison not available for Dictionary");
        }
        return new DictionaryComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeRecord *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(2049, expr->token, "comparison not available for RECORD");
        }
        return new ArrayComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypeEnum *>(left->type) != nullptr) {
        return new NumericComparisonExpression(left, right, comp);
    } else if (dynamic_cast<const TypePointer *>(left->type) != nullptr) {
        if (comp != ComparisonExpression::EQ && comp != ComparisonExpression::NE) {
            error(2173, expr->token, "comparison not available for POINTER");
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
            error(2142, expr->token, "type mismatch");
        }
        return new ArrayInExpression(left, right);
    } else if (dicttype != nullptr) {
        if (not left->type->is_equivalent(TYPE_STRING)) {
            error(2143, expr->token, "type mismatch");
        }
        return new DictionaryInExpression(left, right);
    } else {
        error(2141, expr->left->token, "IN must be used with Array or Dictionary");
    }
}

const Expression *Analyzer::analyze(const pt::ConjunctionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
        return new ConjunctionExpression(left, right);
    } else {
        error(2051, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::DisjunctionExpression *expr)
{
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (left->type->is_equivalent(TYPE_BOOLEAN) && right->type->is_equivalent(TYPE_BOOLEAN)) {
        return new DisjunctionExpression(left, right);
    } else {
        error(2052, expr->token, "type mismatch");
    }
}

const Expression *Analyzer::analyze(const pt::ConditionalExpression *expr)
{
    const Expression *cond = analyze(expr->cond);
    const Expression *left = analyze(expr->left);
    const Expression *right = analyze(expr->right);
    if (not left->type->is_equivalent(right->type)) {
        error(2055, expr->left->token, "type of THEN and ELSE must match");
    }
    return new ConditionalExpression(cond, left, right);
}

const Expression *Analyzer::analyze(const pt::NewRecordExpression *expr)
{
    const TypeRecord *type = dynamic_cast<const TypeRecord *>(analyze(expr->type));
    if (type == nullptr) {
        error(2172, expr->type->token, "record type expected");
    }
    return new NewRecordExpression(type);
}

const Expression *Analyzer::analyze(const pt::ValidPointerExpression * /*expr*/)
{
    // TODO: This happens with multiple pointers in an IF VALID statement.
    internal_error("TODO pt::Expression");
}

const Statement *Analyzer::analyze(const pt::ImportDeclaration *declaration)
{
    if (scope.top()->lookupName(declaration->name) != nullptr) {
        error(2190, declaration->token, "name shadows outer");
    }
    rtl_import(scope.top(), declaration->name);
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::TypeDeclaration *declaration)
{
    std::string name = declaration->token.text;
    if (scope.top()->lookupName(name) != nullptr) {
        error(2019, declaration->token, "name shadows outer");
    }
    const Type *type = analyze(declaration->type);
    scope.top()->addName(name, const_cast<Type *>(type)); // Still ugly.
    const TypeRecord *rectype = dynamic_cast<const TypeRecord *>(type);
    if (rectype != nullptr) {
        scope.top()->resolveForward(name, rectype);
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::ConstantDeclaration *declaration)
{
    std::string name = declaration->name;
    if (scope.top()->lookupName(name) != nullptr) {
        error(2022, declaration->token, "name shadows outer");
    }
    const Type *type = analyze(declaration->type);
    const Expression *value = analyze(declaration->value);
    if (not value->type->is_equivalent(type)) {
        error(2025, declaration->value->token, "type mismatch");
    }
    if (not value->is_constant) {
        error(2026, declaration->value->token, "value must be constant");
    }
    scope.top()->addName(name, new Constant(name, value));
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::VariableDeclaration *declaration)
{
    const Type *type = analyze(declaration->type);
    std::vector<Variable *> variables;
    for (auto name: declaration->names) {
        if (scope.top()->lookupName(name) != nullptr) {
            error(2057, declaration->token, "name shadows outer");
        }
        Variable *v;
        if (scope.top() == global_scope) {
            v = new GlobalVariable(name, type, false);
        } else {
            v = new LocalVariable(name, type, scope.top(), false);
        }
        variables.push_back(v);
    }
    if (declaration->value != nullptr) {
        std::vector<const ReferenceExpression *> refs;
        const Expression *expr = analyze(declaration->value);
        if (not expr->type->is_equivalent(type)) {
            error(2189, declaration->value->token, "type mismatch");
        }
        for (auto v: variables) {
            scope.top()->addName(v->name, v, true);
            refs.push_back(new VariableExpression(v));
        }
        return new AssignmentStatement(declaration->token.line, refs, expr);
    } else {
        for (auto v: variables) {
            scope.top()->addName(v->name, v);
        }
        return new NullStatement(declaration->token.line);
    }
}

const Statement *Analyzer::analyze_decl(const pt::FunctionDeclaration *declaration)
{
    const std::string classtype = declaration->type;
    Type *type = nullptr;
    if (not classtype.empty()) {
        Name *tname = scope.top()->lookupName(classtype);
        if (tname == nullptr) {
            error(2209, declaration->token, "type name not found");
        }
        type = dynamic_cast<Type *>(tname);
        if (type == nullptr) {
            error(2210, declaration->token, "type name is not a type");
        }
    }
    std::string name = declaration->name;
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    Scope *newscope = new Scope(scope.top());
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
                error(2211, x->token, "expected self parameter");
            }
        }
        FunctionParameter *fp = new FunctionParameter(x->name, ptype, mode, newscope);
        args.push_back(fp);
        newscope->addName(x->name, fp, true);
    }
    if (type != nullptr && args.empty()) {
        error(2212, declaration->token, "expected self parameter");
    }
    Function *function;
    if (type != nullptr) {
        auto f = type->methods.find(name);
        if (f != type->methods.end()) {
            function = dynamic_cast<Function *>(f->second);
            newscope = function->scope;
        } else {
            function = new Function(name, returntype, newscope, args);
            type->methods[name] = function;
        }
    } else {
        Name *ident = scope.top()->lookupName(name);
        function = dynamic_cast<Function *>(ident);
        if (function != nullptr) {
            newscope = function->scope;
        } else if (ident != nullptr) {
            error(2074, declaration->token, "name shadows outer");
        } else {
            function = new Function(name, returntype, newscope, args);
            scope.top()->addName(name, function);
        }
    }
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze_body(const pt::FunctionDeclaration *declaration)
{
    const std::string classtype = declaration->type;
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
        auto f = type->methods.find(declaration->name);
        function = dynamic_cast<Function *>(f->second);
    } else {
        function = dynamic_cast<Function *>(scope.top()->lookupName(declaration->name));
    }
    Scope *newscope = function->scope;
    scope.push(newscope);
    functiontypes.push(dynamic_cast<const TypeFunction *>(function->type));
    loops.push(std::list<std::pair<TokenType, unsigned int>>());
    function->statements = analyze(declaration->body);
    const Type *returntype = dynamic_cast<const TypeFunction *>(function->type)->returntype;
    if (returntype != TYPE_NOTHING) {
        if (function->statements.empty() || not function->statements.back()->always_returns()) {
            error(2146, declaration->token, "missing RETURN statement");
        }
    }
    loops.pop();
    functiontypes.pop();
    newscope->checkForward();
    scope.pop();
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::ExternalFunctionDeclaration *declaration)
{
    std::string name = declaration->name;
    if (scope.top()->lookupName(name) != nullptr) {
        error(2163, declaration->token, "name shadows outer");
    }
    const Type *returntype = declaration->returntype != nullptr ? analyze(declaration->returntype) : TYPE_NOTHING;
    Scope *newscope = new Scope(scope.top());
    std::vector<FunctionParameter *> args;
    for (auto x: declaration->args) {
        ParameterType::Mode mode = ParameterType::IN;
        switch (x->mode) {
            case pt::FunctionParameter::IN:    mode = ParameterType::IN;       break;
            case pt::FunctionParameter::INOUT: mode = ParameterType::INOUT;    break;
            case pt::FunctionParameter::OUT:   mode = ParameterType::OUT;      break;
        }
        const Type *ptype = analyze(x->type);
        FunctionParameter *fp = new FunctionParameter(x->name, ptype, mode, newscope);
        args.push_back(fp);
        newscope->addName(x->name, fp, true);
    }
    const DictionaryLiteralExpression *dict = dynamic_cast<const DictionaryLiteralExpression *>(analyze(declaration->dict));
    if (not dict->is_constant) {
        error(2124, declaration->dict->token, "constant dictionary expected");
    }
    if (dynamic_cast<const TypeDictionary *>(dict->elementtype) == nullptr) {
        error(2131, declaration->dict->token, "top level dictionary element not dictionary");
    }
    for (auto elem: dict->dict) {
        auto *d = dynamic_cast<const DictionaryLiteralExpression *>(elem.second);
        if (not d->dict.empty() && not d->elementtype->is_equivalent(TYPE_STRING)) {
            error(2132, declaration->dict->token, "sub level dictionary must have string elements");
        }
    }

    auto klibrary = dict->dict.find("library");
    if (klibrary == dict->dict.end()) {
        error(2133, declaration->dict->token, "\"library\" key not found");
    }
    auto &library_dict = dynamic_cast<const DictionaryLiteralExpression *>(klibrary->second)->dict;
    auto kname = library_dict.find("name");
    if (kname == library_dict.end()) {
        error(2134, declaration->dict->token, "\"name\" key not found in library");
    }
    std::string library_name = kname->second->eval_string();

    auto ktypes = dict->dict.find("types");
    if (ktypes == dict->dict.end()) {
        error(2135, declaration->dict->token, "\"types\" key not found");
    }
    auto &types_dict = dynamic_cast<const DictionaryLiteralExpression *>(ktypes->second)->dict;
    std::map<std::string, std::string> param_types;
    for (auto paramtype: types_dict) {
        param_types[paramtype.first] = paramtype.second->eval_string();
    }
    for (auto a: args) {
        if (param_types.find(a->name) == param_types.end()) {
            error(2168, declaration->dict->token, "parameter type missing: " + a->name);
        }
    }

    ExternalFunction *function = new ExternalFunction(name, returntype, newscope, args, library_name, param_types);
    scope.top()->addName(name, function);
    return new NullStatement(declaration->token.line);
}

const Statement *Analyzer::analyze(const pt::ExceptionDeclaration *declaration)
{
    std::string name = declaration->name;
    if (scope.top()->lookupName(name) != nullptr) {
        error(2191, declaration->token, "name shadows outer");
    }
    scope.top()->addName(name, new Exception(name));
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
        error(2095, statement->variables[0]->token, "expression is not assignable");
    }
    if (expr->is_readonly) {
        error(2180, statement->variables[0]->token, "assignment to readonly expression");
    }
    const Expression *rhs = analyze(statement->expr);
    if (not rhs->type->is_equivalent(expr->type)) {
        error(2094, statement->token, "type mismatch");
    }
    std::vector<const ReferenceExpression *> vars;
    vars.push_back(ref);
    return new AssignmentStatement(statement->token.line, vars, rhs);
}

const Statement *Analyzer::analyze(const pt::CaseStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (not expr->type->is_equivalent(TYPE_NUMBER) && not expr->type->is_equivalent(TYPE_STRING) && dynamic_cast<const TypeEnum *>(expr->type) == nullptr) {
        error(2084, statement->expr->token, "CASE expression must be Number, String, or ENUM");
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
                    error(2085, cwc->expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(2086, cwc->expr->token, "WHEN condition must be constant");
                }
                ComparisonExpression::Comparison comp = (ComparisonExpression::Comparison)cwc->comp; // TODO: remove cast
                const CaseStatement::WhenCondition *cond = new CaseStatement::ComparisonWhenCondition(comp, when);
                for (auto clause: clauses) {
                    for (auto c: clause.first) {
                        if (cond->overlaps(c)) {
                            error(2106, cwc->token, "overlapping case condition");
                        }
                    }
                }
                for (auto c: conditions) {
                    if (cond->overlaps(c)) {
                        error(2107, cwc->token, "overlapping case condition");
                    }
                }
                conditions.push_back(cond);
            }
            if (rwc != nullptr) {
                const Expression *when = analyze(rwc->low_expr);
                if (not when->type->is_equivalent(expr->type)) {
                    error(2087, rwc->low_expr->token, "type mismatch");
                }
                if (not when->is_constant) {
                    error(2088, rwc->low_expr->token, "WHEN condition must be constant");
                }
                const Expression *when2 = analyze(rwc->high_expr);
                if (not when2->type->is_equivalent(expr->type)) {
                    error(2089, rwc->high_expr->token, "type mismatch");
                }
                if (not when2->is_constant) {
                    error(2090, rwc->high_expr->token, "WHEN condition must be constant");
                }
                if (when->type->is_equivalent(TYPE_NUMBER) || dynamic_cast<const TypeEnum *>(when->type) != nullptr) {
                    if (number_is_greater(when->eval_number(), when2->eval_number())) {
                        error(2184, rwc->high_expr->token, "WHEN numeric range condition must be low..high");
                    }
                } else if (when->type->is_equivalent(TYPE_STRING)) {
                    if (when->eval_string() > when2->eval_string()) {
                        error(2185, rwc->high_expr->token, "WHEN string range condition must be low..high");
                    }
                } else {
                    internal_error("range condition type");
                }
                const CaseStatement::WhenCondition *cond = new CaseStatement::RangeWhenCondition(when, when2);
                for (auto clause: clauses) {
                    for (auto c: clause.first) {
                        if (cond->overlaps(c)) {
                            error(2108, rwc->token, "overlapping case condition");
                        }
                    }
                }
                for (auto c: conditions) {
                    if (cond->overlaps(c)) {
                        error(2109, rwc->token, "overlapping case condition");
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
            error(2182, statement->token, "EXIT FUNCTION not allowed outside function");
        } else if (functiontypes.top()->returntype != TYPE_NOTHING) {
            error(2183, statement->token, "function must return a value");
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
    error(2137, statement->token, "no matching loop found in current scope");
}

const Statement *Analyzer::analyze(const pt::ExpressionStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (expr->type == TYPE_NOTHING) {
        return new ExpressionStatement(statement->token.line, analyze(statement->expr));
    }
    if (dynamic_cast<const ComparisonExpression *>(expr) != nullptr) {
        error(2097, statement->token, "':=' expected");
    }
    if (dynamic_cast<const FunctionCall *>(expr) != nullptr) {
        error(2096, statement->token, "return value unused");
    }
    error(2098, statement->token, "Unexpected");
}

const Statement *Analyzer::analyze(const pt::ForStatement *statement)
{
    Name *name = scope.top()->lookupName(statement->var);
    Variable *var = dynamic_cast<Variable *>(name);
    if (var == nullptr) {
        error(2195, statement->token, "name not a variable: " + statement->var);
    }
    if (not var->type->is_equivalent(TYPE_NUMBER)) {
        error(2112, statement->token, "type mismatch");
    }
    if (var->is_readonly) {
        error(2207, statement->token, "cannot use readonly variable in FOR loop");
    }
    var->is_readonly = true;
    const Expression *start = analyze(statement->start);
    if (not start->type->is_equivalent(TYPE_NUMBER)) {
        error(2113, statement->start->token, "numeric expression expected");
    }
    const Expression *end = analyze(statement->end);
    if (not end->type->is_equivalent(TYPE_NUMBER)) {
        error(2115, statement->end->token, "numeric expression expected");
    }
    const Expression *step = nullptr;
    if (statement->step != nullptr) {
        step = analyze(statement->step);
        if (step->type != TYPE_NUMBER) {
            error(2116, statement->step->token, "numeric expression expected");
        }
        if (not step->is_constant) {
            error(2117, statement->step->token, "numeric expression must be constant");
        }
        if (number_is_zero(step->eval_number())) {
            error(2161, statement->step->token, "STEP value cannot be zero");
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
            if (scope.top()->lookupName(valid->name) != nullptr) {
                error(2177, valid->token, "name shadows outer");
            }
            const Expression *ptr = analyze(valid->ptr);
            const TypePointer *ptrtype = dynamic_cast<const TypePointer *>(ptr->type);
            if (ptrtype == nullptr) {
                error(2176, valid->ptr->token, "pointer type expression expected");
            }
            const TypeValidPointer *vtype = new TypeValidPointer(ptrtype);
            Variable *v;
            // TODO: Try to make this a local variable always (give the global scope a local space).
            if (functiontypes.empty()) {
                v = new GlobalVariable(valid->name, vtype, true);
            } else {
                v = new LocalVariable(valid->name, vtype, scope.top(), true);
            }
            scope.top()->addName(valid->name, v, true);
            cond = new ValidPointerExpression(v, ptr);
        } else {
            cond = analyze(c.first);
            if (not cond->type->is_equivalent(TYPE_BOOLEAN)) {
                error(2078, c.first->token, "boolean value expected");
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
    error(2145, statement->token, "no matching loop found in current scope");
}

const Statement *Analyzer::analyze(const pt::RaiseStatement *statement)
{
    const Name *name = scope.top()->lookupName(statement->name);
    if (name == nullptr) {
        error(2157, statement->token, "exception not found: " + statement->name);
    }
    const Exception *exception = dynamic_cast<const Exception *>(name);
    if (exception == nullptr) {
        error(2158, statement->token, "name not an exception");
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
        error(2150, statement->cond->token, "boolean value expected");
    }
    loops.top().pop_back();
    return new RepeatStatement(statement->token.line, loop_id, cond, statements);
}

const Statement *Analyzer::analyze(const pt::ReturnStatement *statement)
{
    const Expression *expr = analyze(statement->expr);
    if (functiontypes.empty()) {
        error(2164, statement->token, "RETURN not allowed outside function");
    } else if (functiontypes.top()->returntype == TYPE_NOTHING) {
        error(2165, statement->token, "function does not return a value");
    } else if (not expr->type->is_equivalent(functiontypes.top()->returntype)) {
        error(2166, statement->token, "type mismatch in RETURN");
    }
    return new ReturnStatement(statement->token.line, expr);
}

const Statement *Analyzer::analyze(const pt::TryStatement *statement)
{
    std::vector<const Statement *> statements = analyze(statement->body);
    std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;
    for (auto x: statement->catches) {
        const Name *name = scope.top()->lookupName(x.first.at(0));
        if (name == nullptr) {
            error(2154, statement->token, "exception not found: " + x.first.at(0));
        }
        const Exception *exception = dynamic_cast<const Exception *>(name);
        if (exception == nullptr) {
            error(2155, statement->token, "name not an exception");
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
        error(2081, statement->cond->token, "boolean value expected");
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
    global_scope = r->scope;
    scope.push(r->scope);
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
