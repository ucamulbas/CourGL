#ifndef FEU
#define FEU

#define L 125
#define PAS 12
#define BRULURE 50
#define REPOUSSE 150
#define TAILLE L*PAS

struct arbre{
  int id;
  int x,z;
};
typedef struct arbre arbre;

extern float Couleur[BRULURE+REPOUSSE][3];
extern arbre p[L+1][L+1], np[L+1][L+1];
extern int nbFeu;

extern void init();
extern void propager();
extern void feu();
extern void foret();



#endif












