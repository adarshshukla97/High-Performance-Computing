
#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))
//  Using the MONOTONIC clock 
#define CLK CLOCK_MONOTONIC

/* Function to compute the difference between two points in time */
struct timespec diff(struct timespec start, struct timespec end);

/* 
   Function to computes the difference between two time instances
 */
struct timespec diff(struct timespec start, struct timespec end){
	struct timespec temp;
	if((end.tv_nsec-start.tv_nsec)<0){
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	}
	else{
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

typedef struct {
  unsigned char red,green,blue;
} PPMPixel;

typedef struct {
  int x, y;
  PPMPixel *data;
} PPMImage;

typedef struct {
  unsigned char gs;
} PPMPixelGS;

typedef struct {
  int x, y;
  PPMPixelGS *data;
} PPMImageGS;


#define RGB_COMPONENT_COLOR 255
double parallel_time;


struct timespec start_e2e, end_e2e, start_alg, end_alg, e2e, alg;


void writePPMGS(const char *filename, PPMImageGS *img)
{
  FILE *fp;
  //open file for output
  fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  //write the header file
  //image format
  fprintf(fp, "P5\n");
  
  //image size
  fprintf(fp, "%d %d\n",img->x,img->y);

  // rgb component depth
  fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

  // pixel data
  fwrite(img->data, img->x, img->y, fp);
  fclose(fp);
}



PPMImageGS * changeImage(PPMImage * im)
{
  int rows = im->x;
  int cols = im->y;
  int i,j;

  PPMImageGS *filtered_image = (PPMImageGS *) malloc(sizeof(PPMImageGS));
  filtered_image->x = rows;
  filtered_image->y = cols;
  filtered_image->data = (PPMPixelGS *) malloc(rows*cols*sizeof(PPMPixelGS));
  int r,g,b,idx;	
  for(i=0;i<rows;i++)
    {
      for(j=0; j<cols; j++)
		{
		  idx = rows*i + j;
		  PPMPixel *temp = im->data + idx;
		  r = temp->red;
		  g = temp->green;
		  b = temp->blue;
		  PPMPixelGS *temp2 = filtered_image->data + idx;
		  temp2->gs = 0.21*r+0.71*g+0.07*b;
		}	
    }
	
  return filtered_image;
}

static PPMImage *readPPM(const char *filename)
{
  char buff[16];
  PPMImage *img;
  FILE *fp;
  int c, rgb_comp_color;
  //open PPM file for reading
  fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  //read image format
  if (!fgets(buff, sizeof(buff), fp)) {
    perror(filename);
    exit(1);
  }

  //check the image format
  if (buff[0] != 'P' || buff[1] != '6') {
    fprintf(stderr, "Invalid image format (must be 'P6')\n");
    exit(1);
  }

  //alloc memory form image
  img = (PPMImage *)malloc(sizeof(PPMImage));
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  //check for comments
  c = getc(fp);
  while (c == '#') {
    while (getc(fp) != '\n') ;
    c = getc(fp);
  }

  ungetc(c, fp);
  //read image size information
  if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
    exit(1);
  }

  //read rgb component
  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
    fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
    exit(1);
  }

  //check rgb component depth
  if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
    fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
    exit(1);
  }

  while (fgetc(fp) != '\n') ;
  //memory allocation for pixel data
  img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  //read pixel data from file
  if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
    fprintf(stderr, "Error loading image '%s'\n", filename);
    exit(1);
  }

  fclose(fp);
  return img;
}

void writePPM(const char *filename, PPMImage *img)
{
  FILE *fp;
  //open file for output
  fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  //write the header file
  //image format
  fprintf(fp, "P6\n");

  //comments


  //image size
  fprintf(fp, "%d %d\n",img->x,img->y);

  // rgb component depth
  fprintf(fp, "%d\n",255);

  // pixel data
  fwrite(img->data, 3 * img->x, img->y, fp);
  fclose(fp);
}

