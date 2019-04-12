#include "disassembler.h"

#include <iso646.h>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "bytecode.h"
#include "debuginfo.h"
#include "opcode.h"
#include "util.h"

class InstructionDisassembler {
public:
    InstructionDisassembler(const Bytecode &obj, size_t &index): obj(obj), index(index), out() {}
    const Bytecode &obj;
    size_t &index;
    std::stringstream out;

    void disassemble();

private:
    void disasm_PUSHB();
    void disasm_PUSHN();
    void disasm_PUSHS();
    void disasm_PUSHY();
    void disasm_PUSHPG();
    void disasm_PUSHPPG();
    void disasm_PUSHPMG();
    void disasm_PUSHPL();
    void disasm_PUSHPOL();
    void disasm_PUSHI();
    void disasm_LOADB();
    void disasm_LOADN();
    void disasm_LOADS();
    void disasm_LOADY();
    void disasm_LOADA();
    void disasm_LOADD();
    void disasm_LOADP();
    void disasm_LOADJ();
    void disasm_LOADV();
    void disasm_STOREB();
    void disasm_STOREN();
    void disasm_STORES();
    void disasm_STOREY();
    void disasm_STOREA();
    void disasm_STORED();
    void disasm_STOREP();
    void disasm_STOREJ();
    void disasm_STOREV();
    void disasm_NEGN();
    void disasm_ADDN();
    void disasm_SUBN();
    void disasm_MULN();
    void disasm_DIVN();
    void disasm_MODN();
    void disasm_EXPN();
    void disasm_EQB();
    void disasm_NEB();
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
    void disasm_EQY();
    void disasm_NEY();
    void disasm_LTY();
    void disasm_GTY();
    void disasm_LEY();
    void disasm_GEY();
    void disasm_EQA();
    void disasm_NEA();
    void disasm_EQD();
    void disasm_NED();
    void disasm_EQP();
    void disasm_NEP();
    void disasm_EQV();
    void disasm_NEV();
    void disasm_ANDB();
    void disasm_ORB();
    void disasm_NOTB();
    void disasm_INDEXAR();
    void disasm_INDEXAW();
    void disasm_INDEXAV();
    void disasm_INDEXAN();
    void disasm_INDEXDR();
    void disasm_INDEXDW();
    void disasm_INDEXDV();
    void disasm_INA();
    void disasm_IND();
    void disasm_CALLP();
    void disasm_CALLF();
    void disasm_CALLMF();
    void disasm_CALLI();
    void disasm_JUMP();
    void disasm_JF();
    void disasm_JT();
    void disasm_JFCHAIN();
    void disasm_DUP();
    void disasm_DUPX1();
    void disasm_DROP();
    void disasm_RET();
    void disasm_CALLE();
    void disasm_CONSA();
    void disasm_CONSD();
    void disasm_EXCEPT();
    void disasm_ALLOC();
    void disasm_PUSHNIL();
    void disasm_JNASSERT();
    void disasm_RESETC();
    void disasm_PUSHPEG();
    void disasm_JUMPTBL();
    void disasm_CALLX();
    void disasm_SWAP();
    void disasm_DROPN();
    void disasm_PUSHM();
    void disasm_CALLV();
    void disasm_PUSHCI();
};

void InstructionDisassembler::disasm_PUSHB()
{
    bool val = obj.code[index+1] != 0;
    index += 2;
    out << "PUSHB " << val;
}

void InstructionDisassembler::disasm_PUSHN()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "PUSHN " << obj.strtable[val];
}

void InstructionDisassembler::disasm_PUSHS()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "PUSHS \"" << obj.strtable[val] << "\"";
}

void InstructionDisassembler::disasm_PUSHY()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "PUSHY " << val;
}

void InstructionDisassembler::disasm_PUSHPG()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "PUSHPG " << addr;
}

void InstructionDisassembler::disasm_PUSHPPG()
{
    index++;
    uint32_t name = Bytecode::get_vint(obj.code, index);
    out << "PUSHPPG " << obj.strtable[name];
}

void InstructionDisassembler::disasm_PUSHPMG()
{
    index++;
    uint32_t module = Bytecode::get_vint(obj.code, index);
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "PUSHPMG " << module << "," << addr;
}

