#ifndef __HACH_H__
#define __HACH_H__	
#include "Reseau.h"

typedef struct{
  int nbElement; //pas necessaire ici
  int tailleMax;
  CellNoeud** T;
} TableHachage ;

int clef(double x,double y);
int hachage(int k,int M);

TableHachage * creer_TableHachage(int M);
Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage*H, double x, double y);
Reseau* reconstitueReseauHachage(Chaines *C, int M);
void liberer_TableHachage(TableHachage * t);
#endif