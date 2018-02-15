/**
 *  \file mandelbrot_serial.cc
 *  \brief Lab 2: Mandelbrot set serial code
 */


#include <iostream>
#include <cstdlib>
#include <cmath>
#include "render.hh"
#include <vector>
using namespace std;

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
void compute_row(int p,int width,vector<gil::rgb8_pixel_t> &img,int N,double minX,double minY,double it,double jt)
{
	

	double y=minY + p*N*it;
  	for (int i = p*N; i < N*(p+1); ++i) 
  	{	
    		double x = minX;
    		for (int j = 0; j < width; ++j) 
		{
      			
			#ifdef DEBUG
			printf("ij=(%d,%d),xy=(%0.1lf,%0.1lf)    ",i,j,x,y);
			#endif
			//img_view(j, i) = render(mandelbrot(x, y)/512.0);
      			img[(i-p*N)*width + j] = render(mandelbrot(x,y)/512.0);
			x += jt;
    		}
    		y += it;
		
		#ifdef DEBUG
		printf("\n");
		#endif
  	}
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
  int Num_Processors = 3;
  int N = ceil(float(height)/float(Num_Processors));
  //vector<vector<vector<int>> all_row_blocks(Num_Processors,vector<vector<int>(N,vector<int>(width)));
  vector<vector<gil::rgb8_pixel_t>> all_row_blocks;
  printf("N = %d\n",N);
  for(int p=0;p<Num_Processors;p++)
  {

	#ifdef DEBUG
	printf("p=%d  y=%0.1lf\n",p,y);
	#endif
	vector<gil::rgb8_pixel_t> row_img(width*N);
	compute_row(p,width,row_img,N,minX,minY,it,jt);
	all_row_blocks.push_back(row_img);
 }
 
 for(int p=0;p<Num_Processors-1;p++)
{
	for (int i = p*N; i < N*(p+1); ++i)
  	{
    		for (int j = 0; j < width; ++j)
		{

			#ifdef DEBUG
			printf("ij=(%d,%d),xy=(%0.1lf,%0.1lf)    ",i,j,x,y);
			#endif
			img_view(j, i) = all_row_blocks[p][(i-p*N)*width+j];
    		}

		#ifdef DEBUG
		printf("\n");
		#endif
  	}
}

int p = Num_Processors-1;

	for (int i = p*N; i < height; ++i)
  	{
    		for (int j = 0; j < width; ++j)
		{

			#ifdef DEBUG
			printf("ij=(%d,%d),xy=(%0.1lf,%0.1lf)    ",i,j,x,y);
			#endif
			img_view(j, i) = all_row_blocks[p][(i-p*N)*width+j];
    		}

		#ifdef DEBUG
		printf("\n");
		#endif
  	}

	

 	gil::png_write_view("mandelbrot.png", const_view(img));
}

/* eof */
