#include "compiler.h"

#include <assert.h>

#include "ast.h"
#include "opcode.h"

class Emitter {
    class Label {
        friend class Emitter;
        Label(): target(-1) {}
        std::vector<int> fixups;
        int target;
    };
public:
    void emit(unsigned char b);
    void emit(const std::vector<unsigned char> &instr);
    std::vector<unsigned char> getObject();
    unsigned int global(const std::string &name);
    unsigned int str(const std::string &s);
    int next_function();
    Label &function_label(int index);
    Label create_label();
    void emit_jump(unsigned char b, Label &label);
    void jump_target(Label &label);
private:
    std::vector<unsigned char> code;
    std::vector<std::string> strings;
    std::vector<std::string> globals;
    std::vector<Label> functions;
};

void Emitter::emit(unsigned char b)
{
    code.push_back(b);
}

void Emitter::emit(const std::vector<unsigned char> &instr)
{
    std::copy(instr.begin(), instr.end(), std::back_inserter(code));
}

std::vector<unsigned char> Emitter::getObject()
{
    std::vector<unsigned char> strtable;
    for (auto s: strings) {
        std::copy(s.begin(), s.end(), std::back_inserter(strtable));
        strtable.push_back(0);
    }
    std::vector<unsigned char> obj;
    obj.push_back(strtable.size() >> 8);
    obj.push_back(strtable.size());
    std::copy(strtable.begin(), strtable.end(), std::back_inserter(obj));
    std::copy(code.begin(), code.end(), std::back_inserter(obj));
    return obj;
}

unsigned int Emitter::global(const std::string &name)
{
    auto i = find(globals.begin(), globals.end(), name);
    if (i == globals.end()) {
        globals.push_back(name);
        i = globals.end() - 1;
    }
    return i - globals.begin();
}

unsigned int Emitter::str(const std::string &s)
{
    auto i = find(strings.begin(), strings.end(), s);
    if (i == strings.end()) {
        strings.push_back(s);
        i = strings.end() - 1;
    }
    return i - strings.begin();
}

int Emitter::next_function()
{
    auto i = functions.size();
    functions.push_back(create_label());
    return i;
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
    if (label.target >= 0) {
        emit(label.target >> 24);
        emit(label.target >> 16);
        emit(label.target >> 8);
        emit(label.target);
    } else {
        label.fixups.push_back(code.size());
        emit(0);
        emit(0);
        emit(0);
        emit(0);
    }
}

void Emitter::jump_target(Label &label)
{
    assert(label.target < 0);
    label.target = code.size();
    for (auto offset: label.fixups) {
        code[offset] = label.target >> 24;
        code[offset+1] = label.target >> 16;
        code[offset+2] = label.target >> 8;
        code[offset+3] = label.target;
    }
}

int Type::declare(const std::string &name, Emitter &emitter) const
{
    return emitter.global(name);
}

void TypeNumber::generate_load(Emitter &emitter, int index) const
{
    emitter.emit(LOADN);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void TypeNumber::generate_store(Emitter &emitter, int index) const
{
    emitter.emit(STOREN);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void TypeNumber::generate_call(Emitter &emitter, int index) const
{
    assert(false);
}

void TypeString::generate_load(Emitter &emitter, int index) const
{
    emitter.emit(LOADS);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void TypeString::generate_store(Emitter &emitter, int index) const
{
    emitter.emit(STORES);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void TypeString::generate_call(Emitter &emitter, int index) const
{
    assert(false);
}

void TypeFunction::generate_load(Emitter &emitter, int index) const
{
    assert(false);
}

void TypeFunction::generate_store(Emitter &emitter, int index) const
{
    assert(false);
}

void TypeFunction::generate_call(Emitter &emitter, int index) const
{
    emitter.emit_jump(CALLF, emitter.function_label(index));
}

int TypePredefinedFunction::declare(const std::string &name, Emitter &emitter) const
{
    return emitter.str(name);
}

void TypePredefinedFunction::generate_load(Emitter &emitter, int index) const
{
    assert(false);
}

void TypePredefinedFunction::generate_store(Emitter &emitter, int index) const
{
    assert(false);
}

void TypePredefinedFunction::generate_call(Emitter &emitter, int index) const
{
    emitter.emit(CALLP);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void Variable::predeclare(Emitter &emitter)
{
    if (referenced) {
        index = type->declare(name, emitter);
    }
}

void Variable::generate_load(Emitter &emitter) const
{
    type->generate_load(emitter, index);
}

void Variable::generate_store(Emitter &emitter) const
{
    type->generate_store(emitter, index);
}

void Variable::generate_call(Emitter &emitter) const
{
    type->generate_call(emitter, index);
}

void Function::predeclare(Emitter &emitter)
{
    if (referenced) {
        index = emitter.next_function();
    }
}

void Function::postdeclare(Emitter &emitter)
{
    if (referenced) {
        // TODO scope->generate(emitter);
        emitter.jump_target(emitter.function_label(index));
        for (auto stmt: statements) {
            stmt->generate(emitter);
        }
        emitter.emit(RET);
    }
}

void Function::generate(Emitter &emitter) const
{
}

void ConstantNumberExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHN);
    // TODO: endian
    const unsigned char *v = reinterpret_cast<const unsigned char *>(&value);
    emitter.emit(std::vector<unsigned char>(v, v+sizeof(value)));
}

void ConstantStringExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHS);
    int index = emitter.str(value);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void UnaryMinusExpression::generate(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NEGN);
}

void AdditionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(ADDN);
}

void SubtractionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(SUBN);
}

void MultiplicationExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MULN);
}

void DivisionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(DIVN);
}

void ScalarVariableReference::generate_load(Emitter &emitter) const
{
    var->generate_load(emitter);
}

void ScalarVariableReference::generate_store(Emitter &emitter) const
{
    var->generate_store(emitter);
}

void ScalarVariableReference::generate_call(Emitter &emitter) const
{
    var->generate_call(emitter);
}

void VariableExpression::generate(Emitter &emitter) const
{
    var->generate_load(emitter);
}

void FunctionCall::generate(Emitter &emitter) const
{
    for (auto arg: args) {
        arg->generate(emitter);
    }
    func->generate_call(emitter);
}

void AssignmentStatement::generate(Emitter &emitter) const
{
    expr->generate(emitter);
    variable->generate_store(emitter);
}

void ExpressionStatement::generate(Emitter &emitter) const
{
    expr->generate(emitter);
}

void IfStatement::generate(Emitter &emitter) const
{
    condition->generate(emitter);
    auto skip = emitter.create_label();
    emitter.emit_jump(JZ, skip);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.jump_target(skip);
}

void ReturnStatement::generate(Emitter &emitter) const
{
    expr->generate(emitter);
    // TODO: jump to block exit
}

void WhileStatement::generate(Emitter &emitter) const
{
    auto top = emitter.create_label();
    emitter.jump_target(top);
    condition->generate(emitter);
    auto skip = emitter.create_label();
    emitter.emit_jump(JZ, skip);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit_jump(JUMP, top);
    emitter.jump_target(skip);
}

void Scope::predeclare(Emitter &emitter) const
{
    for (auto n: names) {
        n.second->predeclare(emitter);
    }
}

void Scope::postdeclare(Emitter &emitter) const
{
    for (auto n: names) {
        n.second->postdeclare(emitter);
    }
}

void Program::generate(Emitter &emitter) const
{
    scope->predeclare(emitter);
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
    emitter.emit(RET);
    scope->postdeclare(emitter);
}

std::vector<unsigned char> compile(const Program *p)
{
    Emitter emitter;
    p->generate(emitter);
    return emitter.getObject();
}
