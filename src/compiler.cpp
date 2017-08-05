#include "compiler.h"

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iso646.h>
#include <sstream>
#include <stack>

#include "ast.h"
#include "bytecode.h"
#include "debuginfo.h"
#include "opcode.h"

class Emitter {
public:
    class Label {
        friend class Emitter;
    public:
        Label(): fixups(), target(UINT_MAX) {}
    private:
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
    Emitter(const std::string &source_hash, DebugInfo *debug): classes(), source_hash(source_hash), object(), globals(), functions(), function_exit(), current_function_depth(), loop_labels(), exported_types(), debug_info(debug), predefined_name_index() {}
    void emit(unsigned char b);
    void emit_uint32(uint32_t value);
    void emit(unsigned char b, uint32_t value);
    void emit(unsigned char b, uint32_t value, uint32_t value2);
    void emit(unsigned char b, uint32_t value, uint32_t value2, uint32_t value3);
    void emit(unsigned char b, const Number &value);
    void emit(const std::vector<unsigned char> &instr);
    std::vector<unsigned char> getObject();
    unsigned int global(const std::string &name);
    unsigned int str(const std::string &s);
    unsigned int current_ip();
    unsigned int next_function(const std::string &name);
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
    void set_current_function_depth(size_t function_depth);
    size_t get_function_depth();
    void declare_export_type(const ast::Type *type);
    void add_export_type(const std::string &name, const std::string &descriptor);
    void add_export_constant(const std::string &name, const std::string &type, const std::string &value);
    void add_export_variable(const std::string &name, const std::string &type, int index);
    void add_export_function(const std::string &name, const std::string &type, int entry);
    void add_export_exception(const std::string &name);
    void add_export_interface(const std::string &name, const std::vector<std::pair<std::string, std::string>> &method_descriptors);
    void add_import(const std::string &name);
    std::string get_type_reference(const ast::Type *type);
    std::vector<std::pair<const ast::TypeClass *, std::vector<std::vector<Label *>>>> classes;
private:
    const std::string source_hash;
    Bytecode object;
    std::vector<std::string> globals;
    std::vector<std::pair<std::string, Label>> functions;
    std::stack<Label *> function_exit;
    size_t current_function_depth;
    std::map<size_t, LoopLabels> loop_labels;
    std::set<const ast::Type *> exported_types;
    DebugInfo *debug_info;
public:
    std::map<const ast::PredefinedFunction *, int> predefined_name_index;
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

void Emitter::emit(unsigned char b, uint32_t value, uint32_t value2, uint32_t value3)
{
    emit(b);
    emit_uint32(value);
    emit_uint32(value2);
    emit_uint32(value3);
}

void Emitter::emit(unsigned char b, const Number &value)
{
    emit(b, str(number_to_string(value)));
}

void Emitter::emit(const std::vector<unsigned char> &instr)
{
    std::copy(instr.begin(), instr.end(), std::back_inserter(object.code));
}

std::vector<unsigned char> Emitter::getObject()
{
    object.source_hash = source_hash;
    object.global_size = globals.size();
    for (auto f: functions) {
        object.functions.push_back(Bytecode::FunctionInfo(str(f.first), f.second.get_target()));
    }
    for (auto c: classes) {
        Bytecode::ClassInfo ci;
        ci.name = str(c.first->name);
        for (auto i: c.second) {
            std::vector<unsigned int> a;
            for (auto m: i) {
                a.push_back(m->get_target());
            }
            ci.interfaces.push_back(a);
        }
        object.classes.push_back(ci);
    }
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

unsigned int Emitter::next_function(const std::string &name)
{
    auto i = functions.size();
    functions.push_back(std::make_pair(name, create_label()));
    return static_cast<unsigned int>(i);
}

Emitter::Label &Emitter::function_label(int index)
{
    return functions[index].second;
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
    // Turns out the function_exit stack is never more than one function deep. Apparently.
    assert(function_exit.empty());
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

void Emitter::set_current_function_depth(size_t function_depth)
{
    current_function_depth = function_depth;
}

size_t Emitter::get_function_depth()
{
    return current_function_depth;
}

void Emitter::declare_export_type(const ast::Type *type)
{
    exported_types.insert(type);
}

void Emitter::add_export_type(const std::string &name, const std::string &descriptor)
{
    Bytecode::Type type;
    type.name = str(name);
    type.descriptor = str(descriptor);
    object.export_types.push_back(type);
}

void Emitter::add_export_constant(const std::string &name, const std::string &type, const std::string &value)
{
    Bytecode::Constant constant;
    constant.name = str(name);
    constant.type = str(type);
    constant.value = Bytecode::Bytes(value.data(), value.data()+value.length());
    object.export_constants.push_back(constant);
}

void Emitter::add_export_variable(const std::string &name, const std::string &type, int index)
{
    Bytecode::Variable variable;
    variable.name = str(name);
    variable.type = str(type);
    variable.index = index;
    object.export_variables.push_back(variable);
}

void Emitter::add_export_function(const std::string &name, const std::string &type, int entry)
{
    Bytecode::Function function;
    function.name = str(name);
    function.descriptor = str(type);
    function.entry = entry;
    object.export_functions.push_back(function);
}

void Emitter::add_export_exception(const std::string &name)
{
    Bytecode::ExceptionExport exception;
    exception.name = str(name);
    object.export_exceptions.push_back(exception);
}

void Emitter::add_export_interface(const std::string &name, const std::vector<std::pair<std::string, std::string>> &method_descriptors)
{
    Bytecode::Interface interface;
    interface.name = str(name);
    for (auto m: method_descriptors) {
        interface.method_descriptors.push_back(std::make_pair(str(m.first), str(m.second)));
    }
    object.export_interfaces.push_back(interface);
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

std::string Emitter::get_type_reference(const ast::Type *type)
{
    if (type->name.find('.') != std::string::npos || exported_types.find(type) != exported_types.end()) {
        return "~" + type->name + ";";
    }
    return type->get_type_descriptor(*this);
}

void ast::Type::predeclare(Emitter &emitter) const
{
    for (auto m: methods) {
        m.second->predeclare(emitter);
    }
}

void ast::Type::postdeclare(Emitter &emitter) const
{
    for (auto m: methods) {
        m.second->postdeclare(emitter);
    }
}

void ast::Type::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_type(name, get_type_descriptor(emitter));
    for (auto m: methods) {
        // TODO: This is kind of a hack. It seems to work because
        // predefined functions are available everywhere and don't
        // need to be actually exported.
        if (dynamic_cast<const ast::PredefinedFunction *>(m.second) != nullptr) {
            continue;
        }
        const ast::Function *f = dynamic_cast<const ast::Function *>(m.second);
        if (f == nullptr) {
            internal_error("method should be function: " + this->name + "." + m.second->name + " " + m.second->text());
        }
        emitter.add_export_function(name + "." + m.first, f->type->get_type_descriptor(emitter), emitter.function_label(f->entry_label).get_target());
    }
}

void ast::TypeBoolean::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADB);
}

void ast::TypeBoolean::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREB);
}

