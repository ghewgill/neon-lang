#ifndef OPCODE_H
#define OPCODE_H

enum Opcode {
    ENTER,      // enter function scope
    LEAVE,      // leave function scope
    PUSHB,      // push boolean immediate
    PUSHN,      // push number immediate
    PUSHS,      // push string immediate
    PUSHAG,     // push address of global
    PUSHAL,     // push address of local
    LOADB,      // load boolean
    LOADN,      // load number
    LOADS,      // load string
    STOREB,     // store boolean
    STOREN,     // store number
    STORES,     // store string
    NEGN,       // negate number
    ADDN,       // add number
    SUBN,       // subtract number
    MULN,       // multiply number
    DIVN,       // divide number
    EXPN,       // exponentiate number
    EQN,        // compare equal number
    NEN,        // compare unequal number
    LTN,        // compare less number
    GTN,        // compare greater number
    LEN,        // compare less equal number
    GEN,        // compare greater equal number
    EQS,        // compare equal string
    NES,        // compare unequal string
    LTS,        // compare less string
    GTS,        // compare greater string
    LES,        // compare less equal string
    GES,        // compare greater equal string
    ANDB,       // and boolean
    ORB,        // or boolean
    NOTB,       // not boolean
    INDEXA,     // index array
    INDEXD,     // index dictionary
    CALLP,      // call predefined
    CALLF,      // call function
    JUMP,       // unconditional jump
    JF,         // jump if false
    RET,        // return
};

#endif
