#include "matrix.h"

Matrix *pile;
int H=0;
int Taille = 20;

void initPile()
{
  if(!pile)
    pile=malloc(Taille * sizeof *pile);
}

void PushMatrix(Matrix mat)
{
  if(H==Taille)
    {
      Taille*=2;
      pile=realloc(pile,Taille * sizeof *pile);
    }
  pile[H++]=mat;
}

Matrix PopMatrix()
{
  Matrix mat = pile[--H];
  return mat;
}

Matrix MatrixLoadIdentity()
{
  Matrix res;
  memset(&res,0,sizeof res);
  res.r[0].x=1.0f;
  res.r[1].y=1.0f;
  res.r[2].z=1.0f;
  res.r[3].w=1.0f;
  return res;
}

Matrix MatrixPerspective(float fovy, float aspect, float zNear, float zFar)
{
  fovy*=(M_PI / 180);
  float f = (cos(fovy/2)/sin(fovy/2));
  Matrix res;
  memset(&res,0,sizeof res);
  res.r[0].x=f/aspect;
  res.r[1].y=f;
  res.r[2].z=(zFar+zNear)/(zNear-zFar);
  res.r[2].w=(2*zFar*zNear)/(zNear-zFar);
  res.r[3].z=-1.0f;
  return res;
}

Matrix MatrixTranslatef(float x, float y, float z)
{
  Matrix res=MatrixLoadIdentity();
  res.r[0].w=x;
  res.r[1].w=y;
  res.r[2].w=z;
  return res;
}

Matrix MatrixRotatef(float fovy, float x, float y, float z)
{
  fovy*=(M_PI / 180);
  Matrix res = MatrixLoadIdentity();
  Vector axe = {x,y,z,1};
  float c=cos(fovy), s=sin(fovy);
  axe = NormalizeV3(axe);

  res.r[0].x=(CARRE(axe.x)*(1-c))+c;
  res.r[0].y=axe.x*axe.y*(1-c)-(axe.z*s);
  res.r[0].z=(axe.x*axe.z*(1-c))+(axe.y*s);

  res.r[1].x=(axe.y*axe.x*(1-c))+(axe.z*s);
  res.r[1].y=(CARRE(axe.y)*(1-c))+c;
  res.r[1].z=(axe.y*axe.z*(1-c))-(axe.x*s);

  res.r[2].x=(axe.x*axe.z*(1-c))-(axe.y*s);
  res.r[2].y=(axe.y*axe.z*(1-c))+(axe.x*s);
  res.r[2].z=(CARRE(axe.z)*(1-c))+c;
  
  return res;
}

Matrix MatrixScalef(float x, float y, float z)
{
  Matrix res = MatrixLoadIdentity();
  res.r[0].x=x;
  res.r[1].y=y;
  res.r[2].z=z;
  return res;
}

Vector NormalizeV3(Vector vec)
{
  Vector res;
  float lenght = sqrt(CARRE(vec.x) + CARRE(vec.y) + CARRE(vec.z));
  res.x=vec.x/lenght;
  res.y=vec.y/lenght;
  res.z=vec.z/lenght;
  return res;
}

Vector MultVector(Vector v1, Vector v2)
{
  Vector res;
  res.x=v1.x*v2.x;
  res.y=v1.y*v2.y;
  res.z=v1.z*v2.z;
  return res;
}

Vector CrossProductV3(Vector v1, Vector v2)
{
  Vector res;
  res.x=(v1.y*v2.z)-(v1.z*v2.y);
  res.y=(v1.z*v2.x)-(v1.x*v2.z);
  res.z=(v1.x*v2.y)-(v1.y*v2.x);
  return res;
}

