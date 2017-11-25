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
#include "sql.h"
#include "token.h"
#include "util.h"

class Analyzer;

// Compiler
class Emitter;

namespace ast {

class IAstVisitor {
public:
    virtual ~IAstVisitor() {}
    virtual void visit(const class TypeNothing *node) = 0;
    virtual void visit(const class TypeDummy *node) = 0;
    virtual void visit(const class TypeBoolean *node) = 0;
    virtual void visit(const class TypeNumber *node) = 0;
    virtual void visit(const class TypeString *node) = 0;
    virtual void visit(const class TypeBytes *node) = 0;
    virtual void visit(const class TypeFunction *node) = 0;
    virtual void visit(const class TypeArray *node) = 0;
    virtual void visit(const class TypeDictionary *node) = 0;
    virtual void visit(const class TypeRecord *node) = 0;
    virtual void visit(const class TypeClass *node) = 0;
    virtual void visit(const class TypePointer *node) = 0;
    virtual void visit(const class TypeInterfacePointer *node) = 0;
    virtual void visit(const class TypeFunctionPointer *node) = 0;
    virtual void visit(const class TypeEnum *node) = 0;
    virtual void visit(const class TypeModule *node) = 0;
    virtual void visit(const class TypeException *node) = 0;
    virtual void visit(const class TypeInterface *node) = 0;
    virtual void visit(const class LoopLabel *node) = 0;
    virtual void visit(const class PredefinedVariable *node) = 0;
    virtual void visit(const class ModuleVariable *node) = 0;
    virtual void visit(const class GlobalVariable *node) = 0;
    virtual void visit(const class ExternalGlobalVariable *node) = 0;
    virtual void visit(const class LocalVariable *node) = 0;
    virtual void visit(const class FunctionParameter *node) = 0;
    virtual void visit(const class Exception *node) = 0;
    virtual void visit(const class Interface *node) = 0;
    virtual void visit(const class Constant *node) = 0;
    virtual void visit(const class ConstantBooleanExpression *node) = 0;
    virtual void visit(const class ConstantNumberExpression *node) = 0;
    virtual void visit(const class ConstantStringExpression *node) = 0;
    virtual void visit(const class ConstantBytesExpression *node) = 0;
    virtual void visit(const class ConstantEnumExpression *node) = 0;
    virtual void visit(const class ConstantNilExpression *node) = 0;
    virtual void visit(const class ConstantNowhereExpression *node) = 0;
    virtual void visit(const class ArrayLiteralExpression *node) = 0;
    virtual void visit(const class DictionaryLiteralExpression *node) = 0;
    virtual void visit(const class RecordLiteralExpression *node) = 0;
    virtual void visit(const class ClassLiteralExpression *node) = 0;
    virtual void visit(const class NewClassExpression *node) = 0;
    virtual void visit(const class UnaryMinusExpression *node) = 0;
    virtual void visit(const class LogicalNotExpression *node) = 0;
    virtual void visit(const class ConditionalExpression *node) = 0;
    virtual void visit(const class TryExpression *node) = 0;
    virtual void visit(const class DisjunctionExpression *node) = 0;
    virtual void visit(const class ConjunctionExpression *node) = 0;
    virtual void visit(const class ArrayInExpression *node) = 0;
    virtual void visit(const class DictionaryInExpression *node) = 0;
    virtual void visit(const class ChainedComparisonExpression *node) = 0;
    virtual void visit(const class BooleanComparisonExpression *node) = 0;
    virtual void visit(const class NumericComparisonExpression *node) = 0;
    virtual void visit(const class EnumComparisonExpression *node) = 0;
    virtual void visit(const class StringComparisonExpression *node) = 0;
    virtual void visit(const class BytesComparisonExpression *node) = 0;
    virtual void visit(const class ArrayComparisonExpression *node) = 0;
    virtual void visit(const class DictionaryComparisonExpression *node) = 0;
    virtual void visit(const class RecordComparisonExpression *node) = 0;
    virtual void visit(const class PointerComparisonExpression *node) = 0;
    virtual void visit(const class ValidPointerExpression *node) = 0;
    virtual void visit(const class FunctionPointerComparisonExpression *node) = 0;
    virtual void visit(const class AdditionExpression *node) = 0;
    virtual void visit(const class SubtractionExpression *node) = 0;
    virtual void visit(const class MultiplicationExpression *node) = 0;
    virtual void visit(const class DivisionExpression *node) = 0;
    virtual void visit(const class ModuloExpression *node) = 0;
    virtual void visit(const class ExponentiationExpression *node) = 0;
    virtual void visit(const class DummyExpression *node) = 0;
    virtual void visit(const class ArrayReferenceIndexExpression *node) = 0;
    virtual void visit(const class ArrayValueIndexExpression *node) = 0;
    virtual void visit(const class DictionaryReferenceIndexExpression *node) = 0;
    virtual void visit(const class DictionaryValueIndexExpression *node) = 0;
    virtual void visit(const class StringReferenceIndexExpression *node) = 0;
    virtual void visit(const class StringValueIndexExpression *node) = 0;
    virtual void visit(const class BytesReferenceIndexExpression *node) = 0;
    virtual void visit(const class BytesValueIndexExpression *node) = 0;
    virtual void visit(const class RecordReferenceFieldExpression *node) = 0;
    virtual void visit(const class RecordValueFieldExpression *node) = 0;
    virtual void visit(const class ArrayReferenceRangeExpression *node) = 0;
    virtual void visit(const class ArrayValueRangeExpression *node) = 0;
    virtual void visit(const class PointerDereferenceExpression *node) = 0;
    virtual void visit(const class ConstantExpression *node) = 0;
    virtual void visit(const class VariableExpression *node) = 0;
    virtual void visit(const class InterfaceMethodExpression *node) = 0;
    virtual void visit(const class FunctionCall *node) = 0;
    virtual void visit(const class StatementExpression *node) = 0;
    virtual void visit(const class NullStatement *node) = 0;
    virtual void visit(const class TypeDeclarationStatement *node) = 0;
    virtual void visit(const class DeclarationStatement *node) = 0;
    virtual void visit(const class ExceptionHandlerStatement *node) = 0;
    virtual void visit(const class AssertStatement *node) = 0;
    virtual void visit(const class AssignmentStatement *node) = 0;
    virtual void visit(const class ExpressionStatement *node) = 0;
    virtual void visit(const class ReturnStatement *node) = 0;
    virtual void visit(const class IncrementStatement *node) = 0;
    virtual void visit(const class IfStatement *node) = 0;
    virtual void visit(const class BaseLoopStatement *node) = 0;
    virtual void visit(const class CaseStatement *node) = 0;
    virtual void visit(const class ExitStatement *node) = 0;
    virtual void visit(const class NextStatement *node) = 0;
    virtual void visit(const class TryStatement *node) = 0;
    virtual void visit(const class RaiseStatement *node) = 0;
    virtual void visit(const class ResetStatement *node) = 0;
    virtual void visit(const class Function *node) = 0;
    virtual void visit(const class PredefinedFunction *node) = 0;
    virtual void visit(const class ExtensionFunction *node) = 0;
    virtual void visit(const class ModuleFunction *node) = 0;
    virtual void visit(const class Module *node) = 0;
    virtual void visit(const class Program *node) = 0;
};

class AstNode {
public:
    AstNode() {}
    virtual ~AstNode() {}
    virtual void accept(IAstVisitor *visitor) const = 0;
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
    virtual ~Frame() {}

