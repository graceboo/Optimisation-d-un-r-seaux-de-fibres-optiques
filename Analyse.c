#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ArbreQuat.h"
#include "Hachage.h"
#include "Reseau.h"
#include "time.h"
#include "Analyse.h"
#define NB_INSTANCES 5 // Nombre d'instances à tester
#define TAILLE_TABLE 300 // Taille de la table de hachage à varier 



void mesurerTempsExecution(FILE *flec,FILE *fecr) {
        Chaines *instance = lectureChaines(flec);

        //Temps pour la reconstitution par liste 
        clock_t start = clock();
        Reseau *reseau_liste = reconstitueReseauListe(instance);
        clock_t end = clock();
        double temps_liste = ((double) (end - start)) / CLOCKS_PER_SEC;

        //Temps pour la table de hachage
        start = clock();
        Reseau *reseau_hachage = reconstitueReseauHachage(instance, TAILLE_TABLE);
        end = clock();
        double temps_hachage = ((double) (end - start)) / CLOCKS_PER_SEC;

        //Temps pour les arbres
        start = clock();
        Reseau *reseau_arbre = reconstitueReseauArbre(instance);
        end = clock();
        double temps_arbre = ((double) (end - start)) / CLOCKS_PER_SEC;

        fprintf(fecr, "%.8lf\t%.8lf\t%.8lf\n", temps_liste, temps_hachage, temps_arbre);
    
        liberer_Chaines(instance);
        libererreseau(reseau_liste);
        libererreseau(reseau_hachage);
        libererreseau(reseau_arbre);
 }
void mesurerTempsExecution_liste(FILE *flec,FILE *fecr,int nbpointchaine,int nbchaines) {
        Chaines *instance = lectureChaines(flec);

        //Temps pour la reconstitution par liste 
        clock_t start = clock();
        Reseau *reseau_liste = reconstitueReseauListe(instance);
        clock_t end = clock();
        double temps_liste = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(fecr, "%d\t%.8lf\n",nbpointchaine*nbchaines, temps_liste);
    
        liberer_Chaines(instance);
        libererreseau(reseau_liste);
 }
void mesurerTempsExecutionArbreQuat_Hachage(FILE *flec,FILE *fecr,int nbpointchaine,int nbchaines) {
        Chaines *instance = lectureChaines(flec);

        //Temps pour la table de hachage
        double start = clock();
        Reseau *reseau_hachage = reconstitueReseauHachage(instance, TAILLE_TABLE);
        double end = clock();
        double temps_hachage = ((double) (end - start)) / CLOCKS_PER_SEC;
        
        //Temps pour les arbres
        start = clock();
        Reseau *reseau_arbre = reconstitueReseauArbre(instance);
        end = clock();
        double temps_arbre = ((double) (end - start)) / CLOCKS_PER_SEC;

        fprintf(fecr, "%d\t%.8lf\t%.8lf\n",nbpointchaine*nbchaines, temps_hachage, temps_arbre);
    
        liberer_Chaines(instance);
        libererreseau(reseau_hachage);
        libererreseau(reseau_arbre);
 }
/* Fonction pour générer un nombre aléatoire entre 0 et max */
double randCoord(double max) {
    return (double)rand() / RAND_MAX * max;
}


/* Fonction pour générer des chaînes aléatoires */
Chaines* generationAleatoire(int nbChaines, int nbPointsChaine, double xmax, double ymax) {
    Chaines *chaines = (Chaines*)malloc(sizeof(Chaines));
    if (chaines == NULL) {
        fprintf(stderr, "Erreur d'allocation de mémoire pour Chaines.\n");
        exit(EXIT_FAILURE);
    }

    chaines->gamma =3; /*à varier de changer pour limiter le nombre de fibre qui y passe */
    chaines->nbChaines = nbChaines;
    chaines->chaines = NULL;

    srand(time(NULL)); /* Initialisation du générateur de nombres aléatoires */

    /* Génération de chaque chaîne */
    for (int i = 0; i < nbChaines; i++) {
        CellChaine *chaine = (CellChaine*)malloc(sizeof(CellChaine));
        if (chaine == NULL) {
            fprintf(stderr, "Erreur d'allocation de mémoire pour CellChaine.\n");
            exit(EXIT_FAILURE);
        }
        chaine->numero = i + 1;
        chaine->points = NULL;

        /* Génération aléatoire des points */
        for (int j = 0; j < nbPointsChaine; j++) {
            double x = randCoord(xmax);
            double y = randCoord(ymax);
            CellPoint *point = (CellPoint*)malloc(sizeof(CellPoint));
            if (point == NULL) {
                fprintf(stderr, "Erreur d'allocation de mémoire pour CellPoint.\n");
                exit(EXIT_FAILURE);
            }
            point->x = x;
            point->y = y;
            point->suiv = chaine->points;
            chaine->points = point;
        }

        chaine->suiv = chaines->chaines;
        chaines->chaines = chaine;
    }

    return chaines;
}

