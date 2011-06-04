#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdlib.h>

/*!
 * \file error.h
 * \brief Messages et codes d'erreurs.
 */

//! Erreur d'exécution
/*!
 * Ce sont les différentes sortes d'erreur rencontrées lors du décodage ou de
 * l'exécution des instructions. Elles sont toutes fatales et provoquent la
 * terminaison du programme (du programme simulé comme du simulateur lui-même
 * !).
 */
typedef enum 
{
    ERR_NOERROR = 0,	//!< Pas d'erreur
    ERR_UNKNOWN, 	//!< Instruction inconnue
    ERR_ILLEGAL,	//!< Instruction illégale
    ERR_CONDITION,	//!< Condition illégale
    ERR_IMMEDIATE,	//!< Valeur immédiate interdite
    ERR_SEGTEXT,	//!< Violation de taille du segment de texte
    ERR_SEGDATA,	//!< Violation de taille du segment de données
    ERR_SEGSTACK,	//!< Violation de taille du segment de pile
} Error; 

//! Dernière valeur possible du code d'erreur
static const unsigned LAST_ERROR = ERR_SEGSTACK;

//! Codes d'avertissement
/*!
 * Ce sont de simples messages informatifs qui ne provoquent pas la terminaison
 * du programme.
 */
typedef enum 
{
    WARN_HALT,		//!< Fin normale du programme (sur HALT)
} Warning;

//! Dernière valeur possible du code d'avertissement
static const unsigned LAST_WARNING = WARN_HALT;

//! Affichage d'une erreur et fin du simulateur
/*!
 * \note Toutes les erreurs étant fatales on ne revient jamais de cette
 * fonction. L'attribut \a noreturn est une extension (non standard) de GNU C
 * qui indique ce fait.
 * 
 * \param err code de l'erreur
 * \param addr adresse de l'erreur
 */
#ifdef __GNUC__
void error(Error err, unsigned addr) __attribute__((noreturn));
#else
void error(Error err, unsigned addr);
#endif


//! Affichage d'un avertissement
/*!
 * \param warn code de l'avertissement
 * \param addr adresse de l'erreur
 */
void warning(Warning warn, unsigned addr);

#endif
