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
    StackEntry(void *value): address_value(value) {}
    StackEntry(bool value): boolean_value(value) {}
    StackEntry(Number value): number_value(value) {}
    StackEntry(const std::string &value): string_value(value) {}
    StackEntry(const char *value): string_value(value) {}
    void *address_value;
    bool boolean_value;
    Number number_value;
    std::string string_value;
};

class ActivationFrame {
public:
    ActivationFrame(size_t count): locals(count) {}
    // TODO (for nested functions) std::vector<ActivationFrame *> outer;
    std::vector<StackEntry> locals;
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
    std::vector<ActivationFrame> frames;

    void exec_ENTER();
    void exec_LEAVE();
    void exec_PUSHB();
    void exec_PUSHN();
    void exec_PUSHS();
    void exec_PUSHAG();
    void exec_PUSHAL();
    void exec_LOADB();
    void exec_LOADN();
    void exec_LOADS();
    void exec_STOREB();
    void exec_STOREN();
    void exec_STORES();
    void exec_NEGN();
    void exec_ADDN();
    void exec_SUBN();
    void exec_MULN();
    void exec_DIVN();
    void exec_EXPN();
    void exec_EQN();
    void exec_NEN();
    void exec_LTN();
    void exec_GTN();
    void exec_LEN();
    void exec_GEN();
    void exec_EQS();
    void exec_NES();
    void exec_LTS();
    void exec_GTS();
    void exec_LES();
    void exec_GES();
    void exec_ANDB();
    void exec_ORB();
    void exec_NOTB();
    void exec_CALLP();
    void exec_CALLF();
    void exec_JUMP();
    void exec_JF();
    void exec_RET();
};

Executor::Executor(const Bytecode::bytecode &obj, std::ostream &out)
  : obj(obj), out(out), ip(0)
{
}

void Executor::exec_ENTER()
{
    int val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    frames.push_back(ActivationFrame(val));
}

void Executor::exec_LEAVE()
{
    frames.pop_back();
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

void Executor::exec_PUSHAG()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    if (globals.size() < addr+1) {
        globals.resize(addr+1);
    }
    stack.push(StackEntry(&globals.at(addr)));
}

void Executor::exec_PUSHAL()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(&frames.back().locals.at(addr)));
}

void Executor::exec_LOADB()
{
    ip++;
    void *addr = stack.top().address_value; stack.pop();
    stack.push(static_cast<StackEntry *>(addr)->boolean_value);
}

void Executor::exec_LOADN()
{
    ip++;
    void *addr = stack.top().address_value; stack.pop();
    stack.push(static_cast<StackEntry *>(addr)->number_value);
}

void Executor::exec_LOADS()
{
    ip++;
    void *addr = stack.top().address_value; stack.pop();
    stack.push(static_cast<StackEntry *>(addr)->string_value);
}

void Executor::exec_STOREB()
{
    ip++;
    void *addr = stack.top().address_value; stack.pop();
    bool val = stack.top().boolean_value; stack.pop();
    *static_cast<StackEntry *>(addr) = StackEntry(val);
}

void Executor::exec_STOREN()
{
    ip++;
    void *addr = stack.top().address_value; stack.pop();
    Number val = stack.top().number_value; stack.pop();
    *static_cast<StackEntry *>(addr) = StackEntry(val);
}

void Executor::exec_STORES()
{
    ip++;
    void *addr = stack.top().address_value; stack.pop();
    std::string val = stack.top().string_value; stack.pop();
    *static_cast<StackEntry *>(addr) = StackEntry(val);
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

void Executor::exec_EXPN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_pow(a, b)));
}

void Executor::exec_EQN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_is_equal(a, b)));
}

void Executor::exec_NEN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_is_not_equal(a, b)));
}

void Executor::exec_LTN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_is_less(a, b)));
}

void Executor::exec_GTN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_is_greater(a, b)));
}

void Executor::exec_LEN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_is_less_equal(a, b)));
}

void Executor::exec_GEN()
{
    ip++;
    Number b = stack.top().number_value; stack.pop();
    Number a = stack.top().number_value; stack.pop();
    stack.push(StackEntry(number_is_greater_equal(a, b)));
}

