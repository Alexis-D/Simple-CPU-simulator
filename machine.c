#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "machine.h"
#include "exec.h"
#include "debug.h"

void create_binary_file(Machine *pmach)
{
    FILE *file;
    if(!(file = fopen("dump.bin", "w+")))
    {
        fprintf(stderr, "Ecriture du fichier impossible.\n");
        exit(1);
    }

    if(fwrite(&(pmach->_textsize), sizeof(unsigned), 1, file) != 1)
    {
        fprintf(stderr, "Erreur durant l'écriture de _textsize.\n");
        exit(2);
    }

    if(fwrite(&(pmach->_datasize), sizeof(unsigned), 1, file) != 1)
    {
        fprintf(stderr, "Erreur durant l'écriture de _datasize.\n");
        exit(2);
    }

    if(fwrite(&(pmach->_dataend), sizeof(unsigned int), 1, file) != 1)
    {
        fprintf(stderr, "Erreur durant l'écriture de _dataend.\n");
        exit(2);
    }

    if(fwrite(&(pmach->_text->_raw), sizeof(Word), pmach->_textsize, file) != pmach->_textsize)
    {
        fprintf(stderr, "Erreur durant l'écriture des instructions.\n");
        exit(2);
    }

    if(fwrite(pmach->_data, sizeof(Word), pmach->_datasize, file) != pmach->_datasize)
    {
        fprintf(stderr, "Erreur durant l'écriture des données.\n");
        exit(2);
    }

    fclose(file);
}

void load_program(Machine *pmach,
        unsigned textsize, Instruction text[textsize],
        unsigned datasize, Word data[datasize],  unsigned dataend)
{
    pmach->_textsize = textsize; //Taille du segment de texte
    pmach->_datasize = datasize; //Taille du segment de données
    pmach->_dataend = dataend;
    pmach->_text = text;
    pmach->_data = data;
    pmach->_pc = 0;
    pmach->_cc = CC_U;
    pmach->_sp = datasize - 1;

    for(int i = 0; i < NREGISTERS - 1; ++i)
    {
        pmach->_registers[i] = 0;
    } 
}

void read_program(Machine *mach, const char *programfile)
{
    FILE *file = fopen(programfile, "r"); //ouverture du fichier en mode lecture
    if(file)
    {
        unsigned sizes[3];
        fread(sizes, sizeof(unsigned), 3, file); //Lecture des tailles des segments à allouer

        printf("%d - %d - %d\n", sizes[0], sizes[1], sizes[2]);

        Instruction *text = malloc(sizes[0] * sizeof(Instruction));
        Word *data = malloc(sizes[1] * sizeof(Word));
        fread(text, sizeof(Instruction), sizes[0], file);
        fread(data, sizeof(Word), sizes[1], file);
        fclose(file);

        load_program(mach, sizes[0], text, sizes[1], data, sizes[2]);
    }

    else
    {
        fprintf(stderr, "Ouverture du fichier \"%s\" impossible.\n", programfile);
        exit(1);
    }
}

void dump_memory(Machine *pmach)
{
    printf("Instruction text[] = {\n");

    for(int i = 0; i < pmach->_textsize; ++i)
        switch(i % 4)
        {
            case 0:
                printf("    0x%08x, ", pmach->_text[i]._raw);
                break;
            case 3:
                printf("0x%08x, \n", pmach->_text[i]._raw);
                break;
            default:
                printf("0x%08x, ", pmach->_text[i]._raw); 
        }

    if(pmach->_textsize % 4 != 0)
        printf("\n");

    printf("};\nunsigned textsize = %d;\n", (pmach->_textsize));
    printf("\nWord data[] = {\n");

    for(int i = 0; i < pmach->_datasize; ++i)
        switch(i % 4)
        {
            case 0:
                printf("    0x%08x, ", pmach->_data[i]);
                break;
            case 3:
                printf("0x%08x, \n", pmach->_data[i]);
                break;
            default:
                printf("0x%08x, ", pmach->_data[i]); 
        }

    if(pmach->_datasize % 4 != 0)
        printf("\n");

    printf("};\nunsigned datasize = %d;\n", (pmach->_datasize));
    printf("unsigned dataend = %d;\n", (pmach->_dataend));

    create_binary_file(pmach);
}

void print_program(Machine *pmach)
{
    printf("\n*** PROGRAM (size: %i) ***\n", pmach->_textsize);
    for(int i = 0; i < pmach->_textsize; ++i)
    {
        printf("0x%04x: 0x%08x \t ", i, pmach->_text[i]._raw);
        print_instruction(pmach->_text[i], i);
        printf("\n");
    }

    printf("\n");
}

void print_data(Machine *pmach)
{
    printf("*** DATA (size: %i, end = Ox%08x (%i)) ***\n", pmach->_datasize, pmach->_dataend, pmach->_dataend);

    for(int i = 0; i < pmach->_datasize; ++i)
    {
        printf("0x%04x: 0x%08x %-6i ", i, pmach->_data[i], pmach->_data[i]);
        if(i % 3 == 2)
            printf("\n");
    }

    if(pmach->_datasize % 3 != 0)
        printf("\n");

    printf("\n");
}

void print_cpu(Machine *pmach)
{
    printf("\n*** CPU ***\nPC:  0x%08x   CC: ", pmach->_pc);
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

    for(int i = 0; i < NREGISTERS; ++i)
    {
        printf("R%02i: Ox%08x %-6i ", i, pmach->_registers[i], pmach->_registers[i]);
        if(i % 3 == 2)
            printf("\n");
    }

    if(NREGISTERS % 3 != 0)
        printf("\n");

    printf("\n");
}

void simul(Machine *pmach, bool debug)
{
    do
    {
        trace("Executing", pmach, pmach->_text[pmach->_pc], pmach->_pc);

        if(debug)
            debug = debug_ask(pmach);
    } while(decode_execute(pmach, pmach->_text[pmach->_pc++]));
}

