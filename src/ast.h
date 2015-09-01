#ifndef AST_H
#define AST_H

#include <iso646.h>
#include <limits.h>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <minijson_writer.hpp>

#include "bytecode.h"
#include "number.h"
#include "token.h"
#include "util.h"

class Analyzer;

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
    const Slot getSlot(size_t slot);
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
    void addName(const Token &token, const std::string &name, Name *ref, bool init_referenced = false, bool allow_shadow = false);
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

    virtual void generate_export(Emitter &emitter, const std::string &name) const = 0;
private:
    Name(const Name &);
    Name &operator=(const Name &);
};

class Type: public Name {
public:
    Type(const Token &declaration, const std::string &name): Name(declaration, name, nullptr), methods() {}

    std::map<std::string, Variable *> methods;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual bool is_equivalent(const Type *rhs) const { return this == rhs; }
    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const = 0;
    virtual void get_type_references(std::set<const Type *> &) const {}
    virtual std::string serialize(const Expression *value) const = 0;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const = 0;
    virtual void debuginfo(minijson::object_writer &out) const = 0;
};

class TypeNothing: public Type {
public:
    TypeNothing(): Type(Token(), "Nothing") {}
    virtual void generate_load(Emitter &) const override { internal_error("TypeNothing"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeNothing"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeNothing"); }
    virtual std::string get_type_descriptor(Emitter &) const override { return "Z"; }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeNothing"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeNothing"); }
    virtual void debuginfo(minijson::object_writer &) const { internal_error("TypeNothing"); }

    virtual std::string text() const override { return "TypeNothing"; }
};

extern TypeNothing *TYPE_NOTHING;

class TypeBoolean: public Type {
public:
    TypeBoolean(): Type(Token(), "Boolean") {}
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "B"; }
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeBoolean"; }
};

extern TypeBoolean *TYPE_BOOLEAN;

class TypeNumber: public Type {
public:
    TypeNumber(const Token &declaration): Type(declaration, "Number") {}
    TypeNumber(const Token &declaration, const std::string &name): Type(declaration, name) {}
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "N"; }
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeNumber"; }
};

extern TypeNumber *TYPE_NUMBER;

class TypeString: public Type {
public:
    TypeString(): Type(Token(), "String") {}
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "S"; }
    static std::string serialize(const std::string &value);
    virtual std::string serialize(const Expression *value) const override;
    static std::string deserialize_string(const Bytecode::Bytes &value, int &i);
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeString"; }
};

extern TypeString *TYPE_STRING;

class TypeBytes: public TypeString {
public:
    TypeBytes(): TypeString() {}

