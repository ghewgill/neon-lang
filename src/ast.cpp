#include "ast.h"

#include <iostream>
#include <iso646.h>
#include <sstream>
#include <string.h>

#include "intrinsic.h"
#include "rtl_compile.h"
#include "rtl_exec.h"

TypeNothing *TYPE_NOTHING = new TypeNothing();
TypeDummy *TYPE_DUMMY = new TypeDummy();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber(Token());
TypeString *TYPE_STRING = new TypeString();
TypeBytes *TYPE_BYTES = new TypeBytes();
TypeArray *TYPE_ARRAY_NUMBER = new TypeArray(Token(), TYPE_NUMBER);
TypeArray *TYPE_ARRAY_STRING = new TypeArray(Token(), TYPE_STRING);
TypeDictionary *TYPE_DICTIONARY_STRING = new TypeDictionary(Token(), TYPE_STRING);
TypeModule *TYPE_MODULE = new TypeModule();
TypeException *TYPE_EXCEPTION = new TypeException();

void AstNode::dump(std::ostream &out, int depth) const
{
    out << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(out, depth);
}

const Expression *TypeBoolean::make_default_value() const
{
    return new ConstantBooleanExpression(false);
}

std::string TypeBoolean::serialize(const Expression *value) const
{
    return value->eval_boolean() ? std::string(1, 1) : std::string(1, 0);
}

const Expression *TypeBoolean::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    unsigned char b = value.at(i);
    i++;
    return new ConstantBooleanExpression(b != 0);
}

const Expression *TypeNumber::make_default_value() const
{
    return new ConstantNumberExpression(number_from_uint32(0));
}

std::string TypeNumber::serialize(const Expression *value) const
{
    Number x = value->eval_number();
    return std::string(reinterpret_cast<const char *>(&x), sizeof(x));
}

const Expression *TypeNumber::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    // TODO: endian
    Number x;
    memcpy(&x, &value.at(i), sizeof(Number));
    i += sizeof(Number);
    return new ConstantNumberExpression(x);
}

const Expression *TypeString::make_default_value() const
{
    return new ConstantStringExpression("");
}

std::string TypeString::serialize(const std::string &value)
{
    uint32_t len = static_cast<uint32_t>(value.length());
    std::string r;
    r.push_back(static_cast<unsigned char>(len >> 24) & 0xff);
    r.push_back(static_cast<unsigned char>(len >> 16) & 0xff);
    r.push_back(static_cast<unsigned char>(len >> 8) & 0xff);
    r.push_back(static_cast<unsigned char>(len & 0xff));
    return r + value;
}

std::string TypeString::serialize(const Expression *value) const
{
    return serialize(value->eval_string());
}

std::string TypeString::deserialize_string(const Bytecode::Bytes &value, int &i)
{
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    std::string s(&value.at(i+4), &value.at(i+4)+len);
    i += 4 + len;
    return s;
}

const Expression *TypeString::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    return new ConstantStringExpression(deserialize_string(value, i));
}

