#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Reseau.h"
#include "SVGwriter.h"


CellNoeud *creer_CellNoeuds(Noeud *n){
    CellNoeud * new=malloc(sizeof(CellNoeud));
    new->nd=n;
    new->suiv=NULL;
    return new;
}

/* Fonction pour libérer une liste de CellNoeud */
void libererCellNoeud(CellNoeud *cell) {
    CellNoeud * c=cell;
    CellNoeud *suiv;
    while (c != NULL) {
        suiv= c->suiv;
        free(c);
        c=suiv;
    }
}

/* Fonction pour libérer une liste de CellCommodite */
void libererCellCommodite(CellCommodite *cell) {
    CellCommodite *c=cell;
    CellCommodite *suiv;
    while (c != NULL) {
        suiv=c->suiv;
        free(c);
        c=suiv;
    }
}
void liberer_Noeud(Noeud *n){
    libererCellNoeud(n->voisins);
    free(n);
}

/* Fonction pour libérer un réseau */
void libererreseau(Reseau *R) {
    if (R == NULL) return;
    // Libérer la liste des nœuds
    CellNoeud * courant=R->noeuds;
    CellNoeud *suiv;
    while(courant){
        suiv=courant->suiv;
        liberer_Noeud(courant->nd);
        free(courant);
        courant=suiv;
    }

    // Libérer la liste des commodités
    libererCellCommodite(R->commodites);

    // Enfin, libérer la structure du réseau elle-même
    free(R);
}




Noeud *creer_Noeud(int num,double x,double y){
    Noeud * n=malloc(sizeof(Noeud));
    n->num=num;
    n->x=x;
    n->y=y;
    n->voisins=NULL;
    return n;
}

CellCommodite * creer_CellCommodite(Noeud * extraA,Noeud * extraB){
    CellCommodite* new=malloc(sizeof(CellCommodite));
    new->extrA=extraA;
    new->extrB=extraB;
    new->suiv=NULL;
    return new;
}   

void inserer_CellCommodite(CellCommodite ** liste,CellCommodite *c){
    if(*liste==NULL){
        *liste=c;
        return;
    }
    CellCommodite *courant = *liste;
    while (courant->suiv != NULL) {
        courant = courant->suiv;
    }
    courant->suiv = c; // Maintenant, courant pointe vers le dernier élément de la liste, nous ajoutons donc c après cela.
    c->suiv = NULL; // Assurer que c est maintenant le dernier élément de la liste.
}

void inserer_CellNoeud_coor(CellNoeud ** liste,CellNoeud *c){
    if(*liste==NULL){
        *liste=c;
        return;
    }
    // Vérifier si c est différent du premier élément de *liste
    if (((*liste)->nd->x != c->nd->x) || ((*liste)->nd->y != c->nd->y)) {
        CellNoeud *courant = *liste;
        while (courant->suiv != NULL) {
            if (courant->suiv->nd->x == c->nd->x && courant->suiv->nd->y == c->nd->y) {
                return; // Si c est déjà dans la liste, nous n'avons rien à faire, donc nous sortons de la fonction.
            }
            courant = courant->suiv;
        }
        courant->suiv = c; // Maintenant, courant pointe vers le dernier élément de la liste, nous ajoutons donc c après cela.
        c->suiv = NULL; // Assurer que c est maintenant le dernier élément de la liste.
    }
}

Noeud *rechercheCreeNoeudListe(Reseau * R,double x,double y){
    CellNoeud *n=R->noeuds;
    while(n != NULL){
        if(n->nd->x == x && n->nd->y==y){
            return n->nd;
        }
        n=n->suiv;
    }
    // Si le nœud n'a pas été trouvé, alors nous le créons.
    Noeud *new = creer_Noeud(R->nbNoeuds + 1, x, y);
    CellNoeud *newCell = creer_CellNoeuds(new);

    // Insérer le nouveau nœud à la fin de la liste en utilisant la fonction inserer_CellNoeud
    inserer_CellNoeud_coor(&(R->noeuds), newCell);

    R->nbNoeuds += 1;
    return new;
}

