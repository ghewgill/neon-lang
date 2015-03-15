#include "ast.h"

#include <iostream>
#include <iso646.h>
#include <sstream>

#include "rtl_compile.h"

TypeNothing *TYPE_NOTHING = new TypeNothing();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber(Token());
TypeString *TYPE_STRING = new TypeString();
TypeBytes *TYPE_BYTES = new TypeBytes();
TypeArray *TYPE_ARRAY_NUMBER = new TypeArray(Token(), TYPE_NUMBER);
TypeArray *TYPE_ARRAY_STRING = new TypeArray(Token(), TYPE_STRING);
TypePointer *TYPE_POINTER = new TypePointer(Token(), nullptr);
TypeModule *TYPE_MODULE = new TypeModule();
TypeException *TYPE_EXCEPTION = new TypeException();

void AstNode::dump(std::ostream &out, int depth) const
{
    out << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(out, depth);
}

TypeArray::TypeArray(const Token &declaration, const Type *elementtype)
  : Type(declaration, "array"),
    elementtype(elementtype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this));
        methods["size"] = new PredefinedFunction("array__size", new TypeFunction(TYPE_NUMBER, params));
    }
}

bool TypeArray::is_equivalent(const Type *rhs) const
{
    const TypeArray *a = dynamic_cast<const TypeArray *>(rhs);
    if (a == nullptr) {
        return false;
    }
    return elementtype == nullptr || a->elementtype == nullptr || elementtype->is_equivalent(a->elementtype);
}

bool TypeDictionary::is_equivalent(const Type *rhs) const
{
    const TypeDictionary *d = dynamic_cast<const TypeDictionary *>(rhs);
    if (d == nullptr) {
        return false;
    }
    return elementtype == nullptr || d->elementtype == nullptr || elementtype->is_equivalent(d->elementtype);
}

bool TypeRecord::is_equivalent(const Type *rhs) const
{
    return this == rhs;
}

bool TypePointer::is_equivalent(const Type *rhs) const
{
    const TypePointer *p = dynamic_cast<const TypePointer *>(rhs);
    if (p == nullptr) {
        return false;
    }
    if (reftype == nullptr || p->reftype == nullptr) {
        return true;
    }
    // Shortcut check avoids infinite recursion on records with pointer to itself.
    return reftype == p->reftype || reftype->is_equivalent(p->reftype);
}

std::string ConstantBooleanExpression::text() const
{
    std::stringstream s;
    s << "ConstantBooleanExpression(" << value << ")";
    return s.str();
}

std::string ConstantNumberExpression::text() const
{
    std::stringstream s;
    s << "ConstantNumberExpression(" << number_to_string(value) << ")";
    return s.str();
}

std::string ConstantStringExpression::text() const
{
    std::stringstream s;
    s << "ConstantStringExpression(" << value << ")";
    return s.str();
}

std::string ConstantEnumExpression::text() const
{
    std::stringstream s;
    s << "ConstantEnumExpression(" << value << ")";
    return s.str();
}

bool ArrayLiteralExpression::all_constant(const std::vector<const Expression *> &elements)
{
    for (auto e: elements) {
        if (not e->is_constant) {
            return false;
        }
    }
    return true;
}

bool DictionaryLiteralExpression::all_constant(const std::vector<std::pair<std::string, const Expression *>> &elements)
{
    for (auto e: elements) {
        if (not e.second->is_constant) {
            return false;
        }
    }
    return true;
}

bool RecordLiteralExpression::all_constant(const std::vector<const Expression *> &elements)
{
    for (auto e: elements) {
        if (not e->is_constant) {
            return false;
        }
    }
    return true;
}

std::map<std::string, const Expression *> DictionaryLiteralExpression::make_dictionary(const std::vector<std::pair<std::string, const Expression *>> &elements)
{
    std::map<std::string, const Expression *> dict;
    for (auto e: elements) {
        dict[e.first] = e.second;
    }
    return dict;
}

