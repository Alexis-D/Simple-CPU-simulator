#include <stdio.h>
#include "error.h"
#include "exec.h"
#include "debug.h"

const char *condition_code_names[] =
{
    "U",
    "Z",
    "P",
    "N",
};

//! Ecriture du programme et des données dans le fichier dump.prog
/*!
 * On écrit le programme qui va être simulé dans un fichier binaire
 * Si le fichier pr 
 * \param pmach la machine en cours d'exécution
 */
void create_binary_file(Machine *pmach)
{
    FILE *file;
    //On ouvre le fichier dump.prog avec les droits en lecture et en écriture.
    //Si le fichier n'existe pas il sera créé. S'il existe déjà son contenu
    //sera effacé.
    if(!(file = fopen("dump.bin", "w+")))
    {
        //Problème survenu lors de l'ouverture ou de la création du fichier.
        //On affiche l'erreur sur la sortie standard et on sort de la fonction
        fprintf(stderr, "Ecriture du fichier impossible.\n");
        exit(1);
    }

    //On commence par écrire la taille du segment de texte.
    if(fwrite(&(pmach->_textsize), sizeof(unsigned), 1, file) != 1)
    {
        //Problème pendant l'écriture : on l'écrit sur la sortie d'erreur
        //standard et on sort de la fonction
        fprintf(stderr, "Erreur durant l'écriture de _textsize.\n");
        exit(1);
    }

    //Ecriture des instructions
    if(fwrite(&(pmach->_datasize), sizeof(unsigned), 1, file) != 1)
    {
        //Problème pendant l'écriture : on l'écrit sur la sortie d'erreur
        //standard et on sort de la fonction
        fprintf(stderr, "Erreur durant l'écriture de _datasize.\n");
        exit(1);
    }

    //Ecriture du dataend
    if(fwrite(&(pmach->_dataend), sizeof(unsigned int), 1, file) != 1)
    {
        //Problème pendant l'écriture : on l'écrit sur la sortie d'erreur
        //standard et on sort de la fonction
        fprintf(stderr, "Erreur durant l'écriture de _dataend.\n");
        exit(1);
    }

    //Ecriture des instructions
    if(fwrite(&(pmach->_text->_raw), sizeof(Word), pmach->_textsize, file) !=
            pmach->_textsize)
    {
        //Problème pendant l'écriture : on l'écrit sur la sortie d'erreur
        //standard et on sort de la fonction
        fprintf(stderr, "Erreur durant l'écriture des instructions.\n");
        exit(1);
    }

    //Ecriture des données
    if(fwrite(pmach->_data, sizeof(Word), pmach->_datasize, file)
            != pmach->_datasize)
    {
        //Problème pendant l'écriture : on l'écrit sur la sortie d'erreur
        //standard et on sort de la fonction
        fprintf(stderr, "Erreur durant l'écriture des données.\n");
        exit(1);
    }
    //On ferme le fichier
    fclose(file);
}

void load_program(Machine *pmach,
        unsigned textsize, Instruction text[textsize],
        unsigned datasize, Word data[datasize],  unsigned dataend)
{
    //Initialisation de la taille du segment de texte
    pmach->_textsize = textsize;
    //Initialisation de la taille du segment de données
    pmach->_datasize = datasize;
    pmach->_dataend = dataend; //Initialisation du dataend
    pmach->_text = text; //Initilisation du segment de texte
    pmach->_data = data; //Initialisation du segment de données
    pmach->_pc = 0; //Initialisation du compteur ordinal
    pmach->_cc = CC_U; //Initialisation du code condition
    pmach->_sp = datasize - 1; //Initialisation du stack pointeur
    //Initialisation des regisres généraux (R00 à R14) à 0
    for(int i = 0; i < NREGISTERS - 1; ++i)
    {
        pmach->_registers[i] = 0;
    } 
}

