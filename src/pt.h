#ifndef PT_H
#define PT_H

#include <memory>
#include <string>
#include <vector>

#include "number.h"
#include "sql.h"
#include "token.h"
#include "utf8string.h"

namespace pt {

class TypeSimple;
class TypeEnum;
class TypeRecord;
class TypeClass;
class TypePointer;
class TypeValidPointer;
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
class NowhereLiteralExpression;
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
class IntegerDivisionExpression;
class ModuloExpression;
class AdditionExpression;
class SubtractionExpression;
class ConcatenationExpression;
class ComparisonExpression;
class ChainedComparisonExpression;
class TypeTestExpression;
class MembershipExpression;
class ConjunctionExpression;
class DisjunctionExpression;
class ConditionalExpression;
class TryExpression;
class NewClassExpression;
class ValidPointerExpression;
class RangeSubscriptExpression;

class ImportDeclaration;
class TypeDeclaration;
class ConstantDeclaration;
class NativeConstantDeclaration;
class ExtensionConstantDeclaration;
class VariableDeclaration;
class NativeVariableDeclaration;
class LetDeclaration;
class FunctionDeclaration;
class ForeignFunctionDeclaration;
class NativeFunctionDeclaration;
class ExtensionFunctionDeclaration;
class ExceptionDeclaration;
class InterfaceDeclaration;
class ExportDeclaration;

class Statement;
class AssertStatement;
class AssignmentStatement;
class CaseStatement;
class CheckStatement;
class ExecStatement;
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
class TryHandlerStatement;
class UnusedStatement;
class WhileStatement;

class Program;

class IParseTreeVisitor {
public:
    virtual ~IParseTreeVisitor() {}

    virtual void visit(const TypeSimple *) = 0;
    virtual void visit(const TypeEnum *) = 0;
    virtual void visit(const TypeRecord *) = 0;
    virtual void visit(const TypeClass *) = 0;
    virtual void visit(const TypePointer *) = 0;
    virtual void visit(const TypeValidPointer *) = 0;
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
    virtual void visit(const NowhereLiteralExpression *) = 0;
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
    virtual void visit(const IntegerDivisionExpression *) = 0;
    virtual void visit(const ModuloExpression *) = 0;
    virtual void visit(const AdditionExpression *) = 0;
    virtual void visit(const SubtractionExpression *) = 0;
    virtual void visit(const ConcatenationExpression *) = 0;
    virtual void visit(const ComparisonExpression *) = 0;
    virtual void visit(const ChainedComparisonExpression *) = 0;
    virtual void visit(const TypeTestExpression *) = 0;
    virtual void visit(const MembershipExpression *) = 0;
    virtual void visit(const ConjunctionExpression *) = 0;
    virtual void visit(const DisjunctionExpression *) = 0;
    virtual void visit(const ConditionalExpression *) = 0;
    virtual void visit(const TryExpression *) = 0;
    virtual void visit(const NewClassExpression *) = 0;
    virtual void visit(const ValidPointerExpression *) = 0;
    virtual void visit(const RangeSubscriptExpression *) = 0;

    virtual void visit(const ImportDeclaration *) = 0;
    virtual void visit(const TypeDeclaration *) = 0;
    virtual void visit(const ConstantDeclaration *) = 0;
    virtual void visit(const NativeConstantDeclaration *) = 0;
    virtual void visit(const ExtensionConstantDeclaration *) = 0;
    virtual void visit(const VariableDeclaration *) = 0;
    virtual void visit(const NativeVariableDeclaration *) = 0;
    virtual void visit(const LetDeclaration *) = 0;
    virtual void visit(const FunctionDeclaration *) = 0;
    virtual void visit(const ForeignFunctionDeclaration *) = 0;
    virtual void visit(const NativeFunctionDeclaration *) = 0;
    virtual void visit(const ExtensionFunctionDeclaration *) = 0;
    virtual void visit(const ExceptionDeclaration *) = 0;
    virtual void visit(const InterfaceDeclaration *) = 0;
    virtual void visit(const ExportDeclaration *) = 0;

