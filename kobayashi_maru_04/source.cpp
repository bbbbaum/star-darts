#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <vector>
#include <time.h>
#include "imageloader.h"
#include "Vec3f.h"

/////KONSTANTE ZA EKSPLOZIJU I INDIKATORI STANJA IGRE
#define NUM_PARTICLES    1000          
#define NUM_DEBRIS       70            
int sudar = 0;
int score = 0;
int over = 0;
/////

using namespace std;

//// ÈESTICE ZA EKSPLOZIJU
struct particleData
{
  float   position[3];
  float   speed[3];
  float   color[3];
};
typedef struct particleData    particleData;

struct debrisData
{
  float   position[3];
  float   speed[3];
  float   orientation[3];      
  float   orientationSpeed[3];
  float   color[3];
  float   scale[3];
};
typedef struct debrisData    debrisData;

particleData     particles[NUM_PARTICLES];
debrisData       debris[NUM_DEBRIS];   
int              fuel = 0;					// 
int dan = 0;								// oznaka mape 
int noc = 1;								// oznaka mape


//brzina eksplozije
void newSpeed (float dest[3])
{
  float    x;
  float    y;
  float    z;
  float    len;

  x = (2.0 * ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
  y = (2.0 * ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
  z = (2.0 * ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;

  if (1)
    {
      len = sqrt (x * x + y * y + z * z);

      if (len)
	{
	  x = x / len;
	  y = y / len;
	  z = z / len;
	}
    }

  dest[0] = x;
  dest[1] = y;
  dest[2] = z;
}

//eksplozija
void newExplosion (void)
{
  int    i;

  for (i = 0; i < NUM_PARTICLES; i++)
    {
      particles[i].position[0] = 0.0;
      particles[i].position[1] = 0.0;
      particles[i].position[2] = 0.0;

      particles[i].color[0] = 1.0;
      particles[i].color[1] = 1.0;
      particles[i].color[2] = 0.5;

      newSpeed (particles[i].speed);
    }

  for (i = 0; i < NUM_DEBRIS; i++)
    {
      debris[i].position[0] = 0.0;
      debris[i].position[1] = 0.0;
      debris[i].position[2] = 0.0;

      debris[i].orientation[0] = 0.0;
      debris[i].orientation[1] = 0.0;
      debris[i].orientation[2] = 0.0;

      debris[i].color[0] = 0.7;
      debris[i].color[1] = 0.7;
      debris[i].color[2] = 0.7;

      debris[i].scale[0] = (2.0 * 
			    ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
      debris[i].scale[1] = (2.0 * 
			    ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
      debris[i].scale[2] = (2.0 * 
			    ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;

      newSpeed (debris[i].speed);
      newSpeed (debris[i].orientationSpeed);
    }
  
  fuel = 100;
}

int lives = 5;										// zivoti
int font=(int)GLUT_BITMAP_HELVETICA_18;				// font
int font2=(int)GLUT_BITMAP_HELVETICA_12;			// font
/////

//sfera
typedef struct Sphere{
	Vec3f* position;
	float precnik;
	int vrednost;
	float crvena;
	float plava;
	float zelena;
} Sphere;

Sphere strelica;
Sphere loptica1;
Sphere loptica2;
Sphere loptica3;

//vrijeme za koje se loptice krecu do trenutka gadjanja
float t0 = 0;

//podloga
typedef struct Surface{
	float size;
} Surface;
Surface surface;

GLuint _textureSurface;
GLuint _textureSrce2;
GLuint _textureSky;
GLuint _textureSphere;
GLuint _textureSrce;
GLUquadricObj *sphereObj = NULL;
GLUquadricObj *cylinder = NULL;
GLUquadricObj *disk1 = NULL;
GLUquadricObj *disk2 = NULL;

//funkcija za ucitavanje tekstura
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width, image->height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image->pixels);
	return textureId;
}

//funkcija za inicijalizaciju podloge
void initSurface(float x){
	surface.size = x;
}


Vec3f pocetnePozLoptica[3];

bool sphereInit = false; //indikator stanja loptica
bool kuglicaJump = false; //indikator stanja strelice

//inicijalizacija loptice
void initLoptice(){
	loptica1.position = new Vec3f(-3, 1.0 , -12.5);
	loptica1.precnik = 1;

	loptica2.position = new Vec3f(0, 1.0, -12.5);
	loptica2.precnik = 1;

	loptica3.position = new Vec3f(3, 1.0 , -12.5);
	loptica3.precnik = 1;

	pocetnePozLoptica[0] = Vec3f(-3, 5.5, -12.5);
	pocetnePozLoptica[1] = Vec3f(0, 5.5, -12.5);
	pocetnePozLoptica[2] = Vec3f(3, 5.5, -12.5);


	loptica1.vrednost = (rand()%10)*10+10;
	loptica2.vrednost = (rand()%10)*10+10;
	loptica3.vrednost = (rand()%10)*10+10;

	loptica1.crvena = 0.045f * (rand()%10) + 0.3f;
	loptica1.plava = 0.045f * (rand()%10) + 0.3f;
	loptica1.zelena = 0.045f * (rand()%10) + 0.3f;

	loptica2.crvena = 0.045f * (rand()%10) + 0.3f;
	loptica2.plava = 0.045f * (rand()%10) + 0.3f;
	loptica2.zelena = 0.045f * (rand()%10) + 0.3f;

	loptica3.crvena = 0.045f * (rand()%10) + 0.3f;
	loptica3.plava = 0.045f * (rand()%10) + 0.3f;
	loptica3.zelena = 0.045f * (rand()%10) + 0.3f;

	sphereInit = true;

}

//inicijalizacija strelice
void initStrelica(){

	strelica.position = new Vec3f( 0, 0, 12.5);
	strelica.precnik = 1;

}

Image* image1;
Image* image2;
Image* image3;
Image* image4;
Image* image5;
Image* image6;

void initRendering() {

	initLoptice();
	initStrelica();

	initSurface(25.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	

	image1 = loadBMP("surface.bmp");
	_textureSurface = loadTexture(image1);

	if(dan){
		image4 = loadBMP("sky_day.bmp");
		_textureSky = loadTexture(image4);

		image5 = loadBMP("srce_dan2.bmp");
		_textureSrce = loadTexture(image5);

		image6 = loadBMP("srce_dan3.bmp");
		_textureSrce2 = loadTexture(image6);
	}

	if(noc){
		image4 = loadBMP("sky.bmp");
		_textureSky = loadTexture(image4);

		image5 = loadBMP("srce_noc3.bmp");
		_textureSrce = loadTexture(image5);

		image6 = loadBMP("srce_noc3.bmp");
		_textureSrce2 = loadTexture(image6);
	}

	image5 = loadBMP("srce_noc3.bmp");
	_textureSrce = loadTexture(image5);

	image3 = loadBMP("sphere.bmp");
	_textureSphere = loadTexture(image3);

	delete image1;
	delete image5;
	delete image3;
	delete image6;
	delete image4;

}


// uglovi za rotiranje i x,y,z komponente mete
float _angle = 0.0f;
float _angle2 = 0.0f; 
float _angle3 = 0.0f;
float width,height;
float xm = 0;
float ym = 0;
float zm = 0;
float selection = 0;
float _pos = 0;

//renderivanje stringa
void renderBitmapString(float x, float y, void *font,char *string)
{  
  char *c;
  
  glRasterPos2f(x, y);
  
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}
char score_char [15];

//iscrtavanje 
void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glMatrixMode(GL_PROJECTION );
	glLoadIdentity();
	glOrtho(-2,2,-15,2,12,-12);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureSky);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glBegin(GL_QUADS);             
      //glColor3f(1.0f, 0.6f, 0.0f); // crvena
	  glTexCoord2f( 0.0, 0.0 );
	  
      glVertex2f(-3.0f, -15.0f);    // x, y
	  
	  glTexCoord2f( 1.0, 0.0 );
      glVertex2f( 3.0f, -15.0f);
	  glTexCoord2f( 1.0, 1.0 );
	  
      glVertex2f( 3.0f,  2.0f);
	  glTexCoord2f( 0.0, 1.0 );
	  
      glVertex2f(-3.0f,  2.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glLoadIdentity();

	if(lives>=1){
		glTranslatef(-1.65,-2.0,0.0);
		glEnable(GL_TEXTURE_2D);
	
		glBindTexture(GL_TEXTURE_2D, _textureSrce);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		glBegin(GL_QUADS);              
		  //glColor3f(1.0f, 0.6f, 0.0f); // 
		  glTexCoord2f( 0.0, 0.0 );
	  
		  glVertex2f(-0.15f, -0.5f);    // x, y
	  
		  glTexCoord2f( 1.0, 0.0 );
		  glVertex2f( 0.15f, -0.5f);
	  
		  glTexCoord2f( 1.0, 1.0 );
	  
		  glVertex2f( 0.15f,  0.5f);
		  glTexCoord2f( 0.0, 1.0 );
	  
		  glVertex2f(-0.15f,  0.5f);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	if(lives>=2){
	glTranslatef(0,-1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, _textureSrce);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBegin(GL_QUADS);              
      //glColor3f(1.0f, 0.6f, 0.0f); // Red
	  glTexCoord2f( 0.0, 0.0 );
	  
      glVertex2f(-0.15f, -0.5f);    // x, y
	  
	  glTexCoord2f( 1.0, 0.0 );
      glVertex2f( 0.15f, -0.5f);
	  
	  glTexCoord2f( 1.0, 1.0 );
	  
      glVertex2f( 0.15f,  0.5f);
	  glTexCoord2f( 0.0, 1.0 );
	  
      glVertex2f(-0.15f,  0.5f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	}
	if(lives>=3){
	glTranslatef(0,-1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, _textureSrce);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBegin(GL_QUADS);             
      //glColor3f(1.0f, 0.6f, 0.0f); // crvena
	  glTexCoord2f( 0.0, 0.0 );
	  
      glVertex2f(-0.15f, -0.5f);    // x, y
	  
	  glTexCoord2f( 1.0, 0.0 );
      glVertex2f( 0.15f, -0.5f);
	  
	  glTexCoord2f( 1.0, 1.0 );
	  
      glVertex2f( 0.15f,  0.5f);
	  glTexCoord2f( 0.0, 1.0 );
	  
      glVertex2f(-0.15f,  0.5f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	}
	if(lives>=4){
	glTranslatef(0,-1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, _textureSrce2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBegin(GL_QUADS);            
      //glColor3f(1.0f, 0.6f, 0.0f); // crvena
	  glTexCoord2f( 0.0, 0.0 );
	  
      glVertex2f(-0.15f, -0.5f);    // x, y
	  
	  glTexCoord2f( 1.0, 0.0 );
      glVertex2f( 0.15f, -0.5f);
	  
	  glTexCoord2f( 1.0, 1.0 );
	  
      glVertex2f( 0.15f,  0.5f);
	  glTexCoord2f( 0.0, 1.0 );
	  
      glVertex2f(-0.15f,  0.5f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	}
	if(lives>=5){
	glTranslatef(0,-1.0,0.0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, _textureSrce2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBegin(GL_QUADS);              // 
      //glColor3f(1.0f, 0.6f, 0.0f); // crvena
	  glTexCoord2f( 0.0, 0.0 );
	  
      glVertex2f(-0.15f, -0.5f);    // x, y
	  
	  glTexCoord2f( 1.0, 0.0 );
      glVertex2f( 0.15f, -0.5f);
	  
	  glTexCoord2f( 1.0, 1.0 );
	  
      glVertex2f( 0.15f,  0.5f);
	  glTexCoord2f( 0.0, 1.0 );
	  
      glVertex2f(-0.15f,  0.5f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat) width / (GLfloat) height, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
	0,0, 12,
	0,0.5, 0,
	0.0, 1.0, 0.0);

	////
	//glClearColor(0.11f, 0.56f, 1.0f, 1.0f);
	glLoadIdentity();
	glTranslatef(0.0f, -10.0f, -40);
	glRotatef(_angle, 0.0f, 1.0f, 0.0f);

	GLfloat ambientColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat lightPos0[] = { -0.5f, 0.8f, 0.1f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//podloga
	glShadeModel(GL_FLAT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureSurface);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_QUADS);
	
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	
	glVertex3f(-surface.size/2, 0, -surface.size/2);
	glTexCoord2f(1.0f, 0.0f);
	
	glVertex3f(-surface.size/2, 0, surface.size/2);
	glTexCoord2f(1.0f, 1.0f);
	
	glVertex3f(surface.size/2, 0, surface.size/2);
	glTexCoord2f(0.0f, 1.0f);
	
	glVertex3f(surface.size/2, 0, -surface.size/2);

	/*
	
	*/
	glEnd();
	glDisable(GL_TEXTURE_2D);
	

	//LOPTICA1
	glColor3f(loptica1.crvena,loptica1.zelena,loptica1.plava);
	sphereObj = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);
	glTranslatef((*loptica1.position)[0], (*loptica1.position)[1], (*loptica1.position)[2]);
	///////**
	if (fuel > 0 && selection==-1)
    {
      glPushMatrix ();
	  
      glDisable (GL_LIGHTING);
      glDisable (GL_DEPTH_TEST);

      glBegin (GL_POINTS);

      for (int i = 0; i < NUM_PARTICLES; i++)
	{
	  glColor3fv (particles[i].color);
	  glVertex3fv (particles[i].position);
	}
  
      glEnd ();

      glPopMatrix ();

      glEnable (GL_LIGHTING); 
      glEnable (GL_LIGHT0); 
      glEnable (GL_DEPTH_TEST);

      glNormal3f (0.0, 0.0, 1.0);

      for (int i = 0; i < NUM_DEBRIS; i++)
	{
	  glColor3fv (debris[i].color);

	  glPushMatrix ();
     
	  glTranslatef (debris[i].position[0],
			debris[i].position[1],
			debris[i].position[2]);

	  glRotatef (debris[i].orientation[0], 1.0, 0.0, 0.0);
	  glRotatef (debris[i].orientation[1], 0.0, 1.0, 0.0);
	  glRotatef (debris[i].orientation[2], 0.0, 0.0, 1.0);

	  glScalef (debris[i].scale[0],
		    debris[i].scale[1],
		    debris[i].scale[2]);

	  glBegin (GL_TRIANGLES);
	  glVertex3f (0.0, 0.5, 0.0);
	  glVertex3f (-0.25, 0.0, 0.0);
	  glVertex3f (0.25, 0.0, 0.0);
	  glEnd ();	  
	  
	  glPopMatrix ();
	}
    }else
		gluSphere(sphereObj, loptica1.precnik, 70, 70);
	//////
	glTranslatef(-(*loptica1.position)[0], -(*loptica1.position)[1], -(*loptica1.position)[2]);
	glTranslatef(xm, ym, zm);
	if (selection == -1){
		//glTranslatef(0, 0, 2);
		glColor3f(1.0f, 0.0f, 0.0f);
		//meta
		glutSolidTorus(loptica2.precnik / 16, loptica2.precnik / 4, 2, 50);
		glutSolidTorus(loptica2.precnik / 32, loptica2.precnik / 16, 2, 50);
		//strelica1
		glTranslatef(3 * loptica2.precnik / 4, 0, 0);
		glRotatef(-90, 0, 1, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(90, 0, 1, 0);
		glTranslatef(-3 * loptica2.precnik / 4, 0, 0);
		//strelica2
		glTranslatef(-3 * loptica2.precnik / 4, 0, 0);
		glRotatef(90, 0, 1, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(3 * loptica2.precnik / 4, 0, 0);
		//strelica3
		glTranslatef(0, 3 * loptica2.precnik / 4, 0);
		glRotatef(90, 1, 0, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(0, -3 * loptica2.precnik / 4, 0);
		//strelica4
		glTranslatef(0, -3 * loptica2.precnik / 4, 0);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 3 * loptica2.precnik / 4, 0);
		//
		//glTranslatef(0, 0, -2);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glTranslatef(-xm, -ym, -zm);
	//LOPTICA2
	glColor3f(loptica2.crvena,loptica2.zelena,loptica2.plava);
	sphereObj = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);
	glTranslatef((*loptica2.position)[0], (*loptica2.position)[1], (*loptica2.position)[2]);
	///////**	
	if (fuel > 0 && selection==0)
    {
      glPushMatrix ();
	  
      glDisable (GL_LIGHTING);
      glDisable (GL_DEPTH_TEST);

      glBegin (GL_POINTS);

      for (int i = 0; i < NUM_PARTICLES; i++)
	{
	  glColor3fv (particles[i].color);
	  glVertex3fv (particles[i].position);
	}
  
      glEnd ();

      glPopMatrix ();

      glEnable (GL_LIGHTING); 
      glEnable (GL_LIGHT0); 
      glEnable (GL_DEPTH_TEST);

      glNormal3f (0.0, 0.0, 1.0);

      for (int i = 0; i < NUM_DEBRIS; i++)
	{
	  glColor3fv (debris[i].color);

	  glPushMatrix ();
     
	  glTranslatef (debris[i].position[0],
			debris[i].position[1],
			debris[i].position[2]);

	  glRotatef (debris[i].orientation[0], 1.0, 0.0, 0.0);
	  glRotatef (debris[i].orientation[1], 0.0, 1.0, 0.0);
	  glRotatef (debris[i].orientation[2], 0.0, 0.0, 1.0);

	  glScalef (debris[i].scale[0],
		    debris[i].scale[1],
		    debris[i].scale[2]);

	  glBegin (GL_TRIANGLES);
	  glVertex3f (0.0, 0.5, 0.0);
	  glVertex3f (-0.25, 0.0, 0.0);
	  glVertex3f (0.25, 0.0, 0.0);
	  glEnd ();	  
	  
	  glPopMatrix ();
	}
    }else
		gluSphere(sphereObj, loptica1.precnik, 70, 70);
	//////
	glTranslatef(-(*loptica2.position)[0], -(*loptica2.position)[1], -(*loptica2.position)[2]);
	glTranslatef(xm, ym, zm);
	if (selection == 0){
		//meta
		//glTranslatef(0, 0, 2);
		glColor3f(1.0f, 0.0f, 0.0f);
		glutSolidTorus(loptica2.precnik / 16, loptica2.precnik / 4, 2, 50);
		glutSolidTorus(loptica2.precnik / 32, loptica2.precnik / 16, 2, 50);
		//strelica1
		glTranslatef(3*loptica2.precnik/4, 0, 0);
		glRotatef(-90,0,1,0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(90, 0, 1, 0);
		glTranslatef(-3*loptica2.precnik/4, 0, 0);
		//strelica2
		glTranslatef(-3*loptica2.precnik/4, 0, 0);
		glRotatef(90, 0, 1, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(3*loptica2.precnik/4, 0, 0);
		//strelica3
		glTranslatef(0, 3 * loptica2.precnik / 4, 0);
		glRotatef(90, 1, 0, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(0, -3 * loptica2.precnik / 4, 0);
		//strelica4
		glTranslatef(0, -3 * loptica2.precnik / 4, 0);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 3 * loptica2.precnik / 4, 0);
		//
		//glTranslatef(0, 0, -2);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glTranslatef(-xm, -ym, -zm);
	
	//LOPTICA3
	glColor3f(loptica3.crvena,loptica3.zelena,loptica3.plava);
	sphereObj = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);
	glTranslatef((*loptica3.position)[0], (*loptica3.position)[1], (*loptica3.position)[2]);
	///////**	
	if (fuel > 0 && selection==1)
    {
      glPushMatrix ();
	  
      glDisable (GL_LIGHTING);
      glDisable (GL_DEPTH_TEST);

      glBegin (GL_POINTS);

      for (int i = 0; i < NUM_PARTICLES; i++)
	{
	  glColor3fv (particles[i].color);
	  glVertex3fv (particles[i].position);
	}
  
      glEnd ();

      glPopMatrix ();

      glEnable (GL_LIGHTING); 
      glEnable (GL_LIGHT0); 
      glEnable (GL_DEPTH_TEST);

      glNormal3f (0.0, 0.0, 1.0);

      for (int i = 0; i < NUM_DEBRIS; i++)
	{
	  glColor3fv (debris[i].color);

	  glPushMatrix ();
     
	  glTranslatef (debris[i].position[0],
			debris[i].position[1],
			debris[i].position[2]);

	  glRotatef (debris[i].orientation[0], 1.0, 0.0, 0.0);
	  glRotatef (debris[i].orientation[1], 0.0, 1.0, 0.0);
	  glRotatef (debris[i].orientation[2], 0.0, 0.0, 1.0);

	  glScalef (debris[i].scale[0],
		    debris[i].scale[1],
		    debris[i].scale[2]);

	  glBegin (GL_TRIANGLES);
	  glVertex3f (0.0, 0.5, 0.0);
	  glVertex3f (-0.25, 0.0, 0.0);
	  glVertex3f (0.25, 0.0, 0.0);
	  glEnd ();	  
	  
	  glPopMatrix ();
	}
    }else
		gluSphere(sphereObj, loptica1.precnik, 70, 70);
	//////
	glTranslatef(-(*loptica3.position)[0], -(*loptica3.position)[1], -(*loptica3.position)[2]);
	glTranslatef(xm, ym, zm);
	if (selection == 1){
		//meta
		//glTranslatef(0, 0, 2);
		glColor3f(1.0f, 0.0f, 0.0f);
		glutSolidTorus(loptica2.precnik / 16, loptica2.precnik / 4, 2, 50);
		glutSolidTorus(loptica2.precnik / 32, loptica2.precnik / 16, 2, 50);
		//strelica1
		glTranslatef(3 * loptica2.precnik / 4, 0, 0);
		glRotatef(-90, 0, 1, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(90, 0, 1, 0);
		glTranslatef(-3 * loptica2.precnik / 4, 0, 0);
		//strelica2
		glTranslatef(-3 * loptica2.precnik / 4, 0, 0);
		glRotatef(90, 0, 1, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(-90, 0, 1, 0);
		glTranslatef(3 * loptica2.precnik / 4, 0, 0);
		//strelica3
		glTranslatef(0, 3 * loptica2.precnik / 4, 0);
		glRotatef(90, 1, 0, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(0, -3 * loptica2.precnik / 4, 0);
		//strelica4
		glTranslatef(0, -3 * loptica2.precnik / 4, 0);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(loptica2.precnik / 8, loptica2.precnik / 4, 10, 5);
		glRotatef(90, 1, 0, 0);
		glTranslatef(0, 3 * loptica2.precnik / 4, 0);
		//
		//glTranslatef(0, 0, -2);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glTranslatef(-xm, -ym, -zm);
	
	//STRELICA
	glColor3f(0.69f, 0.76f, 0.87f);
	sphereObj = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);
	glTranslatef((*strelica.position)[0], (*strelica.position)[1], (*strelica.position)[2]);

	glRotatef(180, 0, 1, 0);
	glRotatef(-_angle2, 1, 0, 0);
	//
	glRotatef(-_angle3, 0, 1, 0);
	glTranslatef(0, 0, 0.2);
	glutSolidCone(0.038, 0.75, 70, 70);
	glTranslatef(0, 0, -0.2);
	//sipka iza strelice
	glColor3f(0.69f, 0.13f, 0.13f);
	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);
	glTranslatef(0, 0, -0.8);
	gluCylinder(cylinder, 0.05, 0.05, 1, 200, 10);
	glTranslatef(0, 0, +0.8);
	//pera na sipki
	
	glColor3f(0.54f, 0.0f, 0.0f);
	disk1 = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);
	
	glTranslatef(0, 0, -0.8);
	glRotatef(90, 0, 1, 0);
	
	gluDisk(disk1, 0, 0.3, 10, 10);
	
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 0, 0.8);
	//
	//pera na sipki
	disk2 = gluNewQuadric();
	gluQuadricDrawStyle(sphereObj, GLU_FILL);
	gluQuadricTexture(sphereObj, GL_TRUE);
	gluQuadricNormals(sphereObj, GLU_SMOOTH);

	glTranslatef(0, 0, -0.8);
	glRotatef(-90, 1, 0, 0);
	
	gluDisk(disk2, 0, 0.3, 10, 10);
	
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, 0.8);
	glColor3f(1.0f, 1.0f, 1.0f);

	//
	//glRotatef(30, 1, 0, 0);
	
	glRotatef(-180, 0, 1, 0);
	glTranslatef(-(*strelica.position)[0], -(*strelica.position)[1], -(*strelica.position)[2]);
	
	///////////////// - 
	

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glScalef(1, -1, 1);
    glTranslatef(0, -height, 0);
	if(noc)
		glColor3f(1.0f,1.0f,1.0f);
	if(dan)
		glColor3f(0.0f,0.0f,0.0f);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();
    sprintf_s (score_char, "Score: %d", score);
	
	renderBitmapString(30,(width/height)*40,(void *)font, score_char);
	renderBitmapString(30,(width/height)*60,(void *)font2,"F2 - Restart      W,A,S,D - Aim      SPACE - Fire      Left click, Right click - Rotate camera");
	renderBitmapString(30,(width/height)*80,(void *)font2,"Q - Day      E - Night");
    renderBitmapString(30,(width/height)*100,(void *)font2,"Esc - Quit");
	if(over){
		renderBitmapString(305,(width/height)*350,(void *)font,"GAME OVER");
		renderBitmapString(325,(width/height)*380,(void *)font2,"F2 - Restart");
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
	glColor3f(1.0f, 1.0f, 1.0f);
	//////////////////

	glutSwapBuffers();
}

/////**
void idle (void)
{
  int    i;

  if (1)
    {
      if (fuel > 0)
	{
	  for (i = 0; i < NUM_PARTICLES; i++)
	    {
	      particles[i].position[0] += particles[i].speed[0] * 0.2;
	      particles[i].position[1] += particles[i].speed[1] * 0.2;
	      particles[i].position[2] += particles[i].speed[2] * 0.2;
	      
	      particles[i].color[0] -= 1.0 / 500.0;
	      if (particles[i].color[0] < 0.0)
		{
		  particles[i].color[0] = 0.0;
		}
	      
	      particles[i].color[1] -= 1.0 / 100.0;
	      if (particles[i].color[1] < 0.0)
		{
		  particles[i].color[1] = 0.0;
		}
	      
	      particles[i].color[2] -= 1.0 / 50.0;
	      if (particles[i].color[2] < 0.0)
		{
		  particles[i].color[2] = 0.0;
		}
	    }
	  
	  for (i = 0; i < NUM_DEBRIS; i++)
	    {
	      debris[i].position[0] += debris[i].speed[0] * 0.1;
	      debris[i].position[1] += debris[i].speed[1] * 0.1;
	      debris[i].position[2] += debris[i].speed[2] * 0.1;
	      
	      debris[i].orientation[0] += debris[i].orientationSpeed[0] * 10;
	      debris[i].orientation[1] += debris[i].orientationSpeed[1] * 10;
	      debris[i].orientation[2] += debris[i].orientationSpeed[2] * 10;
	    }
	  
	  --fuel;
	}
    }
  
  glutPostRedisplay ();
}
/////
// u slucaju resizeovanja prozora
void handleResize(int w, int h) {
		glViewport(0, 0, w, h);
		width = w;
		height = h;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
		glutReshapeWindow(720,720);
}

void tempkey(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //esc key
			exit(0);
			break;
	}
}

//tajmer za eksploziju
void timer(int value){
	newExplosion();
}


float t = 0;
float k = 0;
int change = 0;
float amplituda = 4;
void update(int value) {
	if (sphereInit){
		
		(*loptica1.position)[1] = pocetnePozLoptica[0][1] + 9*sin(t) / 2;
		(*loptica2.position)[1] = pocetnePozLoptica[1][1] + 9*sin(t) / 2;
		(*loptica3.position)[1] = pocetnePozLoptica[2][1] + 9*sin(t) / 2;
		float perioda = sqrt((asin(1 / (2 * amplituda))));
		float brzina = 1 / (1.0 * perioda);
		float tf = 25 / brzina;
		if (kuglicaJump){
			(*strelica.position)[0] = selection * (3 / tf) * k;					//  K O O R D I N A T E
			_angle3 = atan(selection  / tf) * 180 / 3.14;
			(*strelica.position)[1] = amplituda*sin(perioda*k);
			_angle2 = (atan(perioda  * cos(perioda*k)) * 180) / 3.141592; 	
			(*strelica.position)[2] = 12.5 - brzina* k;
			
			if((*strelica.position)[2]<=-11.5 && sudar==0 && change == 0){
					lives--;
					if(lives == 0){
						over = 1;
						glutKeyboardFunc(tempkey);
						glutMouseFunc(NULL);
					}
					change = 1;
					glutPostRedisplay();
			}

			if((*strelica.position)[2]<=-11.5 && sudar==1){
				newExplosion();
				if(selection == -1){
					score += loptica1.vrednost;
					loptica1.crvena = 0.045f * (rand()%10) + 0.3f;
					loptica1.plava = 0.045f *  (rand()%10) + 0.3f;
					loptica1.zelena = 0.045f *  (rand()%10) + 0.3f;
				}else if (selection ==0){
					score += loptica2.vrednost;
					loptica2.crvena = 0.045f * (rand()%10) + 0.3f;
					loptica2.plava = 0.045f * (rand()%10) + 0.3f;
					loptica2.zelena = 0.045f * (rand()%10) + 0.3f;
				}else if (selection ==1){
					score += loptica3.vrednost;
					loptica3.crvena = 0.045f * (rand()%10) + 0.3f;
					loptica3.plava = 0.045f * (rand()%10) + 0.3f;
					loptica3.zelena = 0.045f * (rand()%10) + 0.3f;
				}
				sudar = 0;
				change = 1;
				loptica1.vrednost = (rand()%10)*10+10;
				loptica2.vrednost = (rand()%10)*10+10;
				loptica3.vrednost = (rand()%10)*10+10;
			}
			

			if (((*strelica.position)[1] < 0 || (*strelica.position)[3] > - surface.size * 1.75) && k > 0)
			{
				kuglicaJump = false;
				k=0;
				initStrelica();
				change = 0;
			}
			

			k += 0.1;
		}
		else{
			_angle3 = atan(selection  / tf) * 180 / 3.14;
			_angle2 = atan(perioda *  cos(perioda*k)) * 180 / 3.141592;
		}
		t += 0.1;
		xm = selection * (3 / tf) * tf;
		ym = abs(amplituda*sin(perioda*tf));
		zm = 12.5 - brzina* tf;
	}
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

//**************************************************************************KOLIZIJA


float funkcijaRastojanja(int loptica, float t, float brzina, float perioda, float amplituda, float tf, float t0) {
	Vec3f lopticaPoz(pocetnePozLoptica[loptica].getV(0), pocetnePozLoptica[loptica].getV(1) + 9*sin(t + t0) / 2, pocetnePozLoptica[loptica].getV(2));
	Vec3f strelicaPoz(selection * (3 / tf) * t, amplituda*sin(perioda*t), 12.5 - brzina* t);
	return lopticaPoz.razdaljina(strelicaPoz) - (loptica1.precnik + strelica.precnik/3);
}

float izvodFunkcijeRastojanja(int loptica, float t, float brzina, float perioda, float amplituda, float tf, float t0) {
	float xdio, ydio, zdio;
	Vec3f lopticaPoz(pocetnePozLoptica[loptica].getV(0), pocetnePozLoptica[loptica].getV(1) + 9*sin(t + t0) / 2, pocetnePozLoptica[loptica].getV(2));
	Vec3f strelicaPoz(selection * (3 / tf) * t, amplituda*sin(perioda*t), 12.5 - brzina* t);
	xdio = -(lopticaPoz.getV(0) - strelicaPoz.getV(0)) * selection * 3 / tf;
	ydio = (lopticaPoz.getV(1) - strelicaPoz.getV(1)) * (9*cos(t0 + t) / 2 - amplituda *cos(perioda * t) * perioda);
	zdio = (lopticaPoz.getV(2) - strelicaPoz.getV(2)) * brzina;
	return (xdio + ydio + zdio) / lopticaPoz.razdaljina(strelicaPoz);
}

float kolizija(int maxIter, float tacnost, int loptica, float brzina, float perioda, float amplituda, float tf, float t0) {
	float tn = tf * 4 / 5;			
	float tnplus1 = tf * 4 / 5;		
	float fn = 0, fnplus1 = 0;		
	for (int i = 1; i < maxIter; i++) {
		fn = funkcijaRastojanja(loptica, tn, brzina, perioda, amplituda, tf, t0);
		tnplus1 = tn - (fn / izvodFunkcijeRastojanja(loptica, tn, brzina, perioda, amplituda, tf, t0));
		//tnplus1 = tn + 0.1;    ***************TEST
		fnplus1 = funkcijaRastojanja(loptica, tnplus1, brzina, perioda, amplituda, tf, t0);
		if (fnplus1 < tacnost)
			break;
		tn = tnplus1;
	}
	if (fnplus1>tacnost)
		return 0.0;
	else return tnplus1;
}
//**************************************************************************

void handleKeypress(unsigned char key, int x, int y) {
	
		switch (key) {
		case 27: //esc key
			exit(0);
			break;
		case 'a':
		case 'A':
			if (!kuglicaJump){
				if (selection != -1)
					selection += -1;
				else
					selection = 1;
				break;
			}
		case 'd':
		case 'D':
			if (!kuglicaJump){
				if (selection != 1)
					selection += 1;
				else
					selection = -1;
				break;
			}
		case 'w':
		case 'W':
			if (!kuglicaJump){
				if (amplituda < 10 && !kuglicaJump)
					amplituda += 0.5;
			}
			break;
		case 's':
		case 'S':
			if (!kuglicaJump){
				if (amplituda > 4 && !kuglicaJump)
					amplituda -= 0.5;
			}
			break;
		case 'q':
		case 'Q':
			dan = 1;
			noc = 0;
			image4 = loadBMP("sky_day.bmp");
			_textureSky = loadTexture(image4);
			image5 = loadBMP("srce_dan2.bmp");
			_textureSrce = loadTexture(image5);
			image6 = loadBMP("srce_dan3.bmp");
			_textureSrce2 = loadTexture(image6);
			break;
		case 'e':
		case 'E':
			noc = 1;
			dan = 0;
			image4 = loadBMP("sky.bmp");
			_textureSky = loadTexture(image4);
			image5 = loadBMP("srce_noc3.bmp");
			_textureSrce = loadTexture(image5);
			image6 = loadBMP("srce_noc3.bmp");
			_textureSrce2 = loadTexture(image6);
			break;
		case 32:
			if (!kuglicaJump){
				kuglicaJump = true;
				t0 = t;
				float perioda = sqrt((asin(1 / (2 * amplituda))));
				float brzina = 1 / (1.0 * perioda);
				float tf = 25 / brzina;
				cout << t << endl;
				if (kolizija(100, 0.0001, (int)selection + 1, brzina, perioda, amplituda, tf, t0) != 0.0f){
					cout << "Kolizija" << endl;
					sudar = 1;
				}
				cout << funkcijaRastojanja((int)selection + 1, tf, brzina, perioda, amplituda, tf, t0) << endl;
			}
				
			break;
		}
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			_angle += 10;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			_angle -= 10;
		}
	}
}

void special(int key, int x, int y){
	switch (key){
	case GLUT_KEY_F2:
		lives = 5;
		score = 0;
		over = 0;
		glutKeyboardFunc(handleKeypress);
		glutMouseFunc(mouse);
		glutPostRedisplay();
			break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(720,720);

	glutCreateWindow("Star darts");
	initRendering();

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc(special);
	glutIdleFunc(idle);
	glutReshapeFunc(handleResize);
	glutMouseFunc(mouse);
	glutTimerFunc(25, update, 0);

	srand (time(NULL));

	glutMainLoop();
	return 0;
}