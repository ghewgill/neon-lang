#include "exec.h"

#include <iostream>
#include <sstream>
#include <stack>
#include <stdlib.h>

#include "bytecode.h"
#include "opcode.h"

class StackEntry {
public:
    StackEntry() {}
    StackEntry(int value): intvalue(value) {}
    StackEntry(const std::string &value): strvalue(value) {}
    int intvalue;
    std::string strvalue;
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
    std::vector<StackEntry> vars;

    void exec_PUSHI();
    void exec_PUSHS();
    void exec_LOADI();
    void exec_LOADS();
    void exec_STOREI();
    void exec_STORES();
    void exec_NEGI();
    void exec_ADDI();
    void exec_SUBI();
    void exec_MULI();
    void exec_DIVI();
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

void Executor::exec_PUSHI()
{
    int val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(val));
}

void Executor::exec_PUSHS()
{
    int val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(obj.strtable[val]));
}

void Executor::exec_LOADI()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(vars.at(addr).intvalue));
}

void Executor::exec_LOADS()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(StackEntry(vars.at(addr).strvalue));
}

void Executor::exec_STOREI()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    int val = stack.top().intvalue; stack.pop();
    if (vars.size() < addr+1) {
        vars.resize(addr+1);
    }
    vars[addr] = StackEntry(val);
}

void Executor::exec_STORES()
{
    size_t addr = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    std::string val = stack.top().strvalue; stack.pop();
    if (vars.size() < addr+1) {
        vars.resize(addr+1);
    }
    vars[addr] = val;
}

void Executor::exec_NEGI()
{
    ip++;
    int x = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(-x));
}

void Executor::exec_ADDI()
{
    ip++;
    int b = stack.top().intvalue; stack.pop();
    int a = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(a + b));
}

void Executor::exec_SUBI()
{
    ip++;
    int b = stack.top().intvalue; stack.pop();
    int a = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(a - b));
}

void Executor::exec_MULI()
{
    ip++;
    int b = stack.top().intvalue; stack.pop();
    int a = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(a * b));
}

void Executor::exec_DIVI()
{
    ip++;
    int b = stack.top().intvalue; stack.pop();
    int a = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(a / b));
}

void Executor::exec_CALLP()
{
    size_t val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    std::string func = obj.strtable.at(val);
    if (func == "abs") {
        int x = stack.top().intvalue; stack.pop();
        stack.push(StackEntry(abs(x)));
    } else if (func == "concat") {
        std::string y = stack.top().strvalue; stack.pop();
        std::string x = stack.top().strvalue; stack.pop();
        stack.push(StackEntry(x + y));
    } else if (func == "print") {
        std::string x = stack.top().strvalue; stack.pop();
        out << x << std::endl;
    } else if (func == "str") {
        int x = stack.top().intvalue; stack.pop();
        std::stringstream s;
        s << x;
        stack.push(StackEntry(s.str()));
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
    int a = stack.top().intvalue; stack.pop();
    if (a == 0) {
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
        switch (obj.code[ip]) {
            case PUSHI:  exec_PUSHI(); break;
            case PUSHS:  exec_PUSHS(); break;
            case LOADI:  exec_LOADI(); break;
            case LOADS:  exec_LOADS(); break;
            case STOREI: exec_STOREI(); break;
            case STORES: exec_STORES(); break;
            case NEGI:   exec_NEGI(); break;
            case ADDI:   exec_ADDI(); break;
            case SUBI:   exec_SUBI(); break;
            case MULI:   exec_MULI(); break;
            case DIVI:   exec_DIVI(); break;
            case CALLP:  exec_CALLP(); break;
            case CALLF:  exec_CALLF(); break;
            case JUMP:   exec_JUMP(); break;
            case JZ:     exec_JZ(); break;
            case RET:    exec_RET(); break;
            default:
                printf("exec: Unexpected opcode: %d\n", obj.code[ip]);
                abort();
        }
    }
}

void exec(const Bytecode::bytecode &obj)
{
    Executor(obj, std::cout).exec();
}
