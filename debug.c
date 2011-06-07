#include "debug.h"
#include "instruction.h"
#include "machine.h"
#include "exec.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//! Dialogue de mise au point interactive pour l'instruction courante.
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
void option_H() {

	printf("\tAvailable commands:\n");
	printf("\t\th\thelp\n");
	printf("\t\tc\tcontinue (exit interactive debug mode)\n");
	printf("\t\ts\t\tstep by step (next instruction)\n");
	printf("\t\tRET\t\tstep by step (next instruction)\n");
	printf("\t\tr\t\tprint registers\n");
	printf("\t\td\t\tprint data memory\n");
	printf("\tt\t\tprint text (program) memory\n");
	printf("\t\tp\t\tprint text (program) memory\n");
	printf("\t\tm\t\tprint registers and data memory\n");

}

bool debug_ask(Machine *pmach) {

	char cmd, c;
	
	while (true) {
		printf("DEBUG?");
		scanf("%d", &cmd);
		c = cmd;
		while (c != '\n') {
			c = getchar();
		}
			switch (cmd) {
			case 'h':
				option_H();
				break;
			case 'c':
				return false;
				break;
			case 's':
				return true;
				break;
			case '\r':
				return true;
				break;
			case 'r':
				print_cpu(pmach);
				break;
			case 'd':
				print_data(pmach);
				break;
			case 't':
				print - program(pmach);
				break;
			case 'p':
				print - program(pmach);
				break;
			case 'm':
				print_data(mach);
				print_cpu(mach);
				break;
			default:
				break;
			}
			
	}
	return false;
}

