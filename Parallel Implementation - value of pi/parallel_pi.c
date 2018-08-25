//Here changing the loops with variable as N i.e. the input size, we get the performance curve.
#include "omp.h"
#include "stdio.h"

static long num_steps = 100000000;
int num_threads = 1;


void main()
{
	
	int j;
	for(; num_threads<=4 ;num_threads++) {
		double pi = 0;
		omp_set_num_threads(num_threads);
		double step = 1.0/(double) num_steps;
		double t = omp_get_wtime();
		#pragma omp parallel reduction(+:pi)
		{
			int i; double x, sum = 0;
			
			#pragma omp for
			for( i=0 ; i<=num_steps; i++)
			{
				x = (i+0.5)*step;
				sum = sum+ 4.0/(1.0 + x*x);
			}
	
			pi = pi + step*sum;
		}
		double t1 = omp_get_wtime()-t;	
		double flops = (6.0*(double)num_steps)/(t1*1000000);
		//printf("%f\n, flops");
		printf("%f\n", t1);
	}
}
