#include <stdio.h>

#include "error.h"
#include "exec.h"

/*!
 * \file exec.c
 * \brief Implémentation de exec.h. Execute une instruction.
 */

//! Mets à jour CC
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param res le dernier résultat
 */
static void set_cc(Machine *pmach, int res)
{
    if(res < 0)
        pmach->_cc = CC_N;

    else if(res == 0)
        pmach->_cc = CC_Z;

    else //res > 0
        pmach->_cc = CC_P;
}

//! Calcule l'adresse "réelle" d'une instruction en mode absolu/indexé
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return l'adresse absolu en mode absolu, l'adresse indexée sinon
 */
static unsigned address(Machine *pmach, Instruction instr)
{
    return instr.instr_generic._indexed ?
        pmach->_registers[instr.instr_indexed._rindex] +
        instr.instr_indexed._offset
        :
        instr.instr_absolute._address;
}

//! Appelle error si l'instruction est en mode immédiat
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 */
static void error_if_immediate(Machine *pmach, Instruction instr)
{
    if(instr.instr_generic._immediate)
        error(ERR_IMMEDIATE, pmach->_pc - 1);
}

/*!
 * Appelle error si l'on essayer d'accèder à une donnée en dehors
 * du segment de données
 */
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param addr l'adresse de la donnée à laquelle on veut accèder
 */
static void error_if_segdata(Machine *pmach, unsigned addr)
{
    if(addr >= pmach->_datasize)
        error(ERR_SEGDATA, pmach->_pc - 1);
}

//! Appelle error si l'on veut sortir de la pile
/*!
 * \param pmach la machine/programme en cours d'exécution
 */
static void error_if_segstack(Machine *pmach)
{
    if(pmach->_sp < 0 || pmach->_sp >= pmach->_datasize)
        error(ERR_SEGSTACK, pmach->_pc - 1);
}

//! Effectue un ILLOP sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return cette fonction ne retourne jamais, puisqu'error non plus
 */
static bool illop_func(Machine *pmach, Instruction instr)
{
    error(ERR_ILLEGAL, pmach->_pc - 1);
}

//! Effectue un NOP sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true
 */
static bool nop_func(Machine *pmach, Instruction instr)
{
    return true;
}

//! Effectue un LOAD sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool load_func(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(instr.instr_generic._immediate)
        pmach->_registers[r] = instr.instr_immediate._value;

    else
    {
        unsigned addr = address(pmach, instr);
        error_if_segdata(pmach, addr);
        pmach->_registers[r] = pmach->_data[addr];
    }

    set_cc(pmach, pmach->_registers[r]);
    return true;
}

//! Effectue un STORE sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool store_func(Machine *pmach, Instruction instr)
{
    error_if_immediate(pmach, instr);

    unsigned r = instr.instr_generic._regcond,
             addr = address(pmach, instr);

    error_if_segdata(pmach, addr);
    pmach->_data[addr] = pmach->_registers[r];

    return true;
}

//! Effectue une addition/soustraction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool add_sub_func(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(instr.instr_generic._immediate)
        if(instr.instr_generic._cop == ADD)
            pmach->_registers[r] += instr.instr_immediate._value;

        else
            pmach->_registers[r] -= instr.instr_immediate._value;

    else
    {
        unsigned addr = address(pmach, instr);
        error_if_segdata(pmach, addr);

        if(instr.instr_generic._cop == ADD)
            pmach->_registers[r] += pmach->_data[addr];

        else
            pmach->_registers[r] -= pmach->_data[addr];
    }

    set_cc(pmach, pmach->_registers[r]);
    return true;
}

//! Retourne vrai, si l'on doit sauter false sinon
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si on doit sauter, false sinon, ne retourne pas si erreur
 */
static bool should_jump(Machine *pmach, Instruction instr)
{
    if((instr.instr_generic._regcond != NC && pmach->_cc == CC_U) ||
            instr.instr_generic._regcond > LAST_CONDITION)
        error(ERR_CONDITION, pmach->_pc - 1);

    switch(instr.instr_generic._regcond)
    {
        case NC:
            return true;

        case EQ:
            return pmach->_cc == CC_Z;

        case NE:
            return pmach->_cc != CC_Z;

        case GT:
            return pmach->_cc == CC_P;

        case GE:
            return pmach->_cc == CC_P || pmach->_cc == CC_Z;

        case LT:
            return pmach->_cc == CC_N;

        case LE:
            return pmach->_cc == CC_N || pmach->_cc == CC_Z;

        default:
            error(ERR_CONDITION, pmach->_pc - 1);
    }
}

//! Effectue un BRANCH sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool branch_func(Machine *pmach, Instruction instr)
{

    error_if_immediate(pmach, instr);

    if(should_jump(pmach, instr))
        pmach->_pc = address(pmach, instr);

    return true;
}

//! Effectue un CALL sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool call_func(Machine *pmach, Instruction instr)
{
    error_if_immediate(pmach, instr);

    if(should_jump(pmach, instr))
    {
        error_if_segstack(pmach);
        pmach->_data[pmach->_sp] = pmach->_pc;
        pmach->_pc = address(pmach, instr);
        --pmach->_sp;
    }

    return true;
}

//! Effectue un RET sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool ret_func(Machine *pmach, Instruction instr)
{
    ++pmach->_sp;
    error_if_segstack(pmach);
    pmach->_pc = pmach->_data[pmach->_sp];
    return true;
}

//! Effectue un PUSH sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool push_func(Machine *pmach, Instruction instr)
{
    error_if_segstack(pmach);

    if(pmach->_sp < pmach->_dataend)
        warning(WARN_PUSH_STATIC, pmach->_pc - 1);

    if(instr.instr_generic._immediate)
        pmach->_data[pmach->_sp] = instr.instr_immediate._value;

    else
    {
        unsigned addr = address(pmach, instr);
        error_if_segdata(pmach, addr);
        pmach->_data[pmach->_sp] = pmach->_data[addr];
    }

    --pmach->_sp;
    return true;
}

//! Effectue un POP sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
static bool pop_func(Machine *pmach, Instruction instr)
{
    ++pmach->_sp;
    error_if_immediate(pmach, instr);
    error_if_segstack(pmach);

    unsigned addr = address(pmach, instr);
    error_if_segdata(pmach, addr);

    if(addr < pmach->_datasize)
        pmach->_data[addr] = pmach->_data[pmach->_sp];

    return true;
}

//! Effectue un HALT sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return false
 */
static bool halt_func(Machine *pmach, Instruction instr)
{
    warning(WARN_HALT, pmach->_pc - 1);
    return false;
}

bool decode_execute(Machine *pmach, Instruction instr)
{
    static bool (*funcs[])(Machine *, Instruction) =
    {
        illop_func,
        nop_func,
        load_func,
        store_func,
        add_sub_func,
        add_sub_func,
        branch_func,
        call_func,
        ret_func,
        push_func,
        pop_func,
        halt_func,
    };

    if(instr.instr_generic._cop > LAST_COP)
        error(ERR_ILLEGAL, pmach->_pc - 1);

    return funcs[instr.instr_generic._cop](pmach, instr);
}

void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr)
{
    printf("TRACE: %s: 0x%04x: ", msg, addr);
    print_instruction(instr, addr);
    printf("\n");
}

