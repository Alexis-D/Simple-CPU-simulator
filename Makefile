#-------------------------------------------------------------------
# Simulation d'un processeur simple 
#-------------------------------------------------------------------
# Jean-Paul Rigault --- Mais 2008
#-------------------------------------------------------------------

# On cherche sur quel processeur/OS on est
UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
  CC = /usr/bin/gcc-4.2
  ARCH = -arch i386 -arch x86_64
  ARCHNAME = macosx
else ifeq ($(UNAME), Linux)
  CC = gcc
  ARCH = 
  ARCHNAME = linux-$(shell uname -m)
else
  $(error "Architecture non supportée: " $(UNAME))
endif

# Commandes
CFLAGS = -std=c99 -Wall -g $(ARCH)
LDFLAGS = $(ARCH)
MKDEPEND = $(CC) -MM
AR = ar
RANLIB = ranlib
DOXYGEN = doxygen

# Fichiers
HDR = $(wildcard *.h)

# CHANGER LA DÉFINITION DE CETTE VARIABLE POUR Y INDIQUER VOS PROPRES MODULES
USERSRC = 
USEROBJ = $(patsubst %.c,%.o,$(USERSRC))

PROG = test_simul
LIB = libsimul.a

# Cibles principales

all : depend.out $(PROG)

$(PROG) : $(PROG).o $(USEROBJ) $(LIB) 
	$(CC) $(LDFLAGS) -o $@ $^

# Cibles annexes

endian : .FORCE
	cd Endian; $(MAKE)

doc : $(wildcard *h) $(wildcard *.c) $(wildcard *.dox) Doxyfile
	$(DOXYGEN)

# Nettoyage

clean : all
	-rm $(wildcard *.o) dump.bin

clobber : .FORCE
	-rm $(wildcard *.o) $(PROG) dump.bin depend.out 

clean_doc : .FORCE
	-rm -rf doc

# Cibles secondaires

depend.out : 
	$(MKDEPEND) $(wildcard *.c) > depend.out

.FORCE : 

# Dépendances (le fichier depend.out est généré par "make depend" Note; les
# dépendances peuvent être générées automatiquement par gcc. Ici l'entrée "make
# depend" permet de créer un fichier qui les contient. Bien entendu il *faut*
# réexécuter cette commande chaque fois que l'on modifie l'agencement des
# fichiers sources : création, destruction, modification des inclusions... En
# fait, ici, on recrée ce ficheir à chaque invocation de make.

depend : 
	-rm depend.out
	$(MAKE) depend.out

include depend.out