TypeArray::TypeArray(const Token &declaration, const Type *elementtype)
  : Type(declaration, ""),
    elementtype(elementtype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::IN, elementtype, nullptr));
        methods["append"] = new PredefinedFunction("array__append", new TypeFunction(TYPE_NOTHING, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        methods["extend"] = new PredefinedFunction("array__extend", new TypeFunction(TYPE_NOTHING, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_NUMBER, nullptr));
        methods["resize"] = new PredefinedFunction("array__resize", new TypeFunction(TYPE_NOTHING, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        methods["size"] = new PredefinedFunction("array__size", new TypeFunction(TYPE_NUMBER, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        // TODO: This is just a hack to make this work for now.
        // Need to do this properly in a general purpose way.
        if (elementtype == TYPE_NUMBER) {
            methods["toBytes"] = new PredefinedFunction("array__toBytes__number", new TypeFunction(TYPE_BYTES, params));
            methods["toString"] = new PredefinedFunction("array__toString__number", new TypeFunction(TYPE_STRING, params));
        } else if (elementtype == TYPE_STRING) {
            methods["toString"] = new PredefinedFunction("array__toString__string", new TypeFunction(TYPE_STRING, params));
        }
    }
}

const Expression *TypeArray::make_default_value() const
{
    return new ArrayLiteralExpression(nullptr, {});
}

bool TypeFunction::is_assignment_compatible(const Type *rhs) const
{
    // TODO: There needs to be a mechanism for reporting more detail about why the
    // type does not match. There are quite a few reasons for this to return false,
    // and the user would probably appreciate more detail.
    const TypeFunction *f = dynamic_cast<const TypeFunction *>(rhs);
    if (f == nullptr) {
        const TypeFunctionPointer *p = dynamic_cast<const TypeFunctionPointer *>(rhs);
        if (p == nullptr) {
            return false;
        }
        f = p->functype;
        if (f == nullptr) {
            return true;
        }
    }
    if (returntype != TYPE_NOTHING && f->returntype != TYPE_NOTHING && not returntype->is_assignment_compatible(f->returntype)) {
        return false;
    }
    if (params.size() != f->params.size()) {
        return false;
    }
    for (size_t i = 0; i < params.size(); i++) {
        if (params[i]->declaration.text != f->params[i]->declaration.text) {
            return false;
        }
        if (params[i]->mode != f->params[i]->mode) {
            return false;
        }
        if (not f->params[i]->type->is_assignment_compatible(params[i]->type)) {
            return false;
        }
    }
    return true;
}

bool TypeArray::is_assignment_compatible(const Type *rhs) const
{
    const TypeArray *a = dynamic_cast<const TypeArray *>(rhs);
    if (a == nullptr) {
        return false;
    }
    return elementtype == nullptr || a->elementtype == nullptr || elementtype->is_assignment_compatible(a->elementtype);
}

std::string TypeArray::serialize(const Expression *value) const
{
    std::string r;
    const ArrayLiteralExpression *a = dynamic_cast<const ArrayLiteralExpression *>(value);
    r.push_back(static_cast<unsigned char>(a->elements.size() >> 24) & 0xff);
    r.push_back(static_cast<unsigned char>(a->elements.size() >> 16) & 0xff);
    r.push_back(static_cast<unsigned char>(a->elements.size() >> 8) & 0xff);
    r.push_back(static_cast<unsigned char>(a->elements.size() & 0xff));
    for (auto x: a->elements) {
        r.append(a->elementtype->serialize(x));
    }
    return r;
}

const Expression *TypeArray::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    std::vector<const Expression *> elements;
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    i += 4;
    while (len > 0) {
        elements.push_back(elementtype->deserialize_value(value, i));
        len--;
    }
    return new ArrayLiteralExpression(elementtype, elements);
}

TypeDictionary::TypeDictionary(const Token &declaration, const Type *elementtype)
  : Type(declaration, ""),
    elementtype(elementtype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        methods["size"] = new PredefinedFunction("dictionary__size", new TypeFunction(TYPE_NUMBER, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        methods["keys"] = new PredefinedFunction("dictionary__keys", new TypeFunction(TYPE_ARRAY_STRING, params));
    }
}

const Expression *TypeDictionary::make_default_value() const
{
    return new DictionaryLiteralExpression(nullptr, {});
}

bool TypeDictionary::is_assignment_compatible(const Type *rhs) const
{
    const TypeDictionary *d = dynamic_cast<const TypeDictionary *>(rhs);
    if (d == nullptr) {
        return false;
    }
    return elementtype == nullptr || d->elementtype == nullptr || elementtype->is_assignment_compatible(d->elementtype);
}

std::string TypeDictionary::serialize(const Expression *value) const
{
    std::string r;
    const DictionaryLiteralExpression *d = dynamic_cast<const DictionaryLiteralExpression *>(value);
    r.push_back(static_cast<unsigned char>(d->dict.size() >> 24) & 0xff);
    r.push_back(static_cast<unsigned char>(d->dict.size() >> 16) & 0xff);
    r.push_back(static_cast<unsigned char>(d->dict.size() >> 8) & 0xff);
    r.push_back(static_cast<unsigned char>(d->dict.size() & 0xff));
    for (auto x: d->dict) {
        r.append(TypeString::serialize(x.first));
        r.append(d->elementtype->serialize(x.second));
    }
    return r;
}

const Expression *TypeDictionary::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    std::vector<std::pair<std::string, const Expression *>> dict;
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    i += 4;
    while (len > 0) {
        std::string name = TypeString::deserialize_string(value, i);
        dict.push_back(std::make_pair(name, elementtype->deserialize_value(value, i)));
        len--;
    }
    return new DictionaryLiteralExpression(elementtype, dict);
}

const Expression *TypeRecord::make_default_value() const
{
    return new ArrayLiteralExpression(nullptr, {});
}

bool TypeRecord::is_assignment_compatible(const Type *rhs) const
{
    return this == rhs;
}

std::string TypeRecord::serialize(const Expression *value) const
{
    std::string r;
    const RecordLiteralExpression *a = dynamic_cast<const RecordLiteralExpression *>(value);
    r.push_back(static_cast<unsigned char>(a->values.size() >> 24) & 0xff);
    r.push_back(static_cast<unsigned char>(a->values.size() >> 16) & 0xff);
    r.push_back(static_cast<unsigned char>(a->values.size() >> 8) & 0xff);
    r.push_back(static_cast<unsigned char>(a->values.size() & 0xff));
    for (auto x: a->values) {
        r.append(x->type->serialize(x));
    }
    return r;
}

const Expression *TypeRecord::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    std::vector<const Expression *> elements;
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    i += 4;
    int f = 0;
    while (len > 0) {
        elements.push_back(fields[f].type->deserialize_value(value, i));
        f++;
        len--;
    }
    return new RecordLiteralExpression(this, elements);
}

std::string TypeRecord::text() const
{
    std::string r = "TypeRecord(";
    bool first = true;
    for (auto f: fields) {
        if (not first) {
            r.append(",");
        }
        first = false;
        r.append(f.name.text);
    }
    r.append(")");
    return r;
}

TypePointer::TypePointer(const Token &declaration, const TypeClass *reftype)
  : Type(declaration, ""),
    reftype(reftype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        methods["toString"] = new PredefinedFunction("pointer__toString", new TypeFunction(TYPE_STRING, params));
    }
}

const Expression *TypePointer::make_default_value() const
{
    return new ConstantNilExpression();
}

bool TypePointer::is_assignment_compatible(const Type *rhs) const
{
    if (this == rhs) {
        return true;
    }
    if (dynamic_cast<const TypePointerNil *>(rhs) != nullptr) {
        return true;
    }
    const TypePointer *p = dynamic_cast<const TypePointer *>(rhs);
    if (p == nullptr) {
        return false;
    }
    if (reftype == nullptr || p->reftype == nullptr) {
        return false;
    }
    // Shortcut check avoids infinite recursion on records with pointer to itself.
    return reftype == p->reftype || reftype->is_assignment_compatible(p->reftype);
}

std::string TypePointer::serialize(const Expression *) const
{
    return std::string();
}

const Expression *TypePointer::deserialize_value(const Bytecode::Bytes &, int &) const
{
    return new ConstantNilExpression();
}

bool TypeValidPointer::is_assignment_compatible(const Type *rhs) const
{
    if (this == rhs) {
        return true;
    }
    if (not TypePointer::is_assignment_compatible(rhs)) {
        return false;
    }
    if (dynamic_cast<const TypeValidPointer *>(rhs) == nullptr) {
        return false;
    }
    return true;
}

TypeFunctionPointer::TypeFunctionPointer(const Token &declaration, const TypeFunction *functype)
  : Type(declaration, ""),
    functype(functype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, this, nullptr));
        methods["toString"] = new PredefinedFunction("functionpointer__toString", new TypeFunction(TYPE_STRING, params));
    }
}

