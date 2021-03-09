#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>

#include "mat.h"


int main(void)
{
	struct timespec start;
    struct timespec end;
	double *a, *b, *c; //*c1, *c2;
    int n = 500;
	double time = 0;
	FILE *fp;
	
	fp = fopen("mmmult.txt", "w");
	fclose(fp);
	
	fp = fopen("mmmult.txt", "a");
	
	for(; n <= 3000; n += 500)
	{
		// gen_matrix returns an n*n matrix
		a = gen_matrix (n, n);
		b = gen_matrix (n, n);
		// allocate memory for the result matrix
		c = malloc(sizeof(double) * n * n);
		// start timer
		clock_gettime (CLOCK_REALTIME , &start);
		// call mmult - this will multiply a*b and put the result in c
		mmult(c, a, n, n, b, n, n);
		// end timer
		clock_gettime (CLOCK_REALTIME , &end);
		// calculate and print the time it took to calculate a*b
		time = deltaTime(&start, &end);
		printf("%d %f\n", n, time);
		
		fprintf(fp, "%d %f\n", n, time);
	}
	
	fclose(fp);
}