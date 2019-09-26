/*
 *  Timothy Mason, homework 3, Scene in 3D
 * CSCI 5229 Computer Graphics - University of Colorado Boulder
 *
 * Based heavily on example 8, ex8.c
 *
 *  Key bindings:
 *  a          Toggle axes
 *  arrows     Change view angle
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include "rocket.h"

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int axes=1;       //  Display axes

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*M_PI/180))
#define Sin(x) (sin((x)*M_PI/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 256  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *ch++);
}

/*
 * Draw vertex in cylindrical coordinates (r, theta, z)
 */
static void cylVertex(double r, double th, double z)
{
   glVertex3d(r*Cos(th), r*Sin(th), z);
}

/*
 * Draw a radially symmetric solid
 *
 * profile: pointer to an array of x,y coordinates representing the surface profile of the solid
 * size: The number of points in the profile array
 * bz,by,bz: 3D coordinates of the base of the solid
 * rx,ry,rz: 3D vector for rotation of the solid.
 * ph:  Angle to rotate the solid around (rx,ry,rz)
 * s: the scale of the solid
 */
static void lathe(dpp profile, int size, double bx, double by,double bz, double rx, double ry, double rz, double ph, double s)
{
   const int d=15;
   int th,i;

   // Save transformation
   glPushMatrix();

   // Offset and scale
   glTranslated(bx,by,bz);
   glRotated(ph, rx, ry, rz);
   glScaled(s,s,s);

   // Latitude bands
   for (i=1; i<size; i++)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0; th<=360; th+=d)
      {
         cylVertex(profile[i-1].x, th, profile[i-1].y);
         cylVertex(profile[i].x, th, profile[i].y);
      }
      glEnd();
   }

   // Top cap; if necessary
   if (profile[0].x != 0.0)
   {
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(0,0,1);
      glVertex3d(0, profile[0].y, 0);
      for (th = 0; th <= 360; th += d)
         cylVertex(profile[0].x, th, profile[0].y);
      glEnd();
   }

   // Bottom cap; if necessary
   if (profile[size-1].x != 0.0)
   {
      // Draw a triangle fan from the origin to the final circle.
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(0,0,1);
      glVertex3d(0, profile[size-1].y, 0);
      for (th = 0; th <= 360; th += d)
         cylVertex(profile[size-1].x, th, profile[size-1].y);
      glEnd();
   }

   // undo transformations
   glPopMatrix();
}

/*
 *  Draw vertex in spherical coordinates
 */
static void sphVertex(double th,double ph, double r)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a sphere
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere(double x, double y, double z, double r)
{
   const int d=15;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  South pole cap
   glBegin(GL_TRIANGLE_FAN);
   sphVertex(0,-90, 1);
   for (th=0;th<=360;th+=d)
   {
      sphVertex(th,d-90, 1);
   }
   glEnd();

   //  Latitude bands
   for (ph=d-90;ph<=90-2*d;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         sphVertex(th,ph, 1);
         sphVertex(th,ph+d, 1);
      }
      glEnd();
   }

   //  North pole cap
   glBegin(GL_TRIANGLE_FAN);
   sphVertex(0,90, 1);
   for (th=0;th<=360;th+=d)
   {
      sphVertex(th,90-d, 1);
   }
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);

   //  Draw rocket
   glColor3f(0,1,1);
   lathe(rocket_profile, ROCKET_POINT_COUNT, 1,1,0, 1,1,0,30, 1.0/25.0);
   glColor3f(1,1,0);
   lathe(rocket_profile, ROCKET_POINT_COUNT, -1,0,0, 1,0,1,85, -1.0/20.0);
   glColor3f(1,0,1);
   lathe(rocket_profile, ROCKET_POINT_COUNT, 0,0.5,1.5, 0,1,1,161, 1.0/30.0);
   glColor3f(1,0,0);
   lathe(rocket_profile, ROCKET_POINT_COUNT, 0,-0.5,-1, 0,1,0,35, -1.0/35.0);
   glColor3f(0,1,0);
   lathe(rocket_profile, ROCKET_POINT_COUNT, 1.1,1.1,1.1, 0,0,0,0, 1.0/30.0);

   //  Draw spheres
   // sphere(0,0,0 , 0.4);
   // sphere(1,0,0 , 0.2);
   // sphere(0,1,0 , 0.2);

   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,25);
   //  Print the text string
   Print("Angle=%d,%d",th,ph);
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1-axes;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   const double dim=2.5;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);

   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

   //  Create the window
   glutCreateWindow("Timothy Mason");

   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);

   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);

   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);

   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);

   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();

   return 0;
}
