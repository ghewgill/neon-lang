#ifndef PT_H
#define PT_H

#include <string>
#include <vector>

#include "number.h"
#include "token.h"

namespace pt {

class TypeSimple;
class TypeEnum;
class TypeRecord;
class TypePointer;
class TypeParameterised;

class BooleanLiteralExpression;
class NumberLiteralExpression;
class StringLiteralExpression;
class ArrayLiteralExpression;
class DictionaryLiteralExpression;
class NilLiteralExpression;
class IdentifierExpression;
class DotExpression;
class ArrowExpression;
class SubscriptExpression;
class InterpolatedStringExpression;
class FunctionCallExpression;
class UnaryPlusExpression;
class UnaryMinusExpression;
class LogicalNotExpression;
class ExponentiationExpression;
class MultiplicationExpression;
class DivisionExpression;
class ModuloExpression;
class AdditionExpression;
class SubtractionExpression;
class ConcatenationExpression;
class ComparisonExpression;
class ChainedComparisonExpression;
class MembershipExpression;
class ConjunctionExpression;
class DisjunctionExpression;
class ConditionalExpression;
class NewRecordExpression;
class ValidPointerExpression;
class RangeSubscriptExpression;

class ImportDeclaration;
class TypeDeclaration;
class ConstantDeclaration;
class VariableDeclaration;
class LetDeclaration;
class FunctionDeclaration;
class ExternalFunctionDeclaration;
class ExceptionDeclaration;

class AssignmentStatement;
class CaseStatement;
class ExitStatement;
class ExpressionStatement;
class ForStatement;
class IfStatement;
class LoopStatement;
class NextStatement;
class RaiseStatement;
class RepeatStatement;
class ReturnStatement;
class TryStatement;
class WhileStatement;

class Program;

class IParseTreeVisitor {
public:
    virtual ~IParseTreeVisitor() {}

    virtual void visit(const TypeSimple *) = 0;
    virtual void visit(const TypeEnum *) = 0;
    virtual void visit(const TypeRecord *) = 0;
    virtual void visit(const TypePointer *) = 0;
    virtual void visit(const TypeParameterised *) = 0;

    virtual void visit(const BooleanLiteralExpression *) = 0;
    virtual void visit(const NumberLiteralExpression *) = 0;
    virtual void visit(const StringLiteralExpression *) = 0;
    virtual void visit(const ArrayLiteralExpression *) = 0;
    virtual void visit(const DictionaryLiteralExpression *) = 0;
    virtual void visit(const NilLiteralExpression *) = 0;
    virtual void visit(const IdentifierExpression *) = 0;
    virtual void visit(const DotExpression *) = 0;
    virtual void visit(const ArrowExpression *) = 0;
    virtual void visit(const SubscriptExpression *) = 0;
    virtual void visit(const InterpolatedStringExpression *) = 0;
    virtual void visit(const FunctionCallExpression *) = 0;
    virtual void visit(const UnaryPlusExpression *) = 0;
    virtual void visit(const UnaryMinusExpression *) = 0;
    virtual void visit(const LogicalNotExpression *) = 0;
    virtual void visit(const ExponentiationExpression *) = 0;
    virtual void visit(const MultiplicationExpression *) = 0;
    virtual void visit(const DivisionExpression *) = 0;
    virtual void visit(const ModuloExpression *) = 0;
    virtual void visit(const AdditionExpression *) = 0;
    virtual void visit(const SubtractionExpression *) = 0;
    virtual void visit(const ConcatenationExpression *) = 0;
    virtual void visit(const ComparisonExpression *) = 0;
    virtual void visit(const ChainedComparisonExpression *) = 0;
    virtual void visit(const MembershipExpression *) = 0;
    virtual void visit(const ConjunctionExpression *) = 0;
    virtual void visit(const DisjunctionExpression *) = 0;
    virtual void visit(const ConditionalExpression *) = 0;
    virtual void visit(const NewRecordExpression *) = 0;
    virtual void visit(const ValidPointerExpression *) = 0;
    virtual void visit(const RangeSubscriptExpression *) = 0;

