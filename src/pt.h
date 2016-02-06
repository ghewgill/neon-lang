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
class TypeFunctionPointer;
class TypeParameterised;
class TypeImport;

class DummyExpression;
class IdentityExpression;
class BooleanLiteralExpression;
class NumberLiteralExpression;
class StringLiteralExpression;
class FileLiteralExpression;
class BytesLiteralExpression;
class ArrayLiteralExpression;
class ArrayLiteralRangeExpression;
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
class NativeFunctionDeclaration;
class ExceptionDeclaration;
class ExportDeclaration;
class MainBlock;

class AssertStatement;
class AssignmentStatement;
class CaseStatement;
class CheckStatement;
class ExitStatement;
class ExpressionStatement;
class ForStatement;
class ForeachStatement;
class IfStatement;
class IncrementStatement;
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
    virtual void visit(const TypeFunctionPointer *) = 0;
    virtual void visit(const TypeParameterised *) = 0;
    virtual void visit(const TypeImport *) = 0;

    virtual void visit(const DummyExpression *) = 0;
    virtual void visit(const IdentityExpression *) = 0;
    virtual void visit(const BooleanLiteralExpression *) = 0;
    virtual void visit(const NumberLiteralExpression *) = 0;
    virtual void visit(const StringLiteralExpression *) = 0;
    virtual void visit(const FileLiteralExpression *) = 0;
    virtual void visit(const BytesLiteralExpression *) = 0;
    virtual void visit(const ArrayLiteralExpression *) = 0;
    virtual void visit(const ArrayLiteralRangeExpression *) = 0;
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
    virtual void visit(const NativeFunctionDeclaration *) = 0;
    virtual void visit(const ExceptionDeclaration *) = 0;
    virtual void visit(const ExportDeclaration *) = 0;
    virtual void visit(const MainBlock *) = 0;

    virtual void visit(const AssertStatement *) = 0;
    virtual void visit(const AssignmentStatement *) = 0;
    virtual void visit(const CaseStatement *) = 0;
    virtual void visit(const CheckStatement *) = 0;
    virtual void visit(const ExitStatement *) = 0;
    virtual void visit(const ExpressionStatement *) = 0;
    virtual void visit(const ForStatement *) = 0;
    virtual void visit(const ForeachStatement *) = 0;
    virtual void visit(const IfStatement *) = 0;
    virtual void visit(const IncrementStatement *) = 0;
    virtual void visit(const LoopStatement *) = 0;
    virtual void visit(const NextStatement *) = 0;
    virtual void visit(const RaiseStatement *) = 0;
    virtual void visit(const RepeatStatement *) = 0;
    virtual void visit(const ReturnStatement *) = 0;
    virtual void visit(const TryStatement *) = 0;
    virtual void visit(const WhileStatement *) = 0;
    virtual void visit(const Program *) = 0;
};

class FunctionParameter;

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
    struct Field {
        Field(const Token &name, const Type *type, bool is_private): name(name), type(type), is_private(is_private) {}
        Field(const Field &rhs): name(rhs.name), type(rhs.type), is_private(rhs.is_private) {}
        Field &operator=(const Field &rhs) {
            if (&rhs == this) {
                return *this;
            }
            name = rhs.name;
            type = rhs.type;
            is_private = rhs.is_private;
            return *this;
        }
        Token name;
        const Type *type;
        bool is_private;
    };
    TypeRecord(const Token &token, const std::vector<Field> &fields): Type(token), fields(fields) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<Field> fields;
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

