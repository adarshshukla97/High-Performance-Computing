#include "omp.h"
#include "stdio.h"

int num_steps = 1000;
double step;

void main()
{
	for( ; num_steps<=1000000000; num_steps*=10)
	{
		double pi = 0, sum = 0, x;
		step = 1.0/(double) num_steps; int i;
	
		double t = omp_get_wtime();
		
		for(i=0 ; i<num_steps; i++)
		{
			x = (i+0.5)*step;
			sum = sum+ 4.0/(1.0 + x*x);
		}

		pi = pi + step*sum;
	
		double t1 = omp_get_wtime()-t;	
		double flops = (6.0*(double)num_steps)/(t1*1000000);
		//printf("%f\n", flops);
		printf("%f\n", t1);
	}
}
