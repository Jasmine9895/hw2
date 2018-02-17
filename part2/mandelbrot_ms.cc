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


void compute_single_row(int row_num,int width,Color_Point *row_colors,double minX,double minY,double it,double jt)
{
	#ifdef DEBUG	
	//printf("Computing for row=%d\n",row_num);
	#endif
	double y=minY + row_num*it;
	int curr_index = 0;
		int i=row_num;
    		double x = minX;
    		for (int j = 0; j < width; ++j) 
		{
      			
			curr_index= j;
      			Color_Point t = render_color(mandelbrot(x,y)/512.0);
			row_colors[curr_index].r = t.r;
			row_colors[curr_index].g = t.g;
			row_colors[curr_index].b = t.b;

			#ifdef DEBUG_EXTENDED
			
			int curr_index= j;
			printf("CI:%d r:%d ir:%d",curr_index,t.r,row_colors[curr_index].r);		
			#endif
			x += jt;
    		}
		
  	
	#ifdef DEBUG_EXTENDED
	printf("/n Now printing in compute after computation loop\n");
		for(int j= 0;j<width;j++)
		{
			curr_index=  j;
			printf("CI:%d r:%d ",curr_index,row_colors[curr_index].r);		
		}
		printf("\n");
	#endif 


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

/*

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
  i}
*/
int i=0;
//Here i is the column number
 while(i<height)
{	
	
	Color_Point *single_row_colors = (Color_Point *)malloc(sizeof(Color_Point) * width);
	compute_single_row(i,width,single_row_colors,minX,minY,it,jt);
	
	for(int j=0;j<width;++j)
	{
		Color_Point t = single_row_colors[j];
		img_view(j,i) = gil::rgb8_pixel_t(t.r,t.g,t.b);
	}
	i++;	
}
  gil::png_write_view("mandelbrot.png", const_view(img));
}

/* eof */
