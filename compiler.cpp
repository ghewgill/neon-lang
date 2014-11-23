#include "compiler.h"

#include <algorithm>
#include <assert.h>
#include <iso646.h>
#include <sstream>

#include "ast.h"
#include "debuginfo.h"
#include "opcode.h"

class Emitter {
    class Label {
        friend class Emitter;
        Label(): fixups(), target(UINT_MAX) {}
        std::vector<unsigned int> fixups;
        unsigned int target;
    };
    class LoopLabels {
    public:
        LoopLabels(): exit(nullptr), next(nullptr) {}
        LoopLabels(Label *exit, Label *next): exit(exit), next(next) {}
        Label *exit;
        Label *next;
    };
public:
    Emitter(DebugInfo *debug): code(), strings(), globals(), functions(), loop_labels(), debug_info(debug) {}
    void emit(unsigned char b);
    void emit_uint32(uint32_t value);
    void emit(unsigned char b, uint32_t value);
    void emit(unsigned char b, const Number &value);
    void emit(const std::vector<unsigned char> &instr);
    std::vector<unsigned char> getObject();
    unsigned int global(const std::string &name);
    unsigned int str(const std::string &s);
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
private:
    std::vector<unsigned char> code;
    std::vector<std::string> strings;
    std::vector<std::string> globals;
    std::vector<Label> functions;
    std::map<size_t, LoopLabels> loop_labels;
    DebugInfo *debug_info;
private:
    Emitter(const Emitter &);
    Emitter &operator=(const Emitter &);
};

void Emitter::emit(unsigned char b)
{
    code.push_back(b);
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

void Emitter::emit(unsigned char b, const Number &value)
{
    emit(b);
    // TODO: endian
    const unsigned char *v = reinterpret_cast<const unsigned char *>(&value);
    emit(std::vector<unsigned char>(v, v+sizeof(value)));
}

void Emitter::emit(const std::vector<unsigned char> &instr)
{
    std::copy(instr.begin(), instr.end(), std::back_inserter(code));
}

std::vector<unsigned char> Emitter::getObject()
{
    std::vector<unsigned char> obj;
    obj.push_back(static_cast<unsigned char>(globals.size() >> 8) & 0xff);
    obj.push_back(static_cast<unsigned char>(globals.size() & 0xff));
    std::vector<unsigned char> strtable;
    for (auto s: strings) {
        std::copy(s.begin(), s.end(), std::back_inserter(strtable));
        strtable.push_back(0);
    }
    obj.push_back(static_cast<unsigned char>(strtable.size() >> 8) & 0xff);
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
        label.fixups.push_back(static_cast<unsigned int>(code.size()));
        emit_uint32(UINT32_MAX);
    }
}

void Emitter::jump_target(Label &label)
{
    assert(label.target == UINT_MAX);
    label.target = static_cast<unsigned int>(code.size());
    for (auto offset: label.fixups) {
        code[offset] = static_cast<unsigned char>((label.target >> 24) & 0xff);
        code[offset+1] = static_cast<unsigned char>((label.target >> 16) & 0xff);
        code[offset+2] = static_cast<unsigned char>((label.target >> 8) & 0xff);
        code[offset+3] = static_cast<unsigned char>(label.target & 0xff);
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
    debug_info->line_numbers[code.size()] = line;
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
    index = emitter.global(name);
}

void GlobalVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPG, index);
}

void LocalVariable::predeclare(Emitter &)
{
    index = scope->nextIndex();
}

void LocalVariable::generate_address(Emitter &emitter) const
{
    emitter.emit(PUSHPL, index);
}

void FunctionParameter::generate_address(Emitter &emitter) const
{
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

void Function::predeclare(Emitter &emitter)
{
    entry_label = emitter.next_function();
}

void Function::postdeclare(Emitter &emitter)
{
    scope->predeclare(emitter);
    emitter.jump_target(emitter.function_label(entry_label));
    emitter.emit(ENTER, scope->getCount());
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
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
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

void ExternalFunction::predeclare(Emitter &emitter)
{
    std::stringstream ss;
    ss << library_name << ":" << name << ":";
    auto r = param_types.find("return");
    if (r != param_types.end()) {
        ss << r->second;
    }
    ss << ":";
    for (auto param: params) {
        ss << param_types.at(param->name) << ",";
    }
    external_index = emitter.str(ss.str());
}

void ExternalFunction::generate_call(Emitter &emitter) const
{
    emitter.emit(CALLE, external_index);
}

void ConstantBooleanExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHB);
    emitter.emit(value ? 1 : 0);
}

void ConstantNumberExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHN, value);
}

void ConstantStringExpression::generate(Emitter &emitter) const
{
    unsigned int index = emitter.str(value);
    emitter.emit(PUSHS, index);
}

void ConstantEnumExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHN, number_from_uint32(value));
}

void ArrayLiteralExpression::generate(Emitter &emitter) const
{
    for (auto e = elements.rbegin(); e != elements.rend(); ++e) {
        (*e)->generate(emitter);
    }
    emitter.emit(CONSA, static_cast<uint32_t>(elements.size()));
}

void DictionaryLiteralExpression::generate(Emitter &emitter) const
{
    for (auto d = dict.rbegin(); d != dict.rend(); ++d) {
        emitter.emit(PUSHS, emitter.str(d->first));
        d->second->generate(emitter);
    }
    emitter.emit(CONSD, static_cast<uint32_t>(dict.size()));
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

void ConditionalExpression::generate(Emitter &emitter) const
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

void DisjunctionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    auto true_label = emitter.create_label();
    emitter.emit_jump(JT, true_label);
    emitter.emit(DROP);
    right->generate(emitter);
    emitter.jump_target(true_label);
}

void ConjunctionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    emitter.emit(DUP);
    auto false_label = emitter.create_label();
    emitter.emit_jump(JF, false_label);
    emitter.emit(DROP);
    right->generate(emitter);
    emitter.jump_target(false_label);
}

void ArrayInExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(INA);
}

void DictionaryInExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(IND);
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

void VariableReference::generate_call(Emitter &) const
{
    internal_error("VariableReference");
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
    const TypeFunction *ftype = dynamic_cast<const TypeFunction *>(func->type);
    // TODO: This is a ridiculous hack because the way we compile
    // StringReference::store is not really legal. This assertion
    // holds true for any other function call.
    if (func->text() != "ScalarVariableReference(PredefinedFunction(splice, TypeFunction(...)))") {
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
                arg->get_reference()->generate_address(emitter);
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
                arg->get_reference()->generate_store(emitter);
                break;
        }
    }
}

void Statement::generate(Emitter &emitter) const
{
    emitter.debug_line(line);
    generate_code(emitter);
}

void AssignmentStatement::generate_code(Emitter &emitter) const
{
    expr->generate(emitter);
    variable->generate_store(emitter);
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
    expr->generate(emitter);
    // TODO: jump to block exit
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
    auto skip = emitter.create_label();
    auto loop = emitter.create_label();
    auto next = emitter.create_label();

    start->generate(emitter);
    var->generate_store(emitter);
    emitter.jump_target(loop);

    end->generate(emitter);
    var->generate_load(emitter);
    emitter.emit(GEN);
    emitter.emit_jump(JF, skip);

    emitter.add_loop_labels(loop_id, skip, next);

    for (auto stmt: statements) {
        stmt->generate(emitter);
    }

    emitter.jump_target(next);
    emitter.emit(PUSHN, number_from_uint32(1));
    var->generate_load(emitter);
    emitter.emit(ADDN);
    var->generate_store(emitter);
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

void Scope::predeclare(Emitter &emitter) const
{
    for (auto n: names) {
        if (referenced.find(n.second) != referenced.end()) {
            n.second->predeclare(emitter);
        }
    }
}

void Scope::postdeclare(Emitter &emitter) const
{
    for (auto n: names) {
        if (referenced.find(n.second) != referenced.end()) {
            n.second->postdeclare(emitter);
        }
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

std::vector<unsigned char> compile(const Program *p, DebugInfo *debug)
{
    Emitter emitter(debug);
    p->generate(emitter);
    return emitter.getObject();
}