void ast::TypeBoolean::generate_call(Emitter &) const
{
    internal_error("TypeBoolean");
}

void ast::TypeNumber::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADN);
}

void ast::TypeNumber::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREN);
}

void ast::TypeNumber::generate_call(Emitter &) const
{
    internal_error("TypeNumber");
}

void ast::TypeString::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADS);
}

void ast::TypeString::generate_store(Emitter &emitter) const
{
    emitter.emit(STORES);
}

void ast::TypeString::generate_call(Emitter &) const
{
    internal_error("TypeString");
}

void ast::TypeBytes::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADS);
}

void ast::TypeBytes::generate_store(Emitter &emitter) const
{
    emitter.emit(STORES);
}

void ast::TypeBytes::generate_call(Emitter &) const
{
    internal_error("TypeBytes");
}

void ast::TypeFunction::predeclare(Emitter &emitter) const
{
    Type::predeclare(emitter);
    returntype->predeclare(emitter);
}

void ast::TypeFunction::generate_load(Emitter &) const
{
    internal_error("TypeFunction");
}

void ast::TypeFunction::generate_store(Emitter &) const
{
    internal_error("TypeFunction");
}

void ast::TypeFunction::generate_call(Emitter &) const
{
    internal_error("TypeFunction");
}

std::string ast::TypeFunction::get_type_descriptor(Emitter &emitter) const
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

void ast::TypeArray::predeclare(Emitter &emitter) const
{
    Type::predeclare(emitter);
    if (elementtype != nullptr) {
        elementtype->predeclare(emitter);
    }
}

void ast::TypeArray::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void ast::TypeArray::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void ast::TypeArray::generate_call(Emitter &) const
{
    internal_error("TypeArray");
}

std::string ast::TypeArray::get_type_descriptor(Emitter &emitter) const
{
    return "A<" + emitter.get_type_reference(elementtype) + ">";
}

void ast::TypeArray::get_type_references(std::set<const Type *> &references) const
{
    if (references.insert(elementtype).second) {
        elementtype->get_type_references(references);
    }
}

void ast::TypeDictionary::predeclare(Emitter &emitter) const
{
    Type::predeclare(emitter);
    if (elementtype != nullptr) {
        elementtype->predeclare(emitter);
    }
}

void ast::TypeDictionary::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADD);
}

void ast::TypeDictionary::generate_store(Emitter &emitter) const
{
    emitter.emit(STORED);
}

void ast::TypeDictionary::generate_call(Emitter &) const
{
    internal_error("TypeDictionary");
}

std::string ast::TypeDictionary::get_type_descriptor(Emitter &emitter) const
{
    return "D<" + emitter.get_type_reference(elementtype) + ">";
}

void ast::TypeDictionary::get_type_references(std::set<const Type *> &references) const
{
    if (references.insert(elementtype).second) {
        elementtype->get_type_references(references);
    }
}

void ast::TypeRecord::predeclare(Emitter &emitter) const
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

void ast::TypeRecord::postdeclare(Emitter &emitter) const
{
    // Avoid unbounded recursion.
    if (postdeclared) {
        return;
    }
    postdeclared = true;
    Type::postdeclare(emitter);
}

void ast::TypeRecord::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void ast::TypeRecord::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void ast::TypeRecord::generate_call(Emitter &) const
{
    internal_error("TypeRecord");
}

std::string ast::TypeRecord::get_type_descriptor(Emitter &emitter) const
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

void ast::TypeRecord::get_type_references(std::set<const Type *> &references) const
{
    for (auto f: fields) {
        if (references.insert(f.type).second) {
            f.type->get_type_references(references);
        }
    }
}

std::string ast::TypeClass::get_type_descriptor(Emitter &emitter) const
{
    std::string r = TypeRecord::get_type_descriptor(emitter);
    r[0] = 'C';
    if (not interfaces.empty()) {
        std::string ifaces = "{";
        bool first = true;
        for (auto i: interfaces) {
            if (not first) {
                ifaces.append(",");
            }
            first = false;
            ifaces.append(i->name);
        }
        ifaces.append("}");
        r = r.substr(0, 1) + ifaces + r.substr(1);
    }
    return r;
}

void ast::TypePointer::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADP);
}

void ast::TypePointer::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREP);
}

void ast::TypePointer::generate_call(Emitter &) const
{
    internal_error("TypePointer");
}

void ast::TypePointer::generate_convert(Emitter &emitter, const Type *from) const
{
    if (dynamic_cast<const ast::TypeInterfacePointer *>(from) != nullptr) {
        emitter.emit(PUSHI, 0);
        emitter.emit(INDEXAV);
    }
}

std::string ast::TypePointer::get_type_descriptor(Emitter &emitter) const
{
    return "P<" + (reftype != nullptr ? emitter.get_type_reference(reftype) : "") + ">";
}

void ast::TypePointer::get_type_references(std::set<const Type *> &references) const
{
    if (reftype != nullptr && references.insert(reftype).second) {
        reftype->get_type_references(references);
    }
}

void ast::TypeInterfacePointer::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void ast::TypeInterfacePointer::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void ast::TypeInterfacePointer::generate_call(Emitter &) const
{
    internal_error("TypeInterfacePointer");
}

