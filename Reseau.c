#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Reseau.h"
#include "SVGwriter.h"

CellNoeud *creer_CellNoeuds(Noeud *n)
{
    CellNoeud *new = malloc(sizeof(CellNoeud));
    new->nd = n;
    new->suiv = NULL;
    return new;
}

Noeud *creer_Noeud(int num, double x, double y)
{
    Noeud *n = malloc(sizeof(Noeud));
    n->num = num;
    n->x = x;
    n->y = y;
    n->voisins = NULL;
    return n;
}

CellCommodite *creer_CellCommodite(Noeud *extraA, Noeud *extraB)
{
    CellCommodite *new = malloc(sizeof(CellCommodite));
    new->extrA = extraA;
    new->extrB = extraB;
    new->suiv = NULL;
    return new;
}

void inserer_CellCommodite(CellCommodite **liste, CellCommodite *c)
{
    if (*liste == NULL)
    {
        *liste = c;
        return;
    }
    CellCommodite *courant = *liste;
    while (courant->suiv != NULL)
    {
        courant = courant->suiv;
    }
    courant->suiv = c; // Maintenant, courant pointe vers le dernier élément de la liste, nous ajoutons donc c après cela.
    c->suiv = NULL;    // Assurer que c est maintenant le dernier élément de la liste.
}
int noeud_existe(CellNoeud * noeud_R,double x, double y){
    CellNoeud * courant=noeud_R;
    while(courant){
        if(courant->nd->x==x && courant->nd->y==y){
            return 0;
        }
        courant=courant->suiv;
    }
    return 1;
}
void inserer_CellNoeud_coor(CellNoeud **liste, CellNoeud *c)
{
    if (*liste == NULL)
    {
        *liste = c;
        return;
    }

    CellNoeud *courant = *liste;
    while (courant->suiv != NULL)
    {
        courant = courant->suiv;
    }
    courant->suiv = c; // Maintenant, courant pointe vers le dernier élément de la liste, nous ajoutons donc c après cela.
    c->suiv = NULL;    // Assurer que c est maintenant le dernier élément de la liste.
    
}

Noeud *rechercheCreeNoeudListe(Reseau *R, double x, double y)
{
    CellNoeud *n = R->noeuds;
    while (n != NULL)
    {
        if (n->nd->x == x && n->nd->y == y)
        {
            return n->nd;
        }
        n = n->suiv;
    }
    // Si le nœud n'a pas été trouvé, alors nous le créons.
    Noeud *new = creer_Noeud(R->nbNoeuds + 1, x, y);
    CellNoeud *newCell = creer_CellNoeuds(new);

    // Insérer le nouveau nœud à la fin de la liste en utilisant la fonction inserer_CellNoeud
    inserer_CellNoeud_coor(&(R->noeuds), newCell);

    R->nbNoeuds += 1;
    return new;
}

Reseau *reconstitueReseauListe(Chaines *C)
{
    Reseau *r = malloc(sizeof(Reseau));
    r->gamma = C->gamma;
    r->nbNoeuds = 0;
    r->commodites = NULL;
    r->noeuds = NULL; /* ensemble V */
    Noeud *extrA;
    Noeud *precedent;
    Noeud *extrB;
    CellChaine *courant = C->chaines;
    // on parcourt toutes les chaines
    while (courant != NULL)
    {
        CellPoint *p = courant->points;
        precedent = NULL;
        Noeud *n;
        while (p != NULL)
        {
            n = rechercheCreeNoeudListe(r, p->x, p->y);
            if (precedent != NULL)
            {
                if (p->suiv == NULL)
                {
                    extrB = n;
                }
                if(noeud_existe(n->voisins,precedent->x,precedent->y) == 1){
                    inserer_CellNoeud_coor(&(n->voisins), creer_CellNoeuds(precedent));
                }
                if(noeud_existe(precedent->voisins,n->x,n->y) == 1){
                    inserer_CellNoeud_coor(&(precedent->voisins), creer_CellNoeuds(n));
                }
            }
            else
            {
                extrA = n;
            }

            precedent = n;
            p = p->suiv;
        }
        inserer_CellCommodite(&(r->commodites), creer_CellCommodite(extrA, extrB));
        courant = courant->suiv;
    }

    return r;
}
/* Fonction pour libérer un réseau */
void libererreseau(Reseau *R)
{
    if (R == NULL)
        return;

    // Libérer la liste des nœuds
    CellNoeud *courant = R->noeuds;
    CellNoeud *suiv;
    while (courant)
    {
        suiv = courant->suiv;

        // Libérer la liste des voisins du nœud
        CellNoeud *voisins = courant->nd->voisins;
        CellNoeud *voisin_suiv;
        while (voisins)
        {
            voisin_suiv = voisins->suiv;
            free(voisins);
            voisins = voisin_suiv;
        }

        // Libérer le nœud lui-même
        free(courant->nd);

        // Libérer la cellule de nœud
        free(courant);

        courant = suiv;
    }

    // Libérer la liste des commodités
    CellCommodite *c = R->commodites;
    CellCommodite *c_suiv;
    while (c)
    {
        c_suiv = c->suiv;
        free(c);
        c = c_suiv;
    }

    // Enfin, libérer la structure du réseau elle-même
    free(R);
}

