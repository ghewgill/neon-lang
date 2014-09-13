#include "disassembler.h"

#include <stdlib.h>
#include <string>

#include "bytecode.h"
#include "opcode.h"

class Disassembler {
public:
    Disassembler(const Bytecode::bytecode &obj);
    void disassemble();
private:
    const Bytecode obj;
    Bytecode::bytecode::size_type index;

    void disasm_PUSHI();
    void disasm_ADDROF();
    void disasm_LOADI();
    void disasm_STOREI();
    void disasm_NEGI();
    void disasm_ADDI();
    void disasm_SUBI();
    void disasm_MULI();
    void disasm_DIVI();
    void disasm_CALL();
    void disasm_JUMP();
    void disasm_JZ();
};

Disassembler::Disassembler(const Bytecode::bytecode &obj)
  : obj(obj), index(0)
{
}

void Disassembler::disasm_PUSHI()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    printf("PUSHI %d\n", val);
}

void Disassembler::disasm_LOADI()
{
    printf("LOADI\n");
    index++;
}

void Disassembler::disasm_STOREI()
{
    printf("STOREI\n");
    index++;
}

void Disassembler::disasm_NEGI()
{
    printf("NEGI\n");
    index++;
}

void Disassembler::disasm_ADDI()
{
    printf("ADDI\n");
    index++;
}

void Disassembler::disasm_SUBI()
{
    printf("SUBI\n");
    index++;
}

void Disassembler::disasm_MULI()
{
    printf("MULI\n");
    index++;
}

void Disassembler::disasm_DIVI()
{
    printf("DIVI\n");
    index++;
}

void Disassembler::disasm_CALL()
{
    printf("CALL\n");
    index++;
}

void Disassembler::disasm_JUMP()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    printf("JUMP %d\n", val);
}

void Disassembler::disasm_JZ()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    printf("JZ %d\n", val);
}

void Disassembler::disassemble()
{
    printf("String table: [\n");
    int i = 0;
    for (auto s: obj.strtable) {
        printf("  %4u %s\n", i, s.c_str());
        i++;
    }
    printf("]\n");
    while (index < obj.code.size()) {
        printf("%4lu ", index);
        switch (obj.code[index]) {
            case PUSHI:  disasm_PUSHI(); break;
            case LOADI:  disasm_LOADI(); break;
            case STOREI: disasm_STOREI(); break;
            case NEGI:   disasm_NEGI(); break;
            case ADDI:   disasm_ADDI(); break;
            case SUBI:   disasm_SUBI(); break;
            case MULI:   disasm_MULI(); break;
            case DIVI:   disasm_DIVI(); break;
            case CALL:   disasm_CALL(); break;
            case JUMP:   disasm_JUMP(); break;
            case JZ:     disasm_JZ(); break;
            default:
                printf("disassembler: Unexpected opcode: %d\n", obj.code[index]);
                abort();
        }
    }
}

void disassemble(const Bytecode::bytecode &obj)
{
    Disassembler(obj).disassemble();
}