void ast::TypeInterfacePointer::generate_convert(Emitter &emitter, const Type *from) const
{
    const ast::TypePointer *ptype = dynamic_cast<const ast::TypePointer *>(from);
    if (ptype != nullptr) {
        const ast::TypeClass *ctype = ptype->reftype;
        size_t i = 0;
        while (i < ctype->interfaces.size()) {
            if (interface == ctype->interfaces[i]) {
                break;
            }
            i++;
        }
        assert(i < ctype->interfaces.size());
        emitter.emit(PUSHI, static_cast<uint32_t>(i));
        emitter.emit(SWAP);
        emitter.emit(CONSA, 2);
    }
}

std::string ast::TypeInterfacePointer::get_type_descriptor(Emitter &) const
{
    return "I<" + interface->name + ">";
}

void ast::TypeInterfacePointer::get_type_references(std::set<const Type *> &) const
{
    // TODO
    /*if (references.insert(reftype).second) {
        interface->get_type_references(references);
    }*/
}

void ast::TypeFunctionPointer::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void ast::TypeFunctionPointer::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void ast::TypeFunctionPointer::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLI);
}

std::string ast::TypeFunctionPointer::get_type_descriptor(Emitter &emitter) const
{
    return "Q" + emitter.get_type_reference(functype);
}

void ast::TypeFunctionPointer::get_type_references(std::set<const Type *> &references) const
{
    if (references.insert(functype).second) {
        functype->get_type_references(references);
    }
}

void ast::TypeInterface::generate_convert(Emitter &emitter, const Type *from) const
{
    if (dynamic_cast<const ast::TypeInterfacePointer *>(from) != nullptr) {
        emitter.emit(PUSHI, 0);
        emitter.emit(INDEXAV);
    }
}

void ast::TypeEnum::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADN);
}

void ast::TypeEnum::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREN);
}

void ast::TypeEnum::generate_call(Emitter &) const
{
    internal_error("TypeEnum");
}

std::string ast::TypeEnum::get_type_descriptor(Emitter &) const
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

void ast::Variable::generate_load(Emitter &emitter) const
{
    type->generate_load(emitter);
}

void ast::Variable::generate_store(Emitter &emitter) const
{
    type->generate_store(emitter);
}

void ast::Variable::generate_call(Emitter &emitter) const
{
    type->generate_call(emitter);
}

void ast::PredefinedVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPPG, emitter.str(name));
}

void ast::ModuleVariable::predeclare(Emitter &emitter) const
{
    emitter.add_import(module);
}

void ast::ModuleVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPMG, emitter.str(module), index);
}

void ast::GlobalVariable::predeclare(Emitter &emitter) const
{
    type->predeclare(emitter);
    index = emitter.global(name);
}

void ast::GlobalVariable::generate_address(Emitter &emitter) const
{
    if (index < 0) {
        internal_error("invalid global index: " + name);
    }
    emitter.emit(PUSHPG, index);
}

void ast::GlobalVariable::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_variable(name, emitter.get_type_reference(type), index);
}

void ast::ExternalGlobalVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPEG, emitter.str(name));
}

void ast::LocalVariable::predeclare(Emitter &emitter, int slot)
{
    type->predeclare(emitter);
    index = slot;
}

void ast::LocalVariable::generate_address(Emitter &emitter) const
{
    if (index < 0) {
        internal_error("invalid local index: " + name);
    }
    assert(emitter.get_function_depth() >= nesting_depth);
    if (emitter.get_function_depth() > nesting_depth) {
        emitter.emit(PUSHPOL, static_cast<uint32_t>(emitter.get_function_depth() - nesting_depth), index);
    } else {
        emitter.emit(PUSHPL, index);
    }
}

void ast::FunctionParameter::generate_address(Emitter &emitter) const
{
    if (index < 0) {
        internal_error("invalid local index: " + name);
    }
    assert(emitter.get_function_depth() >= nesting_depth);
    switch (mode) {
        case ParameterType::IN:
        case ParameterType::OUT:
            if (emitter.get_function_depth() > nesting_depth) {
                emitter.emit(PUSHPOL, static_cast<uint32_t>(emitter.get_function_depth() - nesting_depth), index);
            } else {
                emitter.emit(PUSHPL, index);
            }
            break;
        case ParameterType::INOUT:
            if (emitter.get_function_depth() > nesting_depth) {
                emitter.emit(PUSHPOL, static_cast<uint32_t>(emitter.get_function_depth() - nesting_depth), index);
            } else {
                emitter.emit(PUSHPL, index);
            }
            emitter.emit(LOADP);
            break;
    }
}

void ast::Function::predeclare(Emitter &emitter) const
{
    type->predeclare(emitter);
    frame->predeclare(emitter);
    // TODO: This hack ensures that we only allocate the
    // entry label once, even if predeclare() is called
    // more than once.
    if (entry_label == UINT_MAX) {
        entry_label = emitter.next_function(name);
    }
}

void ast::Function::postdeclare(Emitter &emitter) const
{
    emitter.debug_line(declaration.line);
    emitter.jump_target(emitter.function_label(entry_label));
    emitter.set_current_function_depth(nesting_depth);
    emitter.emit(ENTER, static_cast<uint32_t>(nesting_depth), static_cast<uint32_t>(frame->getCount()));
    for (auto p = params.rbegin(); p != params.rend(); ++p) {
        switch ((*p)->mode) {
            case ParameterType::IN:
                (*p)->generate_address(emitter);
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
                (*p)->generate_address(emitter);
                (*p)->generate_load(emitter);
                break;
        }
    }
    emitter.emit(LEAVE);
    emitter.emit(RET);
    frame->postdeclare(emitter);

    auto dot = name.find('.');
    if (dot != std::string::npos) {
        std::string classname = name.substr(0, dot);
        std::string methodname = name.substr(dot+1);
        for (auto &c: emitter.classes) {
            if (c.first->name == classname) {
                for (size_t i = 0; i < c.first->interfaces.size(); i++) {
                    for (size_t m = 0; m < c.first->interfaces[i]->methods.size(); m++) {
                        if (c.first->interfaces[i]->methods[m].first.text == methodname) {
                            c.second[i][m] = &emitter.function_label(entry_label);
                        }
                    }
                }
                break;
            }
        }
    }
}

void ast::Function::generate_load(Emitter &emitter) const
{
    // Get the address of a function for function pointer support.
    emitter.emit_jump(PUSHI, emitter.function_label(entry_label));
    emitter.emit(PUSHM);
    emitter.emit(CONSA, 2);
}

void ast::Function::generate_call(Emitter &emitter) const
{
    emitter.emit_jump(CALLF, emitter.function_label(entry_label));
}