    virtual std::string get_type_descriptor(Emitter &) const override { return "Y"; }
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeBytes"; }
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

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual std::string serialize(const Expression *) const override { internal_error("TypeFunction"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeFunction"); }
    virtual void debuginfo(minijson::object_writer &out) const;

    const Type *returntype;
    const std::vector<const ParameterType *> params;

    virtual std::string text() const override { return "TypeFunction(...)"; }
private:
    TypeFunction(const TypeFunction &);
    TypeFunction &operator=(const TypeFunction &);
};

class TypeArray: public Type {
public:
    TypeArray(const Token &declaration, const Type *elementtype);
    const Type *elementtype;

    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_equivalent(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeArray(" + (elementtype != nullptr ? elementtype->text() : "any") + ")"; }
private:
    TypeArray(const TypeArray &);
    TypeArray &operator=(const TypeArray &);
};

extern TypeArray *TYPE_ARRAY_NUMBER;
extern TypeArray *TYPE_ARRAY_STRING;

class TypeDictionary: public Type {
public:
    TypeDictionary(const Token &declaration, const Type *elementtype);
    const Type *elementtype;

    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_equivalent(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeDictionary(" + (elementtype != nullptr ? elementtype->text() : "any") + ")"; }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
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
    TypeRecord(const Token &declaration, const std::string &name, const std::vector<Field> &fields): Type(declaration, name), fields(fields), field_names(make_field_names(fields)), predeclared(false), postdeclared(false) {}
    const std::vector<Field> fields;
    const std::map<std::string, size_t> field_names;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual bool is_equivalent(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override;
private:
    mutable bool predeclared;
    mutable bool postdeclared;
    static std::map<std::string, size_t> make_field_names(const std::vector<Field> &fields) {
        std::map<std::string, size_t> r;
        size_t i = 0;
        for (auto f: fields) {
            r[f.name.text] = i;
            i++;
        }
        return r;
    }
};

class TypeForwardRecord: public TypeRecord {
public:
    TypeForwardRecord(const Token &declaration): TypeRecord(declaration, "forward", std::vector<Field>()) {}
};

class TypePointer: public Type {
public:
    TypePointer(const Token &declaration, const TypeRecord *reftype);

    const TypeRecord *reftype;

    virtual bool is_equivalent(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypePointer(" + (reftype != nullptr ? reftype->text() : "any") + ")"; }
private:
    TypePointer(const TypePointer &);
    TypePointer &operator=(const TypePointer &);
};

extern TypePointer *TYPE_POINTER;

class TypeValidPointer: public TypePointer {
public:
    TypeValidPointer(const TypePointer *ptrtype): TypePointer(Token(), ptrtype->reftype) {}
};

class TypeFunctionPointer: public Type {
public:
    TypeFunctionPointer(const Token &declaration, const TypeFunction *functype);

    const TypeFunction *functype;

    virtual bool is_equivalent(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeFunctionPointer(" + functype->text() + ")"; }
private:
    TypeFunctionPointer(const TypeFunctionPointer &);
    TypeFunctionPointer &operator=(const TypeFunctionPointer &);
};

class TypeEnum: public TypeNumber {
public:
    TypeEnum(const Token &declaration, const std::string &name, const std::map<std::string, int> &names, Analyzer *analyzer);
    const std::map<std::string, int> names;

    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeEnum(...)"; }
};

class TypeModule: public Type {
public:
    TypeModule(): Type(Token(), "module") {}

    virtual void generate_load(Emitter &) const override { internal_error("TypeModule"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeModule"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeModule"); }
    virtual std::string get_type_descriptor(Emitter &) const override { internal_error("TypeModule"); }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeModule"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeModule"); }
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeModule(...)"; }
};

extern TypeModule *TYPE_MODULE;

class TypeException: public Type {
public:
    TypeException(): Type(Token(), "Exception") {}

    virtual void generate_load(Emitter &) const override { internal_error("TypeException"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeException"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeException"); }
    virtual std::string get_type_descriptor(Emitter &) const override { return "X"; }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeException"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeException"); }
    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "TypeException"; }
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
    virtual void generate_export(Emitter &, const std::string &) const override { internal_error("Variable"); }

    virtual std::string text() const override { return "Variable(" + name + ", " + type->text() + ")"; }
};

class ModuleVariable: public Variable {
public:
    ModuleVariable(const std::string &module, const std::string &name, const Type *type, int index): Variable(Token(), name, type, false), module(module), index(index) {}
    const std::string module;
    const int index;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &emitter, int enclosing) const override;

    virtual std::string text() const override { return "ModuleVariable(" + module + "." + name + ")"; }
};

class GlobalVariable: public Variable {
public:
    GlobalVariable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Variable(declaration, name, type, is_readonly), index(-1) {}
    mutable int index;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &emitter, int enclosing) const override;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual std::string text() const override { return "GlobalVariable(" + name + ", " + type->text() + ")"; }
};

class LocalVariable: public Variable {
public:
    LocalVariable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Variable(declaration, name, type, is_readonly), index(-1) {}
    int index;

    virtual void predeclare(Emitter &) const override { internal_error("LocalVariable"); }
    virtual void predeclare(Emitter &emitter, int slot);
    virtual void generate_address(Emitter &emitter, int enclosing) const override;