void InstructionDisassembler::disasm_PUSHPL()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "PUSHPL " << addr;
}

void InstructionDisassembler::disasm_PUSHPOL()
{
    index++;
    uint32_t enclosing = Bytecode::get_vint(obj.code, index);
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "PUSHPOL " << enclosing << "," << addr;
}

void InstructionDisassembler::disasm_PUSHI()
{
    index++;
    uint32_t x = Bytecode::get_vint(obj.code, index);
    out << "PUSHI " << x;
}

void InstructionDisassembler::disasm_LOADB()
{
    out << "LOADB";
    index++;
}

void InstructionDisassembler::disasm_LOADN()
{
    out << "LOADN";
    index++;
}

void InstructionDisassembler::disasm_LOADS()
{
    out << "LOADS";
    index++;
}

void InstructionDisassembler::disasm_LOADY()
{
    out << "LOADY";
    index++;
}

void InstructionDisassembler::disasm_LOADA()
{
    out << "LOADA";
    index++;
}

void InstructionDisassembler::disasm_LOADD()
{
    out << "LOADD";
    index++;
}

void InstructionDisassembler::disasm_LOADP()
{
    out << "LOADP";
    index++;
}

void InstructionDisassembler::disasm_LOADJ()
{
    out << "LOADJ";
    index++;
}

void InstructionDisassembler::disasm_LOADV()
{
    out << "LOADV";
    index++;
}

void InstructionDisassembler::disasm_STOREB()
{
    out << "STOREB";
    index++;
}

void InstructionDisassembler::disasm_STOREN()
{
    out << "STOREN";
    index++;
}

void InstructionDisassembler::disasm_STORES()
{
    out << "STORES";
    index++;
}

void InstructionDisassembler::disasm_STOREY()
{
    out << "STOREY";
    index++;
}

void InstructionDisassembler::disasm_STOREA()
{
    out << "STOREA";
    index++;
}

void InstructionDisassembler::disasm_STORED()
{
    out << "STORED";
    index++;
}

void InstructionDisassembler::disasm_STOREP()
{
    out << "STOREP";
    index++;
}

void InstructionDisassembler::disasm_STOREJ()
{
    out << "STOREJ";
    index++;
}

void InstructionDisassembler::disasm_STOREV()
{
    out << "STOREV";
    index++;
}

void InstructionDisassembler::disasm_NEGN()
{
    out << "NEGN";
    index++;
}

void InstructionDisassembler::disasm_ADDN()
{
    out << "ADDN";
    index++;
}

void InstructionDisassembler::disasm_SUBN()
{
    out << "SUBN";
    index++;
}

void InstructionDisassembler::disasm_MULN()
{
    out << "MULN";
    index++;
}

void InstructionDisassembler::disasm_DIVN()
{
    out << "DIVN";
    index++;
}

void InstructionDisassembler::disasm_MODN()
{
    out << "MODN";
    index++;
}

void InstructionDisassembler::disasm_EXPN()
{
    out << "EXPN";
    index++;
}

void InstructionDisassembler::disasm_EQB()
{
    out << "EQB";
    index++;
}

void InstructionDisassembler::disasm_NEB()
{
    out << "NEB";
    index++;
}

void InstructionDisassembler::disasm_EQN()
{
    out << "EQN";
    index++;
}

void InstructionDisassembler::disasm_NEN()
{
    out << "NEN";
    index++;
}

void InstructionDisassembler::disasm_LTN()
{
    out << "LTN";
    index++;
}

void InstructionDisassembler::disasm_GTN()
{
    out << "GTN";
    index++;
}

void InstructionDisassembler::disasm_LEN()
{
    out << "LEN";
    index++;
}

void InstructionDisassembler::disasm_GEN()
{
    out << "GEN";
    index++;
}

void InstructionDisassembler::disasm_EQS()
{
    out << "EQS";
    index++;
}

void InstructionDisassembler::disasm_NES()
{
    out << "NES";
    index++;
}

void InstructionDisassembler::disasm_LTS()
{
    out << "LTS";
    index++;
}

void InstructionDisassembler::disasm_GTS()
{
    out << "GTS";
    index++;
}

