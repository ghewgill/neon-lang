#include "compiler.h"

#include <algorithm>
#include <assert.h>

#include "ast.h"
#include "opcode.h"

class Emitter {
    class Label {
        friend class Emitter;
        Label(): fixups(), target(-1) {}
        std::vector<int> fixups;
        int target;
    };
public:
    Emitter(): code(), strings(), globals(), functions() {}
    void emit(unsigned char b);
    void emit_int(int value);
    void emit(unsigned char b, int value);
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

void Emitter::emit_int(int value)
{
    emit(static_cast<unsigned char>(value >> 24));
    emit(static_cast<unsigned char>(value >> 16));
    emit(static_cast<unsigned char>(value >> 8));
    emit(static_cast<unsigned char>(value));
}

void Emitter::emit(unsigned char b, int value)
{
    emit(b);
    emit_int(value);
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
    obj.push_back(static_cast<unsigned char>(strtable.size() >> 8));
    obj.push_back(static_cast<unsigned char>(strtable.size() & 0xff));
    std::copy(strtable.begin(), strtable.end(), std::back_inserter(obj));
    std::copy(code.begin(), code.end(), std::back_inserter(obj));
    return obj;
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
    auto i = std::find(strings.begin(), strings.end(), s);
    if (i == strings.end()) {
        strings.push_back(s);
        i = strings.end() - 1;
    }
    return static_cast<unsigned int>(i - strings.begin());
}

int Emitter::next_function()
{
    auto i = functions.size();
    functions.push_back(create_label());
    return static_cast<int>(i);
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
        emit_int(label.target);
    } else {
        label.fixups.push_back(static_cast<int>(code.size()));
        emit_int(0);
    }
}

void Emitter::jump_target(Label &label)
{
    assert(label.target < 0);
    label.target = static_cast<int>(code.size());
    for (auto offset: label.fixups) {
        code[offset] = static_cast<unsigned char>(label.target >> 24);
        code[offset+1] = static_cast<unsigned char>(label.target >> 16);
        code[offset+2] = static_cast<unsigned char>(label.target >> 8);
        code[offset+3] = static_cast<unsigned char>(label.target);
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

void TypeBoolean::generate_call(Emitter &emitter) const
{
    assert(false);
}

void TypeNumber::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADN);
}

void TypeNumber::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREN);
}

void TypeNumber::generate_call(Emitter &emitter) const
{
    assert(false);
}

void TypeString::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADS);
}

void TypeString::generate_store(Emitter &emitter) const
{
    emitter.emit(STORES);
}

void TypeString::generate_call(Emitter &emitter) const
{
    assert(false);
}

void TypeFunction::generate_load(Emitter &emitter) const
{
    assert(false);
}

void TypeFunction::generate_store(Emitter &emitter) const
{
    assert(false);
}

void TypeFunction::generate_call(Emitter &emitter) const
{
    assert(false);
}

void TypeArray::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void TypeArray::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void TypeArray::generate_call(Emitter &emitter) const
{
    assert(false);
}

void TypeDictionary::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADD);
}

void TypeDictionary::generate_store(Emitter &emitter) const
{
    emitter.emit(STORED);
}

void TypeDictionary::generate_call(Emitter &emitter) const
{
    assert(false);
}

void TypeRecord::generate_load(Emitter &emitter) const
{
    emitter.emit(LOADA);
}

void TypeRecord::generate_store(Emitter &emitter) const
{
    emitter.emit(STOREA);
}

void TypeRecord::generate_call(Emitter &emitter) const
{
    assert(false);
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
    type->generate_call(emitter);
}

void GlobalVariable::predeclare(Emitter &emitter)
{
    if (referenced) {
        index = emitter.global(name);
    }
}

void GlobalVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPG, index);
}

void LocalVariable::predeclare(Emitter &emitter)
{
    if (referenced) {
        index = scope->count;
        scope->count++;
    }
}

void LocalVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPL, index);
}

void Function::predeclare(Emitter &emitter)
{
    if (referenced) {
        entry_label = emitter.next_function();
    }
}