void ast::Function::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_function(name, type->get_type_descriptor(emitter), emitter.function_label(entry_label).get_target());
}

void ast::PredefinedFunction::predeclare(Emitter &emitter) const
{
    emitter.predefined_name_index[this] = emitter.str(name);
}

void ast::PredefinedFunction::generate_call(Emitter &emitter) const
{
    auto i = emitter.predefined_name_index.find(this);
    if (i == emitter.predefined_name_index.end()) {
        //internal_error("predefined function not generated: "+name);
        // If we get here, that means predeclare() wasn't called for this
        // because the traversal never got here. But at this point it's
        // safe to call predeclare() ourselves and sort it out.
        predeclare(emitter);
        i = emitter.predefined_name_index.find(this);
    }
    emitter.emit(CALLP, i->second);
}

void ast::ExtensionFunction::postdeclare(Emitter &emitter) const
{
    emitter.jump_target(emitter.function_label(entry_label));
    generate_call(emitter);
    emitter.emit(RET);
    frame->postdeclare(emitter);
}

void ast::ExtensionFunction::generate_call(Emitter &emitter) const
{
    uint32_t in_param_count = 0;
    uint32_t out_param_count = 0;
    for (auto p: params) {
        switch (p->mode) {
            case ParameterType::IN:
                in_param_count++;
                break;
            case ParameterType::INOUT:
                in_param_count++;
                out_param_count++;
                break;
            case ParameterType::OUT:
                out_param_count++;
                break;
        }
    }
    emitter.emit(CONSA, static_cast<uint32_t>(in_param_count));
    emitter.emit(CALLX, emitter.str(module), emitter.str(name), out_param_count);
    if (dynamic_cast<const TypeFunction *>(type)->returntype == TYPE_NOTHING) {
        emitter.emit(DROPN, out_param_count);
    }
}

void ast::ModuleFunction::predeclare(Emitter &emitter) const
{
    emitter.add_import(module);
}

void ast::ModuleFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLMF, emitter.str(module), entry);
}

void ast::ForeignFunction::predeclare(Emitter &emitter) const
{
    Function::predeclare(emitter);
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
    foreign_index = emitter.str(ss.str());
}

void ast::ForeignFunction::postdeclare(Emitter &emitter) const
{
    emitter.jump_target(emitter.function_label(entry_label));
    generate_call(emitter);
    emitter.emit(RET);
    frame->postdeclare(emitter);
}

void ast::ForeignFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLE, foreign_index);
}

void ast::Exception::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_exception(name);
    for (auto s: subexceptions) {
        s.second->generate_export(emitter, name + "." + s.first);
    }
}

void ast::Interface::generate_export(Emitter &emitter, const std::string &name) const
{
    std::vector<std::pair<std::string, std::string>> method_descriptors;
    for (auto m: methods) {
        method_descriptors.push_back(std::make_pair(m.first.text, m.second->get_type_descriptor(emitter)));
    }
    emitter.add_export_interface(name, method_descriptors);
}

void ast::Constant::generate_export(Emitter &emitter, const std::string &name) const
{
    emitter.add_export_constant(name, emitter.get_type_reference(type), type->serialize(value));
}

void ast::Expression::generate(Emitter &emitter) const
{
    if (type != nullptr) {
        type->predeclare(emitter);
    }
    generate_expr(emitter);
}

void ast::ConstantBooleanExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHB);
    emitter.emit(value ? 1 : 0);
}

void ast::ConstantNumberExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHN, value);
}

void ast::ConstantStringExpression::generate_expr(Emitter &emitter) const
{
    unsigned int index = emitter.str(value);
    emitter.emit(PUSHS, index);
}

void ast::ConstantBytesExpression::generate_expr(Emitter &emitter) const
{
    unsigned int index = emitter.str(contents);
    emitter.emit(PUSHS, index);
}

void ast::ConstantEnumExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHN, number_from_uint32(value));
}

void ast::ConstantNilExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHNIL);
}

void ast::ConstantNowhereExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(PUSHN, number_from_uint32(0));
    emitter.emit(PUSHNIL);
    emitter.emit(CONSA, 2);
}

void ast::ArrayLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto e = elements.rbegin(); e != elements.rend(); ++e) {
        (*e)->generate(emitter);
    }
    emitter.emit(CONSA, static_cast<uint32_t>(elements.size()));
}

void ast::DictionaryLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto d = dict.rbegin(); d != dict.rend(); ++d) {
        emitter.emit(PUSHS, emitter.str(d->first));
        d->second->generate(emitter);
    }
    emitter.emit(CONSD, static_cast<uint32_t>(dict.size()));
}

void ast::RecordLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto v = values.rbegin(); v != values.rend(); ++v) {
        (*v)->generate(emitter);
    }
    emitter.emit(CONSA, static_cast<uint32_t>(values.size()));
}

void ast::ClassLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (size_t i = values.size()-1; i > 0; i--) {
        values[i]->generate(emitter);
    }
    emitter.emit(PUSHCI, emitter.str(type->name));
    emitter.emit(CONSA, static_cast<uint32_t>(values.size()));
}

void ast::NewClassExpression::generate_expr(Emitter &emitter) const
{
    if (value != nullptr) {
        value->generate(emitter);
    }
    emitter.emit(ALLOC, static_cast<uint32_t>(reftype->fields.size()));
    if (value != nullptr) {
        emitter.emit(DUPX1);
        emitter.emit(STOREA);
    }
}

void ast::UnaryMinusExpression::generate_expr(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NEGN);
}

void ast::LogicalNotExpression::generate_expr(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NOTB);
}

void ast::ConditionalExpression::generate_expr(Emitter &emitter) const
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

void ast::TryExpression::generate_expr(Emitter &emitter) const
{
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    expr->generate(emitter);
    auto skip = emitter.create_label();
    emitter.emit_jump(JUMP, skip);
    ei.end = emitter.current_ip();
    for (auto &c: catches) {
        for (auto e: c.exceptions) {
            ei.excid = emitter.str(e->name);
            ei.handler = emitter.current_ip();
            emitter.add_exception(ei);
        }
        if (c.name != nullptr) {
            c.name->generate_address(emitter);
            c.name->generate_store(emitter);
        } else {
            emitter.emit(DROP);
        }
        const ExceptionHandlerStatement *ehs = dynamic_cast<const ExceptionHandlerStatement *>(c.handler);
        const Expression *e = dynamic_cast<const Expression *>(c.handler);
        if (ehs != nullptr) {
            for (auto stmt: ehs->statements) {
                stmt->generate(emitter);
            }
        } else if (e != nullptr) {
            e->generate(emitter);
        } else {
            internal_error("unexpected catch type");
        }
        emitter.emit_jump(JUMP, skip);
    }
    emitter.jump_target(skip);
}

