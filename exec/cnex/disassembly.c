#include "disassembly.h"

#include <stdio.h>
#include <string.h>

#include "bytecode.h"
#include "exec.h"
#include "module.h"
#include "nstring.h"
#include "opcode.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#define MAX_BUFFER      256

typedef struct tagTInstructionDisassembler {
    struct tagTBytecode *obj;
    unsigned int index;
    char out[MAX_BUFFER];
} TInstructionDisassembler;

void disasm_disassemble(TInstructionDisassembler *disasm);




const char *disasm_disassembleInstruction(TExecutor *exec)
{
    static TInstructionDisassembler disasm;

    disasm.obj = exec->module->bytecode;
    disasm.index = exec->ip;
    disasm.out[0] = '\0';
    disasm_disassemble(&disasm);
    return disasm.out;
}

void disasm_PUSHB(TInstructionDisassembler *disasm)
{
    BOOL val = disasm->obj->code[disasm->index+1] != 0;
    disasm->index += 2;
    snprintf(disasm->out, MAX_BUFFER, "PUSHB %d", val);
}

void disasm_PUSHN(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHN %s", TCSTR(disasm->obj->strings[val]));
}

void disasm_PUSHS(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHS \"%s\"", TCSTR(disasm->obj->strings[val]));
}

void disasm_PUSHY(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHY %d", val);
}

void disasm_PUSHPG(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHPG %d", addr);
}

void disasm_PUSHPPG(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t name = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHPPG %s", disasm->obj->strings[name]->data);
}

void disasm_PUSHPMG(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t module = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHPMG %d, %d", module, addr);
}

void disasm_PUSHPL(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHPL %d", addr);
}

void disasm_PUSHPOL(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t enclosing = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHPOL %d,%d", enclosing, addr);
}

void disasm_PUSHI(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t x = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHI %d", x);
}

void disasm_LOADB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADB");
    disasm->index++;
}

void disasm_LOADN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADN");
    disasm->index++;
}

void disasm_LOADS(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADS");
    disasm->index++;
}

void disasm_LOADY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADY");
    disasm->index++;
}

void disasm_LOADA(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADA");
    disasm->index++;
}

void disasm_LOADD(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADD");
    disasm->index++;
}

void disasm_LOADP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADP");
    disasm->index++;
}

void disasm_LOADJ(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADJ");
    disasm->index++;
}

void disasm_LOADV(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LOADV");
    disasm->index++;
}

void disasm_STOREB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREB");
    disasm->index++;
}

void disasm_STOREN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREN");
    disasm->index++;
}

void disasm_STORES(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STORES");
    disasm->index++;
}

void disasm_STOREY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREY");
    disasm->index++;
}

void disasm_STOREA(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREA");
    disasm->index++;
}

void disasm_STORED(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STORED");
    disasm->index++;
}

void disasm_STOREP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREP");
    disasm->index++;
}

void disasm_STOREJ(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREJ");
    disasm->index++;
}

void disasm_STOREV(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "STOREV");
    disasm->index++;
}

void disasm_NEGN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEGN");
    disasm->index++;
}

void disasm_ADDN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "ADDN");
    disasm->index++;
}

void disasm_SUBN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "SUBN");
    disasm->index++;
}

void disasm_MULN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "MULN");
    disasm->index++;
}

void disasm_DIVN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "DIVN");
    disasm->index++;
}

void disasm_MODN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "MODN");
    disasm->index++;
}

void disasm_EXPN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EXPN");
    disasm->index++;
}

void disasm_EQB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQB");
    disasm->index++;
}

void disasm_NEB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEB");
    disasm->index++;
}

void disasm_EQN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQN");
    disasm->index++;
}

void disasm_NEN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEN");
    disasm->index++;
}

void disasm_LTN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LTN");
    disasm->index++;
}

void disasm_GTN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "GTN");
    disasm->index++;
}

void disasm_LEN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LEN");
    disasm->index++;
}

void disasm_GEN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "GEN");
    disasm->index++;
}

void disasm_EQS(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQS");
    disasm->index++;
}

void disasm_NES(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NES");
    disasm->index++;
}

void disasm_LTS(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LTS");
    disasm->index++;
}

void disasm_GTS(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "GTS");
    disasm->index++;
}

void disasm_LES(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LES");
    disasm->index++;
}

void disasm_GES(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "GES");
    disasm->index++;
}

void disasm_EQY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQY");
    disasm->index++;
}

void disasm_NEY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEY");
    disasm->index++;
}

void disasm_LTY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LTY");
    disasm->index++;
}

void disasm_GTY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "GTY");
    disasm->index++;
}

void disasm_LEY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "LEY");
    disasm->index++;
}

void disasm_GEY(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "GEY");
    disasm->index++;
}

void disasm_EQA(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQA");
    disasm->index++;
}

void disasm_NEA(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEA");
    disasm->index++;
}

void disasm_EQD(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQD");
    disasm->index++;
}

void disasm_NED(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NED");
    disasm->index++;
}

void disasm_EQP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQP");
    disasm->index++;
}

