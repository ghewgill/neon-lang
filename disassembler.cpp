#include "disassembler.h"

#include <ostream>
#include <stdlib.h>
#include <string>

#include "bytecode.h"
#include "number.h"
#include "opcode.h"

class Disassembler {
public:
    Disassembler(std::ostream &out, const Bytecode::bytecode &obj);
    void disassemble();
private:
    std::ostream &out;
    const Bytecode obj;
    Bytecode::bytecode::size_type index;

    void disasm_PUSHB();
    void disasm_PUSHN();
    void disasm_PUSHS();
    void disasm_LOADGB();
    void disasm_LOADGN();
    void disasm_LOADGS();
    void disasm_STOREGB();
    void disasm_STOREGN();
    void disasm_STOREGS();
    void disasm_NEGN();
    void disasm_ADDN();
    void disasm_SUBN();
    void disasm_MULN();
    void disasm_DIVN();
    void disasm_CALLP();
    void disasm_CALLF();
    void disasm_JUMP();
    void disasm_JZ();
    void disasm_RET();
};

Disassembler::Disassembler(std::ostream &out, const Bytecode::bytecode &obj)
  : out(out), obj(obj), index(0)
{
}

void Disassembler::disasm_PUSHB()
{
    bool val = obj.code[index+1] != 0;
    index += 2;
    out << "PUSHB " << val << "\n";
}

void Disassembler::disasm_PUSHN()
{
    // TODO: endian
    Number val = *reinterpret_cast<const Number *>(&obj.code[index+1]);
    index += 1 + sizeof(val);
    out << "PUSHN " << number_to_string(val) << "\n";
}

void Disassembler::disasm_PUSHS()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHS \"" << obj.strtable[val] << "\"\n";
}

void Disassembler::disasm_LOADGB()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "LOADGB " << addr << "\n";
}

void Disassembler::disasm_LOADGN()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "LOADGN " << addr << "\n";
}

void Disassembler::disasm_LOADGS()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "LOADGS " << addr << "\n";
}

void Disassembler::disasm_STOREGB()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "STOREGB " << addr << "\n";
}

void Disassembler::disasm_STOREGN()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "STOREGN " << addr << "\n";
}

void Disassembler::disasm_STOREGS()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "STOREGS " << addr << "\n";
}

void Disassembler::disasm_NEGN()
{
    out << "NEGN\n";
    index++;
}

void Disassembler::disasm_ADDN()
{
    out << "ADDN\n";
    index++;
}

void Disassembler::disasm_SUBN()
{
    out << "SUBN\n";
    index++;
}

void Disassembler::disasm_MULN()
{
    out << "MULN\n";
    index++;
}

void Disassembler::disasm_DIVN()
{
    out << "DIVN\n";
    index++;
}

void Disassembler::disasm_CALLP()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CALLP " << obj.strtable[val] << "\n";
}

void Disassembler::disasm_CALLF()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CALLF " << addr << "\n";
}

void Disassembler::disasm_JUMP()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JUMP " << addr << "\n";
}

void Disassembler::disasm_JZ()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JZ " << addr << "\n";
}

void Disassembler::disasm_RET()
{
    out << "RET\n";
    index++;
}

void Disassembler::disassemble()
{
    out << "String table: [\n";
    int i = 0;
    for (auto s: obj.strtable) {
        out << "  " << i << " " << s << "\n";
        i++;
    }
    out << "]\n";
    while (index < obj.code.size()) {
        out << index << " ";
        auto last_index = index;
        switch (static_cast<Opcode>(obj.code[index])) {
            case PUSHB:   disasm_PUSHB(); break;
            case PUSHN:   disasm_PUSHN(); break;
            case PUSHS:   disasm_PUSHS(); break;
            case LOADGB:  disasm_LOADGB(); break;
            case LOADGN:  disasm_LOADGN(); break;
            case LOADGS:  disasm_LOADGS(); break;
            case STOREGB: disasm_STOREGB(); break;
            case STOREGN: disasm_STOREGN(); break;
            case STOREGS: disasm_STOREGS(); break;
            case NEGN:    disasm_NEGN(); break;
            case ADDN:    disasm_ADDN(); break;
            case SUBN:    disasm_SUBN(); break;
            case MULN:    disasm_MULN(); break;
            case DIVN:    disasm_DIVN(); break;
            case CALLP:   disasm_CALLP(); break;
            case CALLF:   disasm_CALLF(); break;
            case JUMP:    disasm_JUMP(); break;
            case JZ:      disasm_JZ(); break;
            case RET:     disasm_RET(); break;
        }
        if (index == last_index) {
            out << "disassembler: Unexpected opcode: " << obj.code[index] << "\n";
            abort();
        }
    }
}

void disassemble(const Bytecode::bytecode &obj, std::ostream &out)
{
    Disassembler(out, obj).disassemble();
}
