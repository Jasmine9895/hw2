/**
 *  \file mandelbrot_serial.cc
 *  \brief Lab 2: Mandelbrot set serial code
 */


#include <iostream>
#include <cstdlib>
#include <cmath>
#include "render.hh"
#include <vector>
#include <mpi.h>
#include <assert.h>

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
void compute_row(int p,int width,Color_Point *img,int N,double minX,double minY,double it,double jt)
{
	

	double y=minY + p*N*it;
  	for (int i = p*N; i < N*(p+1); ++i) 
  	{	
    		double x = minX;
    		for (int j = 0; j < width; ++j) 
		{
      			
			//img_view(j, i) = render(mandelbrot(x, y)/512.0);
      			img[(i-p*N)*width + j] = render_color(mandelbrot(x,y)/512.0);
			#ifdef DEBUG
      			Color_Point t = render_color(mandelbrot(x,y)/512.0);
			printf("ij=(%d,%d),xy=(%0.1lf,%0.1lf),r=%d    ",i,j,x,y,t.r);
			#endif
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
  vector<Color_Point*> all_row_blocks;
  printf("N = %d\n",N);

  for(int p=0;p<Num_Processors;p++)
  {

	#ifdef DEBUG
	printf("p=%d  y=%0.1lf\n",p,y);
	#endif
//	vector<Color_Point> row_img(width*N);
   	Color_Point *row_img = (Color_Point *)malloc(sizeof(Color_Point) * N*width);
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
			int r = all_row_blocks[p][(i-p*N)*width+j].r;
			int g = all_row_blocks[p][(i-p*N)*width+j].g;
			int b = all_row_blocks[p][(i-p*N)*width+j].b;
         		img_view(j, i) = gil::rgb8_pixel_t(r,g,b);
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
         		
			int r = all_row_blocks[p][(i-p*N)*width+j].r;
			int g = all_row_blocks[p][(i-p*N)*width+j].g;
			int b = all_row_blocks[p][(i-p*N)*width+j].b;
         		img_view(j, i) = gil::rgb8_pixel_t(r,g,b);
    		}

		#ifdef DEBUG
		printf("\n");
		#endif
  	}

	

 	gil::png_write_view("mandelbrot.png", const_view(img));
}


/* 
 MPI_Init(NULL, NULL);

  MPI_Datatype dt_point;
  MPI_Type_contiguous(3, MPI_INT, &dt_point);
  MPI_Type_commit(&dt_point);
   int world_rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
   int world_size;
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   int N = ceil(float(height)/float(world_size)); //world = number of processors , this is to find num of rows per processor

   // Create a random array of elements on the root process. Its total
   // size will be the number of elements per process times the number
   // of processes

  int num_elements_per_proc = N*width;


   Color_Point *all_rows = NULL;
   if (world_rank == 0) {
     all_rows = (Color_Point *)malloc(sizeof(Color_Point) * num_elements_per_proc*world_size);
   }



   Color_Point *row_blocks = (Color_Point *)malloc(sizeof(Color_Point) * num_elements_per_proc);
   assert(row_blocks!=NULL);
   compute_row(world_rank,width,row_blocks,N,minX,minY,it,jt);
   // Compute the average of your subset


   // Gather all partial averages down to the root process
   //float *sub_avgs = NULL;



   MPI_Gather(&row_blocks, num_elements_per_proc,dt_point , all_rows, num_elements_per_proc, dt_point, 0, MPI_COMM_WORLD);

   // Now that we have all of the partial averages on the root, compute the
   // total average of all numbers. Since we are assuming each process computed
   // an average across an equal amount of elements, this computation will
   // produce the correct answer.
   if (world_rank == 0) {
  	gil::rgb8_image_t img(height, width);
  	auto img_view = gil::view(img);
     //write to image_file
     for(int i=0;i<height;i++)
     {
       for(int j=0;j<width;j++)
       {
         int r =  all_rows[i*width+j].r;
         int g =  all_rows[i*width+j].g;
         int b =  all_rows[i*width+j].b;

         img_view(j, i) = gil::rgb8_pixel_t(r,g,b);
       }
     }
     gil::png_write_view("mandelbrot.png", const_view(img));
   }

   // Clean up
   if (world_rank == 0) {
     free(all_rows);
     //free(sub_avgs);
   }
   free(row_blocks);

   MPI_Barrier(MPI_COMM_WORLD);
   MPI_Finalize();

}
*/
/* eof */