    virtual std::string text() const override { return "LocalVariable(" + name + ", " + type->text() + ")"; }
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class FunctionParameter: public LocalVariable {
public:
    FunctionParameter(const Token &declaration, const std::string &name, const Type *type, ParameterType::Mode mode, const Expression *default_value): LocalVariable(declaration, name, type, mode == ParameterType::IN), mode(mode), default_value(default_value) {}
    ParameterType::Mode mode;
    const Expression *default_value;

    virtual void generate_address(Emitter &emitter, int enclosing) const override;

    virtual std::string text() const override { return "FunctionParameter(" + name + ", " + type->text() + ")"; }
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class Exception: public Name {
public:
    Exception(const Token &declaration, const std::string &name): Name(declaration, name, TYPE_EXCEPTION) {}
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual std::string text() const override { return "Exception(" + name + ")"; }
private:
    Exception(const Exception &);
    Exception &operator=(const Exception &);
};

class Expression: public AstNode {
public:
    Expression(const Type *type, bool is_constant, bool is_readonly = true): type(type), is_constant(is_constant), is_readonly(is_readonly) {}

    virtual bool eval_boolean() const = 0;
    virtual Number eval_number() const = 0;
    virtual std::string eval_string() const = 0;
    void generate(Emitter &emitter) const;
    virtual void generate_expr(Emitter &emitter) const = 0;
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

    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual std::string text() const override { return "Constant(" + name + ", " + value->text() + ")"; }
private:
    Constant(const Constant &);
    Constant &operator=(const Constant &);
};

class ConstantBooleanExpression: public Expression {
public:
    ConstantBooleanExpression(bool value): Expression(TYPE_BOOLEAN, true), value(value) {}

    const bool value;

    virtual bool eval_boolean() const override { return value; }
    virtual Number eval_number() const override { internal_error("ConstantBooleanExpression"); }
    virtual std::string eval_string() const override { internal_error("ConstantBooleanExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantNumberExpression: public Expression {
public:
    ConstantNumberExpression(Number value): Expression(TYPE_NUMBER, true), value(value) {}

    const Number value;

    virtual bool eval_boolean() const override { internal_error("ConstantNumberExpression"); }
    virtual Number eval_number() const override { return value; }
    virtual std::string eval_string() const override { internal_error("ConstantNumberExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantStringExpression: public Expression {
public:
    ConstantStringExpression(const std::string &value): Expression(TYPE_STRING, true), value(value) {}

    const std::string value;

    virtual bool eval_boolean() const override { internal_error("ConstantStringExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantStringExpression"); }
    virtual std::string eval_string() const override { return value; }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const TypeEnum *type, int value): Expression(type, true), value(value) {}

    const int value;

    virtual bool eval_boolean() const override { internal_error("ConstantEnumExpression"); }
    virtual Number eval_number() const override { return number_from_uint32(value); }
    virtual std::string eval_string() const override { internal_error("ConstantEnumExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(): Expression(new TypePointer(Token(), nullptr), true) {}

    virtual bool eval_boolean() const override { internal_error("ConstantNilExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantNilExpression"); }
    virtual std::string eval_string() const override { internal_error("ConstantNilExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ConstantNilExpression"; }
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Type *elementtype, const std::vector<const Expression *> &elements): Expression(new TypeArray(Token(), elementtype), all_constant(elements)), elementtype(elementtype), elements(elements) {}

    const Type *elementtype;
    const std::vector<const Expression *> elements;

    virtual bool eval_boolean() const override { internal_error("ArrayLiteralExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayLiteralExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayLiteralExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "ArrayLiteralExpression(...)"; }
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

    virtual bool eval_boolean() const override { internal_error("DictionaryLiteralExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryLiteralExpression"); }
    virtual std::string eval_string() const override { internal_error("DictionaryLiteralExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "DictionaryLiteralExpression(...)"; }
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

    virtual bool eval_boolean() const override { internal_error("RecordLiteralExpression"); }
    virtual Number eval_number() const override { internal_error("RecordLiteralExpression"); }
    virtual std::string eval_string() const override { internal_error("RecordLiteralExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "RecordLiteralExpression(...)"; }
private:
    RecordLiteralExpression(const RecordLiteralExpression &);
    RecordLiteralExpression &operator=(const RecordLiteralExpression &);

    static bool all_constant(const std::vector<const Expression *> &values);
};

class NewRecordExpression: public Expression {
public:
    NewRecordExpression(const TypeRecord *reftype): Expression(new TypePointer(Token(), reftype), false), fields(reftype->fields.size()) {}

    const size_t fields;

    virtual bool eval_boolean() const override { internal_error("NewRecordExpression"); }
    virtual Number eval_number() const override { internal_error("NewRecordExpression"); }
    virtual std::string eval_string() const override { internal_error("NewRecordExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "NewRecordExpression(" + type->text() + ")"; }
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

    virtual bool eval_boolean() const override { internal_error("UnaryMinusExpression"); }
    virtual Number eval_number() const override { return number_negate(value->eval_number()); }
    virtual std::string eval_string() const override { internal_error("UnaryMinusExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { return not value->eval_boolean(); }
    virtual Number eval_number() const override { internal_error("LogicalNotExpression"); }
    virtual std::string eval_string() const override { internal_error("LogicalNotExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("ConditionalExpression"); }
    virtual Number eval_number() const override { internal_error("ConditionalExpression"); }
    virtual std::string eval_string() const override { internal_error("ConditionalExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { return left->eval_boolean() || right->eval_boolean(); }
    virtual Number eval_number() const override { internal_error("DisjunctionExpression"); }
    virtual std::string eval_string() const override { internal_error("DisjunctionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { return left->eval_boolean() && right->eval_boolean(); }
    virtual Number eval_number() const override { internal_error("ConjunctionExpression"); }
    virtual std::string eval_string() const override { internal_error("ConjunctionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("ArrayInExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayInExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayInExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ArrayInExpression(" + left->text() + ", " + right->text() + ")"; }
private:
    ArrayInExpression(const ArrayInExpression &);
    ArrayInExpression &operator=(const ArrayInExpression &);
};

class DictionaryInExpression: public Expression {
public:
    DictionaryInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}

    const Expression *left;
    const Expression *right;

    virtual bool eval_boolean() const override { internal_error("DictionaryInExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryInExpression"); }
    virtual std::string eval_string() const override { internal_error("DictionaryInExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "DictionaryInExpression(" + left->text() + ", " + right->text() + ")"; }
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

    virtual void generate_expr(Emitter &emitter) const override;
    virtual void generate_comparison_opcode(Emitter &emitter) const = 0;
private:
    ComparisonExpression(const ComparisonExpression &);
    ComparisonExpression &operator=(const ComparisonExpression &);
};

class ChainedComparisonExpression: public Expression {
public:
    ChainedComparisonExpression(const std::vector<const ComparisonExpression *> &comps): Expression(TYPE_BOOLEAN, false), comps(comps) {}

    const std::vector<const ComparisonExpression *> comps;

    virtual bool eval_boolean() const override { internal_error("ChainedComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("ChainedComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("ChainedComparisonExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ChainedComparisonExpression(...)"; }
private:
    ChainedComparisonExpression(const ChainedComparisonExpression &);
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &);
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    BooleanComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("BooleanComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("BooleanComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("BooleanComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "BooleanComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("NumericComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("NumericComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("NumericComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "NumericComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("StringComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("StringComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("StringComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "StringComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("ArrayComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ArrayComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("DictionaryComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("DictionaryComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "DictionaryComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    PointerComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("PointerComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("PointerComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("PointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "PointerComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    ValidPointerExpression(const Variable *var, const Expression *ptr): PointerComparisonExpression(ptr, new ConstantNilExpression(), ComparisonExpression::NE), var(var) {}

    const Variable *var;

    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ValidPointerExpression(" + left->text() + ")";
    }
private:
    ValidPointerExpression(const ValidPointerExpression &);
    ValidPointerExpression &operator=(const ValidPointerExpression &);
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    FunctionPointerComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}

    virtual bool eval_boolean() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "FunctionPointerComparisonExpression(" + left->text() + std::to_string(comp) + right->text() + ")";
    }
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

    virtual bool eval_boolean() const override { internal_error("AdditionExpression"); }
    virtual Number eval_number() const override { return number_add(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const override { internal_error("AdditionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("SubtractionExpression"); }
    virtual Number eval_number() const override { return number_subtract(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const override { internal_error("SubtractionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("MultiplicationExpression"); }
    virtual Number eval_number() const override { return number_multiply(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const override { internal_error("MultiplicationExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("DivisionExpression"); }
    virtual Number eval_number() const override { return number_divide(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const override { internal_error("DivisionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("ModuloExpression"); }
    virtual Number eval_number() const override { return number_modulo(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const override { internal_error("ModuloExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool eval_boolean() const override { internal_error("ExponentiationExpression"); }
    virtual Number eval_number() const override { return number_pow(left->eval_number(), right->eval_number()); }
    virtual std::string eval_string() const override { internal_error("ExponentiationExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ExponentiationExpression(" + left->text() + "," + right->text() + ")";
    }
private:
    ExponentiationExpression(const ExponentiationExpression &);
    ExponentiationExpression &operator=(const ExponentiationExpression &);
};

class ReferenceExpression: public Expression {
public:
    ReferenceExpression(const Type *type, bool is_readonly): Expression(type, false, is_readonly) {}

    virtual void generate_expr(Emitter &emitter) const override { generate_load(emitter); }
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

    virtual bool eval_boolean() const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const override;
    virtual void generate_address_write(Emitter &) const override;

    virtual std::string text() const override { return "ArrayReferenceIndexExpression(" + array->text() + ", " + index->text() + ")"; }
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

    virtual bool eval_boolean() const override { internal_error("ArrayValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayValueIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayValueIndexExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ArrayValueIndexExpression(" + array->text() + ", " + index->text() + ")"; }
private:
    ArrayValueIndexExpression(const ArrayValueIndexExpression &);
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &);
};

class DictionaryReferenceIndexExpression: public ReferenceExpression {
public:
    DictionaryReferenceIndexExpression(const Type *type, const ReferenceExpression *dictionary, const Expression *index): ReferenceExpression(type, dictionary->is_readonly), dictionary(dictionary), index(index) {}

    const ReferenceExpression *dictionary;
    const Expression *index;

    virtual bool eval_boolean() const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const override;
    virtual void generate_address_write(Emitter &) const override;

    virtual std::string text() const override { return "DictionaryReferenceIndexExpression(" + dictionary->text() + ", " + index->text() + ")"; }
private:
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &);
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &);
};

class DictionaryValueIndexExpression: public Expression {
public:
    DictionaryValueIndexExpression(const Type *type, const Expression *dictionary, const Expression *index): Expression(type, false), dictionary(dictionary), index(index) {}

    const Expression *dictionary;
    const Expression *index;

    virtual bool eval_boolean() const override { internal_error("DictionaryValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryValueIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("DictionaryValueIndexExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "DictionaryValueIndexExpression(" + dictionary->text() + ", " + index->text() + ")"; }
private:
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &);
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &);
};

class StringReferenceIndexExpression: public ReferenceExpression {
public:
    StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);

    const ReferenceExpression *ref;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;
    const FunctionCall *store;

    virtual bool eval_boolean() const override { internal_error("StringReferenceIndexExpression"); }
    virtual Number eval_number() const override { internal_error("StringReferenceIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "StringReferenceIndexExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    StringReferenceIndexExpression(const StringReferenceIndexExpression &);
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &);
};

class StringValueIndexExpression: public Expression {
public:
    StringValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);

    const Expression *str;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual bool eval_boolean() const override { internal_error("StringValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("StringValueIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("StringValueIndexExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "StringValueIndexExpression(" + str->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    StringValueIndexExpression(const StringValueIndexExpression &);
    StringValueIndexExpression &operator=(const StringValueIndexExpression &);
};

class ArrayReferenceRangeExpression: public ReferenceExpression {
public:
    ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);

    const ReferenceExpression *ref;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;
    const FunctionCall *store;

    virtual bool eval_boolean() const override { internal_error("ArrayReferenceRangeExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayReferenceRangeExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayReferenceRangeExpression"); }
    virtual void generate_address_read(Emitter &) const override { internal_error("StringReferenceRangeExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("StringReferenceRangeExpression"); }
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "ArrayReferenceRangeExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &);
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &);
};

class ArrayValueRangeExpression: public Expression {
public:
    ArrayValueRangeExpression(const Expression *array, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);

    const Expression *array;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual bool eval_boolean() const override { internal_error("ArrayValueRangeExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayValueRangeExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayValueRangeExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ArrayValueRangeExpression(" + array->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    ArrayValueRangeExpression(const ArrayValueRangeExpression &);
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &);
};

class PointerDereferenceExpression: public ReferenceExpression {
public:
    PointerDereferenceExpression(const Type *type, const Expression *ptr): ReferenceExpression(type, false), ptr(ptr) {}

    const Expression *ptr;

    virtual bool eval_boolean() const override { internal_error("PointerDereferenceExpression"); }
    virtual Number eval_number() const override { internal_error("PointerDereferenceExpression"); }
    virtual std::string eval_string() const override { internal_error("PointerDereferenceExpression"); }
    virtual void generate_address_read(Emitter &emitter) const override;
    virtual void generate_address_write(Emitter &emitter) const override;

    virtual std::string text() const override { return "PointerDereferenceExpression(" + ptr->text() + ")"; }
private:
    PointerDereferenceExpression(const PointerDereferenceExpression &);
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &);
};

class ConstantExpression: public Expression {
public:
    ConstantExpression(const Constant *constant): Expression(constant->type, true, true), constant(constant) {}

    const Constant *constant;

    virtual bool eval_boolean() const override { return constant->value->eval_boolean(); }
    virtual Number eval_number() const override { return constant->value->eval_number(); }
    virtual std::string eval_string() const override { return constant->value->eval_string(); }
    virtual void generate_expr(Emitter &emitter) const override { constant->value->generate(emitter); }

    virtual std::string text() const override { return "ConstantExpression(" + constant->text() + ")"; }
private:
    ConstantExpression(const ConstantExpression &);
    ConstantExpression &operator=(const ConstantExpression &);
};

class VariableExpression: public ReferenceExpression {
public:
    VariableExpression(const Variable *var): ReferenceExpression(var->type, var->is_readonly), var(var) {}

    const Variable *var;

    virtual bool eval_boolean() const override { internal_error("VariableExpression"); }
    virtual Number eval_number() const override { internal_error("VariableExpression"); }
    virtual std::string eval_string() const override { internal_error("VariableExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override { var->generate_call(emitter); }
    virtual void generate_address_read(Emitter &emitter) const override { var->generate_address(emitter, 0); }
    virtual void generate_address_write(Emitter &emitter) const override { var->generate_address(emitter, 0); }

    virtual std::string text() const override {
        return "VariableExpression(" + var->text() + ")";
    }
private:
    VariableExpression(const VariableExpression &);
    VariableExpression &operator=(const VariableExpression &);
};

class FunctionCall: public Expression {
public:
    FunctionCall(const Expression *func, const std::vector<const Expression *> &args): Expression(get_expr_type(func), false), func(func), args(args) {}

    const Expression *const func;
    const std::vector<const Expression *> args;

    virtual bool eval_boolean() const override { internal_error("FunctionCall"); }
    virtual Number eval_number() const override { internal_error("FunctionCall"); }
    virtual std::string eval_string() const override { internal_error("FunctionCall"); }
    virtual void generate_expr(Emitter &emitter) const override;
    void generate_parameters(Emitter &emitter) const;
    bool all_in_parameters() const;

    virtual std::string text() const override;
private:
    FunctionCall(const FunctionCall &);
    FunctionCall &operator=(const FunctionCall &);

    static const Type *get_expr_type(const Expression *func) {
        const TypeFunction *f = dynamic_cast<const TypeFunction *>(func->type);
        if (f != nullptr) {
            return f->returntype;
        }
        const TypeFunctionPointer *p = dynamic_cast<const TypeFunctionPointer *>(func->type);
        if (p != nullptr) {
            return p->functype->returntype;
        }
        internal_error("not function or functionpointer");
    }
};

class StatementExpression: public Expression {
public:
    StatementExpression(const Statement *stmt): Expression(TYPE_NOTHING, false), stmt(stmt) {}

    const Statement *const stmt;

    virtual bool eval_boolean() const override { internal_error("StatementExpression"); }
    virtual Number eval_number() const override { internal_error("StatementExpression"); }
    virtual std::string eval_string() const override { internal_error("StatementExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "StatementExpression"; }
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

class CompoundStatement: public Statement {
public:
    CompoundStatement(int line, const std::vector<const Statement *> &statements): Statement(line), statements(statements) {}

    const std::vector<const Statement *> statements;

    virtual void dumpsubnodes(std::ostream &out, int depth) const override;
};

class NullStatement: public Statement {
public:
    NullStatement(int line): Statement(line) {}

    virtual void generate_code(Emitter &) const override {}

    virtual std::string text() const override { return "NullStatement"; }
};

class AssertStatement: public CompoundStatement {
public:
    AssertStatement(int line, const std::vector<const Statement *> &statements, const Expression *expr, const std::string &source): CompoundStatement(line, statements), expr(expr), source(source) {}

    const Expression *const expr;
    const std::string source;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "AssertStatement(" + expr->text() + ")"; }

private:
    AssertStatement(const AssertStatement &);
    AssertStatement &operator=(const AssertStatement &);
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(int line, const std::vector<const ReferenceExpression *> &vars, const Expression *expr): Statement(line), variables(vars), expr(expr) {
        for (auto v: variables) {
            if (not v->type->is_equivalent(expr->type)) {
                internal_error("AssignmentStatement");
            }
        }
    }

    const std::vector<const ReferenceExpression *> variables;
    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual bool always_returns() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "ReturnStatement(" + expr->text() + ")"; }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(int line, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(line), condition_statements(condition_statements), else_statements(else_statements) {}

    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "WhileStatement(" + condition->text() + ")";
    }
private:
    WhileStatement(const WhileStatement &);
    WhileStatement &operator=(const WhileStatement &);
};

class ForStatement: public BaseLoopStatement {
public:
    ForStatement(int line, unsigned int loop_id, const VariableExpression *var, const Expression *start, const Expression *end, const Expression *step, const VariableExpression *bound, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements), var(var), start(start), end(end), step(step), bound(bound) {
    }

    const VariableExpression *var;
    const Expression *start;
    const Expression *end;
    const Expression *step;
    const VariableExpression *bound;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ForStatement(" + var->text() + "(" + start->text() + " TO " + end->text() + " STEP " + step->text() + ")";
    }
private:
    ForStatement(const ForStatement &);
    ForStatement &operator=(const ForStatement &);
};

class ForeachStatement: public BaseLoopStatement {
public:
    ForeachStatement(int line, unsigned int loop_id, const VariableExpression *var, const Expression *array, const VariableExpression *index, const VariableExpression *bound, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements), var(var), array(array), index(index), bound(bound) {
    }

    const VariableExpression *var;
    const Expression *array;
    const VariableExpression *index;
    const VariableExpression *bound;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ForeachStatement(" + var->text() + "(" + array->text() + ")";
    }
private:
    ForeachStatement(const ForeachStatement &);
    ForeachStatement &operator=(const ForeachStatement &);
};

class LoopStatement: public BaseLoopStatement {
public:
    LoopStatement(int line, unsigned int loop_id, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements) {}

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "LoopStatement()"; }
private:
    LoopStatement(const LoopStatement &);
    LoopStatement &operator=(const LoopStatement &);
};

class RepeatStatement: public BaseLoopStatement {
public:
    RepeatStatement(int line, unsigned int loop_id, const Expression *condition, const std::vector<const Statement *> &statements): BaseLoopStatement(line, loop_id, statements), condition(condition) {}

    const Expression *condition;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "RepeatStatement(" + condition->text() + ")"; }
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
        virtual bool overlaps(const WhenCondition *cond) const override;
        virtual void generate(Emitter &emitter) const override;
    private:
        ComparisonWhenCondition(const ComparisonWhenCondition &);
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &);
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Token &token, const Expression *low_expr, const Expression *high_expr): WhenCondition(token), low_expr(low_expr), high_expr(high_expr) {}
        const Expression *low_expr;
        const Expression *high_expr;
        virtual bool overlaps(const WhenCondition *cond) const override;
        virtual void generate(Emitter &emitter) const override;
    private:
        RangeWhenCondition(const RangeWhenCondition &);
        RangeWhenCondition &operator=(const RangeWhenCondition &);
    };
    CaseStatement(int line, const Expression *expr, const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> &clauses): Statement(line), expr(expr), clauses(clauses) {}

    const Expression *expr;
    const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
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

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "ExitStatement(...)"; }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class NextStatement: public Statement {
public:
    NextStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}

    const unsigned int loop_id;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "NextStatement(...)"; }
private:
    NextStatement(const NextStatement &);
    NextStatement &operator=(const NextStatement &);
};

class TryStatement: public Statement {
public:
    TryStatement(int line, const std::vector<const Statement *> &statements, const std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> &catches): Statement(line), statements(statements), catches(catches) {}

    const std::vector<const Statement *> statements;
    const std::vector<std::pair<std::vector<const Exception *>, std::vector<const Statement *>>> catches;

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "TryStatement(...)"; }
private:
    TryStatement(const TryStatement &);
    TryStatement &operator=(const TryStatement &);
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(int line, const Exception *exception, const Expression *info): Statement(line), exception(exception), info(info) {}

    const Exception *exception;
    const Expression *info;

    virtual bool always_returns() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "RaiseStatement(" + exception->text() + ")"; }
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

    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &, int) const override {}
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override { internal_error("Function"); }
    virtual void generate_call(Emitter &emitter) const override;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "Function(" + name + ", " + type->text() + ")"; }
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const std::string &name, const Type *type): Variable(Token(), name, type, true), name_index(-1) {}
    mutable int name_index;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &, int) const override { internal_error("PredefinedFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Emitter &emitter) const override;

    virtual std::string text() const override { return "PredefinedFunction(" + name + ", " + type->text() + ")"; }
};

class ModuleFunction: public Variable {
public:
    ModuleFunction(const std::string &module, const std::string &name, const Type *type, unsigned int entry): Variable(Token(), name, type, true), module(module), entry(entry) {}
    const std::string module;
    const unsigned int entry;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &, int) const override { internal_error("ModuleFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_call(Emitter &emitter) const override;

    virtual std::string text() const override { return "ModuleFunction(" + module + "." + name + ", " + type->text() + ")"; }
};

class ExternalFunction: public Function {
public:
    ExternalFunction(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params): Function(declaration, name, returntype, outer, parent, params), library_name(), param_types(), external_index(-1) {}

    std::string library_name;
    std::map<std::string, std::string> param_types;
    mutable int external_index;

    virtual void predeclare(Emitter &) const override;
    virtual void postdeclare(Emitter &) const override;
    virtual void generate_call(Emitter &emitter) const override;

private:
    ExternalFunction(const ExternalFunction &);
    ExternalFunction &operator=(const ExternalFunction &);
};

class Module: public Name {
public:
    Module(const Token &declaration, Scope *scope, const std::string &name): Name(declaration, name, TYPE_MODULE), scope(new Scope(scope, scope->frame)) {}

    Scope *scope;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_export(Emitter &, const std::string &) const override { internal_error("can't export module"); }

    virtual std::string text() const override { return "Module"; }
private:
    Module(const Module &);
    Module &operator=(const Module &);
};

class Program: public AstNode {
public:
    Program(const std::string &source_path, const std::string &source_hash);

    const std::string source_path;
    const std::string source_hash;
    Frame *frame;
    Scope *scope;
    std::vector<const Statement *> statements;
    std::map<std::string, const Name *> exports;

    virtual void generate(Emitter &emitter) const;

    virtual void debuginfo(minijson::object_writer &out) const;

    virtual std::string text() const override { return "Program"; }
    virtual void dumpsubnodes(std::ostream &out, int depth) const override;
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

#endif
