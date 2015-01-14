#ifndef CAM
#define CAM
#include "matrix.h"
#include <GL/gl.h>

void mouseMove(int a, int b);
void keyBoardMove(int a, int b, int c, int d);
Vector getTarget(void);
Vector getPosition(void);

#endif
