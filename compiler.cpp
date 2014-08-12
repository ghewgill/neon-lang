#include "compiler.h"

#include "ast.h"
#include "opcode.h"

class Emitter {
public:
    void emit(unsigned char b);
    void emit(const std::vector<unsigned char> &instr);
    std::vector<unsigned char> getObject();
    unsigned int global(const std::string &s);
    unsigned int str(const std::string &s);
private:
    std::vector<unsigned char> code;
    std::vector<std::string> strings;
    std::vector<std::string> globals;
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

unsigned int Emitter::global(const std::string &s)
{
    auto i = find(globals.begin(), globals.end(), s);
    if (i == globals.end()) {
        globals.push_back(s);
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

void ConstantExpression::generate(Emitter &emitter) const
{
    emitter.emit(PUSHI);
    emitter.emit(value >> 24);
    emitter.emit(value >> 16);
    emitter.emit(value >> 8);
    emitter.emit(value);
}

void UnaryMinusExpression::generate(Emitter &emitter) const
{
    value->generate(emitter);
    emitter.emit(NEGI);
}

void AdditionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(ADDI);
}

void SubtractionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(SUBI);
}

void MultiplicationExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(MULI);
}

void DivisionExpression::generate(Emitter &emitter) const
{
    left->generate(emitter);
    right->generate(emitter);
    emitter.emit(DIVI);
}

void ScalarVariableReference::generate(Emitter &emitter) const
{
    unsigned int index = emitter.global(name);
    emitter.emit(PUSHI);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void FunctionReference::generate(Emitter &emitter) const
{
    unsigned int index = emitter.str(name);
    emitter.emit(PUSHI);
    emitter.emit(index >> 24);
    emitter.emit(index >> 16);
    emitter.emit(index >> 8);
    emitter.emit(index);
}

void VariableExpression::generate(Emitter &emitter) const
{
    var->generate(emitter);
    emitter.emit(LOADI);
}

void FunctionCall::generate(Emitter &emitter) const
{
    for (auto arg: args) {
        arg->generate(emitter);
    }
    func->generate(emitter);
    emitter.emit(CALL);
}

void AssignmentStatement::generate(Emitter &emitter) const
{
    expr->generate(emitter);
    variable->generate(emitter);
    emitter.emit(STOREI);
}

void ExpressionStatement::generate(Emitter &emitter) const
{
    expr->generate(emitter);
}

void Program::generate(Emitter &emitter) const
{
    for (auto stmt: statements) {
        stmt->generate(emitter);
    }
}

std::vector<unsigned char> compile(const Program *p)
{
    Emitter emitter;
    p->generate(emitter);
    return emitter.getObject();
}
