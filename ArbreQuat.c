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


//5.3 insererNoeudArbre
void insererNoeudArbre(Noeud*n,ArbreQuat**a,ArbreQuat*parent){
	if (!n || !parent) return ;
	//cas de l'arbre vide
	
	if(*a==NULL){
		double newCoteX = parent -> coteX * 1.0 / 2;
        double newCoteY = parent -> coteY * 1.0 / 2;
        double newXc, newYc;

        if (n -> x < parent -> xc) {
            newXc = parent -> xc - parent -> coteX * 1.0 / 4;
        } else {
            newXc = parent -> xc + parent -> coteX * 1.0 / 4;
        }
        if (n -> y < parent -> yc) {
            newYc = parent -> yc - parent -> coteY * 1.0 / 4;
        } else {
            newYc = parent -> yc + parent -> coteY * 1.0 / 4;
        }

        (*a) = creerArbreQuat(newXc, newYc, newCoteX, newCoteY);
        /* Ajouter le noeud dans la cellule */
        (*a) -> noeud = n;

        return ;
	}
	//cas de la feuille 
	if((*a)->noeud!=NULL){
		Noeud *nd=(*a)->noeud;

		if (n->x >= (*a)->xc && n->y >= (*a)->yc){
			insererNoeudArbre(n, &((*a)->ne),*a);
		}
		if (n->x < (*a)->xc && n->y >= (*a)->yc){
			insererNoeudArbre(n, &((*a)->no),*a);
		}
		if (n->x >= (*a)->xc && n->y < (*a)->yc){
			insererNoeudArbre(n, &((*a)->se),*a);
		}
		if (n->x < (*a)->xc && n->y < (*a)->yc){
			insererNoeudArbre(n, &((*a)->so),*a);
		}


		if (nd->x >= (*a)->xc && nd->y >= (*a)->yc){
			insererNoeudArbre(nd, &((*a)->ne),*a);
		}
		if (nd->x >= (*a)->xc && nd->y < (*a)->yc){
			insererNoeudArbre(nd, &((*a)->se),*a);
		}
		if (nd->x < (*a)->xc && nd->y >= (*a)->yc){
			insererNoeudArbre(nd ,&((*a)->no),*a);
		}
		if (nd->x < (*a)->xc && nd->y < (*a)->yc){
			insererNoeudArbre(nd, &((*a)->so),*a);
		}
		
		(*a)->noeud=NULL;
		return;
		
		
	}
	//cas du noeud interne 
	if(n->x<(*a)->xc && n->y<(*a)->yc){
		insererNoeudArbre(n,&((*a)->so),*a);
	}
	if(n->x>=(*a)->xc && n->y<(*a)->yc){
		insererNoeudArbre(n,&((*a)->se),*a);
	}
	if(n->x<(*a)->xc && n->y>=(*a)->yc){
		insererNoeudArbre(n,&((*a)->no),*a);
	}
	if(n->x>=(*a)->xc && n->y>=(*a)->yc){
		insererNoeudArbre(n,&((*a)->ne),*a);
	}

}

//5.4 rechercheCreeNoeudArbre
Noeud * rechercheCreeNoeudArbre(Reseau* R,ArbreQuat **a,ArbreQuat *parent,double x, double y){
	ArbreQuat *abr=*a;

	//Premier cas : si l'arbre est vide
	if (abr==NULL){
		//on cree un nouveau noeud
		Noeud* n=creer_Noeud(R->nbNoeuds+1,x,y);

		//on l'insere dans la liste 
        CellNoeud *newCell = creer_CellNoeuds(n);
        inserer_CellNoeud_coor(&(R->noeuds), newCell);
		R->nbNoeuds++;

		//on l'insere dans l'arbreQuat
		insererNoeudArbre(n,&(*a),parent);

		return n;
	}

	//Deuxieme cas : si c'est une feuille
	if(abr->noeud!=NULL){
		if(abr->noeud->x==x && abr->noeud->y==y){
			return abr->noeud;
		}

		//on cree un nouveau noeud

		Noeud* n=creer_Noeud(R->nbNoeuds+1,x,y);

		//on l'insere dans la liste 
        CellNoeud *newCell = creer_CellNoeuds(n);
        inserer_CellNoeud_coor(&(R->noeuds), newCell);
		R->nbNoeuds++;

		//on l'insere dans l'arbreQuat
		insererNoeudArbre(n,&(*a),parent);

		return n;
	}

	//Troisieme cas : si c'est un noeud interne
	if(x<abr->xc && y<abr->yc){
		return rechercheCreeNoeudArbre(R,&(abr->so),abr,x,y);
	}
	if(x>=abr->xc && y<abr->yc){
		return rechercheCreeNoeudArbre(R,&(abr->se),abr,x,y);
	}
	if(x<abr->xc && y>=abr->yc){
		return rechercheCreeNoeudArbre(R,&(abr->no),abr,x,y);
	}
	if(x>=abr->xc && y>=abr->yc){
		return rechercheCreeNoeudArbre(R,&(abr->ne),abr,x,y);
	}
	return NULL;
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
    double xmin,xmax;
    double ymin,ymax;
    xmin=0;
    xmax=0;
    xmax=0;
    ymax=0;
    chaineCoordMinMax(C,&xmin,&ymin,&xmax,&ymax);
    printf("%lf,%lf  %lf,%lf\n\n",xmin,xmax,ymin,ymax);
    ArbreQuat* a=creerArbreQuat((xmax+xmin)/2.0,(ymax+ymin)/2.0,(xmax-xmin),(ymax-ymin));
    CellChaine * courant=C->chaines;
    //on parcourt toutes les chaines 
    while(courant !=NULL ){
        CellPoint * p=courant->points;
        precedent=NULL;
        Noeud * n;
        while( p!=NULL){
            n=rechercheCreeNoeudArbre(r,&a,a,p->x,p->y);
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
    if(a==NULL){
        printf("cest grave \n\n");
    }
    if(a->ne==NULL && a->no==NULL && a->se==NULL && a->so==NULL){
        printf("cest grave \n\n");
    }
    liberer_arbre(a);
    return r;
}
