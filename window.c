#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include <gl4du.h>
#include "matrix.h"
#include "cam.h"
#include "feu.h"


//il faut trouver un moyen de pas mettre les arbre en ligne


static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext);
static void quit(void);
static void initGL(SDL_Window * win);
static void initData(void);
static void resizeGL(SDL_Window * win);
static void loop(SDL_Window * win);
static void manageEvents(SDL_Window * win);
static void draw();

static SDL_Window * _win = NULL;
static int _windowWidth = 800, _windowHeight = 600;
static SDL_GLContext _oglContext = NULL;
static GLuint _vao[8], _buffer[8], _pId[4], _tId[9];
static GLint f=0, b=0, l=0, r=0, posSolX = 0, posSolZ = 0;


struct particule{
  int vie;
  float taille;
  float x,y,z;
};

struct fume{
  struct particule parti[10];
};

typedef struct particule particule;
typedef struct fume fume;

static fume smoke[L+2][L+2];

int main(int argc, char ** argv) {
  srand(time(NULL));
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "Erreur lors de l'initialisation de SDL :  %s", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);
  if((_win = initWindow(_windowWidth, _windowHeight, &_oglContext))) {
    initGL(_win);
    _pId[0] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    _pId[1] = gl4duCreateProgram("<vs>shaders/feuille.vs", "<fs>shaders/feuille.fs", NULL);
    _pId[2] = gl4duCreateProgram("<vs>shaders/tron.vs", "<fs>shaders/tron.fs", NULL);
    _pId[3] = gl4duCreateProgram("<vs>shaders/skybox.vs", "<fs>shaders/skybox.fs", NULL);

    init();
    foret();
    feu();
    
    initData();
    SDL_SetRelativeMouseMode(SDL_TRUE);
    loop(_win);
  } else 
    fprintf(stderr, "Erreur lors de la creation de la fenetre\n");
  return 0;
}

static void quit(void) {
  glDeleteTextures(9,_tId);
  glDeleteVertexArrays(8, _vao);
  glDeleteBuffers(8, _buffer);
  if(_oglContext)
    SDL_GL_DeleteContext(_oglContext);
  if(_win)
    SDL_DestroyWindow(_win);
  gl4duClean(GL4DU_ALL);
}

