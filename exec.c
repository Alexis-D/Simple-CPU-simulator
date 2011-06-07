#include <stdio.h>

#include "error.h"
#include "exec.h"
#include "instruction.h"

/*!
 * \file exec.c
 * \brief Implémentation de exec.h. Execute une intstruction.
 */

//! Mets à jour CC
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param res le dernier résultat
 */
void set_cc(Machine *pmach, int res)
{
    if(res < 0)
        pmach->_cc = CC_N;

    else if(res == 0)
        pmach->_cc = CC_Z;

    else //res > 0
        pmach->_cc = CC_P;
}

//! Libère les segments de mémoires alloués dynamiquement
/*!
 * \param pmach la machine/programme en cours d'exécution
 */
void free_segments(Machine *pmach)
{
    free(pmach->_text);
    free(pmach->_data);
}

//! Calcule l'adresse "réelle" d'une instruction en mode absolu/indexé
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return l'adresse absolu en mode absolu, l'adresse indexée sinon
 */
unsigned address(Machine *pmach, Instruction instr)
{
    return instr.instr_generic._indexed ?
                pmach->_registers[instr.instr_indexed._rindex] + instr.instr_indexed._offset :
                instr.instr_absolute._address;
}

//! Appelle error si l'instruction est en mode immédiat
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 */
void error_if_immediate(Machine *pmach, Instruction instr)
{
    if(instr.instr_generic._immediate)
    {
        free_segments(pmach);
        error(ERR_IMMEDIATE, pmach->_pc - 1);
    }
}

//! Appelle error si l'on essayer d'accèder à une donnée en dehors du segment de données
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param addr l'adresse de la donnée à laquelle on veut accèder
 */
void error_if_segdata(Machine *pmach, unsigned addr)
{
    if(addr >= pmach->_datasize)
    {
        free_segments(pmach);
        error(ERR_SEGDATA, pmach->_pc - 1);
    }
}

//! Appelle error si l'on veut sortir de la pile
/*!
 * \param pmach la machine/programme en cours d'exécution
 */
void error_if_segstack(Machine *pmach)
{
    if(pmach->_sp < 0 || pmach->_sp >= pmach->_datasize)
    {
        free_segments(pmach);
        error(ERR_SEGSTACK, pmach->_pc - 1);
    }
}

//! Effectue un ILLOP sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return cette fonction ne retourne jamais, puisqu'error non plus
 */
bool illop_func(Machine *pmach, Instruction instr)
{
    free_segments(pmach);
    error(ERR_ILLEGAL, pmach->_pc - 1);
    //never reached
}

//! Effectue un NOP sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true
 */
bool nop_func(Machine *pmach, Instruction instr)
{
    return true;
}

//! Effectue un LOAD sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool load_func(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);
        error_if_segdata(pmach, addr);
        pmach->_registers[r] = pmach->_data[addr];
    }

    else
        pmach->_registers[r] = instr.instr_immediate._value;

    set_cc(pmach, pmach->_registers[r]);
    return true;
}

//! Effectue un STORE sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool store_func(Machine *pmach, Instruction instr)
{
    error_if_immediate(pmach, instr);

    unsigned r = instr.instr_generic._regcond;
    unsigned addr = address(pmach, instr);

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
bool add_sub(Machine *pmach, Instruction instr)
{
    unsigned r = instr.instr_generic._regcond;

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);
        error_if_segdata(pmach, addr);

        if(instr.instr_generic._cop == ADD)
            pmach->_registers[r] += pmach->_data[addr];

        else
            pmach->_registers[r] -= pmach->_data[addr];
    }

    else
        if(instr.instr_generic._cop == ADD)
            pmach->_registers[r] += instr.instr_immediate._value;

        else
            pmach->_registers[r] -= instr.instr_immediate._value;

    set_cc(pmach, pmach->_registers[r]);
    return true;
}

//! Effectue un ADD sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool add_func(Machine *pmach, Instruction instr)
{
    return add_sub(pmach, instr);
}

//! Effectue un SUB sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool sub_func(Machine *pmach, Instruction instr)
{
    return add_sub(pmach, instr);
}

//! Retourne vrai, si l'on doit sauter false sinon
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si on doit sauter, false sinon
 */
bool should_jump(Machine *pmach, Instruction instr)
{
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

    return jump;
}

//! Effectue un BRANCH sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool branch_func(Machine *pmach, Instruction instr)
{
    error_if_immediate(pmach, instr);

    if(should_jump(pmach, instr))
    {
        pmach->_pc = address(pmach, instr);
    }

    return true;
}

//! Effectue un CALL sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool call_func(Machine *pmach, Instruction instr)
{
    error_if_immediate(pmach, instr);

    if(should_jump(pmach, instr))
    {
        pmach->_data[pmach->_sp--] = pmach->_pc;
        pmach->_pc = address(pmach, instr);
    }

    return true;
}

//! Effectue un RET sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool ret_func(Machine *pmach, Instruction instr)
{
    error_if_segstack(++pmach->_sp);
    pmach->_pc = pmach->_data[pmach->_sp];
    return true;
}

//! Effectue un PUSH sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool push_func(Machine *pmach, Instruction instr)
{
    error_if_segstack(pmach);

    if(!instr.instr_generic._immediate)
    {
        unsigned addr = address(pmach, instr);
        error_if_segdata(pmach, addr);
        pmach->_data[pmach->_sp] = pmach->_data[addr];
    }

    else
        pmach->_data[pmach->_sp] = instr.instr_immediate._value;

    --pmach->_sp;
    return true;
}

//! Effectue un POP sur la machine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return true si aucune erreur, pas de return sinon
 */
bool pop_func(Machine *pmach, Instruction instr)
{
    ++pmach->_sp;
    error_if_segstack(pmach);
    error_if_immediate(pmach, instr);

    unsigned addr = address(pmach, instr);

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
bool halt_func(Machine *pmach, Instruction instr)
{
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

