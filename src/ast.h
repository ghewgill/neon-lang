#ifndef AST_H
#define AST_H

#include <iso646.h>
#include <limits.h>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "number.h"
#include "token.h"
#include "util.h"

// Compiler
class Emitter;

class AstNode {
public:
    AstNode() {}
    virtual ~AstNode() {}
    void dump(std::ostream &out, int depth = 0) const;
    virtual std::string text() const = 0;
    virtual void dumpsubnodes(std::ostream &/*out*/, int /*depth*/) const {}
private:
    AstNode(const AstNode &);
    AstNode &operator=(const AstNode &);
};

class Name;
class Type;
class TypeRecord;
class TypePointer;
class Variable;
class FunctionCall;
class FunctionParameter;
class Expression;
class Statement;

class Frame {
public:
    Frame(Frame *outer): outer(outer), predeclared(false), slots() {}

    void predeclare(Emitter &emitter);
    void postdeclare(Emitter &emitter);

    struct Slot {
        Slot(const Token &token, const std::string &name, Name *ref, bool referenced): token(token), name(name), ref(ref), referenced(referenced) {}
        Slot(const Slot &rhs): token(rhs.token), name(rhs.name), ref(rhs.ref), referenced(rhs.referenced) {}
        Slot &operator=(const Slot &rhs) {
            if (this == &rhs) {
                return *this;
            }
            token = rhs.token;
            name = rhs.name;
            ref = rhs.ref;
            referenced = rhs.referenced;
            return *this;
        }

        Token token;
        std::string name;
        Name *ref;
        bool referenced;
    };

    size_t getCount() const { return slots.size(); }
    int addSlot(const Token &token, const std::string &name, Name *ref, bool init_referenced);
    const Slot getSlot(int slot);
    void setReferent(int slot, Name *ref);
    void setReferenced(int slot);

private:
    Frame *const outer;
    bool predeclared;
    std::vector<Slot> slots;
private:
    Frame(const Frame &);
    Frame &operator=(const Frame &);
};

class Scope {
public:
    Scope(Scope *parent, Frame *frame): parent(parent), frame(frame), names(), forwards() {}
    virtual ~Scope() {}

    bool allocateName(const Token &token, const std::string &name);
    Name *lookupName(const std::string &name);
    Token getDeclaration(const std::string &name) const;
    void addName(const Token &token, const std::string &name, Name *ref, bool init_referenced = false);
    void addForward(const std::string &name, TypePointer *ptrtype);
    void resolveForward(const std::string &name, const TypeRecord *rectype);
    void checkForward();

    Scope *const parent;
    Frame *const frame;
private:
    std::map<std::string, int> names;
    std::map<std::string, std::vector<TypePointer *>> forwards;
private:
    Scope(const Scope &);
    Scope &operator=(const Scope &);
};

class Name: public AstNode {
public:
    Name(const Token &declaration, const std::string &name, const Type *type): declaration(declaration), name(name), type(type) {}
    const Token declaration;
    const std::string name;
    const Type *type;

    virtual void predeclare(Emitter &) const {}
    virtual void postdeclare(Emitter &) const {}
private:
    Name(const Name &);
    Name &operator=(const Name &);
};

class Type: public Name {
public:
    Type(const Token &declaration, const std::string &name): Name(declaration, name, nullptr), methods() {}

    std::map<std::string, Variable *> methods;

    virtual void predeclare(Emitter &emitter) const;
    virtual void postdeclare(Emitter &emitter) const;
    virtual bool is_equivalent(const Type *rhs) const { return this == rhs; }
    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;
};

class TypeNothing: public Type {
public:
    TypeNothing(): Type(Token(), "Nothing") {}
    virtual void generate_load(Emitter &) const { internal_error("TypeNothing"); }
    virtual void generate_store(Emitter &) const { internal_error("TypeNothing"); }
    virtual void generate_call(Emitter &) const { internal_error("TypeNothing"); }

    virtual std::string text() const { return "TypeNothing"; }
};

extern TypeNothing *TYPE_NOTHING;

class TypeBoolean: public Type {
public:
    TypeBoolean(): Type(Token(), "Boolean") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeBoolean"; }
};

extern TypeBoolean *TYPE_BOOLEAN;

class TypeNumber: public Type {
public:
    TypeNumber(const Token &declaration): Type(declaration, "Number") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeNumber"; }
};

extern TypeNumber *TYPE_NUMBER;

class TypeString: public Type {
public:
    TypeString(): Type(Token(), "String") {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeString"; }
};

extern TypeString *TYPE_STRING;

class TypeBytes: public TypeString {
public:
    TypeBytes(): TypeString() {}

