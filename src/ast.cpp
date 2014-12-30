#include "ast.h"

#include <iostream>
#include <iso646.h>
#include <sstream>

#include "rtl_compile.h"

TypeNothing *TYPE_NOTHING = new TypeNothing();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber();
TypeString *TYPE_STRING = new TypeString();
TypeArray *TYPE_ARRAY_NUMBER = new TypeArray(TYPE_NUMBER);
TypeArray *TYPE_ARRAY_STRING = new TypeArray(TYPE_STRING);
TypePointer *TYPE_POINTER = new TypePointer(nullptr);
TypeModule *TYPE_MODULE = new TypeModule();
TypeException *TYPE_EXCEPTION = new TypeException();

void AstNode::dump(std::ostream &out, int depth) const
{
    out << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(out, depth);
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

Name *Scope::lookupName(const std::string &name)
{
    int enclosing;
    return lookupName(name, enclosing);
}

Name *Scope::lookupName(const std::string &name, int &enclosing)
{
    enclosing = 0;
    Scope *s = this;
    while (s != nullptr) {
        auto n = s->names.find(name);
        if (n != s->names.end()) {
            s->referenced.insert(n->second);
            return n->second;
        }
        enclosing++;
        s = s->parent;
    }
    return nullptr;
}

void Scope::addName(const std::string &name, Name *ref, bool init_referenced)
{
    if (lookupName(name) != nullptr) {
        // If this error occurs, it means a new name was introduced
        // but no check was made with lookupName() to see whether the
        // name already exists yet. This error needs to be detected
        // in the parsing stage so that we have a token available to
        // pass to the normal error function.
        internal_error("name presence not checked");
    }
    names[name] = ref;
    if (init_referenced) {
        referenced.insert(ref);
    }
}

void Scope::scrubName(const std::string &name)
{
    auto i = names.find(name);
    names[std::to_string(reinterpret_cast<intptr_t>(i->second))] = i->second;
    names.erase(i);
    referenced.insert(i->second);
}

int Scope::nextIndex()
{
    return count++;
}

int Scope::getCount() const
{
    return count;
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

const Type *Function::makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params)
{
    std::vector<const ParameterType *> paramtypes;
    for (auto p: params) {
        paramtypes.push_back(new ParameterType(p->mode, p->type));
    }
    return new TypeFunction(returntype, paramtypes);
}

Program::Program()
  : scope(new Scope(nullptr)),
    statements()
{
    scope->addName("Boolean", TYPE_BOOLEAN);
    scope->addName("Number", TYPE_NUMBER);
    scope->addName("String", TYPE_STRING);

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(ParameterType::INOUT, TYPE_STRING));
        TYPE_STRING->methods["length"] = new PredefinedFunction("String.length", new TypeFunction(TYPE_NUMBER, params));
    }

    scope->addName("DivideByZero", new Exception("DivideByZero"));
    scope->addName("ArrayIndex", new Exception("ArrayIndex"));
    scope->addName("DictionaryIndex", new Exception("DictionaryIndex"));
    scope->addName("FunctionNotFound", new Exception("FunctionNotFound"));
    scope->addName("LibraryNotFound", new Exception("LibraryNotFound"));

    rtl_compile_init(scope);
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}
