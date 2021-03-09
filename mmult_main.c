#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>

#include "mat.h"

int main(int argc, char *argv[])
{
	struct timespec start;
    struct timespec end;
	double *a, *b, *c; //*c1, *c2;
    int n;
	double time;
	if(argc > 1)
	{
		// n = size of the square matrix
		n = atoi(argv[1]);
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
		printf("%d %f", n, time);
	}
	else
	{
		fprintf(stderr, "Usage %s <n>\n", argv[0]);
	}
}