void Function::postdeclare(Emitter &emitter)
{
    if (referenced) {
        scope->predeclare(emitter);
        emitter.jump_target(emitter.function_label(entry_label));
        emitter.emit(ENTER, static_cast<int>(scope->names.size()));
        for (auto a = args.rbegin(); a != args.rend(); ++a) {
            (*a)->generate_address(emitter);
            (*a)->generate_store(emitter);
        }
        for (auto stmt: statements) {
            stmt->generate(emitter);
        }
        emitter.emit(LEAVE);
        emitter.emit(RET);
    }
}

void Function::generate_call(Emitter &emitter) const
{
    emitter.emit_jump(CALLF, emitter.function_label(entry_label));
}

void PredefinedFunction::predeclare(Emitter &emitter)
{
    name_index = emitter.str(name);
}

void PredefinedFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLP, name_index);
}

void ConstantBooleanExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHB);
    emitter.emit(value ? 1 : 0);
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
    int index = emitter.str(value);
    emitter.emit(PUSHS, index);
}

void ConstantEnumExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHN);
    Number n = number_from_uint32(value);
    const unsigned char *v = reinterpret_cast<const unsigned char *>(&n);
    emitter.emit(std::vector<unsigned char>(v, v+sizeof(value)));
}

void UnaryMinusExpression::generate(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NEGN);
}

void LogicalNotExpression::generate(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NOTB);
}

void DisjunctionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(ORB);
}

void ConjunctionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(ANDB);
}

void BooleanComparisonExpression::generate(Emitter &emitter) const
{
    static const unsigned char op[] = {EQB, NEB};
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(op[comp]);
}

void NumericComparisonExpression::generate(Emitter &emitter) const
{
    static const unsigned char op[] = {EQN, NEN, LTN, GTN, LEN, GEN};
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(op[comp]);
}

void StringComparisonExpression::generate(Emitter &emitter) const
{
    static const unsigned char op[] = {EQS, NES, LTS, GTS, LES, GES};
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(op[comp]);
}

void ArrayComparisonExpression::generate(Emitter &emitter) const
{
    static const unsigned char op[] = {EQA, NEA};
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(op[comp]);
}

void DictionaryComparisonExpression::generate(Emitter &emitter) const
{
    static const unsigned char op[] = {EQD, NED};
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(op[comp]);
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

void ModuloExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MODN);
}

void ExponentiationExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(EXPN);
}

void VariableReference::generate_load(Emitter &emitter) const
{
    generate_address(emitter);
    type->generate_load(emitter);
}

void VariableReference::generate_store(Emitter &emitter) const
{
    generate_address(emitter);
    type->generate_store(emitter);
}

void VariableReference::generate_call(Emitter &emitter) const
{
    assert(false);
}

void ScalarVariableReference::generate_address(Emitter &emitter) const
{
    var->generate_address(emitter);
}

void ScalarVariableReference::generate_call(Emitter &emitter) const
{
    var->generate_call(emitter);
}

void StringReference::generate_load(Emitter &emitter) const
{
    load->generate(emitter);
}

void StringReference::generate_store(Emitter &emitter) const
{
    store->generate(emitter);
    str->generate_store(emitter);
}

void ArrayReference::generate_address(Emitter &emitter) const
{
    array->generate_address(emitter);
    index->generate(emitter);
    emitter.emit(INDEXA);
}

void DictionaryReference::generate_address(Emitter &emitter) const
{
    dict->generate_address(emitter);
    index->generate(emitter);
    emitter.emit(INDEXD);
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
    auto else_label = emitter.create_label();
    auto end_label = emitter.create_label();
    emitter.emit_jump(JF, else_label);
    for (auto stmt: then_statements) {
        stmt->generate(emitter);
    }
    emitter.emit_jump(JUMP, end_label);
    emitter.jump_target(else_label);
    for (auto stmt: else_statements) {
        stmt->generate(emitter);
    }
    emitter.jump_target(end_label);
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
    emitter.emit_jump(JF, skip);
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

void Module::predeclare(Emitter &emitter)
{
    scope->predeclare(emitter);
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
