#include "compiler.h"

#include <algorithm>
#include <assert.h>
#include <iso646.h>
#include <sstream>
#include <stack>

#include "ast.h"
#include "bytecode.h"
#include "debuginfo.h"
#include "opcode.h"

class Emitter {
    class Label {
        friend class Emitter;
        Label(): fixups(), target(UINT_MAX) {}
        std::vector<unsigned int> fixups;
        unsigned int target;
    public:
        unsigned int get_target() {
            if (target == UINT_MAX) {
                internal_error("Label::get_target");
            }
            return target;
        }
    };
    class LoopLabels {
    public:
        LoopLabels(): exit(nullptr), next(nullptr) {}
        LoopLabels(Label *exit, Label *next): exit(exit), next(next) {}
        Label *exit;
        Label *next;
    };
public:
    Emitter(const std::string &source_hash, DebugInfo *debug): source_hash(source_hash), object(), globals(), functions(), function_exit(), loop_labels(), exported_types(), debug_info(debug) {}
    void emit(unsigned char b);
    void emit_uint32(uint32_t value);
    void emit(unsigned char b, uint32_t value);
    void emit(unsigned char b, uint32_t value, uint32_t value2);
    void emit(unsigned char b, const Number &value);
    void emit(const std::vector<unsigned char> &instr);
    std::vector<unsigned char> getObject();
    unsigned int global(const std::string &name);
    unsigned int str(const std::string &s);
    unsigned int current_ip();
    unsigned int next_function();
    Label &function_label(int index);
    Label create_label();
    void emit_jump(unsigned char b, Label &label);
    void jump_target(Label &label);
    void add_loop_labels(unsigned int loop_id, Label &exit, Label &next);
    void remove_loop_labels(unsigned int loop_id);
    Label &get_exit_label(unsigned int loop_id);
    Label &get_next_label(unsigned int loop_id);
    void debug_line(int line);
    void add_exception(const Bytecode::ExceptionInfo &ei);
    void push_function_exit(Label &label);
    void pop_function_exit();
    Label &get_function_exit();
    void declare_export_type(const Type *type);
    void add_export_type(const std::string &name, const std::string &descriptor);
    void add_export_constant(const std::string &name, const std::string &type, const std::string &value);
    void add_export_variable(const std::string &name, const std::string &type, int index);
    void add_export_function(const std::string &name, const std::string &type, int entry);
    void add_export_exception(const std::string &name);
    void add_import(const std::string &name);
    std::string get_type_reference(const Type *type);
private:
    const std::string source_hash;
    Bytecode object;
    std::vector<std::string> globals;
    std::vector<Label> functions;
    std::stack<Label *> function_exit;
    std::map<size_t, LoopLabels> loop_labels;
    std::set<const Type *> exported_types;
    DebugInfo *debug_info;
private:
    Emitter(const Emitter &);
    Emitter &operator=(const Emitter &);
};

void Emitter::emit(unsigned char b)
{
    object.code.push_back(b);
}

void Emitter::emit_uint32(uint32_t value)
{
    emit(static_cast<unsigned char>((value >> 24) & 0xff));
    emit(static_cast<unsigned char>((value >> 16) & 0xff));
    emit(static_cast<unsigned char>((value >> 8) & 0xff));
    emit(static_cast<unsigned char>(value & 0xff));
}

void Emitter::emit(unsigned char b, uint32_t value)
{
    emit(b);
    emit_uint32(value);
}

void Emitter::emit(unsigned char b, uint32_t value, uint32_t value2)
{
    emit(b);
    emit_uint32(value);
    emit_uint32(value2);
}

void Emitter::emit(unsigned char b, const Number &value)
{
    emit(b);
    // TODO: endian
    const unsigned char *v = reinterpret_cast<const unsigned char *>(&value);
    emit(std::vector<unsigned char>(v, v+sizeof(value)));
}

void Emitter::emit(const std::vector<unsigned char> &instr)
{
    std::copy(instr.begin(), instr.end(), std::back_inserter(object.code));
}

std::vector<unsigned char> Emitter::getObject()
{
    object.source_hash = source_hash;
    object.global_size = globals.size();
    return object.getBytes();
}

unsigned int Emitter::global(const std::string &name)
{
    auto i = std::find(globals.begin(), globals.end(), name);
    if (i == globals.end()) {
        globals.push_back(name);
        i = globals.end() - 1;
    }
    return static_cast<unsigned int>(i - globals.begin());
}

unsigned int Emitter::str(const std::string &s)
{
    auto i = std::find(object.strtable.begin(), object.strtable.end(), s);
    if (i == object.strtable.end()) {
        object.strtable.push_back(s);
        i = object.strtable.end() - 1;
    }
    return static_cast<unsigned int>(i - object.strtable.begin());
}

unsigned int Emitter::current_ip()
{
    return static_cast<unsigned int>(object.code.size());
}

unsigned int Emitter::next_function()
{
    auto i = functions.size();
    functions.push_back(create_label());
    return static_cast<unsigned int>(i);
}