void ast::DisjunctionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    auto true_label = emitter.create_label();
    emitter.emit_jump(JT, true_label);
    emitter.emit(DROP);
    right->generate(emitter);
    emitter.jump_target(true_label);
}

void ast::ConjunctionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    auto false_label = emitter.create_label();
    emitter.emit_jump(JF, false_label);
    emitter.emit(DROP);
    right->generate(emitter);
    emitter.jump_target(false_label);
}

void ast::ArrayInExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(INA);
}

void ast::DictionaryInExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(IND);
}

void ast::ComparisonExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    generate_comparison_opcode(emitter);
}

void ast::ChainedComparisonExpression::generate_expr(Emitter &emitter) const
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

void ast::BooleanComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQB, NEB};
    emitter.emit(op[comp]);
}

void ast::NumericComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQN, NEN, LTN, GTN, LEN, GEN};
    emitter.emit(op[comp]);
}

void ast::EnumComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQN, NEN, LTN, GTN, LEN, GEN};
    emitter.emit(op[comp]);
}

void ast::StringComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQS, NES, LTS, GTS, LES, GES};
    emitter.emit(op[comp]);
}

void ast::BytesComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQS, NES, LTS, GTS, LES, GES};
    emitter.emit(op[comp]);
}

void ast::ArrayComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQA, NEA};
    emitter.emit(op[comp]);
}

void ast::RecordComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQA, NEA};
    emitter.emit(op[comp]);
}

void ast::DictionaryComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQD, NED};
    emitter.emit(op[comp]);
}

void ast::PointerComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQP, NEP};
    emitter.emit(op[comp]);
}

void ast::FunctionPointerComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    static const unsigned char op[] = {EQA, NEA};
    emitter.emit(op[comp]);
}

void ast::ValidPointerExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    var->generate_address(emitter);
    var->generate_store(emitter);
    right->generate(emitter);
    emitter.emit(NEP);
}

void ast::AdditionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(ADDN);
}

void ast::SubtractionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(SUBN);
}

void ast::MultiplicationExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MULN);
}

void ast::DivisionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(DIVN);
}

void ast::ModuloExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MODN);
}

void ast::ExponentiationExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(EXPN);
}

void ast::ReferenceExpression::generate_load(Emitter &emitter) const
{
    generate_address_read(emitter);
    type->generate_load(emitter);
}

void ast::ReferenceExpression::generate_store(Emitter &emitter) const
{
    generate_address_write(emitter);
    type->generate_store(emitter);
}

void ast::DummyExpression::generate_store(Emitter &emitter) const
{
    emitter.emit(DROP);
}

void ast::ArrayReferenceIndexExpression::generate_address_read(Emitter &emitter) const
{
    array->generate_address_read(emitter);
    index->generate(emitter);
    if (always_create) {
        emitter.emit(INDEXAW);
    } else {
        emitter.emit(INDEXAR);
    }
}

void ast::ArrayReferenceIndexExpression::generate_address_write(Emitter &emitter) const
{
    array->generate_address_write(emitter);
    index->generate(emitter);
    emitter.emit(INDEXAW);
}

void ast::ArrayValueIndexExpression::generate_expr(Emitter &emitter) const
{
    array->generate(emitter);
    index->generate(emitter);
    if (always_create) {
        emitter.emit(INDEXAN);
    } else {
        emitter.emit(INDEXAV);
    }
}

void ast::DictionaryReferenceIndexExpression::generate_address_read(Emitter &emitter) const
{
    dictionary->generate_address_read(emitter);
    index->generate(emitter);
    emitter.emit(INDEXDR);
}

void ast::DictionaryReferenceIndexExpression::generate_address_write(Emitter &emitter) const
{
    dictionary->generate_address_write(emitter);
    index->generate(emitter);
    emitter.emit(INDEXDW);
}

void ast::DictionaryValueIndexExpression::generate_expr(Emitter &emitter) const
{
    dictionary->generate(emitter);
    index->generate(emitter);
    emitter.emit(INDEXDV);
}

