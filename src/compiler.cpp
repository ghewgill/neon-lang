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
        Label(): reachable(false), fixups(), target(UINT_MAX), target_stack_depth(-1) {}
    private:
        bool reachable;
        std::vector<unsigned int> fixups;
        unsigned int target;
        int target_stack_depth;
    public:
        bool is_reachable() { return reachable; }
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
    class FunctionInfo {
    public:
        FunctionInfo(const std::string &name, const Label &label): name(name), nest(0), params(0), locals(0), entry_label(label) {}
        std::string name;
        int nest;
        int params;
        int locals;
        Label entry_label;
    };
public:
    Emitter(const std::string &source_hash, DebugInfo *debug): classes(), source_hash(source_hash), object(), globals(), functions({FunctionInfo("", Label())}), function_exit(), current_function_depth(), stack_depth(0), in_jumptbl(false), loop_labels(), exported_types(), debug_info(debug) {}
    Emitter(const Emitter &) = delete;
    Emitter &operator=(const Emitter &) = delete;
    void emit_byte(unsigned char b);
    void emit(Opcode b);
    void emit_uint32(uint32_t value);
    void emit(Opcode b, uint32_t value);
    void emit(Opcode b, uint32_t value, uint32_t value2);
    void emit(Opcode b, uint32_t value, uint32_t value2, uint32_t value3);
    void emit(Opcode b, const Number &value);
    void emit(const std::vector<unsigned char> &instr);
    std::vector<unsigned char> getObject();
    unsigned int global(const std::string &name);
    unsigned int str(const char *s) { return str(std::string(s)); }
    unsigned int str(const std::string &s);
    unsigned int str(const utf8string &s) { return str(s.str()); }
    unsigned int current_ip();
    unsigned int next_function(const std::string &name);
    FunctionInfo &function_info(int index);
    Label create_label();
    void emit_jump(Opcode b, Label &label);
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
    size_t get_function_depth() const;
    void declare_export_type(const ast::Type *type);
    void add_export_type(const std::string &name, const std::string &descriptor);
    void add_export_constant(const std::string &name, const std::string &type, const std::string &value);
    void add_export_variable(const std::string &name, const std::string &type, int index);
    void add_export_function(const std::string &name, const std::string &type, int index);
    void add_export_exception(const std::string &name);
    void add_export_interface(const std::string &name, const std::vector<std::pair<std::string, std::string>> &method_descriptors);
    void add_import(const ast::Module *module);
    std::string get_type_reference(const ast::Type *type);
    int get_stack_depth() { return stack_depth; }
    void set_stack_depth(int depth) { stack_depth = depth; }
    void adjust_stack_depth(int delta) { stack_depth += delta; }
    std::vector<std::pair<const ast::TypeClass *, std::vector<std::vector<int>>>> classes;
private:
    const std::string source_hash;
    Bytecode object;
    std::vector<std::string> globals;
    std::vector<FunctionInfo> functions;
    std::stack<Label *> function_exit;
    size_t current_function_depth;
    int stack_depth;
    bool in_jumptbl;
    std::map<size_t, LoopLabels> loop_labels;
    std::set<const ast::Type *> exported_types;
    DebugInfo *debug_info;
};

void Emitter::emit_byte(unsigned char b)
{
    object.code.push_back(b);
}

void Emitter::emit(Opcode b)
{
    if (debug_info != nullptr) {
        debug_info->stack_depth[object.code.size()] = stack_depth;
    }
    emit_byte(static_cast<unsigned char>(b));
    if (stack_depth >= 0) {
        if (in_jumptbl) {
            in_jumptbl = (b == Opcode::JUMP);
        }
        switch (b) {
            case Opcode::PUSHB:     stack_depth += 1; break;
            case Opcode::PUSHN:     stack_depth += 1; break;
            case Opcode::PUSHS:     stack_depth += 1; break;
            case Opcode::PUSHY:     stack_depth += 1; break;
            case Opcode::PUSHPG:    stack_depth += 1; break;
            case Opcode::PUSHPPG:   stack_depth += 1; break;
            case Opcode::PUSHPMG:   stack_depth += 1; break;
            case Opcode::PUSHPL:    stack_depth += 1; break;
            case Opcode::PUSHPOL:   stack_depth += 1; break;
            case Opcode::PUSHI:     stack_depth += 1; break;
            case Opcode::LOADB:     break;
            case Opcode::LOADN:     break;
            case Opcode::LOADS:     break;
            case Opcode::LOADY:     break;
            case Opcode::LOADA:     break;
            case Opcode::LOADD:     break;
            case Opcode::LOADP:     break;
            case Opcode::LOADJ:     break;
            case Opcode::LOADV:     break;
            case Opcode::STOREB:    stack_depth -= 2; break;
            case Opcode::STOREN:    stack_depth -= 2; break;
            case Opcode::STORES:    stack_depth -= 2; break;
            case Opcode::STOREY:    stack_depth -= 2; break;
            case Opcode::STOREA:    stack_depth -= 2; break;
            case Opcode::STORED:    stack_depth -= 2; break;
            case Opcode::STOREP:    stack_depth -= 2; break;
            case Opcode::STOREJ:    stack_depth -= 2; break;
            case Opcode::STOREV:    stack_depth -= 2; break;
            case Opcode::NEGN:      break;
            case Opcode::ADDN:      stack_depth -= 1; break;
            case Opcode::SUBN:      stack_depth -= 1; break;
            case Opcode::MULN:      stack_depth -= 1; break;
            case Opcode::DIVN:      stack_depth -= 1; break;
            case Opcode::MODN:      stack_depth -= 1; break;
            case Opcode::EXPN:      stack_depth -= 1; break;
            case Opcode::EQB:       stack_depth -= 1; break;
            case Opcode::NEB:       stack_depth -= 1; break;
            case Opcode::EQN:       stack_depth -= 1; break;
            case Opcode::NEN:       stack_depth -= 1; break;
            case Opcode::LTN:       stack_depth -= 1; break;
            case Opcode::GTN:       stack_depth -= 1; break;
            case Opcode::LEN:       stack_depth -= 1; break;
            case Opcode::GEN:       stack_depth -= 1; break;
            case Opcode::EQS:       stack_depth -= 1; break;
            case Opcode::NES:       stack_depth -= 1; break;
            case Opcode::LTS:       stack_depth -= 1; break;
            case Opcode::GTS:       stack_depth -= 1; break;
            case Opcode::LES:       stack_depth -= 1; break;
            case Opcode::GES:       stack_depth -= 1; break;
            case Opcode::EQY:       stack_depth -= 1; break;
            case Opcode::NEY:       stack_depth -= 1; break;
            case Opcode::LTY:       stack_depth -= 1; break;
            case Opcode::GTY:       stack_depth -= 1; break;
            case Opcode::LEY:       stack_depth -= 1; break;
            case Opcode::GEY:       stack_depth -= 1; break;
            case Opcode::EQA:       stack_depth -= 1; break;
            case Opcode::NEA:       stack_depth -= 1; break;
            case Opcode::EQD:       stack_depth -= 1; break;
            case Opcode::NED:       stack_depth -= 1; break;
            case Opcode::EQP:       stack_depth -= 1; break;
            case Opcode::NEP:       stack_depth -= 1; break;
            case Opcode::EQV:       stack_depth -= 1; break;
            case Opcode::NEV:       stack_depth -= 1; break;
            case Opcode::ANDB:      stack_depth -= 1; break;
            case Opcode::ORB:       stack_depth -= 1; break;
            case Opcode::NOTB:      break;
            case Opcode::INDEXAR:   stack_depth -= 1; break;
            case Opcode::INDEXAW:   stack_depth -= 1; break;
            case Opcode::INDEXAV:   stack_depth -= 1; break;
            case Opcode::INDEXAN:   stack_depth -= 1; break;
            case Opcode::INDEXDR:   stack_depth -= 1; break;
            case Opcode::INDEXDW:   stack_depth -= 1; break;
            case Opcode::INDEXDV:   stack_depth -= 1; break;
            case Opcode::INA:       stack_depth -= 1; break;
            case Opcode::IND:       stack_depth -= 1; break;
            case Opcode::CALLP:     break;
            case Opcode::CALLF:     break;
            case Opcode::CALLMF:    break;
            case Opcode::CALLI:     break;
            case Opcode::JUMP:      break;
            case Opcode::JF:        stack_depth -= 1; break;
            case Opcode::JT:        stack_depth -= 1; break;
            case Opcode::DUP:       stack_depth += 1; break;
            case Opcode::DUPX1:     stack_depth += 1; break;
            case Opcode::DROP:      stack_depth -= 1; break;
            case Opcode::RET:       break;
            case Opcode::CONSA:     break;
            case Opcode::CONSD:     break;
            case Opcode::EXCEPT:    stack_depth = -1; break;
            case Opcode::ALLOC:     stack_depth += 1; break;
            case Opcode::PUSHNIL:   stack_depth += 1; break;
            case Opcode::RESETC:    stack_depth -= 1; break;
            case Opcode::PUSHPEG:   stack_depth += 1; break;
            case Opcode::JUMPTBL:   stack_depth -= 1; in_jumptbl = true; break;
            case Opcode::CALLX:     break;
            case Opcode::SWAP:      break;
            case Opcode::DROPN:     break;
            case Opcode::PUSHFP:    stack_depth += 1; break;
            case Opcode::CALLV:     break;
            case Opcode::PUSHCI:    stack_depth += 1; break;
        }
    }
}