Emitter::Label &Emitter::function_label(int index)
{
    return functions[index];
}

Emitter::Label Emitter::create_label()
{
    return Label();
}

void Emitter::emit_jump(unsigned char b, Label &label)
{
    emit(b);
    if (label.target != UINT_MAX) {
        emit_uint32(label.target);
    } else {
        label.fixups.push_back(static_cast<unsigned int>(object.code.size()));
        emit_uint32(UINT32_MAX);
    }
}

void Emitter::jump_target(Label &label)
{
    assert(label.target == UINT_MAX);
    label.target = static_cast<unsigned int>(object.code.size());
    for (auto offset: label.fixups) {
        object.code[offset] = static_cast<unsigned char>((label.target >> 24) & 0xff);
        object.code[offset+1] = static_cast<unsigned char>((label.target >> 16) & 0xff);
        object.code[offset+2] = static_cast<unsigned char>((label.target >> 8) & 0xff);
        object.code[offset+3] = static_cast<unsigned char>(label.target & 0xff);
    }
}

void Emitter::add_loop_labels(unsigned int loop_id, Label &exit, Label &next)
{
    loop_labels[loop_id] = LoopLabels(&exit, &next);
}

void Emitter::remove_loop_labels(unsigned int loop_id)
{
    loop_labels.erase(loop_id);
}

Emitter::Label &Emitter::get_exit_label(unsigned int loop_id)
{
    if (loop_labels.find(loop_id) == loop_labels.end()) {
        internal_error("loop_id not found");
    }
    return *loop_labels[loop_id].exit;
}

Emitter::Label &Emitter::get_next_label(unsigned int loop_id)
{
    if (loop_labels.find(loop_id) == loop_labels.end()) {
        internal_error("loop_id not found");
    }
    return *loop_labels[loop_id].next;
}

void Emitter::debug_line(int line)
{
    if (debug_info == nullptr) {
        return;
    }
    debug_info->line_numbers[object.code.size()] = line;
}

void Emitter::add_exception(const Bytecode::ExceptionInfo &ei)
{
    object.exceptions.push_back(ei);
}

void Emitter::push_function_exit(Label &exit)
{
    function_exit.push(&exit);
}

void Emitter::pop_function_exit()
{
    function_exit.pop();
}

Emitter::Label &Emitter::get_function_exit()
{
    if (function_exit.empty()) {
        internal_error("get_function_exit");
    }
    return *function_exit.top();
}

void Emitter::declare_export_type(const Type *type)
{
    exported_types.insert(type);
}

void Emitter::add_export_type(const std::string &name, const std::string &descriptor)
{
    Bytecode::Type type;
    type.name = str(name);
    type.descriptor = str(descriptor);
    object.types.push_back(type);
}

void Emitter::add_export_constant(const std::string &name, const std::string &type, const std::string &value)
{
    Bytecode::Constant constant;
    constant.name = str(name);
    constant.type = str(type);
    constant.value = Bytecode::Bytes(value.data(), value.data()+value.length());
    object.constants.push_back(constant);
}

void Emitter::add_export_variable(const std::string &name, const std::string &type, int index)
{
    Bytecode::Variable variable;
    variable.name = str(name);
    variable.type = str(type);
    variable.index = index;
    object.variables.push_back(variable);
}

void Emitter::add_export_function(const std::string &name, const std::string &type, int entry)
{
    Bytecode::Function function;
    function.name = str(name);
    function.descriptor = str(type);
    function.entry = entry;
    object.functions.push_back(function);
}

void Emitter::add_export_exception(const std::string &name)
{
    Bytecode::ExceptionExport exception;
    exception.name = str(name);
    object.exception_exports.push_back(exception);
}

void Emitter::add_import(const std::string &name)
{
    unsigned int index = str(name);
    for (auto i: object.imports) {
        if (i.first == index) {
            return;
        }
    }
    object.imports.push_back(std::make_pair(index, std::string(32, '0')));
}

std::string Emitter::get_type_reference(const Type *type)
{
    if (type->name.find('.') != std::string::npos || exported_types.find(type) != exported_types.end()) {
        return "~" + type->name + ";";
    }
    return type->get_type_descriptor(*this);
}

void Type::predeclare(Emitter &emitter) const
{
    for (auto m: methods) {
        m.second->predeclare(emitter);
    }
}

void Type::postdeclare(Emitter &emitter) const
{
    for (auto m: methods) {
        m.second->postdeclare(emitter);
    }
}

void Type::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_type(name, get_type_descriptor(emitter));
    for (auto m: methods) {
        // TODO: This is kind of a hack. It seems to work because
        // predefined functions are available everywhere and don't
        // need to be actually exported.
        if (dynamic_cast<const PredefinedFunction *>(m.second) != nullptr) {
            continue;
        }
        const Function *f = dynamic_cast<const Function *>(m.second);
        if (f == nullptr) {
            internal_error("method should be function: " + this->name + "." + m.second->name + " " + m.second->text());
        }
        emitter.add_export_function(name + "." + m.first, f->type->get_type_descriptor(emitter), emitter.function_label(f->entry_label).get_target());
    }
}