void ast::StringReferenceIndexExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::StringReferenceIndexExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void ast::StringValueIndexExpression::generate_expr(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::BytesReferenceIndexExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::BytesReferenceIndexExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void ast::BytesValueIndexExpression::generate_expr(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::RecordReferenceFieldExpression::generate_address_read(Emitter &emitter) const
{
    ref->generate_address_read(emitter);
    const ast::TypeRecord *rectype = dynamic_cast<const ast::TypeRecord *>(ref->type);
    if (rectype == nullptr) {
        internal_error("RecordReferenceFieldExpression");
    }
    auto f = rectype->field_names.find(field);
    if (f == rectype->field_names.end()) {
        internal_error("RecordReferenceFieldExpression");
    }
    emitter.emit(PUSHN, number_from_uint32(static_cast<uint32_t>(f->second)));
    if (always_create) {
        emitter.emit(INDEXAW);
    } else {
        emitter.emit(INDEXAR);
    }
}

void ast::RecordReferenceFieldExpression::generate_address_write(Emitter &emitter) const
{
    ref->generate_address_write(emitter);
    const ast::TypeRecord *rectype = dynamic_cast<const ast::TypeRecord *>(ref->type);
    if (rectype == nullptr) {
        internal_error("RecordReferenceFieldExpression");
    }
    auto f = rectype->field_names.find(field);
    if (f == rectype->field_names.end()) {
        internal_error("RecordReferenceFieldExpression");
    }
    emitter.emit(PUSHN, number_from_uint32(static_cast<uint32_t>(f->second)));
    emitter.emit(INDEXAW);
}

void ast::RecordValueFieldExpression::generate_expr(Emitter &emitter) const
{
    rec->generate(emitter);
    const ast::TypeRecord *rectype = dynamic_cast<const ast::TypeRecord *>(rec->type);
    if (rectype == nullptr) {
        internal_error("RecordValueFieldExpression");
    }
    auto f = rectype->field_names.find(field);
    if (f == rectype->field_names.end()) {
        internal_error("RecordValueFieldExpression");
    }
    emitter.emit(PUSHN, number_from_uint32(static_cast<uint32_t>(f->second)));
    if (always_create) {
        emitter.emit(INDEXAN);
    } else {
        emitter.emit(INDEXAV);
    }
}

void ast::ArrayReferenceRangeExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::ArrayReferenceRangeExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void ast::ArrayValueRangeExpression::generate_expr(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::PointerDereferenceExpression::generate_address_read(Emitter &emitter) const
{
    ptr->generate(emitter);
}

void ast::PointerDereferenceExpression::generate_address_write(Emitter &emitter) const
{
    ptr->generate(emitter);
}

void ast::VariableExpression::generate_expr(Emitter &emitter) const
{
    generate_address_read(emitter);
    var->generate_load(emitter);
}

void ast::InterfaceMethodExpression::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLV, static_cast<uint32_t>(index));
}

void ast::FunctionCall::generate_parameters(Emitter &emitter) const
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        const TypeFunctionPointer *pf = dynamic_cast<const TypeFunctionPointer *>(func->type);
        if (pf == nullptr) {
            const InterfaceMethodExpression *ime = dynamic_cast<const InterfaceMethodExpression *>(func);
            if (ime == nullptr) {
                internal_error("FunctionCall::generate_expr " + func->text());
            } else {
                ftype = ime->functype;
            }
        } else {
            ftype = pf->functype;
        }
    }
    // TODO: This is a ridiculous hack because the way we compile
    // StringIndexExpression::store is not really legal. This assertion
    // holds true for any other function call.
    if (func->text() != "VariableExpression(PredefinedFunction(string__splice, TypeFunction(...)))"
     && func->text() != "VariableExpression(PredefinedFunction(bytes__splice, TypeFunction(...)))"
     && func->text() != "VariableExpression(PredefinedFunction(array__splice, TypeFunction(...)))") {
        assert(args.size() == ftype->params.size());
    }
    for (size_t i = 0; i < args.size(); i++) {
        auto param = ftype->params[i];
        auto arg = args[i];
        switch (param->mode) {
            case ParameterType::IN:
                arg->generate(emitter);
                if (param->type != nullptr) {
                    param->type->generate_convert(emitter, arg->type);
                }
                break;
            case ParameterType::INOUT:
                dynamic_cast<const ReferenceExpression *>(arg)->generate_address_read(emitter);
                break;
            case ParameterType::OUT:
                break;
        }
    }
}

void ast::FunctionCall::generate_expr(Emitter &emitter) const
{
    generate_parameters(emitter);
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        const TypeFunctionPointer *pf = dynamic_cast<const TypeFunctionPointer *>(func->type);
        if (pf == nullptr) {
            const InterfaceMethodExpression *ime = dynamic_cast<const InterfaceMethodExpression *>(func);
            if (ime == nullptr) {
                internal_error("FunctionCall::generate_expr");
            } else {
                ftype = ime->functype;
            }
        } else {
            ftype = pf->functype;
        }
    }
    if (dispatch != nullptr) {
        dispatch->generate_expr(emitter);
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

bool ast::FunctionCall::all_in_parameters() const
{
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    if (ftype == nullptr) {
        const TypeFunctionPointer *pf = dynamic_cast<const TypeFunctionPointer *>(func->type);
        if (pf == nullptr) {
            internal_error("FunctionCall::generate_expr");
        }
        ftype = pf->functype;
    }
    for (size_t i = 0; i < args.size(); i++) {
        if (ftype->params[i]->mode != ParameterType::IN) {
            return false;
        }
    }
    return true;
}

void ast::StatementExpression::generate_expr(Emitter &emitter) const
{
    stmt->generate(emitter);
}

void ast::Statement::generate(Emitter &emitter) const
{
    emitter.debug_line(line);
    generate_code(emitter);
}

void ast::TypeDeclarationStatement::generate_code(Emitter &emitter) const
{
    // This might better be a virtual function call on Type.
    const ast::TypeClass *type_class = dynamic_cast<const ast::TypeClass *>(type);
    if (type_class != nullptr) {
        std::vector<std::vector<Emitter::Label *>> ifs;
        for (auto i: type_class->interfaces) {
            ifs.push_back(std::vector<Emitter::Label *>(i->methods.size()));
        }
        emitter.classes.push_back(std::make_pair(type_class, ifs));
    }
}

void ast::ExceptionHandlerStatement::generate_code(Emitter &emitter) const
{
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
}

void ast::AssertStatement::generate_code(Emitter &emitter) const
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
    emitter.emit(EXCEPT, emitter.str("AssertFailedException"));
    emitter.jump_target(skip_label);
}

void ast::AssignmentStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
    for (size_t i = 0; i < variables.size() - 1; i++) {
        emitter.emit(DUP);
    }
    for (auto v: variables) {
         v->type->generate_convert(emitter, expr->type);
         v->generate_store(emitter);
    }
}

void ast::ExpressionStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
}

void ast::IncrementStatement::generate_code(Emitter &emitter) const
{
    // TODO: This can be improved considerably.
    ref->generate_address_read(emitter);
    emitter.emit(LOADN);
    emitter.emit(PUSHN, number_from_sint32(delta));
    emitter.emit(ADDN);
    ref->generate_address_write(emitter);
    emitter.emit(STOREN);
}

void ast::IfStatement::generate_code(Emitter &emitter) const
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

