#include "debug.h"
#include "exec.h"
#include <stdio.h>

//! Dialogue de mise au point interactive pour l'instruction courante.

//! Affiche les commandes disponibles dans le debuggueur.
static void usage() {
    printf("Available commands:\n"
           "\th\thelp\n"
           "\tc\tcontinue (exit interactive debug mode)\n"
           "\ts\tstep by step (next instruction)\n"
           "\tRET\tstep by step (next instruction)\n"
           "\tr\tprint registers\n"
           "\td\tprint data memory\n"
           "\tt\tprint text (program) memory\n"
           "\tp\tprint text (program) memory\n"
           "\tm\tprint registers and data memory\n");
}

/*!
 * Cette fonction gère le dialogue pour l'option \c -d (debug). Dans ce mode,
 * elle est invoquée après l'exécution de chaque instruction.  Elle affiche le
 * menu de mise au point et on exécute le choix de l'utilisateur. Si cette
 * fonction retourne faux, on abandonne le mode de mise au point interactive
 * pour les instructions suivantes et jusqu'à la fin du programme.
 * 
 * \param mach la machine/programme en cours de simulation
 * \return vrai si l'on doit continuer en mode debug, faux sinon
 */
bool debug_ask(Machine *pmach) {
    int c;

    while(true)
    {
        printf("DEBUG? ");
        c = getchar();

        if(c == '\n')
            return true;

        int t;

        do
        {
            //vide le buffer d'stdin
            //fflush(stdin) non portable (ne fonctionne pas avec gcc
            //par exemple)
            t = getchar();
        } while(t != '\n' && t != EOF);

        switch(c)
        {
            case 'h':
                usage();
                break;

            case 'c':
                return false;

            case 's':
                return true;

            case 'r':
                print_cpu(pmach);
                break;

            case 'd':
                print_data(pmach);
                break;

            case 'p':
            case 't':
                print_program(pmach);
                break;

            case 'm':
                print_cpu(pmach);
                print_data(pmach);
                break;

            default:
                break;
        }
    }

    printf("\n");
    return false;
}

