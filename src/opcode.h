#ifndef OPCODE_H
#define OPCODE_H

enum Opcode {
    ENTER,      // enter function scope
    LEAVE,      // leave function scope
    PUSHB,      // push boolean immediate
    PUSHN,      // push number immediate
    PUSHS,      // push string immediate
    PUSHPG,     // push pointer to global
    PUSHPL,     // push pointer to local
    PUSHPOL,    // push pointer to outer local
    LOADB,      // load boolean
    LOADN,      // load number
    LOADS,      // load string
    LOADA,      // load array
    LOADD,      // load dictionary
    LOADP,      // load pointer
    STOREB,     // store boolean
    STOREN,     // store number
    STORES,     // store string
    STOREA,     // store array
    STORED,     // store dictionary
    STOREP,     // store pointer
    NEGN,       // negate number
    ADDN,       // add number
    SUBN,       // subtract number
    MULN,       // multiply number
    DIVN,       // divide number
    MODN,       // modulo number
    EXPN,       // exponentiate number
    EQB,        // compare equal boolean
    NEB,        // compare unequal boolean
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
    EQA,        // compare equal array
    NEA,        // compare unequal array
    EQD,        // compare equal dictionary
    NED,        // compare unequal dictionary
    EQP,        // compare equal pointer
    NEP,        // compare unequal pointer
    ANDB,       // and boolean
    ORB,        // or boolean
    NOTB,       // not boolean
    INDEXAR,    // index array for read
    INDEXAW,    // index array for write
    INDEXAV,    // index array value
    INDEXDR,    // index dictionary for read
    INDEXDW,    // index dictionary for write
    INDEXDV,    // index dictionary value
    INA,        // in array
    IND,        // in dictionary
    CALLP,      // call predefined
    CALLF,      // call function
    JUMP,       // unconditional jump
    JF,         // jump if false
    JT,         // jump if true
    DUP,        // duplicate
    DROP,       // drop
    RET,        // return
    CALLE,      // call external
    CONSA,      // construct array
    CONSD,      // construct dictionary
    EXCEPT,     // throw exception
    ALLOC,      // allocate record
    PUSHNIL,    // push nil pointer
};

#endif
