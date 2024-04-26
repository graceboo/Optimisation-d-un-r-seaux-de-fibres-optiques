#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "SVGwriter.h"
#include "Hachage.h"
int clef(double x,double y){
    return (int)( y+(x+y)*(x+y+1)/2);
}
int hachage(int k,int M){
    double A=(sqrt(5)-1)/2;
    return (int)(M*(k*A - (int)(k*A)));

}
TableHachage * creer_TableHachage(int M){
    TableHachage * H=(TableHachage *)malloc(sizeof(TableHachage));
    H->nbElement=0;
    H->tailleMax=M;
    H->T=(CellNoeud** )malloc(sizeof(CellNoeud *)*M);
    for(int i=0;i<M;i++){
        H->T[i]=NULL;
    }
    return H;
}

Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage*H, double x, double y){
    int c=clef(x,y);
    int clefH=hachage(c,H->tailleMax);
    CellNoeud *l=H->T[clefH];
    while(l!=NULL){
        if(l->nd->x== x && l->nd->y==y){
            return l->nd;
        }
        l=l->suiv;
    }
    Noeud* n=creer_Noeud(R->nbNoeuds+1,x,y);
    CellNoeud* new=creer_CellNoeuds(n);
    inserer_CellNoeud_coor(&(H->T[clefH]),new);
    inserer_CellNoeud_coor(&(R->noeuds),new);
    R->nbNoeuds++;
    H->nbElement++;
    return n;

}

void liberer_TableHachage(TableHachage * t){
    free(t->T);
    free(t);
}
Reseau* reconstitueReseauHachage(Chaines *C, int M){
    Reseau * r=malloc(sizeof(Reseau));
    r->gamma=C->gamma;
    r->nbNoeuds=0;
    r->commodites=NULL;
    r->noeuds=NULL;  /* ensemble V */
    Noeud * extrA;
    Noeud * precedent;
    Noeud * extrB;
    CellChaine * courant=C->chaines;
    TableHachage* H=creer_TableHachage(M);
    //on parcourt toutes les chaines 
    while(courant !=NULL ){
        CellPoint * p=courant->points;
        precedent=NULL;
        Noeud * n;
        while( p!=NULL){
            n=rechercheCreeNoeudHachage(r,H,p->x,p->y);
            if(precedent != NULL){
                if(p->suiv == NULL){
                    extrB=n;
                }
                if(noeud_existe(n->voisins,precedent->x,precedent->y) == 1){
                    inserer_CellNoeud_coor(&(n->voisins), creer_CellNoeuds(precedent));
                }
                if(noeud_existe(precedent->voisins,n->x,n->y) == 1){
                    inserer_CellNoeud_coor(&(precedent->voisins), creer_CellNoeuds(n));
                }
            }else{
                extrA=n;
            }
            precedent=n;
            p=p->suiv;

        }
        inserer_CellCommodite(&(r->commodites),creer_CellCommodite(extrA,extrB));
        courant=courant->suiv;
    }
    liberer_TableHachage(H);
    return r;
}