void Emitter::emit_uint32(uint32_t value)
{
    Bytecode::put_vint(object.code, value);
}

void Emitter::emit(Opcode b, uint32_t value)
{
    emit(b);
    emit_uint32(value);
    switch (b) {
        case Opcode::CONSA:     stack_depth -= value - 1; break;
        case Opcode::CONSD:     stack_depth -= 2*value - 1; break;
        case Opcode::DROPN:     stack_depth -= value - 1; break;
        default:                break;
    }
}

void Emitter::emit(Opcode b, uint32_t value, uint32_t value2)
{
    emit(b);
    emit_uint32(value);
    emit_uint32(value2);
}

void Emitter::emit(Opcode b, uint32_t value, uint32_t value2, uint32_t value3)
{
    emit(b);
    emit_uint32(value);
    emit_uint32(value2);
    emit_uint32(value3);
}

void Emitter::emit(Opcode b, const Number &value)
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
        object.functions.push_back(Bytecode::FunctionInfo(str(f.name), f.nest, f.params, f.locals, f.entry_label.get_target()));
    }
    for (auto c: classes) {
        Bytecode::ClassInfo ci;
        ci.name = str(c.first->name);
        for (auto i: c.second) {
            std::vector<unsigned int> a;
            for (auto m: i) {
                a.push_back(m);
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
    functions.push_back(FunctionInfo(name, create_label()));
    return static_cast<unsigned int>(i);
}

Emitter::FunctionInfo &Emitter::function_info(int index)
{
    return functions.at(index);
}

Emitter::Label Emitter::create_label()
{
    return Label();
}

void Emitter::emit_jump(Opcode b, Label &label)
{
    emit(b);
    label.reachable = true;
    if (label.target != UINT_MAX) {
        emit_uint32(label.target);
    } else {
        label.fixups.push_back(static_cast<unsigned int>(object.code.size()));
        // Leave room enough for 32 bit values.
        emit_byte(0);
        emit_byte(0);
        emit_byte(0);
        emit_byte(0);
        emit_byte(0);
    }
    // Do not attempt to adjust the stack depth when calling an in-module function.
    if (b != Opcode::CALLF && b != Opcode::PUSHI) {
        if (stack_depth >= 0) {
            if (label.target_stack_depth < 0) {
                label.target_stack_depth = stack_depth;
            }
            if (label.target_stack_depth != stack_depth) {
                internal_error("emit_jump: target_stack_depth mismatch, ip=" + std::to_string(object.code.size()) + ", expected=" + std::to_string(label.target_stack_depth) + ", current=" + std::to_string(stack_depth));
            }
        }
    }
    if ((b == Opcode::JUMP && not in_jumptbl) || b == Opcode::RET || b == Opcode::EXCEPT) {
        stack_depth = -1;
    }
}

void Emitter::jump_target(Label &label)
{
    assert(label.target == UINT_MAX);
    label.target = static_cast<unsigned int>(object.code.size());
    for (auto offset: label.fixups) {
        std::vector<unsigned char> target;
        Bytecode::put_vint(target, label.target, 5);
        assert(target.size() == 5); // Matches the amount reserved in emit_jump.
        std::copy(target.begin(), target.end(), object.code.begin() + offset);
    }
    if (label.target_stack_depth < 0) {
        label.target_stack_depth = stack_depth;
    } else {
        stack_depth = label.target_stack_depth;
    }
    if (label.target_stack_depth != stack_depth) {
        internal_error("jump_target: target_stack_depth mismatch, ip=" + std::to_string(object.code.size()) + ", expected=" + std::to_string(label.target_stack_depth) + ", current=" + std::to_string(stack_depth));
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

size_t Emitter::get_function_depth() const
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

void Emitter::add_export_function(const std::string &name, const std::string &type, int index)
{
    Bytecode::Function function;
    function.name = str(name);
    function.descriptor = str(type);
    function.index = index;
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

void Emitter::add_import(const ast::Module *module)
{
    unsigned int index = str(module->name);
    for (auto i: object.imports) {
        if (i.name == index) {
            return;
        }
    }
    Bytecode::ModuleImport imp;
    imp.name = index;
    imp.optional = module->optional;
    imp.hash = std::string(32, '0');
    object.imports.push_back(imp);
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
    // Avoid unbounded recursion.
    if (predeclared) {
        return;
    }
    predeclared = true;
    for (auto m: methods) {
        m.second->predeclare(emitter);
    }
}

void ast::Type::postdeclare(Emitter &emitter) const
{
    // Avoid unbounded recursion.
    if (postdeclared) {
        return;
    }
    postdeclared = true;
    for (auto m: methods) {
        m.second->postdeclare(emitter);
    }
}

void ast::Type::generate_export(Emitter &emitter, const std::string &export_name) const
{
    emitter.add_export_type(export_name, get_type_descriptor(emitter));
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
        emitter.add_export_function(export_name + "." + m.first, f->type->get_type_descriptor(emitter), f->function_index);
    }
}

void ast::TypeBoolean::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADB);
}

void ast::TypeBoolean::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREB);
}