    virtual std::string text() const { return "TypeBytes"; }
};

extern TypeBytes *TYPE_BYTES;

class ParameterType {
public:
    enum Mode {
        IN,
        INOUT,
        OUT
    };
    ParameterType(const Token &declaration, Mode mode, const Type *type, const Expression *default_value): declaration(declaration), mode(mode), type(type), default_value(default_value) {}
    const Token declaration;
    const Mode mode;
    const Type *type;
    const Expression *default_value;
private:
    ParameterType(const ParameterType &);
    ParameterType &operator=(const ParameterType &);
};

class TypeFunction: public Type {
public:
    TypeFunction(const Type *returntype, const std::vector<const ParameterType *> &params): Type(Token(), "function"), returntype(returntype), params(params) {}
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    const Type *returntype;
    const std::vector<const ParameterType *> params;

    virtual std::string text() const { return "TypeFunction(...)"; }
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const Token &declaration, const Type *elementtype);
    const Type *elementtype;

    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeArray(" + elementtype->text() + ")"; }
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

extern TypeArray *TYPE_ARRAY_NUMBER;
extern TypeArray *TYPE_ARRAY_STRING;

class TypeDictionary: public Type {
public:
    TypeDictionary(const Token &declaration, const Type *elementtype): Type(declaration, "dictionary"), elementtype(elementtype) {}
    const Type *elementtype;

    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeDictionary(" + elementtype->text() + ")"; }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

class TypeRecord: public Type {
public:
    TypeRecord(const Token &declaration, const std::vector<std::pair<Token, const Type *>> &fields): Type(declaration, "record"), fields(fields), field_names(make_field_names(fields)) {}
    const std::vector<std::pair<Token, const Type *>> fields;
    const std::map<std::string, size_t> field_names;

    virtual void predeclare(Emitter &emitter) const;
    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypeRecord(...)"; }
private:
    static std::map<std::string, size_t> make_field_names(const std::vector<std::pair<Token, const Type *>> &fields) {
        std::map<std::string, size_t> r;
        size_t i = 0;
        for (auto f: fields) {
            r[f.first.text] = i;
            i++;
        }
        return r;
    }
};

class TypeForwardRecord: public TypeRecord {
public:
    TypeForwardRecord(const Token &declaration): TypeRecord(declaration, std::vector<std::pair<Token, const Type *>>()) {}
};

class TypePointer: public Type {
public:
    TypePointer(const Token &declaration, const TypeRecord *reftype): Type(declaration, "pointer"), reftype(reftype) {}

    const TypeRecord *reftype;

    virtual bool is_equivalent(const Type *rhs) const;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "TypePointer(" + reftype->text() + ")"; }
private:
    TypePointer(const TypePointer &);
    TypePointer &operator=(const TypePointer &);
};

extern TypePointer *TYPE_POINTER;

class TypeValidPointer: public TypePointer {
public:
    TypeValidPointer(const TypePointer *ptrtype): TypePointer(Token(), ptrtype->reftype) {}
};

class TypeEnum: public TypeNumber {
public:
    TypeEnum(const Token &declaration, const std::map<std::string, int> &names);
    const std::map<std::string, int> names;

    virtual std::string text() const { return "TypeEnum(...)"; }
};

class TypeModule: public Type {
public:
    TypeModule(): Type(Token(), "module") {}

    virtual void generate_load(Emitter &) const { internal_error("TypeModule"); }
    virtual void generate_store(Emitter &) const { internal_error("TypeModule"); }
    virtual void generate_call(Emitter &) const { internal_error("TypeModule"); }

    virtual std::string text() const { return "TypeModule(...)"; }
};

extern TypeModule *TYPE_MODULE;

class TypeException: public Type {
public:
    TypeException(): Type(Token(), "Exception") {}

    virtual void generate_load(Emitter &) const { internal_error("TypeException"); }
    virtual void generate_store(Emitter &) const { internal_error("TypeException"); }
    virtual void generate_call(Emitter &) const { internal_error("TypeException"); }

    virtual std::string text() const { return "TypeException"; }
};

extern TypeException *TYPE_EXCEPTION;

class Variable: public Name {
public:
    Variable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Name(declaration, name, type), is_readonly(is_readonly) {}

    bool is_readonly;

    virtual void generate_address(Emitter &emitter, int enclosing) const = 0;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "Variable(" + name + ", " + type->text() + ")"; }
};

class GlobalVariable: public Variable {
public:
    GlobalVariable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Variable(declaration, name, type, is_readonly), index(-1) {}
    mutable int index;

    virtual void predeclare(Emitter &emitter) const;
    virtual void generate_address(Emitter &emitter, int enclosing) const;

    virtual std::string text() const { return "GlobalVariable(" + name + ", " + type->text() + ")"; }
};

class LocalVariable: public Variable {
public:
    LocalVariable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Variable(declaration, name, type, is_readonly), index(-1) {}
    int index;

    virtual void predeclare(Emitter &) const { internal_error("LocalVariable"); }
    virtual void predeclare(Emitter &emitter, int slot);
    virtual void generate_address(Emitter &emitter, int enclosing) const;