void TypeBoolean::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADB);
}

void TypeBoolean::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREB);
}

void TypeBoolean::generate_call(Emitter &) const
{
    internal_error("TypeBoolean");
}

void TypeNumber::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADN);
}

void TypeNumber::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREN);
}

void TypeNumber::generate_call(Emitter &) const
{
    internal_error("TypeNumber");
}

void TypeString::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADS);
}

void TypeString::generate_store(Emitter &emitter) const
{
    emitter.emit(STORES);
}

void TypeString::generate_call(Emitter &) const
{
    internal_error("TypeString");
}

void TypeFunction::predeclare(Emitter &emitter) const
{
    Type::predeclare(emitter);
    returntype->predeclare(emitter);
}

void TypeFunction::generate_load(Emitter &) const
{
    internal_error("TypeFunction");
}

void TypeFunction::generate_store(Emitter &) const
{
    internal_error("TypeFunction");
}

void TypeFunction::generate_call(Emitter &) const
{
    internal_error("TypeFunction");
}

std::string TypeFunction::get_type_descriptor(Emitter &emitter) const
{
    std::string r = "F[";
    for (auto p: params) {
        if (r.length() > 2) {
            r += ",";
        }
        char m;
        switch (p->mode) {
            case ParameterType::IN:    m = '>'; break;
            case ParameterType::INOUT: m = '*'; break;
            case ParameterType::OUT:   m = '<'; break;
            default:
                internal_error("invalid parameter mode");
        }
        // TODO: default value
        r += m + p->declaration.text + ":" + emitter.get_type_reference(p->type);
    }
    r += "]:" + emitter.get_type_reference(returntype);
    return r;
}

void TypeArray::predeclare(Emitter &emitter) const
{
    Type::predeclare(emitter);
    if (elementtype != nullptr) {
        elementtype->predeclare(emitter);
    }
}

void TypeArray::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void TypeArray::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void TypeArray::generate_call(Emitter &) const
{
    internal_error("TypeArray");
}

std::string TypeArray::get_type_descriptor(Emitter &emitter) const
{
    return "A<" + emitter.get_type_reference(elementtype) + ">";
}

void TypeArray::get_type_references(std::set<const Type *> &references) const
{
    if (references.insert(elementtype).second) {
        elementtype->get_type_references(references);
    }
}

void TypeDictionary::predeclare(Emitter &emitter) const
{
    Type::predeclare(emitter);
    if (elementtype != nullptr) {
        elementtype->predeclare(emitter);
    }
}

void TypeDictionary::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADD);
}

void TypeDictionary::generate_store(Emitter &emitter) const
{
    emitter.emit(STORED);
}

void TypeDictionary::generate_call(Emitter &) const
{
    internal_error("TypeDictionary");
}

std::string TypeDictionary::get_type_descriptor(Emitter &emitter) const
{
    return "D<" + emitter.get_type_reference(elementtype) + ">";
}

void TypeDictionary::get_type_references(std::set<const Type *> &references) const
{
    if (references.insert(elementtype).second) {
        elementtype->get_type_references(references);
    }
}

void TypeRecord::predeclare(Emitter &emitter) const
{
    // Avoid unbounded recursion.
    if (predeclared) {
        return;
    }
    predeclared = true;
    Type::predeclare(emitter);
    for (auto f: fields) {
        f.type->predeclare(emitter);
    }
}

void TypeRecord::postdeclare(Emitter &emitter) const
{
    // Avoid unbounded recursion.
    if (postdeclared) {
        return;
    }
    postdeclared = true;
    Type::postdeclare(emitter);
}

void TypeRecord::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void TypeRecord::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void TypeRecord::generate_call(Emitter &) const
{
    internal_error("TypeRecord");
}

std::string TypeRecord::get_type_descriptor(Emitter &emitter) const
{
    std::string r = "R[";
    for (auto f: fields) {
        if (r.length() > 2) {
            r += ",";
        }
        if (f.is_private) {
            r += "!";
        }
        r += f.name.text + ":" + emitter.get_type_reference(f.type);
    }
    r += "]";
    return r;
}

void TypeRecord::get_type_references(std::set<const Type *> &references) const
{
    for (auto f: fields) {
        if (references.insert(f.type).second) {
            f.type->get_type_references(references);
        }
    }
}

void TypePointer::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADP);
}

void TypePointer::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREP);
}

void TypePointer::generate_call(Emitter &) const
{
    internal_error("TypePointer");
}

std::string TypePointer::get_type_descriptor(Emitter &emitter) const
{
    return "P<" + (reftype != nullptr ? emitter.get_type_reference(reftype) : "") + ">";
}

void TypePointer::get_type_references(std::set<const Type *> &references) const
{
    if (reftype != nullptr && references.insert(reftype).second) {
        reftype->get_type_references(references);
    }
}

void TypeFunctionPointer::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADN);
}

void TypeFunctionPointer::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREN);
}

void TypeFunctionPointer::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLI);
}