    virtual void predeclare(Emitter &emitter);
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
    virtual int addSlot(const Token &token, const std::string &name, Name *ref, bool init_referenced);
    const Slot getSlot(size_t slot);
    virtual void setReferent(int slot, Name *ref);
    void setReferenced(int slot);

    virtual size_t get_depth() { return 0; }
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) = 0;

protected:
    Frame *const outer;
    bool predeclared;
    std::vector<Slot> slots;
private:
    Frame(const Frame &);
    Frame &operator=(const Frame &);
};

class ExternalGlobalInfo {
public:
    ExternalGlobalInfo(const Token &declaration, Name *ref, bool init_referenced): declaration(declaration), ref(ref), init_referenced(init_referenced) {}
    ExternalGlobalInfo(const ExternalGlobalInfo &rhs): declaration(rhs.declaration), ref(rhs.ref), init_referenced(rhs.init_referenced) {}
    ExternalGlobalInfo &operator=(const ExternalGlobalInfo &rhs) {
        if (this == &rhs) {
            return *this;
        }
        declaration = rhs.declaration;
        ref = rhs.ref;
        init_referenced = rhs.init_referenced;
        return *this;
    }
    Token declaration;
    Name *ref;
    bool init_referenced;
};

class ExternalGlobalFrame: public Frame {
public:
    ExternalGlobalFrame(Frame *outer, std::map<std::string, ExternalGlobalInfo> &external_globals): Frame(outer), external_globals(external_globals) {}
    virtual void predeclare(Emitter &emitter) override;
    virtual int addSlot(const Token &token, const std::string &name, Name *ref, bool init_referenced) override;
    virtual void setReferent(int slot, Name *ref) override;
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) override;
private:
    std::map<std::string, ExternalGlobalInfo> &external_globals;
};

class GlobalFrame: public Frame {
public:
    GlobalFrame(Frame *outer): Frame(outer) {}
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) override;
};

class LocalFrame: public Frame {
public:
    LocalFrame(Frame *outer): Frame(outer), nesting_depth(outer->get_depth()+1) {}
    virtual size_t get_depth() override { return nesting_depth; }
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) override;
    size_t nesting_depth;
};

class Scope {
public:
    Scope(Scope *parent, Frame *frame): parent(parent), frame(frame), names(), forwards() {
        for (int x = 0; x < SqlWheneverConditionCount; x++) {
            sql_whenever[x] = parent != nullptr ? parent->sql_whenever[x] : SqlWheneverAction::Continue;
        }
    }
    virtual ~Scope() {}

    bool allocateName(const Token &token, const std::string &name);
    Name *lookupName(const std::string &name, bool mark_referenced = true);
    Token getDeclaration(const std::string &name) const;
    virtual void addName(const Token &token, const std::string &name, Name *ref, bool init_referenced = false, bool allow_shadow = false);
    void addForward(const std::string &name, TypePointer *ptrtype);
    void resolveForward(const std::string &name, const TypeClass *classtype);
    void checkForward();
    Variable *makeTemporary(const Type *type);

    Scope *const parent;
    Frame *const frame;
    SqlWheneverAction sql_whenever[SqlWheneverConditionCount];
private:
    std::map<std::string, int> names;
    std::map<std::string, std::vector<TypePointer *>> forwards;
private:
    Scope(const Scope &);
    Scope &operator=(const Scope &);
};

class ExternalGlobalScope: public Scope {
public:
    ExternalGlobalScope(Scope *parent, Frame *frame, std::map<std::string, ExternalGlobalInfo> &external_globals);
};

class Name: public AstNode {
public:
    Name(const Token &declaration, const std::string &name, const Type *type): declaration(declaration), name(name), type(type) {}
    const Token declaration;
    const std::string name;
    const Type *type;

    virtual void reset() {}
    virtual void predeclare(Emitter &) const {}
    virtual void postdeclare(Emitter &) const {}

    virtual void generate_export(Emitter &emitter, const std::string &name) const = 0;
private:
    Name(const Name &);
    Name &operator=(const Name &);
};

class Type: public Name {
public:
    Type(const Token &declaration, const std::string &name): Name(declaration, name, nullptr), methods(), predeclared(false), postdeclared(false) {}

    std::map<std::string, Variable *> methods;