static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext) {
  SDL_Window * win = NULL;
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  if( (win = SDL_CreateWindow("Fenetre GL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			      w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | 
			      SDL_WINDOW_SHOWN)) == NULL )
    return NULL;
  if( (*poglContext = SDL_GL_CreateContext(win)) == NULL ) {
    SDL_DestroyWindow(win);
    return NULL;
  }
  fprintf(stderr, "Version d'OpenGL : %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Version de shaders supportes : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));  
  atexit(quit);
  return win;
}

static void initGL(SDL_Window * win) {
  glClearColor(0.0f, 0.4f, 0.9f, 0.0f);
  glViewport(0, 0, _windowWidth, _windowHeight);
  initPile();
  resizeGL(win);
}


static void initData(void) {

  int i,j,k;

  for(i=0;i<L;i++)
    for(j=0;j<L;j++)
      for(k=0;k<10;k++)
	{
	  smoke[i][j].parti[k].vie=100;
	  smoke[i][j].parti[k].taille=0.01;
	  smoke[i][j].parti[k].x=p[i][j].x;
	  smoke[i][j].parti[k].y=9;
	  smoke[i][j].parti[k].z=p[i][j].z;
	}
  for(k=0;k<10;k++)
    {
      smoke[L+1][L+1].parti[k].vie=100;
      smoke[L+1][L+1].parti[k].taille=0.01;
      smoke[L+1][L+1].parti[k].x=getPosition().x;
      smoke[L+1][L+1].parti[k].y=getPosition().y;
      smoke[L+1][L+1].parti[k].z=getPosition().z+4;
    }
  
  GLfloat data[] = {
    -700.0,0.0,700.0,
    700.0,0.0,700.0,
    -700.0,0.0,-700.0,
    700.0,0.0,-700.0,

    0.0,1.0,0.0,
    0.0,1.0,0.0,
    0.0,1.0,0.0,
    0.0,1.0,0.0,

    0.0,0.0,
    20.0,0.0,
    0.0,20.0,
    20.0,20.0
  };

  GLfloat data2[] = {
    -8.0,8.0,0.0,
    8.0,8.0,0.0,
    -8.0,-2.0,0.0,
    8.0,-2.0,0.0,

    0.0,0.0,
    1.0,0.0,
    0.0,1.0,
    1.0,1.0
  };

  GLfloat data3[] = {
    -8.0,4.0,0.0,
    8.0,4.0,0.0,
    -8.0,-2.0,0.0,
    8.0,-2.0,0.0,

    0.0,0.0,
    1.0,0.0,
    0.0,1.0,
    1.0,1.0
  };

  GLfloat data4[] = {
    -1.0f, -1.0f, 1.0,
    1.0f, -1.0f, 1.0,
    -1.0f,  1.0f, 1.0,
    1.0, 1.0, 1.0,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f};
  GLfloat data5[] = {
    //face supp
    -1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
      -1.0, 1.0, -1.0,
      1.0, 1.0, -1.0,   
      0.0f, 1.0f, 1.0f,1.0f,
      0.0f, 0.0f, 1.0f, 0.0f};
  GLfloat data6[] = {
    //face arriere
    -1.0, 1.0, -1.0,
      1.0, 1.0, -1.0,
      -1.0, -1.0, -1.0,
      1.0, -1.0, -1.0,
      0.0f, 1.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 0.0f};
  GLfloat data7[] = {
    /* //face gauche */
    -1.0f, -1.0f, 1.0,
      -1.0f, -1.0f, -1.0,
      -1.0f,  1.0f, 1.0,
      -1.0, 1.0, -1.0,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f, 1.0f};
  GLfloat data8[] = {
    /* //face droite */
    1.0f, -1.0f, 1.0,
      1.0f, -1.0f, -1.0,
      1.0, 1.0, 1.0,
      1.0, 1.0, -1.0,
      1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, 1.0f, 0.0f, 1.0f};


  glGenVertexArrays(8, _vao);
  glGenBuffers(8, _buffer);

  glBindVertexArray(_vao[0]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(24 * sizeof *data));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(_vao[1]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data2, data2, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data2));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  //charger le buffer des feuilles et tout le bordel
  glBindVertexArray(_vao[2]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data3, data3, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data3));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(_vao[3]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data4, data4, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data4));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(_vao[4]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data5, data5, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data5));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(_vao[5]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data6, data6, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data6));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(_vao[6]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[6]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data7, data7, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data7));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindVertexArray(_vao[7]);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer[7]);
  glBufferData(GL_ARRAY_BUFFER, sizeof data8, data8, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(12 * sizeof *data8));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);




  SDL_Surface *texture;
  glGenTextures(9,_tId);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/herbe.jpg");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->w,texture->h,0,GL_RGB,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur herbe\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/tron.png");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texture->w,texture->h,0,GL_BGRA,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur tron\n");
      return;
    }

  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, _tId[2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/feuilleArbre.png");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texture->w,texture->h,0,GL_BGRA,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur feuilleArbre\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, _tId[3]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/arriere.jpg");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->w,texture->h,0,GL_RGB,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur arriere\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, _tId[4]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/dessu.jpg");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->w,texture->h,0,GL_RGB,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur dessu\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, _tId[5]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/devant.jpg");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->w,texture->h,0,GL_RGB,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur devant\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

 glBindTexture(GL_TEXTURE_2D, _tId[6]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/gauche.jpg");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->w,texture->h,0,GL_RGB,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur gauche\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

 glBindTexture(GL_TEXTURE_2D, _tId[7]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/droite.jpg");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->w,texture->h,0,GL_RGB,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur droite\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindTexture(GL_TEXTURE_2D, _tId[8]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  texture = IMG_Load("image/smoke.png");
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,texture->w,texture->h,0,GL_BGRA,GL_UNSIGNED_BYTE,texture->pixels);
  if(texture == 0)
    { 
      fprintf(stderr, "erreur smoke\n");
      return;
    }
  glBindTexture(GL_TEXTURE_2D, 0);

  free(texture->pixels);
  free(texture);
}

static void resizeGL(SDL_Window * win) {
  SDL_GetWindowSize(win, &_windowWidth, &_windowHeight);
  glViewport(0, 0, _windowWidth, _windowHeight);
}


