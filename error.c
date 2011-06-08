#include <stdio.h>

#include "error.h"

const char *error_names[] =
{
    "NOERRROR",
    "UNKNOWN",
    "ILLEGAL",
    "CONDITION",
    "IMMEDIATE",
    "SEGTEXT",
    "SEGDATA",
    "SEGSTACK",
};

const char *warning_names[] =
{
    "HALT",
    "PUSH_STATIC",
}; 

void error(Error err, unsigned addr)
{
    fprintf(stderr, "ERROR: %s reached at address 0x%x\n",
            error_names[err], addr);
    exit(1);
}

void warning(Warning warn, unsigned addr)
{
    fprintf(stderr, "WARNING: %s reached at address 0x%x\n",
            warning_names[warn], addr);
}

