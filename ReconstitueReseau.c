#include <stdio.h>
#include <stdlib.h>
#include "Reseau.h"
#include "Chaine.h"
#include "SVGwriter.h"
#include "Hachage.h"

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
    Reseau * r=reconstitueReseauListe(c);

    //ouvrir le fichier pour l'écriture 
    FILE *fe=fopen("reseau.cha","w");
    ecrireReseau(r,fe);
    afficheReseauSVG(r,"fichiersvg");
    liberer_Chaines(c);
    libererreseau(r);
    fclose(fe);
    fclose(fl);
    int x,y;
    for(x=1;x<10;x++){
        for(y=1;y<10;y++){
            printf("x= %d y=%d res= %d\n", x,y,clef(x,y));
        }
    }
    return 0;


}