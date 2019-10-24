#ifndef OPCODE_H
#define OPCODE_H

#define FOREACH_OPCODE(OPCODE) \
    OPCODE(PUSHB)      /* push boolean immediate */\
    OPCODE(PUSHN)      /* push number immediate */\
    OPCODE(PUSHS)      /* push string immediate */\
    OPCODE(PUSHY)      /* push bytes immediate */\
    OPCODE(PUSHPG)     /* push pointer to global */\
    OPCODE(PUSHPPG)    /* push pointer to predefined global */\
    OPCODE(PUSHPMG)    /* push pointer to module global */\
    OPCODE(PUSHPL)     /* push pointer to local */\
    OPCODE(PUSHPOL)    /* push pointer to outer local */\
    OPCODE(PUSHI)      /* push 32-bit integer immediate */\
    OPCODE(LOADB)      /* load boolean */\
    OPCODE(LOADN)      /* load number */\
    OPCODE(LOADS)      /* load string */\
    OPCODE(LOADY)      /* load bytes */\
    OPCODE(LOADA)      /* load array */\
    OPCODE(LOADD)      /* load dictionary */\
    OPCODE(LOADP)      /* load pointer */\
    OPCODE(LOADJ)      /* load object */\
    OPCODE(LOADV)      /* load voidptr */\
    OPCODE(STOREB)     /* store boolean */\
    OPCODE(STOREN)     /* store number */\
    OPCODE(STORES)     /* store string */\
    OPCODE(STOREY)     /* store bytes */\
    OPCODE(STOREA)     /* store array */\
    OPCODE(STORED)     /* store dictionary */\
    OPCODE(STOREP)     /* store pointer */\
    OPCODE(STOREJ)     /* store object */\
    OPCODE(STOREV)     /* store voidptr */\
    OPCODE(NEGN)       /* negate number */\
    OPCODE(ADDN)       /* add number */\
    OPCODE(SUBN)       /* subtract number */\
    OPCODE(MULN)       /* multiply number */\
    OPCODE(DIVN)       /* divide number */\
    OPCODE(MODN)       /* modulo number */\
    OPCODE(EXPN)       /* exponentiate number */\
    OPCODE(EQB)        /* compare equal boolean */\
    OPCODE(NEB)        /* compare unequal boolean */\
    OPCODE(EQN)        /* compare equal number */\
    OPCODE(NEN)        /* compare unequal number */\
    OPCODE(LTN)        /* compare less number */\
    OPCODE(GTN)        /* compare greater number */\
    OPCODE(LEN)        /* compare less equal number */\
    OPCODE(GEN)        /* compare greater equal number */\
    OPCODE(EQS)        /* compare equal string */\
    OPCODE(NES)        /* compare unequal string */\
    OPCODE(LTS)        /* compare less string */\
    OPCODE(GTS)        /* compare greater string */\
    OPCODE(LES)        /* compare less equal string */\
    OPCODE(GES)        /* compare greater equal string */\
    OPCODE(EQY)        /* compare equal bytes */\
    OPCODE(NEY)        /* compare unequal bytes */\
    OPCODE(LTY)        /* compare less bytes */\
    OPCODE(GTY)        /* compare greater bytes */\
    OPCODE(LEY)        /* compare less equal bytes */\
    OPCODE(GEY)        /* compare greater equal bytes */\
    OPCODE(EQA)        /* compare equal array */\
    OPCODE(NEA)        /* compare unequal array */\
    OPCODE(EQD)        /* compare equal dictionary */\
    OPCODE(NED)        /* compare unequal dictionary */\
    OPCODE(EQP)        /* compare equal pointer */\
    OPCODE(NEP)        /* compare unequal pointer */\
    OPCODE(EQV)        /* compare equal voidptr */\
    OPCODE(NEV)        /* compare unequal voidptr */\
    OPCODE(ANDB)       /* and boolean */\
    OPCODE(ORB)        /* or boolean */\
    OPCODE(NOTB)       /* not boolean */\
    OPCODE(INDEXAR)    /* index array for read */\
    OPCODE(INDEXAW)    /* index array for write */\
    OPCODE(INDEXAV)    /* index array value */\
    OPCODE(INDEXAN)    /* index array value, no exception */\
    OPCODE(INDEXDR)    /* index dictionary for read */\
    OPCODE(INDEXDW)    /* index dictionary for write */\
    OPCODE(INDEXDV)    /* index dictionary value */\
    OPCODE(INA)        /* in array */\
    OPCODE(IND)        /* in dictionary */\
    OPCODE(CALLP)      /* call predefined */\
    OPCODE(CALLF)      /* call function */\
    OPCODE(CALLMF)     /* call module function */\
    OPCODE(CALLI)      /* call indirect */\
    OPCODE(JUMP)       /* unconditional jump */\
    OPCODE(JF)         /* jump if false */\
    OPCODE(JT)         /* jump if true */\
    OPCODE(JFCHAIN)    /* jump and drop next if false */\
    OPCODE(DUP)        /* duplicate */\
    OPCODE(DUPX1)      /* duplicate under second value */\
    OPCODE(DROP)       /* drop */\
    OPCODE(RET)        /* return */\
    OPCODE(CONSA)      /* construct array */\
    OPCODE(CONSD)      /* construct dictionary */\
    OPCODE(EXCEPT)     /* throw exception */\
    OPCODE(ALLOC)      /* allocate record */\
    OPCODE(PUSHNIL)    /* push nil pointer */\
    OPCODE(RESETC)     /* reset cell */\
    OPCODE(PUSHPEG)    /* push pointer to external global */\
    OPCODE(JUMPTBL)    /* jump table */\
    OPCODE(CALLX)      /* call extension */\
    OPCODE(SWAP)       /* swap two top stack elements */\
    OPCODE(DROPN)      /* drop element n */\
    OPCODE(PUSHFP)     /* push function pointer */\
    OPCODE(CALLV)      /* call virtual */\
    OPCODE(PUSHCI)     /* push class info */\

#define GENERATE_ENUM(ENUM)     ENUM,
#define GENERATE_NAME(STRING)   #STRING,

enum Opcode {
    FOREACH_OPCODE(GENERATE_ENUM)
};

static const char *sOpcode[] = {
    FOREACH_OPCODE(GENERATE_NAME)
};

#endif
