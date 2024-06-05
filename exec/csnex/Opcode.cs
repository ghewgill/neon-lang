namespace csnex
{
    public enum Opcode: byte
    {
        PUSHB,      // push boolean immediate
        PUSHN,      // push number immediate
        PUSHS,      // push string immediate
        PUSHY,      // push bytes immediate
        PUSHPG,     // push pointer to global
        PUSHPPG,    // push pointer to predefined global
        PUSHPMG,    // push pointer to module global
        PUSHPL,     // push pointer to local
        PUSHPOL,    // push pointer to outer local
        PUSHI,      // push 32-bit integer immediate
        LOADB,      // load boolean
        LOADN,      // load number
        LOADS,      // load string
        LOADY,      // load bytes
        LOADA,      // load array
        LOADD,      // load dictionary
        LOADP,      // load pointer
        LOADJ,      // load object
        STOREB,     // store boolean
        STOREN,     // store number
        STORES,     // store string
        STOREY,     // store bytes
        STOREA,     // store array
        STORED,     // store dictionary
        STOREP,     // store pointer
        STOREJ,     // store object
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
        EQY,        // compare equal bytes
        NEY,        // compare unequal bytes
        LTY,        // compare less bytes
        GTY,        // compare greater bytes
        LEY,        // compare less equal bytes
        GEY,        // compare greater equal bytes
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
        INDEXAN,    // index array value, no exception
        INDEXDR,    // index dictionary for read
        INDEXDW,    // index dictionary for write
        INDEXDV,    // index dictionary value
        INA,        // in array
        IND,        // in dictionary
        CALLP,      // call predefined
        CALLF,      // call function
        CALLMF,     // call module function
        CALLI,      // call indirect
        JUMP,       // unconditional jump
        JF,         // jump if false
        JT,         // jump if true
        DUP,        // duplicate
        DUPX1,      // duplicate under second value
        DROP,       // drop
        RET,        // return
        CONSA,      // construct array
        CONSD,      // construct dictionary
        EXCEPT,     // throw exception
        ALLOC,      // allocate record
        PUSHNIL,    // push nil pointer
        PUSHPEG,    // push pointer to external global
        JUMPTBL,    // jump table
        CALLX,      // call extension
        SWAP,       // swap two top stack elements
        DROPN,      // drop element n
        PUSHFP,     // push function pointer
        CALLV,      // call virtual
        PUSHCI,     // push class info
        PUSHMFP,    // push modle function pointer
    }
}