void ast::TypeBoolean::generate_call(Emitter &) const
{
    internal_error("TypeBoolean");
}

void ast::TypeNumber::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADN);
}

void ast::TypeNumber::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREN);
}

void ast::TypeNumber::generate_call(Emitter &) const
{
    internal_error("TypeNumber");
}

void ast::TypeString::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADS);
}

void ast::TypeString::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STORES);
}

void ast::TypeString::generate_call(Emitter &) const
{
    internal_error("TypeString");
}

void ast::TypeBytes::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADY);
}

void ast::TypeBytes::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREY);
}

void ast::TypeBytes::generate_call(Emitter &) const
{
    internal_error("TypeBytes");
}

void ast::TypeObject::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADJ);
}

void ast::TypeObject::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREJ);
}

void ast::TypeObject::generate_call(Emitter &) const
{
    internal_error("TypeObject");
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
            case ParameterType::Mode::IN:    m = '>'; break;
            case ParameterType::Mode::INOUT: m = '*'; break;
            case ParameterType::Mode::OUT:   m = '<'; break;
            default:
                internal_error("invalid parameter mode");
        }
        r += m + p->declaration.text + ":" + emitter.get_type_reference(p->type);
        if (p->default_value != nullptr && dynamic_cast<const ast::DummyExpression *>(p->default_value) == nullptr) {
            r += "=" + p->type->serialize(p->default_value);
        }
    }
    r += "]";
    if (variadic) {
        r += "V";
    }
    r += ":" + emitter.get_type_reference(returntype);
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
    emitter.emit(Opcode::LOADA);
}

void ast::TypeArray::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREA);
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
    emitter.emit(Opcode::LOADD);
}

void ast::TypeDictionary::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STORED);
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
    const bool was_predeclared = predeclared;
    Type::predeclare(emitter);
    if (was_predeclared) {
        return;
    }
    for (auto f: fields) {
        f.type->predeclare(emitter);
    }
}

void ast::TypeRecord::postdeclare(Emitter &emitter) const
{
    // Avoid unbounded recursion.
    const bool was_postdeclared = postdeclared;
    Type::postdeclare(emitter);
    if (was_postdeclared) {
        return;
    }
}

void ast::TypeRecord::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADA);
}

void ast::TypeRecord::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREA);
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
    if (reftype != nullptr) {
        emitter.emit(Opcode::LOADP);
    } else {
        emitter.emit(Opcode::LOADV);
    }
}

void ast::TypePointer::generate_store(Emitter &emitter) const
{
    if (reftype != nullptr) {
        emitter.emit(Opcode::STOREP);
    } else {
        emitter.emit(Opcode::STOREV);
    }
}

void ast::TypePointer::generate_call(Emitter &) const
{
    internal_error("TypePointer");
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
    emitter.emit(Opcode::LOADA);
}

void ast::TypeInterfacePointer::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREA);
}

void ast::TypeInterfacePointer::generate_call(Emitter &) const
{
    internal_error("TypeInterfacePointer");
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
    emitter.emit(Opcode::LOADA);
}

void ast::TypeFunctionPointer::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREA);
}

void ast::TypeFunctionPointer::generate_call(Emitter &emitter) const
{
    emitter.emit(Opcode::CALLI);
    emitter.adjust_stack_depth(functype->get_stack_delta() - 1);
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

void ast::TypeEnum::generate_load(Emitter &emitter) const
{
    emitter.emit(Opcode::LOADN);
}

void ast::TypeEnum::generate_store(Emitter &emitter) const
{
    emitter.emit(Opcode::STOREN);
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
    emitter.emit(Opcode::PUSHPPG, emitter.str(name));
}

void ast::ModuleVariable::predeclare(Emitter &emitter) const
{
    emitter.add_import(module);
}

void ast::ModuleVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(Opcode::PUSHPMG, emitter.str(module->name), emitter.str(name));
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
    emitter.emit(Opcode::PUSHPG, index);
}

void ast::GlobalVariable::generate_export(Emitter &emitter, const std::string &export_name) const
{
    emitter.add_export_variable(export_name, emitter.get_type_reference(type), index);
}

void ast::ExternalGlobalVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(Opcode::PUSHPEG, emitter.str(name));
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
        emitter.emit(Opcode::PUSHPOL, static_cast<uint32_t>(emitter.get_function_depth() - nesting_depth), index);
    } else {
        emitter.emit(Opcode::PUSHPL, index);
    }
}

void ast::FunctionParameter::generate_address(Emitter &emitter) const
{
    if (index < 0) {
        internal_error("invalid local index: " + name);
    }
    assert(emitter.get_function_depth() >= nesting_depth);
    switch (mode) {
        case ParameterType::Mode::IN:
        case ParameterType::Mode::OUT:
            if (emitter.get_function_depth() > nesting_depth) {
                emitter.emit(Opcode::PUSHPOL, static_cast<uint32_t>(emitter.get_function_depth() - nesting_depth), index);
            } else {
                emitter.emit(Opcode::PUSHPL, index);
            }
            break;
        case ParameterType::Mode::INOUT:
            if (emitter.get_function_depth() > nesting_depth) {
                emitter.emit(Opcode::PUSHPOL, static_cast<uint32_t>(emitter.get_function_depth() - nesting_depth), index);
            } else {
                emitter.emit(Opcode::PUSHPL, index);
            }
            emitter.emit(Opcode::LOADP);
            break;
    }
}

void ast::BaseFunction::predeclare(Emitter &emitter) const
{
    type->predeclare(emitter);
    // TODO: This hack ensures that we only allocate the
    // entry label once, even if predeclare() is called
    // more than once.
    if (function_index == UINT_MAX) {
        function_index = emitter.next_function(name);
    }
}

void ast::BaseFunction::generate_export(Emitter &emitter, const std::string &export_name) const
{
    emitter.add_export_function(export_name, type->get_type_descriptor(emitter), function_index);
}

void ast::Function::predeclare(Emitter &emitter) const
{
    BaseFunction::predeclare(emitter);
    frame->predeclare(emitter);
}

