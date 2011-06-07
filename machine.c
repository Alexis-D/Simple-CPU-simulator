#include "exec.h"
#include "machine.h"

#include <stdio.h>

void print_program(Machine *pmach)
{
	
}

void print_data(Machine *pmach)
{
	printf("***   DATA (size: %i, end = Ox%x (%i))   ***\n\n",pmach->_datasize,pmach->_dataend,pmach->_dataend);
	int i;
	for(i = 0; i < pmach->_datasize; ++i)
  {
		printf("0x%04x: 0x%0x %i.\n",i,pmach->_data[i],pmach->_data[i]);
		if(i%3 == 2) printf("\n");
  }
	printf("\n");
}

void print_cpu(Machine *pmach)
{
	printf("***   CPU   ***\nPC:\t0x%x\tCC: ",pmach->_pc);
	switch (pmach->_cc)
	{
		case CC_U :
			printf("U");
			break;
		case CC_Z :
			printf("Z");
			break;
		case CC_P
			printf("P");
			break;
		case CC_N
			printf("N");
			break;
	}

	printf("\n\n");

	int i;
	for(i = 0; i < NREGISTERS; ++i)
	{
		printf("R%02i:\tOx%04x %i",i,pmach->_registers[i],pmach->_registers[i]);
		if(i%3 == 2) printf("\n");
	}
	printf("\n");
}

void simul(Machine *pmach, bool debug)
{
	do
	{
		if(debug)
		{
			ask_debug(&pmach);
		}
	} while(exec(&mach,pmach->_text[pmach->_pc++])

	printf("exécution terminée, sortie normale...\n");

}