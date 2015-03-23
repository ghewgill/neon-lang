#include "disassembler.h"

#include <iso646.h>
#include <ostream>
#include <stdlib.h>
#include <string>

#include "bytecode.h"
#include "debuginfo.h"
#include "number.h"
#include "opcode.h"
#include "util.h"

class Disassembler {
public:
    Disassembler(std::ostream &out, const Bytecode::Bytes &obj, const DebugInfo *debug);
    void disassemble();
private:
    std::ostream &out;
    const Bytecode obj;
    const DebugInfo *debug;
    Bytecode::Bytes::size_type index;

    void disasm_ENTER();
    void disasm_LEAVE();
    void disasm_PUSHB();
    void disasm_PUSHN();
    void disasm_PUSHS();
    void disasm_PUSHPG();
    void disasm_PUSHPMG();
    void disasm_PUSHPL();
    void disasm_PUSHPOL();
    void disasm_LOADB();
    void disasm_LOADN();
    void disasm_LOADS();
    void disasm_LOADA();
    void disasm_LOADD();
    void disasm_LOADP();
    void disasm_STOREB();
    void disasm_STOREN();
    void disasm_STORES();
    void disasm_STOREA();
    void disasm_STORED();
    void disasm_STOREP();
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
    void disasm_EQA();
    void disasm_NEA();
    void disasm_EQD();
    void disasm_NED();
    void disasm_EQP();
    void disasm_NEP();
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
    void disasm_CLREXC();
    void disasm_ALLOC();
    void disasm_PUSHNIL();

    std::string decode_value(const std::string &type, const Bytecode::Bytes &value);
private:
    Disassembler(const Disassembler &);
    Disassembler &operator=(const Disassembler &);
};

Disassembler::Disassembler(std::ostream &out, const Bytecode::Bytes &obj, const DebugInfo *debug)
  : out(out), obj(obj), debug(debug), index(0)
{
}

void Disassembler::disasm_ENTER()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
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
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHS \"" << obj.strtable[val] << "\"\n";
}

void Disassembler::disasm_PUSHPG()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHPG " << addr << "\n";
}

void Disassembler::disasm_PUSHPMG()
{
    index++;
    uint32_t module = (obj.code[index] << 24) | (obj.code[index+1] << 16) | (obj.code[index+2] << 8) | obj.code[index+3];
    index += 4;
    uint32_t addr = (obj.code[index] << 24) | (obj.code[index+1] << 16) | (obj.code[index+2] << 8) | obj.code[index+3];
    index += 4;
    out << "PUSHPMG " << module << "," << addr << "\n";
}

void Disassembler::disasm_PUSHPL()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "PUSHPL " << addr << "\n";
}