static int count_in_parameters(const std::vector<const ast::ParameterType *> &params)
{
    int r = 0;
    for (auto p: params) {
        if (p->mode == ast::ParameterType::Mode::IN || p->mode == ast::ParameterType::Mode::INOUT) {
            r++;
        }
    }
    return r;
}

static int count_out_parameters(const std::vector<const ast::ParameterType *> &params)
{
    int r = 0;
    for (auto p: params) {
        if (p->mode == ast::ParameterType::Mode::OUT) {
            r++;
        }
    }
    return r;
}

void ast::Function::postdeclare(Emitter &emitter) const
{
    emitter.debug_line(declaration.line);
    emitter.jump_target(emitter.function_info(function_index).entry_label);
    emitter.set_current_function_depth(nesting_depth);
    emitter.set_stack_depth(count_in_parameters(ftype->params));
    emitter.function_info(function_index).nest = static_cast<int>(nesting_depth);
    emitter.function_info(function_index).params = count_in_parameters(ftype->params);
    emitter.function_info(function_index).locals = static_cast<int>(frame->getCount());
    for (auto p = params.rbegin(); p != params.rend(); ++p) {
        switch ((*p)->mode) {
            case ParameterType::Mode::IN:
                (*p)->generate_address(emitter);
                (*p)->generate_store(emitter);
                break;
            case ParameterType::Mode::INOUT:
                emitter.emit(Opcode::PUSHPL, (*p)->index);
                emitter.emit(Opcode::STOREP);
                break;
            case ParameterType::Mode::OUT:
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
            case ParameterType::Mode::IN:
            case ParameterType::Mode::INOUT:
                break;
            case ParameterType::Mode::OUT:
                (*p)->generate_address(emitter);
                (*p)->generate_load(emitter);
                break;
        }
    }
    emitter.emit(Opcode::RET);
    if (exit.is_reachable() && emitter.get_stack_depth() >= 0) {
        if (dynamic_cast<const TypeFunction *>(type)->returntype != TYPE_NOTHING) {
            emitter.adjust_stack_depth(-1);
        }
        emitter.adjust_stack_depth(-count_out_parameters(ftype->params));
        if (emitter.get_stack_depth() != 0) {
            internal_error("ip=" + std::to_string(emitter.current_ip()) + " function end stack_depth " + std::to_string(emitter.get_stack_depth()));
        }
    }
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
                            c.second[i][m] = function_index;
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
    emitter.emit(Opcode::PUSHFP, function_index);
}

void ast::Function::generate_call(Emitter &emitter) const
{
    emitter.emit(Opcode::CALLF, function_index);
    emitter.adjust_stack_depth(ftype->get_stack_delta());
}

void ast::PredefinedFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(Opcode::CALLP, emitter.str(name));
    emitter.adjust_stack_depth(ftype->get_stack_delta());
}

void ast::ExtensionFunction::postdeclare(Emitter &emitter) const
{
    emitter.jump_target(emitter.function_info(function_index).entry_label);
    emitter.set_stack_depth(count_in_parameters(ftype->params));
    generate_call(emitter);
    emitter.emit(Opcode::RET);
}

void ast::ExtensionFunction::generate_call(Emitter &emitter) const
{
    int in_param_count = 0;
    int out_param_count = 0;
    for (auto p: ftype->params) {
        switch (p->mode) {
            case ParameterType::Mode::IN:
                in_param_count++;
                break;
            case ParameterType::Mode::INOUT:
                in_param_count++;
                out_param_count++;
                break;
            case ParameterType::Mode::OUT:
                out_param_count++;
                break;
        }
    }
    emitter.emit(Opcode::CONSA, static_cast<uint32_t>(in_param_count));
    emitter.emit(Opcode::CALLX, emitter.str(module), emitter.str(name), out_param_count);
    emitter.adjust_stack_depth(out_param_count);
    if (dynamic_cast<const TypeFunction *>(type)->returntype == TYPE_NOTHING) {
        emitter.emit(Opcode::DROPN, out_param_count);
    }
}

void ast::ModuleFunction::predeclare(Emitter &emitter) const
{
    emitter.add_import(module);
}

void ast::ModuleFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(Opcode::CALLMF, emitter.str(module->name), emitter.str(name + "," + descriptor));
    emitter.adjust_stack_depth(ftype->get_stack_delta());
}

void ast::Exception::generate_export(Emitter &emitter, const std::string &export_name) const
{
    emitter.add_export_exception(export_name);
    for (auto s: subexceptions) {
        s.second->generate_export(emitter, export_name + "." + s.first);
    }
}

void ast::Interface::generate_export(Emitter &emitter, const std::string &export_name) const
{
    std::vector<std::pair<std::string, std::string>> method_descriptors;
    for (auto m: methods) {
        method_descriptors.push_back(std::make_pair(m.first.text, m.second->get_type_descriptor(emitter)));
    }
    emitter.add_export_interface(export_name, method_descriptors);
}

void ast::Constant::generate_export(Emitter &emitter, const std::string &export_name) const
{
    emitter.add_export_constant(export_name, emitter.get_type_reference(type), type->serialize(value));
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
    emitter.emit(Opcode::PUSHB);
    emitter.emit_byte(value ? 1 : 0);
}

void ast::ConstantNumberExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(Opcode::PUSHN, value);
}

void ast::ConstantStringExpression::generate_expr(Emitter &emitter) const
{
    unsigned int index = emitter.str(value);
    emitter.emit(Opcode::PUSHS, index);
}

void ast::ConstantBytesExpression::generate_expr(Emitter &emitter) const
{
    unsigned int index = emitter.str(contents);
    emitter.emit(Opcode::PUSHY, index);
}

void ast::ConstantEnumExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(Opcode::PUSHN, number_from_uint32(value));
}

void ast::ConstantNilExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(Opcode::PUSHNIL);
}

void ast::ConstantNowhereExpression::generate_expr(Emitter &emitter) const
{
    emitter.emit(Opcode::PUSHFP, 0);
}

void ast::ConstantNilObject::generate_expr(Emitter &emitter) const
{
    emitter.emit(Opcode::CALLP, emitter.str("object__makeNull"));
    emitter.adjust_stack_depth(1);
}

void ast::TypeConversionExpression::generate_expr(Emitter &emitter) const
{
    expr->generate(emitter);
}

void ast::ArrayLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto e = elements.rbegin(); e != elements.rend(); ++e) {
        (*e)->generate(emitter);
    }
    emitter.emit(Opcode::CONSA, static_cast<uint32_t>(elements.size()));
}

void ast::DictionaryLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (auto d = dict.rbegin(); d != dict.rend(); ++d) {
        emitter.emit(Opcode::PUSHS, emitter.str(d->first));
        d->second->generate(emitter);
    }
    emitter.emit(Opcode::CONSD, static_cast<uint32_t>(dict.size()));
}

