#ifndef OPCODE_H
#define OPCODE_H

enum Opcode {
    PUSHB,
    PUSHN,
    PUSHS,
    LOADGB,
    LOADGN,
    LOADGS,
    STOREGB,
    STOREGN,
    STOREGS,
    NEGN,
    ADDN,
    SUBN,
    MULN,
    DIVN,
    CALLP,
    CALLF,
    JUMP,
    JZ,
    RET,
};

#endif