void disasm_NEP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEP");
    disasm->index++;
}

void disasm_EQV(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "EQV");
    disasm->index++;
}

void disasm_NEV(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NEV");
    disasm->index++;
}

void disasm_ANDB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "ANDB");
    disasm->index++;
}

void disasm_ORB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "ORB");
    disasm->index++;
}

void disasm_NOTB(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "NOTB");
    disasm->index++;
}

void disasm_INDEXAR(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXAR");
    disasm->index++;
}

void disasm_INDEXAW(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXAW");
    disasm->index++;
}

void disasm_INDEXAV(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXAV");
    disasm->index++;
}

void disasm_INDEXAN(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXAN");
    disasm->index++;
}

void disasm_INDEXDR(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXDR");
    disasm->index++;
}

void disasm_INDEXDW(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXDW");
    disasm->index++;
}

void disasm_INDEXDV(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INDEXDV");
    disasm->index++;
}

void disasm_INA(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "INA");
    disasm->index++;
}

void disasm_IND(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "IND");
    disasm->index++;
}

void disasm_CALLP(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CALLP %d %s", val, (val < disasm->obj->strtablesize ? TCSTR(disasm->obj->strings[val]) : "(invalid)"));
}

void disasm_CALLF(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t findex = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CALLF %d %s", disasm->obj->functions[findex].name, TCSTR(disasm->obj->strings[disasm->obj->functions[findex].name]));
}

void disasm_CALLMF(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t mod = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    uint32_t func = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CALLMF %s,%s", TCSTR(disasm->obj->strings[mod]), TCSTR(disasm->obj->strings[func]));
}

void disasm_CALLI(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "CALLI");
    disasm->index++;
}

void disasm_JUMP(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "JUMP %d", addr);
}

void disasm_JF(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "JF %d", addr);
}

void disasm_JT(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t addr = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "JT %d", addr);
}

void disasm_DUP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "DUP");
    disasm->index++;
}

void disasm_DUPX1(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "DUPX1");
    disasm->index++;
}

void disasm_DROP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "DROP");
    disasm->index++;
}

void disasm_RET(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "RET");
    disasm->index++;
}

void disasm_CALLE(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CALLE %s", TCSTR(disasm->obj->strings[val]));
}

void disasm_CONSA(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CONSA %d", val);
}

void disasm_CONSD(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CONSD %d", val);
}

void disasm_EXCEPT(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "EXCEPT \"%s\"", TCSTR(disasm->obj->strings[val]));
}

void disasm_ALLOC(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "ALLOC %d", val);
}

void disasm_PUSHNIL(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "PUSHNIL");
    disasm->index++;
}

void disasm_RESETC(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "RESETC");
    disasm->index++;
}

void disasm_PUSHPEG(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHPEG \"%s\"", TCSTR(disasm->obj->strings[val]));
}

void disasm_JUMPTBL(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "JUMPTBL %d", val);
}

void disasm_CALLX(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t module = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    uint32_t name = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    uint32_t out_param_count = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CALLX %s.%s,%d", TCSTR(disasm->obj->strings[module]), TCSTR(disasm->obj->strings[name]), out_param_count);
}

void disasm_SWAP(TInstructionDisassembler *disasm)
{
    snprintf(disasm->out, MAX_BUFFER, "SWAP");
    disasm->index++;
}

void disasm_DROPN(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "DROPN %d", val);
}

void disasm_PUSHFP(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHFP %d", val);
}

void disasm_CALLV(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "CALLV %d", val);
}

void disasm_PUSHCI(TInstructionDisassembler *disasm)
{
    disasm->index++;
    uint32_t val = get_vint(disasm->obj->code, disasm->obj->codelen, &disasm->index);
    snprintf(disasm->out, MAX_BUFFER, "PUSHCI \"%s\"", TCSTR(disasm->obj->strings[val]));
}