std::string TypeFunctionPointer::get_type_descriptor(Emitter &emitter) const
{
    return functype->get_type_descriptor(emitter);
}

void TypeFunctionPointer::get_type_references(std::set<const Type *> &references) const
{
    if (references.insert(functype).second) {
        functype->get_type_references(references);
    }
}

std::string TypeEnum::get_type_descriptor(Emitter &) const
{
    std::string r = "E[";
    std::vector<std::string> namevector(names.size());
    for (auto n: names) {
        if (not namevector[n.second].empty()) {
            internal_error("duplicate enum value");
        }
        namevector[n.second] = n.first;
    }
    for (auto n: namevector) {
        if (r.length() > 2) {
            r += ",";
        }
        r += n;
    }
    r += "]";
    return r;
}

void Variable::generate_load(Emitter &emitter) const
{
    type->generate_load(emitter);
}

void Variable::generate_store(Emitter &emitter) const
{
    type->generate_store(emitter);
}

void Variable::generate_call(Emitter &emitter) const
{
    // These two statements are for function pointer support.
    // I'm not quite sure why this doesn't cause an internal compiler error
    // for other things like PredefinedFunction (since its generate_address
    // is not implemented), but nevertheless this seems to work.
    generate_address(emitter, 0);
    type->generate_load(emitter);

    type->generate_call(emitter);
}

void ModuleVariable::predeclare(Emitter &emitter) const
{
    emitter.add_import(module);
}

void ModuleVariable::generate_address(Emitter &emitter, int) const
{
    emitter.emit(PUSHPMG, emitter.str(module), index);
}

void GlobalVariable::predeclare(Emitter &emitter) const
{
    type->predeclare(emitter);
    index = emitter.global(name);
}

void GlobalVariable::generate_address(Emitter &emitter, int) const
{
    if (index < 0) {
        internal_error("invalid global index: " + name);
    }
    emitter.emit(PUSHPG, index);
}

void GlobalVariable::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_variable(name, emitter.get_type_reference(type), index);
}

void LocalVariable::predeclare(Emitter &emitter, int slot)
{
    type->predeclare(emitter);
    index = slot;
}

void LocalVariable::generate_address(Emitter &emitter, int enclosing) const
{
    if (index < 0) {
        internal_error("invalid local index: " + name);
    }
    if (enclosing > 0) {
        emitter.emit(PUSHPOL, enclosing, index);
    } else {
        emitter.emit(PUSHPL, index);
    }
}

void FunctionParameter::generate_address(Emitter &emitter, int) const
{
    if (index < 0) {
        internal_error("invalid local index: " + name);
    }
    switch (mode) {
        case ParameterType::IN:
        case ParameterType::OUT:
            emitter.emit(PUSHPL, index);
            break;
        case ParameterType::INOUT:
            emitter.emit(PUSHPL, index);
            emitter.emit(LOADP);
            break;
    }
}

void Function::predeclare(Emitter &emitter) const
{
    type->predeclare(emitter);
    frame->predeclare(emitter);
    // TODO: This hack ensures that we only allocate the
    // entry label once, even if predeclare() is called
    // more than once.
    if (entry_label == UINT_MAX) {
        entry_label = emitter.next_function();
    }
}

void Function::postdeclare(Emitter &emitter) const
{
    emitter.jump_target(emitter.function_label(entry_label));
    emitter.emit(ENTER, static_cast<uint32_t>(frame->getCount()));
    for (auto p = params.rbegin(); p != params.rend(); ++p) {
        switch ((*p)->mode) {
            case ParameterType::IN:
                (*p)->generate_address(emitter, 0);
                (*p)->generate_store(emitter);
                break;
            case ParameterType::INOUT:
                emitter.emit(PUSHPL, (*p)->index);
                emitter.emit(STOREP);
                break;
            case ParameterType::OUT:
                break;
        }
    }
    auto exit = emitter.create_label();
    emitter.push_function_exit(exit);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.pop_function_exit();
    emitter.jump_target(exit);
    for (auto p = params.rbegin(); p != params.rend(); ++p) {
        switch ((*p)->mode) {
            case ParameterType::IN:
            case ParameterType::INOUT:
                break;
            case ParameterType::OUT:
                (*p)->generate_address(emitter, 0);
                (*p)->generate_load(emitter);
                break;
        }
    }
    emitter.emit(LEAVE);
    emitter.emit(RET);
    frame->postdeclare(emitter);
}

void Function::generate_load(Emitter &emitter) const
{
    // Get the address of a function for function pointer support.
    emitter.emit_jump(PUSHI, emitter.function_label(entry_label));
}

void Function::generate_call(Emitter &emitter) const
{
    emitter.emit_jump(CALLF, emitter.function_label(entry_label));
}

void Function::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_function(name, type->get_type_descriptor(emitter), emitter.function_label(entry_label).get_target());
}

void PredefinedFunction::predeclare(Emitter &emitter) const
{
    name_index = emitter.str(name);
}