static void loop(SDL_Window * win) {
  for(;;) {
    manageEvents(win);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyBoardMove(f,b,l,r);

    if(nbFeu)
      {
	//SDL_Delay(70);
	propager();
      }
    
    draw();
    SDL_GL_SwapWindow(win);
    gl4duUpdateShaders();
  }
}
static void manageEvents(SDL_Window * win) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) 
    switch (event.type) {
    case SDL_MOUSEMOTION:
      mouseMove(event.motion.xrel, event.motion.yrel);
      break;
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
      case SDLK_ESCAPE:
	exit(0);
      case 'q':
	l=1;
	break;
      case 'd':
	r=1;
	break;
      case 'z':
	f=1;
	break;
      case 's':
	b=1;
	break;
      case 'f':
	feu();
	break;
      }
      break;
    case SDL_KEYUP:
      switch(event.key.keysym.sym) {
      case 'q':
	l=0;
	break;
      case 'd':
	r=0;
	break;
      case 'z':
	f=0;
	break;
      case 's':
	b=0;
	break;
      }
      break;
    case SDL_WINDOWEVENT:
      if(event.window.windowID == SDL_GetWindowID(win)) {
	switch (event.window.event)  {
	case SDL_WINDOWEVENT_RESIZED:
	  resizeGL(win);
	  break;
	case SDL_WINDOWEVENT_CLOSE:
	  event.type = SDL_QUIT;
	  SDL_PushEvent(&event);
	  break;
	}
      }
      break;
    case SDL_QUIT:
      exit(0);
    }
}