void ast::RecordLiteralExpression::generate_expr(Emitter &emitter) const
{
    auto f = dynamic_cast<const ast::TypeRecord *>(type)->fields.rbegin();
    for (auto v = values.rbegin(); v != values.rend(); ++v) {
        assert(f != dynamic_cast<const ast::TypeRecord *>(type)->fields.rend());
        (*v)->generate(emitter);
        f++;
    }
    emitter.emit(Opcode::CONSA, static_cast<uint32_t>(values.size()));
}

void ast::ClassLiteralExpression::generate_expr(Emitter &emitter) const
{
    for (size_t i = values.size()-1; i > 0; i--) {
        values[i]->generate(emitter);
    }
    emitter.emit(Opcode::PUSHCI, emitter.str(type->name));
    emitter.emit(Opcode::CONSA, static_cast<uint32_t>(values.size()));
}

void ast::NewClassExpression::generate_expr(Emitter &emitter) const
{
    if (value != nullptr) {
        value->generate(emitter);
    }
    emitter.emit(Opcode::ALLOC, static_cast<uint32_t>(reftype->fields.size()));
    if (value != nullptr) {
        emitter.emit(Opcode::DUPX1);
        emitter.emit(Opcode::STOREA);
    }
}

void ast::UnaryMinusExpression::generate_expr(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(Opcode::NEGN);
}

void ast::LogicalNotExpression::generate_expr(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(Opcode::NOTB);
}

void ast::ConditionalExpression::generate_expr(Emitter &emitter) const
{
    condition->generate(emitter);
    auto else_label = emitter.create_label();
    emitter.emit_jump(Opcode::JF, else_label);
    left->generate(emitter);
    auto end_label = emitter.create_label();
    emitter.emit_jump(Opcode::JUMP, end_label);
    emitter.jump_target(else_label);
    right->generate(emitter);
    emitter.jump_target(end_label);
}

void ast::TryExpression::generate_expr(Emitter &emitter) const
{
    int start_stack_depth = emitter.get_stack_depth();
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    expr->generate(emitter);
    auto skip = emitter.create_label();
    emitter.emit_jump(Opcode::JUMP, skip);
    ei.end = emitter.current_ip();
    for (auto &c: catches) {
        // Exception handling pushes the exception info onto the stack here.
        emitter.set_stack_depth(start_stack_depth + 1);
        for (auto e: c.exceptions) {
            ei.excid = emitter.str(e->name);
            ei.handler = emitter.current_ip();
            ei.stack_depth = start_stack_depth;
            emitter.add_exception(ei);
        }
        if (c.name != nullptr) {
            c.name->generate_address(emitter);
            c.name->generate_store(emitter);
        } else {
            emitter.emit(Opcode::DROP);
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
        emitter.emit_jump(Opcode::JUMP, skip);
    }
    emitter.jump_target(skip);
}

void ast::DisjunctionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(Opcode::DUP);
    auto true_label = emitter.create_label();
    emitter.emit_jump(Opcode::JT, true_label);
    emitter.emit(Opcode::DROP);
    right->generate(emitter);
    emitter.jump_target(true_label);
}

void ast::ConjunctionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(Opcode::DUP);
    auto false_label = emitter.create_label();
    emitter.emit_jump(Opcode::JF, false_label);
    emitter.emit(Opcode::DROP);
    right->generate(emitter);
    emitter.jump_target(false_label);
}

void ast::TypeTestExpression::generate_expr(Emitter &emitter) const
{
    assert(expr_before_conversion->type == TYPE_OBJECT);
    if (expr_after_conversion == nullptr) {
        emitter.emit(Opcode::PUSHB, 0);
        return;
    }
    int start_stack_depth = emitter.get_stack_depth();
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    expr_after_conversion->generate(emitter);
    emitter.emit(Opcode::DROP);
    emitter.emit(Opcode::PUSHB, 1);
    auto skip = emitter.create_label();
    emitter.emit_jump(Opcode::JUMP, skip);
    ei.end = emitter.current_ip();
    ei.excid = emitter.str("DynamicConversionException");
    ei.handler = emitter.current_ip();
    ei.stack_depth = start_stack_depth;
    emitter.add_exception(ei);
    emitter.set_stack_depth(start_stack_depth + 1);
    emitter.emit(Opcode::DROP);
    emitter.emit(Opcode::PUSHB, 0);
    emitter.jump_target(skip);
}

void ast::ArrayInExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::INA);
}

void ast::DictionaryInExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::IND);
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
    auto false_label = emitter.create_label();
    auto end_label = emitter.create_label();
    size_t i = 0;
    for (auto c: comps) {
        bool last = i == comps.size() - 1;
        c->right->generate(emitter);
        if (not last) {
            emitter.emit(Opcode::DUPX1);
        }
        c->generate_comparison_opcode(emitter);
        if (last) {
            emitter.emit_jump(Opcode::JUMP, end_label);
        } else {
            emitter.emit_jump(Opcode::JF, false_label);
        }
        i++;
    }
    emitter.jump_target(false_label);
    emitter.emit(Opcode::DROP);
    emitter.emit(Opcode::PUSHB, 0);
    emitter.jump_target(end_label);
}

void ast::BooleanComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQB); break;
        case Comparison::NE: emitter.emit(Opcode::NEB); break;
        default:
            internal_error("unexpected comparison type");
    }
}

void ast::NumericComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQN); break;
        case Comparison::NE: emitter.emit(Opcode::NEN); break;
        case Comparison::LT: emitter.emit(Opcode::LTN); break;
        case Comparison::GT: emitter.emit(Opcode::GTN); break;
        case Comparison::LE: emitter.emit(Opcode::LEN); break;
        case Comparison::GE: emitter.emit(Opcode::GEN); break;
    }
}

void ast::EnumComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQN); break;
        case Comparison::NE: emitter.emit(Opcode::NEN); break;
        case Comparison::LT: emitter.emit(Opcode::LTN); break;
        case Comparison::GT: emitter.emit(Opcode::GTN); break;
        case Comparison::LE: emitter.emit(Opcode::LEN); break;
        case Comparison::GE: emitter.emit(Opcode::GEN); break;
    }
}

void ast::StringComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQS); break;
        case Comparison::NE: emitter.emit(Opcode::NES); break;
        case Comparison::LT: emitter.emit(Opcode::LTS); break;
        case Comparison::GT: emitter.emit(Opcode::GTS); break;
        case Comparison::LE: emitter.emit(Opcode::LES); break;
        case Comparison::GE: emitter.emit(Opcode::GES); break;
    }
}

void ast::BytesComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQY); break;
        case Comparison::NE: emitter.emit(Opcode::NEY); break;
        case Comparison::LT: emitter.emit(Opcode::LTY); break;
        case Comparison::GT: emitter.emit(Opcode::GTY); break;
        case Comparison::LE: emitter.emit(Opcode::LEY); break;
        case Comparison::GE: emitter.emit(Opcode::GEY); break;
    }
}

