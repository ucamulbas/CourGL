#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "feu.h"

float Couleur[BRULURE+REPOUSSE][3];
arbre p[L+1][L+1], np[L+1][L+1];
int nbFeu = 0;

extern void init()
{
  int i, depart=240, inc=240/BRULURE;
  Couleur[0][0]=1;
  Couleur[0][1]=1;
  Couleur[0][2]=1;

  Couleur[1][0]=0;
  Couleur[1][1]=200.0/255.0;
  Couleur[1][2]=0;
  
  Couleur[2][0]=1;
  Couleur[2][1]=0;
  Couleur[2][2]=0;

  for(i=3;i<10;i++)
    {
      Couleur[i][0]=1;
      Couleur[i][1]=0;
      Couleur[i][2]=0;
    }
  
  for(i=10;i<BRULURE;i++)
    {
      Couleur[i][0]=(depart-inc)/255.0;
      Couleur[i][1]=0;
      Couleur[i][2]=0;
  
      
      depart-=inc;
      if(depart<0)
	depart=0;
    }
  //recopie les la dernierre valeur pour avoir un delai
  int invi = 70;
  for(i=BRULURE;i<BRULURE+REPOUSSE-invi;i++)
    {
      Couleur[i][0]=.1;
      Couleur[i][1]=0;
      Couleur[i][2]=0;
    }
  for(i=BRULURE+REPOUSSE-invi;i<BRULURE+REPOUSSE;i++)
     {
      Couleur[i][0]=0;
      Couleur[i][1]=0;
      Couleur[i][2]=0;
    }
}

extern void propager()
{
  int i,j, h,b,d,g;
  for(i=0;i<L;i++)
    for(j=0;j<L;j++)
      {
	if(p[i][j].id==1)
	  {
	    h=i-1;
	    b=i+1;
	    d=j+1;
	    g=j-1;

	    if(p[h][j].id==9 || p[b][j].id==9 || p[i][g].id==9 || p[i][d].id==9)
	      {
		np[i][j].id=2;
		nbFeu++;
	      }
	  }
	else if(p[i][j].id>=2 && p[i][j].id<BRULURE)
	  np[i][j].id=p[i][j].id+1;

	else if(p[i][j].id>=BRULURE &&  p[i][j].id<BRULURE+REPOUSSE)
	  np[i][j].id=p[i][j].id+1;
	
	if(p[i][j].id==BRULURE+REPOUSSE)
	  {
	    np[i][j].id=1;
	    nbFeu--;
	  }
      }
  for(i=0;i<L;i++)
    for(j=0;j<L;j++)
      if(np[i][j].id!=p[i][j].id)
	p[i][j].id=np[i][j].id;
}

extern void feu()
{
  int i,j,x,y,Feu;
  for(Feu=0;Feu<10;Feu++,nbFeu++)
    {
      while(p[x=rand()%L][y=rand()%L].id!=1);
      p[x][y].id=2;
    }
  for(i=0;i<L;i++)
    for(j=0;j<L;j++)
      np[i][j].id=p[i][j].id;
}

extern void foret()
{
  float densite=0.6f, h;
  int i,j;
  for(i=0;i<L;i++)
    for(j=0;j<L;j++)
      {
	h=rand()%1000;
	if(h<(densite*1000))
	  {
	    p[i][j].id=1;
	    p[i][j].x=i*PAS+rand()%20-10;
	    p[i][j].z=j*PAS+rand()%20-10;
	  }
      }
}
