#include "ast.h"

#include <iostream>
#include <iso646.h>
#include <sstream>
#include <string.h>

#include "intrinsic.h"
#include "rtl_compile.h"
#include "rtl_exec.h"

namespace ast {

TypeNothing *TYPE_NOTHING = new TypeNothing();
TypeDummy *TYPE_DUMMY = new TypeDummy();
TypeBoolean *TYPE_BOOLEAN = new TypeBoolean();
TypeNumber *TYPE_NUMBER = new TypeNumber(Token());
TypeString *TYPE_STRING = new TypeString();
TypeBytes *TYPE_BYTES = new TypeBytes();
TypeObject *TYPE_OBJECT = new TypeObject();
TypeArray *TYPE_ARRAY_NUMBER = new TypeArray(Token(), TYPE_NUMBER);
TypeArray *TYPE_ARRAY_STRING = new TypeArray(Token(), TYPE_STRING);
TypeArray *TYPE_ARRAY_OBJECT = new TypeArray(Token(), TYPE_OBJECT);
TypeDictionary *TYPE_DICTIONARY_NUMBER = new TypeDictionary(Token(), TYPE_NUMBER);
TypeDictionary *TYPE_DICTIONARY_STRING = new TypeDictionary(Token(), TYPE_STRING);
TypeDictionary *TYPE_DICTIONARY_OBJECT = new TypeDictionary(Token(), TYPE_OBJECT);
TypeModule *TYPE_MODULE = new TypeModule();
TypeException *TYPE_EXCEPTION = new TypeException();
TypeInterface *TYPE_INTERFACE = new TypeInterface();
Module *MODULE_MISSING = new Module(Token(), new Scope(nullptr, nullptr), "", false);

void AstNode::dump(std::ostream &out, int depth) const
{
    out << std::string(depth*2, ' ') << text() << "\n";
    dumpsubnodes(out, depth);
}

static const Expression *identity_conversion(Analyzer *, const Expression *e) { return e; }

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> Type::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    } else {
        fprintf(stderr, "might need make_conversion for %s from %s\n", text().c_str(), from->text().c_str());
        return nullptr;
    }
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeDummy::make_converter(const Type *from) const
{
    if (from == TYPE_NOTHING) {
        return nullptr;
    }
    return [this](Analyzer *, const Expression *e) {
        return new TypeConversionExpression(this, e);
    };
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
    return TypeString::serialize(utf8string(number_to_string(x)));
}

const Expression *TypeNumber::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    return new ConstantNumberExpression(number_from_string(TypeString::deserialize_string(value, i).str()));
}

const Expression *TypeString::make_default_value() const
{
    return new ConstantStringExpression(utf8string(""));
}

std::string TypeString::serialize(const utf8string &value)
{
    uint32_t len = static_cast<uint32_t>(value.size());
    std::string r;
    // TODO: vint
    r.push_back(static_cast<unsigned char>(len >> 24) & 0xff);
    r.push_back(static_cast<unsigned char>(len >> 16) & 0xff);
    r.push_back(static_cast<unsigned char>(len >> 8) & 0xff);
    r.push_back(static_cast<unsigned char>(len & 0xff));
    return r + value.str();
}

std::string TypeString::serialize(const Expression *value) const
{
    return serialize(value->eval_string());
}

utf8string TypeString::deserialize_string(const Bytecode::Bytes &value, int &i)
{
    // TODO: vint
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    std::string s(&value.at(i+4), &value.at(i+4)+len);
    i += 4 + len;
    return utf8string(s);
}

const Expression *TypeString::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    return new ConstantStringExpression(deserialize_string(value, i));
}

const Expression *TypeBytes::make_default_value() const
{
    return new ConstantBytesExpression("", "");
}

std::string TypeBytes::serialize(const Expression *value) const
{
    return TypeString::serialize(value->eval_string());
}

