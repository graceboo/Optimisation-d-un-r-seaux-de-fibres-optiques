#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Chaine.h"
#include "SVGwriter.h"


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
    char chaine[512];
    char buffer[2500];
    char *res=fgets(buffer,2500,f);
    if(res!=NULL){
        sscanf(res,"%s %d",chaine,&nbChaine);
        res= fgets(buffer,2500,f);
    }
    if(res!=NULL){  
        sscanf(res,"%s %d",chaine,&gamma);
        res= fgets(buffer,2500,f);
    }
    int num;
    int nbPoints;
    char lpoints[2500];
    char rest[2500];
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
        res=fgets(buffer,2500,f);
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


