#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

/*!
 * \file instruction.h
 * \brief Description du jeu d'instruction.
 */

#include <stdbool.h>
#include <stdint.h>

//! Codes opérations
typedef enum 
{
    ILLOP = 0,	//!< Instruction illégale
    NOP,	//!< Instruction sans effet
    LOAD,	//!< Chargement d'un registre
    STORE,	//!< Rangement du contenu d'un registre 
    ADD,	//!< Addition à un registre 
    SUB,	//!< Soustraction d'un registre 
    BRANCH,	//!< Branchement conditionnel ou non  
    CALL,	//!< Appel de sous-programme 
    RET,	//!< Retour de sous-programme 
    PUSH,	//!< Empilement sur la pile d'exécution 
    POP,	//!< Dépilement de la pile d'exécution
    HALT,	//!< Arrêt (normal) du programme
} Code_Op;

//! Dernière valeur possible du code opération
const static unsigned LAST_COP = HALT;


//! Structure d'une instruction 
/*!
 * Toutes les instrcutions occupent un mot machine de 32 bits. Il y a
 * différents formats possibles selon le type de l'instruction et de ses
 * opérandes.

 * \note Bien entendu, on aurait pu se contenter de décrire une instruction
 * comme un mot de 32 bits (\c uint32_t) et extraire les différents champs à
 * coup de masquage, d'opérations bit à bit ou autres décalages. Mais
 * pourquoi ne pas laisser le compilateur faire toutes ces horreurs à notre
 * place ? Pour cela nous utilisons une union pour représenter les différents
 * formats et des champs de bits pour représenter les différents constituants
 * de l'instruction.
 *
 * \note Ceci a un inconvénient (léger) : les champs de bits ne s'appliquent
 * qu'à des types de nature entière et pas à des structures ni des unions. Ceci
 * nous oblige donc à avoir une union de structures, chaque structure reprenant
 * les champs communs.
 */
typedef union Instruction
{ 
    //! Format brut : un mot de 32 bits
    uint32_t _raw;

    //! Format générique : les premiers champs sont communs
    struct 
    { 
        Code_Op _cop : 6; 	//!< Code opération 
        bool _immediate : 1;	//!< Adressage immédiat ?
        bool _indexed : 1;	//!< Adressage indirect ?
        unsigned _regcond : 4;	//!< Numéro de registre ou condition
        unsigned _pad : 20;     //<! Format variable...
    } instr_generic;

    //! Format d'une instruction à adressage absolue
    struct 
    {
        Code_Op _cop : 6; 	//!< Code opération
        bool _immediate : 1;	//!< Adressage immédiat ?
        bool _indexed : 1;	//!< Adressage indirect ?
        unsigned _regcond : 4;	//!< Numéro de registre ou condition
        unsigned _address : 20;	//!< Adresse absolue
    } instr_absolute;

     //! Format d'une instruction à valeur immédiate
    struct 
    {
        Code_Op _cop : 6; 	//!< Code opération
        bool _immediate : 1;	//!< Adressage immédiat ?
        bool _indexed : 1;	//!< Adressage indirect ?
        unsigned _regcond : 4;	//!< Numéro de registre ou condition
        signed int _value : 20;	//!< Valeur immédiate
    } instr_immediate;

    //! Format d'une instruction à adressage indéxé
    struct 
    {
        Code_Op _cop : 6; 	//!< Code opération
        bool _immediate : 1;	//!< Adressage immédiat ?
        bool _indexed : 1;	//!< Adressage indirect ?
        unsigned _regcond : 4;	//!< Numéro de registre ou condition
        unsigned _rindex : 4;   //!< Numéro du registre d'index
        signed int _offset : 16;//!< Déplacement
    } instr_indexed;

} Instruction;

//! Conditions
/*!
 * Ces valeurs sont associées à l'instruction de branchement (\c BRANCH et \c CALL) et
 * déterminent la condition à tester par référence à la valeur du code
 * condition (\link Condition_Code \endlink).
 * 
 * Une valeur de ce type remplace le numéro de registres dans les isntructions
 * \c BRANCH et \c CALL.
 */
typedef enum 
{
    NC, //!< Pas de condition (nrachement inconditionnel)
    EQ,	//!< Égal à 0
    NE,	//!< Différent de 0
    GT, //!< Strictement positif
    GE,	//!< Positif ou nul
    LT, //!< Strictement négatif
    LE, //!< Négatif ou null
} Condition;

//! Dernière valeur possible d'une condition
static const unsigned LAST_CONDITION = LE;

//! Type d'un mot de donnée
typedef uint32_t Word;

//! Forme imprimable des codes opérations
extern const char *cop_names[];

//! Forme imprimable des conditions
extern const char *condition_names[];

//! Impression d'une instruction sous forme lisible (désassemblage)
/*!
 * \param instr l'instruction à imprimer
 * \param addr son adresse
 */
void print_instruction(Instruction instr, unsigned addr);

#endif
