#ifndef MATRIX_HEAD
#define MATRIX_HEAD
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define CARRE(X) ((X)*(X))

typedef struct Vector
{
  float x,y,z,w;
}Vector;

typedef struct Matrix
{
  Vector r[4];
}Matrix;

extern Matrix *pile;
extern int H;
extern int Taille;

extern void initPile();
extern void PushMatrix(Matrix mat);
extern Matrix PopMatrix();

extern Matrix MatrixLoadIdentity();
extern Matrix MatrixMultiply(Matrix m1, Matrix m2);
extern Matrix MatrixPerspective(float fovy, float aspect, float zNear, float zFar);
extern Matrix MatrixTranslatef(float x, float y, float z);
extern Matrix MatrixRotatef(float fovy, float x, float y, float z);
extern Matrix MatrixScalef(float x, float y, float z);
extern Matrix MatrixLookAt(float ex, float ey, float ez, float cx, float cy, float cz, float upx, float upy, float upz);
extern Vector NormalizeV3(Vector vec);
extern Vector MultVector(Vector v1, Vector v2);
extern Vector CrossProductV3(Vector v1, Vector v2);
extern void MatrixPrint(Matrix mat);
extern Matrix MatrixOrthonormal(float left, float right, float bottom, float top, float near, float far);


#endif