void InstructionDisassembler::disasm_LES()
{
    out << "LES";
    index++;
}

void InstructionDisassembler::disasm_GES()
{
    out << "GES";
    index++;
}

void InstructionDisassembler::disasm_EQY()
{
    out << "EQY";
    index++;
}

void InstructionDisassembler::disasm_NEY()
{
    out << "NEY";
    index++;
}

void InstructionDisassembler::disasm_LTY()
{
    out << "LTY";
    index++;
}

void InstructionDisassembler::disasm_GTY()
{
    out << "GTY";
    index++;
}

void InstructionDisassembler::disasm_LEY()
{
    out << "LEY";
    index++;
}

void InstructionDisassembler::disasm_GEY()
{
    out << "GEY";
    index++;
}

void InstructionDisassembler::disasm_EQA()
{
    out << "EQA";
    index++;
}

void InstructionDisassembler::disasm_NEA()
{
    out << "NEA";
    index++;
}

void InstructionDisassembler::disasm_EQD()
{
    out << "EQD";
    index++;
}

void InstructionDisassembler::disasm_NED()
{
    out << "NED";
    index++;
}

void InstructionDisassembler::disasm_EQP()
{
    out << "EQP";
    index++;
}

void InstructionDisassembler::disasm_NEP()
{
    out << "NEP";
    index++;
}

void InstructionDisassembler::disasm_EQV()
{
    out << "EQV";
    index++;
}

void InstructionDisassembler::disasm_NEV()
{
    out << "NEV";
    index++;
}

void InstructionDisassembler::disasm_ANDB()
{
    out << "ANDB";
    index++;
}

void InstructionDisassembler::disasm_ORB()
{
    out << "ORB";
    index++;
}

void InstructionDisassembler::disasm_NOTB()
{
    out << "NOTB";
    index++;
}

void InstructionDisassembler::disasm_INDEXAR()
{
    out << "INDEXAR";
    index++;
}

void InstructionDisassembler::disasm_INDEXAW()
{
    out << "INDEXAW";
    index++;
}

void InstructionDisassembler::disasm_INDEXAV()
{
    out << "INDEXAV";
    index++;
}

void InstructionDisassembler::disasm_INDEXAN()
{
    out << "INDEXAN";
    index++;
}

void InstructionDisassembler::disasm_INDEXDR()
{
    out << "INDEXDR";
    index++;
}

void InstructionDisassembler::disasm_INDEXDW()
{
    out << "INDEXDW";
    index++;
}

void InstructionDisassembler::disasm_INDEXDV()
{
    out << "INDEXDV";
    index++;
}

void InstructionDisassembler::disasm_INA()
{
    out << "INA";
    index++;
}

void InstructionDisassembler::disasm_IND()
{
    out << "IND";
    index++;
}

void InstructionDisassembler::disasm_CALLP()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "CALLP " << val << " " << (val < obj.strtable.size() ? obj.strtable[val] : "(invalid)");
}

void InstructionDisassembler::disasm_CALLF()
{
    index++;
    uint32_t findex = Bytecode::get_vint(obj.code, index);
    out << "CALLF " << obj.functions[findex].name << " " << obj.strtable[obj.functions[findex].name];
}

void InstructionDisassembler::disasm_CALLMF()
{
    index++;
    uint32_t mod = Bytecode::get_vint(obj.code, index);
    uint32_t func = Bytecode::get_vint(obj.code, index);
    out << "CALLMF " << obj.strtable[mod] << "," << obj.strtable[func];
}

void InstructionDisassembler::disasm_CALLI()
{
    out << "CALLI";
    index++;
}

void InstructionDisassembler::disasm_JUMP()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "JUMP " << addr;
}

void InstructionDisassembler::disasm_JF()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "JF " << addr;
}

void InstructionDisassembler::disasm_JT()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "JT " << addr;
}

void InstructionDisassembler::disasm_JFCHAIN()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "JFCHAIN " << addr;
}

void InstructionDisassembler::disasm_DUP()
{
    out << "DUP";
    index++;
}

void InstructionDisassembler::disasm_DUPX1()
{
    out << "DUPX1";
    index++;
}