int main(int argc, char** argv) {
    char* nfl;
    char*nfe;
    char* fichier_liste = "temps_liste.txt";
    char* fichier_table_arbre = "temps_table_arbre.txt";
    if (argc ==3) {
        nfl=argv[1];
        nfe=argv[2];
    } else {
        printf("argument manquant,usage: %s <fichier>\n", argv[0]);
        exit(1);
    }

    /*on ouvre le fichier pour la lecture */
    FILE *fl = fopen(nfl, "r");
    if (fl == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie.\n");
        return 1;
    }
    FILE *fe = fopen(nfe, "w");
    if (fe == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie.\n");
        return 1;
    }

    mesurerTempsExecution(fl,fe);

    /*on ouvre le fichier pour la l'écriture des temps de la liste chaînée */
    FILE *f2 = fopen(fichier_liste, "a");
    if (f2 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier pour les temps de liste chaînée.\n");
        return 1;
    }
    fprintf(f2, "# Nombre de points total\tTemps de calcul (s)\n");

    /*on ouvre le fichier pour la l'écriture des temps de la table de hachage et de l'arbre quaternaire */
    FILE *f3 = fopen(fichier_table_arbre, "a");
    if (f3 == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier pour les temps de table de hachage et arbre quaternaire.\n");
        return 1;
    }
    fprintf(f3, "# Nombre de points total\tTemps de calcul pour table de hachage (s)\tTemps de calcul pour arbre quaternaire (s)\n");

    /* Génération des données */
    int nbPointsChaine = 6;
    double xmax = 5000;
    double ymax = 5000;
    int pas = 500;
    int maxChaines = 5000;
    
    for (int nbChaines = pas; nbChaines <= maxChaines; nbChaines += pas) {
        // Génération des données pour la liste chaînée
        Chaines *inst = generationAleatoire(nbChaines, nbPointsChaine, xmax, ymax);
       
        
        // Convertir l'entier en chaîne de caractères
        char chaineX[20]; // Définir une taille suffisante pour la chaîne
        sprintf(chaineX, "%d", nbChaines);
        // Concaténer "chaine" avec la chaîne représentant x
        char resultat[30]; // Définir une taille suffisante pour la chaîne résultat
        strcpy(resultat, "instance");
        strcat(resultat, chaineX);

        FILE *f_chaine= fopen(resultat, "w");
        if (f_chaine == NULL) {
            fprintf(stderr, "Erreur lors de l'ouverture du fichier pour les temps de table de hachage et arbre quaternaire.\n");
            return 1;
        }

        ecrireChaines(inst,f_chaine);
        FILE* fichier_l_liste=fopen(resultat,"r");
        // Génération des données pour liste
        mesurerTempsExecution_liste(fichier_l_liste,f2,nbPointsChaine,nbChaines);
        fclose(fichier_l_liste);

        FILE* fichier_l_hachage_arbre=fopen(resultat,"r");
        // Génération des données pour la table de hachage
        mesurerTempsExecutionArbreQuat_Hachage(fichier_l_hachage_arbre,f3,nbPointsChaine,nbChaines);
        fclose(fichier_l_hachage_arbre);

        liberer_Chaines(inst);
        fclose(f_chaine);
    }

    fclose(fl);
    fclose(fe);
    fclose(f2);
    fclose(f3);

}