    virtual std::string text() const { return "LocalVariable(" + name + ", " + type->text() + ")"; }
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class FunctionParameter: public LocalVariable {
public:
    FunctionParameter(const Token &declaration, const std::string &name, const Type *type, ParameterType::Mode mode, const Expression *default_value): LocalVariable(declaration, name, type, mode == ParameterType::IN), mode(mode), default_value(default_value) {}
    ParameterType::Mode mode;
    const Expression *default_value;

    virtual void generate_address(Emitter &emitter, int enclosing) const;

    virtual std::string text() const { return "FunctionParameter(" + name + ", " + type->text() + ")"; }
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class Exception: public Name {
public:
    Exception(const Token &declaration, const std::string &name): Name(declaration, name, TYPE_EXCEPTION) {}

    virtual std::string text() const { return "Exception(" + name + ")"; }
private:
    Exception(const Exception &);
    Exception &operator=(const Exception &);
};

class Expression: public AstNode {
public:
    Expression(const Type *type, bool is_constant, bool is_readonly = true): type(type), is_constant(is_constant), is_readonly(is_readonly) {}

    virtual Number eval_number() const = 0;
    virtual std::string eval_string() const = 0;
    virtual void generate(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &) const { internal_error("Expression::generate_call"); }

    const Type *type;
    const bool is_constant;
    const bool is_readonly;
private:
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

class Constant: public Name {
public:
    Constant(const Token &declaration, const std::string &name, const Expression *value): Name(declaration, name, value->type), value(value) {}

    const Expression *value;

    virtual std::string text() const { return "Constant(" + name + ", " + value->text() + ")"; }
private:
    Constant(const Constant &);
    Constant &operator=(const Constant &);
};

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(bool value): Expression(TYPE_BOOLEAN, true), value(value) {}

    const bool value;

    virtual Number eval_number() const { internal_error("ConstantBooleanExpression"); }
    virtual std::string eval_string() const { internal_error("ConstantBooleanExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(Number value): Expression(TYPE_NUMBER, true), value(value) {}

    const Number value;

    virtual Number eval_number() const { return value; }
    virtual std::string eval_string() const { internal_error("ConstantNumberExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const std::string &value): Expression(TYPE_STRING, true), value(value) {}

    const std::string value;

    virtual Number eval_number() const { internal_error("ConstantStringExpression"); }
    virtual std::string eval_string() const { return value; }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const TypeEnum *type, int value): Expression(type, true), value(value) {}

    const int value;

    virtual Number eval_number() const { return number_from_uint32(value); }
    virtual std::string eval_string() const { internal_error("ConstantEnumExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(): Expression(new TypePointer(Token(), nullptr), true) {}

    virtual Number eval_number() const { internal_error("ConstantNilExpression"); }
    virtual std::string eval_string() const { internal_error("ConstantNilExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ConstantNilExpression"; }
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Type *elementtype, const std::vector<const Expression *> &elements): Expression(new TypeArray(Token(), elementtype), all_constant(elements)), elementtype(elementtype), elements(elements) {}

    const Type *elementtype;
    const std::vector<const Expression *> elements;

    virtual Number eval_number() const { internal_error("ArrayLiteralExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayLiteralExpression"); }
    virtual void generate(Emitter &) const;

    virtual std::string text() const { return "ArrayLiteralExpression(...)"; }
private:
    ArrayLiteralExpression(const ArrayLiteralExpression &);
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &);

    static bool all_constant(const std::vector<const Expression *> &elements);
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const Type *elementtype, const std::vector<std::pair<std::string, const Expression *>> &elements): Expression(new TypeDictionary(Token(), elementtype), all_constant(elements)), elementtype(elementtype), dict(make_dictionary(elements)) {}

    const Type *elementtype;
    const std::map<std::string, const Expression *> dict;

    virtual Number eval_number() const { internal_error("DictionaryLiteralExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryLiteralExpression"); }
    virtual void generate(Emitter &) const;

    virtual std::string text() const { return "DictionaryLiteralExpression(...)"; }
private:
    DictionaryLiteralExpression(const DictionaryLiteralExpression &);
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &);

    static bool all_constant(const std::vector<std::pair<std::string, const Expression *>> &elements);
    static std::map<std::string, const Expression *> make_dictionary(const std::vector<std::pair<std::string, const Expression *>> &elements);
};

class RecordLiteralExpression: public Expression {
public:
    RecordLiteralExpression(const TypeRecord *type, const std::vector<const Expression *> &values): Expression(type, all_constant(values)), values(values) {}

    const std::vector<const Expression *> values;

    virtual Number eval_number() const { internal_error("RecordLiteralExpression"); }
    virtual std::string eval_string() const { internal_error("RecordLiteralExpression"); }
    virtual void generate(Emitter &) const;

    virtual std::string text() const { return "RecordLiteralExpression(...)"; }
private:
    RecordLiteralExpression(const RecordLiteralExpression &);
    RecordLiteralExpression &operator=(const RecordLiteralExpression &);

    static bool all_constant(const std::vector<const Expression *> &values);
};

class NewRecordExpression: public Expression {
public:
    NewRecordExpression(const TypeRecord *reftype): Expression(new TypePointer(Token(), reftype), false), fields(reftype->fields.size()) {}

    const size_t fields;

    virtual Number eval_number() const { internal_error("NewRecordExpression"); }
    virtual std::string eval_string() const { internal_error("NewRecordExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "NewRecordExpression(" + type->text() + ")"; }
private:
    NewRecordExpression(const NewRecordExpression &);
    NewRecordExpression &operator=(const NewRecordExpression &);
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const Expression *value): Expression(value->type, value->is_constant), value(value) {
        if (not type->is_equivalent(TYPE_NUMBER)) {
            internal_error("UnaryMinusExpression");
        }
    }

    const Expression *const value;

    virtual Number eval_number() const { return number_negate(value->eval_number()); }
    virtual std::string eval_string() const { internal_error("UnaryMinusExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "UnaryMinusExpression(" + value->text() + ")";
    }
private:
    UnaryMinusExpression(const UnaryMinusExpression &);
    UnaryMinusExpression &operator=(const UnaryMinusExpression &);
};

class LogicalNotExpression: public Expression {
public:
    LogicalNotExpression(const Expression *value): Expression(value->type, value->is_constant), value(value) {
        if (not type->is_equivalent(TYPE_BOOLEAN)) {
            internal_error("LogicalNotExpression");
        }
    }

    const Expression *const value;

    virtual Number eval_number() const { internal_error("LogicalNotExpression"); }
    virtual std::string eval_string() const { internal_error("LogicalNotExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "LogicalNotExpression(" + value->text() + ")";
    }
private:
    LogicalNotExpression(const LogicalNotExpression &);
    LogicalNotExpression &operator=(const LogicalNotExpression &);
};

class ConditionalExpression: public Expression {
public:
    ConditionalExpression(const Expression *condition, const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), condition(condition), left(left), right(right) {
        if (not left->type->is_equivalent(right->type)) {
            internal_error("ConditionalExpression");
        }
    }

    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual Number eval_number() const { internal_error("ConditionalExpression"); }
    virtual std::string eval_string() const { internal_error("ConditionalExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ConditionalExpression(" + condition->text() + "," + left->text() + "," + right->text() + ")";
    }
private:
    ConditionalExpression(const ConditionalExpression &);
    ConditionalExpression &operator=(const ConditionalExpression &);
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_BOOLEAN) || not right->type->is_equivalent(TYPE_BOOLEAN)) {
            internal_error("DisjunctionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { internal_error("DisjunctionExpression"); }
    virtual std::string eval_string() const { internal_error("DisjunctionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DisjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    DisjunctionExpression(const DisjunctionExpression &);
    DisjunctionExpression &operator=(const DisjunctionExpression &);
};

class ConjunctionExpression: public Expression {
public:
    ConjunctionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_BOOLEAN) || not right->type->is_equivalent(TYPE_BOOLEAN)) {
            internal_error("ConjunctionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { internal_error("ConjunctionExpression"); }
    virtual std::string eval_string() const { internal_error("ConjunctionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ConjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ConjunctionExpression(const ConjunctionExpression &);
    ConjunctionExpression &operator=(const ConjunctionExpression &);
};

class ArrayInExpression: public Expression {
public:
    ArrayInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}

    const Expression *left;
    const Expression *right;

    virtual Number eval_number() const { internal_error("ArrayInExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayInExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ArrayInExpression(" + left->text() + ", " + right->text() + ")"; }
private:
    ArrayInExpression(const ArrayInExpression &);
    ArrayInExpression &operator=(const ArrayInExpression &);
};

class DictionaryInExpression: public Expression {
public:
    DictionaryInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}

    const Expression *left;
    const Expression *right;

    virtual Number eval_number() const { internal_error("DictionaryInExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryInExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "DictionaryInExpression(" + left->text() + ", " + right->text() + ")"; }
private:
    DictionaryInExpression(const DictionaryInExpression &);
    DictionaryInExpression &operator=(const DictionaryInExpression &);
};

class ComparisonExpression: public Expression {
public:
    enum Comparison {
        EQ, NE, LT, GT, LE, GE
    };
    ComparisonExpression(const Expression *left, const Expression *right, Comparison comp): Expression(TYPE_BOOLEAN, left->is_constant && right->is_constant), left(left), right(right), comp(comp) {}

    const Expression *const left;
    const Expression *const right;
    const Comparison comp;

    virtual void generate(Emitter &emitter) const;
    virtual void generate_comparison_opcode(Emitter &emitter) const = 0;
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
};

class ChainedComparisonExpression: public Expression {
public:
    ChainedComparisonExpression(const std::vector<const ComparisonExpression *> &comps): Expression(TYPE_BOOLEAN, false), comps(comps) {}

    const std::vector<const ComparisonExpression *> comps;

    virtual Number eval_number() const { internal_error("ChainedComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("ChainedComparisonExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ChainedComparisonExpression(...)"; }
private:
    ChainedComparisonExpression(const ChainedComparisonExpression &);
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &);
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    BooleanComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("BooleanComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("BooleanComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const;

    virtual std::string text() const {
        return "BooleanComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("NumericComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("NumericComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const;

    virtual std::string text() const {
        return "NumericComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("StringComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("StringComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const;

    virtual std::string text() const {
        return "StringComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("ArrayComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const;

    virtual std::string text() const {
        return "ArrayComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("DictionaryComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const;

    virtual std::string text() const {
        return "DictionaryComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    PointerComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual Number eval_number() const { internal_error("PointerComparisonExpression"); }
    virtual std::string eval_string() const { internal_error("PointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const;

    virtual std::string text() const {
        return "PointerComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    ValidPointerExpression(const Variable *var, const Expression *ptr): PointerComparisonExpression(ptr, new ConstantNilExpression(), ComparisonExpression::NE), var(var) {}

    const Variable *var;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ValidPointerExpression(" + left->text() + ")";
    }
private:
    ValidPointerExpression(const ValidPointerExpression &);
    ValidPointerExpression &operator=(const ValidPointerExpression &);
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("AdditionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_add(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("AdditionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "AdditionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    AdditionExpression(const AdditionExpression &);
    AdditionExpression &operator=(const AdditionExpression &);
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("SubtractionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_subtract(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("SubtractionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "SubtractionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    SubtractionExpression(const SubtractionExpression &);
    SubtractionExpression &operator=(const SubtractionExpression &);
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("MultiplicationExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_multiply(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("MultiplicationExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "MultiplicationExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    MultiplicationExpression(const MultiplicationExpression &);
    MultiplicationExpression &operator=(const MultiplicationExpression &);
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("DivisionExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_divide(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("DivisionExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "DivisionExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    DivisionExpression(const DivisionExpression &);
    DivisionExpression &operator=(const DivisionExpression &);
};

class ModuloExpression: public Expression {
public:
    ModuloExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("ModuloExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_modulo(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("ModuloExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ModuloExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ModuloExpression(const ModuloExpression &);
    ModuloExpression &operator=(const ModuloExpression &);
};

class ExponentiationExpression: public Expression {
public:
    ExponentiationExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_equivalent(TYPE_NUMBER) || not right->type->is_equivalent(TYPE_NUMBER)) {
            internal_error("ExponentiationExpression");
        }
    }

    const Expression *const left;
    const Expression *const right;

    virtual Number eval_number() const { return number_pow(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const { internal_error("ExponentiationExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExponentiationExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class ReferenceExpression: public Expression {
public:
    ReferenceExpression(const Type *type, bool is_readonly): Expression(type, false, is_readonly) {}

    virtual void generate(Emitter &emitter) const { generate_load(emitter); }
    virtual void generate_address_read(Emitter &) const = 0;
    virtual void generate_address_write(Emitter &) const = 0;
    virtual void generate_load(Emitter &) const;
    virtual void generate_store(Emitter &) const;
private:
    ReferenceExpression(const ReferenceExpression &);
    ReferenceExpression &operator=(const ReferenceExpression &);
};

class ArrayReferenceIndexExpression: public ReferenceExpression {
public:
    ArrayReferenceIndexExpression(const Type *type, const ReferenceExpression *array, const Expression *index, bool always_create): ReferenceExpression(type, array->is_readonly), array(array), index(index), always_create(always_create) {}

    const ReferenceExpression *array;
    const Expression *index;
    const bool always_create;

    virtual Number eval_number() const { internal_error("ArrayReferenceIndexExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const;
    virtual void generate_address_write(Emitter &) const;

    virtual std::string text() const { return "ArrayReferenceIndexExpression(" + array->text() + ", " + index->text() + ")"; }
private:
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &);
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &);
};

class ArrayValueIndexExpression: public Expression {
public:
    ArrayValueIndexExpression(const Type *type, const Expression *array, const Expression *index, bool always_create): Expression(type, false), array(array), index(index), always_create(always_create) {}

    const Expression *array;
    const Expression *index;
    const bool always_create;

    virtual Number eval_number() const { internal_error("ArrayValueIndexExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayValueIndexExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ArrayValueIndexExpression(" + array->text() + ", " + index->text() + ")"; }
private:
    ArrayValueIndexExpression(const ArrayValueIndexExpression &);
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &);
};

class DictionaryReferenceIndexExpression: public ReferenceExpression {
public:
    DictionaryReferenceIndexExpression(const Type *type, const ReferenceExpression *dictionary, const Expression *index): ReferenceExpression(type, dictionary->is_readonly), dictionary(dictionary), index(index) {}

    const ReferenceExpression *dictionary;
    const Expression *index;

    virtual Number eval_number() const { internal_error("DictionaryReferenceIndexExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const;
    virtual void generate_address_write(Emitter &) const;

    virtual std::string text() const { return "DictionaryReferenceIndexExpression(" + dictionary->text() + ", " + index->text() + ")"; }
private:
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &);
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &);
};

class DictionaryValueIndexExpression: public Expression {
public:
    DictionaryValueIndexExpression(const Type *type, const Expression *dictionary, const Expression *index): Expression(type, false), dictionary(dictionary), index(index) {}

    const Expression *dictionary;
    const Expression *index;

    virtual Number eval_number() const { internal_error("DictionaryValueIndexExpression"); }
    virtual std::string eval_string() const { internal_error("DictionaryValueIndexExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "DictionaryValueIndexExpression(" + dictionary->text() + ", " + index->text() + ")"; }
private:
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &);
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &);
};

class StringReferenceIndexExpression: public ReferenceExpression {
public:
    StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end);

    const ReferenceExpression *ref;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;
    const FunctionCall *store;

    virtual Number eval_number() const { internal_error("StringReferenceIndexExpression"); }
    virtual std::string eval_string() const { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_address_write(Emitter &) const { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_load(Emitter &) const;
    virtual void generate_store(Emitter &) const;

    virtual std::string text() const { return "StringReferenceIndexExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    StringReferenceIndexExpression(const StringReferenceIndexExpression &);
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &);
};

class StringValueIndexExpression: public Expression {
public:
    StringValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end);

    const Expression *str;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual Number eval_number() const { internal_error("StringValueIndexExpression"); }
    virtual std::string eval_string() const { internal_error("StringValueIndexExpression"); }
    virtual void generate_address_read(Emitter &) const { internal_error("StringValueIndexExpression"); }
    virtual void generate_address_write(Emitter &) const { internal_error("StringValueIndexExpression"); }
    virtual void generate(Emitter &) const;

    virtual std::string text() const { return "StringValueIndexExpression(" + str->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    StringValueIndexExpression(const StringValueIndexExpression &);
    StringValueIndexExpression &operator=(const StringValueIndexExpression &);
};

class ArrayReferenceRangeExpression: public ReferenceExpression {
public:
    ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end);

    const ReferenceExpression *ref;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;
    const FunctionCall *store;

    virtual Number eval_number() const { internal_error("ArrayReferenceRangeExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayReferenceRangeExpression"); }
    virtual void generate_address_read(Emitter &) const { internal_error("StringReferenceRangeExpression"); }
    virtual void generate_address_write(Emitter &) const { internal_error("StringReferenceRangeExpression"); }
    virtual void generate_load(Emitter &) const;
    virtual void generate_store(Emitter &) const;

    virtual std::string text() const { return "ArrayReferenceRangeExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &);
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &);
};

class ArrayValueRangeExpression: public Expression {
public:
    ArrayValueRangeExpression(const Expression *array, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end);

    const Expression *array;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual Number eval_number() const { internal_error("ArrayValueRangeExpression"); }
    virtual std::string eval_string() const { internal_error("ArrayValueRangeExpression"); }
    virtual void generate_address_read(Emitter &) const { internal_error("StringValueRangeExpression"); }
    virtual void generate_address_write(Emitter &) const { internal_error("StringValueRangeExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "ArrayValueRangeExpression(" + array->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    ArrayValueRangeExpression(const ArrayValueRangeExpression &);
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &);
};

class PointerDereferenceExpression: public ReferenceExpression {
public:
    PointerDereferenceExpression(const Type *type, const Expression *ptr): ReferenceExpression(type, false), ptr(ptr) {}

    const Expression *ptr;

    virtual Number eval_number() const { internal_error("PointerDereferenceExpression"); }
    virtual std::string eval_string() const { internal_error("PointerDereferenceExpression"); }
    virtual void generate_address_read(Emitter &emitter) const;
    virtual void generate_address_write(Emitter &emitter) const;

    virtual std::string text() const { return "PointerDereferenceExpression(" + ptr->text() + ")"; }
private:
    PointerDereferenceExpression(const PointerDereferenceExpression &);
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &);
};

class ConstantExpression: public Expression {
public:
    ConstantExpression(const Constant *constant): Expression(constant->type, true, true), constant(constant) {}

    const Constant *constant;

    virtual Number eval_number() const { return constant->value->eval_number(); }
    virtual std::string eval_string() const { return constant->value->eval_string(); }
    virtual void generate(Emitter &emitter) const { constant->value->generate(emitter); }

    virtual std::string text() const { return "ConstantExpression(" + constant->text() + ")"; }
private:
    ConstantExpression(const ConstantExpression &);
    ConstantExpression &operator=(const ConstantExpression &);
};

class VariableExpression: public ReferenceExpression {
public:
    VariableExpression(const Variable *var): ReferenceExpression(var->type, var->is_readonly), var(var) {}

    const Variable *var;

    virtual Number eval_number() const { internal_error("VariableExpression"); }
    virtual std::string eval_string() const { internal_error("VariableExpression"); }
    virtual void generate(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const { var->generate_call(emitter); }
    virtual void generate_address_read(Emitter &emitter) const { var->generate_address(emitter, 0); }
    virtual void generate_address_write(Emitter &emitter) const { var->generate_address(emitter, 0); }

    virtual std::string text() const {
        return "VariableExpression(" + var->text() + ")";
    }
private:
    VariableExpression(const VariableExpression &);
    VariableExpression &operator=(const VariableExpression &);
};

class FunctionCall: public Expression {
public:
    FunctionCall(const Expression *func, const std::vector<const Expression *> &args): Expression(dynamic_cast<const TypeFunction *>(func->type)->returntype, false), func(func), args(args) {}

    const Expression *const func;
    const std::vector<const Expression *> args;

    virtual Number eval_number() const { internal_error("FunctionCall"); }
    virtual std::string eval_string() const { internal_error("FunctionCall"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const;
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);
};

class StatementExpression: public Expression {
public:
    StatementExpression(const Statement *stmt): Expression(TYPE_NOTHING, false), stmt(stmt) {}

    const Statement *const stmt;

    virtual Number eval_number() const { internal_error("StatementExpression"); }
    virtual std::string eval_string() const { internal_error("StatementExpression"); }
    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "StatementExpression"; }
private:
    StatementExpression(const StatementExpression &);
    StatementExpression &operator=(const StatementExpression &);
};

class Statement: public AstNode {
public:
    Statement(int line): line(line) {}
    const int line;

    virtual bool always_returns() const { return false; }

    void generate(Emitter &emitter) const;
    virtual void generate_code(Emitter &emitter) const = 0;
};

class NullStatement: public Statement {
public:
    NullStatement(int line): Statement(line) {}

    virtual void generate_code(Emitter &) const {}

    virtual std::string text() const { return "NullStatement"; }
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(int line, const std::vector<const ReferenceExpression *> vars, const Expression *expr): Statement(line), variables(vars), expr(expr) {
        for (auto v: variables) {
            if (not v->type->is_equivalent(expr->type)) {
                internal_error("AssignmentStatement");
            }
        }
    }

    const std::vector<const ReferenceExpression *> variables;
    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        std::string s = "AssignmentStatement(";
        for (auto v: variables) {
            s += v->text() + ", ";
        }
        return s + expr->text() + ")";
    }

private:
    AssignmentStatement(const AssignmentStatement &);
    AssignmentStatement &operator=(const AssignmentStatement &);
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(int line, const Expression *expr): Statement(line), expr(expr) {}

    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "ExpressionStatement(" + expr->text() + ")";
    }
private:
    ExpressionStatement(const ExpressionStatement &);
    ExpressionStatement &operator=(const ExpressionStatement &);
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(int line, const Expression *expr): Statement(line), expr(expr) {}

    const Expression *const expr;

    virtual bool always_returns() const { return true; }

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "ReturnStatement(" + expr->text() + ")"; }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(int line, const std::vector<const Statement *> &statements): Statement(line), statements(statements) {}

    const std::vector<const Statement *> statements;

    virtual void dumpsubnodes(std::ostream &out, int depth) const;
};

class IfStatement: public Statement {
public:
    IfStatement(int line, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(line), condition_statements(condition_statements), else_statements(else_statements) {}

    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;

    virtual bool always_returns() const;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "IfStatement(" + condition_statements[0].first->text() + ")";
    }
private:
    IfStatement(const IfStatement &);
    IfStatement &operator=(const IfStatement &);
};

class BaseLoopStatement: public CompoundStatement {
public:
    BaseLoopStatement(int line, unsigned int loop_id, const std::vector<const Statement *> &statements): CompoundStatement(line, statements), loop_id(loop_id) {}

    const unsigned int loop_id;
};

class WhileStatement: public BaseLoopStatement {
public:
    WhileStatement(int line, unsigned int loop_id, const Expression *condition, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements), condition(condition) {}

    const Expression *condition;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "WhileStatement(" + condition->text() + ")";
    }
private:
    WhileStatement(const WhileStatement &);
    WhileStatement &operator=(const WhileStatement &);
};

class ForStatement: public BaseLoopStatement {
public:
    ForStatement(int line, unsigned int loop_id, const VariableExpression *var, const Expression *start, const Expression *end, const Expression *step, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements), var(var), start(start), end(end), step(step) {
    }

    const VariableExpression *var;
    const Expression *start;
    const Expression *end;
    const Expression *step;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "ForStatement(" + var->text() + "(" + start->text() + " TO " + end->text() + " STEP " + step->text() + ")";
    }
private:
    ForStatement(const ForStatement &);
    ForStatement &operator=(const ForStatement &);
};

class LoopStatement: public BaseLoopStatement {
public:
    LoopStatement(int line, unsigned int loop_id, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements) {}

    virtual bool always_returns() const;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "LoopStatement()"; }
private:
    LoopStatement(const LoopStatement &);
    LoopStatement &operator=(const LoopStatement &);
};

class RepeatStatement: public BaseLoopStatement {
public:
    RepeatStatement(int line, unsigned int loop_id, const Expression *condition, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements), condition(condition) {}

    const Expression *condition;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "RepeatStatement(" + condition->text() + ")"; }
private:
    RepeatStatement(const RepeatStatement &);
    RepeatStatement &operator=(const RepeatStatement &);
};

class CaseStatement: public Statement {
public:
    class WhenCondition {
    public:
        WhenCondition(const Token &token): token(token) {}
        virtual ~WhenCondition() {}
        const Token token;
        virtual bool overlaps(const WhenCondition *cond) const = 0;
        virtual void generate(Emitter &emitter) const = 0;
    private:
        WhenCondition(const WhenCondition &);
        WhenCondition &operator=(const WhenCondition &);
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(const Token &token, ComparisonExpression::Comparison comp, const Expression *expr): WhenCondition(token), comp(comp), expr(expr) {}
        ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual bool overlaps(const WhenCondition *cond) const;
        virtual void generate(Emitter &emitter) const;
    private:
        ComparisonWhenCondition(const ComparisonWhenCondition &);
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &);
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Token &token, const Expression *low_expr, const Expression *high_expr): WhenCondition(token), low_expr(low_expr), high_expr(high_expr) {}
        const Expression *low_expr;
        const Expression *high_expr;
        virtual bool overlaps(const WhenCondition *cond) const;
        virtual void generate(Emitter &emitter) const;
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(int line, const Expression *expr, const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> &clauses): Statement(line), expr(expr), clauses(clauses) {}

    const Expression *expr;
    const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual bool always_returns() const;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const {
        return "CaseStatement(" + expr->text() + ")";
    }
private:
    CaseStatement(const CaseStatement &);
    CaseStatement &operator=(const CaseStatement &);
};

class ExitStatement: public Statement {
public:
    ExitStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}

    const unsigned int loop_id;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "ExitStatement(...)"; }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class NextStatement: public Statement {
public:
    NextStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}

    const unsigned int loop_id;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "NextStatement(...)"; }
private:
    NextStatement(const NextStatement &);
    NextStatement &operator=(const NextStatement &);
};

class TryStatement: public Statement {
public:
    TryStatement(int line, const std::vector<const Statement *> &statements, const std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> &catches): Statement(line), statements(statements), catches(catches) {}

    const std::vector<const Statement *> statements;
    const std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;

    virtual bool always_returns() const;

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "TryStatement(...)"; }
private:
    TryStatement(const TryStatement &);
    TryStatement &operator=(const TryStatement &);
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(int line, const Exception *exception, const Expression *info): Statement(line), exception(exception), info(info) {}

    const Exception *exception;
    const Expression *info;

    virtual bool always_returns() const { return true; }

    virtual void generate_code(Emitter &emitter) const;

    virtual std::string text() const { return "RaiseStatement(" + exception->text() + ")"; }
private:
    RaiseStatement(const RaiseStatement &);
    RaiseStatement &operator=(const RaiseStatement &);
};

class Function: public Variable {
public:
    Function(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params);

    Frame *frame;
    Scope *scope;
    const std::vector<FunctionParameter *> params;
    mutable unsigned int entry_label;

    std::vector<const Statement *> statements;

    static const Type *makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params);

    virtual void predeclare(Emitter &emitter) const;
    virtual void postdeclare(Emitter &emitter) const;
    virtual void generate_address(Emitter &, int) const { internal_error("Function"); }
    virtual void generate_load(Emitter &) const { internal_error("Function"); }
    virtual void generate_store(Emitter &) const { internal_error("Function"); }
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "Function(" + name + ", " + type->text() + ")"; }
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const std::string &name, const Type *type): Variable(Token(), name, type, true), name_index(-1) {}
    mutable int name_index;

    virtual void predeclare(Emitter &emitter) const;
    virtual void generate_address(Emitter &, int) const { internal_error("PredefinedFunction"); }
    virtual void generate_load(Emitter &) const { internal_error("PredefinedFunction"); }
    virtual void generate_store(Emitter &) const { internal_error("PredefinedFunction"); }
    virtual void generate_call(Emitter &emitter) const;

    virtual std::string text() const { return "PredefinedFunction(" + name + ", " + type->text() + ")"; }
};

class ExternalFunction: public Function {
public:
    ExternalFunction(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params): Function(declaration, name, returntype, outer, parent, params), library_name(), param_types(), external_index(-1) {}

    std::string library_name;
    std::map<std::string, std::string> param_types;
    mutable int external_index;

    virtual void predeclare(Emitter &) const;
    virtual void postdeclare(Emitter &) const {} // TODO: Stub this out so we don't inherit the one from Function. Fix Function hierarchy.
    virtual void generate_call(Emitter &emitter) const;

private:
    ExternalFunction(const ExternalFunction &);
    ExternalFunction &operator=(const ExternalFunction &);
};

class Module: public Name {
public:
    Module(const Token &declaration, Scope *scope, const std::string &name): Name(declaration, name, TYPE_MODULE), scope(new Scope(scope, scope->frame)) {}

    Scope *scope;

    virtual void predeclare(Emitter &emitter) const;

    virtual std::string text() const { return "Module"; }
private:
    Module(const Module &);
    Module &operator=(const Module &);
};

class Program: public AstNode {
public:
    Program();

    Frame *frame;
    Scope *scope;
    std::vector<const Statement *> statements;

    virtual void generate(Emitter &emitter) const;

    virtual std::string text() const { return "Program"; }
    virtual void dumpsubnodes(std::ostream &out, int depth) const;
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

#endif