void InstructionDisassembler::disasm_DROP()
{
    out << "DROP";
    index++;
}

void InstructionDisassembler::disasm_RET()
{
    out << "RET";
    index++;
}

void InstructionDisassembler::disasm_CALLE()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "CALLE " << obj.strtable[val];
}

void InstructionDisassembler::disasm_CONSA()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "CONSA " << val;
}

void InstructionDisassembler::disasm_CONSD()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "CONSD " << val;
}

void InstructionDisassembler::disasm_EXCEPT()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "EXCEPT \"" << obj.strtable[val] << "\"";
}

void InstructionDisassembler::disasm_ALLOC()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "ALLOC " << val;
}

void InstructionDisassembler::disasm_PUSHNIL()
{
    out << "PUSHNIL";
    index++;
}

void InstructionDisassembler::disasm_JNASSERT()
{
    index++;
    uint32_t addr = Bytecode::get_vint(obj.code, index);
    out << "JNASSERT " << addr;
}

void InstructionDisassembler::disasm_RESETC()
{
    out << "RESETC";
    index++;
}

void InstructionDisassembler::disasm_PUSHPEG()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "PUSHPEG \"" << obj.strtable[val] << "\"";
}

void InstructionDisassembler::disasm_JUMPTBL()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "JUMPTBL " << val;
}

void InstructionDisassembler::disasm_CALLX()
{
    index++;
    uint32_t module = Bytecode::get_vint(obj.code, index);
    uint32_t name = Bytecode::get_vint(obj.code, index);
    uint32_t out_param_count = Bytecode::get_vint(obj.code, index);
    out << "CALLX " << obj.strtable[module] << "." << obj.strtable[name] << "," << out_param_count;
}

void InstructionDisassembler::disasm_SWAP()
{
    out << "SWAP";
    index++;
}

void InstructionDisassembler::disasm_DROPN()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "DROPN " << val;
}

void InstructionDisassembler::disasm_PUSHM()
{
    out << "PUSHM";
    index++;
}

void InstructionDisassembler::disasm_CALLV()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "CALLV " << val;
}

void InstructionDisassembler::disasm_PUSHCI()
{
    index++;
    uint32_t val = Bytecode::get_vint(obj.code, index);
    out << "PUSHCI \"" << obj.strtable[val] << "\"";
}