void Disassembler::disasm_PUSHPOL()
{
    index++;
    uint32_t enclosing = (obj.code[index] << 24) | (obj.code[index+1] << 16) | (obj.code[index+2] << 8) | obj.code[index+3];
    index += 4;
    uint32_t addr = (obj.code[index] << 24) | (obj.code[index+1] << 16) | (obj.code[index+2] << 8) | obj.code[index+3];
    index += 4;
    out << "PUSHPL " << enclosing << "," << addr << "\n";
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

void Disassembler::disasm_LOADA()
{
    out << "LOADA\n";
    index++;
}

void Disassembler::disasm_LOADD()
{
    out << "LOADD\n";
    index++;
}

void Disassembler::disasm_LOADP()
{
    out << "LOADP\n";
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

void Disassembler::disasm_STOREA()
{
    out << "STOREA\n";
    index++;
}

void Disassembler::disasm_STORED()
{
    out << "STORED\n";
    index++;
}

void Disassembler::disasm_STOREP()
{
    out << "STOREP\n";
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

void Disassembler::disasm_MODN()
{
    out << "MODN\n";
    index++;
}

void Disassembler::disasm_EXPN()
{
    out << "EXPN\n";
    index++;
}

void Disassembler::disasm_EQB()
{
    out << "EQB\n";
    index++;
}

void Disassembler::disasm_NEB()
{
    out << "NEB\n";
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

void Disassembler::disasm_EQA()
{
    out << "EQA\n";
    index++;
}

void Disassembler::disasm_NEA()
{
    out << "NEA\n";
    index++;
}

void Disassembler::disasm_EQD()
{
    out << "EQD\n";
    index++;
}

void Disassembler::disasm_NED()
{
    out << "NED\n";
    index++;
}

void Disassembler::disasm_EQP()
{
    out << "EQP\n";
    index++;
}

void Disassembler::disasm_NEP()
{
    out << "NEP\n";
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

void Disassembler::disasm_INDEXAR()
{
    out << "INDEXAR\n";
    index++;
}

void Disassembler::disasm_INDEXAW()
{
    out << "INDEXAW\n";
    index++;
}

void Disassembler::disasm_INDEXAV()
{
    out << "INDEXAV\n";
    index++;
}

void Disassembler::disasm_INDEXAN()
{
    out << "INDEXAN\n";
    index++;
}

void Disassembler::disasm_INDEXDR()
{
    out << "INDEXDR\n";
    index++;
}

void Disassembler::disasm_INDEXDW()
{
    out << "INDEXDW\n";
    index++;
}

void Disassembler::disasm_INDEXDV()
{
    out << "INDEXDV\n";
    index++;
}

void Disassembler::disasm_INA()
{
    out << "INA\n";
    index++;
}

void Disassembler::disasm_IND()
{
    out << "IND\n";
    index++;
}

void Disassembler::disasm_CALLP()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CALLP " << val << " " << (val < obj.strtable.size() ? obj.strtable[val] : "(invalid)") << "\n";
}

void Disassembler::disasm_CALLF()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CALLF " << addr << "\n";
}

void Disassembler::disasm_CALLMF()
{
    index++;
    uint32_t mod = (obj.code[index] << 24) | (obj.code[index+1] << 16) | (obj.code[index+2] << 8) | obj.code[index+3];
    index += 4;
    uint32_t addr = (obj.code[index] << 24) | (obj.code[index+1] << 16) | (obj.code[index+2] << 8) | obj.code[index+3];
    index += 4;
    out << "CALLMF " << mod << "," << addr << "\n";
}

void Disassembler::disasm_JUMP()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JUMP " << addr << "\n";
}

void Disassembler::disasm_JF()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JF " << addr << "\n";
}

void Disassembler::disasm_JT()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JT " << addr << "\n";
}

void Disassembler::disasm_JFCHAIN()
{
    uint32_t addr = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "JFCHAIN " << addr << "\n";
}

void Disassembler::disasm_DUP()
{
    out << "DUP\n";
    index++;
}

void Disassembler::disasm_DUPX1()
{
    out << "DUPX1\n";
    index++;
}

void Disassembler::disasm_DROP()
{
    out << "DROP\n";
    index++;
}

void Disassembler::disasm_RET()
{
    out << "RET\n";
    index++;
}

void Disassembler::disasm_CALLE()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CALLE " << obj.strtable[val] << "\n";
}

void Disassembler::disasm_CONSA()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CONSA " << val << "\n";
}

void Disassembler::disasm_CONSD()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "CONSD " << val << "\n";
}

void Disassembler::disasm_EXCEPT()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "EXCEPT \"" << obj.strtable[val] << "\"\n";
}

void Disassembler::disasm_CLREXC()
{
    out << "CLREXC\n";
    index++;
}

void Disassembler::disasm_ALLOC()
{
    uint32_t val = (obj.code[index+1] << 24) | (obj.code[index+2] << 16) | (obj.code[index+3] << 8) | obj.code[index+4];
    index += 5;
    out << "ALLOC " << val << "\n";
}

void Disassembler::disasm_PUSHNIL()
{
    out << "PUSHNIL\n";
    index++;
}

