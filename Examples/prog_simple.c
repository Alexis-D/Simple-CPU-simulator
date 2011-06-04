#include "machine.h"

//! Premier exemple de segment de texte.
/*!
 * \note On voit ici l'intérêt de l'initialisation d'une union avec nommage
 * explicite des champs.
 */

Instruction text[] = {
//   type		 cop	imm	ind	regcond	operand
//-------------------------------------------------------------
    {.instr_absolute =   {LOAD, 	 false, false, 	0, 	0	}},  // 0
    {.instr_absolute =   {LOAD, 	 false, false, 	1, 	1	}},  // 1
    {.instr_immediate = {SUB, 	 true, 	false, 	1, 	1	}},  // 2
    {.instr_absolute =  {BRANCH, false, false, 	LE, 	7	}},  // 3
    {.instr_indexed =   {ADD, 	 false, false, 	0, 	0	}},  // 4
    {.instr_immediate = {SUB, 	 true, 	false, 	1, 	1	}},  // 5 
    {.instr_absolute =  {BRANCH, false, false, 	NC, 	3	}},  // 6   
    {.instr_absolute =  {STORE,  false, false, 	0, 	2	}},  // 7
    {.instr_generic =   {HALT,					}},  // 5
    {.instr_generic =   {ILLOP,					}},  // 5
    {.instr_generic =   {ILLOP,					}},  // 5
    {.instr_generic =   {ILLOP,					}},  // 5
};

//! Taille utile du programme
const unsigned textsize = sizeof(text) / sizeof(Instruction);

//! Premier exemple de segment de données initial
Word data[20] = {
    10, // 0: premier opérande
    5,  // 1: second opérande
    20, // 2: résultat
    0,  // 3
};

//! Fin de la zone de données utiles
const unsigned dataend = 5;

//! Taille utile du segment de données
const unsigned datasize = sizeof(data) / sizeof(Word);  
