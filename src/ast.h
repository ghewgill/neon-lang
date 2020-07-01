#ifndef AST_H
#define AST_H

#include <functional>
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
#include "utf8string.h"
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
    virtual void visit(const class TypeObject *node) = 0;
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
    virtual void visit(const class ConstantNilObject *node) = 0;
    virtual void visit(const class TypeConversionExpression *node) = 0;
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
    virtual void visit(const class TypeTestExpression *node) = 0;
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
    virtual void visit(const class ObjectSubscriptExpression *node) = 0;
    virtual void visit(const class RecordReferenceFieldExpression *node) = 0;
    virtual void visit(const class RecordValueFieldExpression *node) = 0;
    virtual void visit(const class ArrayReferenceRangeExpression *node) = 0;
    virtual void visit(const class ArrayValueRangeExpression *node) = 0;
    virtual void visit(const class PointerDereferenceExpression *node) = 0;
    virtual void visit(const class ConstantExpression *node) = 0;
    virtual void visit(const class VariableExpression *node) = 0;
    virtual void visit(const class InterfaceMethodExpression *node) = 0;
    virtual void visit(const class InterfacePointerConstructor *node) = 0;
    virtual void visit(const class InterfacePointerDeconstructor *node) = 0;
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
    AstNode(const AstNode &) = delete;
    AstNode &operator=(const AstNode &) = delete;
    virtual ~AstNode() {}
    virtual void accept(IAstVisitor *visitor) const = 0;
    void dump(std::ostream &out, int depth = 0) const;
    virtual std::string text() const = 0;
    virtual void dumpsubnodes(std::ostream &/*out*/, int /*depth*/) const {}
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
    explicit Frame(Frame *outer): outer(outer), predeclared(false), slots() {}
    Frame(const Frame &) = delete;
    Frame &operator=(const Frame &) = delete;
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
    virtual void setReferent(int slot, const std::string &name, Name *ref);
    void setReferenced(int slot);

    virtual size_t get_depth() { return 0; }
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) = 0;

protected:
    Frame *const outer;
    bool predeclared;
    std::vector<Slot> slots;
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
    virtual void setReferent(int slot, const std::string &name, Name *ref) override;
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) override;
private:
    std::map<std::string, ExternalGlobalInfo> &external_globals;
};

class GlobalFrame: public Frame {
public:
    explicit GlobalFrame(Frame *outer): Frame(outer) {}
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) override;
};

class LocalFrame: public Frame {
public:
    explicit LocalFrame(Frame *outer): Frame(outer), nesting_depth(outer->get_depth()+1) {}
    virtual size_t get_depth() override { return nesting_depth; }
    virtual Variable *createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly) override;
    size_t nesting_depth;
};

class Scope {
public:
    Scope(Scope *parent, Frame *frame): parent(parent), frame(frame), names(), forwards() {
        for (int x = 0; x < SqlWheneverConditionCount; x++) {
            sql_whenever[x] = parent != nullptr ? parent->sql_whenever[x] : SqlWheneverAction();
        }
    }
    Scope(const Scope &) = delete;
    Scope &operator=(const Scope &) = delete;
    virtual ~Scope() {}

    bool allocateName(const Token &token, const std::string &name);
    Name *lookupName(const std::string &name, bool mark_referenced = true);
    Token getDeclaration(const std::string &name) const;
    virtual void addName(const Token &token, const std::string &name, Name *ref, bool init_referenced = false, bool allow_shadow = false);
    void replaceName(const Token &token, const std::string &name, Name *ref);
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
};

class ExternalGlobalScope: public Scope {
public:
    ExternalGlobalScope(Scope *parent, Frame *frame, std::map<std::string, ExternalGlobalInfo> &external_globals);
};

class Name: public AstNode {
public:
    Name(const Token &declaration, const std::string &name, const Type *type): declaration(declaration), name(name), type(type) {}
    Name(const Name &) = delete;
    Name &operator=(const Name &) = delete;
    const Token declaration;
    const std::string name;
    const Type *type;

    virtual void reset() {}
    virtual void predeclare(Emitter &) const {}
    virtual void postdeclare(Emitter &) const {}

    virtual void generate_export(Emitter &emitter, const std::string &name) const = 0;
};

class Type: public Name {
public:
    Type(const Token &declaration, const std::string &name): Name(declaration, name, nullptr), methods(), predeclared(false), postdeclared(false) {}

    std::map<std::string, Variable *> methods;

    virtual const Expression *make_default_value() const = 0;
    virtual void reset() override { predeclared = false; postdeclared = false; }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual bool is_structure_compatible(const Type *rhs) const { return this == rhs; }
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const;
    virtual void generate_load(Emitter &emitter) const = 0;
    virtual void generate_store(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &emitter) const = 0;
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
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *) const override { return nullptr; }
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
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &) const override { internal_error("TypeDummy"); }
    virtual void generate_store(Emitter &) const override { internal_error("TypeDummy"); }
    virtual void generate_call(Emitter &) const override { internal_error("TypeDummy"); }
    virtual std::string get_type_descriptor(Emitter &) const override { internal_error("TypeDummy"); }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeDummy"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeDummy"); }
    virtual void debuginfo(Emitter &, minijson::object_writer &) const override { internal_error("TypeDummy"); }

    virtual std::string text() const override { return "TypeDummy"; }
};

extern TypeDummy *TYPE_DUMMY;

class TypeBoolean: public Type {
public:
    TypeBoolean(): Type(Token(), "Boolean") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
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
    explicit TypeNumber(const Token &declaration): Type(declaration, "Number") {}
    TypeNumber(const Token &declaration, const std::string &name): Type(declaration, name) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual const Expression *make_default_value() const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
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
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "S"; }
    static std::string serialize(const utf8string &value);
    virtual std::string serialize(const Expression *value) const override;
    static utf8string deserialize_string(const Bytecode::Bytes &value, int &i);
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
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
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

class TypeObject: public Type {
public:
    TypeObject(): Type(Token(), "Object") {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &) const override { return "O"; }
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeObject"; }
};

extern TypeObject *TYPE_OBJECT;

class ParameterType {
public:
    enum class Mode {
        IN,
        INOUT,
        OUT
    };
    ParameterType(const Token &declaration, Mode mode, const Type *type, const Expression *default_value): declaration(declaration), mode(mode), type(type), default_value(default_value) {}
    ParameterType(const ParameterType &) = delete;
    ParameterType &operator=(const ParameterType &) = delete;
    const Token declaration;
    const Mode mode;
    const Type *type;
    const Expression *default_value;

    std::string text() const { return std::string("ParameterType(mode=") + (mode == Mode::IN ? "IN" : mode == Mode::INOUT ? "OUT" : mode == Mode::OUT ? "OUT" : "unknown") + ",type=" + (type != nullptr ? type->text() : "none") + ")"; }
};

