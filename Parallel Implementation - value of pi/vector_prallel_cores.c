#include "stdio.h"
#include "omp.h"
#include "stdlib.h"

int n = 10000000;
int num_t=4;

int main()
{
	int j;
	for( j=1; j<=num_t; j++)
	{
		int* a =(int*)calloc(sizeof(int), n);
		int* b = (int*)calloc(sizeof(int), n);
		int* c = (int*)calloc(sizeof(int), n);
		
		omp_set_num_threads(j);
		int i;
		
		for(i=0; i<n; i++)
		{
			a[i] = 1;
			b[i] = 1;
		}
		
		double t = omp_get_wtime();
		#pragma omp parallel
		{
			int k;
			#pragma omp for
			for(k=0; k<n; k++)
			{
				c[k] = a[k] + b[k];
			}
		}
		double t1 = omp_get_wtime() - t;
		
		double flops = (double) n/(t1*1000000);
		
		//printf("%d\n", n);
		//printf("%f\n", flops);
		printf("%f\n" , t1);
	}
	
	return 0;
}