const Expression *TypeFunctionPointer::make_default_value() const
{
    return new ConstantNowhereExpression();
}

bool TypeFunctionPointer::is_assignment_compatible(const Type *rhs) const
{
    return functype != nullptr && functype->is_assignment_compatible(rhs);
}

std::string TypeFunctionPointer::serialize(const Expression *) const
{
    return std::string();
}

const Expression *TypeFunctionPointer::deserialize_value(const Bytecode::Bytes &, int &) const
{
    return new ConstantNumberExpression(number_from_sint32(0));
}

bool Expression::eval_boolean(const Token &token) const
{
    try {
        return eval_boolean();
    } catch (RtlException &e) {
        error(3195, token, "Boolean evaluation exception: " + e.name + ": " + e.info);
    }
}

Number Expression::eval_number(const Token &token) const
{
    try {
        return eval_number();
    } catch (RtlException &e) {
        error(3196, token, "Numeric evaluation exception: " + e.name + ": " + e.info);
    }
}

std::string Expression::eval_string(const Token &token) const
{
    try {
        return eval_string();
    } catch (RtlException &e) {
        error(3197, token, "String evaluation exception: " + e.name + ": " + e.info);
    }
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

std::string ConstantBytesExpression::text() const
{
    std::stringstream s;
    s << "ConstantBytesExpression(" << name << ")";
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

bool BooleanComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case EQ: return left->eval_boolean() == right->eval_boolean();
        case NE: return left->eval_boolean() != right->eval_boolean();
        case LT:
        case GT:
        case LE:
        case GE:
            internal_error("BooleanComparisonExpression");
    }
    internal_error("BooleanComparisonExpression");
}