void InstructionDisassembler::disassemble()
{
    switch (static_cast<Opcode>(obj.code[index])) {
        case PUSHB:   disasm_PUSHB(); break;
        case PUSHN:   disasm_PUSHN(); break;
        case PUSHS:   disasm_PUSHS(); break;
        case PUSHY:   disasm_PUSHY(); break;
        case PUSHPG:  disasm_PUSHPG(); break;
        case PUSHPPG: disasm_PUSHPPG(); break;
        case PUSHPMG: disasm_PUSHPMG(); break;
        case PUSHPL:  disasm_PUSHPL(); break;
        case PUSHPOL: disasm_PUSHPOL(); break;
        case PUSHI:   disasm_PUSHI(); break;
        case LOADB:   disasm_LOADB(); break;
        case LOADN:   disasm_LOADN(); break;
        case LOADS:   disasm_LOADS(); break;
        case LOADY:   disasm_LOADY(); break;
        case LOADA:   disasm_LOADA(); break;
        case LOADD:   disasm_LOADD(); break;
        case LOADP:   disasm_LOADP(); break;
        case LOADJ:   disasm_LOADJ(); break;
        case LOADV:   disasm_LOADV(); break;
        case STOREB:  disasm_STOREB(); break;
        case STOREN:  disasm_STOREN(); break;
        case STORES:  disasm_STORES(); break;
        case STOREY:  disasm_STOREY(); break;
        case STOREA:  disasm_STOREA(); break;
        case STORED:  disasm_STORED(); break;
        case STOREP:  disasm_STOREP(); break;
        case STOREJ:  disasm_STOREJ(); break;
        case STOREV:  disasm_STOREV(); break;
        case NEGN:    disasm_NEGN(); break;
        case ADDN:    disasm_ADDN(); break;
        case SUBN:    disasm_SUBN(); break;
        case MULN:    disasm_MULN(); break;
        case DIVN:    disasm_DIVN(); break;
        case MODN:    disasm_MODN(); break;
        case EXPN:    disasm_EXPN(); break;
        case EQB:     disasm_EQB(); break;
        case NEB:     disasm_NEB(); break;
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
        case EQY:     disasm_EQY(); break;
        case NEY:     disasm_NEY(); break;
        case LTY:     disasm_LTY(); break;
        case GTY:     disasm_GTY(); break;
        case LEY:     disasm_LEY(); break;
        case GEY:     disasm_GEY(); break;
        case EQA:     disasm_EQA(); break;
        case NEA:     disasm_NEA(); break;
        case EQD:     disasm_EQD(); break;
        case NED:     disasm_NED(); break;
        case EQP:     disasm_EQP(); break;
        case NEP:     disasm_NEP(); break;
        case EQV:     disasm_EQV(); break;
        case NEV:     disasm_NEV(); break;
        case ANDB:    disasm_ANDB(); break;
        case ORB:     disasm_ORB(); break;
        case NOTB:    disasm_NOTB(); break;
        case INDEXAR: disasm_INDEXAR(); break;
        case INDEXAW: disasm_INDEXAW(); break;
        case INDEXAV: disasm_INDEXAV(); break;
        case INDEXAN: disasm_INDEXAN(); break;
        case INDEXDR: disasm_INDEXDR(); break;
        case INDEXDW: disasm_INDEXDW(); break;
        case INDEXDV: disasm_INDEXDV(); break;
        case INA:     disasm_INA(); break;
        case IND:     disasm_IND(); break;
        case CALLP:   disasm_CALLP(); break;
        case CALLF:   disasm_CALLF(); break;
        case CALLMF:  disasm_CALLMF(); break;
        case CALLI:   disasm_CALLI(); break;
        case JUMP:    disasm_JUMP(); break;
        case JF:      disasm_JF(); break;
        case JT:      disasm_JT(); break;
        case JFCHAIN: disasm_JFCHAIN(); break;
        case DUP:     disasm_DUP(); break;
        case DUPX1:   disasm_DUPX1(); break;
        case DROP:    disasm_DROP(); break;
        case RET:     disasm_RET(); break;
        case CALLE:   disasm_CALLE(); break;
        case CONSA:   disasm_CONSA(); break;
        case CONSD:   disasm_CONSD(); break;
        case EXCEPT:  disasm_EXCEPT(); break;
        case ALLOC:   disasm_ALLOC(); break;
        case PUSHNIL: disasm_PUSHNIL(); break;
        case JNASSERT:disasm_JNASSERT(); break;
        case RESETC:  disasm_RESETC(); break;
        case PUSHPEG: disasm_PUSHPEG(); break;
        case JUMPTBL: disasm_JUMPTBL(); break;
        case CALLX:   disasm_CALLX(); break;
        case SWAP:    disasm_SWAP(); break;
        case DROPN:   disasm_DROPN(); break;
        case PUSHM:   disasm_PUSHM(); break;
        case CALLV:   disasm_CALLV(); break;
        case PUSHCI:  disasm_PUSHCI(); break;
    }
}

class Disassembler {
public:
    Disassembler(std::ostream &out, const Bytecode::Bytes &bytes, const DebugInfo *debug);
    void disassemble();
private:
    std::ostream &out;
    Bytecode obj;
    const DebugInfo *debug;
    Bytecode::Bytes::size_type index;

    std::string decode_value(const std::string &type, const Bytecode::Bytes &value);
private:
    Disassembler(const Disassembler &);
    Disassembler &operator=(const Disassembler &);
};

Disassembler::Disassembler(std::ostream &out, const Bytecode::Bytes &bytes, const DebugInfo *debug)
  : out(out), obj(), debug(debug), index(0)
{
    obj.load("-disassembler-", bytes);
}