void PredefinedFunction::generate_call(Emitter &emitter) const
{
    if (name_index == -1) {
        internal_error("predefined function not generated: "+name);
    }
    emitter.emit(CALLP, name_index);
}

void ModuleFunction::predeclare(Emitter &emitter) const
{
    emitter.add_import(module);
}

void ModuleFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLMF, emitter.str(module), entry);
}

void ExternalFunction::predeclare(Emitter &emitter) const
{
    std::stringstream ss;
    ss << library_name << ":" << name << ":";
    auto r = param_types.find("return");
    if (r != param_types.end()) {
        ss << r->second;
    }
    ss << ":";
    bool first = true;
    for (auto param: params) {
        if (not first) {
            ss << ",";
        }
        first = false;
        if (param->mode == ParameterType::INOUT) {
            ss << '*';
        }
        ss << param_types.at(param->name);
    }
    external_index = emitter.str(ss.str());
}

void ExternalFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLE, external_index);
}

void Exception::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_exception(name);
}

void Constant::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_constant(name, emitter.get_type_reference(type), type->serialize(value));
}

void Expression::generate(Emitter &emitter) const
{
    if (type != nullptr) {
        type->predeclare(emitter);
    }
    generate_expr(emitter);
}

void ConstantBooleanExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHB);
    emitter.emit(value ? 1 : 0);
}

void ConstantNumberExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHN, value);
}

void ConstantStringExpression::generate_expr(Emitter &emitter) const
{
    unsigned int index = emitter.str(value);
    emitter.emit(PUSHS, index);
}

void ConstantEnumExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHN, number_from_uint32(value));
}

void ConstantNilExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHNIL);
}

void ArrayLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto e = elements.rbegin(); e != elements.rend(); ++e) {
        (*e)->generate(emitter);
    }
    emitter.emit(CONSA, static_cast<uint32_t>(elements.size()));
}

void DictionaryLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto d = dict.rbegin(); d != dict.rend(); ++d) {
        emitter.emit(PUSHS, emitter.str(d->first));
        d->second->generate(emitter);
    }
    emitter.emit(CONSD, static_cast<uint32_t>(dict.size()));
}

void RecordLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto v = values.rbegin(); v != values.rend(); ++v) {
        (*v)->generate(emitter);
    }
    emitter.emit(CONSA, static_cast<uint32_t>(values.size()));
}

void NewRecordExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(ALLOC, static_cast<uint32_t>(fields));
}

void UnaryMinusExpression::generate_expr(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NEGN);
}

void LogicalNotExpression::generate_expr(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NOTB);
}

void ConditionalExpression::generate_expr(Emitter &emitter) const
{
    condition->generate(emitter);
    auto else_label = emitter.create_label();
    emitter.emit_jump(JF, else_label);
    left->generate(emitter);
    auto end_label = emitter.create_label();
    emitter.emit_jump(JUMP, end_label);
    emitter.jump_target(else_label);
    right->generate(emitter);
    emitter.jump_target(end_label);
}

void DisjunctionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    auto true_label = emitter.create_label();
    emitter.emit_jump(JT, true_label);
    emitter.emit(DROP);
    right->generate(emitter);
    emitter.jump_target(true_label);
}

void ConjunctionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    auto false_label = emitter.create_label();
    emitter.emit_jump(JF, false_label);
    emitter.emit(DROP);
    right->generate(emitter);
    emitter.jump_target(false_label);
}

void ArrayInExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(INA);
}

void DictionaryInExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(IND);
}

void ComparisonExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    generate_comparison_opcode(emitter);
}

void ChainedComparisonExpression::generate_expr(Emitter &emitter) const
{
    comps[0]->left->generate(emitter);
    auto skip_label = emitter.create_label();
    size_t i = 0;
    for (auto c: comps) {
        bool last = i == comps.size() - 1;
        c->right->generate(emitter);
        if (not last) {
            emitter.emit(DUPX1);
        }
        c->generate_comparison_opcode(emitter);
        if (not last) {
            emitter.emit_jump(JFCHAIN, skip_label);
        }
        i++;
    }
    emitter.jump_target(skip_label);
}

void BooleanComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQB, NEB};
    emitter.emit(op[comp]);
}

void NumericComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQN, NEN, LTN, GTN, LEN, GEN};
    emitter.emit(op[comp]);
}

void StringComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQS, NES, LTS, GTS, LES, GES};
    emitter.emit(op[comp]);
}

void ArrayComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQA, NEA};
    emitter.emit(op[comp]);
}

void DictionaryComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQD, NED};
    emitter.emit(op[comp]);
}

void PointerComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQP, NEP};
    emitter.emit(op[comp]);
}

void ValidPointerExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    var->generate_address(emitter, 0);
    var->generate_store(emitter);
    right->generate(emitter);
    emitter.emit(NEP);
}

void AdditionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(ADDN);
}

void SubtractionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(SUBN);
}

void MultiplicationExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MULN);
}

void DivisionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(DIVN);
}

void ModuloExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MODN);
}

void ExponentiationExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(EXPN);
}