bool NumericComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case EQ: return number_is_equal        (left->eval_number(), right->eval_number());
        case NE: return number_is_not_equal    (left->eval_number(), right->eval_number());
        case LT: return number_is_less         (left->eval_number(), right->eval_number());
        case GT: return number_is_greater      (left->eval_number(), right->eval_number());
        case LE: return number_is_less_equal   (left->eval_number(), right->eval_number());
        case GE: return number_is_greater_equal(left->eval_number(), right->eval_number());
    }
    internal_error("NumericComparisonExpression");
}

bool StringComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case EQ: return left->eval_string() == right->eval_string();
        case NE: return left->eval_string() != right->eval_string();
        case LT: return left->eval_string() <  right->eval_string();
        case GT: return left->eval_string() >  right->eval_string();
        case LE: return left->eval_string() <= right->eval_string();
        case GE: return left->eval_string() >= right->eval_string();
    }
    internal_error("StringComparisonExpression");
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

bool IfStatement::is_scope_exit_statement() const
{
    for (auto cond: condition_statements) {
        if (cond.second.empty() || not cond.second.back()->is_scope_exit_statement()) {
            return false;
        }
    }
    if (else_statements.empty() || not else_statements.back()->is_scope_exit_statement()) {
        return false;
    }
    return true;
}

