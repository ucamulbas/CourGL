#include "cam.h"

static Vector position = {62.5*4,70.0,-40.0};
static Vector target;
static Vector forward;
static Vector left;



void mouseMove(int xrel, int yrel)
{
  static float atheta =0.0, aphi=0.0;
  GLfloat tmp;
  atheta-=((GLfloat)xrel * 0.2) * M_PI/180;
  aphi-=((GLfloat)yrel * 0.2) * M_PI/180;

  if(aphi >= M_PI/2) aphi = 89.0f * M_PI / 180;
  else if(aphi <= -M_PI/2) aphi = -89.0f * M_PI / 180;
  
  tmp = cos(aphi);
  forward.x = tmp * sin(atheta);
  forward.y = sin(aphi);
  forward.z = tmp * cos(atheta);

  left.x = forward.z;
  left.y = 0;
  left.z = -forward.x;
  left=NormalizeV3(left);


  target.x = position.x + forward.x;
  target.y = position.y + forward.y;
  target.z = position.z + forward.z;
  
  
}

void keyBoardMove(int f, int b, int l, int r)
{
  GLfloat speed = 0.5;  
  if(f)
    {
      position.x -= left.z * speed;
      position.z += left.x * speed;
    }
  if(b)
    {
      position.x += left.z * speed;
      position.z -= left.x * speed;
    }
  if(l)
    {
      position.x += left.x * speed;
      position.z += left.z * speed;
    }
  if(r)
    {
      position.x -= left.x * speed;
      position.z -= left.z * speed;
    }
  
  target.x = position.x + forward.x;
  target.y = position.y + forward.y;
  target.z = position.z + forward.z;
}

Vector getPosition(void)
{
  return position;
}

Vector getTarget(void)
{
  return target;
}