static void drawForet(int i, int j)
{
  Matrix MVP;
  Matrix projectionMatrix = MatrixPerspective(90,(GLfloat)800/600, 0.1f, 500.0f);
  Matrix viewMatrix = MatrixLookAt(getPosition().x,getPosition().y,getPosition().z, getTarget().x,getTarget().y,getTarget().z, 0.0,1.0,0.0);
  Matrix modelMatrix = MatrixLoadIdentity();
  GLfloat camPos[3]={getPosition().x,getPosition().y,getPosition().z};
  glEnable(GL_DEPTH_TEST);
  glUseProgram(_pId[2]);
  glBindVertexArray(_vao[1]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  //dessin du tron
  MVP = MatrixMultiply(projectionMatrix, viewMatrix);
  PushMatrix(MVP);
  PushMatrix(modelMatrix);
  modelMatrix=MatrixMultiply(modelMatrix, MatrixTranslatef(p[i][j].x,2,p[i][j].z));
  MVP=MatrixMultiply(MVP,modelMatrix);
  glUniform1i(glGetUniformLocation(_pId[2], "myTexture"), 0);
  glUniformMatrix4fv(glGetUniformLocation(_pId[2], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
  glUniformMatrix4fv(glGetUniformLocation(_pId[2], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
  glUniform3fv(glGetUniformLocation(_pId[2], "camPos"), 1, camPos);
  glUniform3fv(glGetUniformLocation(_pId[2], "couleur"), 1, Couleur[p[i][j].id]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  MVP = MatrixMultiply(projectionMatrix, viewMatrix);
  modelMatrix=MatrixMultiply(MatrixLoadIdentity(), MatrixTranslatef(p[i][j].x,2,p[i][j].z));
  modelMatrix=MatrixMultiply(modelMatrix, MatrixRotatef(90,0,1,0));
  MVP=MatrixMultiply(MVP,modelMatrix);
  glUniform1i(glGetUniformLocation(_pId[2], "myTexture"), 0);
  glUniformMatrix4fv(glGetUniformLocation(_pId[2], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
  glUniformMatrix4fv(glGetUniformLocation(_pId[2], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
  glUniform3fv(glGetUniformLocation(_pId[2], "camPos"), 1, camPos);
  glUniform3fv(glGetUniformLocation(_pId[2], "couleur"), 1, Couleur[p[i][j].id]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  glUseProgram(0);
  //fin dessin du tron

  //dessin des feuilles
  glUseProgram(_pId[1]);
  glBindVertexArray(_vao[2]);
  modelMatrix=PopMatrix();
  MVP=PopMatrix();
  modelMatrix=MatrixMultiply(modelMatrix, MatrixTranslatef(p[i][j].x,6,p[i][j].z));
  PushMatrix(modelMatrix);
  MVP = MatrixMultiply(MVP,modelMatrix);
  glBindTexture(GL_TEXTURE_2D, _tId[2]);
  glUniform1i(glGetUniformLocation(_pId[1], "myTexture"), 0);
  glUniformMatrix4fv(glGetUniformLocation(_pId[1], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
  glUniformMatrix4fv(glGetUniformLocation(_pId[1], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
  glUniform3fv(glGetUniformLocation(_pId[1], "camPos"), 1, camPos);
  glUniform3fv(glGetUniformLocation(_pId[1], "couleur"), 1, Couleur[p[i][j].id]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  modelMatrix = PopMatrix();
  MVP = MatrixMultiply(projectionMatrix, viewMatrix);
  modelMatrix=MatrixMultiply(modelMatrix, MatrixRotatef(90,0,1,0));
  MVP = MatrixMultiply(MVP,modelMatrix);
  glUniform1i(glGetUniformLocation(_pId[1], "myTexture"), 0);
  glUniformMatrix4fv(glGetUniformLocation(_pId[1], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
  glUniformMatrix4fv(glGetUniformLocation(_pId[1], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
  glUniform3fv(glGetUniformLocation(_pId[1], "camPos"), 1, camPos);
  glUniform3fv(glGetUniformLocation(_pId[1], "couleur"), 1, Couleur[p[i][j].id]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
  glUseProgram(0);
}


static void drawSmoke(int i, int j)
{
  Matrix MVP;
  Matrix projectionMatrix = MatrixPerspective(90,(GLfloat)800/600, 0.1f, 1000.0f);
  Matrix viewMatrix = MatrixLookAt(getPosition().x,getPosition().y,getPosition().z, getTarget().x,getTarget().y,getTarget().z, 0.0,1.0,0.0);
  Matrix modelMatrix = MatrixLoadIdentity();
  glUseProgram(_pId[3]);
  int k;

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
  for(k=0;k<10;k++)
    {
      if(k!=0  && smoke[i][j].parti[k-1].vie<=80)
      	{
      	  glBindVertexArray(_vao[5]);
      	  modelMatrix = MatrixLoadIdentity();
      	  glBindTexture(GL_TEXTURE_2D, _tId[8]);
      	  glUniform1i(glGetUniformLocation(_pId[3], "myTexture"), 0);
      	  modelMatrix = MatrixMultiply(modelMatrix,MatrixTranslatef(smoke[i][j].parti[k].x,smoke[i][j].parti[k].y,smoke[i][j].parti[k].z));
	  modelMatrix = MatrixMultiply(modelMatrix,MatrixScalef(smoke[i][j].parti[k].taille,smoke[i][j].parti[k].taille,smoke[i][j].parti[k].taille));
      	  MVP=MatrixMultiply(projectionMatrix, viewMatrix);
      	  MVP=MatrixMultiply(MVP,modelMatrix);
      	  glUniform1f(glGetUniformLocation(_pId[3], "alpha"), (float)smoke[i][j].parti[k].vie/100);
      	  glUniformMatrix4fv(glGetUniformLocation(_pId[3], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
      	  glUniformMatrix4fv(glGetUniformLocation(_pId[3], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
      	  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      	  glBindVertexArray(0);
      	  smoke[i][j].parti[k].vie--;
      	  smoke[i][j].parti[k].taille+=0.2;
	  smoke[i][j].parti[k].y+=0.2;
      	  if(smoke[i][j].parti[k].vie<=0)
      	    {
      	      smoke[i][j].parti[k].vie=100;
      	      smoke[i][j].parti[k].taille=0.1;
	      smoke[i][j].parti[k].y=7;
      	    }
	}
      else if(!k)
      	{
	  glBindVertexArray(_vao[5]);
	  modelMatrix = MatrixLoadIdentity();
	  glBindTexture(GL_TEXTURE_2D, _tId[8]);
	  glUniform1i(glGetUniformLocation(_pId[3], "myTexture"), 0);
	  modelMatrix = MatrixMultiply(modelMatrix,MatrixTranslatef(smoke[i][j].parti[k].x,smoke[i][j].parti[k].y,smoke[i][j].parti[k].z));
	  modelMatrix = MatrixMultiply(modelMatrix,MatrixScalef(smoke[i][j].parti[k].taille,smoke[i][j].parti[k].taille,smoke[i][j].parti[k].taille));
	  MVP=MatrixMultiply(projectionMatrix, viewMatrix);
	  MVP=MatrixMultiply(MVP,modelMatrix);
	  glUniform1f(glGetUniformLocation(_pId[3], "alpha"), smoke[i][j].parti[k].vie/100);
	  glUniformMatrix4fv(glGetUniformLocation(_pId[3], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
	  glUniformMatrix4fv(glGetUniformLocation(_pId[3], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
	  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	  glBindVertexArray(0);
	  smoke[i][j].parti[k].vie--;
	  smoke[i][j].parti[k].taille+=0.2;
	  smoke[i][j].parti[k].y+=0.2;
	  if(smoke[i][j].parti[k].vie<=0)
	    {
	      smoke[i][j].parti[k].vie=100;
	      smoke[i][j].parti[k].taille=0.1;
	      smoke[i][j].parti[k].y=7;
	    }
	}
    }
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(2);
  glUseProgram(0);
}


static void draw() {

  Matrix MVP;
  Matrix projectionMatrix = MatrixPerspective(90,(GLfloat)800/600, 0.1f, 1000.0f);
  Matrix viewMatrix = MatrixLookAt(getPosition().x,getPosition().y,getPosition().z, getTarget().x,getTarget().y,getTarget().z, 0.0,1.0,0.0);
  Matrix modelMatrix = MatrixLoadIdentity();
  GLfloat lumpos[3]={getPosition().x,getPosition().y+10,getPosition().z};
  GLfloat campos[3]={getPosition().x,getPosition().y,getPosition().z};
  static float oldX = 0, oldZ = 0;
  int i,j;
  GLfloat dx,dz,distance;


  glUseProgram(_pId[3]);
  glDisable(GL_DEPTH_TEST);
  for(i=3;i<8;i++)
    {
      glBindVertexArray(_vao[i]);
      modelMatrix = MatrixLoadIdentity();
      glBindTexture(GL_TEXTURE_2D, _tId[i]);
      glUniform1i(glGetUniformLocation(_pId[3], "myTexture"), 0);
      modelMatrix = MatrixMultiply(MatrixTranslatef(getPosition().x,getPosition().y,getPosition().z),MatrixScalef(5,5,5));
      MVP=MatrixMultiply(projectionMatrix, viewMatrix);
      MVP=MatrixMultiply(MVP,modelMatrix);
      glUniform1f(glGetUniformLocation(_pId[3], "alpha"), 255.0);
      glUniformMatrix4fv(glGetUniformLocation(_pId[3], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
      glUniformMatrix4fv(glGetUniformLocation(_pId[3], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      glBindVertexArray(0);
    }
  glEnable(GL_DEPTH);
  


  if(posSolX==0 && posSolZ==0)
    {
      posSolX=getPosition().x;
      posSolZ=getPosition().z;
    }
  if(((int)campos[2]%20)==0 && campos[2]!=oldZ)
    {
      if(campos[2]>oldZ)
  	{
  	  posSolZ+=campos[2]-oldZ;
  	  oldZ=campos[2];
  	}
      else if(campos[2]<oldZ)
  	{
  	  posSolZ-=oldZ-campos[2];
  	  oldZ=campos[2];
  	}
    }
  if(((int)campos[0]%20)==0 && campos[0]!=oldX)
    {
      if(campos[0]>oldX)
  	{
  	  posSolX+=campos[0]-oldX;
  	  oldX=campos[0];
  	}
      else if(campos[0]<oldX)
  	{
  	  posSolX-=oldX-campos[0];
  	  oldX=campos[0];
  	}
    }
  modelMatrix = MatrixTranslatef(posSolX, 0, posSolZ);
  MVP=MatrixMultiply(projectionMatrix, viewMatrix);
  MVP=MatrixMultiply(MVP,modelMatrix);
  glActiveTexture(GL_TEXTURE0);
  glUseProgram(_pId[0]);
  glUniform1i(glGetUniformLocation(_pId[0], "myTexture"), 0);
  glUniform3fv(glGetUniformLocation(_pId[0],"lumPos"),1,lumpos);
  glUniform3fv(glGetUniformLocation(_pId[0],"camPos"),1,campos);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  glBindVertexArray(_vao[0]);
  glUniformMatrix4fv(glGetUniformLocation(_pId[0], "modelViewProjectionMatrix"), 1, GL_FALSE,(GLfloat*) &MVP);
  glUniformMatrix4fv(glGetUniformLocation(_pId[0], "modelMatrix"), 1, GL_FALSE,(GLfloat*) &modelMatrix);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D,0);
  glUseProgram(0);

  for(i=0;i<L;i++)
    for(j=0;j<L;j++)
      if(p[i][j].id!=0)
	{
	  dx=((p[i][j].x)-campos[0])*((p[i][j].x)-campos[0]);
	  dz=((p[i][j].z)-campos[2])*((p[i][j].z)-campos[2]);
	  distance=sqrt(dx+dz);
	  if(distance<=400)
	    {
	      drawForet(i,j);
	      if(p[i][j].id>=2 && p[i][j].id<BRULURE+(REPOUSSE/2))
		drawSmoke(i,j);
	    }
	}
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

