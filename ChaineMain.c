#include<stdio.h>
#include<stdlib.h>
#include"Chaine.h"
#include"SVGwriter.h"

int main(int argc,char ** argv ){
    char* fichier;
    if(argc==2){
        fichier=argv[1];
    }else{
        printf("argument manquant,usage: %s<fichier>",argv[0]);
        exit(1);
    }

    /*on ouvre le fichier pour la lecture */
    FILE *fl=fopen(fichier,"r");
    Chaines *c=lectureChaines(fl);
    
    /*On demande un nom pour le fichier d'écriture*/
    char buffer[256];
    char nom_fic[256];
    printf("Veuillez passer le nom du fichier à créer");
    fgets(buffer,256,stdin);
    sscanf(buffer,"%s",nom_fic);

    //ouvrir le fichier pour l'écriture 
    FILE *fe=fopen(nom_fic,"w");
    ecrireChaines(c,fe);

    afficheChainesSVG(c,"fichiersvg");


    printf("la longueur totale des chaines :%.2lf\nle nombre total de points:%d\n",longueurTotale(c),comptePointsTotal(c));
    fclose(fe);
    fclose(fl);
    liberer_Chaines(c);
    return 1;
}