    virtual const Expression *make_default_value() const = 0;
    virtual void reset() override { predeclared = false; postdeclared = false; }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual bool is_assignment_compatible(const Type *rhs) const { return this == rhs; }
    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;
    virtual void generate_convert(Emitter &emitter, const Type *from) const;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const = 0;
    virtual void get_type_references(std::set<const Type *> &) const {}
    virtual std::string serialize(const Expression *value) const = 0;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const = 0;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const = 0;
protected:
    mutable bool predeclared;
    mutable bool postdeclared;
};

class TypeNothing: public Type {
public:
    TypeNothing(): Type(Token(), "Nothing") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override { internal_error("TypeNothing"); }
    virtual bool is_assignment_compatible(const Type *) const override { return false; }
    virtual void generate_load(Emitter &) const override { internal_error("TypeNothing"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeNothing"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeNothing"); }
    virtual std::string get_type_descriptor(Emitter &) const override { return "Z"; }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeNothing"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeNothing"); }
    virtual void debuginfo(Emitter &, minijson::object_writer &) const override { internal_error("TypeNothing"); }

    virtual std::string text() const override { return "TypeNothing"; }
};

extern TypeNothing *TYPE_NOTHING;

class TypeDummy: public Type {
public:
    TypeDummy(): Type(Token(), "Dummy") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override { internal_error("TypeDummy"); }
    virtual bool is_assignment_compatible(const Type *type) const override { return type != TYPE_NOTHING; }
    virtual void generate_load(Emitter &) const override { internal_error("TypeDummy"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeDummy"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeDummy"); }
    virtual std::string get_type_descriptor(Emitter &) const override { internal_error("TypeDummy"); }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeDummy"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeDummy"); }
    virtual void debuginfo(Emitter &, minijson::object_writer &) const override { internal_error("TypeDummy"); }

    virtual std::string text() const override { return "TypeNothing"; }
};

extern TypeDummy *TYPE_DUMMY;

class TypeBoolean: public Type {
public:
    TypeBoolean(): Type(Token(), "Boolean") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "B"; }
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeBoolean"; }
};

extern TypeBoolean *TYPE_BOOLEAN;

class TypeNumber: public Type {
public:
    TypeNumber(const Token &declaration): Type(declaration, "Number") {}
    TypeNumber(const Token &declaration, const std::string &name): Type(declaration, name) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "N"; }
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeNumber"; }
};

extern TypeNumber *TYPE_NUMBER;

class TypeString: public Type {
public:
    TypeString(): Type(Token(), "String") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "S"; }
    static std::string serialize(const std::string &value);
    virtual std::string serialize(const Expression *value) const override;
    static std::string deserialize_string(const Bytecode::Bytes &value, int &i);
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeString"; }
};

extern TypeString *TYPE_STRING;

class TypeBytes: public Type {
public:
    TypeBytes(): Type(Token(), "Bytes") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "Y"; }
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeBytes"; }
};

extern TypeBytes *TYPE_BYTES;

class ParameterType {
public:
    enum class Mode {
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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override { internal_error("TypeFunction"); }
    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual std::string serialize(const Expression *) const override { internal_error("TypeFunction"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeFunction"); }
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const Type *elementtype;

    virtual const Expression *make_default_value() const override;
    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const Type *elementtype;

    virtual const Expression *make_default_value() const override;
    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeDictionary(" + (elementtype != nullptr ? elementtype->text() : "any") + ")"; }
private:
    TypeDictionary(const TypeDictionary &);
    TypeDictionary &operator=(const TypeDictionary &);
};

extern TypeDictionary *TYPE_DICTIONARY_STRING;

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
    TypeRecord(const Token &declaration, const std::string &module, const std::string &name, const std::vector<Field> &fields): Type(declaration, name), module(module), fields(fields), field_names(make_field_names(fields)) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string module;
    const std::vector<Field> fields;
    const std::map<std::string, size_t> field_names;

    virtual const Expression *make_default_value() const override;
    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override;
private:
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

class TypeClass: public TypeRecord {
public:
    TypeClass(const Token &declaration, const std::string &module, const std::string &name, const std::vector<Field> &fields, const std::vector<const Interface *> &interfaces): TypeRecord(declaration, module, name, fields), interfaces(interfaces) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::vector<const Interface *> interfaces;

    virtual std::string get_type_descriptor(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class TypeForwardClass: public TypeClass {
public:
    TypeForwardClass(const Token &declaration): TypeClass(declaration, "module", "forward", std::vector<Field>(), std::vector<const Interface *>()) {}
};

class TypePointer: public Type {
public:
    TypePointer(const Token &declaration, const TypeClass *reftype);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeClass *reftype;

    virtual const Expression *make_default_value() const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual void generate_convert(Emitter &emitter, const Type *from) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypePointer(" + (reftype != nullptr ? reftype->text() : "any") + ")"; }
private:
    TypePointer(const TypePointer &);
    TypePointer &operator=(const TypePointer &);
};

class TypePointerNil: public TypePointer {
public:
    TypePointerNil(): TypePointer(Token(), nullptr) {}
};

class TypeValidPointer: public TypePointer {
public:
    TypeValidPointer(const Token &declaration, const TypeClass *classtype): TypePointer(declaration, classtype) {}
    virtual bool is_assignment_compatible(const Type *rhs) const override;

    virtual std::string text() const override { return "TypeValidPointer(" + reftype->text() + ")"; }
};

class TypeInterfacePointer: public Type {
public:
    TypeInterfacePointer(const Token &declaration, const Interface *interface);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Interface *interface;

    virtual const Expression *make_default_value() const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual void generate_convert(Emitter &emitter, const Type *from) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override;
private:
    TypeInterfacePointer(const TypeInterfacePointer &);
    TypeInterfacePointer &operator=(const TypeInterfacePointer &);
};

class TypeValidInterfacePointer: public TypeInterfacePointer {
public:
    TypeValidInterfacePointer(const Token &declaration, const Interface *interface): TypeInterfacePointer(declaration, interface) {}
    virtual bool is_assignment_compatible(const Type *rhs) const override;

    virtual std::string text() const override;
};

class TypeFunctionPointer: public Type {
public:
    TypeFunctionPointer(const Token &declaration, const TypeFunction *functype);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeFunction *functype;

    virtual const Expression *make_default_value() const override;
    virtual bool is_assignment_compatible(const Type *rhs) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeFunctionPointer(" + functype->text() + ")"; }
private:
    TypeFunctionPointer(const TypeFunctionPointer &);
    TypeFunctionPointer &operator=(const TypeFunctionPointer &);
};

class TypeFunctionPointerNowhere: public TypeFunctionPointer {
public:
    TypeFunctionPointerNowhere(): TypeFunctionPointer(Token(), nullptr) {}
};

class TypeEnum: public Type {
public:
    TypeEnum(const Token &declaration, const std::string &module, const std::string &name, const std::map<std::string, int> &names, Analyzer *analyzer);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string module;
    const std::map<std::string, int> names;

    virtual const Expression *make_default_value() const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeEnum(...)"; }
};

class TypeModule: public Type {
public:
    TypeModule(): Type(Token(), "module") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override { internal_error("TypeModule"); }
    virtual void generate_load(Emitter &) const override { internal_error("TypeModule"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeModule"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeModule"); }
    virtual std::string get_type_descriptor(Emitter &) const override { internal_error("TypeModule"); }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeModule"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeModule"); }
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeModule(...)"; }
};

extern TypeModule *TYPE_MODULE;

class TypeException: public Type {
public:
    TypeException(): Type(Token(), "Exception") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override { internal_error("TypeException"); }
    virtual void generate_load(Emitter &) const override { internal_error("TypeException"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeException"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeException"); }
    virtual std::string get_type_descriptor(Emitter &) const override { return "X"; }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeException"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeException"); }
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeException"; }
};

extern TypeException *TYPE_EXCEPTION;

class TypeInterface: public Type {
public:
    TypeInterface(): Type(Token(), "Interface") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override { internal_error("TypeInterface"); }
    virtual void generate_load(Emitter &) const override { internal_error("TypeInterface"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeInterface"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeInterface"); }
    virtual void generate_convert(Emitter &emitter, const Type *from) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "J"; }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeInterface"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeInterface"); }
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeInterface"; }
};

