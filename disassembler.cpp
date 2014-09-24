#include "disassembler.h"

#include <ostream>
#include <stdlib.h>
#include <string>

#include "bytecode.h"
#include "opcode.h"

class Disassembler {
public:
    Disassembler(std::ostream &out, const Bytecode::bytecode &obj);
    void disassemble();
private:
    std::ostream &out;
    const Bytecode obj;
    Bytecode::bytecode::size_type index;

    void disasm_PUSHI();
    void disasm_PUSHS();
    void disasm_ADDROF();
    void disasm_LOADI();
    void disasm_LOADS();
    void disasm_STOREI();
    void disasm_STORES();
    void disasm_NEGI();
    void disasm_ADDI();
    void disasm_SUBI();
    void disasm_MULI();
    void disasm_DIVI();
    void disasm_CALL();
    void disasm_JUMP();
    void disasm_JZ();
};

Disassembler::Disassembler(std::ostream &out, const Bytecode::bytecode &obj)
  : out(out), obj(obj), index(0)
{
}

void Disassembler::disasm_PUSHI()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHI " << val << "\n";
}

void Disassembler::disasm_PUSHS()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHS \"" << obj.strtable[val] << "\"\n";
}

void Disassembler::disasm_LOADI()
{
    out << "LOADI\n";
    index++;
}

void Disassembler::disasm_LOADS()
{
    out << "LOADS\n";
    index++;
}

void Disassembler::disasm_STOREI()
{
    out << "STOREI\n";
    index++;
}

void Disassembler::disasm_STORES()
{
    out << "STORES\n";
    index++;
}

void Disassembler::disasm_NEGI()
{
    out << "NEGI\n";
    index++;
}

void Disassembler::disasm_ADDI()
{
    out << "ADDI\n";
    index++;
}

void Disassembler::disasm_SUBI()
{
    out << "SUBI\n";
    index++;
}

void Disassembler::disasm_MULI()
{
    out << "MULI\n";
    index++;
}

void Disassembler::disasm_DIVI()
{
    out << "DIVI\n";
    index++;
}

void Disassembler::disasm_CALL()
{
    out << "CALL\n";
    index++;
}

void Disassembler::disasm_JUMP()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JUMP " << val << "\n";
}

void Disassembler::disasm_JZ()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JZ " << val << "\n";
}

void Disassembler::disassemble()
{
    out << "String table: [\n";
    int i = 0;
    for (auto s: obj.strtable) {
        out << "  " << i << " " << s << "\n";
        i++;
    }
    out << "[\n";
    while (index < obj.code.size()) {
        out << index << " ";
        switch (obj.code[index]) {
            case PUSHI:  disasm_PUSHI(); break;
            case PUSHS:  disasm_PUSHS(); break;
            case LOADI:  disasm_LOADI(); break;
            case LOADS:  disasm_LOADS(); break;
            case STOREI: disasm_STOREI(); break;
            case STORES: disasm_STORES(); break;
            case NEGI:   disasm_NEGI(); break;
            case ADDI:   disasm_ADDI(); break;
            case SUBI:   disasm_SUBI(); break;
            case MULI:   disasm_MULI(); break;
            case DIVI:   disasm_DIVI(); break;
            case CALL:   disasm_CALL(); break;
            case JUMP:   disasm_JUMP(); break;
            case JZ:     disasm_JZ(); break;
            default:
                out << "disassembler: Unexpected opcode: " << obj.code[index] << "\n";
                abort();
        }
    }
}

void disassemble(const Bytecode::bytecode &obj, std::ostream &out)
{
    Disassembler(out, obj).disassemble();
}
