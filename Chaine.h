#ifndef __CHAINE_H__
#define __CHAINE_H__	

/* Liste chainee de points */
typedef struct cellPoint{
  double x,y;                   /* Coordonnees du point */
  struct cellPoint *suiv;       /* Cellule suivante dans la liste */
} CellPoint;

/* Celllule d une liste (chainee) de chaines */
typedef struct cellChaine{
  int numero;                   /* Numero de la chaine */
  CellPoint *points;            /* Liste des points de la chaine */
  struct cellChaine *suiv;      /* Cellule suivante dans la liste */
} CellChaine;

/* L'ensemble des chaines */
typedef struct {
  int gamma;                    /* Nombre maximal de fibres par cable */
  int nbChaines;                /* Nombre de chaines */
  CellChaine *chaines;          /* La liste chainee des chaines */
} Chaines;

//lecture dans un fichier 
Chaines* lectureChaines(FILE *f);

//ecriture dans un fichier 
void ecrireChaines(Chaines *C, FILE *f);

//creer un fichier svg
void afficheChainesSVG(Chaines *C, char* nomInstance);

//longueur de toutes les chaines
double longueurTotale(Chaines *C);

//nombre de points de toutes les choints 
int comptePointsTotal(Chaines *C);

//creer un point 
CellPoint* creer_point(double x, double y);

//on insere un point de coordonnées (x,y) dans une liste chainée de points 
void inserer_point(CellPoint* p,double x,double y);

//creer une cellule de chaine
CellChaine* creer_CellChaine(int num,CellPoint *p);

//on insere une cellule de chaine (numero num,et liste chainée de points p1) dans une liste chainée de cellules de chaine
void inserer_chaine(CellChaine* c,int num,CellPoint *p1);

//creer  la structure Chaines
Chaines* creer_chaines(int nbChaines, int gamma,CellChaine *chaines);

//libere un point
void liberer_CellPoint(CellPoint *p);

//livbere une chaine
void liberer_CellChaine(CellChaine *c);

//libere 
void liberer_Chaines(Chaines *c);

//compte le nombres de points dans la liste chainée p
int nb_points(CellPoint *p);


#endif	