class TypeFunction: public Type {
public:
    TypeFunction(const Type *returntype, const std::vector<const ParameterType *> &params, bool variadic): Type(Token(), "function"), returntype(returntype), params(params), variadic(variadic) {}
    TypeFunction(const TypeFunction &) = delete;
    TypeFunction &operator=(const TypeFunction &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual const Expression *make_default_value() const override { internal_error("TypeFunction"); }
    virtual void predeclare(Emitter &emitter) const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual std::string serialize(const Expression *) const override { internal_error("TypeFunction"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeFunction"); }
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    int get_stack_delta() const;

    const Type *returntype;
    const std::vector<const ParameterType *> params;
    const bool variadic;

    virtual std::string text() const override {
        std::string r = "TypeFunction(returntype=" + (returntype != nullptr ? returntype->text() : "none") + ", params=";
        bool first = true;
        for (auto p: params) {
            if (first) {
                first = false;
            } else {
                r += ",";
            }
            r += p->text();
        }
        return r + ")";
    }
};

class TypeArray: public Type {
public:
    TypeArray(const Token &declaration, const Type *elementtype);
    TypeArray(const TypeArray &) = delete;
    TypeArray &operator=(const TypeArray &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const Type *elementtype;

    virtual const Expression *make_default_value() const override;
    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_structure_compatible(const Type *rhs) const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeArray(" + (elementtype != nullptr ? elementtype->text() : "any") + ")"; }
};

extern TypeArray *TYPE_ARRAY_NUMBER;
extern TypeArray *TYPE_ARRAY_STRING;
extern TypeArray *TYPE_ARRAY_OBJECT;

class TypeArrayLiteral: public TypeArray {
public:
    TypeArrayLiteral(const Token &declaration, const Type *elementtype, const std::vector<const Expression *> &elements, const std::vector<Token> &elementtokens): TypeArray(declaration, elementtype), elements(elements), elementtokens(elementtokens) {}
    std::vector<const Expression *> elements;
    std::vector<Token> elementtokens;
};

class TypeDictionary: public Type {
public:
    TypeDictionary(const Token &declaration, const Type *elementtype);
    TypeDictionary(const TypeDictionary &) = delete;
    TypeDictionary &operator=(const TypeDictionary &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const Type *elementtype;

    virtual const Expression *make_default_value() const override;
    virtual void predeclare(Emitter &emitter) const override;
    virtual bool is_structure_compatible(const Type *rhs) const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *value) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeDictionary(" + (elementtype != nullptr ? elementtype->text() : "any") + ")"; }
};

extern TypeDictionary *TYPE_DICTIONARY_STRING;
extern TypeDictionary *TYPE_DICTIONARY_OBJECT;

class TypeDictionaryLiteral: public TypeDictionary {
public:
    TypeDictionaryLiteral(const Token &declaration, const Type *elementtype, const std::vector<std::pair<utf8string, const Expression *>> &elements, const std::vector<Token> &elementtokens): TypeDictionary(declaration, elementtype), elements(elements), elementtokens(elementtokens) {}
    std::vector<std::pair<utf8string, const Expression *>> elements;
    std::vector<Token> elementtokens;
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
    TypeRecord(const Token &declaration, const std::string &module, const std::string &name, const std::vector<Field> &fields): Type(declaration, name), module(module), fields(fields), field_names(make_field_names(fields)) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string module;
    const std::vector<Field> fields;
    const std::map<std::string, size_t> field_names;

    virtual const Expression *make_default_value() const override;
    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
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
    explicit TypeForwardClass(const Token &declaration): TypeClass(declaration, "module", "forward", std::vector<Field>(), std::vector<const Interface *>()) {}
};

class TypePointer: public Type {
public:
    TypePointer(const Token &declaration, const TypeClass *reftype);
    TypePointer(const TypePointer &) = delete;
    TypePointer &operator=(const TypePointer &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeClass *reftype;

    virtual const Expression *make_default_value() const override;
    virtual bool is_structure_compatible(const Type *rhs) const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypePointer(" + (reftype != nullptr ? reftype->text() : "any") + ")"; }
};

class TypePointerNil: public TypePointer {
public:
    TypePointerNil(): TypePointer(Token(), nullptr) {}
};

class TypeValidPointer: public TypePointer {
public:
    TypeValidPointer(const Token &declaration, const TypeClass *classtype): TypePointer(declaration, classtype) {}
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;

    virtual std::string text() const override { return "TypeValidPointer(" + reftype->text() + ")"; }
};

class TypeInterfacePointer: public Type {
public:
    TypeInterfacePointer(const Token &declaration, const Interface *interface);
    TypeInterfacePointer(const TypeInterfacePointer &) = delete;
    TypeInterfacePointer &operator=(const TypeInterfacePointer &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Interface *interface;

    virtual const Expression *make_default_value() const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override;
};

class TypeValidInterfacePointer: public TypeInterfacePointer {
public:
    TypeValidInterfacePointer(const Token &declaration, const Interface *interface): TypeInterfacePointer(declaration, interface) {}
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;

    virtual std::string text() const override;
};

class TypeFunctionPointer: public Type {
public:
    TypeFunctionPointer(const Token &declaration, const TypeFunction *functype);
    TypeFunctionPointer(const TypeFunctionPointer &) = delete;
    TypeFunctionPointer &operator=(const TypeFunctionPointer &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeFunction *functype;

    virtual const Expression *make_default_value() const override;
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *e)> make_converter(const Type *from) const override;
    virtual void generate_load(Emitter &emitter) const override;
    virtual void generate_store(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override;
    virtual std::string get_type_descriptor(Emitter &emitter) const override;
    virtual void get_type_references(std::set<const Type *> &references) const override;
    virtual std::string serialize(const Expression *) const override;
    virtual const Expression *deserialize_value(const Bytecode::Bytes &value, int &i) const override;
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeFunctionPointer(" + (functype != nullptr ? functype->text() : "nowhere") + ")"; }
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
    virtual std::function<const Expression *(Analyzer *analyzer, const Expression *from)> make_converter(const Type *from) const override;
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
    virtual std::string get_type_descriptor(Emitter &) const override { return "J"; }
    virtual std::string serialize(const Expression *) const override { internal_error("TypeInterface"); }
    virtual const Expression *deserialize_value(const Bytecode::Bytes &, int &) const override { internal_error("TypeInterface"); }
    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const override;

    virtual std::string text() const override { return "TypeInterface"; }
};

extern TypeInterface *TYPE_INTERFACE;

class LoopLabel: public Name {
public:
    explicit LoopLabel(const Token &declaration): Name(declaration, declaration.text, nullptr) {}
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
    ModuleVariable(const Module *module, const std::string &name, const Type *type, int index): Variable(Token(), name, type, true), module(module), index(index) {}
    ModuleVariable(const ModuleVariable &) = delete;
    ModuleVariable &operator=(const ModuleVariable &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const Module *module;
    const int index;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override;
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
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const size_t nesting_depth;
    int index;

    virtual void predeclare(Emitter &) const override { internal_error("LocalVariable"); }
    virtual void predeclare(Emitter &emitter, int slot);
    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "LocalVariable(" + name + ", " + type->text() + ")"; }
};

class FunctionParameter: public LocalVariable {
public:
    FunctionParameter(const Token &declaration, const std::string &name, const Type *type, size_t nesting_depth, ParameterType::Mode mode, const Expression *default_value): LocalVariable(declaration, name, type, nesting_depth, mode == ParameterType::Mode::IN), mode(mode), default_value(default_value) {}
    FunctionParameter(const FunctionParameter &) = delete;
    FunctionParameter &operator=(const FunctionParameter &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    ParameterType::Mode mode;
    const Expression *default_value;

    virtual void generate_address(Emitter &emitter) const override;

    virtual std::string text() const override { return "FunctionParameter(" + name + ", " + type->text() + ")"; }
};

class Exception: public Name {
public:
    Exception(const Token &declaration, const std::string &name): Name(declaration, name, TYPE_EXCEPTION), subexceptions() {}
    Exception(const Exception &) = delete;
    Exception &operator=(const Exception &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    std::map<std::string, Exception *> subexceptions;

    virtual std::string text() const override { return "Exception(" + name + ")"; }
};

class Interface: public Name {
public:
    Interface(const Token &declaration, const std::string &name, const std::vector<std::pair<Token, const TypeFunction *>> &methods): Name(declaration, name, TYPE_INTERFACE), methods(methods) {}
    Interface(const Interface &) = delete;
    Interface &operator=(const Interface &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    const std::vector<std::pair<Token, const TypeFunction *>> methods;

    virtual std::string text() const override { return "Interface(" + name + ")"; }
};

class Expression: public AstNode {
public:
    Expression(const Type *type, bool is_constant, bool is_readonly = true): type(type), is_constant(is_constant), is_readonly(is_readonly) {}
    Expression(const Expression &) = delete;
    Expression &operator=(const Expression &) = delete;

    bool eval_boolean(const Token &token) const;
    Number eval_number(const Token &token) const;
    utf8string eval_string(const Token &token) const;
    void generate(Emitter &emitter) const;
    virtual void generate_expr(Emitter &emitter) const = 0;
    virtual void generate_call(Emitter &) const { internal_error("Expression::generate_call"); }

    const Type *type;
    const bool is_constant;
    const bool is_readonly;
protected:
    virtual bool eval_boolean() const = 0;
    virtual Number eval_number() const = 0;
    virtual utf8string eval_string() const = 0;
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
};

class Constant: public Name {
public:
    Constant(const Token &declaration, const std::string &name, const Expression *value): Name(declaration, name, value->type), value(value) {}
    Constant(const Constant &) = delete;
    Constant &operator=(const Constant &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *value;

    virtual void generate_export(Emitter &emitter, const std::string &name) const override;

    virtual std::string text() const override { return "Constant(" + name + ", " + value->text() + ")"; }
};

class ConstantBooleanExpression: public Expression {
public:
    explicit ConstantBooleanExpression(bool value): Expression(TYPE_BOOLEAN, true), value(value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const bool value;

    virtual bool eval_boolean() const override { return value; }
    virtual Number eval_number() const override { internal_error("ConstantBooleanExpression"); }
    virtual utf8string eval_string() const override { internal_error("ConstantBooleanExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantNumberExpression: public Expression {
public:
    explicit ConstantNumberExpression(Number value): Expression(TYPE_NUMBER, true), value(value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Number value;

    virtual bool eval_boolean() const override { internal_error("ConstantNumberExpression"); }
    virtual Number eval_number() const override { return value; }
    virtual utf8string eval_string() const override { internal_error("ConstantNumberExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantStringExpression: public Expression {
public:
    explicit ConstantStringExpression(const utf8string &value): Expression(TYPE_STRING, true), value(value) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const utf8string value;

    virtual bool eval_boolean() const override { internal_error("ConstantStringExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantStringExpression"); }
    virtual utf8string eval_string() const override { return value; }
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
    virtual utf8string eval_string() const override { internal_error("ConstantBytesExpression"); }
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
    virtual utf8string eval_string() const override { internal_error("ConstantEnumExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override;
};

class ConstantNilExpression: public Expression {
public:
    ConstantNilExpression(): Expression(new TypePointerNil(), true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ConstantNilExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantNilExpression"); }
    virtual utf8string eval_string() const override { internal_error("ConstantNilExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ConstantNilExpression"; }
};

class ConstantNowhereExpression: public Expression {
public:
    ConstantNowhereExpression(): Expression(new TypeFunctionPointerNowhere(), true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ConstantNowhereExpression"); }
    virtual Number eval_number() const override { internal_error("ConstantNowhereExpression"); }
    virtual utf8string eval_string() const override { internal_error("ConstantNowhereExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ConstantNowhereExpression"; }
};

class ConstantNilObject: public Expression {
public:
    ConstantNilObject(): Expression(new TypeObject(), true) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ConstantNilObject"); }
    virtual Number eval_number() const override { internal_error("ConstantNilObject"); }
    virtual utf8string eval_string() const override { internal_error("ConstantNilObject"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ConstantNilObject"; }
};

class TypeConversionExpression: public Expression {
public:
    TypeConversionExpression(const Type *type, const Expression *expr): Expression(type, expr->is_constant), expr(expr) {}
    TypeConversionExpression(const TypeConversionExpression &) = delete;
    TypeConversionExpression &operator=(const TypeConversionExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *expr;

    virtual bool eval_boolean() const override { internal_error("TypeConversionExpression"); }
    virtual Number eval_number() const override { internal_error("TypeConversionExpression"); }
    virtual utf8string eval_string() const override { internal_error("TypeConversionExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "TypeConversionExpression(" + expr->text() + ")"; }
};

class ArrayLiteralExpression: public Expression {
public:
    ArrayLiteralExpression(const Type *elementtype, const std::vector<const Expression *> &elements, const std::vector<Token> &elementtokens): Expression(new TypeArrayLiteral(Token(), elementtype, elements, elementtokens), all_constant(elements)), elementtype(elementtype), elements(elements) {}
    ArrayLiteralExpression(const ArrayLiteralExpression &) = delete;
    ArrayLiteralExpression &operator=(const ArrayLiteralExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Type *elementtype;
    const std::vector<const Expression *> elements;

    virtual bool eval_boolean() const override { internal_error("ArrayLiteralExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayLiteralExpression"); }
    virtual utf8string eval_string() const override { internal_error("ArrayLiteralExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "ArrayLiteralExpression(...)"; }
private:
    static bool all_constant(const std::vector<const Expression *> &elements);
};

class DictionaryLiteralExpression: public Expression {
public:
    DictionaryLiteralExpression(const Type *elementtype, const std::vector<std::pair<utf8string, const Expression *>> &elements, const std::vector<Token> &elementtokens): Expression(new TypeDictionaryLiteral(Token(), elementtype, elements, elementtokens), all_constant(elements)), elementtype(elementtype), dict(make_dictionary(elements)) {}
    DictionaryLiteralExpression(const DictionaryLiteralExpression &) = delete;
    DictionaryLiteralExpression &operator=(const DictionaryLiteralExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Type *elementtype;
    const std::map<utf8string, const Expression *> dict;

    virtual bool eval_boolean() const override { internal_error("DictionaryLiteralExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryLiteralExpression"); }
    virtual utf8string eval_string() const override { internal_error("DictionaryLiteralExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "DictionaryLiteralExpression(...)"; }
private:
    static bool all_constant(const std::vector<std::pair<utf8string, const Expression *>> &elements);
    static std::map<utf8string, const Expression *> make_dictionary(const std::vector<std::pair<utf8string, const Expression *>> &elements);
};

class RecordLiteralExpression: public Expression {
public:
    RecordLiteralExpression(const TypeRecord *type, const std::vector<const Expression *> &values): Expression(type, all_constant(values)), values(values) {
        if (type == nullptr) {
            internal_error("RecordLiteralExpression: unexpected null type");
        }
    }
    RecordLiteralExpression(const RecordLiteralExpression &) = delete;
    RecordLiteralExpression &operator=(const RecordLiteralExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<const Expression *> values;

    virtual bool eval_boolean() const override { internal_error("RecordLiteralExpression"); }
    virtual Number eval_number() const override { internal_error("RecordLiteralExpression"); }
    virtual utf8string eval_string() const override { internal_error("RecordLiteralExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "RecordLiteralExpression(...)"; }
private:
    static bool all_constant(const std::vector<const Expression *> &values);
};

class ClassLiteralExpression: public RecordLiteralExpression {
public:
    ClassLiteralExpression(const TypeClass *type, const std::vector<const Expression *> &values): RecordLiteralExpression(type, values) {}
    ClassLiteralExpression(const ClassLiteralExpression &) = delete;
    ClassLiteralExpression &operator=(const ClassLiteralExpression &) = delete;
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "ClassLiteralExpression(...)"; }
};

class NewClassExpression: public Expression {
public:
    NewClassExpression(const TypeClass *reftype, const Expression *value): Expression(new TypeValidPointer(Token(), reftype), false), reftype(reftype), value(value) {}
    NewClassExpression(const NewClassExpression &) = delete;
    NewClassExpression &operator=(const NewClassExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeClass *reftype;
    const Expression *value;

    virtual bool eval_boolean() const override { internal_error("NewClassExpression"); }
    virtual Number eval_number() const override { internal_error("NewClassExpression"); }
    virtual utf8string eval_string() const override { internal_error("NewClassExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "NewClassExpression(" + type->text() + ")"; }
};

class UnaryMinusExpression: public Expression {
public:
    explicit UnaryMinusExpression(const Expression *value): Expression(TYPE_NUMBER, value->is_constant), value(value) {
        if (type != TYPE_NUMBER) {
            internal_error("UnaryMinusExpression");
        }
    }
    UnaryMinusExpression(const UnaryMinusExpression &) = delete;
    UnaryMinusExpression &operator=(const UnaryMinusExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const value;

    virtual bool eval_boolean() const override { internal_error("UnaryMinusExpression"); }
    virtual Number eval_number() const override { return number_negate(value->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("UnaryMinusExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "UnaryMinusExpression(" + value->text() + ")";
    }
};

class LogicalNotExpression: public Expression {
public:
    explicit LogicalNotExpression(const Expression *value): Expression(TYPE_BOOLEAN, value->is_constant), value(value) {
        if (type != TYPE_BOOLEAN) {
            internal_error("LogicalNotExpression");
        }
    }
    LogicalNotExpression(const LogicalNotExpression &) = delete;
    LogicalNotExpression &operator=(const LogicalNotExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const value;

    virtual bool eval_boolean() const override { return not value->eval_boolean(); }
    virtual Number eval_number() const override { internal_error("LogicalNotExpression"); }
    virtual utf8string eval_string() const override { internal_error("LogicalNotExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "LogicalNotExpression(" + value->text() + ")";
    }
};

class ConditionalExpression: public Expression {
public:
    ConditionalExpression(const Expression *condition, const Expression *left, const Expression *right): Expression(left->type, left->is_constant && right->is_constant), condition(condition), left(left), right(right) {
        if (left->type != right->type) {
            internal_error("ConditionalExpression");
        }
    }
    ConditionalExpression(const ConditionalExpression &) = delete;
    ConditionalExpression &operator=(const ConditionalExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *condition;
    const Expression *left;
    const Expression *right;

    virtual bool eval_boolean() const override { internal_error("ConditionalExpression"); }
    virtual Number eval_number() const override { internal_error("ConditionalExpression"); }
    virtual utf8string eval_string() const override { internal_error("ConditionalExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ConditionalExpression(" + condition->text() + "," + left->text() + "," + right->text() + ")";
    }
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
    TryExpression(const TryExpression &) = delete;
    TryExpression &operator=(const TryExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *expr;
    const std::vector<TryTrap> catches;

    virtual bool eval_boolean() const override { internal_error("TryExpression"); }
    virtual Number eval_number() const override { internal_error("TryExpression"); }
    virtual utf8string eval_string() const override { internal_error("TryExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "TryExpression(" + expr->text() + ")";
    }
};

class DisjunctionExpression: public Expression {
public:
    DisjunctionExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_BOOLEAN || right->type != TYPE_BOOLEAN) {
            internal_error("DisjunctionExpression");
        }
    }
    DisjunctionExpression(const DisjunctionExpression &) = delete;
    DisjunctionExpression &operator=(const DisjunctionExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { return left->eval_boolean() || right->eval_boolean(); }
    virtual Number eval_number() const override { internal_error("DisjunctionExpression"); }
    virtual utf8string eval_string() const override { internal_error("DisjunctionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "DisjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ConjunctionExpression: public Expression {
public:
    ConjunctionExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_BOOLEAN || right->type != TYPE_BOOLEAN) {
            internal_error("ConjunctionExpression");
        }
    }
    ConjunctionExpression(const ConjunctionExpression &) = delete;
    ConjunctionExpression &operator=(const ConjunctionExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { return left->eval_boolean() && right->eval_boolean(); }
    virtual Number eval_number() const override { internal_error("ConjunctionExpression"); }
    virtual utf8string eval_string() const override { internal_error("ConjunctionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ConjunctionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class TypeTestExpression: public Expression {
public:
    TypeTestExpression(const Expression *expr_before_conversion, const Expression *expr_after_conversion): Expression(TYPE_BOOLEAN, expr_before_conversion->type != TYPE_OBJECT), expr_before_conversion(expr_before_conversion), expr_after_conversion(expr_after_conversion) {}
    TypeTestExpression(const TypeTestExpression &) = delete;
    TypeTestExpression &operator=(const TypeTestExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *expr_before_conversion;
    const Expression *expr_after_conversion;

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("TypeTestExpression"); }
    virtual utf8string eval_string() const override { internal_error("TypeTestExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "TypeTestExpression(" + expr_before_conversion->text() + ", " + expr_after_conversion->text() + ")"; }
};

class ArrayInExpression: public Expression {
public:
    ArrayInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}
    ArrayInExpression(const ArrayInExpression &) = delete;
    ArrayInExpression &operator=(const ArrayInExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *left;
    const Expression *right;

    virtual bool eval_boolean() const override { internal_error("ArrayInExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayInExpression"); }
    virtual utf8string eval_string() const override { internal_error("ArrayInExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ArrayInExpression(" + left->text() + ", " + right->text() + ")"; }
};

class DictionaryInExpression: public Expression {
public:
    DictionaryInExpression(const Expression *left, const Expression *right): Expression(TYPE_BOOLEAN, false), left(left), right(right) {}
    DictionaryInExpression(const DictionaryInExpression &) = delete;
    DictionaryInExpression &operator=(const DictionaryInExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *left;
    const Expression *right;

    virtual bool eval_boolean() const override { internal_error("DictionaryInExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryInExpression"); }
    virtual utf8string eval_string() const override { internal_error("DictionaryInExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "DictionaryInExpression(" + left->text() + ", " + right->text() + ")"; }
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
    ComparisonExpression(const Type *operand_type, const Expression *left, const Expression *right, Comparison comp): Expression(TYPE_BOOLEAN, left->is_constant && right->is_constant), operand_type(operand_type), left(left), right(right), comp(comp) {}
    ComparisonExpression(const ComparisonExpression &) = delete;
    ComparisonExpression &operator=(const ComparisonExpression &) = delete;

    const Type *const operand_type;
    const Expression *const left;
    const Expression *const right;
    const Comparison comp;

    virtual void generate_expr(Emitter &emitter) const override;
    virtual void generate_comparison_opcode(Emitter &emitter) const = 0;
};

class ChainedComparisonExpression: public Expression {
public:
    explicit ChainedComparisonExpression(const std::vector<const ComparisonExpression *> &comps): Expression(TYPE_BOOLEAN, false), comps(comps) {}
    ChainedComparisonExpression(const ChainedComparisonExpression &) = delete;
    ChainedComparisonExpression &operator=(const ChainedComparisonExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<const ComparisonExpression *> comps;

    virtual bool eval_boolean() const override { internal_error("ChainedComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("ChainedComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("ChainedComparisonExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ChainedComparisonExpression(...)"; }
};

class BooleanComparisonExpression: public ComparisonExpression {
public:
    BooleanComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(TYPE_BOOLEAN, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("BooleanComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("BooleanComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "BooleanComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class NumericComparisonExpression: public ComparisonExpression {
public:
    NumericComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(TYPE_NUMBER, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("NumericComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("NumericComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "NumericComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class EnumComparisonExpression: public ComparisonExpression {
public:
    EnumComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left->type, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("EnumComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("EnumComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "EnumComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class StringComparisonExpression: public ComparisonExpression {
public:
    StringComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(TYPE_STRING, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("StringComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("StringComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "StringComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class BytesComparisonExpression: public ComparisonExpression {
public:
    BytesComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(TYPE_BYTES, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override { internal_error("BytesComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("BytesComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "BytesComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class ArrayComparisonExpression: public ComparisonExpression {
public:
    ArrayComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left->type, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("ArrayComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("ArrayComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ArrayComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class DictionaryComparisonExpression: public ComparisonExpression {
public:
    DictionaryComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left->type, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("DictionaryComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("DictionaryComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "DictionaryComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class RecordComparisonExpression: public ComparisonExpression {
public:
    RecordComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left->type, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("RecordComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("RecordComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("RecordComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "RecordComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class PointerComparisonExpression: public ComparisonExpression {
public:
    PointerComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left->type, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("PointerComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("PointerComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("PointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "PointerComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class ValidPointerExpression: public PointerComparisonExpression {
public:
    ValidPointerExpression(const Variable *var, const Expression *ptr): PointerComparisonExpression(ptr, new ConstantNilExpression(), ComparisonExpression::Comparison::NE), var(var) {}
    ValidPointerExpression(const ValidPointerExpression &) = delete;
    ValidPointerExpression &operator=(const ValidPointerExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Variable *var;

    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ValidPointerExpression(" + left->text() + ")";
    }
};

class FunctionPointerComparisonExpression: public ComparisonExpression {
public:
    FunctionPointerComparisonExpression(const Expression *left, const Expression *right, Comparison comp): ComparisonExpression(left->type, left, right, comp) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual Number eval_number() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual utf8string eval_string() const override { internal_error("FunctionPointerComparisonExpression"); }
    virtual void generate_comparison_opcode(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "FunctionPointerComparisonExpression(" + left->text() + to_string(comp) + right->text() + ")";
    }
};

class AdditionExpression: public Expression {
public:
    AdditionExpression(const Expression *left, const Expression *right): Expression(TYPE_NUMBER, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_NUMBER || right->type != TYPE_NUMBER) {
            internal_error("AdditionExpression");
        }
    }
    AdditionExpression(const AdditionExpression &) = delete;
    AdditionExpression &operator=(const AdditionExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { internal_error("AdditionExpression"); }
    virtual Number eval_number() const override { return number_add(left->eval_number(), right->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("AdditionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "AdditionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class SubtractionExpression: public Expression {
public:
    SubtractionExpression(const Expression *left, const Expression *right): Expression(TYPE_NUMBER, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_NUMBER || right->type != TYPE_NUMBER) {
            internal_error("SubtractionExpression");
        }
    }
    SubtractionExpression(const SubtractionExpression &) = delete;
    SubtractionExpression &operator=(const SubtractionExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { internal_error("SubtractionExpression"); }
    virtual Number eval_number() const override { return number_subtract(left->eval_number(), right->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("SubtractionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "SubtractionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class MultiplicationExpression: public Expression {
public:
    MultiplicationExpression(const Expression *left, const Expression *right): Expression(TYPE_NUMBER, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_NUMBER || right->type != TYPE_NUMBER) {
            internal_error("MultiplicationExpression");
        }
    }
    MultiplicationExpression(const MultiplicationExpression &) = delete;
    MultiplicationExpression &operator=(const MultiplicationExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { internal_error("MultiplicationExpression"); }
    virtual Number eval_number() const override { return number_multiply(left->eval_number(), right->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("MultiplicationExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "MultiplicationExpression(" + left->text() + "," + right->text() + ")";
    }
};

class DivisionExpression: public Expression {
public:
    DivisionExpression(const Expression *left, const Expression *right): Expression(TYPE_NUMBER, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_NUMBER || right->type != TYPE_NUMBER) {
            internal_error("DivisionExpression");
        }
    }
    DivisionExpression(const DivisionExpression &) = delete;
    DivisionExpression &operator=(const DivisionExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { internal_error("DivisionExpression"); }
    virtual Number eval_number() const override { return number_divide(left->eval_number(), right->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("DivisionExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "DivisionExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ModuloExpression: public Expression {
public:
    ModuloExpression(const Expression *left, const Expression *right): Expression(TYPE_NUMBER, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_NUMBER || right->type != TYPE_NUMBER) {
            internal_error("ModuloExpression");
        }
    }
    ModuloExpression(const ModuloExpression &) = delete;
    ModuloExpression &operator=(const ModuloExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { internal_error("ModuloExpression"); }
    virtual Number eval_number() const override { return number_modulo(left->eval_number(), right->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("ModuloExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ModuloExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ExponentiationExpression: public Expression {
public:
    ExponentiationExpression(const Expression *left, const Expression *right): Expression(TYPE_NUMBER, left->is_constant && right->is_constant), left(left), right(right) {
        if (left->type != TYPE_NUMBER || right->type != TYPE_NUMBER) {
            internal_error("ExponentiationExpression");
        }
    }
    ExponentiationExpression(const ExponentiationExpression &) = delete;
    ExponentiationExpression &operator=(const ExponentiationExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const left;
    const Expression *const right;

    virtual bool eval_boolean() const override { internal_error("ExponentiationExpression"); }
    virtual Number eval_number() const override { return number_pow(left->eval_number(), right->eval_number()); }
    virtual utf8string eval_string() const override { internal_error("ExponentiationExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ExponentiationExpression(" + left->text() + "," + right->text() + ")";
    }
};

class ReferenceExpression: public Expression {
public:
    ReferenceExpression(const Type *type, bool is_readonly): Expression(type, false, is_readonly) {}
    ReferenceExpression(const ReferenceExpression &) = delete;
    ReferenceExpression &operator=(const ReferenceExpression &) = delete;

    virtual void generate_expr(Emitter &emitter) const override { generate_load(emitter); }
    virtual bool can_generate_address() const { return true; }
    virtual void generate_address_read(Emitter &) const = 0;
    virtual void generate_address_write(Emitter &) const = 0;
    virtual void generate_load(Emitter &) const;
    virtual void generate_store(Emitter &) const;
};

class DummyExpression: public ReferenceExpression {
public:
    DummyExpression(): ReferenceExpression(TYPE_DUMMY, false) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual bool eval_boolean() const override { internal_error("DummyExpression"); }
    virtual Number eval_number() const override { internal_error("DummyExpression"); }
    virtual utf8string eval_string() const override { internal_error("DummyExpression"); }
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
    ArrayReferenceIndexExpression(const ArrayReferenceIndexExpression &) = delete;
    ArrayReferenceIndexExpression &operator=(const ArrayReferenceIndexExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *array;
    const Expression *index;
    const bool always_create;

    virtual bool eval_boolean() const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual utf8string eval_string() const override { internal_error("ArrayReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const override;
    virtual void generate_address_write(Emitter &) const override;

    virtual std::string text() const override { return "ArrayReferenceIndexExpression(" + array->text() + ", " + index->text() + ")"; }
};

class ArrayValueIndexExpression: public Expression {
public:
    ArrayValueIndexExpression(const Type *type, const Expression *array, const Expression *index, bool always_create): Expression(type, false), array(array), index(index), always_create(always_create) {}
    ArrayValueIndexExpression(const ArrayValueIndexExpression &) = delete;
    ArrayValueIndexExpression &operator=(const ArrayValueIndexExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *array;
    const Expression *index;
    const bool always_create;

    virtual bool eval_boolean() const override { internal_error("ArrayValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayValueIndexExpression"); }
    virtual utf8string eval_string() const override { internal_error("ArrayValueIndexExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ArrayValueIndexExpression(" + array->text() + ", " + index->text() + ")"; }
};

class DictionaryReferenceIndexExpression: public ReferenceExpression {
public:
    DictionaryReferenceIndexExpression(const Type *type, const ReferenceExpression *dictionary, const Expression *index): ReferenceExpression(type, dictionary->is_readonly), dictionary(dictionary), index(index) {}
    DictionaryReferenceIndexExpression(const DictionaryReferenceIndexExpression &) = delete;
    DictionaryReferenceIndexExpression &operator=(const DictionaryReferenceIndexExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *dictionary;
    const Expression *index;

    virtual bool eval_boolean() const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual utf8string eval_string() const override { internal_error("DictionaryReferenceIndexExpression"); }
    virtual void generate_address_read(Emitter &) const override;
    virtual void generate_address_write(Emitter &) const override;

    virtual std::string text() const override { return "DictionaryReferenceIndexExpression(" + dictionary->text() + ", " + index->text() + ")"; }
};

class DictionaryValueIndexExpression: public Expression {
public:
    DictionaryValueIndexExpression(const Type *type, const Expression *dictionary, const Expression *index): Expression(type, false), dictionary(dictionary), index(index) {}
    DictionaryValueIndexExpression(const DictionaryValueIndexExpression &) = delete;
    DictionaryValueIndexExpression &operator=(const DictionaryValueIndexExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *dictionary;
    const Expression *index;

    virtual bool eval_boolean() const override { internal_error("DictionaryValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("DictionaryValueIndexExpression"); }
    virtual utf8string eval_string() const override { internal_error("DictionaryValueIndexExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "DictionaryValueIndexExpression(" + dictionary->text() + ", " + index->text() + ")"; }
};

class StringReferenceIndexExpression: public ReferenceExpression {
public:
    StringReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    StringReferenceIndexExpression(const StringReferenceIndexExpression &) = delete;
    StringReferenceIndexExpression &operator=(const StringReferenceIndexExpression &) = delete;
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
    virtual utf8string eval_string() const override { internal_error("StringReferenceIndexExpression"); }
    virtual bool can_generate_address() const override { return false; }
    virtual void generate_address_read(Emitter &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("StringReferenceIndexExpression"); }
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "StringReferenceIndexExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
};

class StringValueIndexExpression: public Expression {
public:
    StringValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    StringValueIndexExpression(const StringValueIndexExpression &) = delete;
    StringValueIndexExpression &operator=(const StringValueIndexExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *str;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual bool eval_boolean() const override { internal_error("StringValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("StringValueIndexExpression"); }
    virtual utf8string eval_string() const override { internal_error("StringValueIndexExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "StringValueIndexExpression(" + str->text() + ", " + first->text() + ", " + last->text() + ")"; }
};

class BytesReferenceIndexExpression: public ReferenceExpression {
public:
    BytesReferenceIndexExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    BytesReferenceIndexExpression(const BytesReferenceIndexExpression &) = delete;
    BytesReferenceIndexExpression &operator=(const BytesReferenceIndexExpression &) = delete;
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
    virtual utf8string eval_string() const override { internal_error("BytesReferenceIndexExpression"); }
    virtual bool can_generate_address() const override { return false; }
    virtual void generate_address_read(Emitter &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("BytesReferenceIndexExpression"); }
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "BytesReferenceIndexExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
};

class BytesValueIndexExpression: public Expression {
public:
    BytesValueIndexExpression(const Expression *str, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    BytesValueIndexExpression(const BytesValueIndexExpression &) = delete;
    BytesValueIndexExpression &operator=(const BytesValueIndexExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *str;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual bool eval_boolean() const override { internal_error("BytesValueIndexExpression"); }
    virtual Number eval_number() const override { internal_error("BytesValueIndexExpression"); }
    virtual utf8string eval_string() const override { internal_error("BytesValueIndexExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "BytesValueIndexExpression(" + str->text() + ", " + first->text() + ", " + last->text() + ")"; }
};

class ObjectSubscriptExpression: public Expression {
public:
    ObjectSubscriptExpression(const Expression *obj, const Expression *index): Expression(TYPE_OBJECT, false), obj(obj), index(index) {}
    ObjectSubscriptExpression(const ObjectSubscriptExpression &) = delete;
    ObjectSubscriptExpression &operator=(const ObjectSubscriptExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *obj;
    const Expression *index;

    virtual bool eval_boolean() const override { internal_error("ObjectSubscriptExpression"); }
    virtual Number eval_number() const override { internal_error("ObjectSubscriptExpression"); }
    virtual utf8string eval_string() const override { internal_error("ObjectSubscriptExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "ObjectSubscriptExpression(" + obj->text() + ", " + index->text() + ")"; }
};

class RecordReferenceFieldExpression: public ReferenceExpression {
public:
    RecordReferenceFieldExpression(const Type *type, const ReferenceExpression *ref, const std::string &field, bool always_create): ReferenceExpression(type, ref->is_readonly), ref(ref), field(field), always_create(always_create) {}
    RecordReferenceFieldExpression(const RecordReferenceFieldExpression &) = delete;
    RecordReferenceFieldExpression &operator=(const RecordReferenceFieldExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *ref;
    const std::string field;
    const bool always_create;

    virtual bool eval_boolean() const override { internal_error("RecordReferenceFieldExpression"); }
    virtual Number eval_number() const override { internal_error("RecordReferenceFieldExpression"); }
    virtual utf8string eval_string() const override { internal_error("RecordReferenceFieldExpression"); }
    virtual void generate_address_read(Emitter &) const override;
    virtual void generate_address_write(Emitter &) const override;

    virtual std::string text() const override { return "RecordReferenceFieldExpression(" + ref->text() + ", " + field + ")"; }
};

class RecordValueFieldExpression: public Expression {
public:
    RecordValueFieldExpression(const Type *type, const Expression *rec, const std::string &field, bool always_create): Expression(type, rec->is_constant, rec->is_readonly), rec(rec), field(field), always_create(always_create) {}
    RecordValueFieldExpression(const RecordValueFieldExpression &) = delete;
    RecordValueFieldExpression &operator=(const RecordValueFieldExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *rec;
    const std::string field;
    const bool always_create;

    virtual bool eval_boolean() const override { internal_error("RecordValueFieldExpression"); }
    virtual Number eval_number() const override { internal_error("RecordValueFieldExpression"); }
    virtual utf8string eval_string() const override { internal_error("RecordValueFieldExpression"); }
    virtual void generate_expr(Emitter &) const override;

    virtual std::string text() const override { return "RecordValueFieldExpression(" + rec->text() + ", " + field + ")"; }
};

class ArrayReferenceRangeExpression: public ReferenceExpression {
public:
    ArrayReferenceRangeExpression(const ReferenceExpression *ref, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    ArrayReferenceRangeExpression(const ArrayReferenceRangeExpression &) = delete;
    ArrayReferenceRangeExpression &operator=(const ArrayReferenceRangeExpression &) = delete;
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
    virtual utf8string eval_string() const override { internal_error("ArrayReferenceRangeExpression"); }
    virtual bool can_generate_address() const override { return false; }
    virtual void generate_address_read(Emitter &) const override { internal_error("StringReferenceRangeExpression"); }
    virtual void generate_address_write(Emitter &) const override { internal_error("StringReferenceRangeExpression"); }
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override;

    virtual std::string text() const override { return "ArrayReferenceRangeExpression(" + ref->text() + ", " + first->text() + ", " + last->text() + ")"; }
};

class ArrayValueRangeExpression: public Expression {
public:
    ArrayValueRangeExpression(const Expression *array, const Expression *first, bool first_from_end, const Expression *last, bool last_from_end, Analyzer *analyzer);
    ArrayValueRangeExpression(const ArrayValueRangeExpression &) = delete;
    ArrayValueRangeExpression &operator=(const ArrayValueRangeExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *array;
    const Expression *first;
    const bool first_from_end;
    const Expression *last;
    const bool last_from_end;

    const FunctionCall *load;

    virtual bool eval_boolean() const override { internal_error("ArrayValueRangeExpression"); }
    virtual Number eval_number() const override { internal_error("ArrayValueRangeExpression"); }
    virtual utf8string eval_string() const override { internal_error("ArrayValueRangeExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "ArrayValueRangeExpression(" + array->text() + ", " + first->text() + ", " + last->text() + ")"; }
};

class PointerDereferenceExpression: public ReferenceExpression {
public:
    PointerDereferenceExpression(const Type *type, const Expression *ptr): ReferenceExpression(type, false), ptr(ptr) {}
    PointerDereferenceExpression(const PointerDereferenceExpression &) = delete;
    PointerDereferenceExpression &operator=(const PointerDereferenceExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *ptr;

    virtual bool eval_boolean() const override { internal_error("PointerDereferenceExpression"); }
    virtual Number eval_number() const override { internal_error("PointerDereferenceExpression"); }
    virtual utf8string eval_string() const override { internal_error("PointerDereferenceExpression"); }
    virtual void generate_address_read(Emitter &emitter) const override;
    virtual void generate_address_write(Emitter &emitter) const override;

    virtual std::string text() const override { return "PointerDereferenceExpression(" + ptr->text() + ")"; }
};

class ConstantExpression: public Expression {
public:
    explicit ConstantExpression(const Constant *constant): Expression(constant->type, true, true), constant(constant) {}
    ConstantExpression(const ConstantExpression &) = delete;
    ConstantExpression &operator=(const ConstantExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Constant *constant;

    virtual bool eval_boolean() const override { return constant->value->eval_boolean(); }
    virtual Number eval_number() const override { return constant->value->eval_number(); }
    virtual utf8string eval_string() const override { return constant->value->eval_string(); }
    virtual void generate_expr(Emitter &emitter) const override { constant->value->generate(emitter); }

    virtual std::string text() const override { return "ConstantExpression(" + constant->text() + ")"; }
};

class VariableExpression: public ReferenceExpression {
public:
    explicit VariableExpression(const Variable *var): ReferenceExpression(var->type, var->is_readonly), var(var) {}
    VariableExpression(const VariableExpression &) = delete;
    VariableExpression &operator=(const VariableExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Variable *var;

    virtual bool eval_boolean() const override { internal_error("VariableExpression"); }
    virtual Number eval_number() const override { internal_error("VariableExpression"); }
    virtual utf8string eval_string() const override { internal_error("VariableExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;
    virtual void generate_call(Emitter &emitter) const override { var->generate_call(emitter); }
    virtual void generate_address_read(Emitter &emitter) const override { var->generate_address(emitter); }
    virtual void generate_address_write(Emitter &emitter) const override { var->generate_address(emitter); }

    virtual std::string text() const override {
        return "VariableExpression(" + var->text() + ")";
    }
};

class InterfaceMethodExpression: public Expression {
public:
    InterfaceMethodExpression(const TypeFunction *functype, size_t index): Expression(functype->returntype, false), functype(functype), index(index) {}
    InterfaceMethodExpression(const InterfaceMethodExpression &) = delete;
    InterfaceMethodExpression &operator=(const InterfaceMethodExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const TypeFunction *functype;
    const size_t index;

    virtual bool eval_boolean() const override { internal_error("InterfaceMethodExpression"); }
    virtual Number eval_number() const override { internal_error("InterfaceMethodExpression"); }
    virtual utf8string eval_string() const override { internal_error("InterfaceMethodExpression"); }
    virtual void generate_expr(Emitter &) const override { internal_error("InterfaceMethodExpression"); }
    virtual void generate_call(Emitter &) const override;

    virtual std::string text() const override { return "InterfaceMethodExpression(" + std::to_string(index) + ")"; }
};

class InterfacePointerConstructor: public Expression {
public:
    InterfacePointerConstructor(const TypeInterfacePointer *type, const Expression *expr, size_t index): Expression(type, false), expr(expr), index(index) {}
    InterfacePointerConstructor(const InterfacePointerConstructor &) = delete;
    InterfacePointerConstructor &operator=(const InterfacePointerConstructor &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const expr;
    const size_t index;

    virtual bool eval_boolean() const override { internal_error("InterfacePointerConstructor"); }
    virtual Number eval_number() const override { internal_error("InterfacePointerConstructor"); }
    virtual utf8string eval_string() const override { internal_error("InterfacePointerConstructor"); }
    virtual void generate_expr(Emitter &) const override;
    virtual void generate_call(Emitter &) const override { internal_error("InterfacePointerConstructor"); }

    virtual std::string text() const override { return "InterfacePointerConstructor(" + std::to_string(index) + ")"; }
};

class InterfacePointerDeconstructor: public Expression {
public:
    explicit InterfacePointerDeconstructor(const Expression *expr): Expression(new TypePointer(Token(), nullptr), false), expr(expr) {}
    InterfacePointerDeconstructor(const InterfacePointerDeconstructor &) = delete;
    InterfacePointerDeconstructor &operator=(const InterfacePointerDeconstructor &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const expr;

    virtual bool eval_boolean() const override { internal_error("InterfacePointerDeconstructor"); }
    virtual Number eval_number() const override { internal_error("InterfacePointerDeconstructor"); }
    virtual utf8string eval_string() const override { internal_error("InterfacePointerDeconstructor"); }
    virtual void generate_expr(Emitter &) const override;
    virtual void generate_call(Emitter &) const override { internal_error("InterfacePointerDeconstructor"); }

    virtual std::string text() const override { return "InterfacePointerDeconstructor()"; }
};

class FunctionCall: public Expression {
public:
    FunctionCall(const Expression *func, const std::vector<const Expression *> &args, const Expression *dispatch = nullptr): Expression(get_expr_type(func), is_intrinsic(func, args)), func(func), dispatch(dispatch), args(args) {}
    FunctionCall(const FunctionCall &) = delete;
    FunctionCall &operator=(const FunctionCall &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const func;
    const Expression *const dispatch;
    const std::vector<const Expression *> args;

    virtual bool eval_boolean() const override;
    virtual Number eval_number() const override;
    virtual utf8string eval_string() const override;
    virtual void generate_expr(Emitter &emitter) const override;
    void generate_parameters(Emitter &emitter) const;
    bool all_in_parameters() const;

    virtual std::string text() const override;
private:
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
    StatementExpression(const Statement *stmt, const Expression *expr): Expression(expr != nullptr ? expr->type : TYPE_NOTHING, false), stmt(stmt), expr(expr) {}
    StatementExpression(const StatementExpression &) = delete;
    StatementExpression &operator=(const StatementExpression &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Statement *const stmt;
    const Expression *const expr;

    virtual bool eval_boolean() const override { internal_error("StatementExpression"); }
    virtual Number eval_number() const override { internal_error("StatementExpression"); }
    virtual utf8string eval_string() const override { internal_error("StatementExpression"); }
    virtual void generate_expr(Emitter &emitter) const override;

    virtual std::string text() const override { return "StatementExpression"; }
};

class Statement: public AstNode {
public:
    explicit Statement(int line): line(line) {}
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
    explicit NullStatement(int line): Statement(line) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void generate_code(Emitter &) const override {}

    virtual std::string text() const override { return "NullStatement"; }
};

class TypeDeclarationStatement: public Statement {
public:
    TypeDeclarationStatement(int line, const std::string &name, const ast::Type *type): Statement(line), name(name), type(type) {}
    TypeDeclarationStatement(const TypeDeclarationStatement &) = delete;
    TypeDeclarationStatement &operator=(const TypeDeclarationStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    const std::string name;
    const ast::Type *type;

    virtual void generate_code(Emitter &) const override;

    virtual std::string text() const override { return "TypeDeclarationStatement(" + name + ", " + type->text() + ")"; }
};

class DeclarationStatement: public Statement {
public:
    DeclarationStatement(int line, Variable *decl): Statement(line), decl(decl) {}
    DeclarationStatement(const DeclarationStatement &) = delete;
    DeclarationStatement &operator=(const DeclarationStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }
    Variable *decl;

    virtual void generate_code(Emitter &) const override {}

    virtual std::string text() const override { return "DeclarationStatement(" + decl->text() + ")"; }
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
    AssertStatement(const AssertStatement &) = delete;
    AssertStatement &operator=(const AssertStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const expr;
    const std::string source;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "AssertStatement(" + expr->text() + ")"; }
};

class AssignmentStatement: public Statement {
public:
    AssignmentStatement(int line, const std::vector<const ReferenceExpression *> &vars, const Expression *expr): Statement(line), variables(vars), expr(expr) {
        for (auto v: variables) {
            if (v->type->make_converter(expr->type) == nullptr) {
                internal_error("AssignmentStatement: found " + expr->type->text() + ", cannot convert to " + v->type->text());
            }
        }
    }
    AssignmentStatement(const AssignmentStatement &) = delete;
    AssignmentStatement &operator=(const AssignmentStatement &) = delete;
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
};

class ExpressionStatement: public Statement {
public:
    ExpressionStatement(int line, const Expression *expr): Statement(line), expr(expr) {}
    ExpressionStatement(const ExpressionStatement &) = delete;
    ExpressionStatement &operator=(const ExpressionStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const expr;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "ExpressionStatement(" + expr->text() + ")";
    }
};

class ReturnStatement: public Statement {
public:
    ReturnStatement(int line, const Expression *expr): Statement(line), expr(expr) {}
    ReturnStatement(const ReturnStatement &) = delete;
    ReturnStatement &operator=(const ReturnStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *const expr;

    virtual bool always_returns() const override { return true; }
    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "ReturnStatement(" + (expr != nullptr ? expr->text() : "") + ")"; }
};

class IncrementStatement: public Statement {
public:
    IncrementStatement(int line, const ReferenceExpression *ref, int delta): Statement(line), ref(ref), delta(delta) {}
    IncrementStatement(const IncrementStatement &) = delete;
    IncrementStatement &operator=(const IncrementStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const ReferenceExpression *ref;
    int delta;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "IncrementStatement(" + ref->text() + ", " + std::to_string(delta) + ")";
    }
};

class IfStatement: public Statement {
public:
    IfStatement(int line, const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> &condition_statements, const std::vector<const Statement *> &else_statements): Statement(line), condition_statements(condition_statements), else_statements(else_statements) {}
    IfStatement(const IfStatement &) = delete;
    IfStatement &operator=(const IfStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<std::pair<const Expression *, std::vector<const Statement *>>> condition_statements;
    const std::vector<const Statement *> else_statements;

    virtual bool always_returns() const override;
    virtual bool is_scope_exit_statement() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "IfStatement(" + condition_statements[0].first->text() + ")";
    }
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
        explicit WhenCondition(const Token &token): token(token) {}
        WhenCondition(const WhenCondition &) = delete;
        WhenCondition &operator=(const WhenCondition &) = delete;
        virtual ~WhenCondition() {}
        const Token token;
        virtual bool overlaps(const WhenCondition *cond) const = 0;
        virtual void generate(Emitter &emitter) const = 0;
    };
    class ComparisonWhenCondition: public WhenCondition {
    public:
        ComparisonWhenCondition(const Token &token, ComparisonExpression::Comparison comp, const Expression *expr): WhenCondition(token), comp(comp), expr(expr) {}
        ComparisonWhenCondition(const ComparisonWhenCondition &) = delete;
        ComparisonWhenCondition &operator=(const ComparisonWhenCondition &) = delete;
        ComparisonExpression::Comparison comp;
        const Expression *expr;
        virtual bool overlaps(const WhenCondition *cond) const override;
        virtual void generate(Emitter &emitter) const override;
    };
    class RangeWhenCondition: public WhenCondition {
    public:
        RangeWhenCondition(const Token &token, const Expression *low_expr, const Expression *high_expr): WhenCondition(token), low_expr(low_expr), high_expr(high_expr) {}
        RangeWhenCondition(const RangeWhenCondition &) = delete;
        RangeWhenCondition &operator=(const RangeWhenCondition &) = delete;
        const Expression *low_expr;
        const Expression *high_expr;
        virtual bool overlaps(const WhenCondition *cond) const override;
        virtual void generate(Emitter &emitter) const override;
    };
    class TypeTestWhenCondition: public WhenCondition {
    public:
        TypeTestWhenCondition(const Token &token, const Expression *expr, const Type *target): WhenCondition(token), expr(expr), target(target) {}
        TypeTestWhenCondition(const TypeTestWhenCondition &) = delete;
        TypeTestWhenCondition &operator=(const TypeTestWhenCondition &) = delete;
        const Expression *expr;
        const Type *target;
        virtual bool overlaps(const WhenCondition *cond) const override;
        virtual void generate(Emitter &emitter) const override;
    };
    CaseStatement(int line, const Expression *expr, const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> &clauses): Statement(line), expr(expr), clauses(clauses) {}
    CaseStatement(const CaseStatement &) = delete;
    CaseStatement &operator=(const CaseStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Expression *expr;
    const std::vector<std::pair<std::vector<const WhenCondition *>, std::vector<const Statement *>>> clauses;

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override {
        return "CaseStatement(" + expr->text() + ")";
    }
};

class ExitStatement: public Statement {
public:
    ExitStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}
    ExitStatement(const ExitStatement &) = delete;
    ExitStatement &operator=(const ExitStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const unsigned int loop_id;

    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "ExitStatement(...)"; }
};

class NextStatement: public Statement {
public:
    NextStatement(int line, unsigned int loop_id): Statement(line), loop_id(loop_id) {}
    NextStatement(const NextStatement &) = delete;
    NextStatement &operator=(const NextStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const unsigned int loop_id;

    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "NextStatement(...)"; }
};

class TryStatement: public Statement {
public:
    TryStatement(int line, const std::vector<const Statement *> &statements, const std::vector<TryTrap> &catches): Statement(line), statements(statements), catches(catches) {}
    TryStatement(const TryStatement &) = delete;
    TryStatement &operator=(const TryStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::vector<const Statement *> statements;
    const std::vector<TryTrap> catches;

    virtual bool always_returns() const override;

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "TryStatement(...)"; }
};

class RaiseStatement: public Statement {
public:
    RaiseStatement(int line, const Exception *exception, const Expression *info): Statement(line), exception(exception), info(info) {}
    RaiseStatement(const RaiseStatement &) = delete;
    RaiseStatement &operator=(const RaiseStatement &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Exception *exception;
    const Expression *info;

    virtual bool always_returns() const override { return true; }
    virtual bool is_scope_exit_statement() const override { return true; }

    virtual void generate_code(Emitter &emitter) const override;

    virtual std::string text() const override { return "RaiseStatement(" + exception->text() + ")"; }
};

class ResetStatement: public Statement {
public:
    ResetStatement(int line, const std::vector<const ReferenceExpression *> &vars): Statement(line), variables(vars) {}
    ResetStatement(const ResetStatement &) = delete;
    ResetStatement &operator=(const ResetStatement &) = delete;
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
};

class BaseFunction: public Variable {
public:
    BaseFunction(const Token &declaration, const std::string &name, const TypeFunction *ftype): Variable(declaration, name, ftype, true), ftype(ftype), function_index(UINT_MAX) {}
    BaseFunction(const BaseFunction &) = delete;
    BaseFunction &operator=(const BaseFunction &) = delete;
    const TypeFunction *ftype;
    mutable unsigned int function_index;

    virtual void reset() override { function_index = UINT_MAX; }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_export(Emitter &emitter, const std::string &name) const override;
};

class Function: public BaseFunction {
public:
    Function(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params, bool variadic, size_t nesting_depth);
    Function(const Function &) = delete;
    Function &operator=(const Function &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    Frame *frame;
    Scope *scope;
    const std::vector<FunctionParameter *> params;
    size_t nesting_depth;

    std::vector<const Statement *> statements;

    static const TypeFunction *makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params, bool variadic);
    int get_stack_delta() const;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void postdeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &) const override {}
    virtual void generate_load(Emitter &) const override;
    virtual void generate_store(Emitter &) const override { internal_error("Function"); }
    virtual void generate_call(Emitter &emitter) const override;

    virtual void debuginfo(Emitter &emitter, minijson::object_writer &out) const;

    virtual std::string text() const override { return "Function(" + name + ", " + type->text() + ")"; }
};

class PredefinedFunction: public BaseFunction {
public:
    PredefinedFunction(const std::string &name, const TypeFunction *ftype): BaseFunction(Token(), name, ftype) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    virtual void reset() override {}
    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("PredefinedFunction"); }
    virtual void generate_call(Emitter &emitter) const override;
    virtual void generate_export(Emitter &, const std::string &) const override {}

    int get_stack_delta() const;

    virtual std::string text() const override { return "PredefinedFunction(" + name + ", " + type->text() + ")"; }
};

class ExtensionFunction: public BaseFunction {
public:
    ExtensionFunction(const Token &declaration, const std::string &module, const std::string &name, const TypeFunction *ftype): BaseFunction(declaration, name, ftype), module(module) {}
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const std::string module;

    virtual void reset() override {}
    virtual void postdeclare(Emitter &) const override;
    virtual void generate_address(Emitter &) const override { internal_error("ExtensionFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("ExtensionFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("ExtensionFunction"); }
    virtual void generate_call(Emitter &emitter) const override;

    virtual std::string text() const override { return "ExtensionFunction(" + module + ", " + name + ", " + type->text() + ")"; }
};

class ModuleFunction: public BaseFunction {
public:
    ModuleFunction(const Module *module, const std::string &name, const TypeFunction *ftype, const std::string &descriptor): BaseFunction(Token(), name, ftype), module(module), name(name), descriptor(descriptor) {}
    ModuleFunction(const ModuleFunction &) = delete;
    ModuleFunction &operator=(const ModuleFunction &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    const Module *module;
    const std::string name;
    const std::string descriptor;

    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_address(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_load(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_store(Emitter &) const override { internal_error("ModuleFunction"); }
    virtual void generate_call(Emitter &emitter) const override;

    int get_stack_delta() const;

    virtual std::string text() const override;
};

class Module: public Name {
public:
    Module(const Token &declaration, Scope *scope, const std::string &name, bool optional): Name(declaration, name, TYPE_MODULE), scope(new Scope(scope, scope->frame)), optional(optional) {}
    Module(const Module &) = delete;
    Module &operator=(const Module &) = delete;
    virtual void accept(IAstVisitor *visitor) const override { visitor->visit(this); }

    Scope *scope;
    bool optional;

    virtual void reset() override { for (size_t i = 0; i < scope->frame->getCount(); i++) { scope->frame->getSlot(i).ref->reset(); } }
    virtual void predeclare(Emitter &emitter) const override;
    virtual void generate_export(Emitter &, const std::string &) const override { internal_error("can't export module"); }

    virtual std::string text() const override { return "Module"; }
};

extern Module *MODULE_MISSING;

class Program: public AstNode {
public:
    Program(const std::string &source_path, const std::string &source_hash, const std::string &module_name);
    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;
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
};

} // namespace ast

#endif