Reseau * reconstitueReseauListe(Chaines *C){
    Reseau * r=malloc(sizeof(Reseau));
    r->gamma=C->gamma;
    r->nbNoeuds=0;
    r->commodites=NULL;
    r->noeuds=NULL;  /* ensemble V */
    Noeud * extrA;
    Noeud * precedent;
    Noeud * extrB;
    CellChaine * courant=C->chaines;
    //on parcourt toutes les chaines 
    while(courant !=NULL ){
        CellPoint * p=courant->points;
        precedent=NULL;
        Noeud * n;
        while( p!=NULL){
            n=rechercheCreeNoeudListe(r,p->x,p->y);
            if(precedent != NULL){
                if(p->suiv == NULL){
                    extrB=n;
                }
                inserer_CellNoeud_coor(&(n->voisins),creer_CellNoeuds(precedent));
                inserer_CellNoeud_coor(&(precedent->voisins),creer_CellNoeuds(n));
            }else{
                extrA=n;
            }
            precedent=n;
            p=p->suiv;

        }
        inserer_CellCommodite(&(r->commodites),creer_CellCommodite(extrA,extrB));
        courant=courant->suiv;
    }
    
    return r;
}
int nbLiaisons(Reseau *R){
    CellNoeud * courant=R->noeuds;
    CellNoeud * voisin;
    int nbvoisins=0;
    while(courant!=NULL){
       voisin=courant->nd->voisins;
        while(voisin!=NULL){
                nbvoisins++;
                //on le rajoute à liste 
                voisin=voisin->suiv;
        }
        courant=courant->suiv;
    }
    return nbvoisins/2;
}

int nbCommodites(Reseau *R){
    CellCommodite * courant =R->commodites;
    int nb=0;
    while(courant!=NULL){
        nb++;
        courant=courant->suiv;
    }
    return nb;
}
void ecrireReseau(Reseau *R, FILE *f){
    fprintf(f,"%s %d\n","NbNoeuds:",R->nbNoeuds);
    fprintf(f,"%s %d\n","NbLiaisons:",nbLiaisons(R));
    fprintf(f,"%s %d\n","NbCommodites:",nbCommodites(R));
    fprintf(f,"%s %d\n","Gamma:",R->gamma);
    fprintf(f,"\n");

    //on écrit les noeuds 
    CellNoeud * n=R->noeuds;
    while(n != NULL){
        fprintf(f,"v %d %lf %lf\n",n->nd->num,n->nd->x,n->nd->y);
        n=n->suiv;
    }
    fprintf(f,"\n");

    //on écrit les liaisons
    CellNoeud * courant=R->noeuds;
    CellNoeud * voisin;

    while(courant != NULL){
        voisin=courant->nd->voisins;
        while(voisin != NULL){
            if(courant->nd->num < voisin->nd->num){
                fprintf(f,"l %d %d\n",courant->nd->num,voisin->nd->num);
            }
            voisin=voisin->suiv;
        }
        courant=courant->suiv;    
    }
    fprintf(f,"\n");

    //on écrit les commodités 
    CellCommodite * c=R->commodites;
    while(c != NULL){
        fprintf(f,"k %d %d\n",c->extrA->num,c->extrB->num);
        c=c->suiv;
    }
}


void afficheReseauSVG(Reseau *R, char* nomInstance){
    CellNoeud *courN,*courv;
    SVGwriter svg;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;

    courN=R->noeuds;
    while (courN!=NULL){
        if (maxx<courN->nd->x) maxx=courN->nd->x;
        if (maxy<courN->nd->y) maxy=courN->nd->y;
        if (minx>courN->nd->x) minx=courN->nd->x;
        if (miny>courN->nd->y) miny=courN->nd->y;
        courN=courN->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    courN=R->noeuds;
    while (courN!=NULL){
        SVGpoint(&svg,500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
        courv=courN->nd->voisins;
        while (courv!=NULL){
            if (courv->nd->num<courN->nd->num)
                SVGline(&svg,500*(courv->nd->x-minx)/(maxx-minx),500*(courv->nd->y-miny)/(maxy-miny),500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
            courv=courv->suiv;
        }
        courN=courN->suiv;
    }
    SVGfinalize(&svg);
}