int nbLiaisons(Reseau *R)
{
    CellNoeud *courant = R->noeuds;
    CellNoeud *voisin;
    int nbvoisins = 0;
    while (courant != NULL)
    {
        voisin = courant->nd->voisins;
        while (voisin != NULL)
        {
            nbvoisins++;
            // on le rajoute à liste
            voisin = voisin->suiv;
        }
        courant = courant->suiv;
    }
    return nbvoisins / 2;
}

int nbCommodites(Reseau *R)
{
    CellCommodite *courant = R->commodites;
    int nb = 0;
    while (courant != NULL)
    {
        nb++;
        courant = courant->suiv;
    }
    return nb;
}
void ecrireReseau(Reseau *R, FILE *f)
{
    fprintf(f, "%s %d\n", "NbNoeuds:", R->nbNoeuds);
    fprintf(f, "%s %d\n", "NbLiaisons:", nbLiaisons(R));
    fprintf(f, "%s %d\n", "NbCommodites:", nbCommodites(R));
    fprintf(f, "%s %d\n", "Gamma:", R->gamma);
    fprintf(f, "\n");

    // on écrit les noeuds
    CellNoeud *n = R->noeuds;
    while (n != NULL)
    {
        fprintf(f, "v %d %lf %lf\n", n->nd->num, n->nd->x, n->nd->y);
        n = n->suiv;
    }
    fprintf(f, "\n");

    // on écrit les liaisons
    CellNoeud *courant = R->noeuds;
    CellNoeud *voisin;

    while (courant != NULL)
    {
        voisin = courant->nd->voisins;
        while (voisin != NULL)
        {
            if (courant->nd->num < voisin->nd->num)
            {
                fprintf(f, "l %d %d\n", courant->nd->num, voisin->nd->num);
            }
            voisin = voisin->suiv;
        }
        courant = courant->suiv;
    }
    fprintf(f, "\n");

    // on écrit les commodités
    CellCommodite *c = R->commodites;
    while (c != NULL)
    {
        fprintf(f, "k %d %d\n", c->extrA->num, c->extrB->num);
        c = c->suiv;
    }
}

void afficheReseauSVG(Reseau *R, char *nomInstance)
{
    CellNoeud *courN, *courv;
    SVGwriter svg;
    double maxx = 0, maxy = 0, minx = 1e6, miny = 1e6;

    courN = R->noeuds;
    while (courN != NULL)
    {
        if (maxx < courN->nd->x)
            maxx = courN->nd->x;
        if (maxy < courN->nd->y)
            maxy = courN->nd->y;
        if (minx > courN->nd->x)
            minx = courN->nd->x;
        if (miny > courN->nd->y)
            miny = courN->nd->y;
        courN = courN->suiv;
    }
    SVGinit(&svg, nomInstance, 500, 500);
    courN = R->noeuds;
    while (courN != NULL)
    {
        SVGpoint(&svg, 500 * (courN->nd->x - minx) / (maxx - minx), 500 * (courN->nd->y - miny) / (maxy - miny));
        courv = courN->nd->voisins;
        while (courv != NULL)
        {
            if (courv->nd->num < courN->nd->num)
                SVGline(&svg, 500 * (courv->nd->x - minx) / (maxx - minx), 500 * (courv->nd->y - miny) / (maxy - miny), 500 * (courN->nd->x - minx) / (maxx - minx), 500 * (courN->nd->y - miny) / (maxy - miny));
            courv = courv->suiv;
        }
        courN = courN->suiv;
    }
    SVGfinalize(&svg);
}