const Expression *TypeBytes::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    return new ConstantBytesExpression("Imported value", TypeString::deserialize_string(value, i).str());
}

const Expression *TypeObject::make_default_value() const
{
    return new ConstantNilObject();
}

std::string TypeObject::serialize(const Expression *) const
{
    return "";
}

const Expression *TypeObject::deserialize_value(const Bytecode::Bytes &, int &) const
{
    return nullptr; // TODO
}

TypeArray::TypeArray(const Token &declaration, const Type *elementtype)
  : Type(declaration, ""),
    elementtype(elementtype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, elementtype, nullptr));
        methods["append"] = new PredefinedFunction("array__append", new TypeFunction(TYPE_NOTHING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["extend"] = new PredefinedFunction("array__extend", new TypeFunction(TYPE_NOTHING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, elementtype, nullptr));
        methods["find"] = new PredefinedFunction("array__find", new TypeFunction(TYPE_NUMBER, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_NUMBER, nullptr));
        methods["remove"] = new PredefinedFunction("array__remove", new TypeFunction(TYPE_NOTHING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_NUMBER, nullptr));
        methods["resize"] = new PredefinedFunction("array__resize", new TypeFunction(TYPE_NOTHING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["reversed"] = new PredefinedFunction("array__reversed", new TypeFunction(this, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["size"] = new PredefinedFunction("array__size", new TypeFunction(TYPE_NUMBER, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        // TODO: This is just a hack to make this work for now.
        // Need to do this properly in a general purpose way.
        if (elementtype == TYPE_NUMBER) {
            methods["toBytes"] = new PredefinedFunction("array__toBytes__number", new TypeFunction(TYPE_BYTES, params, false));
            methods["toString"] = new PredefinedFunction("array__toString__number", new TypeFunction(TYPE_STRING, params, false));
        } else if (elementtype == TYPE_STRING) {
            methods["toString"] = new PredefinedFunction("array__toString__string", new TypeFunction(TYPE_STRING, params, false));
        } else if (elementtype == TYPE_OBJECT) {
            methods["toString"] = new PredefinedFunction("array__toString__object", new TypeFunction(TYPE_STRING, params, false));
        }
    }
}

const Expression *TypeArray::make_default_value() const
{
    return new ArrayLiteralExpression(nullptr, {}, {});
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeFunction::make_converter(const Type *from) const
{
    // TODO: There needs to be a mechanism for reporting more detail about why the
    // type does not match. There are quite a few reasons for this to return false,
    // and the user would probably appreciate more detail.
    const TypeFunction *f = dynamic_cast<const TypeFunction *>(from);
    if (f == nullptr) {
        const TypeFunctionPointer *p = dynamic_cast<const TypeFunctionPointer *>(from);
        if (p == nullptr) {
            return nullptr;
        }
        f = p->functype;
        if (f == nullptr) {
            return identity_conversion;
        }
    }
    if (returntype != TYPE_NOTHING && f->returntype != TYPE_NOTHING && returntype->make_converter(f->returntype) == nullptr) {
        return nullptr;
    }
    if (params.size() != f->params.size()) {
        return nullptr;
    }
    for (size_t i = 0; i < params.size(); i++) {
        if (params[i]->declaration.text != f->params[i]->declaration.text) {
            return nullptr;
        }
        if (params[i]->mode != f->params[i]->mode) {
            return nullptr;
        }
        if (f->params[i]->type->make_converter(params[i]->type) == nullptr) {
            return nullptr;
        }
    }
    return identity_conversion;
}

int TypeFunction::get_stack_delta() const
{
    int input_params = 0;
    int output_params = (returntype != TYPE_NOTHING ? 1 : 0);
    for (auto p: params) {
        switch (p->mode) {
            case ParameterType::Mode::IN:
            case ParameterType::Mode::INOUT:
                input_params++;
                break;
            case ParameterType::Mode::OUT:
                output_params++;
                break;
        }
    }
    return output_params - input_params;
}

bool TypeArray::is_structure_compatible(const Type *rhs) const
{
    const TypeArray *a = dynamic_cast<const TypeArray *>(rhs);
    if (a == nullptr) {
        return false;
    }
    // If elementtype is nullptr, then this array is an empty array
    // and it is compatible with any other kind of array.
    return elementtype == nullptr || elementtype->is_structure_compatible(a->elementtype);
}

std::string TypeArray::serialize(const Expression *value) const
{
    std::string r;
    const ArrayLiteralExpression *a = dynamic_cast<const ArrayLiteralExpression *>(value);
    // TODO: vint
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
    // TODO: vint
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    i += 4;
    while (len > 0) {
        elements.push_back(elementtype->deserialize_value(value, i));
        len--;
    }
    return new ArrayLiteralExpression(elementtype, elements, {});
}

TypeDictionary::TypeDictionary(const Token &declaration, const Type *elementtype)
  : Type(declaration, ""),
    elementtype(elementtype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["size"] = new PredefinedFunction("dictionary__size", new TypeFunction(TYPE_NUMBER, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["keys"] = new PredefinedFunction("dictionary__keys", new TypeFunction(TYPE_ARRAY_STRING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, this, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_STRING, nullptr));
        methods["remove"] = new PredefinedFunction("dictionary__remove", new TypeFunction(TYPE_NOTHING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        // TODO: This is just a hack to make this work for now.
        // Need to do this properly in a general purpose way.
        if (elementtype == TYPE_NUMBER) {
            methods["toString"] = new PredefinedFunction("dictionary__toString__number", new TypeFunction(TYPE_STRING, params, false));
        } else if (elementtype == TYPE_STRING) {
            methods["toString"] = new PredefinedFunction("dictionary__toString__string", new TypeFunction(TYPE_STRING, params, false));
        } else if (elementtype == TYPE_OBJECT) {
            methods["toString"] = new PredefinedFunction("dictionary__toString__object", new TypeFunction(TYPE_STRING, params, false));
        }
    }
}

const Expression *TypeDictionary::make_default_value() const
{
    return new DictionaryLiteralExpression(nullptr, {}, {});
}

bool TypeDictionary::is_structure_compatible(const Type *rhs) const
{
    const TypeDictionary *d = dynamic_cast<const TypeDictionary *>(rhs);
    if (d == nullptr) {
        return false;
    }
    // If elementtype is nullptr, then this dictionary is an empty dictionary
    // and it is compatible with any other kind of dictionary.
    return elementtype == nullptr || elementtype->is_structure_compatible(d->elementtype);
}

std::string TypeDictionary::serialize(const Expression *value) const
{
    std::string r;
    const DictionaryLiteralExpression *d = dynamic_cast<const DictionaryLiteralExpression *>(value);
    // TODO: vint
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
    std::vector<std::pair<utf8string, const Expression *>> dict;
    // TODO: vint
    uint32_t len = (value.at(i) << 24) | (value.at(i+1) << 16) | (value.at(i+2) << 8) | value.at(i+3);
    i += 4;
    while (len > 0) {
        utf8string key = TypeString::deserialize_string(value, i);
        dict.push_back(std::make_pair(key, elementtype->deserialize_value(value, i)));
        len--;
    }
    return new DictionaryLiteralExpression(elementtype, dict, {});
}

const Expression *TypeRecord::make_default_value() const
{
    std::vector<const Expression *> values;
    std::vector<Token> tokens;
    for (auto &f: fields) {
        values.push_back(f.type->make_default_value());
        tokens.push_back(Token());
    }
    return new ArrayLiteralExpression(nullptr, values, tokens);
}

std::string TypeRecord::serialize(const Expression *value) const
{
    std::string r;
    const RecordLiteralExpression *a = dynamic_cast<const RecordLiteralExpression *>(value);
    // TODO: vint
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
    // TODO: vint
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

std::string TypeClass::text() const
{
    std::string r = "TypeClass(";
    if (not interfaces.empty()) {
        bool first = true;
        for (auto i: interfaces) {
            if (not first) {
                r.append(",");
            }
            first = false;
            r.append(i->name);
        }
        r.append(";");
    }
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
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["toString"] = new PredefinedFunction("pointer__toString", new TypeFunction(TYPE_STRING, params, false));
    }
}

const Expression *TypePointer::make_default_value() const
{
    return new ConstantNilExpression();
}

bool TypePointer::is_structure_compatible(const Type *rhs) const
{
    const TypePointer *p = dynamic_cast<const TypePointer *>(rhs);
    if (p == nullptr) {
        return false;
    }
    if (reftype == nullptr) {
        return true;
    }
    return reftype->is_structure_compatible(p->reftype);
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypePointer::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (dynamic_cast<const TypePointerNil *>(from) != nullptr) {
        return identity_conversion;
    }
    const TypePointer *p = dynamic_cast<const TypePointer *>(from);
    if (p == nullptr) {
        return nullptr;
    }
    if (reftype == nullptr || p->reftype == nullptr) {
        return nullptr;
    }
    // Shortcut check avoids infinite recursion on records with pointer to itself.
    if (reftype == p->reftype || reftype->make_converter(p->reftype) != nullptr) {
        return identity_conversion;
    }
    return nullptr;
}

std::string TypePointer::serialize(const Expression *) const
{
    return std::string();
}

const Expression *TypePointer::deserialize_value(const Bytecode::Bytes &, int &) const
{
    return new ConstantNilExpression();
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeValidPointer::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (dynamic_cast<const TypeValidPointer *>(from) == nullptr) {
        return nullptr;
    }
    return TypePointer::make_converter(from);
}

TypeInterfacePointer::TypeInterfacePointer(const Token &declaration, const Interface *interface)
  : Type(declaration, ""),
    interface(interface)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["toString"] = new PredefinedFunction("interfacepointer__toString", new TypeFunction(TYPE_STRING, params, false));
    }
}

const Expression *TypeInterfacePointer::make_default_value() const
{
    return new InterfacePointerConstructor(nullptr, new ConstantNilExpression(), 0);
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeInterfacePointer::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (dynamic_cast<const TypePointerNil *>(from) != nullptr) {
        return [this](Analyzer *, const Expression *) {
            return new InterfacePointerConstructor(this, new ConstantNilExpression(), 0);
        };
    }
    const TypeInterfacePointer *p = dynamic_cast<const TypeInterfacePointer *>(from);
    if (p != nullptr && interface == p->interface) {
        return identity_conversion;
    }
    const TypePointer *pc = dynamic_cast<const TypePointer *>(from);
    if (pc != nullptr) {
        size_t i = 0;
        for (auto it: pc->reftype->interfaces) {
            if (it == interface) {
                return [this, i](Analyzer *, const Expression *e) {
                    return new InterfacePointerConstructor(this, e, i);
                };
            }
            i++;
        }
    }
    return nullptr;
}

std::string TypeInterfacePointer::serialize(const Expression *) const
{
    return std::string();
}

const Expression *TypeInterfacePointer::deserialize_value(const Bytecode::Bytes &, int &) const
{
    return new ConstantNilExpression();
}

std::string TypeInterfacePointer::text() const
{
    return "TypeInterfacePointer(" + interface->text() + ")";
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeValidInterfacePointer::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    if (dynamic_cast<const TypeValidPointer *>(from) == nullptr && dynamic_cast<const TypeValidInterfacePointer *>(from) == nullptr) {
        return nullptr;
    }
    return TypeInterfacePointer::make_converter(from);
}

std::string TypeValidInterfacePointer::text() const
{
    return "TypeValidInterfacePointer(" + interface->text() + ")";
}

TypeFunctionPointer::TypeFunctionPointer(const Token &declaration, const TypeFunction *functype)
  : Type(declaration, ""),
    functype(functype)
{
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, this, nullptr));
        methods["toString"] = new PredefinedFunction("functionpointer__toString", new TypeFunction(TYPE_STRING, params, false));
    }
}

const Expression *TypeFunctionPointer::make_default_value() const
{
    return new ConstantNowhereExpression();
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeFunctionPointer::make_converter(const Type *from) const
{
    if (functype == nullptr) {
        return nullptr;
    }
    if (functype->make_converter(from) == nullptr) {
        return nullptr;
    }
    return [this](Analyzer *, const Expression *e) {
        return new TypeConversionExpression(this, e);
    };
}

std::string TypeFunctionPointer::serialize(const Expression *) const
{
    return std::string();
}

const Expression *TypeFunctionPointer::deserialize_value(const Bytecode::Bytes &, int &) const
{
    return new ConstantNumberExpression(number_from_sint32(0));
}

const Expression *TypeEnum::make_default_value() const
{
    return new ConstantEnumExpression(this, 0);
}

std::function<const Expression *(Analyzer *analyzer, const Expression *e)> TypeEnum::make_converter(const Type *from) const
{
    if (from == this) {
        return identity_conversion;
    }
    return nullptr;
}

std::string TypeEnum::serialize(const Expression *value) const
{
    Number x = value->eval_number();
    return TypeString::serialize(utf8string(number_to_string(x)));
}

const Expression *TypeEnum::deserialize_value(const Bytecode::Bytes &value, int &i) const
{
    return new ConstantEnumExpression(this, std::stoi(TypeString::deserialize_string(value, i).str()));
}

std::string ModuleVariable::text() const
{
    return "ModuleVariable(" + module->name + "." + name + ")";
}

std::string ModuleFunction::text() const
{
    return "ModuleFunction(" + module->name + "." + name + ", " + type->text() + ")";
}

bool Expression::eval_boolean(const Token &token) const
{
    try {
        return eval_boolean();
    } catch (RtlException &e) {
        error(3195, token, "Boolean evaluation exception: " + e.name + ": " + e.info.str());
    }
}

Number Expression::eval_number(const Token &token) const
{
    try {
        return eval_number();
    } catch (RtlException &e) {
        error(3196, token, "Numeric evaluation exception: " + e.name + ": " + e.info.str());
    }
}

utf8string Expression::eval_string(const Token &token) const
{
    try {
        return eval_string();
    } catch (RtlException &e) {
        error(3197, token, "String evaluation exception: " + e.name + ": " + e.info.str());
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
    s << "ConstantStringExpression(" << value.str() << ")";
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

bool DictionaryLiteralExpression::all_constant(const std::vector<std::pair<utf8string, const Expression *>> &elements)
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

std::map<utf8string, const Expression *> DictionaryLiteralExpression::make_dictionary(const std::vector<std::pair<utf8string, const Expression *>> &elements)
{
    std::map<utf8string, const Expression *> dict;
    for (auto e: elements) {
        dict[e.first] = e.second;
    }
    return dict;
}

bool TypeTestExpression::eval_boolean() const
{
    if (type == TYPE_OBJECT) {
        internal_error("unexpected object type");
    }
    return expr_after_conversion->type->make_converter(expr_before_conversion->type) != nullptr;
}

bool BooleanComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case Comparison::EQ: return left->eval_boolean() == right->eval_boolean();
        case Comparison::NE: return left->eval_boolean() != right->eval_boolean();
        case Comparison::LT:
        case Comparison::GT:
        case Comparison::LE:
        case Comparison::GE:
            internal_error("BooleanComparisonExpression");
    }
    internal_error("BooleanComparisonExpression");
}

bool NumericComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case Comparison::EQ: return number_is_equal        (left->eval_number(), right->eval_number());
        case Comparison::NE: return number_is_not_equal    (left->eval_number(), right->eval_number());
        case Comparison::LT: return number_is_less         (left->eval_number(), right->eval_number());
        case Comparison::GT: return number_is_greater      (left->eval_number(), right->eval_number());
        case Comparison::LE: return number_is_less_equal   (left->eval_number(), right->eval_number());
        case Comparison::GE: return number_is_greater_equal(left->eval_number(), right->eval_number());
    }
    internal_error("NumericComparisonExpression");
}

bool EnumComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case Comparison::EQ: return number_is_equal        (left->eval_number(), right->eval_number());
        case Comparison::NE: return number_is_not_equal    (left->eval_number(), right->eval_number());
        case Comparison::LT:
        case Comparison::GT:
        case Comparison::LE:
        case Comparison::GE:
            break;
    }
    internal_error("EnumComparisonExpression");
}

bool StringComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case Comparison::EQ: return left->eval_string() == right->eval_string();
        case Comparison::NE: return left->eval_string() != right->eval_string();
        case Comparison::LT: return left->eval_string() <  right->eval_string();
        case Comparison::GT: return left->eval_string() >  right->eval_string();
        case Comparison::LE: return left->eval_string() <= right->eval_string();
        case Comparison::GE: return left->eval_string() >= right->eval_string();
    }
    internal_error("StringComparisonExpression");
}

