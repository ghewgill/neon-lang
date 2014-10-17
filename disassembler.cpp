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

    void disasm_ENTER();
    void disasm_LEAVE();
    void disasm_PUSHB();
    void disasm_PUSHN();
    void disasm_PUSHS();
    void disasm_PUSHAG();
    void disasm_PUSHAL();
    void disasm_LOADB();
    void disasm_LOADN();
    void disasm_LOADS();
    void disasm_STOREB();
    void disasm_STOREN();
    void disasm_STORES();
    void disasm_NEGN();
    void disasm_ADDN();
    void disasm_SUBN();
    void disasm_MULN();
    void disasm_DIVN();
    void disasm_EXPN();
    void disasm_EQN();
    void disasm_NEN();
    void disasm_LTN();
    void disasm_GTN();
    void disasm_LEN();
    void disasm_GEN();
    void disasm_EQS();
    void disasm_NES();
    void disasm_LTS();
    void disasm_GTS();
    void disasm_LES();
    void disasm_GES();
    void disasm_ANDB();
    void disasm_ORB();
    void disasm_NOTB();
    void disasm_INDEXA();
    void disasm_INDEXD();
    void disasm_CALLP();
    void disasm_CALLF();
    void disasm_JUMP();
    void disasm_JF();
    void disasm_RET();
};

Disassembler::Disassembler(std::ostream &out, const Bytecode::bytecode &obj)
  : out(out), obj(obj), index(0)
{
}

void Disassembler::disasm_ENTER()
{
    int val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "ENTER " << val << "\n";
}

void Disassembler::disasm_LEAVE()
{
    out << "LEAVE\n";
    index++;
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

void Disassembler::disasm_PUSHAG()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHAG " << addr << "\n";
}

void Disassembler::disasm_PUSHAL()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHAL " << addr << "\n";
}

void Disassembler::disasm_LOADB()
{
    out << "LOADB\n";
    index++;
}

void Disassembler::disasm_LOADN()
{
    out << "LOADN\n";
    index++;
}

void Disassembler::disasm_LOADS()
{
    out << "LOADS\n";
    index++;
}

void Disassembler::disasm_STOREB()
{
    out << "STOREB\n";
    index++;
}

void Disassembler::disasm_STOREN()
{
    out << "STOREN\n";
    index++;
}

void Disassembler::disasm_STORES()
{
    out << "STORES\n";
    index++;
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

void Disassembler::disasm_EXPN()
{
    out << "EXPN\n";
    index++;
}

void Disassembler::disasm_EQN()
{
    out << "EQN\n";
    index++;
}

void Disassembler::disasm_NEN()
{
    out << "NEN\n";
    index++;
}

void Disassembler::disasm_LTN()
{
    out << "LTN\n";
    index++;
}

void Disassembler::disasm_GTN()
{
    out << "GTN\n";
    index++;
}

void Disassembler::disasm_LEN()
{
    out << "LEN\n";
    index++;
}

void Disassembler::disasm_GEN()
{
    out << "GEN\n";
    index++;
}

void Disassembler::disasm_EQS()
{
    out << "EQS\n";
    index++;
}

void Disassembler::disasm_NES()
{
    out << "NES\n";
    index++;
}

void Disassembler::disasm_LTS()
{
    out << "LTS\n";
    index++;
}

void Disassembler::disasm_GTS()
{
    out << "GTS\n";
    index++;
}

void Disassembler::disasm_LES()
{
    out << "LES\n";
    index++;
}

void Disassembler::disasm_GES()
{
    out << "GES\n";
    index++;
}


void Disassembler::disasm_ANDB()
{
    out << "ANDB\n";
    index++;
}

void Disassembler::disasm_ORB()
{
    out << "ORB\n";
    index++;
}

void Disassembler::disasm_NOTB()
{
    out << "NOTB\n";
    index++;
}

void Disassembler::disasm_INDEXA()
{
    out << "INDEXA\n";
    index++;
}

void Disassembler::disasm_INDEXD()
{
    out << "INDEXD\n";
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

void Disassembler::disasm_JF()
{
    int addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JF " << addr << "\n";
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
            case ENTER:   disasm_ENTER(); break;
            case LEAVE:   disasm_LEAVE(); break;
            case PUSHB:   disasm_PUSHB(); break;
            case PUSHN:   disasm_PUSHN(); break;
            case PUSHS:   disasm_PUSHS(); break;
            case PUSHAG:  disasm_PUSHAG(); break;
            case PUSHAL:  disasm_PUSHAL(); break;
            case LOADB:   disasm_LOADB(); break;
            case LOADN:   disasm_LOADN(); break;
            case LOADS:   disasm_LOADS(); break;
            case STOREB:  disasm_STOREB(); break;
            case STOREN:  disasm_STOREN(); break;
            case STORES:  disasm_STORES(); break;
            case NEGN:    disasm_NEGN(); break;
            case ADDN:    disasm_ADDN(); break;
            case SUBN:    disasm_SUBN(); break;
            case MULN:    disasm_MULN(); break;
            case DIVN:    disasm_DIVN(); break;
            case EXPN:    disasm_EXPN(); break;
            case EQN:     disasm_EQN(); break;
            case NEN:     disasm_NEN(); break;
            case LTN:     disasm_LTN(); break;
            case GTN:     disasm_GTN(); break;
            case LEN:     disasm_LEN(); break;
            case GEN:     disasm_GEN(); break;
            case EQS:     disasm_EQS(); break;
            case NES:     disasm_NES(); break;
            case LTS:     disasm_LTS(); break;
            case GTS:     disasm_GTS(); break;
            case LES:     disasm_LES(); break;
            case GES:     disasm_GES(); break;
            case ANDB:    disasm_ANDB(); break;
            case ORB:     disasm_ORB(); break;
            case NOTB:    disasm_NOTB(); break;
            case INDEXA:  disasm_INDEXA(); break;
            case INDEXD:  disasm_INDEXD(); break;
            case CALLP:   disasm_CALLP(); break;
            case CALLF:   disasm_CALLF(); break;
            case JUMP:    disasm_JUMP(); break;
            case JF:      disasm_JF(); break;
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