void ast::ReturnStatement::generate_code(Emitter &emitter) const
{
    // Check for a possible tail call. We can only do this if all
    // of the following conditions hold:
    //  - The RETURN statement returns only a call to another function
    //  - The function takes only IN parameters (it might also be possible
    //    to do this with some kinds of INOUT parameters, but not in general)
    //  - The function is an ordinary function that we can jump to (not a
    //    predefined function or foreign function)
    //  - The nesting level of the next function is at the same level or
    //    further out than this function.
    // If possible, then generate the parameters for the next function,
    // emit a LEAVE instruction to discard our frame, and then jump to
    // the entry point for the next function.
    const FunctionCall *call = dynamic_cast<const FunctionCall *>(expr);
    if (call != nullptr) {
        if (call->all_in_parameters()) {
            const VariableExpression *var = dynamic_cast<const VariableExpression *>(call->func);
            if (var != nullptr) {
                const Function *func = dynamic_cast<const Function *>(var->var);
                if (func != nullptr) {
                    if (func->nesting_depth <= emitter.get_function_depth()) {
                        call->generate_parameters(emitter);
                        emitter.emit(LEAVE);
                        emitter.emit_jump(JUMP, emitter.function_label(func->entry_label));
                        return;
                    }
                }
            }
        }
    }

    if (expr != nullptr) {
        expr->generate(emitter);
    }
    emitter.emit_jump(JUMP, emitter.get_function_exit());
}

void ast::CaseStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
    auto end_label = emitter.create_label();

    if (dynamic_cast<const TypeNumber *>(expr->type) != nullptr) {
        std::map<int32_t, const std::vector<const Statement *> *> values;
        std::vector<const Statement *> when_others;
        bool all_integer_equality = true;
        for (auto &clause: clauses) {
            for (auto cond: clause.first) {
                const ComparisonWhenCondition *comp = dynamic_cast<const ComparisonWhenCondition *>(cond);
                if (comp != nullptr && comp->comp == ComparisonExpression::EQ) {
                    Number n = comp->expr->eval_number(Token());
                    if (number_is_integer(n)) {
                        int32_t x = number_to_sint32(n);
                        if (x > INT32_MIN && x < INT32_MAX) {
                            values[x] = &clause.second;
                        }
                    } else {
                        all_integer_equality = false;
                    }
                } else {
                    all_integer_equality = false;
                }
            }
            if (clause.first.empty()) {
                when_others = clause.second;
            }
        }
        if (all_integer_equality && not values.empty()) {
            const int32_t min = values.begin()->first;
            const int32_t max = values.rbegin()->first;
            const int32_t range = max - min + 1;
            if (range < 10000 && values.size() > static_cast<size_t>(range)/10) {
                if (min != 0) {
                    emitter.emit(PUSHN, number_from_sint32(min));
                    emitter.emit(SUBN);
                }
                emitter.emit(JUMPTBL, range);
                std::map<const std::vector<const Statement *> *, Emitter::Label> labels;
                auto others_label = emitter.create_label();
                for (int32_t i = min; i <= max; i++) {
                    auto v = values.find(i);
                    if (v != values.end()) {
                        auto label = labels.find(v->second);
                        if (label == labels.end()) {
                            labels[v->second] = emitter.create_label();
                        }
                        emitter.emit_jump(JUMP, labels[v->second]);
                    } else {
                        emitter.emit_jump(JUMP, others_label);
                    }
                }
                emitter.emit_jump(JUMP, others_label);
                for (auto &label: labels) {
                    emitter.jump_target(label.second);
                    for (auto stmt: *label.first) {
                        stmt->generate(emitter);
                    }
                    emitter.emit_jump(JUMP, end_label);
                }
                emitter.jump_target(others_label);
                for (auto stmt: when_others) {
                    stmt->generate(emitter);
                }
                emitter.jump_target(end_label);
                return;
            }
        }
    }

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

void ast::CaseStatement::ComparisonWhenCondition::generate(Emitter &emitter) const
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

void ast::CaseStatement::RangeWhenCondition::generate(Emitter &emitter) const
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

void ast::BaseLoopStatement::generate_code(Emitter &emitter) const
{
    for (auto stmt: prologue) {
        stmt->generate(emitter);
    }
    auto top = emitter.create_label();
    emitter.jump_target(top);
    auto skip = emitter.create_label();
    auto next = emitter.create_label();
    emitter.add_loop_labels(loop_id, skip, next);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.jump_target(next);
    for (auto stmt: tail) {
        stmt->generate(emitter);
    }
    emitter.emit_jump(JUMP, top);
    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void ast::ExitStatement::generate_code(Emitter &emitter) const
{
    emitter.emit_jump(JUMP, emitter.get_exit_label(loop_id));
}

void ast::NextStatement::generate_code(Emitter &emitter) const
{
    emitter.emit_jump(JUMP, emitter.get_next_label(loop_id));
}

void ast::TryStatement::generate_code(Emitter &emitter) const
{
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    auto skip = emitter.create_label();
    emitter.emit_jump(JUMP, skip);
    ei.end = emitter.current_ip();
    for (auto &c: catches) {
        for (auto e: c.exceptions) {
            ei.excid = emitter.str(e->name);
            ei.handler = emitter.current_ip();
            emitter.add_exception(ei);
        }
        if (c.name != nullptr) {
            c.name->generate_address(emitter);
            c.name->generate_store(emitter);
        } else {
            emitter.emit(DROP);
        }
        const ExceptionHandlerStatement *ehs = dynamic_cast<const ExceptionHandlerStatement *>(c.handler);
        if (ehs != nullptr) {
            for (auto stmt: ehs->statements) {
                stmt->generate(emitter);
            }
        } else {
            internal_error("unexpected catch type");
        }
        emitter.emit_jump(JUMP, skip);
    }
    emitter.jump_target(skip);
}

void ast::RaiseStatement::generate_code(Emitter &emitter) const
{
    info->generate(emitter);
    unsigned int index = emitter.str(exception->name);
    emitter.emit(EXCEPT, index);
}

void ast::ResetStatement::generate_code(Emitter &emitter) const
{
    for (auto v: variables) {
         v->generate_address_write(emitter);
         emitter.emit(RESETC);
    }
}

void ast::Frame::predeclare(Emitter &emitter)
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

void ast::Frame::postdeclare(Emitter &emitter)
{
    for (auto s: slots) {
        if (s.referenced) {
            s.ref->postdeclare(emitter);
        }
    }
}

void ast::ExternalGlobalFrame::predeclare(Emitter &emitter)
{
    outer->predeclare(emitter);
    Frame::predeclare(emitter);
}

void ast::Module::predeclare(Emitter &) const
{
    // TODO? scope->predeclare(emitter);
}

static std::vector<std::pair<std::string, const ast::Type *>> topo_sort(std::vector<std::pair<std::string, const ast::Type *>> types, std::map<const ast::Type *, std::set<const ast::Type *>> references)
{
    const size_t orig_types_size = types.size();
    // First, scrub the dependency list of things we aren't actually exporting.
    // Also remove self-references.
    for (auto &ref: references) {
        auto t = ref.second.begin();
        while (t != ref.second.end()) {
            auto r = references.find(*t);
            if (r == references.end() || *t == ref.first) {
                auto u = t;
                ++t;
                ref.second.erase(u);
            } else {
                ++t;
            }
        }
    }
    std::vector<std::pair<std::string, const ast::Type *>> r;
    while (not types.empty()) {
        bool any = false;
        for (auto t = types.begin(); t != types.end(); ++t) {
            auto ref = references.find(t->second);
            if (ref != references.end() && ref->second.size() == 0) {
                r.push_back(*t);
                for (auto &rr: references) {
                    rr.second.erase(t->second);
                }
                types.erase(t);
                any = true;
                break;
            }
        }
        if (not any) {
            internal_error("recursive exported type dependency");
        }
    }
    assert(r.size() == orig_types_size);
    return r;
}

void ast::Program::generate(Emitter &emitter) const
{
    // Call predeclare on standard types so they always work in interpolations.
    TYPE_BOOLEAN->predeclare(emitter);
    TYPE_NUMBER->predeclare(emitter);

    frame->predeclare(emitter);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    const Function *main = dynamic_cast<const Function *>(scope->lookupName("MAIN"));
    if (main != nullptr) {
        emitter.emit(CALLP, emitter.str("runtime$moduleIsMain"));
        auto skip = emitter.create_label();
        emitter.emit_jump(JF, skip);
        emitter.emit_jump(CALLF, emitter.function_label(main->entry_label));
        emitter.jump_target(skip);
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
    export_types = topo_sort(export_types, type_references);
    for (auto t: export_types) {
        t.second->generate_export(emitter, t.first);
    }

    for (auto exp: exports) {
        if (dynamic_cast<const Type *>(exp.second) == nullptr) {
            exp.second->generate_export(emitter, exp.first);
        }
    }
}

static std::string hex_from_binary(const std::string &bin)
{
    std::stringstream r;
    r << std::hex << std::setfill('0');
    for (auto b: bin) {
        r << std::setw(2) << (static_cast<int>(b) & 0xff);
    }
    return r.str();
}

void ast::TypeBoolean::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Boolean");
    type.write("representation", "boolean");
    type.close();
}

void ast::TypeNumber::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Number");
    type.write("representation", "number");
    type.close();
}

