#include <stdio.h>

#include "instruction.h"

void print_instruction(Instruction instr, unsigned addr)
{
    char *ops[] =
        {
        "ILLOP",
        "NOP",
        "LOAD",
        "STORE",
        "ADD",
        "SUB",
        "BRANCH",
        "CALL",
        "RET",
        "PUSH",
        "POP",
        "HALT",
        };
    Code_Op op = instr.instr_generic._cop;

    printf("%s ", ops[op]);

    if(op == RET || op == HALT || op == NOP || op == ILLOP)
    {
        return;
    }

    else if(op == BRANCH || op == CALL)
    {
        char *conds[] =
            {
            "NC",
            "EQ",
            "NE",
            "GT",
            "GE",
            "LT",
            "LE",
            };

        printf("%s, ", conds[instr.instr_generic._regcond]);
    }

    else if(op != PUSH)
    {
        printf("R%02u, ", instr.instr_generic._regcond);
    }

    if(instr.instr_generic._immediate)
    {
        printf("#%u", instr.instr_immediate._value);
    }

    else if(instr.instr_generic._indexed)
    {
        printf("%d[R%02u]", instr.instr_indexed._offset, instr.instr_indexed._rindex);
    }

    else
    {
        printf("@0x%04x", instr.instr_absolute._address);
    }
}

