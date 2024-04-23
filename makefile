CFLAGS= -Wall -Werror 
CC=gcc

all: ChaineMain ReconstitueReseau Analyse Graphe

ReconstitueReseau: Chaine.o Reseau.o Hachage.o ArbreQuat.o ReconstitueReseau.o SVGwriter.o 
	$(CC) $(CFLAGS) -ggdb -o ReconstitueReseau Chaine.o Reseau.o Hachage.o ArbreQuat.o SVGwriter.o ReconstitueReseau.o -lm 
ChaineMain: Chaine.o ChaineMain.o SVGwriter.o
	$(CC) $(CFLAGS) -ggdb -o ChaineMain Chaine.o SVGwriter.o ChaineMain.o -lm
Analyse: Chaine.o Reseau.o Hachage.o ArbreQuat.o Analyse.o SVGwriter.o
	$(CC) $(CFLAGS) -ggdb -o Analyse  Chaine.o Reseau.o Hachage.o ArbreQuat.o Analyse.o SVGwriter.o -lm
Graphe:Graphe.o Reseau.o Chaine.o Struct_File.o SVGwriter.o
	$(CC) $(CFLAGS) -ggdb -o Graphe Struct_File.o Reseau.o Graphe.o Chaine.o SVGwriter.o -lm
Chaine.o:Chaine.c Chaine.h SVGwriter.h
	$(CC) $(CFLAGS) -c Chaine.c 
Reseau.o: Reseau.c Reseau.h Chaine.h SVGwriter.h
	$(CC) $(CFLAGS) -c Reseau.c 
Hachage.o:Hachage.c Hachage.h 
	$(CC) $(CFLAGS) -c Hachage.c 
ArbreQuat.o:ArbreQuat.c ArbreQuat.h Chaine.h Reseau.h
	$(CC) $(CFLAGS) -c ArbreQuat.c 
Graphe.o: Graphe.c Graphe.h Reseau.h Chaine.h Struct_File.h SVGwriter.h
	$(CC) $(CFLAGS) -c Graphe.c
Struct_File.o: Struct_File.c Struct_File.h
	$(CC) $(CFLAGS) -c Struct_File.c
Analyse.o:Analyse.c Analyse.h Chaine.h Reseau.h Hachage.h ArbreQuat.h
	$(CC) $(CFLAGS) -c Analyse.c 	
SVGwriter.o: SVGwriter.c SVGwriter.h 
	$(CC) $(CFLAGS) -c SVGwriter.c 
ChaineMain.o: ChaineMain.c Chaine.h 
	$(CC) $(CFLAGS) -c ChaineMain.c
ReconstitueReseau.o: ReconstitueReseau.c Reseau.h ArbreQuat.h Chaine.h SVGwriter.h Hachage.h Graphe.h
	$(CC) $(CFLAGS) -c ReconstitueReseau.c

clean:
	rm -f *.o ChaineMain