void ast::ArrayComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQA); break;
        case Comparison::NE: emitter.emit(Opcode::NEA); break;
        default:
            internal_error("unexpected comparison type");
    }
}

void ast::RecordComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQA); break;
        case Comparison::NE: emitter.emit(Opcode::NEA); break;
        default:
            internal_error("unexpected comparison type");
    }
}

void ast::DictionaryComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQD); break;
        case Comparison::NE: emitter.emit(Opcode::NED); break;
        default:
            internal_error("unexpected comparison type");
    }
}

void ast::PointerComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    const ast::TypePointer *tp1 = dynamic_cast<const ast::TypePointer *>(left->type);
    const ast::TypePointer *tp2 = dynamic_cast<const ast::TypePointer *>(right->type);
    if (tp1->reftype != nullptr || tp2->reftype != nullptr) {
        switch (comp) {
            case Comparison::EQ: emitter.emit(Opcode::EQP); break;
            case Comparison::NE: emitter.emit(Opcode::NEP); break;
            default:
                internal_error("unexpected comparison type");
        }
    } else {
        switch (comp) {
            case Comparison::EQ: emitter.emit(Opcode::EQV); break;
            case Comparison::NE: emitter.emit(Opcode::NEV); break;
            default:
                internal_error("unexpected comparison type");
        }
    }
}

void ast::FunctionPointerComparisonExpression::generate_comparison_opcode(Emitter &emitter) const
{
    switch (comp) {
        case Comparison::EQ: emitter.emit(Opcode::EQA); break;
        case Comparison::NE: emitter.emit(Opcode::NEA); break;
        default:
            internal_error("unexpected comparison type");
    }
}

void ast::ValidPointerExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(Opcode::DUP);
    var->generate_address(emitter);
    var->generate_store(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::NEP);
}

void ast::AdditionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::ADDN);
}

void ast::SubtractionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::SUBN);
}

void ast::MultiplicationExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::MULN);
}

void ast::DivisionExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::DIVN);
}

void ast::ModuloExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::MODN);
}

void ast::ExponentiationExpression::generate_expr(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(Opcode::EXPN);
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
    emitter.emit(Opcode::DROP);
}

void ast::ArrayReferenceIndexExpression::generate_address_read(Emitter &emitter) const
{
    array->generate_address_read(emitter);
    index->generate(emitter);
    if (always_create) {
        emitter.emit(Opcode::INDEXAW);
    } else {
        emitter.emit(Opcode::INDEXAR);
    }
}

void ast::ArrayReferenceIndexExpression::generate_address_write(Emitter &emitter) const
{
    array->generate_address_write(emitter);
    index->generate(emitter);
    emitter.emit(Opcode::INDEXAW);
}

void ast::ArrayValueIndexExpression::generate_expr(Emitter &emitter) const
{
    array->generate(emitter);
    index->generate(emitter);
    if (always_create) {
        emitter.emit(Opcode::INDEXAN);
    } else {
        emitter.emit(Opcode::INDEXAV);
    }
}

void ast::DictionaryReferenceIndexExpression::generate_address_read(Emitter &emitter) const
{
    dictionary->generate_address_read(emitter);
    index->generate(emitter);
    emitter.emit(Opcode::INDEXDR);
}

void ast::DictionaryReferenceIndexExpression::generate_address_write(Emitter &emitter) const
{
    dictionary->generate_address_write(emitter);
    index->generate(emitter);
    emitter.emit(Opcode::INDEXDW);
}

void ast::DictionaryValueIndexExpression::generate_expr(Emitter &emitter) const
{
    dictionary->generate(emitter);
    index->generate(emitter);
    emitter.emit(Opcode::INDEXDV);
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

void ast::StringReferenceRangeIndexExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::StringReferenceRangeIndexExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void ast::StringValueRangeIndexExpression::generate_expr(Emitter &emitter) const
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

void ast::BytesReferenceRangeIndexExpression::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::BytesReferenceRangeIndexExpression::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    ref->generate_store(emitter);
}

void ast::BytesValueRangeIndexExpression::generate_expr(Emitter &emitter) const
{
    load->generate(emitter);
}