    virtual void visit(const ImportDeclaration *) = 0;
    virtual void visit(const TypeDeclaration *) = 0;
    virtual void visit(const ConstantDeclaration *) = 0;
    virtual void visit(const VariableDeclaration *) = 0;
    virtual void visit(const LetDeclaration *) = 0;
    virtual void visit(const FunctionDeclaration *) = 0;
    virtual void visit(const ExternalFunctionDeclaration *) = 0;
    virtual void visit(const ExceptionDeclaration *) = 0;

    virtual void visit(const AssignmentStatement *) = 0;
    virtual void visit(const CaseStatement *) = 0;
    virtual void visit(const ExitStatement *) = 0;
    virtual void visit(const ExpressionStatement *) = 0;
    virtual void visit(const ForStatement *) = 0;
    virtual void visit(const IfStatement *) = 0;
    virtual void visit(const LoopStatement *) = 0;
    virtual void visit(const NextStatement *) = 0;
    virtual void visit(const RaiseStatement *) = 0;
    virtual void visit(const RepeatStatement *) = 0;
    virtual void visit(const ReturnStatement *) = 0;
    virtual void visit(const TryStatement *) = 0;
    virtual void visit(const WhileStatement *) = 0;
    virtual void visit(const Program *) = 0;
};

class ParseTreeNode {
public:
    ParseTreeNode(const Token &token): token(token) {}
    virtual ~ParseTreeNode() {}
    virtual void accept(IParseTreeVisitor *visitor) const = 0;
    const Token token;
private:
    ParseTreeNode(const ParseTreeNode &);
    ParseTreeNode &operator=(const ParseTreeNode &);
};

class Type: public ParseTreeNode {
public:
    Type(const Token &token): ParseTreeNode(token) {}
};

class TypeSimple: public Type {
public:
    TypeSimple(const Token &token, const std::string &name): Type(token), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string name;
};

class TypeEnum: public Type {
public:
    TypeEnum(const Token &token, const std::vector<std::pair<Token, int>> &names): Type(token), names(names) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<Token, int>> names;
};

class TypeRecord: public Type {
public:
    TypeRecord(const Token &token, const std::vector<std::pair<Token, const Type *>> &fields): Type(token), fields(fields) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<Token, const Type *>> fields;
};

class TypePointer: public Type {
public:
    TypePointer(const Token &token, const Type *reftype): Type(token), reftype(reftype) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Type *const reftype;
private:
    TypePointer(const TypePointer &);
    TypePointer &operator=(const TypePointer &);
};

class TypeParameterised: public Type {
public:
    TypeParameterised(const Token &name, const Type *elementtype): Type(name), name(token), elementtype(elementtype) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    const Type *const elementtype;
private:
    TypeParameterised(const TypeParameterised &);
    TypeParameterised &operator=(const TypeParameterised &);
};

class Expression: public ParseTreeNode {
public:
    Expression(const Token &token): ParseTreeNode(token) {}
};

class BooleanLiteralExpression: public Expression {
public:
    BooleanLiteralExpression(const Token &token, bool value): Expression(token), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const bool value;
};

class NumberLiteralExpression: public Expression {
public:
    NumberLiteralExpression(const Token &token, Number value): Expression(token), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Number value;
};

class StringLiteralExpression: public Expression {
public:
    StringLiteralExpression(const Token &token, const std::string &value): Expression(token), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string value;
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Token &token, const std::vector<const Expression *> &elements): Expression(token), elements(elements) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const Expression *> elements;
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const Token &token, const std::vector<std::pair<Token, const Expression *>> &elements): Expression(token), elements(elements) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<Token, const Expression *>> elements;
};

