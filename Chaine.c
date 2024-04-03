#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "SVGwriter.h"
#include "Reseau.h"
#include "Chaine.h"
#include "Hachage.h"
#include "ArbreQuat.h"
//hello
CellPoint* creer_point(double x, double y){
    CellPoint *c =malloc(sizeof(CellPoint));
    c->x=x;
    c->y=y;
    c->suiv=NULL;
    return c;
}

void inserer_point(CellPoint* p,double x,double y){
    CellPoint* p1=creer_point(x,y);
 
    
    CellPoint* courant=p;
    while(courant->suiv!=NULL){
        courant=courant->suiv;
    }
    courant->suiv=p1;
}
CellChaine* creer_CellChaine(int num,CellPoint *p){
    CellChaine *c=malloc(sizeof(CellChaine));
    c->numero=num;
    c->points=p;
    c->suiv=NULL;
    return c;
}

void inserer_chaine(CellChaine* c,int num,CellPoint *p1){
    CellChaine *c1=creer_CellChaine(num,p1);
   
    CellChaine *courant=c;
    while(courant->suiv!=NULL){
        courant=courant->suiv;
    }
    courant->suiv=c1;

}
Chaines* creer_chaines(int nbChaines, int gamma,CellChaine *chaines){
    Chaines *c= malloc(sizeof(Chaines));
    c->nbChaines=nbChaines;
    c->gamma=gamma;
    c->chaines=chaines;
    return c;
}

Chaines* lectureChaines(FILE *f){
    int nbChaine;
    int gamma;
    char chaine[256];
    char buffer[256];
    char *res=fgets(buffer,256,f);
    if(res!=NULL){
        sscanf(res,"%s %d",chaine,&nbChaine);
        res= fgets(buffer,256,f);
    }
    if(res!=NULL){  
        sscanf(res,"%s %d",chaine,&gamma);
        res= fgets(buffer,256,f);
    }
    int num;
    int nbPoints;
    char lpoints[256];
    char rest[256];
    CellPoint *p=NULL;
    CellChaine *chaines=NULL;
    double x,y;
    while(res!=NULL){
        sscanf(res,"%d %d %[^\n]",&num,&nbPoints,lpoints);
        for(int i=0;i<nbPoints;i++){
            sscanf(lpoints,"%lf %lf %[^\n]",&x,&y,rest);
            strcpy(lpoints,rest);
            if(p == NULL){
                p=creer_point(x,y);
            }
            else{
                inserer_point(p,x,y);
            }
        }
        if(chaines==NULL){
            chaines=creer_CellChaine(num,p);
        }else{
            inserer_chaine(chaines,num,p);
        }
        p=NULL;
        res=fgets(buffer,256,f);
    }
    Chaines *c=creer_chaines(nbChaine,gamma,chaines);
    return c;
}
void liberer_CellPoint(CellPoint *p){
    free(p);
}
void liberer_CellChaine(CellChaine *c){
    CellPoint *tmp=c->points;
    CellPoint *suiv;
    while (tmp != NULL){
        suiv=tmp->suiv;
        liberer_CellPoint(tmp);
        tmp=suiv;
    }
    free(c);
    
}

void liberer_Chaines(Chaines *c){
    CellChaine * courant=c->chaines;
    CellChaine *suiv;
    while(courant != NULL){
        suiv=courant->suiv;
        liberer_CellChaine(courant);
        courant=suiv;
    }
    free(c);
}

int nb_points(CellPoint *p){
    CellPoint * courant=p;
    int nb=0;
    while(courant!=NULL){
        nb++;
        courant=courant->suiv;
    }
    return nb;
}
void ecrireChaines(Chaines *C, FILE *f){
    fprintf(f,"%s %d\n","NbChain:",C->nbChaines);
    fprintf(f,"%s %d\n","Gamma:",C->gamma);
    CellChaine *courant=C->chaines;
    while(courant != NULL){
        CellPoint* p=courant->points;
        fprintf(f,"%d %d ",courant->numero,nb_points(p));
        while(p != NULL){
            fprintf(f,"%.2lf %.2lf ",p->x,p->y);
            p=p->suiv;
        }
        fprintf(f,"\n");
        courant=courant->suiv;
    }

}