void ast::ObjectSubscriptExpression::generate_expr(Emitter &emitter) const
{
    obj->generate(emitter);
    index->generate(emitter);
    emitter.emit(Opcode::CALLP, emitter.str("object__subscript"));
    emitter.adjust_stack_depth(-1);
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
    emitter.emit(Opcode::PUSHN, number_from_uint32(static_cast<uint32_t>(f->second)));
    if (always_create) {
        emitter.emit(Opcode::INDEXAW);
    } else {
        emitter.emit(Opcode::INDEXAR);
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
    emitter.emit(Opcode::PUSHN, number_from_uint32(static_cast<uint32_t>(f->second)));
    emitter.emit(Opcode::INDEXAW);
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
    emitter.emit(Opcode::PUSHN, number_from_uint32(static_cast<uint32_t>(f->second)));
    if (always_create) {
        emitter.emit(Opcode::INDEXAN);
    } else {
        emitter.emit(Opcode::INDEXAV);
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
    emitter.emit(Opcode::CALLV, static_cast<uint32_t>(index));
    emitter.adjust_stack_depth(functype->get_stack_delta() - 1);
}

void ast::InterfacePointerConstructor::generate_expr(Emitter &emitter) const
{
    expr->generate(emitter);
    emitter.emit(Opcode::PUSHI, static_cast<uint32_t>(index));
    emitter.emit(Opcode::SWAP);
    emitter.emit(Opcode::CONSA, 2);
}

void ast::InterfacePointerDeconstructor::generate_expr(Emitter &emitter) const
{
    expr->generate(emitter);
    emitter.emit(Opcode::PUSHI, 0);
    emitter.emit(Opcode::INDEXAV);
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
    // StringRangeIndexExpression::store is not really legal. This assertion
    // holds true for any other function call.
    bool check = true;
    const VariableExpression *ve = dynamic_cast<const VariableExpression *>(func);
    if (ve != nullptr) {
        const PredefinedFunction *pf = dynamic_cast<const PredefinedFunction *>(ve->var);
        if (pf != nullptr) {
            if (pf->name == "string__splice"
             || pf->name == "bytes__splice"
             || pf->name == "array__splice") {
                check = false;
            }
        }
    }
    if (check) {
        assert(args.size() == ftype->params.size());
    }
    for (size_t i = 0; i < args.size(); i++) {
        auto param = ftype->params[i];
        auto arg = args[i];
        switch (param->mode) {
            case ParameterType::Mode::IN:
                arg->generate(emitter);
                break;
            case ParameterType::Mode::INOUT:
                dynamic_cast<const ReferenceExpression *>(arg)->generate_address_read(emitter);
                break;
            case ParameterType::Mode::OUT:
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
            case ParameterType::Mode::IN:
            case ParameterType::Mode::INOUT:
                break;
            case ParameterType::Mode::OUT:
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
        if (ftype->params[i]->mode != ParameterType::Mode::IN) {
            return false;
        }
    }
    return true;
}

void ast::StatementExpression::generate_expr(Emitter &emitter) const
{
    stmt->generate(emitter);
    if (expr != nullptr) {
        expr->generate(emitter);
    }
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
        std::vector<std::vector<int>> ifs;
        for (auto i: type_class->interfaces) {
            ifs.push_back(std::vector<int>(i->methods.size()));
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
    emitter.emit(Opcode::CALLP, emitter.str("runtime$assertionsEnabled"));
    emitter.adjust_stack_depth(1);
    emitter.emit_jump(Opcode::JF, skip_label);
    expr->generate(emitter);
    emitter.emit_jump(Opcode::JT, skip_label);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit(Opcode::PUSHS, emitter.str(source));
    emitter.emit(Opcode::CALLP, emitter.str("object__makeString"));
    emitter.emit(Opcode::EXCEPT, emitter.str("AssertFailedException"));
    emitter.jump_target(skip_label);
}

void ast::AssignmentStatement::generate_code(Emitter &emitter) const
{
    assert(variables.size() > 0);
    expr->generate(emitter);
    for (size_t i = 0; i < variables.size() - 1; i++) {
        emitter.emit(Opcode::DUP);
    }
    for (auto v: variables) {
         v->generate_store(emitter);
    }
}

void ast::ExpressionStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
}

void ast::IncrementStatement::generate_code(Emitter &emitter) const
{
    ref->generate_address_write(emitter);
    emitter.emit(Opcode::DUP);
    emitter.emit(Opcode::LOADN);
    emitter.emit(Opcode::PUSHN, number_from_sint32(delta));
    emitter.emit(Opcode::ADDN);
    emitter.emit(Opcode::SWAP);
    emitter.emit(Opcode::STOREN);
}

void ast::IfStatement::generate_code(Emitter &emitter) const
{
    auto end_label = emitter.create_label();
    for (auto cs: condition_statements) {
        const Expression *condition = cs.first;
        const std::vector<const Statement *> &statements = cs.second;
        condition->generate(emitter);
        auto else_label = emitter.create_label();
        emitter.emit_jump(Opcode::JF, else_label);
        for (auto stmt: statements) {
            stmt->generate(emitter);
        }
        emitter.emit_jump(Opcode::JUMP, end_label);
        emitter.jump_target(else_label);
    }
    for (auto stmt: else_statements) {
        stmt->generate(emitter);
    }
    emitter.jump_target(end_label);
}

void ast::ReturnStatement::generate_code(Emitter &emitter) const
{
#if 0 // It doesn't look like tail calls will be possible without separate ENTER/LEAVE opcodes.
    // Check for a possible tail call. We can only do this if all
    // of the following conditions hold:
    //  - The RETURN statement returns only a call to another function
    //  - The function takes only IN parameters (it might also be possible
    //    to do this with some kinds of INOUT parameters, but not in general)
    //  - The function is an ordinary function that we can jump to (not a
    //    predefined function)
    //  - The nesting level of the next function is at the same level or
    //    further out than this function.
    // If possible, then generate the parameters for the next function,
    // emit a LEAVE instruction to discard our frame, and then jump to
    // the entry point for the next function.
    //
#if 0
    // TODO: This currently messes up the stack_depth tracking and is
    // therefore disabled. The tail-call.neon test file is also skipped.
    const FunctionCall *call = dynamic_cast<const FunctionCall *>(expr);
    if (call != nullptr) {
        if (call->all_in_parameters()) {
            const VariableExpression *var = dynamic_cast<const VariableExpression *>(call->func);
            if (var != nullptr) {
                const Function *func = dynamic_cast<const Function *>(var->var);
                if (func != nullptr) {
                    if (func->nesting_depth <= emitter.get_function_depth()) {
                        call->generate_parameters(emitter);
                        emitter.emit(Opcode::LEAVE);
                        emitter.emit_jump(Opcode::JUMP, emitter.function_info(func->function_index).entry_label);
                        return;
                    }
                }
            }
        }
    }
#endif
#endif

    if (expr != nullptr) {
        expr->generate(emitter);
    }
    emitter.emit_jump(Opcode::JUMP, emitter.get_function_exit());
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
                if (comp != nullptr && comp->comp == ComparisonExpression::Comparison::EQ) {
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
                    emitter.emit(Opcode::PUSHN, number_from_sint32(min));
                    emitter.emit(Opcode::SUBN);
                }
                emitter.emit(Opcode::JUMPTBL, range);
                std::map<const std::vector<const Statement *> *, Emitter::Label> labels;
                auto others_label = emitter.create_label();
                for (int32_t i = min; i <= max; i++) {
                    auto v = values.find(i);
                    if (v != values.end()) {
                        auto label = labels.find(v->second);
                        if (label == labels.end()) {
                            labels[v->second] = emitter.create_label();
                        }
                        emitter.emit_jump(Opcode::JUMP, labels[v->second]);
                    } else {
                        emitter.emit_jump(Opcode::JUMP, others_label);
                    }
                }
                emitter.emit_jump(Opcode::JUMP, others_label);
                for (auto &label: labels) {
                    emitter.jump_target(label.second);
                    for (auto stmt: *label.first) {
                        stmt->generate(emitter);
                    }
                    emitter.emit_jump(Opcode::JUMP, end_label);
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
                emitter.emit_jump(Opcode::JT, match_label);
            }
            emitter.emit_jump(Opcode::JUMP, next_label);
            emitter.jump_target(match_label);
        }
        emitter.emit(Opcode::DROP);
        for (auto stmt: statements) {
            stmt->generate(emitter);
        }
        emitter.emit_jump(Opcode::JUMP, end_label);
        emitter.jump_target(next_label);
    }
    emitter.emit(Opcode::DROP);
    emitter.jump_target(end_label);
}

void ast::CaseStatement::ComparisonWhenCondition::generate(Emitter &emitter) const
{
    static const Opcode opn[] = {Opcode::EQN, Opcode::NEN, Opcode::LTN, Opcode::GTN, Opcode::LEN, Opcode::GEN};
    static const Opcode ops[] = {Opcode::EQS, Opcode::NES, Opcode::LTS, Opcode::GTS, Opcode::LES, Opcode::GES};
    const Opcode *op;
    if (expr->type == TYPE_NUMBER || dynamic_cast<const TypeEnum *>(expr->type) != nullptr) {
        op = opn;
    } else if (expr->type == TYPE_STRING) {
        op = ops;
    } else {
        internal_error("when condition not number or string");
    }

    emitter.emit(Opcode::DUP);
    expr->generate(emitter);
    emitter.emit(op[static_cast<int>(comp)]); // Casting enum to int for index.
}

void ast::CaseStatement::RangeWhenCondition::generate(Emitter &emitter) const
{
    static const Opcode opn[] = {Opcode::EQN, Opcode::NEN, Opcode::LTN, Opcode::GTN, Opcode::LEN, Opcode::GEN};
    static const Opcode ops[] = {Opcode::EQS, Opcode::NES, Opcode::LTS, Opcode::GTS, Opcode::LES, Opcode::GES};
    const Opcode *op;
    if (low_expr->type == TYPE_NUMBER || dynamic_cast<const TypeEnum *>(low_expr->type) != nullptr) {
        op = opn;
    } else if (low_expr->type == TYPE_STRING) {
        op = ops;
    } else {
        internal_error("when condition not number or string");
    }

    emitter.emit(Opcode::DUP);
    auto result_label = emitter.create_label();
    low_expr->generate(emitter);
    emitter.emit(op[static_cast<int>(ComparisonExpression::Comparison::GE)]);
    emitter.emit(Opcode::DUP);
    emitter.emit_jump(Opcode::JF, result_label);
    emitter.emit(Opcode::DROP);
    emitter.emit(Opcode::DUP);
    high_expr->generate(emitter);
    emitter.emit(op[static_cast<int>(ComparisonExpression::Comparison::LE)]);
    emitter.jump_target(result_label);
}

void ast::CaseStatement::TypeTestWhenCondition::generate(Emitter &emitter) const
{
    int start_stack_depth = emitter.get_stack_depth();
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    expr->generate(emitter);
    emitter.emit(Opcode::DROP);
    emitter.emit(Opcode::PUSHB, 1);
    auto skip = emitter.create_label();
    emitter.emit_jump(Opcode::JUMP, skip);
    ei.end = emitter.current_ip();
    ei.excid = emitter.str("DynamicConversionException");
    ei.handler = emitter.current_ip();
    ei.stack_depth = start_stack_depth;
    emitter.add_exception(ei);
    emitter.set_stack_depth(start_stack_depth + 1);
    emitter.emit(Opcode::DROP);
    emitter.emit(Opcode::PUSHB, 0);
    emitter.jump_target(skip);
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
    emitter.emit_jump(Opcode::JUMP, top);
    emitter.jump_target(skip);
    emitter.remove_loop_labels(loop_id);
}

void ast::ExitStatement::generate_code(Emitter &emitter) const
{
    emitter.emit_jump(Opcode::JUMP, emitter.get_exit_label(loop_id));
}

void ast::NextStatement::generate_code(Emitter &emitter) const
{
    emitter.emit_jump(Opcode::JUMP, emitter.get_next_label(loop_id));
}

void ast::TryStatement::generate_code(Emitter &emitter) const
{
    int start_stack_depth = emitter.get_stack_depth();
    Bytecode::ExceptionInfo ei;
    ei.start = emitter.current_ip();
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    auto skip = emitter.create_label();
    emitter.emit_jump(Opcode::JUMP, skip);
    ei.end = emitter.current_ip();
    for (auto &c: catches) {
        // Exception handling pushes the exception info onto the stack here.
        emitter.set_stack_depth(start_stack_depth + 1);
        for (auto e: c.exceptions) {
            ei.excid = emitter.str(e->name);
            ei.handler = emitter.current_ip();
            ei.stack_depth = start_stack_depth;
            emitter.add_exception(ei);
        }
        if (c.name != nullptr) {
            c.name->generate_address(emitter);
            c.name->generate_store(emitter);
        } else {
            emitter.emit(Opcode::DROP);
        }
        const ExceptionHandlerStatement *ehs = dynamic_cast<const ExceptionHandlerStatement *>(c.handler);
        if (ehs != nullptr) {
            for (auto stmt: ehs->statements) {
                stmt->generate(emitter);
            }
        } else {
            internal_error("unexpected catch type");
        }
        emitter.emit_jump(Opcode::JUMP, skip);
    }
    emitter.jump_target(skip);
}

void ast::RaiseStatement::generate_code(Emitter &emitter) const
{
    assert(info->type == TYPE_OBJECT);
    info->generate(emitter);
    unsigned int index = emitter.str(exception->name);
    emitter.emit(Opcode::EXCEPT, index);
}

void ast::ResetStatement::generate_code(Emitter &emitter) const
{
    for (auto v: variables) {
         v->generate_address_write(emitter);
         emitter.emit(Opcode::RESETC);
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

    emitter.jump_target(emitter.function_info(0).entry_label);
    frame->predeclare(emitter);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    const Function *main = dynamic_cast<const Function *>(scope->lookupName("MAIN"));
    if (main != nullptr) {
        emitter.emit(Opcode::CALLP, emitter.str("runtime$moduleIsMain"));
        emitter.adjust_stack_depth(1);
        auto skip = emitter.create_label();
        emitter.emit_jump(Opcode::JF, skip);
        emitter.emit(Opcode::CALLF, main->function_index);
        emitter.adjust_stack_depth(0);
        emitter.jump_target(skip);
    }
    emitter.emit(Opcode::RET);
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
    auto node = out.nested_object("type");
    node.write("display", "Boolean");
    node.write("representation", "boolean");
    node.close();
}

void ast::TypeNumber::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Number");
    node.write("representation", "number");
    node.close();
}

void ast::TypeString::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "String");
    node.write("representation", "string");
    node.close();
}

