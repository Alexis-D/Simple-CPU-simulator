/*!
 * \file test_simul.c
 * \brief Test du simulateur
 */

#include <stdio.h>
#include <stdlib.h>

#include "machine.h"
#include "debug.h"

//! Segment de texte
extern Instruction text[];

//! Taille utile du programme
extern const unsigned textsize;

//! Premier exemple de segment de données initial
extern Word data[];

//! Fin de la zone de données utile
extern const unsigned dataend;

//! Taille utile du segment de données
extern const unsigned datasize;  

//! Help message.
/*!
 * Printed with option \c -h.
 */
static void usage()
{
    printf("Usage: test_simul [options] [binfile]\n");
    printf("where options are:\n"
           "\t-d\tDebug mode (interactive execution)\n"
           "\t-b\tA binary file is provided\n"
           "\t-l\tDo not execute; just display the listing\n"
           "\t-h\tprint this help message\n"
           "If -b is given, the next argument must be a file name containing\n"
           "a valid program in binary format. Otherwise an internally defined\n"
           "example program is used; the program is also dumped in binary into\n"
           "the file dump.bin\n");
}

//! Programme de test
/*!
 * Options de la ligne de commande :
 *
 * <dl>
 *   <dt>-d</dt><dd>mode pas à pas (mise au point)</dd>
 *
 *   <dt>-f</dt><dd>le programme est dans un fichier binaire ; le nom de ce
 *   fichier doit être fourni également en paramètre de la ligne de
 *   commande ; sans cette option, on exécute un programme de test prédéfini.</dd>
 *
 * </dl>
 */
int main(int argc, char *argv[])
{
    bool debug = false;
    bool binfile = false;
    bool no_exec = false;
    char *programfile = NULL;

    if (argc > 1) 
    {
        for (int iarg = 1; iarg < argc; ++iarg)
        {
            if (argv[iarg][0] == '-')
                switch (argv[iarg][1])
                {
                case 'd':
                    debug = true;
                    break;
                case 'b': 
                    binfile = true;
                    break;
                 case 'l': 
                    no_exec = true;
                    break;
                  case 'h':
                    usage();
                    exit(EXIT_SUCCESS);
                default:
                    fprintf(stderr, "Unknown option: %s\n", argv[iarg]);
                    usage();
                    exit(EXIT_FAILURE);
                }
            else if (binfile)
                programfile = argv[iarg];
            else 
                fprintf(stderr, "Trailing options ignored...\n");
        }
    }

    Machine mach;

    if (!binfile) 
        load_program(&mach, textsize, text, datasize, data, dataend);
    else 
        read_program(&mach, programfile);   

    printf("\n*** Sauvegarde des programmes et données initiales en format binaire ***\n\n");
    dump_memory(&mach);

    printf("\n*** Machine state before execution ***\n");
    print_program(&mach);
    print_data(&mach);
    print_cpu(&mach);

    if (no_exec) 
        return 0;

    printf("\n*** Execution trace ***\n\n");
    simul(&mach, debug);

    printf("\n*** Machine state after execution ***\n");
    print_cpu(&mach);
    print_data(&mach);

    return 0; 
}