void ast::TypeString::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "String");
    type.write("representation", "string");
    type.close();
}

void ast::TypeBytes::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Bytes");
    type.write("representation", "string");
    type.close();
}

void ast::TypeFunction::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Function(...)");
    type.write("representation", "function");
    type.close();
}

void ast::TypeArray::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Array");
    type.write("representation", "array");
    type.close();
}

void ast::TypeDictionary::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Dictionary");
    type.write("representation", "dictionary");
    type.close();
}

void ast::TypeRecord::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Record");
    type.write("representation", "array");
    type.close();
}

void ast::TypePointer::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Pointer");
    type.write("representation", "address");
    type.close();
}

void ast::TypeInterfacePointer::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "InterfacePointer");
    type.write("representation", "array");
    type.close();
}

void ast::TypeFunctionPointer::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "FunctionPointer");
    type.write("representation", "array");
    type.close();
}

void ast::TypeEnum::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Enum");
    type.write("representation", "number");
    type.close();
}

void ast::TypeModule::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Module");
    type.write("representation", "module");
    type.close();
}

void ast::TypeException::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Exception");
    type.write("representation", "none");
    type.close();
}

void ast::TypeInterface::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto type = out.nested_object("type");
    type.write("display", "Interface");
    type.write("representation", "none");
    type.close();
}

void ast::Function::debuginfo(Emitter &emitter, minijson::object_writer &out) const
{
    out.write("entry", emitter.function_label(entry_label).get_target());
    auto locals = out.nested_array("locals");
    for (size_t i = 0; i < frame->getCount(); i++) {
        auto slot = frame->getSlot(i);
        const LocalVariable *local = dynamic_cast<const LocalVariable *>(slot.ref);
        if (local != nullptr) {
            auto var = locals.nested_object();
            var.write("name", slot.name);
            var.write("index", local->index);
            local->type->debuginfo(emitter, var);
            var.close();
        }
    }
    locals.close();
}

void ast::Program::debuginfo(Emitter &emitter, minijson::object_writer &out) const
{
    {
        auto globals = out.nested_array("globals");
        for (size_t i = 0; i < frame->getCount(); i++) {
            auto slot = frame->getSlot(i);
            const GlobalVariable *global = dynamic_cast<const GlobalVariable *>(slot.ref);
            if (global != nullptr) {
                auto var = globals.nested_object();
                var.write("name", slot.name);
                var.write("index", global->index);
                global->type->debuginfo(emitter, var);
                var.close();
            }
        }
        globals.close();
    }
    {
        auto functions = out.nested_array("functions");
        for (size_t i = 0; i < frame->getCount(); i++) {
            auto slot = frame->getSlot(i);
            const Function *function = dynamic_cast<const Function *>(slot.ref);
            if (function != nullptr) {
                auto func = functions.nested_object();
                func.write("name", slot.name);
                function->debuginfo(emitter, func);
                func.close();
            }
        }
        functions.close();
    }
}

std::vector<unsigned char> compile(const ast::Program *p, DebugInfo *debug)
{
    Emitter emitter(p->source_hash, debug);
    p->generate(emitter);
    if (p->source_path != "-" && debug != nullptr) {
        std::ofstream out(p->source_path + "d");
        minijson::object_writer writer(out, minijson::writer_configuration().pretty_printing(true));
        writer.write("source_path", p->source_path);
        writer.write("source_hash", hex_from_binary(p->source_hash));
        writer.write_array("source", debug->source_lines.begin(), debug->source_lines.end());
        std::vector<std::pair<size_t, int>> lines;
        auto lw = writer.nested_array("line_numbers");
        for (auto &n: debug->line_numbers) {
            auto a = lw.nested_array();
            a.write(n.first);
            a.write(n.second);
            a.close();
        }
        lw.close();
        p->debuginfo(emitter, writer);
        writer.close();
    }
    return emitter.getObject();
}