Matrix MatrixMultiply(Matrix mat1, Matrix mat2)
{
  Matrix res;

  res.r[0].x = (mat1.r[0].x * mat2.r[0].x) + (mat1.r[0].y * mat2.r[1].x) + (mat1.r[0].z * mat2.r[2].x) + (mat1.r[0].w * mat2.r[3].x);
  res.r[0].y = (mat1.r[0].x * mat2.r[0].y) + (mat1.r[0].y * mat2.r[1].y) + (mat1.r[0].z * mat2.r[2].y) + (mat1.r[0].w * mat2.r[3].y);
  res.r[0].z = (mat1.r[0].x * mat2.r[0].z) + (mat1.r[0].y * mat2.r[1].z) + (mat1.r[0].z * mat2.r[2].z) + (mat1.r[0].w * mat2.r[3].z);
  res.r[0].w = (mat1.r[0].x * mat2.r[0].w) + (mat1.r[0].y * mat2.r[1].w) + (mat1.r[0].z * mat2.r[2].w) + (mat1.r[0].w * mat2.r[3].w);

  res.r[1].x = (mat1.r[1].x * mat2.r[0].x) + (mat1.r[1].y * mat2.r[1].x) + (mat1.r[1].z * mat2.r[2].x) + (mat1.r[1].w * mat2.r[3].x);
  res.r[1].y = (mat1.r[1].x * mat2.r[0].y) + (mat1.r[1].y * mat2.r[1].y) + (mat1.r[1].z * mat2.r[2].y) + (mat1.r[1].w * mat2.r[3].y);
  res.r[1].z = (mat1.r[1].x * mat2.r[0].z) + (mat1.r[1].y * mat2.r[1].z) + (mat1.r[1].z * mat2.r[2].z) + (mat1.r[1].w * mat2.r[3].z);
  res.r[1].w = (mat1.r[1].x * mat2.r[0].w) + (mat1.r[1].y * mat2.r[1].w) + (mat1.r[1].z * mat2.r[2].w) + (mat1.r[1].w * mat2.r[3].w);

  res.r[2].x = (mat1.r[2].x * mat2.r[0].x) + (mat1.r[2].y * mat2.r[1].x) + (mat1.r[2].z * mat2.r[2].x) + (mat1.r[2].w * mat2.r[3].x);
  res.r[2].y = (mat1.r[2].x * mat2.r[0].y) + (mat1.r[2].y * mat2.r[1].y) + (mat1.r[2].z * mat2.r[2].y) + (mat1.r[2].w * mat2.r[3].y);
  res.r[2].z = (mat1.r[2].x * mat2.r[0].z) + (mat1.r[2].y * mat2.r[1].z) + (mat1.r[2].z * mat2.r[2].z) + (mat1.r[2].w * mat2.r[3].z);
  res.r[2].w = (mat1.r[2].x * mat2.r[0].w) + (mat1.r[2].y * mat2.r[1].w) + (mat1.r[2].z * mat2.r[2].w) + (mat1.r[2].w * mat2.r[3].w);

  res.r[3].x = (mat1.r[3].x * mat2.r[0].x) + (mat1.r[3].y * mat2.r[1].x) + (mat1.r[3].z * mat2.r[2].x) + (mat1.r[3].w * mat2.r[3].x);
  res.r[3].y = (mat1.r[3].x * mat2.r[0].y) + (mat1.r[3].y * mat2.r[1].y) + (mat1.r[3].z * mat2.r[2].y) + (mat1.r[3].w * mat2.r[3].y);
  res.r[3].z = (mat1.r[3].x * mat2.r[0].z) + (mat1.r[3].y * mat2.r[1].z) + (mat1.r[3].z * mat2.r[2].z) + (mat1.r[3].w * mat2.r[3].z);
  res.r[3].w = (mat1.r[3].x * mat2.r[0].w) + (mat1.r[3].y * mat2.r[1].w) + (mat1.r[3].z * mat2.r[2].w) + (mat1.r[3].w * mat2.r[3].w);

  return res;
}

Matrix MatrixLookAt(float ex, float ey, float ez, float cx, float cy, float cz, float upx, float upy, float upz)
{
  Matrix res=MatrixLoadIdentity();
  Vector forward={cx-ex,cy-ey,cz-ez,1}, up={upx,upy,upz,1}, s,u;
  forward=NormalizeV3(forward);
  up=NormalizeV3(up);
  s=CrossProductV3(forward,up);
  s=NormalizeV3(s);
  u=CrossProductV3(s,forward);
  
  res.r[0].x=s.x;
  res.r[0].y=s.y;
  res.r[0].z=s.z;
  
  res.r[1].x=u.x;
  res.r[1].y=u.y;
  res.r[1].z=u.z;
  
  res.r[2].x=-forward.x;
  res.r[2].y=-forward.y;
  res.r[2].z=-forward.z;
  
  res=MatrixMultiply(res,MatrixTranslatef(-ex,-ey,-ez));
  return res;

}


void MatrixPrint(Matrix mat)
{
     int i;
     for(i=0;i<4;i++)
	  printf("%f %f %f %f\n", mat.r[i].x, mat.r[i].y,mat.r[i].z,mat.r[i].w);
     putchar('\n');
}

Matrix MatrixOrthonormal(float left, float right, float bottom, float top, float near, float far)
{
     Matrix res=MatrixLoadIdentity();
     float tx = -((right+left)/(right-left));
     float ty = -((top+bottom)/(top-bottom));
     float tz = -((far+near)/(far-near));
     res.r[0].x = 2/(right-left);
     res.r[0].z = tx;
     res.r[1].y = 2/(top-bottom);
     res.r[1].w = ty;
     res.r[2].z = -2/(far-near);
     res.r[2].w = tz;
     return res;
}


/* float ProduitScalaire (Vector a, Vector b) */
/* { */
/*   float res; */
  
/* } */