void Executor::exec_EQS()
{
    ip++;
    std::string b = stack.top().string_value; stack.pop();
    std::string a = stack.top().string_value; stack.pop();
    stack.push(StackEntry(a == b));
}

void Executor::exec_NES()
{
    ip++;
    std::string b = stack.top().string_value; stack.pop();
    std::string a = stack.top().string_value; stack.pop();
    stack.push(StackEntry(a != b));
}

void Executor::exec_LTS()
{
    ip++;
    std::string b = stack.top().string_value; stack.pop();
    std::string a = stack.top().string_value; stack.pop();
    stack.push(StackEntry(a < b));
}

void Executor::exec_GTS()
{
    ip++;
    std::string b = stack.top().string_value; stack.pop();
    std::string a = stack.top().string_value; stack.pop();
    stack.push(StackEntry(a > b));
}

void Executor::exec_LES()
{
    ip++;
    std::string b = stack.top().string_value; stack.pop();
    std::string a = stack.top().string_value; stack.pop();
    stack.push(StackEntry(a <= b));
}

void Executor::exec_GES()
{
    ip++;
    std::string b = stack.top().string_value; stack.pop();
    std::string a = stack.top().string_value; stack.pop();
    stack.push(StackEntry(a >= b));
}

void Executor::exec_ANDB()
{
    ip++;
    bool b = stack.top().boolean_value; stack.pop();
    bool a = stack.top().boolean_value; stack.pop();
    stack.push(StackEntry(a && b));
}

void Executor::exec_ORB()
{
    ip++;
    bool b = stack.top().boolean_value; stack.pop();
    bool a = stack.top().boolean_value; stack.pop();
    stack.push(StackEntry(a || b));
}

void Executor::exec_NOTB()
{
    ip++;
    bool x = stack.top().boolean_value; stack.pop();
    stack.push(StackEntry(not x));
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
    } else if (func == "strb") {
        bool x = stack.top().boolean_value; stack.pop();
        stack.push(StackEntry(x ? "TRUE" : "FALSE"));
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

void Executor::exec_JF()
{
    int target = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    bool a = stack.top().boolean_value; stack.pop();
    if (not a) {
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
            case ENTER:   exec_ENTER(); break;
            case LEAVE:   exec_LEAVE(); break;
            case PUSHB:   exec_PUSHB(); break;
            case PUSHN:   exec_PUSHN(); break;
            case PUSHS:   exec_PUSHS(); break;
            case PUSHAG:  exec_PUSHAG(); break;
            case PUSHAL:  exec_PUSHAL(); break;
            case LOADB:   exec_LOADB(); break;
            case LOADN:   exec_LOADN(); break;
            case LOADS:   exec_LOADS(); break;
            case STOREB:  exec_STOREB(); break;
            case STOREN:  exec_STOREN(); break;
            case STORES:  exec_STORES(); break;
            case NEGN:    exec_NEGN(); break;
            case ADDN:    exec_ADDN(); break;
            case SUBN:    exec_SUBN(); break;
            case MULN:    exec_MULN(); break;
            case DIVN:    exec_DIVN(); break;
            case EXPN:    exec_EXPN(); break;
            case EQN:     exec_EQN(); break;
            case NEN:     exec_NEN(); break;
            case LTN:     exec_LTN(); break;
            case GTN:     exec_GTN(); break;
            case LEN:     exec_LEN(); break;
            case GEN:     exec_GEN(); break;
            case EQS:     exec_EQS(); break;
            case NES:     exec_NES(); break;
            case LTS:     exec_LTS(); break;
            case GTS:     exec_GTS(); break;
            case LES:     exec_LES(); break;
            case GES:     exec_GES(); break;
            case ANDB:    exec_ANDB(); break;
            case ORB:     exec_ORB(); break;
            case NOTB:    exec_NOTB(); break;
            case CALLP:   exec_CALLP(); break;
            case CALLF:   exec_CALLF(); break;
            case JUMP:    exec_JUMP(); break;
            case JF:      exec_JF(); break;
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
