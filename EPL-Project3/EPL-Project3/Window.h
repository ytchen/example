#ifndef _Window_h
#define _Window_h 

#if !(NO_WINDOW)
#include <GL/freeglut.h>
#endif
#include "Color.h"

class Canvas {
  int width; 
  int height;
  static const char* color_strings[];
  
  static float red[8];
  static float green[8];
  static float blue[8];
  static void display(void);
	static void reshape(int w,int h);
public:
  Canvas(int width = 600, int height = 450);
  ~Canvas() {} // should close the window, yes?
 
  void set_color(Color);
  void draw_rectangle(int x1, int y1, int , int , bool=true);
	void draw_point(int x, int y);
	void draw_line(int x1, int y1, int x2, int y2);
	static void flush(void);
  void clear(void);
 
};



#endif /* ! _Window_h */
