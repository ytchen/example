#include <stdio.h>
#include <stdlib.h>
#include "LifeForm.h"


#if !(NO_WINDOW)
#include <GL/freeglut.h>   // glut utility toolkit header
#endif
#include "Window.h"
const char* Canvas::color_strings[] = {
  "black", "blue", "green", "cyan", 
  "magenta", "red", "coral", "yellow"
  };



/*
   Main procedure:  initialize OpenGL display, set up monochrome or color
   mapping, create a window, wait for it to be mapped, draw rectangles
   in various forms, and exit.
*/
Canvas::Canvas(int w, int h) : width(w), height(h)
{
#if !(NO_WINDOW)
	// I'm a horrible person, but I'm faking the glutinit call
	int argc = 1;
	char *argv[1];
	argv[0] = new char[9];
	strncpy(argv[0],"LifeForm",9);
	glutInit(&argc, argv);
        glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				                // Black Background
	glClearDepth(1.0f);								// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
        glEnable ( GL_COLOR_MATERIAL );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
        glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
        glutCreateWindow    ( "LifeForm Project" ); // Window Title (argv[0] for current directory as title)
	glutDisplayFunc     ( Canvas::display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc     ( Canvas::reshape );
 
#endif /* !(NO_WINDOW) */
}
//static
void Canvas::display(void) {
#if ! (NO_WINDOW)
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	LifeForm::redisplay_all();
        glFlush();
	glutSwapBuffers();
	glutMainLoopEvent();
#endif
	// display the winder
}
void Canvas::reshape(int w, int h) {
#if ! (NO_WINDOW)
  glViewport     ( 0, 0, w, h );
  glMatrixMode   ( GL_PROJECTION );  // Select The Projection Matrix
  glLoadIdentity ( );                // Reset The Projection Matrix
	glOrtho(0,500,0,500,-100,5000);
  glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
  glLoadIdentity ( );    // Reset The Model View Matrix
#endif
}
void Canvas::set_color(Color x)
{
#if !(NO_WINDOW)
	// translate color to glColor4f thingy
	glColor4f(red[x],green[x],blue[x],1.0f);

#endif /* !(NO_WINDOW) */
}

void Canvas::draw_point(int x, int y)
{
#if !(NO_WINDOW)
        glBegin(GL_POINTS);
	glVertex2i(x,y);
	glEnd();
#endif /* !(NO_WINDOW) */ 
}

void Canvas::flush(void) 
{
#if !(NO_WINDOW)
	// make sure all the drawing stuff happens
    glFlush();
	glutSwapBuffers();
	glutMainLoopEvent();
	//display();
#endif /* !(NO_WINDOW) */
}


void Canvas::draw_rectangle(int x1, int y1, int x2, int y2, bool fill)
{
#if !(NO_WINDOW)
	glRecti(x1,y1,x2,y2);
#endif /* !(NO_WINDOW) */
}


void Canvas::draw_line(int x1, int y1, int x2, int y2)
{
#if !(NO_WINDOW)

	glBegin(GL_LINE);
	glVertex3i(x1,y1,0);
	glVertex3i(x2,y2,0);
	glEnd();
#endif /* !(NO_WINDOW) */
}
 

void Canvas::clear(void)
{
#if !(NO_WINDOW)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#endif /* !(NO_WINDOW) */
}
#if !(NO_WINDOW)
GLfloat Canvas::red[8] = {0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f};
GLfloat Canvas::green[8] = {0.0f,0.0f,1.0f,1.0f,0.0f,0.0f,0.55f,1.0f};
GLfloat Canvas::blue[8] = {0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f,0.0f};

#endif
