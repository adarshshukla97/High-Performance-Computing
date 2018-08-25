//Here changing the loops with variable as N i.e. the input size, we get the performance curve.
#include "omp.h"
#include "stdio.h"

static int N = 1000000000;
int num_threads = 1;
void main()
{
	int j;
	for( ; num_threads<=4; num_threads++){
		double pi=0;
		omp_set_num_threads(num_threads);	
		double t = omp_get_wtime();

		#pragma omp parallel reduction(+:pi)
		{
			int k;
 			#pragma omp for
			for(k=0; k<=N; k++)
			{
				if(k%2==0)
				{
					pi = pi +(1.0/ (double)(2*k + 1));
				}
	
				else
				{
					pi = pi -(1.0/ (double)(2*k + 1));
				}
			}
		}
		double t1 = omp_get_wtime() - t;
		double flops = (4.0*(double)N)/(t1*1000000);
		pi = pi*4.0;
		printf("%f\n", t1);
	
	}
	
}
