CFLAGS= -Wall -Werror 
CC=gcc

all: ChaineMain ReconstitueReseau

ReconstitueReseau: Chaine.o ReconstitueReseau.o SVGwriter.o 
	$(CC) $(CFLAGS) -ggdb -o ReconstitueReseau Chaine.o SVGwriter.o ReconstitueReseau.o -lm

ChaineMain: Chaine.o ChaineMain.o SVGwriter.o
	$(CC) $(CFLAGS) -ggdb -o ChaineMain Chaine.o SVGwriter.o ChaineMain.o -lm

Chaine.o: Chaine.c Chaine.h Reseau.h Hachage.h
	$(CC) $(CFLAGS) -c Chaine.c 

SVGwriter.o: SVGwriter.c SVGwriter.h 
	$(CC) $(CFLAGS) -c SVGwriter.c 

ChaineMain.o: ChaineMain.c Chaine.h 
	$(CC) $(CFLAGS) -c ChaineMain.c
ReconstitueReseau.o: ReconstitueReseau.c 
	$(CC) $(CFLAGS) -c ReconstitueReseau.c

clean:
	rm -f *.o ChaineMain