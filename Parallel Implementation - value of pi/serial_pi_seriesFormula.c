#include "omp.h"
#include "stdio.h"

static int N = 1000;

void main()
{
	int k; double pi = 0;
	for( ;N<=1000000000;N=N*10){
		double t = omp_get_wtime();
		for(k=0; k<=N; k++)
		{
			if(k%2==0)
				pi = pi +(1.0/ (double)(2*k + 1));
			else
				pi = pi -(1.0/ (double)(2*k + 1));
			
		}
		double t1 = omp_get_wtime()-t;
		pi = pi*4.0;
		double flops = (4.0* (double)N) /(t1 * 1000000);
		printf("%f\n",t1);
	}
}
