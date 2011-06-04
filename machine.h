#ifndef _MACHINE_H_
#define _MACHINE_H_

/*!
 * \file machine.h
 * \brief Description de la structure du processeur et de sa mémoire
 */

#include <stdbool.h>

#include "instruction.h"

//! Nombre de resitres généraux
#define NREGISTERS 16

//! Code condition
/*! 
 * Le code condition donne le signe du résultat de la dernière instruction
 * arithmétique (addition, soustraction) exécutée par le processeur. 
 *
 * \note Dans un processeur réel, le code condition fait partie du mot
 * d'état du processeur. Il contient d'autres indications que le simple
 * signe du résultat : par exemple le fait qu'une retenue ait été générée
 * ou bien qu'un dépassement de capacité (\e overflow) se soit produit.
 */
typedef enum 
{
    CC_U = 0,	//!< Signe du résultat inconnu
    CC_Z,	//!< Résultat nul
    CC_P,	//!< Résultat positif
    CC_N,	//!< Résultat négatif
} Condition_Code;

//! Dernière valeur possible du code condition
static const unsigned LAST_CC = CC_N;

//! Taille minimale de la pile d'exécution
static const unsigned MINSTACKSIZE = 10;

//! Structure générale de la machine.
/*!
 * Cette machine simple est composée de mémoire et d'un processeur. 
 *
 * Pour simplifier, une adresse machine référence un mot de 32 bits (et non un
 * octet comme dans la plupart des machines actuelles !). La mémoire est
 * découpée en deux segments : \b texte contenant les instructions du
 * programme; \b données contenant les données manipulées ou calculées par le
 * programme. L'adresse de début de chacun de ces segments est 0 ; ils sont
 * donc adressés indépendemment. Le segment de données contient les données
 * statiques dans ses adresses basses et la <i>pile d'exécution</i> dans ses
 * adresses hautes. Le sommet de la pile d'exécution est à l'adresse contenue
 * dans le registre \c R15 (\c SP). Chacun de ces segments a une taille
 * maximale. En outre on conserve la taille utile de chaque segment.
 *
 * Le processeur comporte 
 *
 *   - un registre servant de <b>compteur ordinal</b>  (<em>program counter</em>)
 *   qui contient l'adresse (dans le segment de texte) de la prochaine
 *   instruction à exécuter ;
 *
 *   - un registre contenant le code condition (voir \link Condition_Code \endlink) ;
 *
 *   - un ensemble de 16 <b>registres généraux</b> servant d'accumulateurs
 *   (registres de calcul). Tous ces registres sont identiques et
 *   supportent les mêmes opérations. Cependant, le registre 15 (connu
 *   aussi sous le nom \c _sp) joue un rôle spécial, celui de pointeur de
 *   la pile d'exécution : il doit contenir en permanence l'adresse du
 *   sommet de pile (premier élément libre de la pile).
 */
typedef struct
{
    // Segments de mémoire
    Instruction *_text;		//!< Mémoire pour les instructions
    unsigned int _textsize;	//!< Taille utilisée pour les instructions

    Word *_data;		//!< Mémoire de données
    unsigned int _datasize;	//!< Taille utilisée pour les données

    unsigned int _dataend;      //!< Première adresse libre après les données statiques

    // Registres de l'unité centrale
    unsigned _pc;		//!< Compteur ordinal
    Condition_Code _cc;		//!< Code condition : signe de la dernière opération
    Word _registers[NREGISTERS];//!< Registres généraux (accumulateurs)

//! Définition de _sp comme synonyme du registre R15    
#   define _sp _registers[NREGISTERS - 1] 
} Machine;

//! Chargement d'un programme
/*!
 * La machine est réinitialisée et ses segments de texte et de données sont
 * remplacés par ceux fournis en paramètre.
 *
 * \param pmach la machine en cours d'exécution
 * \param textsize taille utile du segment de texte
 * \param text le contenu du segment de texte
 * \param datasize taille utile du segment de données
 * \param data le contenu initial du segment de texte
 */
void load_program(Machine *pmach,
                  unsigned textsize, Instruction text[textsize],
                  unsigned datasize, Word data[datasize],  unsigned dataend);

//! Lecture d'un programme depuis un fichier binaire
/*!
 * Le fichier binaire a le format suivant :
 * 
 *    - 3 entiers non signés, la taille du segment de texte (\c textsize),
 *    celle du segment de données (\c datasize) et la première adresse libre de
 *    données (\c dataend) ;
 *
 *    - une suite de \c textsize entiers non signés représentant le contenu du
 *    segment de texte (les instructions) ;
 *
 *    - une suite de \c datasize entiers non signés représentant le contenu initial du
 *    segment de données.
 *
 * Tous les entiers font 32 bits et les adresses de chaque segment commencent à
 * 0. La fonction initialise complétement la machine.
 *
 * \param pmach la machine à simuler
 * \param programfile le nom du fichier binaire
 *
 */
void read_program(Machine *mach, const char *programfile);  
 
//! Affichage du programme et des données
/*!
 * On affiche les instruction et les données en format hexadécimal, sous une
 * forme prête à être coupée-collée dans le simulateur.
 *
 * Pendant qu'on y est, on produit aussi un dump binaire dans le fichier
 * dump.prog. Le format de ce fichier est compatible avec l'option -b de
 * test_simul.
 *
 * \param pmach la machine en cours d'exécution
 */
void dump_memory(Machine *pmach);

//! Affichage des instructions du programme
/*!
 * Les instructions sont affichées sous forme symbolique, précédées de leur adresse.
.* 
 * \param pmach la machine en cours d'exécution
 */
void print_program(Machine *pmach);

//! Affichage des données du programme
/*!
 * Les valeurs sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_data(Machine *pmach);

//! Affichage des registres du CPU
/*!
 * Les registres généraux sont affichées en format hexadécimal et décimal.
 *
 * \param pmach la machine en cours d'exécution
 */
void print_cpu(Machine *pmach);

//! Simulation
/*!
 * La boucle de simualtion est très simple : recherche de l'instruction
 * suivante (pointée par le compteur ordinal \c _pc) puis décodage et exécution
 * de l'instruction.
 *
 * \param pmach la machine en cours d'exécution
 * \param debug mode de mise au point (pas à apas) ?
 */
void simul(Machine *pmach, bool debug);

#endif