    virtual void visit(const AssertStatement *) = 0;
    virtual void visit(const AssignmentStatement *) = 0;
    virtual void visit(const CaseStatement *) = 0;
    virtual void visit(const CheckStatement *) = 0;
    virtual void visit(const ExecStatement *) = 0;
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
    virtual void visit(const TryHandlerStatement *) = 0;
    virtual void visit(const UnusedStatement *) = 0;
    virtual void visit(const WhileStatement *) = 0;
    virtual void visit(const Program *) = 0;
};

class FunctionParameterGroup;

class ParseTreeNode {
public:
    explicit ParseTreeNode(const Token &token): token(token) {}
    virtual ~ParseTreeNode() {}
    virtual void accept(IParseTreeVisitor *visitor) const = 0;
    const Token token;
private:
    ParseTreeNode(const ParseTreeNode &) = delete;
    ParseTreeNode &operator=(const ParseTreeNode &) = delete;
};

class Type: public ParseTreeNode {
public:
    explicit Type(const Token &token): ParseTreeNode(token) {}
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
        explicit Field(const Token &name, std::unique_ptr<Type> &&type, bool is_private): name(name), type(std::move(type)), is_private(is_private) {}
        Token name;
        std::unique_ptr<Type> type;
        bool is_private;
    };
    explicit TypeRecord(const Token &token, std::vector<std::unique_ptr<Field>> &&fields): Type(token), fields(std::move(fields)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::unique_ptr<Field>> fields;
};

class TypeClass: public TypeRecord {
public:
    explicit TypeClass(const Token &token, std::vector<std::unique_ptr<Field>> &&fields, const std::vector<std::pair<Token, Token>> &interfaces): TypeRecord(token, std::move(fields)), interfaces(interfaces) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::pair<Token, Token>> interfaces;
};

class TypePointer: public Type {
public:
    explicit TypePointer(const Token &token, std::unique_ptr<Type> &&reftype): Type(token), reftype(std::move(reftype)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Type> reftype;
};

class TypeValidPointer: public TypePointer {
public:
    explicit TypeValidPointer(const Token &token, std::unique_ptr<Type> &&reftype): TypePointer(token, std::move(reftype)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class TypeFunctionPointer: public Type {
public:
    explicit TypeFunctionPointer(const Token &token, std::unique_ptr<Type> &&returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &&args): Type(token), returntype(std::move(returntype)), args(std::move(args)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Type> returntype;
    std::vector<std::unique_ptr<FunctionParameterGroup>> args;
};

class TypeParameterised: public Type {
public:
    explicit TypeParameterised(const Token &name, std::unique_ptr<Type> &&elementtype): Type(name), name(token), elementtype(std::move(elementtype)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    std::unique_ptr<Type> elementtype;
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
    IdentityExpression(const Token &token, size_t start_column, size_t end_column, std::unique_ptr<Expression> &&expr): Expression(token, start_column, end_column), expr(std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
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
    StringLiteralExpression(const Token &token, size_t end_column, const utf8string &value): Expression(token, token.column, end_column), value(value) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const utf8string value;
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
    ArrayLiteralExpression(const Token &token, size_t end_column, std::vector<std::unique_ptr<Expression>> &&elements): Expression(token, token.column, end_column), elements(std::move(elements)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::unique_ptr<Expression>> elements;
};

class ArrayLiteralRangeExpression: public Expression {
public:
    ArrayLiteralRangeExpression(const Token &token, size_t end_column, std::unique_ptr<Expression> &&first, std::unique_ptr<Expression> &&last, std::unique_ptr<Expression> &&step): Expression(token, token.column, end_column), first(std::move(first)), last(std::move(last)), step(std::move(step)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> first;
    std::unique_ptr<Expression> last;
    std::unique_ptr<Expression> step;
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const Token &token, size_t end_column, std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> &&elements): Expression(token, token.column, end_column), elements(std::move(elements)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> elements;
};

class NilLiteralExpression: public Expression {
public:
    explicit NilLiteralExpression(const Token &token): Expression(token, token.column, token.column+token.text.length()) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class NowhereLiteralExpression: public Expression {
public:
    explicit NowhereLiteralExpression(const Token &token): Expression(token, token.column, token.column+token.text.length()) {}
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
    explicit DotExpression(const Token &token, std::unique_ptr<Expression> &&base, const Token &name): Expression(token, base->get_start_column(), name.column+name.text.length()), base(std::move(base)), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> base;
    const Token name;
};

class ArrowExpression: public Expression {
public:
    explicit ArrowExpression(const Token &token, std::unique_ptr<Expression> &&base, const Token &name): Expression(token, base->get_start_column(), name.column+name.text.length()), base(std::move(base)), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> base;
    const Token name;
};

class SubscriptExpression: public Expression {
public:
    SubscriptExpression(const Token &token, size_t end_column, std::unique_ptr<Expression> &&base, std::unique_ptr<Expression> &&index, bool from_last): Expression(token, base->get_start_column(), end_column), base(std::move(base)), index(std::move(index)), from_last(from_last) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> base;
    std::unique_ptr<Expression> index;
    bool from_last;
};

class InterpolatedStringExpression: public Expression {
public:
    explicit InterpolatedStringExpression(const Token &token, std::vector<std::pair<std::unique_ptr<Expression>, Token>> &&parts): Expression(token, parts.front().first->get_start_column(), parts.back().first->get_end_column()), parts(std::move(parts)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::pair<std::unique_ptr<Expression>, Token>> parts;
};

class FunctionCallExpression: public Expression {
public:
    class Argument {
    public:
        Argument(const Token &mode, const Token &name, std::unique_ptr<Expression> &&expr): mode(mode), name(name), expr(std::move(expr)) {}
        Token mode;
        Token name;
        std::unique_ptr<Expression> expr;
    };
    explicit FunctionCallExpression(const Token &token, std::unique_ptr<Expression> &&base, std::vector<std::unique_ptr<Argument>> &&args, const Token &rparen): Expression(token, base->get_start_column(), rparen.column+1), base(std::move(base)), args(std::move(args)), rparen(rparen) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> base;
    std::vector<std::unique_ptr<Argument>> args;
    const Token rparen;
};

class UnaryExpression: public Expression {
public:
    explicit UnaryExpression(const Token &token, std::unique_ptr<Expression> &&expr): Expression(token, token.column, expr->get_end_column()), expr(std::move(expr)) {}
    std::unique_ptr<Expression> expr;
};

class UnaryPlusExpression: public UnaryExpression {
public:
    explicit UnaryPlusExpression(const Token &token, std::unique_ptr<Expression> &&expr): UnaryExpression(token, std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class UnaryMinusExpression: public UnaryExpression {
public:
    explicit UnaryMinusExpression(const Token &token, std::unique_ptr<Expression> &&expr): UnaryExpression(token, std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class LogicalNotExpression: public UnaryExpression {
public:
    explicit LogicalNotExpression(const Token &token, std::unique_ptr<Expression> &&expr): UnaryExpression(token, std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class BinaryExpression: public Expression {
public:
    explicit BinaryExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): Expression(token, left->get_start_column(), right->get_end_column()), left(std::move(left)), right(std::move(right)) {}
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class ExponentiationExpression: public BinaryExpression {
public:
    explicit ExponentiationExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class MultiplicationExpression: public BinaryExpression {
public:
    explicit MultiplicationExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class DivisionExpression: public BinaryExpression {
public:
    explicit DivisionExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class IntegerDivisionExpression: public BinaryExpression {
public:
    explicit IntegerDivisionExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ModuloExpression: public BinaryExpression {
public:
    explicit ModuloExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class AdditionExpression: public BinaryExpression {
public:
    explicit AdditionExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class SubtractionExpression: public BinaryExpression {
public:
    explicit SubtractionExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ConcatenationExpression: public BinaryExpression {
public:
    explicit ConcatenationExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ComparisonExpression: public BinaryExpression {
public:
    enum class Comparison {
        EQ, NE, LT, GT, LE, GE
    };
    static std::string to_string(Comparison comp) {
        switch (comp) {
            case Comparison::EQ: return "EQ";
            case Comparison::NE: return "NE";
            case Comparison::LT: return "LT";
            case Comparison::GT: return "GT";
            case Comparison::LE: return "LE";
            case Comparison::GE: return "GE";
        }
        return "(undefined)";
    }
    explicit ComparisonExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right, Comparison comp): BinaryExpression(token, std::move(left), std::move(right)), comp(comp) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Comparison comp;
};

class ChainedComparisonExpression: public Expression {
public:
    class Part {
    public:
        explicit Part(ComparisonExpression::Comparison comp, std::unique_ptr<Expression> &&right): comp(comp), right(std::move(right)) {}
        ComparisonExpression::Comparison comp;
        std::unique_ptr<Expression> right;
    };
    ChainedComparisonExpression(std::unique_ptr<Expression> &&left, std::vector<std::unique_ptr<Part>> &&comps): Expression(left->token, left->get_start_column(), comps.back()->right->get_end_column()), left(std::move(left)), comps(std::move(comps)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> left;
    std::vector<std::unique_ptr<Part>> comps;
};

class TypeTestExpression: public Expression {
public:
    explicit TypeTestExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Type> &&target): Expression(token, left->get_start_column(), token.column + token.text.length()/* TODO */), left(std::move(left)), target(std::move(target)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> left;
    std::unique_ptr<Type> target;
};

class MembershipExpression: public BinaryExpression {
public:
    explicit MembershipExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ConjunctionExpression: public BinaryExpression {
public:
    explicit ConjunctionExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class DisjunctionExpression: public BinaryExpression {
public:
    explicit DisjunctionExpression(const Token &token, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): BinaryExpression(token, std::move(left), std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ConditionalExpression: public Expression {
public:
    explicit ConditionalExpression(const Token &token, std::unique_ptr<Expression> &&cond, std::unique_ptr<Expression> &&left, std::unique_ptr<Expression> &&right): Expression(token, token.column, right->get_end_column()), cond(std::move(cond)), left(std::move(left)), right(std::move(right)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> cond;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class TryTrap {
public:
    TryTrap(const std::vector<std::vector<Token>> &exceptions, const Token &name, std::unique_ptr<ParseTreeNode> &&handler): exceptions(exceptions), name(name), handler(std::move(handler)) {}
    TryTrap(TryTrap &&rhs): exceptions(rhs.exceptions), name(rhs.name), handler(std::move(rhs.handler)) {}
    std::vector<std::vector<Token>> exceptions;
    Token name;
    std::unique_ptr<ParseTreeNode> handler;
};

class TryExpression: public Expression {
public:
    explicit TryExpression(const Token &token, std::unique_ptr<Expression> &&expr, std::vector<std::unique_ptr<TryTrap>> &&catches): Expression(token, token.column, token.column), expr(std::move(expr)), catches(std::move(catches)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
    std::vector<std::unique_ptr<TryTrap>> catches;
};

class NewClassExpression: public Expression {
public:
    NewClassExpression(const Token &token, size_t end_column, std::unique_ptr<Expression> &&expr): Expression(token, token.column, end_column), expr(std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
};

class ValidPointerExpression: public Expression {
public:
    struct Clause {
        Clause(std::unique_ptr<Expression> &&expr, const Token &name, bool shorthand): expr(std::move(expr)), name(name), shorthand(shorthand) {}
        std::unique_ptr<Expression> expr;
        Token name;
        bool shorthand;
    };
    explicit ValidPointerExpression(const Token &token, std::vector<std::unique_ptr<Clause>> &&tests): Expression(token, tests.front()->expr->get_start_column(), tests.back()->expr->get_end_column()), tests(std::move(tests)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::unique_ptr<Clause>> tests;
};

class ArrayRange {
public:
    explicit ArrayRange(const Token &token, std::unique_ptr<Expression> &&first, bool first_from_end, std::unique_ptr<Expression> &&last, bool last_from_end): token(token), first(std::move(first)), first_from_end(first_from_end), last(std::move(last)), last_from_end(last_from_end) {}
    const Token token;
    std::unique_ptr<Expression> first;
    const bool first_from_end;
    std::unique_ptr<Expression> last;
    const bool last_from_end;
};

class RangeSubscriptExpression: public Expression {
public:
    RangeSubscriptExpression(const Token &token, size_t end_column, std::unique_ptr<Expression> &&base, std::unique_ptr<ArrayRange> &&range): Expression(token, base->get_start_column(), end_column), base(std::move(base)), range(std::move(range)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> base;
    std::unique_ptr<ArrayRange> range;
};

class FunctionParameterGroup {
public:
    enum class Mode {
        IN,
        INOUT,
        OUT
    };
    static std::string to_string(Mode mode) {
        switch (mode) {
            case Mode::IN:    return "IN";
            case Mode::INOUT: return "INOUT";
            case Mode::OUT:   return "OUT";
        }
        return "(undefined)";
    }
    FunctionParameterGroup(const Token &token, const std::vector<Token> &names, std::unique_ptr<Type> &&type, Mode mode, std::unique_ptr<Expression> &&default_value): token(token), names(names), type(std::move(type)), mode(mode), default_value(std::move(default_value)) {}
    const Token token;
    const std::vector<Token> names;
    std::unique_ptr<Type> type;
    const Mode mode;
    std::unique_ptr<Expression> default_value;
};

class Statement: public ParseTreeNode {
public:
    explicit Statement(const Token &token): ParseTreeNode(token) {}
};

class Declaration: public Statement {
public:
    Declaration(const Token &token, const std::vector<Token> &names): Statement(token), names(names) {}
    const std::vector<Token> names;
};

class ImportDeclaration: public Declaration {
public:
    ImportDeclaration(const Token &token, const Token &module, const Token &name, const Token &alias): Declaration(token, {name}), module(module), name(name), alias(alias) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token module;
    const Token name;
    const Token alias;
};

class TypeDeclaration: public Declaration {
public:
    explicit TypeDeclaration(const Token &token, std::unique_ptr<Type> &&type): Declaration(token, {}), type(std::move(type)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Type> type;
};

class ConstantDeclaration: public Declaration {
public:
    ConstantDeclaration(const Token &token, const Token &name, std::unique_ptr<Type> &&type, std::unique_ptr<Expression> &&value): Declaration(token, {name}), name(name), type(std::move(type)), value(std::move(value)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> value;
};

class NativeConstantDeclaration: public Declaration {
public:
    NativeConstantDeclaration(const Token &token, const Token &name, std::unique_ptr<Type> &&type): Declaration(token, {name}), name(name), type(std::move(type)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    std::unique_ptr<Type> type;
};

class ExtensionConstantDeclaration: public Declaration {
public:
    ExtensionConstantDeclaration(const Token &token, const Token &name, std::unique_ptr<Type> &&type): Declaration(token, {name}), name(name), type(std::move(type)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    std::unique_ptr<Type> type;
};

class VariableDeclaration: public Declaration {
public:
    VariableDeclaration(const Token &token, const std::vector<Token> &names, std::unique_ptr<Type> &&type, std::unique_ptr<Expression> &&value): Declaration(token, names), type(std::move(type)), value(std::move(value)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> value;
};

class NativeVariableDeclaration: public Declaration {
public:
    NativeVariableDeclaration(const Token &token, const Token &name, std::unique_ptr<Type> &&type): Declaration(token, {name}), name(name), type(std::move(type)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    std::unique_ptr<Type> type;
};

class LetDeclaration: public Declaration {
public:
    LetDeclaration(const Token &token, const Token &name, std::unique_ptr<Type> &&type, std::unique_ptr<Expression> &&value): Declaration(token, {name}), name(name), type(std::move(type)), value(std::move(value)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token name;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> value;
};

class BaseFunctionDeclaration: public Declaration {
public:
    BaseFunctionDeclaration(const Token &token, const Token &type, const Token &name, std::unique_ptr<Type> &&returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &&args, const Token &rparen): Declaration(token, {name}), type(type), name(name), returntype(std::move(returntype)), args(std::move(args)), rparen(rparen) {}
    const Token type;
    const Token name;
    std::unique_ptr<Type> returntype;
    std::vector<std::unique_ptr<FunctionParameterGroup>> args;
    const Token rparen;
};

class FunctionDeclaration: public BaseFunctionDeclaration {
public:
    FunctionDeclaration(const Token &token, const Token &type, const Token &name, std::unique_ptr<Type> &&returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &&args, const Token &rparen, std::vector<std::unique_ptr<Statement>> &&body, const Token &end_function): BaseFunctionDeclaration(token, type, name, std::move(returntype), std::move(args), rparen), body(std::move(body)), end_function(end_function) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<std::unique_ptr<Statement>> body;
    const Token end_function;
};

class ForeignFunctionDeclaration: public BaseFunctionDeclaration {
public:
    ForeignFunctionDeclaration(const Token &token, const Token &type, const Token &name, std::unique_ptr<Type> &&returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &&args, const Token &rparen, std::unique_ptr<Expression> &&dict): BaseFunctionDeclaration(token, type, name, std::move(returntype), std::move(args), rparen), dict(std::move(dict)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> dict;
};

class NativeFunctionDeclaration: public BaseFunctionDeclaration {
public:
    NativeFunctionDeclaration(const Token &token, const Token &type, const Token &name, std::unique_ptr<Type> &&returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &&args, const Token &rparen): BaseFunctionDeclaration(token, type, name, std::move(returntype), std::move(args), rparen) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ExtensionFunctionDeclaration: public BaseFunctionDeclaration {
public:
    ExtensionFunctionDeclaration(const Token &token, const Token &type, const Token &name, std::unique_ptr<Type> &&returntype, std::vector<std::unique_ptr<FunctionParameterGroup>> &&args, const Token &rparen): BaseFunctionDeclaration(token, type, name, std::move(returntype), std::move(args), rparen) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class ExceptionDeclaration: public Declaration {
public:
    ExceptionDeclaration(const Token &token, const std::vector<Token> &name): Declaration(token, {name}), name(name) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<Token> name;
};

class InterfaceDeclaration: public Declaration {
public:
    InterfaceDeclaration(const Token &token, const Token &name, std::vector<std::pair<Token, std::unique_ptr<TypeFunctionPointer>>> &&methods): Declaration(token, {name}), methods(std::move(methods)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::pair<Token, std::unique_ptr<TypeFunctionPointer>>> methods;
};

class ExportDeclaration: public Declaration {
public:
    ExportDeclaration(const Token &token, const std::vector<Token> &names, std::unique_ptr<Declaration> &&declaration): Declaration(token, names), declaration(std::move(declaration)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Declaration> declaration;
};

class BlockStatement: public Statement {
public:
    explicit BlockStatement(const Token &token, std::vector<std::unique_ptr<Statement>> &&body): Statement(token), body(std::move(body)) {}
    std::vector<std::unique_ptr<Statement>> body;
};

class BaseLoopStatement: public BlockStatement {
public:
    BaseLoopStatement(const Token &token, const Token &label, std::vector<std::unique_ptr<Statement>> &&body): BlockStatement(token, std::move(body)), label(label) {}
    const Token label;
};

class AssertStatement: public Statement {
public:
    explicit AssertStatement(const Token &token, std::vector<std::unique_ptr<Expression>> &&exprs, const std::string &source): Statement(token), exprs(std::move(exprs)), source(source) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::unique_ptr<Expression>> exprs;
    const std::string source;
};

class AssignmentStatement: public Statement {
public:
    explicit AssignmentStatement(const Token &token, std::vector<std::unique_ptr<Expression>> &&variables, std::unique_ptr<Expression> &&expr): Statement(token), variables(std::move(variables)), expr(std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::unique_ptr<Expression>> variables;
    std::unique_ptr<Expression> expr;
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        explicit WhenCondition(const Token &token): token(token) {}
        virtual ~WhenCondition() {}
        const Token token;
    private:
        WhenCondition(const WhenCondition &) = delete;
        WhenCondition &operator=(const WhenCondition &) = delete;
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(const Token &token, ComparisonExpression::Comparison comp, std::unique_ptr<Expression> &&expr): WhenCondition(token), comp(comp), expr(std::move(expr)) {}
        const ComparisonExpression::Comparison comp;
        std::unique_ptr<Expression> expr;
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        explicit RangeWhenCondition(const Token &token, std::unique_ptr<Expression> &&low_expr, std::unique_ptr<Expression> &&high_expr): WhenCondition(token), low_expr(std::move(low_expr)), high_expr(std::move(high_expr)) {}
        std::unique_ptr<Expression> low_expr;
        std::unique_ptr<Expression> high_expr;
    };
    class TypeTestWhenCondition: public WhenCondition {
    public:
        explicit TypeTestWhenCondition(const Token &token, std::unique_ptr<Type> &&target): WhenCondition(token), target(std::move(target)) {}
        std::unique_ptr<Type> target;
    };
    explicit CaseStatement(const Token &token, std::unique_ptr<Expression> &&expr, std::vector<std::pair<std::vector<std::unique_ptr<WhenCondition>>, std::vector<std::unique_ptr<Statement>>>> &&clauses): Statement(token), expr(std::move(expr)), clauses(std::move(clauses)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
    const std::vector<std::pair<std::vector<std::unique_ptr<WhenCondition>>, std::vector<std::unique_ptr<Statement>>>> clauses;
};

class CheckStatement: public BlockStatement {
public:
    explicit CheckStatement(const Token &token, std::unique_ptr<Expression> &&cond, std::vector<std::unique_ptr<Statement>> &&body): BlockStatement(token, std::move(body)), cond(std::move(cond)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> cond;
};

class ExecStatement: public Statement {
public:
    ExecStatement(const Token &token, const std::string &text, std::unique_ptr<SqlStatementInfo> &&info): Statement(token), text(text), info(std::move(info)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string text;
    std::unique_ptr<SqlStatementInfo> info;
};

class ExitStatement: public Statement {
public:
    ExitStatement(const Token &token, const Token &type): Statement(token), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token type;
};

class ExpressionStatement: public Statement {
public:
    explicit ExpressionStatement(const Token &token, std::unique_ptr<Expression> &&expr): Statement(token), expr(std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
};

class ForStatement: public BaseLoopStatement {
public:
    ForStatement(const Token &token, const Token &var, std::unique_ptr<Expression> &&start, std::unique_ptr<Expression> &&end, std::unique_ptr<Expression> &&step, const Token &label, std::vector<std::unique_ptr<Statement>> &&body): BaseLoopStatement(token, label, std::move(body)), var(var), start(std::move(start)), end(std::move(end)), step(std::move(step)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token var;
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;
    std::unique_ptr<Expression> step;
};

class ForeachStatement: public BaseLoopStatement {
public:
    ForeachStatement(const Token &token, const Token &var, std::unique_ptr<Expression> &&array, const Token &index, const Token &label, std::vector<std::unique_ptr<Statement>> &&body): BaseLoopStatement(token, label, std::move(body)), var(var), array(std::move(array)), index(index) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token var;
    std::unique_ptr<Expression> array;
    const Token index;
};

class IfStatement: public Statement {
public:
    explicit IfStatement(const Token &token, std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::unique_ptr<Statement>>>> &&condition_statements, std::vector<std::unique_ptr<Statement>> &&else_statements): Statement(token), condition_statements(std::move(condition_statements)), else_statements(std::move(else_statements)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::unique_ptr<Statement>>>> condition_statements;
    std::vector<std::unique_ptr<Statement>> else_statements;
};

class IncrementStatement: public Statement {
public:
    explicit IncrementStatement(const Token &token, std::unique_ptr<Expression> &&expr, int delta): Statement(token), expr(std::move(expr)), delta(delta) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
    const int delta;
};

class LoopStatement: public BaseLoopStatement {
public:
    LoopStatement(const Token &token, const Token &label, std::vector<std::unique_ptr<Statement>> &&body): BaseLoopStatement(token, label, std::move(body)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class NextStatement: public Statement {
public:
    NextStatement(const Token &token, const Token &type): Statement(token), type(type) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const Token type;
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(const Token &token, const std::vector<Token> &name, std::unique_ptr<Expression> &&info): Statement(token), name(name), info(std::move(info)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<Token> name;
    std::unique_ptr<Expression> info;
};

class RepeatStatement: public BaseLoopStatement {
public:
    RepeatStatement(const Token &token, const Token &label, std::unique_ptr<Expression> &&cond, std::vector<std::unique_ptr<Statement>> &&body): BaseLoopStatement(token, label, std::move(body)), cond(std::move(cond)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> cond;
};

class ReturnStatement: public Statement {
public:
    explicit ReturnStatement(const Token &token, std::unique_ptr<Expression> &&expr): Statement(token), expr(std::move(expr)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> expr;
};

class TryStatement: public BlockStatement {
public:
    explicit TryStatement(const Token &token, std::vector<std::unique_ptr<Statement>> &&body, std::vector<std::unique_ptr<TryTrap>> &&catches): BlockStatement(token, std::move(body)), catches(std::move(catches)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<std::unique_ptr<TryTrap>> catches;
};

class TryHandlerStatement: public BlockStatement {
public:
    explicit TryHandlerStatement(const Token &token, std::vector<std::unique_ptr<Statement>> &&body): BlockStatement(token, std::move(body)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
};

class UnusedStatement: public Statement {
public:
    UnusedStatement(const Token &token, const std::vector<Token> &vars): Statement(token), vars(vars) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::vector<Token> vars;
};

class WhileStatement: public BaseLoopStatement {
public:
    explicit WhileStatement(const Token &token, std::unique_ptr<Expression> &&cond, const Token &label, std::vector<std::unique_ptr<Statement>> &&body): BaseLoopStatement(token, label, std::move(body)), cond(std::move(cond)) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    std::unique_ptr<Expression> cond;
};

class Program: public BlockStatement {
public:
    explicit Program(const Token &token, std::vector<std::unique_ptr<Statement>> &&body, const std::string &source_path, const std::string &source_hash): BlockStatement(token, std::move(body)), source_path(source_path), source_hash(source_hash) {}
    virtual void accept(IParseTreeVisitor *visitor) const override { visitor->visit(this); }
    const std::string source_path;
    const std::string source_hash;
};

} // namespace pt

#endif
