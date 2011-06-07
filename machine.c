#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "machine.h"
#include "instruction.h"
#include "exec.h"
#include "debug.h"

void read_program(Machine *mach, const char *programfile)
{
  FILE *file = fopen(programfile, "r"); //ouverture du fichier en mode lecture
  if(file != NULL){
    unsigned int *sizes = malloc(sizeof(unsigned int));
    fread(sizes, sizeof(unsigned int),3, file);//Lecture des tailles des segments à allouer
    mach->_textsize = sizes[0];//Taille du segment de texte
    mach->_datasize = sizes[1];//Taille du segment de données
    mach->_dataend = sizes[2];
    free(sizes);
    
    mach->_text = malloc((mach->_textsize)*sizeof(Instruction));
     
    fread((mach->_text)->_raw, sizeof(uint32_t),mach->_textsize, file);

    mach->_data = malloc((mach->_datasize)*sizeof(Word));
    fread((mach->_data), sizeof(uint32_t), mach->_datasize, file);
    fclose(file);
    mach->_pc = &(mach->_text);
    mach->_cc = LAST_CC;
    mach->_registers[NREGISTERS-1]=&((mach->_data)[mach->_datasize-1)]);
  }
  else{
    perror("Ouverture du fichier %s impossible\n", *programfile);
    exit(1);
  }
}


void dump_memory(Machine *pmach)
{
  int cpt = 0;
  printf("\nInstruction text[] = {\n");
  for(int i = 0 ; i < pmach->_textsize ; i++)
    {
      if(cpt == 0){
	printf("\t0x%08x,", (pmach->_text)[i]);
	cpt++;
      }
      else{
	if(cpt == 3){
	  printf(" 0x%08x,\n", (pmach->_text)[i]);
	  cpt = 0;
	}
	else{
	  printf(" 0x%08x,", (pmach->_text)[i]); 
	  cpt++;
	}
      }
    }
  if(cpt!=0){
    printf("\n");
    cpt = 0;
  }

  printf("};\nunsigned textsize = %d;\n", (pmach->_textsize));
  
  print("\nWord data[] = {\n");
  for(int i = 0 ; i < pmach->_datasize ; i++)
    {
      if(cpt == 0){
	printf("\t0x%08x,", (pmach->_data)[i]);
	cpt++;
      }
      else{
	if(cpt == 3){
	  printf(" 0x%08x,\n", (pmach->_data)[i]);
	  cpt = 0;
	}
	else{
	  printf(" 0x%08x,", (pmach->_data)[i]); 
	  cpt++;
	}
      }
    }
  if(cpt!=0){
    printf("\n");
    cpt = 0;
  }

  printf("};\nunsigned datasize = %d;\n", (pmach->_datasize));
  printf("unsigned dataend = %d;\n", (pmach->_dataend));

  create_binary_file(&pmach);
}

void create_binary_file(Machine *pmach)
{
  FILE *file;
  if((file = fopen("dump.bin", "w+")) == NULL){
    perror("Ecriture du fichier dump.bin impossible !");
    exit(1);
  }

  if(fwrite(pmach->_textsize, sizeof(unsigned int), 1, file) != 1){
    fwrite("Erreur a l'ecriture de textesize\n", 33, 1, stderr);
    exit(2);
  }
  
  if(fwrite(pmach->_datasize, sizeof(unsigned int), 1, file) != 1){
    fwrite("Erreur a l'ecriture de datasize\n", 32, 1, stderr);
    exit(2);
  }

  if(fwrite(pmach->_dataend, sizeof(unsigned int), 1, file) != 1){
    fwrite("Erreur a l'ecriture de dataend\n", 31, 1, stderr);
    exit(2);
  }

  if(fwrite((pmach->_text)->_raw, sizeof(uint32_t), pmach->_textsize, file) != pmach->_textsize){
    fwrite("Erreur a l'ecriture des instructions\n", 37, 1, stderr);
    exit(2);
  }

  if(fwrite(pmach->_data, sizeof(uint32_t), pmach->_datasize, file) != pmach->_datasize){
    fwrite("Erreur a l'ecriture des donnees\n", 32, 1, stderr);
    exit(2);
  }

  fclose(file);
}

void print_program(Machine *pmach)
{
	printf("***   PROGRAM (size: %i)   ***\n\n",pmach->_textsize);
	int i;
	for(i = 0; i < pmach->_textsize; ++i)
	{
		printf("0x%04x: %0x\t",i,pmach->_text[i]._raw);
		print_instruction(pmach->_text[i],i);
		printf("\n");
	}
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
    case CC_P :
      printf("P");
    break;
    case CC_N :
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
			debug = ask_debug(pmach);
		}
	} while(exec(pmach,pmach->_text[pmach->_pc++]));

	free(pmach->_text);
	free(pmach->_data);
  printf("exécution terminée, sortie normale...\n");
}
