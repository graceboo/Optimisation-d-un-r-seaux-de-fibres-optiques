#include <stdlib.h>
#include <stdio.h>
#include "Reseau.h"
#include "Graphe.h"
#include "Chaine.h"
#include "Struct_File.h"
#include "SVGwriter.h"
Graphe* creerGraphe(Reseau* r){
    Graphe *g = (Graphe *)malloc(sizeof(Graphe));
    g->nbsom = r->nbNoeuds;
    g->T_som = (Sommet **)malloc(g->nbsom * sizeof(Sommet *));
    g->gamma = r->gamma;
    g->nbcommod = nbCommodites(r);
    g->T_commod = (Commod *)malloc(g->nbcommod * sizeof(Commod));
    CellNoeud *courant = r->noeuds;
    CellCommodite *c = r->commodites;
    while(courant) {
        g->T_som[courant->nd->num] = (Sommet *)malloc(sizeof(Sommet));
        g->T_som[courant->nd->num]->num = courant->nd->num;
        g->T_som[courant->nd->num]->x = courant->nd->x;
        g->T_som[courant->nd->num]->y = courant->nd->y;
        g->T_som[courant->nd->num]->L_voisin = NULL;

        CellNoeud *voisin = courant->nd->voisins;

        while(voisin != NULL) {
            Cellule_arete *ca = (Cellule_arete *)malloc(sizeof(Cellule_arete));
            ca->suiv = NULL; // Initialisation du pointeur suivant

            if(courant->nd->num < voisin->nd->num) {
                ca->a = (Arete *)malloc(sizeof(Arete));
                ca->a->u = courant->nd->num;
                ca->a->v = voisin->nd->num;
            } else {
                Cellule_arete *voi_v = g->T_som[voisin->nd->num]->L_voisin;
                while(voi_v != NULL) {
                    if(voi_v->a->v == courant->nd->num) {
                        ca->a = voi_v->a;
                        break; // Sortie de la boucle
                    }
                    voi_v = voi_v->suiv;
                }
            }
            // Insertion en tête
            ca->suiv = g->T_som[courant->nd->num]->L_voisin;
            g->T_som[courant->nd->num]->L_voisin = ca;

            voisin = voisin->suiv;
        }
        courant = courant->suiv;
    }

    for(int i = 0; i < g->nbcommod && c != NULL; i++) {
        g->T_commod[i].e1 = c->extrA->num;
        g->T_commod[i].e2 = c->extrB->num;
        c = c->suiv;
    }
    return g;
}


int* cheminDeUV(int u, int v, int* predecesseurs) {
    // Initialisez une liste pour stocker le chemin de u à v
    int* chemin = (int*)malloc(sizeof(int));
    int sommet = v;
    int taille = 0;

    // Remontez depuis v jusqu'à u en suivant les prédécesseurs
    while (sommet != -1 && sommet != u) {
        chemin[taille++] = sommet;
        sommet = predecesseurs[sommet];
    }

    // Si sommet est -1, cela signifie qu'il n'y a pas de chemin de u à v
    if (sommet == -1) {
        printf("Il n'y a pas de chemin de %d à %d\n", u, v);
        free(chemin);
        return NULL;
    }

    // Ajoutez le sommet de départ u au chemin
    chemin[taille++] = u;

    // Inversez l'ordre des sommets dans le chemin
    int i, j;
    for (i = 0, j = taille - 1; i < j; i++, j--) {
        int temp = chemin[i];
        chemin[i] = chemin[j];
        chemin[j] = temp;
    }

    // Retournez le chemin
    return chemin;
}

int* plusPetitNombreAretes(Graphe *g, int u, int v) {
    int *distances = (int*)malloc(g->nbsom * sizeof(int));
    int *predecesseurs=(int*)malloc(g->nbsom*sizeof(int));
    int i;

    for (i = 0; i < g->nbsom; i++) {
        distances[i] = -1; // -1 pour représenter l'infini
        predecesseurs[i] = -1; // Pas de prédecesseur pour le moment(pour stocker l'arborescence)
    }

    distances[u] = 0; // Distance de u à u est 0

    S_file file;
    Init_file(&file);
    enfile(&file, u);

    while (!estFileVide(&file)) {
        int sommetCourant = defile(&file);
        Cellule_arete *voisin = g->T_som[sommetCourant]->L_voisin;
        while (voisin != NULL) {
            if (distances[voisin->a->v] == -1) {
                distances[voisin->a->v] = distances[sommetCourant] + 1;
                predecesseurs[voisin->a->v] = sommetCourant;
                enfile(&file, voisin->a->v);
            }
            voisin = voisin->suiv;
        }
    }

    // Libération de la mémoire
    free(distances);

    return cheminDeUV(u,v,predecesseurs);
}
int reorganiseReseau(Reseau* r) {
    // Création du graphe correspondant au réseau
    Graphe* g = creerGraphe(r);

    // Calcul de la plus courte chaîne pour chaque commodité
    // Initialisation de la matrice pour compter le nombre de chaînes par arête
    int** nbChainsParArete = (int**)malloc(g->nbsom * sizeof(int*));
    for (int i = 0; i < g->nbsom; i++) {
        nbChainsParArete[i] = (int*)calloc(g->nbsom, sizeof(int));
    }

    // Calcul de la plus courte chaîne pour chaque commodité
    for (int i = 0; i < g->nbcommod; i++) {
        int e1 = g->T_commod[i].e1;
        int e2 = g->T_commod[i].e2;
        int* predecesseurs = plusPetitNombreAretes(g, e1, e2);
        int sommet = e2;
        while (predecesseurs!=NULL&&sommet != -1 && sommet != e1) {
            nbChainsParArete[sommet][predecesseurs[sommet]]++;
            nbChainsParArete[predecesseurs[sommet]][sommet]++;
            sommet = predecesseurs[sommet];
        }
        free(predecesseurs);
    }

    // Vérification du nombre de chaînes passant par chaque arête
    int passentMoinsQueGamma = 1; // On suppose d'abord que toutes les arêtes satisfont la condition
    for (int i = 0; i < g->nbsom; i++) {
        for (int j = 0; j < g->nbsom; j++) {
            if (nbChainsParArete[i][j] >= g->gamma) {
                passentMoinsQueGamma = 0;
                break;
            }
        }
        if (!passentMoinsQueGamma) break;
    }

    // Libération de la mémoire allouée pour le graphe
    for (int i = 0; i < g->nbsom; i++) {
        Cellule_arete* voisin = g->T_som[i]->L_voisin;
        while (voisin != NULL) {
            Cellule_arete* temp = voisin;
            voisin = voisin->suiv;
            free(temp->a);
            free(temp);
        }
        free(g->T_som[i]);
    }
    free(g->T_som);
    free(g->T_commod);
    free(g);

    return passentMoinsQueGamma;
}
int main(int argc , char ** argv){
    char* fichier;
    if(argc==2){
        fichier=argv[1];
    }else{
        printf("argument manquant usage: fichier");
        exit(1);
    }
    /*on ouvre le fichier pour la lecture */
    FILE *fl=fopen(fichier,"r");
    Chaines *c=lectureChaines(fl);
    //recupération d'un réseau par quelconque méthode
    Reseau * r1=reconstitueReseauListe(c);

    int resultat1 = reorganiseReseau(r1);
    printf("Le résultat pour le réseau est : %d\n",resultat1);

    return 0;

}