extern TypeInterface *TYPE_INTERFACE;

class LoopLabel: public Name {
public:
    LoopLabel(const Token &declaration): Name(declaration, declaration.text, nullptr) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void generate_export(Emitter &, const std::string &) const override { internal_error("LoopLabel"); }

    virtual std::string text() const override { return "LoopLable(" + declaration.text + ")"; }
};

class Variable: public Name {
public:
    Variable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Name(declaration, name, type), is_readonly(is_readonly) {}

    bool is_readonly;

    virtual void generate_address(Emitter &emitter) const = 0;
    virtual void generate_load(Emitter &emitter) const;
    virtual void generate_store(Emitter &emitter) const;
    virtual void generate_call(Emitter &emitter) const;
    virtual void generate_export(Emitter &, const std::string &) const override { internal_error("Variable"); }

    virtual std::string text() const override { return "Variable(" + name + ", " + type->text() + ")"; }
};

class PredefinedVariable: public Variable {
public:
    PredefinedVariable(const std::string &name, const Type *type): Variable(Token(), name, type, true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "PredefinedVariable(" + name + ", " + type->text() + ")"; }
};

class ModuleVariable: public Variable {
public:
    ModuleVariable(const std::string &module, const std::string &name, const Type *type, int index): Variable(Token(), name, type, true), module(module), index(index) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string module;
    const int index;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "ModuleVariable(" + module + "." + name + ")"; }
};

class GlobalVariable: public Variable {
public:
    GlobalVariable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Variable(declaration, name, type, is_readonly), index(-1) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    mutable int index;

    virtual void reset() override { index = -1; }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &emitter) const override;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual std::string text() const override { return "GlobalVariable(" + name + ", " + type->text() + ")"; }
};

class ExternalGlobalVariable: public Variable {
public:
    ExternalGlobalVariable(const Token &declaration, const std::string &name, const Type *type, bool is_readonly): Variable(declaration, name, type, is_readonly) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "ExternalGlobalVariable(" + name + ")"; }
};

class LocalVariable: public Variable {
public:
    LocalVariable(const Token &declaration, const std::string &name, const Type *type, size_t nesting_depth, bool is_readonly): Variable(declaration, name, type, is_readonly), nesting_depth(nesting_depth), index(-1) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const size_t nesting_depth;
    int index;

    virtual void predeclare(Emitter &) const override { internal_error("LocalVariable"); }
    virtual void predeclare(Emitter &emitter, int slot);
    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "LocalVariable(" + name + ", " + type->text() + ")"; }
private:
    LocalVariable(const LocalVariable &);
    LocalVariable &operator=(const LocalVariable &);
};

class FunctionParameter: public LocalVariable {
public:
    FunctionParameter(const Token &declaration, const std::string &name, const Type *type, size_t nesting_depth, ParameterType::Mode mode, const Expression *default_value): LocalVariable(declaration, name, type, nesting_depth, mode == ParameterType::Mode::IN), mode(mode), default_value(default_value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    ParameterType::Mode mode;
    const Expression *default_value;

    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "FunctionParameter(" + name + ", " + type->text() + ")"; }
private:
    FunctionParameter(const FunctionParameter &);
    FunctionParameter &operator=(const FunctionParameter &);
};

class Exception: public Name {
public:
    Exception(const Token &declaration, const std::string &name): Name(declaration, name, TYPE_EXCEPTION), subexceptions() {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    std::map<std::string, Exception *> subexceptions;

    virtual std::string text() const override { return "Exception(" + name + ")"; }
private:
    Exception(const Exception &);
    Exception &operator=(const Exception &);
};

class Interface: public Name {
public:
    Interface(const Token &declaration, const std::string &name, const std::vector<std::pair<Token, const TypeFunction *>> &methods): Name(declaration, name, TYPE_INTERFACE), methods(methods) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    const std::vector<std::pair<Token, const TypeFunction *>> methods;