std::string Disassembler::decode_value(const std::string &type, const Bytecode::Bytes &value)
{
    switch (type.at(0)) {
        case 'B': {
            return value.at(0) != 0 ? "TRUE" : "FALSE";
        }
        case 'N': {
            Number x = *reinterpret_cast<const Number *>(&value.at(0));
            return number_to_string(x);
        }
        case 'S': {
            uint32_t len = (value.at(0) << 24) | (value.at(1) << 16) | (value.at(1) << 8) | value.at(0);
            return std::string(&value.at(4), &value.at(4)+len);
        }
        default:
            internal_error("TODO unimplemented type in decode_value");
    }
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

    out << "Exports:\n";
    out << "  Types:\n";
    for (auto t: obj.types) {
        out << "    " << obj.strtable[t.name] << " " << obj.strtable[t.descriptor] << "\n";
    }
    out << "  Constants:\n";
    for (auto c: obj.constants) {
        out << "    " << obj.strtable[c.name] << " " << obj.strtable[c.type] << " " << decode_value(obj.strtable[c.type], c.value) << "\n";
    }
    out << "  Variables:\n";
    for (auto v: obj.variables) {
        out << "    " << obj.strtable[v.name] << " " << obj.strtable[v.type] << " " << v.index << "\n";
    }
    out << "  Functions:\n";
    for (auto f: obj.functions) {
        out << "    " << obj.strtable[f.name] << " " << obj.strtable[f.descriptor] << " " << f.entry << "\n";
    }

    out << "Imports " << obj.imports.size() << ":\n";
    for (auto i: obj.imports) {
        out << "  " << obj.strtable[i.first] << "\n";
    }

    while (index < obj.code.size()) {
        if (debug != nullptr) {
            auto line = debug->line_numbers.find(index);
            if (line != debug->line_numbers.end()) {
                out << line->second << " | " << debug->source_lines.at(line->second) << "\n";
            }
        }
        out << "  " << index << " ";
        auto last_index = index;
        switch (static_cast<Opcode>(obj.code[index])) {
            case ENTER:   disasm_ENTER(); break;
            case LEAVE:   disasm_LEAVE(); break;
            case PUSHB:   disasm_PUSHB(); break;
            case PUSHN:   disasm_PUSHN(); break;
            case PUSHS:   disasm_PUSHS(); break;
            case PUSHPG:  disasm_PUSHPG(); break;
            case PUSHPMG: disasm_PUSHPMG(); break;
            case PUSHPL:  disasm_PUSHPL(); break;
            case PUSHPOL: disasm_PUSHPOL(); break;
            case LOADB:   disasm_LOADB(); break;
            case LOADN:   disasm_LOADN(); break;
            case LOADS:   disasm_LOADS(); break;
            case LOADA:   disasm_LOADA(); break;
            case LOADD:   disasm_LOADD(); break;
            case LOADP:   disasm_LOADP(); break;
            case STOREB:  disasm_STOREB(); break;
            case STOREN:  disasm_STOREN(); break;
            case STORES:  disasm_STORES(); break;
            case STOREA:  disasm_STOREA(); break;
            case STORED:  disasm_STORED(); break;
            case STOREP:  disasm_STOREP(); break;
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
            case EQA:     disasm_EQA(); break;
            case NEA:     disasm_NEA(); break;
            case EQD:     disasm_EQD(); break;
            case NED:     disasm_NED(); break;
            case EQP:     disasm_EQP(); break;
            case NEP:     disasm_NEP(); break;
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
            case CLREXC:  disasm_CLREXC(); break;
            case ALLOC:   disasm_ALLOC(); break;
            case PUSHNIL: disasm_PUSHNIL(); break;
        }
        if (index == last_index) {
            out << "disassembler: Unexpected opcode: " << static_cast<int>(obj.code[index]) << "\n";
            abort();
        }
    }
    if (not obj.exceptions.empty()) {
        out << "Exception table:\n";
        for (auto e: obj.exceptions) {
            out << "  " << e.start << "-" << e.end << " " << obj.strtable[e.excid] << " " << e.handler << "\n";
        }
    }
}

void disassemble(const Bytecode::Bytes &obj, std::ostream &out, const DebugInfo *debug)
{
    Disassembler(out, obj, debug).disassemble();
}