class TypeFunctionPointer: public Type {
public:
    TypeFunctionPointer(const Token &token, const Type *returntype, const std::vector<const FunctionParameter *> &args): Type(token), returntype(returntype), args(args) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Type *returntype;
    const std::vector<const FunctionParameter *> args;
private:
    TypeFunctionPointer(const TypeFunctionPointer &);
    TypeFunctionPointer &operator=(const TypeFunctionPointer &);
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

class TypeImport: public Type {
public:
    TypeImport(const Token &token, const Token &modname, const Token &subname): Type(token), modname(modname), subname(subname) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token modname;
    const Token subname;
};

class Expression: public ParseTreeNode {
public:
    Expression(const Token &token, size_t start_column, size_t end_column): ParseTreeNode(token), start_column(start_column), end_column(end_column) {}
    size_t get_start_column() const { return start_column; }
    size_t get_end_column() const { return end_column; }
private:
    size_t start_column;
    size_t end_column;
};

class DummyExpression: public Expression {
public:
    DummyExpression(const Token &token, size_t start_column, size_t end_column): Expression(token, start_column, end_column) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class IdentityExpression: public Expression {
public:
    IdentityExpression(const Token &token, size_t start_column, size_t end_column, const Expression *expr): Expression(token, start_column, end_column), expr(expr) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const expr;
private:
    IdentityExpression(const IdentityExpression &);
    IdentityExpression &operator=(const IdentityExpression &);
};

class BooleanLiteralExpression: public Expression {
public:
    BooleanLiteralExpression(const Token &token, bool value): Expression(token, token.column, token.column+token.text.length()), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const bool value;
};

class NumberLiteralExpression: public Expression {
public:
    NumberLiteralExpression(const Token &token, Number value): Expression(token, token.column, token.column+token.text.length()), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Number value;
};

class StringLiteralExpression: public Expression {
public:
    StringLiteralExpression(const Token &token, size_t end_column, const std::string &value): Expression(token, token.column, end_column), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string value;
};

class FileLiteralExpression: public Expression {
public:
    FileLiteralExpression(const Token &token, size_t end_column, const std::string &name): Expression(token, token.column, end_column), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string name;
};

class BytesLiteralExpression: public Expression {
public:
    BytesLiteralExpression(const Token &token, size_t end_column, const std::string &data): Expression(token, token.column, end_column), data(data) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string data;
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Token &token, size_t end_column, const std::vector<const Expression *> &elements): Expression(token, token.column, end_column), elements(elements) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const Expression *> elements;
};

class ArrayLiteralRangeExpression: public Expression {
public:
    ArrayLiteralRangeExpression(const Token &token, size_t end_column, const Expression *first, const Expression *last, const Expression *step): Expression(token, token.column, end_column), first(first), last(last), step(step) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *first;
    const Expression *last;
    const Expression *step;
private:
    ArrayLiteralRangeExpression(const ArrayLiteralRangeExpression &);
    ArrayLiteralRangeExpression &operator=(const ArrayLiteralRangeExpression &);
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const Token &token, size_t end_column, const std::vector<std::pair<Token, const Expression *>> &elements): Expression(token, token.column, end_column), elements(elements) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<Token, const Expression *>> elements;
};