/*
	function to perform partition of array for quick sort
*/	
int partition(double *arr, int left, int right) {
	double pivot = arr[right];
	int i = left-1;
	double temp;
	int j;
	
	for(j = left; j < right; j++) {
		if(arr[j] <= pivot) {
			i++;
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	}
	
	temp = arr[i+1];
	arr[i+1] = pivot;
	arr[right] = temp;
	
	return i+1;
}

/*
	quick sort
*/
void quick_sort(double *arr, int left, int right) {
	if(right <= left) {
		return;
	}		
	int index = partition(arr, left, right);
	quick_sort(arr, left, index-1);
	quick_sort(arr, index+1, right);
}

PPMPixel bilinearinterpolation(PPMImage* image, int rows, int columns, double x, double y) 
{

	// the values of x and y to interpolate on		
	int xf= floor(x);
	int yf= floor(y);

	int xc= ceil(x);
	int yc=	ceil(y);

	//changes made to return pixel instead of pointer to it
	PPMPixel *fq11, *fq21, *fq12, *fq22; // *f;
	PPMPixel pixtoreturn,fxy1, fxy2;
	
	double xfactor1, xfactor2, yfactor1, yfactor2;

	// constants for first interpolation
	xfactor1 = (xc-x)/(xc-xf);
	xfactor2 = (x-xf)/(xc-xf);

	// accessing data for first interpolation
	fq11 = image->data + xf*columns + yf;
	fq21 = image->data + xf*columns + yc;
	fq12 = image->data + xc*columns + yf;
	fq22 = image->data + xc*columns + yc;

	// first interpolation
/*	Function to perform image warping for given input image
	takes in input as ppmimage
	warps and returns a ppm
*/
	fxy1.red = xfactor1 * fq11->red + xfactor2 * fq21->red;
	fxy1.green = xfactor1 * fq11->green + xfactor2 * fq21->green;
	fxy1.blue = xfactor1 * fq11->blue + xfactor2 * fq21->blue;
	
	fxy2.red = xfactor1 * fq12->red + xfactor2 * fq22->red;
	fxy2.green = xfactor1 * fq12->green + xfactor2 * fq22->green;
	fxy2.blue = xfactor1 * fq12->blue + xfactor2 * fq22->blue;

	// constants for second interpolation
	yfactor1 = (yc-y)/(yc-yf);
	yfactor2 = (y-yf)/(yc-yf);
	 
	//second interpolation
	pixtoreturn.red= yfactor1 * fxy1.red + yfactor2 * fxy2.red;
	pixtoreturn.green = yfactor1 * fxy1.green + yfactor2 * fxy2.green;
	pixtoreturn.blue = yfactor1 * fxy1.blue + yfactor2 * fxy2.blue; 
	 
	return pixtoreturn;
}




/* 
	function to perform the median filtering of given image
	input: image and its half_width
	output: filtered image
*/
//--------------------------PPMImage *filter(PPMImage *im, int hw)
//{
	
//	return filtered_image;
//}


int main(int argc, char* argv[])
{
	
	/* Should start before anything else */
	clock_gettime(CLK, &start_e2e);

	/* Check if enough command-line arguments are taken in. */
	//if(argc < 3){
	//	printf( "Usage: %s n p \n", argv[0] );
	//	return -1;
	//}

	int n=atoi(argv[1]);	/* size of input array */
	int p=atoi(argv[2]);	/* number of processors*/
	char *problem_name = "image_warping";
	char *approach_name = "data_division";
//	char buffer[10];
//	FILE* inputFile;
	FILE* outputFile;
	//	inputFile = fopen(argv[3],"r");

	char outputFileName[50];		
	sprintf(outputFileName,"output/%s_%s_%s_%s_output.txt",problem_name,approach_name,argv[1],argv[2]);

	//***************
	/*int num_step=atof(argv[1]);	
	int **a= (int**)malloc(sizeof(int *)*num_step);
	int **b= (int**)malloc(sizeof(int *)*num_step);
	int **c= (int**)malloc(sizeof(int *)*num_step);
	int l;
	for(l=0;l<num_step;l++)
	{
		a[l] = malloc(sizeof(int)*num_step);
		b[l] = malloc(sizeof(int)*num_step);
		c[l] = malloc(sizeof(int)*num_step);

	}

	int i,j,k,ii,jj,kk;

	for(i=0;i<num_step;i++)
	{
		for(j=0;j<num_step;j++)
		{
			a[i][j]=2;
			b[i][j]=5;
			c[i][j]=0;
		}

	}
	int block_size=(int)sqrt((double)num_step);
	int sum=0;*/
	//***************


	//clock_gettime(CLK, &start_alg);	/* Start the algo timer */

	/*----------------------Core algorithm starts here----------------------------------------------*/
	char filename[30];
	strcpy(filename, argv[1]);
	strcat(filename, ".ppm");

  	PPMImage *im;
  	//clock_t start, end;

	//PPMImage *image;
	double start, end;

	im = readPPM(filename);

	//start=omp_get_wtime();
	// warping 
	//%*********************************************************-----------------------------------------------------------------PPMImage* im = filter (image);
	int i,j, ri, rj, hw = 3;

	// total block/mask/stencil size for given half-width
	
	
	int rows = im->x;
	int columns = im->y;
	
	//image 2 declaration
	PPMImage *out_image = (PPMImage*) malloc(sizeof(PPMImage));
  	out_image->x = rows;
  	out_image->y = columns;
  	out_image->data = (PPMPixel *) malloc(rows*columns*sizeof(PPMPixel));
  	
	// arrays storing the rgb components of all pixels in the stencil
  	int r = 0, g = 0, b = 0;
	float rsd = 0, bsd = 0, gsd = 0;
  	
  	int idx,idx2;

  	PPMPixel *temp;
	
	//double par_start, par_end;
	
	clock_gettime(CLK, &start_alg);	/* Start the algo timer */

	//loop through whole image
	
	for (i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			idx2= columns*i + j;
			
			
			// traversing through the image.
			temp = im->data + idx2;
			r+=temp->red;
			b+=temp->blue;
			g+=temp->green;
		}
	}
	
	//Mean values.
	r = r/(rows*columns);
	b = b/(rows*columns);
	g = g/(rows*columns);
	
	//Finding the standard deviation---------------------------
	for (i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			idx2= columns*i + j;
			
			
			// traversing through the image.
			temp = im->data + idx2;
			rsd+=(temp->red-r)*(temp->red-r);
			bsd+=(temp->blue-b)*(temp->blue-b);
			gsd+=(temp->green-g)*(temp->green-g);
		}
	}
	
	//Mean values.
	rsd = sqrt((float)rsd/(float)(rows*columns));
	bsd = sqrt((float)bsd/((float)rows*columns));
	gsd = sqrt((float)gsd/((float)rows*columns));
	
	//------------------------------------------------------------
	
	//UPDATING THE IMAGE---------------%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (i=0;i<rows;i++)
	{
		for(j=0;j<columns;j++)
		{
			idx2= columns*i + j;
			
			// traversing through the image.
			temp = im->data + idx2;
			(out_image->data + idx2)->red = (char)(128*((abs(temp->red-r))/rsd));
			(out_image->data + idx2)->blue = (char)(128*((abs(temp->blue-b))/bsd));
			(out_image->data + idx2)->green = (char)(128*((abs(temp->green-g))/gsd));
		}
	}
	
	//%%%%%%%%%%%%%%%------------------------------------------------------

	//clock_gettime(CLK, &end_alg);	/* End the algo timer */
	//parallel_time = par_end-par_start;

	//printf("MFLOPS = %f\n", (3*hw*hw + 10)*rows*columns / (1000000 * parallel_time) );

	//end=omp_get_wtime();
	
	//printf("Time: %f\n", (end-start) );

	writePPM("normalised.ppm",out_image);

	/*----------------------Core algorithm finished--------------------------------------------------*/

	clock_gettime(CLK, &end_alg);	/* End the algo timer */
	/* Ensure that only the algorithm is present between these two
	   timers. Further, the whole algorithm should be present. */


	/* Should end before anything else (printing comes later) */
	clock_gettime(CLK, &end_e2e);
	e2e = diff(start_e2e, end_e2e);
	alg = diff(start_alg, end_alg);

	/* problem_name,approach_name,n,p,e2e_sec,e2e_nsec,alg_sec,alg_nsec
	   Change problem_name to whatever problem you've been assigned
	   Change approach_name to whatever approach has been assigned
	   p should be 0 for serial codes!! 
	 */
	printf("%s,%s,%d,%d,%d,%ld,%d,%ld\n", problem_name, approach_name, n, p, e2e.tv_sec, e2e.tv_nsec, alg.tv_sec, alg.tv_nsec);

	return 0;

}

