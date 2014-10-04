#include "exec.h"

#include <iostream>
#include <sstream>
#include <stack>
#include <stdlib.h>

#include "bytecode.h"
#include "number.h"
#include "opcode.h"

class StackEntry {
public:
    StackEntry() {}
    StackEntry(bool value): boolean_value(value) {}
    StackEntry(Number value): number_value(value) {}
    StackEntry(const std::string &value): string_value(value) {}
    bool boolean_value;
    Number number_value;
    std::string string_value;
};

class Executor {
public:
    Executor(const Bytecode::bytecode &obj, std::ostream &out);
    void exec();
private:
    const Bytecode obj;
    std::ostream &out;
    Bytecode::bytecode::size_type ip;
    std::stack<StackEntry> stack;
    std::stack<Bytecode::bytecode::size_type> callstack;
    std::vector<StackEntry> globals;

    void exec_PUSHB();
    void exec_PUSHN();
    void exec_PUSHS();
    void exec_LOADGB();
    void exec_LOADGN();
    void exec_LOADGS();
    void exec_STOREGB();
    void exec_STOREGN();
    void exec_STOREGS();
    void exec_NEGN();
    void exec_ADDN();
    void exec_SUBN();
    void exec_MULN();
    void exec_DIVN();
    void exec_CALLP();
    void exec_CALLF();
    void exec_JUMP();
    void exec_JZ();
    void exec_RET();
};

Executor::Executor(const Bytecode::bytecode &obj, std::ostream &out)
  : obj(obj), out(out), ip(0)
{
}

void Executor::exec_PUSHB()
{
    bool val = obj.code[ip+1] != 0;
    ip += 2;
    stack.push(StackEntry(val));
}

void Executor::exec_PUSHN()
{
    // TODO: endian
    Number val = *reinterpret_cast<const Number *>(&obj.code[ip+1]);
    ip += 1 + sizeof(val);
    stack.push(StackEntry(val));
}

void Executor::exec_PUSHS()
{
    int val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(obj.strtable[val]));
}

void Executor::exec_LOADGB()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(globals.at(addr).boolean_value));
}

void Executor::exec_LOADGN()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(globals.at(addr).number_value));
}

void Executor::exec_LOADGS()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(globals.at(addr).string_value));
}

void Executor::exec_STOREGB()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    bool val = stack.top().boolean_value; stack.pop();
    if (globals.size() < addr+1) {
        globals.resize(addr+1);
    }
    globals[addr] = StackEntry(val);
}

void Executor::exec_STOREGN()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    Number val = stack.top().number_value; stack.pop();
    if (globals.size() < addr+1) {
        globals.resize(addr+1);
    }
    globals[addr] = StackEntry(val);
}

void Executor::exec_STOREGS()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    std::string val = stack.top().string_value; stack.pop();
    if (globals.size() < addr+1) {
        globals.resize(addr+1);
    }
    globals[addr] = val;
}

void Executor::exec_NEGN()
{
    ip++;
    Number x = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_negate(x)));
}

void Executor::exec_ADDN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_add(a, b)));
}

void Executor::exec_SUBN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_subtract(a, b)));
}

void Executor::exec_MULN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_multiply(a, b)));
}

void Executor::exec_DIVN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_divide(a, b)));
}

void Executor::exec_CALLP()
{
    size_t val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    std::string func = obj.strtable.at(val);
    if (func == "abs") {
        Number x = stack.top().number_value; stack.pop();
        stack.push(StackEntry(number_abs(x)));
    } else if (func == "concat") {
        std::string y = stack.top().string_value; stack.pop();
        std::string x = stack.top().string_value; stack.pop();
        stack.push(StackEntry(x + y));
    } else if (func == "print") {
        std::string x = stack.top().string_value; stack.pop();
        out << x << std::endl;
    } else if (func == "str") {
        Number x = stack.top().number_value; stack.pop();
        stack.push(StackEntry(number_to_string(x)));
    } else {
        fprintf(stderr, "simple: function not found: %s\n", func.c_str());
        abort();
    }
}

void Executor::exec_CALLF()
{
    size_t val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    callstack.push(ip);
    ip = val;
}

void Executor::exec_JUMP()
{
    int target = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    ip = target;
}

void Executor::exec_JZ()
{
    int target = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    Number a = stack.top().number_value; stack.pop();
    if (number_is_zero(a)) {
        ip = target;
    }
}

void Executor::exec_RET()
{
    ip = callstack.top();
    callstack.pop();
}

void Executor::exec()
{
    callstack.push(0);
    while (not callstack.empty() && ip < obj.code.size()) {
        //std::cerr << "ip " << ip << " op " << (int)obj.code[ip] << "\n";
        auto last_ip = ip;
        switch (static_cast<Opcode>(obj.code[ip])) {
            case PUSHB:   exec_PUSHB(); break;
            case PUSHN:   exec_PUSHN(); break;
            case PUSHS:   exec_PUSHS(); break;
            case LOADGB:  exec_LOADGB(); break;
            case LOADGN:  exec_LOADGN(); break;
            case LOADGS:  exec_LOADGS(); break;
            case STOREGB: exec_STOREGB(); break;
            case STOREGN: exec_STOREGN(); break;
            case STOREGS: exec_STOREGS(); break;
            case NEGN:    exec_NEGN(); break;
            case ADDN:    exec_ADDN(); break;
            case SUBN:    exec_SUBN(); break;
            case MULN:    exec_MULN(); break;
            case DIVN:    exec_DIVN(); break;
            case CALLP:   exec_CALLP(); break;
            case CALLF:   exec_CALLF(); break;
            case JUMP:    exec_JUMP(); break;
            case JZ:      exec_JZ(); break;
            case RET:     exec_RET(); break;
        }
        if (ip == last_ip) {
            fprintf(stderr, "exec: Unexpected opcode: %d\n", obj.code[ip]);
            abort();
        }
    }
}

void exec(const Bytecode::bytecode &obj)
{
    Executor(obj, std::cout).exec();
}