void read_program(Machine *mach, const char *programfile)
{
    FILE *file;
    //Ouverture du fichier binaire passé en paramètre en mode lecture seul
    if((file = fopen(programfile, "r")) == NULL)
    {
        //Problème pendant l'écriture : on l'écrit sur la sortie d'erreur
        //standard et on sort de la fonction
        fprintf(stderr, "Ouverture du fichier \"%s\" impossible.\n",
                programfile);
        exit(1);
    }

    //Tableau d'entiers non signés pour la récupération de
    //textsize, datasize et dataend
    unsigned sizes[3];
    //Récupération des tailles des segments à allouer
    fread(sizes, sizeof(unsigned), 3, file);

    unsigned int stack_size = sizes[1] - sizes[2]; //Place occupée par la pile
    //On teste si on a assez de place pour la pile
    if(stack_size < MINSTACKSIZE)
    {
        //Si c'est inférieur, on modifie la taille de manière à ce
        //que la taille pile d'execution soit de la taille minimale imposée
        stack_size = MINSTACKSIZE;
    }

    //Allocation de l'espace nécessaire pour stocker les instructions
    //du programme à simuler
    Instruction *text = malloc(sizes[0] * sizeof(Instruction));
    //Allocation de l'espace nécessaire pour stocker les données du programme
    Word *data = malloc((sizes[2] + stack_size) * sizeof(Word));
    //On extrait du fichier binaire les instructions du programme
    //et on les place dans le segment de texte
    fread(text, sizeof(Instruction), sizes[0], file);
    //On extrait du fichier binaire les données du programme et on
    //les place dans le segment de données
    fread(data, sizeof(Word), sizes[1], file);
    //Fermeture du fichier
    fclose(file);

    //On appelle load_program pour initialiser la machine avec les
    //données que l'on vient de récuperer
    load_program(mach, sizes[0], text, sizes[2] + stack_size, data, sizes[2]);
}

void dump_memory(Machine *pmach)
{
    printf("Instruction text[] = {\n");

    //Affichage du contenu du segment de texte
    for(int i = 0; i < pmach->_textsize; ++i)
        switch(i % 4)
        {
            case 0:
                //Début de ligne donc on fait un alinéa
                printf("    0x%08x, ", pmach->_text[i]._raw);
                break;
            case 3:
                //4 instructions sur une ligne donc on fait un saut de ligne
                printf("0x%08x, \n", pmach->_text[i]._raw);
                break;
            default:
                printf("0x%08x, ", pmach->_text[i]._raw); 
        }

    if(pmach->_textsize % 4 != 0)
        printf("\n");

    // Affichage de la taille du segment de texte
    printf("};\nunsigned textsize = %d;\n", (pmach->_textsize));
    printf("\nWord data[] = {\n");

    //Affichage du contenu du segment de données
    for(int i = 0; i < pmach->_datasize; ++i)
        switch(i % 4)
        {
            case 0:
                //Début de ligne donc on fait un alinéa
                printf("    0x%08x, ", pmach->_data[i]);
                break;
            case 3:
                //4 instructions sur une ligne donc on fait un saut de ligne
                printf("0x%08x, \n", pmach->_data[i]);
                break;
            default:
                printf("0x%08x, ", pmach->_data[i]); 
        }

    if(pmach->_datasize % 4 != 0)
        printf("\n");

    //Affichage de la taille du segment de données
    printf("};\nunsigned datasize = %d;\n", (pmach->_datasize));
    //Affichage du dataend
    printf("unsigned dataend = %d;\n", (pmach->_dataend));

    //On créé le fichier binaire correspondant au programme que l'on va simuler
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
    printf("*** DATA (size: %i, end = 0x%08x (%i)) ***\n",
            pmach->_datasize, pmach->_dataend, pmach->_dataend);

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
    printf("\n*** CPU ***\nPC:  0x%08x   CC: %s\n\n",
            pmach->_pc, condition_code_names[pmach->_cc]);

    for(int i = 0; i < NREGISTERS; ++i)
    {
        printf("R%02i: 0x%08x %-6i ",
                i, pmach->_registers[i], pmach->_registers[i]);
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
        if(pmach->_pc >= pmach->_textsize)
            error(ERR_SEGTEXT, pmach->_pc);

        trace("Executing", pmach, pmach->_text[pmach->_pc], pmach->_pc);

        if(debug)
            debug = debug_ask(pmach);
    } while(decode_execute(pmach, pmach->_text[pmach->_pc++]));
}