void disasm_disassemble(TInstructionDisassembler *disasm)
{
    switch (disasm->obj->code[disasm->index]) {
        case PUSHB:   disasm_PUSHB(disasm); break;
        case PUSHN:   disasm_PUSHN(disasm); break;
        case PUSHS:   disasm_PUSHS(disasm); break;
        case PUSHY:   disasm_PUSHY(disasm); break;
        case PUSHPG:  disasm_PUSHPG(disasm); break;
        case PUSHPPG: disasm_PUSHPPG(disasm); break;
        case PUSHPMG: disasm_PUSHPMG(disasm); break;
        case PUSHPL:  disasm_PUSHPL(disasm); break;
        case PUSHPOL: disasm_PUSHPOL(disasm); break;
        case PUSHI:   disasm_PUSHI(disasm); break;
        case LOADB:   disasm_LOADB(disasm); break;
        case LOADN:   disasm_LOADN(disasm); break;
        case LOADS:   disasm_LOADS(disasm); break;
        case LOADY:   disasm_LOADY(disasm); break;
        case LOADA:   disasm_LOADA(disasm); break;
        case LOADD:   disasm_LOADD(disasm); break;
        case LOADP:   disasm_LOADP(disasm); break;
        case LOADJ:   disasm_LOADJ(disasm); break;
        case LOADV:   disasm_LOADV(disasm); break;
        case STOREB:  disasm_STOREB(disasm); break;
        case STOREN:  disasm_STOREN(disasm); break;
        case STORES:  disasm_STORES(disasm); break;
        case STOREY:  disasm_STOREY(disasm); break;
        case STOREA:  disasm_STOREA(disasm); break;
        case STORED:  disasm_STORED(disasm); break;
        case STOREP:  disasm_STOREP(disasm); break;
        case STOREJ:  disasm_STOREJ(disasm); break;
        case STOREV:  disasm_STOREV(disasm); break;
        case NEGN:    disasm_NEGN(disasm); break;
        case ADDN:    disasm_ADDN(disasm); break;
        case SUBN:    disasm_SUBN(disasm); break;
        case MULN:    disasm_MULN(disasm); break;
        case DIVN:    disasm_DIVN(disasm); break;
        case MODN:    disasm_MODN(disasm); break;
        case EXPN:    disasm_EXPN(disasm); break;
        case EQB:     disasm_EQB(disasm); break;
        case NEB:     disasm_NEB(disasm); break;
        case EQN:     disasm_EQN(disasm); break;
        case NEN:     disasm_NEN(disasm); break;
        case LTN:     disasm_LTN(disasm); break;
        case GTN:     disasm_GTN(disasm); break;
        case LEN:     disasm_LEN(disasm); break;
        case GEN:     disasm_GEN(disasm); break;
        case EQS:     disasm_EQS(disasm); break;
        case NES:     disasm_NES(disasm); break;
        case LTS:     disasm_LTS(disasm); break;
        case GTS:     disasm_GTS(disasm); break;
        case LES:     disasm_LES(disasm); break;
        case GES:     disasm_GES(disasm); break;
        case EQY:     disasm_EQY(disasm); break;
        case NEY:     disasm_NEY(disasm); break;
        case LTY:     disasm_LTY(disasm); break;
        case GTY:     disasm_GTY(disasm); break;
        case LEY:     disasm_LEY(disasm); break;
        case GEY:     disasm_GEY(disasm); break;
        case EQA:     disasm_EQA(disasm); break;
        case NEA:     disasm_NEA(disasm); break;
        case EQD:     disasm_EQD(disasm); break;
        case NED:     disasm_NED(disasm); break;
        case EQP:     disasm_EQP(disasm); break;
        case NEP:     disasm_NEP(disasm); break;
        case EQV:     disasm_EQV(disasm); break;
        case NEV:     disasm_NEV(disasm); break;
        case ANDB:    disasm_ANDB(disasm); break;
        case ORB:     disasm_ORB(disasm); break;
        case NOTB:    disasm_NOTB(disasm); break;
        case INDEXAR: disasm_INDEXAR(disasm); break;
        case INDEXAW: disasm_INDEXAW(disasm); break;
        case INDEXAV: disasm_INDEXAV(disasm); break;
        case INDEXAN: disasm_INDEXAN(disasm); break;
        case INDEXDR: disasm_INDEXDR(disasm); break;
        case INDEXDW: disasm_INDEXDW(disasm); break;
        case INDEXDV: disasm_INDEXDV(disasm); break;
        case INA:     disasm_INA(disasm); break;
        case IND:     disasm_IND(disasm); break;
        case CALLP:   disasm_CALLP(disasm); break;
        case CALLF:   disasm_CALLF(disasm); break;
        case CALLMF:  disasm_CALLMF(disasm); break;
        case CALLI:   disasm_CALLI(disasm); break;
        case JUMP:    disasm_JUMP(disasm); break;
        case JF:      disasm_JF(disasm); break;
        case JT:      disasm_JT(disasm); break;
        case DUP:     disasm_DUP(disasm); break;
        case DUPX1:   disasm_DUPX1(disasm); break;
        case DROP:    disasm_DROP(disasm); break;
        case RET:     disasm_RET(disasm); break;
        case CONSA:   disasm_CONSA(disasm); break;
        case CONSD:   disasm_CONSD(disasm); break;
        case EXCEPT:  disasm_EXCEPT(disasm); break;
        case ALLOC:   disasm_ALLOC(disasm); break;
        case PUSHNIL: disasm_PUSHNIL(disasm); break;
        case RESETC:  disasm_RESETC(disasm); break;
        case PUSHPEG: disasm_PUSHPEG(disasm); break;
        case JUMPTBL: disasm_JUMPTBL(disasm); break;
        case CALLX:   disasm_CALLX(disasm); break;
        case SWAP:    disasm_SWAP(disasm); break;
        case DROPN:   disasm_DROPN(disasm); break;
        case PUSHFP:  disasm_PUSHFP(disasm); break;
        case CALLV:   disasm_CALLV(disasm); break;
        case PUSHCI:  disasm_PUSHCI(disasm); break;
        default:
            snprintf(disasm->out, MAX_BUFFER, "Unknown opcode: %d", disasm->obj->code[disasm->index]);
            disasm->index++;
            break;
    }
}