bool IfStatement::always_returns() const
{
    for (auto cond: condition_statements) {
        if (cond.second.empty() || not cond.second.back()->always_returns()) {
            return false;
        }
    }
    if (else_statements.empty() || not else_statements.back()->always_returns()) {
        return false;
    }
    return true;
}

bool LoopStatement::always_returns() const
{
    for (auto s: statements) {
        if (dynamic_cast<const ExitStatement *>(s) != nullptr) {
            return false;
        }
    }
    return true;
}

bool CaseStatement::always_returns() const
{
    for (auto clause: clauses) {
        if (clause.second.empty() || not clause.second.back()->always_returns()) {
            return false;
        }
    }
    return true;
}

bool TryStatement::always_returns() const
{
    if (statements.empty() || not statements.back()->always_returns()) {
        return false;
    }
    for (auto c: catches) {
        if (c.second.empty() || not c.second.back()->always_returns()) {
            return false;
        }
    }
    return true;
}

std::string FunctionCall::text() const
{
    std::stringstream s;
    s << "FunctionCall(" << func->text() << ", [";
    for (std::vector<const Expression *>::const_iterator i = args.begin(); i != args.end(); ++i) {
        s << (*i)->text();
        if (i+1 != args.end()) {
            s << ", ";
        }
    }
    s << "])";
    return s.str();
}

void CompoundStatement::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}

int Frame::addSlot(const Token &token, const std::string &name, Name *ref, bool init_referenced)
{
    Slot slot(token, name, ref, init_referenced);
    int r = static_cast<int>(slots.size());
    slots.push_back(slot);
    return r;
}

const Frame::Slot Frame::getSlot(int slot)
{
    return slots.at(slot);
}

void Frame::setReferent(int slot, Name *ref)
{
    if (slots.at(slot).ref != nullptr) {
        internal_error("ref not null");
    }
    slots.at(slot).ref = ref;
}

void Frame::setReferenced(int slot)
{
    slots.at(slot).referenced = true;
}

bool Scope::allocateName(const Token &token, const std::string &name)
{
    if (getDeclaration(name).type != NONE) {
        return false;
    }
    names[name] = frame->addSlot(token, name, nullptr, false);
    return true;
}

Name *Scope::lookupName(const std::string &name)
{
    Scope *s = this;
    while (s != nullptr) {
        auto n = s->names.find(name);
        if (n != s->names.end()) {
            s->frame->setReferenced(n->second);
            return s->frame->getSlot(n->second).ref;
        }
        s = s->parent;
    }
    return nullptr;
}

Token Scope::getDeclaration(const std::string &name) const
{
    const Scope *s = this;
    while (s != nullptr) {
        auto d = s->names.find(name);
        if (d != s->names.end()) {
            return s->frame->getSlot(d->second).token;
        }
        s = s->parent;
    }
    return Token();
}

void Scope::addName(const Token &token, const std::string &name, Name *ref, bool init_referenced)
{
    if (lookupName(name) != nullptr) {
        // If this error occurs, it means a new name was introduced
        // but no check was made with lookupName() to see whether the
        // name already exists yet. This error needs to be detected
        // in the parsing stage so that we have a token available to
        // pass to the normal error function.
        internal_error("name presence not checked: " + name);
    }
    auto a = names.find(name);
    if (a != names.end()) {
        frame->setReferent(a->second, ref);
        if (init_referenced) {
            frame->setReferenced(a->second);
        }
    } else {
        names[name] = frame->addSlot(token, name, ref, init_referenced);
    }
}

void Scope::addForward(const std::string &name, TypePointer *ptrtype)
{
    forwards[name].push_back(ptrtype);
}

void Scope::resolveForward(const std::string &name, const TypeRecord *rectype)
{
    auto i = forwards.find(name);
    if (i != forwards.end()) {
        for (auto p: i->second) {
            delete p->reftype;
            p->reftype = rectype;
        }
        forwards.erase(i);
    }
}

