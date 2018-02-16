/**
 *  \file mandelbrot_serial.cc
 *  \brief Lab 2: Mandelbrot set serial code
 */


#include <iostream>
#include <cstdlib>

#include "render.hh"

using namespace std;

#define DEBUG
#define WIDTH 1000
#define HEIGHT 1000

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}


Color_Point Compute_Color(int r,int c,double minX,double minY,double jt,double it)
{
	double x = minX + c*jt;
	double y = minY + r*it;
		
        Color_Point temp = render_color(mandelbrot(x, y)/512.0);
	return temp;		

}


int
main(int argc, char* argv[]) {
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  
  int height, width;
  if (argc == 3) {
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    assert (height > 0 && width > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }

  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;


  gil::rgb8_image_t img(height, width);
  auto img_view = gil::view(img);



  y = minY;
  for (int i = 0; i < height; ++i) {
    x = minX;
    for (int j = 0; j < width; ++j) {
	#ifdef DEBUG
	printf("ij=(%d,%d),xy=(%0.1lf,%0.1lf)    ",i,j,x,y);
	#endif
      	Color_Point t = Compute_Color(i,j,minX,minY,jt,it);	
	img_view(j, i) = gil::rgb8_pixel_t(t.r,t.g,t.b);
      x += jt;
    }
   #ifdef DEBUG
   printf("\n");
   #endif
    y += it;
  }
  gil::png_write_view("mandelbrot.png", const_view(img));
}

/* eof */
