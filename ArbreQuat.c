#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Chaine.h"
#include "ArbreQuat.h"
void chaineCoordMinMax(Chaines* C,double* xmin ,double* ymin,double* xmax,double*ymax){
    *xmin=1000;
    *ymin=1000;
    *xmax=0;
    *ymax=0;
    CellChaine *chaine=C->chaines ;
    CellPoint* point;
    /*on parcourt toutes les chaines*/
    while( chaine!=NULL){
        point=chaine->points;
        /*On parcourt tous les points de la chaine*/ 
        while(point!=NULL){ 
            if(point->x < *xmin) *xmin=point->x;
            if(point->y < *ymin) *ymin=point->y;
            if(point->x > *xmax) *xmax=point->x;
            if(point->y > *ymax) *ymax=point->y;
            point=point->suiv;
        }
        chaine=chaine->suiv;
    }
}

ArbreQuat* creerArbreQuat(double xc, double yc, double coteX,double coteY){
    ArbreQuat * arbre = malloc(sizeof(ArbreQuat));
    arbre->xc=xc;
    arbre->yc=yc;
    arbre->coteX=coteX;
    arbre->coteY=coteY;
    arbre->noeud=NULL;
    arbre->ne=NULL;
    arbre->no=NULL;
    arbre->se=NULL;
    arbre->so=NULL;
    return arbre;
}

 void insererNoeudArbre(Noeud* n, ArbreQuat** a, ArbreQuat* parent){
    if(*a == NULL){//si l'arbre est NULL
        double new_xc, new_yc, new_coteX, new_coteY;

        if (parent){
            new_coteX = parent->coteX / 2 ; // on declare coteX et coteY pour l'arbre
            new_coteY = parent->coteY / 2 ;

            //on determine la cellule ou inserer le noeud selon les coordonnees du noeud 
            if(n->x > parent->xc){  
                new_xc = parent->xc + parent->coteX / 4 ; 
            }else{ 
                new_xc = parent->xc - parent->coteX / 4 ;
            }
            if(n->y > parent->yc){
               new_yc = parent->yc + parent->coteY / 4 ;
            }else{
                new_yc = parent->yc - parent->coteY / 4 ;
            }
        }
    *a = creerArbreQuat(new_xc , new_yc , new_coteX , new_coteY); //on cree l'arbre 
    (*a)-> noeud = n; // on ajoute le noeud 
    }

    //Si le nœud doit être inséré au niveau d’une feuille de l’arbre
    else if ((*a)->noeud != NULL){
        Noeud * ancien_noeud = (*a)-> noeud ; // on sauvegarde le noeud deja présent dans la cellule 
        (*a)-> noeud = NULL ; // on libere la cellule 
        
        parent = (*a); //on perd le parent 
        if(n->x > (*a)-> xc ){
            if(n->y > (*a)-> yc){ 
                (*a)=(*a)->ne;//NE
            }else{
                (*a)=(*a)->se;//SE
            }
        }else{
            if(n->y > (*a)-> yc){
               (*a)=(*a)->no;//NO
            }else{
                (*a)=(*a)->so;//SO
            }
        }
        insererNoeudArbre (n, a, parent);  

        if(ancien_noeud->x > (*a)-> xc ){
            if(ancien_noeud->y > (*a)-> yc){ 
                (*a)=(*a)->ne;//NE
            }else{
                (*a)=(*a)->se;//SE
            }
        }else{
            if(ancien_noeud->y > (*a)-> yc){
               (*a)=(*a)->no;//NO
            }else{
                (*a)=(*a)->so;//SO
            }
        }
        insererNoeudArbre (ancien_noeud, a, parent);
    }
    else{

        if(n->x > (*a)-> xc ){
            if(n->y > (*a)-> yc){ 
                insererNoeudArbre(n,&((*a)->ne),*a);//NE
            }else{
                insererNoeudArbre(n,&((*a)->se),*a);//SE
            }
        }else{
            if(n->y > (*a)-> yc){
                insererNoeudArbre(n,&((*a)->no),*a);//NO
            }else{
                insererNoeudArbre(n,&((*a)->so),*a);//SO
            }
        }

    }

} 
Noeud* rechercheCreeNoeudArbre(Reseau* R, ArbreQuat** a, ArbreQuat* parent, double x, double y){
    if(!R || !a ){
        printf("Erreur : le réseau ou l'arbre est NULL\n");
        return NULL;
    }
    
    if (*a == NULL){
        Noeud* n = rechercheCreeNoeudListe(R,x,y);
        insererNoeudArbre(n, a, parent);
        return n ;
    }
    else if((*a)->noeud != NULL ){
        Noeud* n = (*a)->noeud;
        if (n->x == x && n->y == y) {
            return n ;
        }else{
            Noeud* n = rechercheCreeNoeudListe(R,x,y);
            insererNoeudArbre(n, a, parent);
            return n ;
        }
    }
    else{
        ArbreQuat ** noeud_cour = NULL ;// pour parcourir l'arbre
        if(x > (*a)-> xc ){
            if(y > (*a)-> yc){ 
                noeud_cour = &((*a)->ne);//NE
            }else{
                noeud_cour = &((*a)->se);//SE
            }
        }else{
            if(y > (*a)-> yc){
                noeud_cour = &((*a)->no);//NO
            }else{
                noeud_cour = &((*a)->so);//SO
            }
        }
        return rechercheCreeNoeudArbre(R, noeud_cour, *a, x, y);//déterminer dans quelle cellule de l’arbre chercher le nœud du réseau
    }

} 
void liberer_arbre(ArbreQuat * a){
    if(a==NULL) return;
    liberer_arbre(a->ne);
    liberer_arbre(a->no);
    liberer_arbre(a->se);
    liberer_arbre(a->so);
    free(a);
} 
 Reseau* reconstitueReseauArbre(Chaines* C){
    //réseau
    Reseau * r=malloc(sizeof(Reseau));
    r->gamma=C->gamma;
    r->nbNoeuds=0;
    r->commodites=NULL;
    r->noeuds=NULL;  /* ensemble V */
    Noeud * extrA;
    Noeud * precedent;
    Noeud * extrB;

    // arbre 
    ArbreQuat* a=NULL;
    double xmin,xmax;
    double ymin,ymax;
    chaineCoordMinMax(C,&xmin,&ymin,&xmax,&ymax);
    ArbreQuat* parent=creerArbreQuat((xmax-xmin)/2.0,(ymax-ymin)/2.0,(xmax-xmin),(ymax-ymin));
    ArbreQuat* pointeur_p;
    CellChaine * courant=C->chaines;
    //on parcourt toutes les chaines 
    while(courant !=NULL ){
        CellPoint * p=courant->points;
        precedent=NULL;
        Noeud * n;
        while( p!=NULL){
            n=rechercheCreeNoeudArbre(r,&a,parent,p->x,p->y);
            if(r->nbNoeuds==1){
                pointeur_p=a;
            }
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
    liberer_arbre(parent);
    if((pointeur_p->ne==NULL)&& (pointeur_p->no==NULL)&&(pointeur_p->se==NULL)&&(pointeur_p->so==NULL)){
        printf("c'est grave\n\n");
    }
    return r;
}
