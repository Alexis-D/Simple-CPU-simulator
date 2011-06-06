#include <stdio.h>

#include "error.h"
#include "exec.h"
#include "instruction.h"

void set_cc(Machine *pmach, int res)
{
    if(res < 0)
    {
        pmach->_cc = CC_N;
    }

    else if(res == 0)
    {
        pmach->_cc = CC_Z;
    }

    else
    {
        //res > 0
        pmach->_cc = CC_P;
    }

    //CC_U???
}

unsigned address(Machine *pmach, Instruction instr)
{
    return instr.instr_generic._indexed ?
                pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset :
                instr.instr_absolute._address;
}

bool illop_func(Machine *pmach, Instruction instr)
{
    //free
    error(ERR_ILLEGAL, pmach->_pc - 1);
    return true; //never reached
}

bool nop_func(Machine *pmach, Instruction instr)
{
    return true;
}

bool load_func(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);

        //printf("R%d\n", instr.instr_indexed._rindex);
        //printf("= %d\n", pmach->_registers[instr.instr_indexed._rindex]);
        //printf("off = %d\n", instr.instr_indexed._offset);
        //printf("-> %d\n", addr);

        if(addr < pmach->_datasize)
        {
            pmach->_registers[r] = pmach->_data[addr];
        }

        else
        {
            //free
            error(ERR_SEGDATA, pmach->_pc - 1);
        }
    }

    else
    {
        pmach->_registers[r] = instr.instr_immediate._value;
    }

    set_cc(pmach, pmach->_registers[r]);
    return true;
}

bool store_func(Machine *pmach, Instruction instr)
{
    if(instr.instr_generic._immediate)
    {
        //free
        error(ERR_IMMEDIATE, pmach->_pc - 1);
    }

    unsigned r = instr.instr_generic._regcond;
    unsigned addr = address(pmach, instr);

    if(addr < pmach->_datasize)
    {
        pmach->_data[addr] = pmach->_registers[r];
    }

    else
    {
        //free
        error(ERR_SEGDATA, pmach->_pc - 1);
    }

    return true;
}

bool add_func(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);

        if(addr < pmach->_datasize)
        {
            pmach->_registers[r] += pmach->_data[addr];
        }

        else
        {
            //free
            error(ERR_SEGDATA, pmach->_pc - 1);
        }
    }

    else
    {
        pmach->_registers[r] += instr.instr_immediate._value;
    }

    set_cc(pmach, pmach->_registers[r]);
    return true;
}

bool sub_func(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);

        if(addr < pmach->_datasize)
        {
            pmach->_registers[r] -= pmach->_data[addr];
        }

        else
        {
            //free
            error(ERR_SEGDATA, pmach->_pc - 1);
        }
    }

    else
    {
        pmach->_registers[r] -= instr.instr_immediate._value;
    }

    set_cc(pmach, pmach->_registers[r]);
    return true;

}

bool branch_func(Machine *pmach, Instruction instr)
{
    // FAIL
    if(instr.instr_generic._immediate)
    {
        error(ERR_IMMEDIATE, pmach->_pc - 1);
    }

    Condition regcond = instr.instr_generic._regcond;
    bool jump = false;

    switch(regcond)
    {
        case NC:
            jump = true;
            break;

        case EQ:
            jump = pmach->_cc == CC_Z;
            break;

        case NE:
            jump = pmach->_cc != CC_Z;
            break;

        case GT:
            jump = pmach->_cc == CC_P;
            break;

        case GE:
            jump = pmach->_cc == CC_P || pmach->_cc == CC_Z;
            break;

        case LT:
            jump = pmach->_cc == CC_N;
            break;

        case LE:
            jump = pmach->_cc == CC_N || pmach->_cc == CC_Z;
            break;
    }

    if(jump)
    {
        pmach->_pc = address(pmach, instr);
    }

    return true;
}

bool call_func(Machine *pmach, Instruction instr)
{
    if(instr.instr_generic._immediate)
    {
        error(ERR_IMMEDIATE, pmach->_pc - 1);
    }

    //if(pmach->_cc)
    //{
        pmach->_data[pmach->_sp] = pmach->_pc;
        --pmach->_sp;
        pmach->_pc = address(pmach, instr);
    //}

    return true;
}

bool ret_func(Machine *pmach, Instruction instr)
{
    ++pmach->_sp;
    pmach->_pc = pmach->_data[pmach->_sp];
    return true;
}

bool push_func(Machine *pmach, Instruction instr)
{
    if(pmach->_sp < 0)
    {
        //free
        error(ERR_SEGSTACK, pmach->_pc - 1);
    }

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);

        if(addr < pmach->_datasize)
        {
            pmach->_data[pmach->_sp] = pmach->_data[addr];
        }

        else
        {
            error(ERR_SEGDATA, pmach->_pc - 1);
        }
    }

    else
    {
        pmach->_data[pmach->_sp] = instr.instr_immediate._value;
    }

    --pmach->_sp;
    return true;
}

bool pop_func(Machine *pmach, Instruction instr)
{
    ++pmach->_sp;

    if(pmach->_sp >= pmach->_datasize)
    {
        error(ERR_SEGSTACK, pmach->_pc - 1);
    }

    if(instr.instr_generic._immediate)
    {
        error(ERR_IMMEDIATE, pmach->_pc - 1);
    }

    unsigned addr = address(pmach, instr);

    if(addr < pmach->_datasize)
    {
        pmach->_data[addr] = pmach->_data[pmach->_sp];
    }

    return true;
}

bool halt_func(Machine *pmach, Instruction instr)
{
    //pas de free ici
    warning(WARN_HALT, pmach->_pc - 1);
    return false;
}

bool decode_execute(Machine *pmach, Instruction instr) {
    bool (*func[])(Machine *, Instruction) =
        {
        illop_func,
        nop_func,
        load_func,
        store_func,
        add_func,
        sub_func,
        branch_func,
        call_func,
        ret_func,
        push_func,
        pop_func,
        halt_func
        };

    return func[instr.instr_generic._cop](pmach, instr);
}

void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr)
{
    printf("TRACE: %s: 0x%04x: ", msg, addr);
    print_instruction(instr, addr);
    printf("\n");
}

