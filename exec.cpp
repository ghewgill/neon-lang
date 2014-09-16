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
    std::vector<StackEntry> vars;

    void exec_PUSHI();
    void exec_PUSHS();
    void exec_ADDROF();
    void exec_LOADI();
    void exec_LOADS();
    void exec_STOREI();
    void exec_STORES();
    void exec_NEGI();
    void exec_ADDI();
    void exec_SUBI();
    void exec_MULI();
    void exec_DIVI();
    void exec_CALL();
    void exec_JUMP();
    void exec_JZ();
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
    ip++;
    unsigned int addr = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(vars.at(addr).intvalue));
}

void Executor::exec_LOADS()
{
    ip++;
    unsigned int addr = stack.top().intvalue; stack.pop();
    stack.push(StackEntry(vars.at(addr).strvalue));
}

void Executor::exec_STOREI()
{
    ip++;
    unsigned int addr = stack.top().intvalue; stack.pop();
    int val = stack.top().intvalue; stack.pop();
    if (vars.size() < addr+1) {
        vars.resize(addr+1);
    }
    vars[addr] = StackEntry(val);
}

void Executor::exec_STORES()
{
    ip++;
    unsigned int addr = stack.top().intvalue; stack.pop();
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

void Executor::exec_CALL()
{
    ip++;
    std::string func = obj.strtable.at(stack.top().intvalue); stack.pop();
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

void Executor::exec()
{
    while (ip < obj.code.size()) {
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
            case CALL:   exec_CALL(); break;
            case JUMP:   exec_JUMP(); break;
            case JZ:     exec_JZ(); break;
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