class NilLiteralExpression: public Expression {
public:
    NilLiteralExpression(const Token &token): Expression(token, token.column, token.column+3) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class IdentifierExpression: public Expression {
public:
    IdentifierExpression(const Token &token, const std::string &name): Expression(token, token.column, token.column+token.text.length()), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string name;
};

class DotExpression: public Expression {
public:
    DotExpression(const Token &token, const Expression *base, const Token &name): Expression(token, base->get_start_column(), name.column+name.text.length()), base(base), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const Token name;
private:
    DotExpression(const DotExpression &);
    DotExpression &operator=(const DotExpression &);
};

class ArrowExpression: public Expression {
public:
    ArrowExpression(const Token &token, const Expression *base, const Token &name): Expression(token, base->get_start_column(), name.column+name.text.length()), base(base), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const Token name;
private:
    ArrowExpression(const ArrowExpression &);
    ArrowExpression &operator=(const ArrowExpression &);
};

class SubscriptExpression: public Expression {
public:
    SubscriptExpression(const Token &token, size_t end_column, const Expression *base, const Expression *index): Expression(token, base->get_start_column(), end_column), base(base), index(index) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const Expression *const index;
private:
    SubscriptExpression(const SubscriptExpression &);
    SubscriptExpression &operator=(const SubscriptExpression &);
};

class InterpolatedStringExpression: public Expression {
public:
    InterpolatedStringExpression(const Token &token, const std::vector<std::pair<const Expression *, Token>> &parts): Expression(token, parts.front().first->get_start_column(), parts.back().first->get_end_column()), parts(parts) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<const Expression *, Token>> parts;
};

class FunctionCallExpression: public Expression {
public:
    class Argument {
    public:
        Argument(const Token &mode, const Token &name, const Expression *expr): mode(mode), name(name), expr(expr) {}
        Argument(const Argument &rhs): mode(rhs.mode), name(rhs.name), expr(rhs.expr) {}
        Argument &operator=(const Argument &rhs) {
            if (this == &rhs) {
                return *this;
            }
            mode = rhs.mode;
            name = rhs.name;
            expr = rhs.expr;
            return *this;
        }
        Token mode;
        Token name;
        const Expression *expr;
    };
    FunctionCallExpression(const Token &token, const Expression *base, const std::vector<Argument> &args, const Token &rparen): Expression(token, base->get_start_column(), rparen.column+1), base(base), args(args), rparen(rparen) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const base;
    const std::vector<Argument> args;
    const Token rparen;
private:
    FunctionCallExpression(const FunctionCallExpression &);
    FunctionCallExpression &operator=(const FunctionCallExpression &);
};

class UnaryExpression: public Expression {
public:
    UnaryExpression(const Token &token, const Expression *expr): Expression(token, token.column, expr->get_end_column()), expr(expr) {}
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
    BinaryExpression(const Token &token, const Expression *left, const Expression *right): Expression(token, left->get_start_column(), right->get_end_column()), left(left), right(right) {}
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
    ChainedComparisonExpression(const std::vector<const ComparisonExpression *> &comps): Expression(comps[0]->token, comps.front()->get_start_column(), comps.back()->get_end_column()), comps(comps) {}
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
    ConditionalExpression(const Token &token, const Expression *cond, const Expression *left, const Expression *right): Expression(token, token.column, right->get_end_column()), cond(cond), left(left), right(right) {}
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
    NewRecordExpression(const Token &token, size_t end_column, const Type *type): Expression(token, token.column, end_column), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Type *const type;
private:
    NewRecordExpression(const NewRecordExpression &);
    NewRecordExpression &operator=(const NewRecordExpression &);
};

class ValidPointerExpression: public Expression {
public:
    struct Clause {
        Clause(const Expression *expr, const Token &name, bool shorthand): expr(expr), name(name), shorthand(shorthand) {}
        Clause(const Clause &rhs): expr(rhs.expr), name(rhs.name), shorthand(rhs.shorthand) {}
        Clause &operator=(const Clause &rhs) {
            if (this == &rhs) {
                return *this;
            }
            expr = rhs.expr;
            name = rhs.name;
            shorthand = rhs.shorthand;
            return *this;
        }
        const Expression *expr;
        Token name;
        bool shorthand;
    };
    ValidPointerExpression(const Token &token, std::vector<Clause> &tests): Expression(token, tests.front().expr->get_start_column(), tests.back().expr->get_end_column()), tests(tests) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<Clause> tests;
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
    RangeSubscriptExpression(const Token &token, size_t end_column, const Expression *base, const ArrayRange *range): Expression(token, base->get_start_column(), end_column), base(base), range(range) {}
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
    ImportDeclaration(const Token &token, const Token &module, const Token &name, const Token &alias): Declaration(token), module(module), name(name), alias(alias) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token module;
    const Token name;
    const Token alias;
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
    BaseFunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args, const Token &rparen): Declaration(token), type(type), name(name), returntype(returntype), args(args), rparen(rparen) {}
    const Token type;
    const Token name;
    const Type *const returntype;
    const std::vector<const FunctionParameter *> args;
    const Token rparen;
private:
    BaseFunctionDeclaration(const BaseFunctionDeclaration &);
    BaseFunctionDeclaration &operator=(const BaseFunctionDeclaration &);
};