void ast::TypeBytes::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Bytes");
    node.write("representation", "string");
    node.close();
}

void ast::TypeObject::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Object");
    node.write("representation", "object");
    node.close();
}

void ast::TypeFunction::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Function(...)");
    node.write("representation", "function");
    node.close();
}

void ast::TypeArray::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Array");
    node.write("representation", "array");
    node.close();
}

void ast::TypeDictionary::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Dictionary");
    node.write("representation", "dictionary");
    node.close();
}

void ast::TypeRecord::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Record");
    node.write("representation", "array");
    node.close();
}

void ast::TypePointer::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Pointer");
    node.write("representation", "address");
    node.close();
}

void ast::TypeInterfacePointer::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "InterfacePointer");
    node.write("representation", "array");
    node.close();
}

void ast::TypeFunctionPointer::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "FunctionPointer");
    node.write("representation", "array");
    node.close();
}

void ast::TypeEnum::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Enum");
    node.write("representation", "number");
    node.close();
}

void ast::TypeModule::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Module");
    node.write("representation", "module");
    node.close();
}

void ast::TypeException::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Exception");
    node.write("representation", "none");
    node.close();
}

void ast::TypeInterface::debuginfo(Emitter &, minijson::object_writer &out) const
{
    auto node = out.nested_object("type");
    node.write("display", "Interface");
    node.write("representation", "none");
    node.close();
}

void ast::Function::debuginfo(Emitter &emitter, minijson::object_writer &out) const
{
    out.write("entry", emitter.function_info(function_index).entry_label.get_target());
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
            if (function != nullptr && slot.referenced) {
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