bool BytesComparisonExpression::eval_boolean() const
{
    switch (comp) {
        case Comparison::EQ: return left->eval_string() == right->eval_string();
        case Comparison::NE: return left->eval_string() != right->eval_string();
        case Comparison::LT: return left->eval_string() <  right->eval_string();
        case Comparison::GT: return left->eval_string() >  right->eval_string();
        case Comparison::LE: return left->eval_string() <= right->eval_string();
        case Comparison::GE: return left->eval_string() >= right->eval_string();
    }
    internal_error("BytesComparisonExpression");
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

bool CaseStatement::is_scope_exit_statement() const
{
    bool seen_others = false;
    for (auto clause: clauses) {
        seen_others |= clause.first.empty();
        if (clause.second.empty() || not clause.second.back()->is_scope_exit_statement()) {
            return false;
        }
    }
    if (not seen_others) {
        return false;
    }
    return true;
}

bool TryStatement::always_returns() const
{
    if (statements.empty() || not statements.back()->always_returns()) {
        return false;
    }
    for (auto c: catches) {
        const ExceptionHandlerStatement *ehs = dynamic_cast<const ExceptionHandlerStatement *>(c.handler);
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

bool FunctionCall::eval_boolean() const
{
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    const PredefinedFunction *f = dynamic_cast<const PredefinedFunction *>(ve->var);
    if (f->name == "math$odd") return rtl::ne_global::odd(args[0]->eval_number());
    internal_error("unexpected intrinsic");
}

Number FunctionCall::eval_number() const
{
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    const PredefinedFunction *f = dynamic_cast<const PredefinedFunction *>(ve->var);
    if (f->name == "string$toCodePoint") return rtl::ne_global::ord(args[0]->eval_string());
    if (f->name == "math$trunc") return rtl::ne_global::int_(args[0]->eval_number());
    if (f->name == "math$max") return rtl::ne_global::max(args[0]->eval_number(), args[1]->eval_number());
    if (f->name == "math$min") return rtl::ne_global::min(args[0]->eval_number(), args[1]->eval_number());
    if (f->name == "num") return rtl::ne_global::num(args[0]->eval_string());
    if (f->name == "math$round") return rtl::ne_global::round(args[0]->eval_number(), args[1]->eval_number());
    internal_error("unexpected intrinsic");
}

utf8string FunctionCall::eval_string() const
{
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    const PredefinedFunction *f = dynamic_cast<const PredefinedFunction *>(ve->var);
    if (f->name == "string$fromCodePoint") return rtl::ne_global::chr(args[0]->eval_number());
    if (f->name == "string__concat") return rtl::ne_global::string__concat(args[0]->eval_string(), args[1]->eval_string());
    if (f->name == "str") return rtl::ne_global::str(args[0]->eval_number());
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
    if (f->name == "string$fromCodePoint"
     || f->name == "string__concat"
     || f->name == "math$trunc"
     || f->name == "math$max"
     || f->name == "math$min"
     || f->name == "num"
     || f->name == "math$odd"
     || f->name == "string$toCodePoint"
     || f->name == "math$round"
     || f->name == "str") {
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

void Frame::setReferent(int slot, const std::string &, Name *ref)
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

void ExternalGlobalFrame::setReferent(int slot, const std::string &name, Name *ref)
{
    auto g = external_globals.find(name);
    if (g == external_globals.end()) {
        internal_error("external global does not exist: " + name);
    }
    g->second.ref = ref;
    Frame::setReferent(slot, name, ref);
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
        frame->setReferent(a->second, name, ref);
        if (init_referenced) {
            frame->setReferenced(a->second);
        }
    } else {
        names[name] = frame->addSlot(token, name, ref, init_referenced);
    }
}

void Scope::replaceName(const Token &token, const std::string &name, Name *ref)
{
    auto a = names.find(name);
    if (a != names.end()) {
        names.erase(name);
    }
    addName(token, name, ref, true, true);
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
        error(3264, forwards.begin()->second[0]->declaration, "unresolved forward declaration");
    }
}

Variable *Scope::makeTemporary(const Type *type)
{
    std::string name = "temp__" + std::to_string(names.size()) + "__" + std::to_string(reinterpret_cast<intptr_t>(this));
    Variable *r = frame->createVariable(Token(), name, type, false);
    addName(Token(IDENTIFIER, ""), name, r, true);
    return r;
}

ExternalGlobalScope::ExternalGlobalScope(Scope *parent, Frame *frame, std::map<std::string, ExternalGlobalInfo> &external_globals)
  : Scope(parent, frame)
{
    for (auto &g: external_globals) {
        g.second.ref->reset();
        Scope::addName(g.second.declaration, g.first, g.second.ref, g.second.init_referenced);
    }
}

Function::Function(const Token &declaration, const std::string &name, const Type *returntype, Frame *outer, Scope *parent, const std::vector<FunctionParameter *> &params, bool variadic, size_t nesting_depth)
  : BaseFunction(declaration, name, makeFunctionType(returntype, params, variadic)),
    frame(new LocalFrame(outer)),
    scope(new Scope(parent, frame)),
    params(params),
    nesting_depth(nesting_depth),
    statements()
{
    for (auto p: params) {
        scope->addName(p->declaration, p->name, p, true);
    }
}

const TypeFunction *Function::makeFunctionType(const Type *returntype, const std::vector<FunctionParameter *> &params, bool variadic)
{
    std::vector<const ParameterType *> paramtypes;
    for (auto p: params) {
        paramtypes.push_back(new ParameterType(p->declaration, p->mode, p->type, p->default_value));
    }
    return new TypeFunction(returntype, paramtypes, variadic);
}

bool PredefinedFunction::is_pure(std::set<const ast::Function *> &) const
{
    static std::set<std::string> impure_modules {
        "console",
        "debugger",
        "file",
        "io",
        "mmap",
        "net",
        "os",
        "posix",
        "process",
        "random",
        "runtime",
        "sqlite",
        "sys",
        "textio",
        "time",
    };
    auto sep = name.find('$');
    auto mod = name.substr(0, sep);
    return impure_modules.find(mod) == impure_modules.end();
}

Program::Program(const std::string &source_path, const std::string &source_hash, const std::string &module_name)
  : source_path(source_path),
    source_hash(source_hash),
    module_name(module_name),
    frame(new GlobalFrame(nullptr)),
    scope(new Scope(nullptr, frame)),
    statements(),
    exports()
{
    scope->addName(Token(IDENTIFIER, "Boolean"), "Boolean", TYPE_BOOLEAN);
    scope->addName(Token(IDENTIFIER, "Number"), "Number", TYPE_NUMBER);
    scope->addName(Token(IDENTIFIER, "String"), "String", TYPE_STRING);
    scope->addName(Token(IDENTIFIER, "Bytes"), "Bytes", TYPE_BYTES);
    scope->addName(Token(IDENTIFIER, "Object"), "Object", TYPE_OBJECT);

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_BOOLEAN, nullptr));
        TYPE_BOOLEAN->methods["toString"] = new PredefinedFunction("boolean__toString", new TypeFunction(TYPE_STRING, params, false));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_NUMBER, nullptr));
        TYPE_NUMBER->methods["toString"] = new PredefinedFunction("number__toString", new TypeFunction(TYPE_STRING, params, false));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["length"] = new PredefinedFunction("string__length", new TypeFunction(TYPE_NUMBER, params, false));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, TYPE_STRING, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["append"] = new PredefinedFunction("string__append", new TypeFunction(TYPE_NOTHING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["toBytes"] = new PredefinedFunction("string__toBytes", new TypeFunction(TYPE_BYTES, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_STRING, nullptr));
        TYPE_STRING->methods["toString"] = new PredefinedFunction("string__toString", new TypeFunction(TYPE_STRING, params, false));
    }

    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::INOUT, TYPE_BYTES, nullptr));
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_ARRAY_NUMBER, nullptr));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["size"] = new PredefinedFunction("bytes__size", new TypeFunction(TYPE_NUMBER, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["decodeToString"] = new PredefinedFunction("bytes__decodeToString", new TypeFunction(TYPE_STRING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["toArray"] = new PredefinedFunction("bytes__toArray", new TypeFunction(TYPE_ARRAY_NUMBER, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_BYTES, nullptr));
        TYPE_BYTES->methods["toString"] = new PredefinedFunction("bytes__toString", new TypeFunction(TYPE_STRING, params, false));
    }
    {
        std::vector<const ParameterType *> params;
        params.push_back(new ParameterType(Token(), ParameterType::Mode::IN, TYPE_OBJECT, nullptr));
        TYPE_OBJECT->methods["toString"] = new PredefinedFunction("object__toString", new TypeFunction(TYPE_STRING, params, false));
    }

    for (auto x: rtl::ExceptionNames) {
        Exception *e = new Exception(Token(), x.name);
        for (int i = 0; x.sub[i] != nullptr; i++) {
            e->subexceptions[x.sub[i]] = new Exception(Token(), std::string(x.name) + "." + x.sub[i]);
        }
        scope->addName(Token(IDENTIFIER, x.name), x.name, e);
    }

    {
        // The fields here must match the corresponding references to
        // ExceptionType in exec.cpp, and the declaration in global.neon.
        std::vector<TypeRecord::Field> fields;
        fields.push_back(TypeRecord::Field(Token("name"), TYPE_STRING, false));
        fields.push_back(TypeRecord::Field(Token("info"), TYPE_OBJECT, false));
        fields.push_back(TypeRecord::Field(Token("offset"), TYPE_NUMBER, false));
        Type *exception_type = new TypeRecord(Token(), "global", "ExceptionType", fields);
        exception_type->methods["toString"] = new PredefinedFunction("exceptiontype__toString", new TypeFunction(TYPE_STRING, { new ParameterType(Token(), ParameterType::Mode::IN, exception_type, nullptr) }, false));
        scope->addName(Token(IDENTIFIER, "ExceptionType"), "ExceptionType", exception_type, true);
    }

    rtl_compile_init(scope);
}

void Program::dumpsubnodes(std::ostream &out, int depth) const
{
    for (std::vector<const Statement *>::const_iterator i = statements.begin(); i != statements.end(); ++i) {
        (*i)->dump(out, depth+1);
    }
}

} // namespace ast