class FunctionDeclaration: public BaseFunctionDeclaration {
public:
    FunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args, const Token &rparen, const std::vector<const Statement *> &body, const Token &end_function): BaseFunctionDeclaration(token, type, name, returntype, args, rparen), body(body), end_function(end_function) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const Statement *> body;
    const Token end_function;
};

class ExternalFunctionDeclaration: public BaseFunctionDeclaration {
public:
    ExternalFunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args, const Token &rparen, const Expression *dict): BaseFunctionDeclaration(token, type, name, returntype, args, rparen), dict(dict) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const dict;
private:
    ExternalFunctionDeclaration(const ExternalFunctionDeclaration &);
    ExternalFunctionDeclaration &operator=(const ExternalFunctionDeclaration &);
};

class NativeFunctionDeclaration: public BaseFunctionDeclaration {
public:
    NativeFunctionDeclaration(const Token &token, const Token &type, const Token &name, const Type *returntype, const std::vector<const FunctionParameter *> &args, const Token &rparen): BaseFunctionDeclaration(token, type, name, returntype, args, rparen) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
private:
    NativeFunctionDeclaration(const NativeFunctionDeclaration &);
    NativeFunctionDeclaration &operator=(const NativeFunctionDeclaration &);
};

class ExceptionDeclaration: public Declaration {
public:
    ExceptionDeclaration(const Token &token, const Token &name): Declaration(token), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
};

class ExportDeclaration: public Declaration {
public:
    ExportDeclaration(const Token &token, const Token &name): Declaration(token), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
};

class BlockStatement: public Statement {
public:
    BlockStatement(const Token &token, const std::vector<const Statement *> &body): Statement(token), body(body) {}
    const std::vector<const Statement *> body;
};

class MainBlock: public BlockStatement {
public:
    MainBlock(const Token &token, const std::vector<const Statement *> &body): BlockStatement(token, body) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class AssertStatement: public BlockStatement {
public:
    AssertStatement(const Token &token, std::vector<const Statement *> &body, const Expression *expr, const std::string &source): BlockStatement(token, body), expr(expr), source(source) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const expr;
    const std::string source;
private:
    AssertStatement(const AssertStatement &);
    AssertStatement &operator=(const AssertStatement &);
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

class CheckStatement: public BlockStatement {
public:
    CheckStatement(const Token &token, const Expression *cond, const std::vector<const Statement *> &body): BlockStatement(token, body), cond(cond) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *const cond;
private:
    CheckStatement(const CheckStatement &);
    CheckStatement &operator=(const CheckStatement &);
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

class ForeachStatement: public BlockStatement {
public:
    ForeachStatement(const Token &token, const Token &var, const Expression *array, const Token &index, const std::vector<const Statement *> &body): BlockStatement(token, body), var(var), array(array), index(index) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token var;
    const Expression *const array;
    const Token index;
private:
    ForeachStatement(const ForeachStatement &);
    ForeachStatement &operator=(const ForeachStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(const Token &token, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(token), condition_statements(condition_statements), else_statements(else_statements) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;
};

class IncrementStatement: public Statement {
public:
    IncrementStatement(const Token &token, const Expression *expr, int delta): Statement(token), expr(expr), delta(delta) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Expression *expr;
    const int delta;
private:
    IncrementStatement(const IncrementStatement &);
    IncrementStatement &operator=(const IncrementStatement &);
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
    RaiseStatement(const Token &token, const std::pair<Token, Token> &name, const Expression *info): Statement(token), name(name), info(info) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::pair<Token, Token> name;
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
    TryStatement(const Token &token, const std::vector<const Statement *> &body, const std::vector<std::pair<std::vector<std::pair<Token, Token>>, std::vector<const Statement *>>> &catches): BlockStatement(token, body), catches(catches) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<std::vector<std::pair<Token, Token>>, std::vector<const Statement *>>> catches;
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
    Program(const Token &token, const std::vector<const Statement *> &body, const std::string &source_path, const std::string &source_hash): BlockStatement(token, body), source_path(source_path), source_hash(source_hash) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string source_path;
    const std::string source_hash;
};

} // namespace pt

#endif