    virtual std::string text() const override { return "Interface(" + name + ")"; }
private:
    Interface(const Interface &);
    Interface &operator=(const Interface &);
};

class Expression: public AstNode {
public:
    Expression(const Type *type, bool is_constant, bool is_readonly = true): type(type), is_constant(is_constant), is_readonly(is_readonly) {}

    bool eval_boolean(const Token &token) const;
    Number eval_number(const Token &token) const;
    std::string eval_string(const Token &token) const;
    void generate(Emitter &emitter) const;
    virtual void generate_expr(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &) const { internal_error("Expression::generate_call"); }

    const Type *type;
    const bool is_constant;
    const bool is_readonly;
protected:
    virtual bool eval_boolean() const = 0;
    virtual Number eval_number() const = 0;
    virtual std::string eval_string() const = 0;
    friend class TypeBoolean;
    friend class TypeNumber;
    friend class TypeString;
    friend class TypeBytes;
    friend class TypeEnum;
    friend class UnaryMinusExpression;
    friend class LogicalNotExpression;
    friend class DisjunctionExpression;
    friend class ConjunctionExpression;
    friend class AdditionExpression;
    friend class SubtractionExpression;
    friend class MultiplicationExpression;
    friend class DivisionExpression;
    friend class ModuloExpression;
    friend class ExponentiationExpression;
    friend class BooleanComparisonExpression;
    friend class NumericComparisonExpression;
    friend class EnumComparisonExpression;
    friend class StringComparisonExpression;
    friend class BytesComparisonExpression;
    friend class ConstantExpression;
    friend class FunctionCall;
private:
    Expression(const Expression &);
    Expression &operator=(const Expression &);
};

class Constant: public Name {
public:
    Constant(const Token &declaration, const std::string &name, const Expression *value): Name(declaration, name, value->type), value(value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::string value;

    virtual bool eval_boolean() const override { internal_error("ConstantStringExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantStringExpression"); }
    virtual std::string eval_string() const override { return value; }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantBytesExpression: public Expression {
public:
    ConstantBytesExpression(const std::string &name, const std::string &contents): Expression(TYPE_BYTES, true), name(name), contents(contents) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::string name;
    const std::string contents;

    virtual bool eval_boolean() const override { internal_error("ConstantBytesExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantBytesExpression"); }
    virtual std::string eval_string() const override { internal_error("ConstantBytesExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantEnumExpression: public Expression {
public:
    ConstantEnumExpression(const TypeEnum *type, int value): Expression(type, true), value(value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const int value;

    virtual bool eval_boolean() const override { internal_error("ConstantEnumExpression"); }
    virtual Number eval_number() const override { return number_from_uint32(value); }
    virtual std::string eval_string() const override { internal_error("ConstantEnumExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(): Expression(new TypePointerNil(), true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ConstantNilExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantNilExpression"); }
    virtual std::string eval_string() const override { internal_error("ConstantNilExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ConstantNilExpression"; }
};

class ConstantNowhereExpression: public Expression {
public:
    ConstantNowhereExpression(): Expression(new TypeFunctionPointerNowhere(), true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ConstantNowhereExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantNowhereExpression"); }
    virtual std::string eval_string() const override { internal_error("ConstantNowhereExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ConstantNowhereExpression"; }
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Type *elementtype, const std::vector<const Expression *> &elements): Expression(new TypeArray(Token(), elementtype), all_constant(elements)), elementtype(elementtype), elements(elements) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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

class ClassLiteralExpression: public RecordLiteralExpression {
public:
    ClassLiteralExpression(const TypeClass *type, const std::vector<const Expression *> &values): RecordLiteralExpression(type, values) {}
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "ClassLiteralExpression(...)"; }
private:
    ClassLiteralExpression(const ClassLiteralExpression &);
    ClassLiteralExpression &operator=(const ClassLiteralExpression &);
};

class NewClassExpression: public Expression {
public:
    NewClassExpression(const TypeClass *reftype, const Expression *value): Expression(new TypeValidPointer(Token(), reftype), false), reftype(reftype), value(value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeClass *reftype;
    const Expression *value;

    virtual bool eval_boolean() const override { internal_error("NewClassExpression"); }
    virtual Number eval_number() const override { internal_error("NewClassExpression"); }
    virtual std::string eval_string() const override { internal_error("NewClassExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "NewClassExpression(" + type->text() + ")"; }
private:
    NewClassExpression(const NewClassExpression &);
    NewClassExpression &operator=(const NewClassExpression &);
};

class UnaryMinusExpression: public Expression {
public:
    UnaryMinusExpression(const Expression *value): Expression(value->type, value->is_constant), value(value) {
        if (not type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("UnaryMinusExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not type->is_assignment_compatible(TYPE_BOOLEAN)) {
            internal_error("LogicalNotExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(right->type)) {
            internal_error("ConditionalExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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

class TryTrap {
public:
    TryTrap(const std::vector<const Exception *> &exceptions, const Variable *name, const AstNode *handler): exceptions(exceptions), name(name), handler(handler) {}
    TryTrap(const TryTrap &rhs): exceptions(rhs.exceptions), name(rhs.name), handler(rhs.handler) {}
    TryTrap &operator=(const TryTrap &rhs) {
        if (this == &rhs) {
            return *this;
        }
        exceptions = rhs.exceptions;
        name = rhs.name;
        handler = rhs.handler;
        return *this;
    }
    std::vector<const Exception *> exceptions;
    const Variable *name;
    const AstNode *handler;
};

class TryExpression: public Expression {
public:
    TryExpression(const Expression *expr, const std::vector<TryTrap> &catches): Expression(expr->type, false), expr(expr), catches(catches) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *expr;
    const std::vector<TryTrap> catches;

    virtual bool eval_boolean() const override { internal_error("TryExpression"); }
    virtual Number eval_number() const override { internal_error("TryExpression"); }
    virtual std::string eval_string() const override { internal_error("TryExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "TryExpression(" + expr->text() + ")";
    }
private:
    TryExpression(const TryExpression &);
    TryExpression &operator=(const TryExpression &);
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_assignment_compatible(TYPE_BOOLEAN) || not right->type->is_assignment_compatible(TYPE_BOOLEAN)) {
            internal_error("DisjunctionExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(TYPE_BOOLEAN) || not right->type->is_assignment_compatible(TYPE_BOOLEAN)) {
            internal_error("ConjunctionExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("BooleanComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("BooleanComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "BooleanComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("NumericComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("NumericComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "NumericComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    EnumComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("EnumComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("EnumComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "EnumComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("StringComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("StringComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "StringComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    BytesComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("BytesComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("BytesComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "BytesComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ArrayComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("ArrayComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ArrayComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("DictionaryComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("DictionaryComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "DictionaryComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    RecordComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("RecordComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("RecordComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("RecordComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "RecordComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    PointerComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("PointerComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("PointerComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("PointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "PointerComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    ValidPointerExpression(const Variable *var, const Expression *ptr): PointerComparisonExpression(ptr, new ConstantNilExpression(), ComparisonExpression::Comparison::NE), var(var) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual std::string eval_string() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "FunctionPointerComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), left(left), right(right) {
        if (not left->type->is_assignment_compatible(TYPE_NUMBER) || not right->type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("AdditionExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(TYPE_NUMBER) || not right->type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("SubtractionExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(TYPE_NUMBER) || not right->type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("MultiplicationExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(TYPE_NUMBER) || not right->type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("DivisionExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(TYPE_NUMBER) || not right->type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("ModuloExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
        if (not left->type->is_assignment_compatible(TYPE_NUMBER) || not right->type->is_assignment_compatible(TYPE_NUMBER)) {
            internal_error("ExponentiationExpression");
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual bool can_generate_address() const { return true; }
    virtual void generate_address_read(Emitter &) const = 0;
    virtual void generate_address_write(Emitter &) const = 0;
    virtual void generate_load(Emitter &) const;
    virtual void generate_store(Emitter &) const;
private:
    ReferenceExpression(const ReferenceExpression &);
    ReferenceExpression &operator=(const ReferenceExpression &);
};

class DummyExpression: public ReferenceExpression {
public:
    DummyExpression(): ReferenceExpression(TYPE_DUMMY, false) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("DummyExpression"); }
    virtual Number eval_number() const override { internal_error("DummyExpression"); }
    virtual std::string eval_string() const override { internal_error("DummyExpression"); }
    virtual bool can_generate_address() const override { return false; }
    virtual void generate_address_read(Emitter &) const override { internal_error("DummyExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("DummyExpression"); }
    virtual void generate_load(Emitter &) const override { internal_error("DummyExpression"); }
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "DummyExpression"; }
};

class ArrayReferenceIndexExpression: public ReferenceExpression {
public:
    ArrayReferenceIndexExpression(const Type *type, const ReferenceExpression *array, const Expression *index, bool always_create): ReferenceExpression(type, array->is_readonly), array(array), index(index), always_create(always_create) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual bool can_generate_address() const override { return false; }
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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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

class BytesReferenceIndexExpression: public ReferenceExpression {
public:
    BytesReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *ref;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;
    const FunctionCall *store;

    virtual bool eval_boolean() const override { internal_error("BytesReferenceIndexExpression"); }
    virtual Number eval_number() const override { internal_error("BytesReferenceIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("BytesReferenceIndexExpression"); }
    virtual bool can_generate_address() const override { return false; }
    virtual void generate_address_read(Emitter &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "BytesReferenceIndexExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &);
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &);
};

class BytesValueIndexExpression: public Expression {
public:
    BytesValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *str;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual bool eval_boolean() const override { internal_error("BytesValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("BytesValueIndexExpression"); }
    virtual std::string eval_string() const override { internal_error("BytesValueIndexExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "BytesValueIndexExpression(" + str->text() + ", " + first->text() + ", " + last->text() + ")"; }
private:
    BytesValueIndexExpression(const BytesValueIndexExpression &);
    BytesValueIndexExpression &operator=(const BytesValueIndexExpression &);
};

class RecordReferenceFieldExpression: public ReferenceExpression {
public:
    RecordReferenceFieldExpression(const Type *type, const ReferenceExpression *ref, const std::string &field, bool always_create): ReferenceExpression(type, ref->is_readonly), ref(ref), field(field), always_create(always_create) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *ref;
    const std::string field;
    const bool always_create;

    virtual bool eval_boolean() const override { internal_error("RecordReferenceFieldExpression"); }
    virtual Number eval_number() const override { internal_error("RecordReferenceFieldExpression"); }
    virtual std::string eval_string() const override { internal_error("RecordReferenceFieldExpression"); }
    virtual void generate_address_read(Emitter &) const override;
    virtual void generate_address_write(Emitter &) const override;

    virtual std::string text() const override { return "RecordReferenceFieldExpression(" + ref->text() + ", " + field + ")"; }
private:
    RecordReferenceFieldExpression(const RecordReferenceFieldExpression &);
    RecordReferenceFieldExpression &operator=(const RecordReferenceFieldExpression &);
};

class RecordValueFieldExpression: public Expression {
public:
    RecordValueFieldExpression(const Type *type, const Expression *rec, const std::string &field, bool always_create): Expression(type, rec->is_constant, rec->is_readonly), rec(rec), field(field), always_create(always_create) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *rec;
    const std::string field;
    const bool always_create;

    virtual bool eval_boolean() const override { internal_error("RecordValueFieldExpression"); }
    virtual Number eval_number() const override { internal_error("RecordValueFieldExpression"); }
    virtual std::string eval_string() const override { internal_error("RecordValueFieldExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "RecordValueFieldExpression(" + rec->text() + ", " + field + ")"; }
private:
    RecordValueFieldExpression(const RecordValueFieldExpression &);
    RecordValueFieldExpression &operator=(const RecordValueFieldExpression &);
};

class ArrayReferenceRangeExpression: public ReferenceExpression {
public:
    ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual bool can_generate_address() const override { return false; }
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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Variable *var;

    virtual bool eval_boolean() const override { internal_error("VariableExpression"); }
    virtual Number eval_number() const override { internal_error("VariableExpression"); }
    virtual std::string eval_string() const override { internal_error("VariableExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override { var->generate_call(emitter); }
    virtual void generate_address_read(Emitter &emitter) const override { var->generate_address(emitter); }
    virtual void generate_address_write(Emitter &emitter) const override { var->generate_address(emitter); }

    virtual std::string text() const override {
        return "VariableExpression(" + var->text() + ")";
    }
private:
    VariableExpression(const VariableExpression &);
    VariableExpression &operator=(const VariableExpression &);
};

class InterfaceMethodExpression: public Expression {
public:
    InterfaceMethodExpression(const TypeFunction *functype, size_t index): Expression(functype->returntype, false), functype(functype), index(index) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeFunction *functype;
    const size_t index;

    virtual bool eval_boolean() const override { internal_error("InterfaceMethodExpression"); }
    virtual Number eval_number() const override { internal_error("InterfaceMethodExpression"); }
    virtual std::string eval_string() const override { internal_error("InterfaceMethodExpression"); }
    virtual void generate_expr(Emitter &) const override { internal_error("InterfaceMethodExpression"); }
    virtual void generate_call(Emitter &) const override;

    virtual std::string text() const override { return "InterfaceMethodExpression(" + std::to_string(index) + ")"; }
private:
    InterfaceMethodExpression(const InterfaceMethodExpression &);
    InterfaceMethodExpression &operator=(const InterfaceMethodExpression &);
};

class FunctionCall: public Expression {
public:
    FunctionCall(const Expression *func, const std::vector<const Expression *> &args, const Expression *dispatch = nullptr): Expression(get_expr_type(func), is_intrinsic(func, args)), func(func), dispatch(dispatch), args(args) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const func;
    const Expression *const dispatch;
    const std::vector<const Expression *> args;

    virtual bool eval_boolean() const override { internal_error("FunctionCall"); }
    virtual Number eval_number() const override;
    virtual std::string eval_string() const override;
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
        const InterfaceMethodExpression *ime = dynamic_cast<const InterfaceMethodExpression *>(func);
        if (ime != nullptr) {
            return ime->type;
        }
        internal_error("not function or functionpointer: " + func->text());
    }

    static bool is_intrinsic(const Expression *func, const std::vector<const Expression *> &args);
};

class StatementExpression: public Expression {
public:
    StatementExpression(const Statement *stmt): Expression(TYPE_NOTHING, false), stmt(stmt) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual bool is_scope_exit_statement() const { return false; }

    void generate(Emitter &emitter) const;
    virtual void generate_code(Emitter &emitter) const = 0;
};

class CompoundStatement: public Statement {
public:
    CompoundStatement(int line, const std::vector<const Statement *> &statements): Statement(line), statements(statements) {}
    virtual void accept(IAstVisitor *visitor) const override { for (auto s: statements) s->accept(visitor); }

    const std::vector<const Statement *> statements;

    virtual void generate_code(Emitter &emitter) const override { for (auto s: statements) s->generate_code(emitter); }

    virtual void dumpsubnodes(std::ostream &out, int depth) const override;
    virtual std::string text() const override { return "CompoundStatement"; }
};

class NullStatement: public Statement {
public:
    NullStatement(int line): Statement(line) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void generate_code(Emitter &) const override {}

    virtual std::string text() const override { return "NullStatement"; }
};

class TypeDeclarationStatement: public Statement {
public:
    TypeDeclarationStatement(int line, const std::string &name, const ast::Type *type): Statement(line), name(name), type(type) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string name;
    const ast::Type *type;

    virtual void generate_code(Emitter &) const override;

    virtual std::string text() const override { return "TypeDeclarationStatement(" + name + ", " + type->text() + ")"; }
private:
    TypeDeclarationStatement(const TypeDeclarationStatement &);
    TypeDeclarationStatement &operator=(const TypeDeclarationStatement &);
};

class DeclarationStatement: public Statement {
public:
    DeclarationStatement(int line, Variable *decl): Statement(line), decl(decl) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    Variable *decl;

    virtual void generate_code(Emitter &) const override {}

    virtual std::string text() const override { return "DeclarationStatement(" + decl->text() + ")"; }
private:
    DeclarationStatement(const DeclarationStatement &);
    DeclarationStatement &operator=(const DeclarationStatement &);
};

class ExceptionHandlerStatement: public CompoundStatement {
public:
    ExceptionHandlerStatement(int line, const std::vector<const Statement *> &statements): CompoundStatement(line, statements) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void generate_code(Emitter &) const override;

    virtual std::string text() const override { return "ExceptionHandlerStatement"; }
};

class AssertStatement: public CompoundStatement {
public:
    AssertStatement(int line, const std::vector<const Statement *> &statements, const Expression *expr, const std::string &source): CompoundStatement(line, statements), expr(expr), source(source) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
            if (not v->type->is_assignment_compatible(expr->type)) {
                internal_error("AssignmentStatement");
            }
        }
    }
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const expr;

    virtual bool always_returns() const override { return true; }
    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "ReturnStatement(" + (expr != nullptr ? expr->text() : "") + ")"; }
private:
    ReturnStatement(const ReturnStatement &);
    ReturnStatement &operator=(const ReturnStatement &);
};

class IncrementStatement: public Statement {
public:
    IncrementStatement(int line, const ReferenceExpression *ref, int delta): Statement(line), ref(ref), delta(delta) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *ref;
    int delta;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "IncrementStatement(" + ref->text() + ", " + std::to_string(delta) + ")";
    }
private:
    IncrementStatement(const IncrementStatement &);
    IncrementStatement &operator=(const IncrementStatement &);
};

class IfStatement: public Statement {
public:
    IfStatement(int line, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(line), condition_statements(condition_statements), else_statements(else_statements) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;

    virtual bool always_returns() const override;
    virtual bool is_scope_exit_statement() const override;

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
    BaseLoopStatement(int line, unsigned int loop_id, const std::vector<const Statement *> &prologue, const std::vector<const Statement *> &statements, const std::vector<const Statement *> &tail, bool infinite_loop): CompoundStatement(line, statements), prologue(prologue), tail(tail), infinite_loop(infinite_loop), loop_id(loop_id) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<const Statement *> prologue;
    const std::vector<const Statement *> tail;

    const bool infinite_loop;
    const unsigned int loop_id;

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "BaseLoopStatement(...)";
    }
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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const unsigned int loop_id;

    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "ExitStatement(...)"; }
private:
    ExitStatement(const ExitStatement &);
    ExitStatement &operator=(const ExitStatement &);
};

class NextStatement: public Statement {
public:
    NextStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const unsigned int loop_id;

    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "NextStatement(...)"; }
private:
    NextStatement(const NextStatement &);
    NextStatement &operator=(const NextStatement &);
};

class TryStatement: public Statement {
public:
    TryStatement(int line, const std::vector<const Statement *> &statements, const std::vector<TryTrap> &catches): Statement(line), statements(statements), catches(catches) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<const Statement *> statements;
    const std::vector<TryTrap> catches;

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
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Exception *exception;
    const Expression *info;

    virtual bool always_returns() const override { return true; }
    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "RaiseStatement(" + exception->text() + ")"; }
private:
    RaiseStatement(const RaiseStatement &);
    RaiseStatement &operator=(const RaiseStatement &);
};

class ResetStatement: public Statement {
public:
    ResetStatement(int line, const std::vector<const ReferenceExpression *> &vars): Statement(line), variables(vars) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<const ReferenceExpression *> variables;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        std::string s = "ResetStatement(";
        for (auto v: variables) {
            s += v->text() + ", ";
        }
        return s + ")";
    }

private:
    ResetStatement(const ResetStatement &);
    ResetStatement &operator=(const ResetStatement &);
};

class Function: public Variable {
public:
    Function(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params, size_t nesting_depth);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    Frame *frame;
    Scope *scope;
    const std::vector<FunctionParameter *> params;
    size_t nesting_depth;
    mutable unsigned int entry_label;

    std::vector<const Statement *> statements;

    static const Type *makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params);

    virtual void reset() override { entry_label = UINT_MAX; }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &) const override {}
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override { internal_error("Function"); }
    virtual void generate_call(Emitter &emitter) const override;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const;

    virtual std::string text() const override { return "Function(" + name + ", " + type->text() + ")"; }
private:
    Function(const Function &);
    Function &operator=(const Function &);
};

class PredefinedFunction: public Variable {
public:
    PredefinedFunction(const std::string &name, const Type *type): Variable(Token(), name, type, true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void reset() override {}
    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Emitter &emitter) const override;
    virtual void generate_export(Emitter &, const std::string &) const override {}

    virtual std::string text() const override { return "PredefinedFunction(" + name + ", " + type->text() + ")"; }
};

class ExtensionFunction: public Function {
public:
    ExtensionFunction(const Token &declaration, const std::string &module, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params): Function(declaration, name, returntype, outer, parent, params, 1), module(module) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::string module;

    virtual void reset() override {}
    virtual void postdeclare(Emitter &) const override;
    virtual void generate_call(Emitter &emitter) const override;

    virtual std::string text() const override { return "ExtensionFunction(" + module + ", " + name + ", " + type->text() + ")"; }
};

class ModuleFunction: public Variable {
public:
    ModuleFunction(const std::string &module, const std::string &name, const Type *type, const std::string &descriptor): Variable(Token(), name, type, true), module(module), name(name), descriptor(descriptor) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string module;
    const std::string name;
    const std::string descriptor;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_call(Emitter &emitter) const override;

    virtual std::string text() const override { return "ModuleFunction(" + module + "." + name + ", " + type->text() + ")"; }
};

class ForeignFunction: public Function {
public:
    ForeignFunction(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params): Function(declaration, name, returntype, outer, parent, params, 1), library_name(), param_types(), foreign_index(-1) {}

    std::string library_name;
    std::map<std::string, std::string> param_types;
    mutable int foreign_index;

    virtual void reset() override { foreign_index = -1; }
    virtual void predeclare(Emitter &) const override;
    virtual void postdeclare(Emitter &) const override;
    virtual void generate_call(Emitter &emitter) const override;

private:
    ForeignFunction(const ForeignFunction &);
    ForeignFunction &operator=(const ForeignFunction &);
};

class Module: public Name {
public:
    Module(const Token &declaration, Scope *scope, const std::string &name): Name(declaration, name, TYPE_MODULE), scope(new Scope(scope, scope->frame)) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    Scope *scope;

    virtual void reset() override { for (size_t i = 0; i < scope->frame->getCount(); i++) { scope->frame->getSlot(i).ref->reset(); } }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_export(Emitter &, const std::string &) const override { internal_error("can't export module"); }

    virtual std::string text() const override { return "Module"; }
private:
    Module(const Module &);
    Module &operator=(const Module &);
};

class Program: public AstNode {
public:
    Program(const std::string &source_path, const std::string &source_hash, const std::string &module_name);
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::string source_path;
    const std::string source_hash;
    const std::string module_name;
    Frame *frame;
    Scope *scope;
    std::vector<const Statement *> statements;
    std::map<std::string, const Name *> exports;

    virtual void generate(Emitter &emitter) const;

    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const;

    virtual std::string text() const override { return "Program"; }
    virtual void dumpsubnodes(std::ostream &out, int depth) const override;
private:
    Program(const Program &);
    Program &operator=(const Program &);
};

} // namespace ast

#endif
