#include <stdio.h>
#include <stdlib.h>
#include "Reseau.h"
#include "Chaine.h"
#include "Hachage.h"
#include "SVGwriter.h"
#include "ArbreQuat.h"


int main(int argc , char ** argv){
    char* fichier;
    //int n;
    if(argc==3){
        fichier=argv[1];
        //n=atoi(argv[2]);
    }else{
        printf("argument manquant usage: fichier,chiffre");
        exit(1);
    }

    //Reconstitue le réseau

    /*on ouvre le fichier pour la lecture */
    FILE *fl=fopen(fichier,"r");
    Chaines *c=lectureChaines(fl);
    
    Reseau * r1=reconstitueReseauListe(c);
    Reseau*  r2=reconstitueReseauHachage(c,200);
    Reseau*  r3=reconstitueReseauArbre(c);

    //ouvrir le fichier pour l'écriture 
    FILE *fe=fopen("reseau.cha","w");
    ecrireReseau(r1,fe);
    afficheReseauSVG(r1,"fichiersvg_liste");

    FILE *fe2=fopen("reseau2.cha","w");
    ecrireReseau(r2,fe2);
    afficheReseauSVG(r2,"fichiersvg_hachage");


    FILE *fe3=fopen("reseau3.cha","w");
    ecrireReseau(r3,fe3);
    afficheReseauSVG(r3,"fichiersvg_Arbre");

    liberer_Chaines(c);
    libererreseau(r1);
    libererreseau(r2);
    libererreseau(r3);


    fclose(fe);
    fclose(fe2);
    fclose(fe3);
    fclose(fl);


  //table de hachage
    int x,y;
    for(x=1;x<10;x++){
        for(y=1;y<10;y++){
            printf("x= %d y=%d res= %d\n", x,y,clef(x,y));
        }
    }
   return 0;


}