std::string Disassembler::decode_value(const std::string &type, const Bytecode::Bytes &value)
{
    switch (type.at(0)) {
        case 'B': {
            return value.at(0) != 0 ? "TRUE" : "FALSE";
        }
        case 'N': {
            // TODO: vint
            uint32_t len = (value.at(0) << 24) | (value.at(1) << 16) | (value.at(2) << 8) | value.at(3);
            return std::string(&value.at(4), &value.at(4)+len);
        }
        case 'S': {
            // TODO: vint
            uint32_t len = (value.at(0) << 24) | (value.at(1) << 16) | (value.at(2) << 8) | value.at(3);
            return std::string(&value.at(4), &value.at(4)+len);
        }
        default:
            // TODO internal_error("TODO unimplemented type in decode_value");
            return "<unknown>";
    }
}

void Disassembler::disassemble()
{
    out << "Global size: " << obj.global_size << "\n";
    out << "String table: [\n";
    {
        int i = 0;
        for (auto s: obj.strtable) {
            out << "  " << i << " " << s << "\n";
            i++;
        }
    }
    out << "]\n";

    out << "Exports:\n";
    out << "  Types:\n";
    for (auto t: obj.export_types) {
        out << "    " << obj.strtable[t.name] << " " << obj.strtable[t.descriptor] << "\n";
    }
    out << "  Constants:\n";
    for (auto c: obj.export_constants) {
        out << "    " << obj.strtable[c.name] << " " << obj.strtable[c.type] << " " << decode_value(obj.strtable[c.type], c.value) << "\n";
    }
    out << "  Variables:\n";
    for (auto v: obj.export_variables) {
        out << "    " << obj.strtable[v.name] << " " << obj.strtable[v.type] << " " << v.index << "\n";
    }
    out << "  Functions:\n";
    for (auto f: obj.export_functions) {
        out << "    " << obj.strtable[f.name] << " " << obj.strtable[f.descriptor] << " " << f.index << "\n";
    }
    out << "  Exceptions:\n";
    for (auto e: obj.export_exceptions) {
        out << "    " << obj.strtable[e.name] << "\n";
    }
    out << "  Interfaces:\n";
    for (auto i: obj.export_interfaces) {
        out << "    " << obj.strtable[i.name] << "\n";
        for (auto m: i.method_descriptors) {
            out << "      " << obj.strtable[m.first] << " " << obj.strtable[m.second] << "\n";
        }
    }

    out << "Imports " << obj.imports.size() << ":\n";
    for (auto i: obj.imports) {
        out << "  " << obj.strtable[i.first] << "\n";
    }

    out << "Functions:\n";
    for (auto f: obj.functions) {
        out << "  " << obj.strtable[f.name] << " nest=" << f.nest << " params=" << f.params << " locals=" << f.locals << " entry=" << f.entry << "\n";
    }

    out << "Classes:\n";
    for (auto c: obj.classes) {
        out << "  " << obj.strtable[c.name] << "\n";
        for (auto i: c.interfaces) {
            out << "    ";
            for (auto m: i) {
                out << m << " ";
            }
            out << "\n";
        }
    }

    out << "Code:\n";
    while (index < obj.code.size()) {
        if (debug != nullptr) {
            auto line = debug->line_numbers.find(index);
            if (line != debug->line_numbers.end()) {
                out << line->second << " | " << debug->source_lines.at(line->second) << "\n";
            }
        }
        out << "  " << index << " ";
        if (debug != nullptr) {
            auto st = debug->stack_depth.find(index);
            if (st != debug->stack_depth.end()) {
                out << "(" << st->second << ") ";
            }
        }
        auto last_index = index;
        std::string s = disassemble_instruction(obj, index);
        out << s << "\n";
        if (index == last_index) {
            out << "disassembler: Unexpected opcode: " << static_cast<int>(obj.code[index]) << "\n";
            abort();
        }
    }
    if (not obj.exceptions.empty()) {
        out << "Exception table:\n";
        for (auto e: obj.exceptions) {
            out << "  " << e.start << "-" << e.end << " " << obj.strtable[e.excid] << " handler=" << e.handler << " stack_depth=" << e.stack_depth << "\n";
        }
    }
}

std::string disassemble_instruction(const Bytecode &obj, std::size_t &index)
{
    InstructionDisassembler id(obj, index);
    id.disassemble();
    return id.out.str();
}

void disassemble(const Bytecode::Bytes &obj, std::ostream &out, const DebugInfo *debug)
{
    Disassembler(out, obj, debug).disassemble();
}