void ReferenceExpression::generate_load(Emitter &emitter) const
{
    generate_address_read(emitter);
    type->generate_load(emitter);
}

void ReferenceExpression::generate_store(Emitter &emitter) const
{
    generate_address_write(emitter);
    type->generate_store(emitter);
}

void ArrayReferenceIndexExpression::generate_address_read(Emitter &emitter) const
{
    array->generate_address_read(emitter);
    index->generate(emitter);
    if (always_create) {
        emitter.emit(INDEXAW);
    } else {
        emitter.emit(INDEXAR);
    }
}

void ArrayReferenceIndexExpression::generate_address_write(Emitter &emitter) const
{
    array->generate_address_read(emitter);
    index->generate(emitter);
    emitter.emit(INDEXAW);
}

void ArrayValueIndexExpression::generate_expr(Emitter &emitter) const
{
    array->generate(emitter);
    index->generate(emitter);
    if (always_create) {
        emitter.emit(INDEXAN);
    } else {
        emitter.emit(INDEXAV);
    }
}

void DictionaryReferenceIndexExpression::generate_address_read(Emitter &emitter) const
{
    dictionary->generate_address_read(emitter);
    index->generate(emitter);
    emitter.emit(INDEXDR);
}

void DictionaryReferenceIndexExpression::generate_address_write(Emitter &emitter) const
{
    dictionary->generate_address_read(emitter);
    index->generate(emitter);
    emitter.emit(INDEXDW);
}

void DictionaryValueIndexExpression::generate_expr(Emitter &emitter) const
{
    dictionary->generate(emitter);
    index->generate(emitter);
    emitter.emit(INDEXDV);
}

void StringReferenceIndexExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void StringReferenceIndexExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void StringValueIndexExpression::generate_expr(Emitter &emitter) const
{
    load->generate(emitter);
}

void ArrayReferenceRangeExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void ArrayReferenceRangeExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void ArrayValueRangeExpression::generate_expr(Emitter &emitter) const
{
    load->generate(emitter);
}

void PointerDereferenceExpression::generate_address_read(Emitter &emitter) const
{
    ptr->generate(emitter);
}

void PointerDereferenceExpression::generate_address_write(Emitter &emitter) const
{
    ptr->generate(emitter);
}

void VariableExpression::generate_expr(Emitter &emitter) const
{
    generate_address_read(emitter);
    var->generate_load(emitter);
}

void FunctionCall::generate_expr(Emitter &emitter) const
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        const TypeFunctionPointer *pf = dynamic_cast<const TypeFunctionPointer *>(func->type);
        if (pf == nullptr) {
            internal_error("FunctionCall::generate_expr");
        }
        ftype = pf->functype;
    }
    // TODO: This is a ridiculous hack because the way we compile
    // StringIndexExpression::store is not really legal. This assertion
    // holds true for any other function call.
    if (func->text() != "VariableExpression(PredefinedFunction(string__splice, TypeFunction(...)))"
     && func->text() != "VariableExpression(PredefinedFunction(array__splice, TypeFunction(...)))") {
        assert(args.size() == ftype->params.size());
    }
    for (size_t i = 0; i < args.size(); i++) {
        auto param = ftype->params[i];
        auto arg = args[i];
        switch (param->mode) {
            case ParameterType::IN:
                arg->generate(emitter);
                break;
            case ParameterType::INOUT:
                dynamic_cast<const ReferenceExpression *>(arg)->generate_address_read(emitter);
                break;
            case ParameterType::OUT:
                break;
        }
    }
    func->generate_call(emitter);
    for (size_t i = 0; i < args.size(); i++) {
        auto param = ftype->params[i];
        auto arg = args[i];
        switch (param->mode) {
            case ParameterType::IN:
            case ParameterType::INOUT:
                break;
            case ParameterType::OUT:
                dynamic_cast<const ReferenceExpression *>(arg)->generate_store(emitter);
                break;
        }
    }
}

void StatementExpression::generate_expr(Emitter &emitter) const
{
    stmt->generate(emitter);
}

void Statement::generate(Emitter &emitter) const
{
    emitter.debug_line(line);
    generate_code(emitter);
}

void AssertStatement::generate_code(Emitter &emitter) const
{
    auto skip_label = emitter.create_label();
    emitter.emit_jump(JNASSERT, skip_label);
    expr->generate(emitter);
    emitter.emit_jump(JT, skip_label);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit(PUSHS, emitter.str(source));
    emitter.emit(CONSA, 1);
    emitter.emit(EXCEPT, emitter.str("AssertFailed"));
    emitter.jump_target(skip_label);
}

void AssignmentStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
    for (size_t i = 0; i < variables.size() - 1; i++) {
        emitter.emit(DUP);
    }
    for (auto v: variables) {
         v->generate_store(emitter);
    }
}

void ExpressionStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
}

