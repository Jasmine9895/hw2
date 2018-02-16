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
/*	
	printf("Computing for rank=%d\n",p);
	double y=minY + p*N*it;
  	for (int i = p*N; i < N*(p+1); ++i) 
  	{	
    		double x = minX;
    		for (int j = 0; j < width; ++j) 
		{
      			
      			//img[(i-p*N)*width + j] = render_color(mandelbrot(x,y)/512.0);
			int curr_index= i*width + j;
      			Color_Point t = render_color(mandelbrot(x,y)/512.0);
			img[curr_index].r = t.r;
			img[curr_index].g = t.g;
			img[curr_index].b = t.b;

			#ifdef DEBUG_EXTENDED
			printf("ij=(%d,%d),xy=(%0.1lf,%0.1lf),r=%d r=%d  ",i,j,x,y,t.r,img[(i-p*N)*width +j].r);
			#endif
			x += jt;
    		}
    		y += it;
		
		#ifdef DEBUG
		printf("\n");
		#endif
  	}
	#ifdef DEBUG_EXTENDED
	for(int i=0;i<N;i++)
	{
		for(int j= 0;j<width;j++)
		{
			int curr_index= i*width + j;
			img[curr_index].g = 20;
			printf("CI:%d r:%d ",curr_index,img[curr_index].r);		
		}
		printf("\n");
	}
	#endif 
*/

img[0].r = p;
img[0].g = 7*p;
img[0].b = 9*p; 

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
/*  
  int Num_Processors = 3;
  int N = ceil(float(height)/float(Num_Processors));
  printf("N = %d\n",N);

  vector<Color_Point*> all_row_blocks;
  for(int p=0;p<Num_Processors;p++)
  {

	#ifdef DEBUG
	printf("p=%d  y=%0.1lf\n",p,y);
	#endif
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
*/

 
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

  printf("Num_elem_per_proc = %d, N =%d, width = %d",num_elements_per_proc,N,width);
   Color_Point *all_rows = NULL;
   if (world_rank == 0) {
     all_rows = (Color_Point *)malloc(sizeof(Color_Point) * num_elements_per_proc*world_size);
	printf("World size: %d\n",world_size);   
 }



   Color_Point *row_blocks = (Color_Point *)malloc(sizeof(Color_Point) * num_elements_per_proc);
   assert(row_blocks!=NULL);
	printf("Computing values for all rows\n");  
 compute_row(world_rank,width,row_blocks,N,minX,minY,it,jt);
   // Compute the average of your subset


   // Gather all partial averages down to the root process
   //float *sub_avgs = NULL;

  printf("------------------\n");
  printf("my_rank: %d\n",world_rank);
	printf("Gathering values\n");
	printf("My values");
	printf("Rank=%d, RGB: %d,%d,%d",world_rank,row_blocks[0].r,row_blocks[0].g,row_blocks[0].b);
	#ifdef DEBUG	
	for(int i=0;i<N;i++)
	{
		for(int j= 0;j<width;j++)
		{
			
			int curr_index= i*width + j;
			printf("r:%d  g:%d   ",row_blocks[curr_index].r,row_blocks[curr_index].g);		
		}
		printf("\n");
	}
	#endif
	
printf("----------------------\n");		

  // MPI_Gather(&row_blocks, num_elements_per_proc,dt_point , all_rows, num_elements_per_proc, dt_point, 0, MPI_COMM_WORLD);
   MPI_Gather(row_blocks, 1,dt_point , all_rows, 1, dt_point, 0, MPI_COMM_WORLD);
printf("Gathering done\n");
   if (world_rank == 0) 
{
	for(int p1=0;p1<world_size;p1++)
	{	
		printf("p:%d RGB=(%d,%d,%d)",p1,all_rows[p1].r,all_rows[p1].g,all_rows[p1].b);
	}
/* 
 	gil::rgb8_image_t img(height, width);
  	auto img_view = gil::view(img);
     write to image_file
     printf("Storing in img\n");

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
*/
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

/* eof */