bool BaseLoopStatement::always_returns() const
{
    // TODO: This doesn't look deep enough for exit statements, eg:
    //
    //  LOOP
    //      IF condition THEN
    //          EXIT LOOP
    //      END IF
    //  END LOOP
    for (auto s: statements) {
        if (dynamic_cast<const ExitStatement *>(s) != nullptr) {
            return false;
        }
    }
    return infinite_loop;
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
        const ExceptionHandlerStatement *ehs = dynamic_cast<const ExceptionHandlerStatement *>(c.second);
        if (ehs == nullptr) {
            return false;
        }
        if (ehs->statements.empty() || not ehs->statements.back()->always_returns()) {
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

Number FunctionCall::eval_number() const
{
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    const PredefinedFunction *f = dynamic_cast<const PredefinedFunction *>(ve->var);
    if (f->name == "ord") return rtl::global::ord(args[0]->eval_string());
    if (f->name == "int") return rtl::global::int_(args[0]->eval_number());
    if (f->name == "max") return rtl::global::max(args[0]->eval_number(), args[1]->eval_number());
    if (f->name == "min") return rtl::global::min(args[0]->eval_number(), args[1]->eval_number());
    if (f->name == "num") return rtl::global::num(args[0]->eval_string());
    internal_error("unexpected intrinsic");
}

std::string FunctionCall::eval_string() const
{
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    const PredefinedFunction *f = dynamic_cast<const PredefinedFunction *>(ve->var);
    if (f->name == "chr") return rtl::global::chr(args[0]->eval_number());
    if (f->name == "concat") return rtl::global::concat(args[0]->eval_string(), args[1]->eval_string());
    if (f->name == "format") return rtl::global::format(args[0]->eval_string(), args[1]->eval_string());
    if (f->name == "str") return rtl::global::str(args[0]->eval_number());
    if (f->name == "strb") return rtl::global::strb(args[0]->eval_boolean());
    if (f->name == "substring") return rtl::global::substring(args[0]->eval_string(), args[1]->eval_number(), args[2]->eval_number());
    internal_error("unexpected intrinsic");
}

bool FunctionCall::is_intrinsic(const Expression *func, const std::vector<const Expression *> &args)
{
    for (auto a: args) {
        if (not a->is_constant) {
            return false;
        }
    }
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    if (ve == nullptr) {
        return false;
    }
    const PredefinedFunction *f = dynamic_cast<const PredefinedFunction *>(ve->var);
    if (f == nullptr) {
        return false;
    }
    if (f->name == "chr"
     || f->name == "concat"
     || f->name == "format"
     || f->name == "int"
     || f->name == "max"
     || f->name == "min"
     || f->name == "num"
     || f->name == "odd"
     || f->name == "ord"
     || f->name == "str"
     || f->name == "strb"
     || f->name == "substring") {
        return true;
    }
    return false;
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

const Frame::Slot Frame::getSlot(size_t slot)
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

Variable *ExternalGlobalFrame::createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly)
{
    return new ExternalGlobalVariable(token, name, type, is_readonly);
}

int ExternalGlobalFrame::addSlot(const Token &token, const std::string &name, Name *ref, bool init_referenced)
{
    external_globals.insert(std::make_pair(name, ExternalGlobalInfo(token, ref, init_referenced)));
    return Frame::addSlot(token, name, ref, init_referenced);
}

void ExternalGlobalFrame::setReferent(int slot, Name *ref)
{
    auto g = external_globals.find(ref->name);
    if (g == external_globals.end()) {
        internal_error("external global does not exist");
    }
    g->second.ref = ref;
    Frame::setReferent(slot, ref);
}

Variable *GlobalFrame::createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly)
{
    return new GlobalVariable(token, name, type, is_readonly);
}

Variable *LocalFrame::createVariable(const Token &token, const std::string &name, const Type *type, bool is_readonly)
{
    return new LocalVariable(token, name, type, nesting_depth, is_readonly);
}

bool Scope::allocateName(const Token &token, const std::string &name)
{
    if (getDeclaration(name).type != NONE) {
        return false;
    }
    names[name] = frame->addSlot(token, name, nullptr, false);
    return true;
}

Name *Scope::lookupName(const std::string &name, bool mark_referenced)
{
    Scope *s = this;
    while (s != nullptr) {
        auto n = s->names.find(name);
        if (n != s->names.end()) {
            if (mark_referenced) {
                s->frame->setReferenced(n->second);
            }
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

void Scope::addName(const Token &token, const std::string &name, Name *ref, bool init_referenced, bool allow_shadow)
{
    if (token.type == NONE) {
        internal_error("no token for name: " + name + " " + ref->text());
    }
    if (not allow_shadow and lookupName(name, false) != nullptr) {
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

void Scope::resolveForward(const std::string &name, const TypeClass *classtype)
{
    auto i = forwards.find(name);
    if (i != forwards.end()) {
        for (auto p: i->second) {
            delete p->reftype;
            p->reftype = classtype;
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

ExternalGlobalScope::ExternalGlobalScope(Scope *parent, Frame *frame, std::map<std::string, ExternalGlobalInfo> &external_globals)
  : Scope(parent, frame)
{
    for (auto &g: external_globals) {
        g.second.ref->reset();
        Scope::addName(g.second.declaration, g.first, g.second.ref, g.second.init_referenced);
    }
}

Function::Function(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params, size_t nesting_depth)
  : Variable(declaration, name, makeFunctionType(returntype, params), true),
    frame(new LocalFrame(outer)),
    scope(new Scope(parent, frame)),
    params(params),
    nesting_depth(nesting_depth),
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
        paramtypes.push_back(new ParameterType(p->declaration, p->mode, p->type, p->default_value));
    }
    return new TypeFunction(returntype, paramtypes);
}

Program::Program(const std::string &source_path, const std::string &source_hash)
  : source_path(source_path),
    source_hash(source_hash),
    frame(new GlobalFrame(nullptr)),
    scope(new Scope(nullptr, frame)),
    statements(),
    exports()
{
    scope->addName(Token(IDENTIFIER, "Boolean"), "Boolean", TYPE_BOOLEAN);
    scope->addName(Token(IDENTIFIER, "Number"), "Number", TYPE_NUMBER);
    scope->addName(Token(IDENTIFIER, "String"), "String", TYPE_STRING);
    scope->addName(Token(IDENTIFIER, "Bytes"), "Bytes", TYPE_BYTES);

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BOOLEAN, nullptr));
        TYPE_BOOLEAN->methods["toString"] = new PredefinedFunction("boolean__toString", new TypeFunction(TYPE_STRING, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_NUMBER, nullptr));
        TYPE_NUMBER->methods["toString"] = new PredefinedFunction("number__toString", new TypeFunction(TYPE_STRING, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["length"] = new PredefinedFunction("string__length", new TypeFunction(TYPE_NUMBER, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::INOUT, TYPE_STRING, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["append"] = new PredefinedFunction("string__append", new TypeFunction(TYPE_NOTHING, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["toBytes"] = new PredefinedFunction("string__toBytes", new TypeFunction(TYPE_BYTES, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["toString"] = new PredefinedFunction("string__toString", new TypeFunction(TYPE_STRING, params));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::INOUT, TYPE_BYTES, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_ARRAY_NUMBER, nullptr));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["size"] = new PredefinedFunction("bytes__size", new TypeFunction(TYPE_NUMBER, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["decodeToString"] = new PredefinedFunction("bytes__decodeToString", new TypeFunction(TYPE_STRING, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["toArray"] = new PredefinedFunction("bytes__toArray", new TypeFunction(TYPE_ARRAY_NUMBER, params));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["toString"] = new PredefinedFunction("bytes__toString", new TypeFunction(TYPE_STRING, params));
    }

    for (auto e: rtl::ExceptionNames) {
        scope->addName(Token(IDENTIFIER, e.name), e.name, new Exception(Token(), e.name));
    }

    {
        // The fields here must match the corresponding references to
        // ExceptionInfo in exec.cpp.
        std::vector<TypeRecord::Field> fields;
        fields.push_back(TypeRecord::Field(Token("info"), TYPE_STRING, false));
        fields.push_back(TypeRecord::Field(Token("code"), TYPE_NUMBER, false));
        Type *exception_info = new TypeRecord(Token(), "ExceptionInfo", fields);
        scope->addName(Token(IDENTIFIER, "ExceptionInfo"), "ExceptionInfo", exception_info, true);
    }
    {
        // The fields here must match the corresponding references to
        // ExceptionType in exec.cpp.
        std::vector<TypeRecord::Field> fields;
        fields.push_back(TypeRecord::Field(Token("name"), TYPE_STRING, false));
        fields.push_back(TypeRecord::Field(Token("info"), TYPE_STRING, false));
        fields.push_back(TypeRecord::Field(Token("code"), TYPE_NUMBER, false));
        fields.push_back(TypeRecord::Field(Token("offset"), TYPE_NUMBER, false));
        Type *exception_type = new TypeRecord(Token(), "ExceptionType", fields);
        scope->addName(Token(IDENTIFIER, "ExceptionType"), "ExceptionType", exception_type, true);
        GlobalVariable *current_exception = new GlobalVariable(Token(), "CURRENT_EXCEPTION", exception_type, true);
        scope->addName(Token(IDENTIFIER, "CURRENT_EXCEPTION"), "CURRENT_EXCEPTION", current_exception, true);
    }

    rtl_compile_init(scope);
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}
