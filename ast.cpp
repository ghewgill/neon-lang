#include "ast.h"

#include <iostream>
#include <iso646.h>
#include <sstream>

#include "rtl_compile.h"

TypeNothing *TYPE_NOTHING = new TypeNothing();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber();
TypeString *TYPE_STRING = new TypeString();
TypeArray *TYPE_ARRAY_STRING = new TypeArray(TYPE_STRING);
TypeModule *TYPE_MODULE = new TypeModule();

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
    return elementtype->is_equivalent(a->elementtype);
}

bool TypeDictionary::is_equivalent(const Type *rhs) const
{
    const TypeDictionary *d = dynamic_cast<const TypeDictionary *>(rhs);
    if (d == nullptr) {
        return false;
    }
    return elementtype->is_equivalent(d->elementtype);
}

bool TypeRecord::is_equivalent(const Type *rhs) const
{
    const TypeRecord *r = dynamic_cast<const TypeRecord *>(rhs);
    if (r == nullptr) {
        return false;
    }
    if (fields.size() != r->fields.size()) {
        return false;
    }
    for (auto f: fields) {
        auto g = r->fields.find(f.first);
        // Check name.
        if (g == r->fields.end()) {
            return false;
        }
        // Check position.
        if (f.second.first != g->second.first) {
            return false;
        }
        // Check type.
        if (not f.second.second->is_equivalent(g->second.second)) {
            return false;
        }
    }
    return true;
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

const Name *Scope::lookupName(const std::string &name)
{
    Scope *s = this;
    while (s != nullptr) {
        auto n = s->names.find(name);
        if (n != s->names.end()) {
            s->referenced.insert(n->second);
            return n->second;
        }
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

int Scope::nextIndex()
{
    return count++;
}

int Scope::getCount() const
{
    return count;
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

    rtl_compile_init(scope);
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}