void afficheChainesSVG(Chaines *C, char* nomInstance){

    double maxx=0,maxy=0,minx=1e6,miny=1e6;
    CellChaine *ccour;
    CellPoint *pcour;
    double precx,precy;
    SVGwriter svg;
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        while (pcour!=NULL){
            if (maxx<pcour->x) maxx=pcour->x;
            if (maxy<pcour->y) maxy=pcour->y;
            if (minx>pcour->x) minx=pcour->x;
            if (miny>pcour->y) miny=pcour->y;  
            pcour=pcour->suiv;
        }
    ccour=ccour->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        SVGlineRandColor(&svg);
        SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny)); 
        precx=pcour->x;
        precy=pcour->y;  
        pcour=pcour->suiv;
        while (pcour!=NULL){
            SVGline(&svg,500*(precx-minx)/(maxx-minx),500*(precy-miny)/(maxy-miny),500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            precx=pcour->x;
            precy=pcour->y;    
            pcour=pcour->suiv;
        }
        ccour=ccour->suiv;
    }
    SVGfinalize(&svg);
}

double longueurChaine(CellChaine *c){
    CellPoint *p=c->points;
    double longueur=0;
    double xa,ya,xb,yb;
    while (p->suiv!=NULL){   
        xa=p->x;
        ya=p->y;
        xb=p->suiv->x;
        yb=p->suiv->y;
        longueur+=sqrt((xb-xa)*(xb-xa)+(yb-ya)*(yb-ya));
        p=p->suiv;

    }
    return longueur;
    
}
double longueurTotale(Chaines *C){
    CellChaine *courant=C->chaines;
    double total=0;
    while(courant != NULL){
        total+=longueurChaine(courant);
        courant=courant->suiv;
    }
    return total;
}

int comptePointsTotal(Chaines *C){
    int nb=0;
    CellChaine *courant=C->chaines;
    while(courant != NULL){
        nb+=nb_points(courant->points);
        courant=courant->suiv;
    }
    return nb;
}

CellNoeud *creer_CellNoeuds(Noeud *n){
    CellNoeud * new=malloc(sizeof(CellNoeud));
    new->nd=n;
    new->suiv=NULL;
    return new;
}
#include <stdlib.h> // Pour pouvoir utiliser free()

/* Fonction pour libérer une liste de CellNoeud */
void libererCellNoeud(CellNoeud *cell) {
    while (cell != NULL) {
        CellNoeud *temp = cell;
        cell = cell->suiv;
        free(temp);
    }
}

/* Fonction pour libérer une liste de CellCommodite */
void libererCellCommodite(CellCommodite *cell) {
    while (cell != NULL) {
        CellCommodite *temp = cell;
        cell = cell->suiv;
        free(temp);
    }
}

/* Fonction pour libérer un réseau */
void libererreseau(Reseau *R) {
    if (R == NULL) return;

    // Libérer la liste des nœuds
    libererCellNoeud(R->noeuds);

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
        printf("\n\nnoeud %d: %lf,%lf voisins:\n",courant->nd->num,courant->nd->x,courant->nd->y);
        voisin=courant->nd->voisins;
        while(voisin!=NULL){
                nbvoisins++;
                printf("%d,%lf,%lf**\n",voisin->nd->num,voisin->nd->x,voisin->nd->y);
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
int clef(double x,double y){
    return (int)( y+(x+y)*(x+y+1)/2);
}
int hachage(int k,int M){
    double A=(sqrt(5)-1)/2;
    return (int)(M*(k*A - (int)(k*A)));

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

void chaineCoordMinMax(Chaines* C,double* xmin ,double* ymin,double* xmax,double*ymax){
    *xmin=__DBL_MAX__;
    *ymin=__DBL_MAX__;
    *xmax=__DBL_MAX__;
    *ymax=__DBL_MAX__;
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
    ArbreQuat * arbre=malloc(sizeof(ArbreQuat));
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
