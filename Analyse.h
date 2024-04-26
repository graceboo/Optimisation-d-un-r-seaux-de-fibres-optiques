#ifndef __ANALYSE_H__
#define __ANALYSE_H__
void mesurerTempsExecution(FILE *flec,FILE* fecr);
Chaines* generationAleatoire(int nbChaines, int nbPointsChaine, double xmax, double ymax);
double randCoord(double max);
void mesurerTempsExecutionArbreQuat_Hachage(FILE *flec,FILE *fecr,int nbpointchaine,int nbchaines);
void mesurerTempsExecution_liste(FILE *flec,FILE *fecr,int nbpointchaine,int nbchaines);
#endif