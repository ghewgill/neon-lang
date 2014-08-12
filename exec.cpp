#include "exec.h"

#include <stack>
#include <stdlib.h>

#include "bytecode.h"
#include "opcode.h"

class Executor {
public:
    Executor(const Bytecode::bytecode &obj);
    void exec();
private:
    const Bytecode obj;
    Bytecode::bytecode::size_type ip;
    std::stack<int> stack;
    std::vector<int> vars;

    void exec_PUSHI();
    void exec_ADDROF();
    void exec_LOADI();
    void exec_STOREI();
    void exec_NEGI();
    void exec_ADDI();
    void exec_SUBI();
    void exec_MULI();
    void exec_DIVI();
    void exec_CALL();
};

Executor::Executor(const Bytecode::bytecode &obj)
  : obj(obj), ip(0)
{
}

void Executor::exec_PUSHI()
{
    int val = (obj.code[ip+1] << 24) | (obj.code[ip+2] << 16) | (obj.code[ip+3] << 8) | obj.code[ip+4];
    ip += 5;
    stack.push(val);
}

void Executor::exec_LOADI()
{
    ip++;
    unsigned int addr = stack.top(); stack.pop();
    stack.push(vars[addr]);
}

void Executor::exec_STOREI()
{
    ip++;
    unsigned int addr = stack.top(); stack.pop();
    int val = stack.top(); stack.pop();
    if (vars.size() < addr+1) {
        vars.resize(addr+1);
    }
    vars[addr] = val;
}

void Executor::exec_NEGI()
{
    ip++;
    int x = stack.top(); stack.pop();
    stack.push(-x);
}

void Executor::exec_ADDI()
{
    ip++;
    int b = stack.top(); stack.pop();
    int a = stack.top(); stack.pop();
    stack.push(a + b);
}

void Executor::exec_SUBI()
{
    ip++;
    int b = stack.top(); stack.pop();
    int a = stack.top(); stack.pop();
    stack.push(a - b);
}

void Executor::exec_MULI()
{
    ip++;
    int b = stack.top(); stack.pop();
    int a = stack.top(); stack.pop();
    stack.push(a * b);
}

void Executor::exec_DIVI()
{
    ip++;
    int b = stack.top(); stack.pop();
    int a = stack.top(); stack.pop();
    stack.push(a / b);
}

void Executor::exec_CALL()
{
    ip++;
    std::string func = obj.strtable[stack.top()]; stack.pop();
    if (func == "abs") {
        int x = stack.top(); stack.pop();
        stack.push(abs(x));
    } else if (func == "print") {
        int x = stack.top(); stack.pop();
        printf("%d\n", x);
    } else {
        abort();
    }
}

void Executor::exec()
{
    while (ip < obj.code.size()) {
        switch (obj.code[ip]) {
            case PUSHI:  exec_PUSHI(); break;
            case LOADI:  exec_LOADI(); break;
            case STOREI: exec_STOREI(); break;
            case NEGI:   exec_NEGI(); break;
            case ADDI:   exec_ADDI(); break;
            case SUBI:   exec_SUBI(); break;
            case MULI:   exec_MULI(); break;
            case DIVI:   exec_DIVI(); break;
            case CALL:   exec_CALL(); break;
            default:
                printf("Unexpected opcode: %d\n", obj.code[ip]);
                abort();
        }
    }
}

void exec(const Bytecode::bytecode &obj)
{
    Executor(obj).exec();
}