void Scope::checkForward()
{
    if (not forwards.empty()) {
        // TODO: Make this a real error that points to a token.
        internal_error("unresolved forward declaration: " + forwards.begin()->first);
    }
}

Function::Function(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params)
  : Variable(declaration, name, makeFunctionType(returntype, params), true),
    frame(new Frame(outer)),
    scope(new Scope(parent, frame)),
    params(params),
    entry_label(UINT_MAX),
    statements()
{
    for (auto p: params) {
        scope->addName(p->declaration, p->name, p, true);
    }
}

const Type *Function::makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params)
{
    std::vector<const ParameterType *> paramtypes;
    for (auto p: params) {
        paramtypes.push_back(new ParameterType(p->declaration, p->mode, p->type));
    }
    return new TypeFunction(returntype, paramtypes);
}

Program::Program()
  : frame(new Frame(nullptr)),
    scope(new Scope(nullptr, frame)),
    statements()
{
    scope->addName(Token(), "Boolean", TYPE_BOOLEAN);
    scope->addName(Token(), "Number", TYPE_NUMBER);
    scope->addName(Token(), "String", TYPE_STRING);
    scope->addName(Token(), "Bytes", TYPE_BYTES);

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BOOLEAN));
        TYPE_BOOLEAN->methods["to_string"] = new PredefinedFunction("boolean__to_string", new TypeFunction(TYPE_STRING, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_NUMBER));
        TYPE_NUMBER->methods["to_string"] = new PredefinedFunction("number__to_string", new TypeFunction(TYPE_STRING, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_STRING));
        TYPE_STRING->methods["length"] = new PredefinedFunction("string__length", new TypeFunction(TYPE_NUMBER, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::INOUT, TYPE_BYTES));
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_ARRAY_NUMBER));
        TYPE_BYTES->methods["from_array"] = new PredefinedFunction("bytes__from_array", new TypeFunction(TYPE_NOTHING, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BYTES));
        TYPE_BYTES->methods["size"] = new PredefinedFunction("bytes__size", new TypeFunction(TYPE_NUMBER, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BYTES));
        TYPE_BYTES->methods["to_array"] = new PredefinedFunction("bytes__to_array", new TypeFunction(TYPE_ARRAY_NUMBER, params));
    }

    scope->addName(Token(), "DivideByZero", new Exception(Token(), "DivideByZero"));
    scope->addName(Token(), "ArrayIndex", new Exception(Token(), "ArrayIndex"));
    scope->addName(Token(), "DictionaryIndex", new Exception(Token(), "DictionaryIndex"));
    scope->addName(Token(), "FunctionNotFound", new Exception(Token(), "FunctionNotFound"));
    scope->addName(Token(), "LibraryNotFound", new Exception(Token(), "LibraryNotFound"));
    scope->addName(Token(), "ByteOutOfRange", new Exception(Token(), "ByteOutOfRange"));

    scope->addName(Token(), "DirectoryExists", new Exception(Token(), "DirectoryExists"));
    scope->addName(Token(), "PathNotFound", new Exception(Token(), "PathNotFound"));
    scope->addName(Token(), "PermissionDenied", new Exception(Token(), "PermissionDenied"));

    {
        // The fields here must match the corresponding references to
        // ExceptionType in exec.cpp.
        std::vector<std::pair<Token, const Type *>> fields;
        fields.push_back(std::make_pair(Token("name"), TYPE_STRING));
        fields.push_back(std::make_pair(Token("info"), TYPE_STRING));
        fields.push_back(std::make_pair(Token("offset"), TYPE_NUMBER));
        Type *exception_type = new TypeRecord(Token(), fields);
        scope->addName(Token(), "ExceptionType", exception_type, true);
        GlobalVariable *current_exception = new GlobalVariable(Token(), "CURRENT_EXCEPTION", exception_type, true);
        scope->addName(Token(), "CURRENT_EXCEPTION", current_exception, true);
    }

    rtl_compile_init(scope);
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}
