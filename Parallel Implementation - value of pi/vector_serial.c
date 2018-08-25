#include "stdio.h"
#include "omp.h"
#include "stdlib.h"

int n = 1000;

int main()
{
	for( ; n<=100000000; n*=10)
	{
		int* a = (int*)calloc(sizeof(int), n);
		int* b = (int*)calloc(sizeof(int), n);
		int* c = (int*)calloc(sizeof(int), n);;
		int i;
	
		for(i=0; i<n; i++)
		{
			a[i] = 1;
			b[i] = 1;
		}
	
		double t = omp_get_wtime();
		
		for(i=0; i<n; i++)
		{
			c[i] = a[i] + b[i];
		}
		
		double t1 = omp_get_wtime() - t;
		
		double flops = (double)n/(t1*1000000);
		printf("%f\n", flops);
		//printf("%d\n", n);
		//p/rintf("%f\n", t1);
	}
	return 0;
}
