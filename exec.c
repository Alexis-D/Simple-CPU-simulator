#include <stdio.h>

#include "instruction.h"
#include "exec.h"

bool decode_execute(Machine *pmach, Instruction instr) {
    return true;
}

void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr) {
    printf("TRACE: %s: ", msg);
    print_instruction(instr, addr);
    printf("\n");
}

