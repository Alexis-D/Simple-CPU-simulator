//-------------------------------------------------------
// Exemples décrivant la syntaxe de l'assembleur
//-------------------------------------------------------

        // Un programme comporte deux sections (texte et données)
        // qui doivent toujours être dans cet ordre. La taille
        // de la section de texte est facultative ; celle de
        // la section de données est obligatoire.

        
        // Début de la section de texte (instructions)
        TEXT    20      // 20 est la taille totale (optionnel)

        // EQU : directive de définition de symboles
        //------------------------------------------
        // La directive EQU définit un symbole et sa valeur.
        // **Elle n'alloue pas de mémoire**. 
        // Les symboles peuvent être référencés avant d'être
        // définis.
        // Cet assembleur ne permet pas d'écrire des expressions
        // arithmétiques, que ce soit avec des symboles ou des
        // valeurs.

        // Définition du symbole start ayant la valeur
        // du compteur d'assemblage '*', cad de l'adresse
        // courante dans la section : ici la valeur de start
        // sera donc 0
start   EQU     *

        // Definition d'un symbole avec une valeur
n1      EQU     10      // valeur décimale
n2      EQU     -20     // valeur décimale négative
n       EQU     0xffaa  // valeur hexadécimale (non signée)
np      EQU     0xFFAA  // équivalent au précédent
        
        // Symboles synonymes
firstop EQU     op1     // op1 défini plus loin ; les deux
                        // symboles ont la même valeur, celle d'op1
        
        // Utilisation des symboles dans les instructions
        //-----------------------------------------------
        // Les symboles peuvent être utilisés pour remplacer
        // des valeurs immédiates (préfixées par #), des adresses
        // absolues (préfixées par @) ou des offsets (pas de préfixe)

        // @op2 est l'adresse absolue de valeur op2
        // instr1 a la valeur du compteur d'assemblage
        // cad encore 0 (start)
instr1  LOAD   R00, @op2  // chargement adresse op2 (2)
        LOAD    R00, @2   // équivalent au précédent
        LOAD    R00, @0x2 // équivalent aux deux précédents
        
        LOAD    R01, n2[R03] // adressage relatif
        LOAD    R01, -20[R03] // ici, équivalent au précédent

        LOAD    R03, #op2 // chargement d'une valeur immédiate
        LOAD    R03, #-2 // ici, équivalent au précédent

        // Fin de la section de texte
        END

        // Début de la section de données
        DATA    30      // 30 est la taille totale.
                        // ceci doit inclure la taille nécessaire
                        // pour la pile d'exécution

        WORD    0       // définition d'un mot de donnée
                        // contenant la valeur 0 (et à l'adresse 0)

op1     WORD    -3      // définition d'un mot de donnée
                        // contenant la valeur -3 ; le symbole
                        // op1 représente l'adresse de ce mot
                        // dans la section de données, cad ici 1

op2     WORD    0xff    // définition d'un mot de donnée
                        // contenant la valeur ff en hexa (255 décimal)
                        // le symbole op2 représente l'adresse de ce mot
                        // dans la section de données, cad ici 2

        WORD    n2      // définition d'un mot de donnée
                        // contenant la valeur associé au symbole
                        // n2, cad -20

        // Fin de la section de données
        END

        

        