void IfStatement::generate_code(Emitter &emitter) const
{
    auto end_label = emitter.create_label();
    for (auto cs: condition_statements) {
        const Expression *condition = cs.first;
        const std::vector<const Statement *> &statements = cs.second;
        condition->generate(emitter);
        auto else_label = emitter.create_label();
        emitter.emit_jump(JF, else_label);
        for (auto stmt: statements) {
            stmt->generate(emitter);
        }
        emitter.emit_jump(JUMP, end_label);
        emitter.jump_target(else_label);
    }
    for (auto stmt: else_statements) {
        stmt->generate(emitter);
    }
    emitter.jump_target(end_label);
}

void ReturnStatement::generate_code(Emitter &emitter) const
{
    if (expr != nullptr) {
        expr->generate(emitter);
    }
    emitter.emit_jump(JUMP, emitter.get_function_exit());
}

void WhileStatement::generate_code(Emitter &emitter) const
{
    auto top = emitter.create_label();
    emitter.jump_target(top);
    condition->generate(emitter);
    auto skip = emitter.create_label();
    emitter.emit_jump(JF, skip);
    emitter.add_loop_labels(loop_id, skip, top);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit_jump(JUMP, top);
    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void CaseStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
    auto end_label = emitter.create_label();
    for (auto clause: clauses) {
        auto &conditions = clause.first;
        auto &statements = clause.second;
        auto next_label = emitter.create_label();
        if (not conditions.empty()) {
            auto match_label = emitter.create_label();
            for (auto *condition: conditions) {
                condition->generate(emitter);
                emitter.emit_jump(JT, match_label);
            }
            emitter.emit_jump(JUMP, next_label);
            emitter.jump_target(match_label);
        }
        emitter.emit(DROP);
        for (auto stmt: statements) {
            stmt->generate(emitter);
        }
        emitter.emit_jump(JUMP, end_label);
        emitter.jump_target(next_label);
    }
    emitter.jump_target(end_label);
}

void CaseStatement::ComparisonWhenCondition::generate(Emitter &emitter) const
{
    static const unsigned char opn[] = {EQN, NEN, LTN, GTN, LEN, GEN};
    static const unsigned char ops[] = {EQS, NES, LTS, GTS, LES, GES};
    const unsigned char *op;
    if (expr->type == TYPE_NUMBER || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
        op = opn;
    } else if (expr->type == TYPE_STRING) {
        op = ops;
    } else {
        internal_error("when condition not number or string");
    }

    emitter.emit(DUP);
    expr->generate(emitter);
    emitter.emit(op[comp]);
}

void CaseStatement::RangeWhenCondition::generate(Emitter &emitter) const
{
    static const unsigned char opn[] = {EQN, NEN, LTN, GTN, LEN, GEN};
    static const unsigned char ops[] = {EQS, NES, LTS, GTS, LES, GES};
    const unsigned char *op;
    if (low_expr->type == TYPE_NUMBER || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
        op = opn;
    } else if (low_expr->type == TYPE_STRING) {
        op = ops;
    } else {
        internal_error("when condition not number or string");
    }

    emitter.emit(DUP);
    auto result_label = emitter.create_label();
    low_expr->generate(emitter);
    emitter.emit(op[ComparisonExpression::GE]);
    emitter.emit(DUP);
    emitter.emit_jump(JF, result_label);
    emitter.emit(DROP);
    emitter.emit(DUP);
    high_expr->generate(emitter);
    emitter.emit(op[ComparisonExpression::LE]);
    emitter.jump_target(result_label);
}