class NilLiteralExpression: public Expression {
public:
    NilLiteralExpression(const Token &token): Expression(token) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class IdentifierExpression: public Expression {
public:
    IdentifierExpression(const Token &token, const std::string &name): Expression(token), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string name;
};

class DotExpression: public Expression {
public:
    DotExpression(const Token &token, const Expression *base, const Token &name): Expression(token), base(base), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const Token name;
private:
    DotExpression(const DotExpression &);
    DotExpression &operator=(const DotExpression &);
};

class ArrowExpression: public Expression {
public:
    ArrowExpression(const Token &token, const Expression *base, const Token &name): Expression(token), base(base), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const Token name;
private:
    ArrowExpression(const ArrowExpression &);
    ArrowExpression &operator=(const ArrowExpression &);
};

class SubscriptExpression: public Expression {
public:
    SubscriptExpression(const Token &token, const Expression *base, const Expression *index): Expression(token), base(base), index(index) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const Expression *const index;
private:
    SubscriptExpression(const SubscriptExpression &);
    SubscriptExpression &operator=(const SubscriptExpression &);
};

class InterpolatedStringExpression: public Expression {
public:
    InterpolatedStringExpression(const Token &token, const std::vector<std::pair<const Expression *, std::string>> &parts): Expression(token), parts(parts) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<const Expression *, std::string>> parts;
};

class FunctionCallExpression: public Expression {
public:
    FunctionCallExpression(const Token &token, const Expression *base, const std::vector<std::pair<Token, const Expression *>> &args): Expression(token), base(base), args(args) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const std::vector<std::pair<Token, const Expression *>> args;
private:
    FunctionCallExpression(const FunctionCallExpression &);
    FunctionCallExpression &operator=(const FunctionCallExpression &);
};

class UnaryExpression: public Expression {
public:
    UnaryExpression(const Token &token, const Expression *expr): Expression(token), expr(expr) {}
    const Expression *const expr;
private:
    UnaryExpression(const UnaryExpression &);
    UnaryExpression &operator=(const UnaryExpression &);
};

class UnaryPlusExpression: public UnaryExpression {
public:
    UnaryPlusExpression(const Token &token, const Expression *expr): UnaryExpression(token, expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class UnaryMinusExpression: public UnaryExpression {
public:
    UnaryMinusExpression(const Token &token, const Expression *expr): UnaryExpression(token, expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class LogicalNotExpression: public UnaryExpression {
public:
    LogicalNotExpression(const Token &token, const Expression *expr): UnaryExpression(token, expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class BinaryExpression: public Expression {
public:
    BinaryExpression(const Token &token, const Expression *left, const Expression *right): Expression(token), left(left), right(right) {}
    const Expression *const left;
    const Expression *const right;
private:
    BinaryExpression(const BinaryExpression &);
    BinaryExpression &operator=(const BinaryExpression &);
};

class ExponentiationExpression: public BinaryExpression {
public:
    ExponentiationExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class MultiplicationExpression: public BinaryExpression {
public:
    MultiplicationExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class DivisionExpression: public BinaryExpression {
public:
    DivisionExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ModuloExpression: public BinaryExpression {
public:
    ModuloExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class AdditionExpression: public BinaryExpression {
public:
    AdditionExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class SubtractionExpression: public BinaryExpression {
public:
    SubtractionExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ConcatenationExpression: public BinaryExpression {
public:
    ConcatenationExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ComparisonExpression: public BinaryExpression {
public:
    enum Comparison {
        EQ, NE, LT, GT, LE, GE
    };
    ComparisonExpression(const Token &token, const Expression *left, const Expression *right, Comparison comp): BinaryExpression(token, left, right), comp(comp) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Comparison comp;
};

class ChainedComparisonExpression: public Expression {
public:
    ChainedComparisonExpression(const std::vector<const ComparisonExpression *> &comps): Expression(comps[0]->token), comps(comps) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const ComparisonExpression *> comps;
};

class MembershipExpression: public BinaryExpression {
public:
    MembershipExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ConjunctionExpression: public BinaryExpression {
public:
    ConjunctionExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class DisjunctionExpression: public BinaryExpression {
public:
    DisjunctionExpression(const Token &token, const Expression *left, const Expression *right): BinaryExpression(token, left, right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ConditionalExpression: public Expression {
public:
    ConditionalExpression(const Token &token, const Expression *cond, const Expression *left, const Expression *right): Expression(token), cond(cond), left(left), right(right) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const cond;
    const Expression *const left;
    const Expression *const right;
private:
    ConditionalExpression(const ConditionalExpression &);
    ConditionalExpression &operator=(const ConditionalExpression &);
};

class NewRecordExpression: public Expression {
public:
    NewRecordExpression(const Token &token, const Type *type): Expression(token), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Type *const type;
private:
    NewRecordExpression(const NewRecordExpression &);
    NewRecordExpression &operator=(const NewRecordExpression &);
};

class ValidPointerExpression: public Expression {
public:
    ValidPointerExpression(const Token &token, std::vector<std::pair<Token, const Expression *>> &tests): Expression(token), tests(tests) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::pair<Token, const Expression *>> tests;
private:
    ValidPointerExpression(const ValidPointerExpression &);
    ValidPointerExpression &operator=(const ValidPointerExpression &);
};

class ArrayRange {
public:
    ArrayRange(const Token &token, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end): token(token), first(first), first_from_end(first_from_end), last(last), last_from_end(last_from_end) {}
    const Token token;
    const Expression *const first;
    const bool first_from_end;
    const Expression *const last;
    const bool last_from_end;
private:
    ArrayRange(const ArrayRange &);
    ArrayRange &operator=(const ArrayRange &);
};

class RangeSubscriptExpression: public Expression {
public:
    RangeSubscriptExpression(const Token &token, const Expression *base, const ArrayRange *range): Expression(token), base(base), range(range) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const ArrayRange *const range;
private:
    RangeSubscriptExpression(const RangeSubscriptExpression &);
    RangeSubscriptExpression &operator=(const RangeSubscriptExpression &);
};

class FunctionParameter {
public:
    enum Mode {
        IN,
        INOUT,
        OUT
    };
    FunctionParameter(const Token &token, const Token &name, const Type *type, Mode mode, const Expression *default_value): token(token), name(name), type(type), mode(mode), default_value(default_value) {}
    const Token token;
    const Token name;
    const Type *const type;
    const Mode mode;
    const Expression *default_value;
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class Statement: public ParseTreeNode {
public:
    Statement(const Token &token): ParseTreeNode(token) {}
};

class Declaration: public Statement {
public:
    Declaration(const Token &token): Statement(token) {}
};

class ImportDeclaration: public Declaration {
public:
    ImportDeclaration(const Token &token, const Token &name): Declaration(token), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
};

class TypeDeclaration: public Declaration {
public:
    TypeDeclaration(const Token &token, const Type *type): Declaration(token), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Type *const type;
private:
    TypeDeclaration(const TypeDeclaration &);
    TypeDeclaration &operator=(const TypeDeclaration &);
};

class ConstantDeclaration: public Declaration {
public:
    ConstantDeclaration(const Token &token, const Token &name, const Type *type, const Expression *value): Declaration(token), name(name), type(type), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    const Type *const type;
    const Expression *const value;
private:
    ConstantDeclaration(const ConstantDeclaration &);
    ConstantDeclaration &operator=(const ConstantDeclaration &);
};

class VariableDeclaration: public Declaration {
public:
    VariableDeclaration(const Token &token, const std::vector<Token> &names, const Type *type, const Expression *value): Declaration(token), names(names), type(type), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<Token> names;
    const Type *const type;
    const Expression *const value;
private:
    VariableDeclaration(const VariableDeclaration &);
    VariableDeclaration &operator=(const VariableDeclaration &);
};

class LetDeclaration: public Declaration {
public:
    LetDeclaration(const Token &token, const Token &name, const Type *type, const Expression *value): Declaration(token), name(name), type(type), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    const Type *const type;
    const Expression *const value;
private:
    LetDeclaration(const LetDeclaration &);
    LetDeclaration &operator=(const LetDeclaration &);
};

class BaseFunctionDeclaration: public Declaration {
public:
    BaseFunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args): Declaration(token), type(type), name(name), returntype(returntype), args(args) {}
    const Token type;
    const Token name;
    const Type *const returntype;
    const std::vector<const FunctionParameter *> args;
private:
    BaseFunctionDeclaration(const BaseFunctionDeclaration &);
    BaseFunctionDeclaration &operator=(const BaseFunctionDeclaration &);
};

class FunctionDeclaration: public BaseFunctionDeclaration {
public:
    FunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args, const std::vector<const Statement *> &body): BaseFunctionDeclaration(token, type, name, returntype, args), body(body) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const Statement *> body;
};

class ExternalFunctionDeclaration: public BaseFunctionDeclaration {
public:
    ExternalFunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args, const Expression *dict): BaseFunctionDeclaration(token, type, name, returntype, args), dict(dict) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const dict;
private:
    ExternalFunctionDeclaration(const ExternalFunctionDeclaration &);
    ExternalFunctionDeclaration &operator=(const ExternalFunctionDeclaration &);
};

class ExceptionDeclaration: public Declaration {
public:
    ExceptionDeclaration(const Token &token, const Token &name): Declaration(token), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
};

class BlockStatement: public Statement {
public:
    BlockStatement(const Token &token, const std::vector<const Statement *> &body): Statement(token), body(body) {}
    const std::vector<const Statement *> body;
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(const Token &token, const std::vector<const Expression *> &variables, const Expression *expr): Statement(token), variables(variables), expr(expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const Expression *> variables;
    const Expression *const expr;
private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        WhenCondition(const Token &token): token(token) {}
        virtual ~WhenCondition() {}
        const Token token;
    private:
        WhenCondition(const WhenCondition &);
        WhenCondition &operator=(const WhenCondition &);
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(const Token &token, ComparisonExpression::Comparison comp, const Expression *expr): WhenCondition(token), comp(comp), expr(expr) {}
        const ComparisonExpression::Comparison comp;
        const Expression *const expr;
    private:
        ComparisonWhenCondition(const ComparisonWhenCondition &);
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &);
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Token &token, const Expression *low_expr, const Expression *high_expr): WhenCondition(token), low_expr(low_expr), high_expr(high_expr) {}
        const Expression *const low_expr;
        const Expression *const high_expr;
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(const Token &token, const Expression *expr, const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> &clauses): Statement(token), expr(expr), clauses(clauses) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const expr;
    const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;
private:
    CaseStatement(const CaseStatement &);
    CaseStatement &operator=(const CaseStatement &);
};

class ExitStatement: public Statement {
public:
    ExitStatement(const Token &token, TokenType type): Statement(token), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const TokenType type;
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(const Token &token, const Expression *expr): Statement(token), expr(expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const expr;
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class ForStatement: public BlockStatement {
public:
    ForStatement(const Token &token, const Token &var, const Expression *start, const Expression *end, const Expression *step, const std::vector<const Statement *> &body): BlockStatement(token, body), var(var), start(start), end(end), step(step) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token var;
    const Expression *const start;
    const Expression *const end;
    const Expression *const step;
private:
    ForStatement(const ForStatement &);
    ForStatement &operator=(const ForStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(const Token &token, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(token), condition_statements(condition_statements), else_statements(else_statements) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;
};

class LoopStatement: public BlockStatement {
public:
    LoopStatement(const Token &token, const std::vector<const Statement *> &body): BlockStatement(token, body) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class NextStatement: public Statement {
public:
    NextStatement(const Token &token, TokenType type): Statement(token), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const TokenType type;
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(const Token &token, const Token &name, const Expression *info): Statement(token), name(name), info(info) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    const Expression *const info;
private:
    RaiseStatement(const RaiseStatement &);
    RaiseStatement &operator=(const RaiseStatement &);
};

class RepeatStatement: public BlockStatement {
public:
    RepeatStatement(const Token &token, const Expression *cond, const std::vector<const Statement *> &body): BlockStatement(token, body), cond(cond) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const cond;
private:
    RepeatStatement(const RepeatStatement &);
    RepeatStatement &operator=(const RepeatStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(const Token &token, const Expression *expr): Statement(token), expr(expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const expr;
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class TryStatement: public BlockStatement {
public:
    TryStatement(const Token &token, const std::vector<const Statement *> &body, const std::vector<std::pair<std::vector<Token>, std::vector<const Statement *>>> &catches): BlockStatement(token, body), catches(catches) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<std::vector<Token>, std::vector<const Statement *>>> catches;
};

class WhileStatement: public BlockStatement {
public:
    WhileStatement(const Token &token, const Expression *cond, const std::vector<const Statement *> &body): BlockStatement(token, body), cond(cond) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const cond;
private:
    WhileStatement(const WhileStatement &);
    WhileStatement &operator=(const WhileStatement &);
};

class Program: public BlockStatement {
public:
    Program(const Token &token, const std::vector<const Statement *> &body): BlockStatement(token, body) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

} // namespace pt

#endif
