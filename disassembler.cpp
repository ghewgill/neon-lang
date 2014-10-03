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

    void disasm_PUSHN();
    void disasm_PUSHS();
    void disasm_LOADN();
    void disasm_LOADS();
    void disasm_STOREN();
    void disasm_STORES();
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

void Disassembler::disasm_LOADN()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "LOADN " << val << "\n";
}

void Disassembler::disasm_LOADS()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "LOADS " << val << "\n";
}

void Disassembler::disasm_STOREN()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "STOREN " << val << "\n";
}

void Disassembler::disasm_STORES()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "STORES " << val << "\n";
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
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CALLF " << val << "\n";
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
        switch (obj.code[index]) {
            case PUSHN:  disasm_PUSHN(); break;
            case PUSHS:  disasm_PUSHS(); break;
            case LOADN:  disasm_LOADN(); break;
            case LOADS:  disasm_LOADS(); break;
            case STOREN: disasm_STOREN(); break;
            case STORES: disasm_STORES(); break;
            case NEGN:   disasm_NEGN(); break;
            case ADDN:   disasm_ADDN(); break;
            case SUBN:   disasm_SUBN(); break;
            case MULN:   disasm_MULN(); break;
            case DIVN:   disasm_DIVN(); break;
            case CALLP:  disasm_CALLP(); break;
            case CALLF:  disasm_CALLF(); break;
            case JUMP:   disasm_JUMP(); break;
            case JZ:     disasm_JZ(); break;
            case RET:    disasm_RET(); break;
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