void ForStatement::generate_code(Emitter &emitter) const
{
    // Decide if we are looping forward or backwards, based on the step value.
    Opcode comp = GEN;
    if (number_is_negative(step->eval_number())) {
        comp = LEN;
    }

    auto skip = emitter.create_label();
    auto loop = emitter.create_label();
    auto next = emitter.create_label();

    start->generate(emitter);
    var->generate_store(emitter);
    end->generate(emitter);
    bound->generate_store(emitter);
    emitter.jump_target(loop);

    bound->generate_load(emitter);
    var->generate_load(emitter);
    emitter.emit(static_cast<char>(comp));
    emitter.emit_jump(JF, skip);

    emitter.add_loop_labels(loop_id, skip, next);

    for (auto stmt: statements) {
        stmt->generate(emitter);
    }

    emitter.jump_target(next);
    step->generate(emitter);
    var->generate_load(emitter);
    emitter.emit(ADDN);
    var->generate_store(emitter);
    emitter.emit_jump(JUMP, loop);

    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void ForeachStatement::generate_code(Emitter &emitter) const
{
    auto skip = emitter.create_label();
    auto loop = emitter.create_label();
    auto next = emitter.create_label();

    emitter.emit(PUSHN, number_from_uint32(0));
    index->generate_store(emitter);
    array->generate(emitter);
    emitter.emit(CALLP, dynamic_cast<const PredefinedFunction *>(array->type->methods.at("size"))->name_index);
    bound->generate_store(emitter);

    emitter.jump_target(loop);
    bound->generate_load(emitter);
    index->generate_load(emitter);
    emitter.emit(GTN);
    emitter.emit_jump(JF, skip);

    emitter.add_loop_labels(loop_id, skip, next);

    array->generate(emitter);
    index->generate(emitter);
    emitter.emit(INDEXAV);
    var->generate_store(emitter);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }

    emitter.jump_target(next);
    index->generate_load(emitter);
    emitter.emit(PUSHN, number_from_uint32(1));
    emitter.emit(ADDN);
    index->generate_store(emitter);
    emitter.emit_jump(JUMP, loop);

    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void LoopStatement::generate_code(Emitter &emitter) const
{
    auto top = emitter.create_label();
    emitter.jump_target(top);
    auto skip = emitter.create_label();
    emitter.add_loop_labels(loop_id, skip, top);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit_jump(JUMP, top);
    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void RepeatStatement::generate_code(Emitter &emitter) const
{
    auto top = emitter.create_label();
    emitter.jump_target(top);
    auto skip = emitter.create_label();
    emitter.add_loop_labels(loop_id, skip, top);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    condition->generate(emitter);
    emitter.emit_jump(JF, top);
    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void ExitStatement::generate_code(Emitter &emitter) const
{
    emitter.emit_jump(JUMP, emitter.get_exit_label(loop_id));
}

void NextStatement::generate_code(Emitter &emitter) const
{
    emitter.emit_jump(JUMP, emitter.get_next_label(loop_id));
}

void TryStatement::generate_code(Emitter &emitter) const
{
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    auto skip = emitter.create_label();
    emitter.emit_jump(JUMP, skip);
    ei.end = emitter.current_ip();
    for (auto c: catches) {
        for (auto e: c.first) {
            ei.excid = emitter.str(e->name);
            ei.handler = emitter.current_ip();
            emitter.add_exception(ei);
        }
        for (auto stmt: c.second) {
            stmt->generate(emitter);
        }
        // TODO: Currently CLREXC only happens when exiting the
        // exception handler block normally. How would the exception
        // get cleared if the handler block is exited some other way,
        // such as a RETURN or EXIT?
        // See: t/exception-clear.neon
        emitter.emit(CLREXC);
        emitter.emit_jump(JUMP, skip);
    }
    emitter.jump_target(skip);
}

void RaiseStatement::generate_code(Emitter &emitter) const
{
    info->generate(emitter);
    unsigned int index = emitter.str(exception->name);
    emitter.emit(EXCEPT, index);
}

void Frame::predeclare(Emitter &emitter)
{
    // Avoid unbounded recursion.
    if (predeclared) {
        return;
    }
    predeclared = true;
    int slot = 0;
    for (auto s: slots) {
        if (s.referenced) {
            // TODO: This hack passes the slot value to LocalVariable
            // names, but doesn't bother for all other kinds of names.
            LocalVariable *lv = dynamic_cast<LocalVariable *>(s.ref);
            if (lv != nullptr) {
                lv->predeclare(emitter, slot);
            } else {
                s.ref->predeclare(emitter);
            }
        }
        slot++;
    }
}

void Frame::postdeclare(Emitter &emitter)
{
    for (auto s: slots) {
        if (s.referenced) {
            s.ref->postdeclare(emitter);
        }
    }
}

void Module::predeclare(Emitter &) const
{
    // TODO? scope->predeclare(emitter);
}

void Program::generate(Emitter &emitter) const
{
    // Call predeclare on standard types so they always work in interpolations.
    TYPE_BOOLEAN->predeclare(emitter);
    TYPE_NUMBER->predeclare(emitter);

    frame->predeclare(emitter);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit(RET);
    frame->postdeclare(emitter);

    TYPE_BOOLEAN->postdeclare(emitter);
    TYPE_NUMBER->postdeclare(emitter);

    // The following block does a topological sort on the dependencies
    // between types. This is necessary so that the export table in a
    // module always declares a type before any types that reference
    // that exported type (using ~ notation).
    std::map<const Type *, std::set<const Type *>> type_references;
    for (auto exp: exports) {
        const Type *type = dynamic_cast<const Type *>(exp.second);
        if (type != nullptr) {
            emitter.declare_export_type(type);
            std::set<const Type *> references;
            type->get_type_references(references);
            type_references[type] = references;
        }
    }
    std::vector<std::pair<std::string, const Type *>> export_types;
    for (auto exp: exports) {
        const Type *type = dynamic_cast<const Type *>(exp.second);
        if (type != nullptr) {
            export_types.push_back(std::make_pair(exp.first, type));
        }
    }
    std::sort(export_types.begin(), export_types.end(), [type_references](const std::pair<std::string, const Type *> &a, const std::pair<std::string, const Type *> &b) { return type_references.at(a.second).find(b.second) == type_references.at(a.second).end(); });
    for (auto t: export_types) {
        t.second->generate_export(emitter, t.first);
    }

    for (auto exp: exports) {
        if (dynamic_cast<const Type *>(exp.second) == nullptr) {
            exp.second->generate_export(emitter, exp.first);
        }
    }
}

std::vector<unsigned char> compile(const Program *p, DebugInfo *debug)
{
    Emitter emitter(p->source_hash, debug);
    p->generate(emitter);
    return emitter.getObject();
}
