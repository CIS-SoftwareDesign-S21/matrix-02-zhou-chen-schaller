#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>

#include "mat.h"

int main(void)
{
	struct timespec start;
    struct timespec end;
	double *a, *b, *c;
	FILE *fp;
	
	int run = 1;
	int selection;
	
	while(run)
	{
		int n = 100;
		double time = 0;
		
		printf("\nPlease select a matrix multiplication option: \n"
				"1 - unoptimized\n"
				"2 - SIMD\n"
				"3 - SIMD w/ -O3\n"
				"4 - openMP\n"
				"-1 - End the program\n"
				"> ");
		if(scanf("%d", &selection) != 1)
		{
			printf("Error: failed to read integer.\n");
			exit(0);
		}
		else
		{
			puts("");
			
			if(selection == 1)
			{
				fp = fopen("mmult.txt", "w");
				fclose(fp);
				
				for(; n <= 1000; n += 100)
				{
					fp = fopen("mmult.txt", "a");
				
					// gen_matrix returns an n*n matrix
					a = gen_matrix (n, n);
					b = gen_matrix (n, n);
					// allocate memory for the result matrix
					c = malloc(sizeof(double) * n * n);
					// start timer
					clock_gettime(CLOCK_REALTIME , &start);
					// call mmult - this will multiply a*b and put the result in c
					mmult(c, a, n, n, b, n, n);
					// end timer
					clock_gettime(CLOCK_REALTIME , &end);
					// calculate and print the time it took to calculate a*b
					time = deltaTime(&start, &end);
					printf("%d %f\n", n, time);
					
					fprintf(fp, "%d %f\n", n, time);
					fclose(fp);
				}
				
				free(c);
			}
			else if(selection == 2)
			{
				fp = fopen("mmult_simd.txt", "w");
				fclose(fp);
				
				for(; n <= 1000; n += 100)
				{
					fp = fopen("mmult_simd.txt", "a");
				
					// gen_matrix returns an n*n matrix
					a = gen_matrix (n, n);
					b = gen_matrix (n, n);
					// allocate memory for the result matrix
					c = malloc(sizeof(double) * n * n);
					// start timer
					clock_gettime(CLOCK_REALTIME , &start);
					// call mmult_simd
					mmult_simd(c, a, n, n, b, n, n);
					// end timer
					clock_gettime(CLOCK_REALTIME , &end);
					// calculate and print the time it took to calculate a*b
					time = deltaTime(&start, &end);
					printf("%d %f\n", n, time);
					
					fprintf(fp, "%d %f\n", n, time);
					fclose(fp);
				}
				
				free(c);
			}
			else if(selection == 3)
			{
				fp = fopen("mmult_simd_O3.txt", "w");
				fclose(fp);
				
				for(; n <= 1000; n += 100)
				{
					fp = fopen("mmult_simd_O3.txt", "a");
				
					// gen_matrix returns an n*n matrix
					a = gen_matrix (n, n);
					b = gen_matrix (n, n);
					// allocate memory for the result matrix
					c = malloc(sizeof(double) * n * n);
					// start timer
					clock_gettime(CLOCK_REALTIME , &start);
					// call mmult_simd_O3
					mmult_simd_O3(c, a, n, n, b, n, n);
					// end timer
					clock_gettime(CLOCK_REALTIME , &end);
					// calculate and print the time it took to calculate a*b
					time = deltaTime(&start, &end);
					printf("%d %f\n", n, time);
					
					fprintf(fp, "%d %f\n", n, time);
					fclose(fp);
				}
				
				free(c);
			}
			else if(selection == 4)
			{
				fp = fopen("mmult_openMP.txt", "w");
				fclose(fp);
				
				for(; n <= 1000; n += 100)
				{
					fp = fopen("mmult_openMP.txt", "a");
					
					// gen_matrix returns an n*n matrix
					a = gen_matrix (n, n);
					b = gen_matrix (n, n);
					// allocate memory for the result matrix
					c = malloc(sizeof(double) * n * n);
					// start timer
					clock_gettime (CLOCK_REALTIME , &start);
					// call mmult_omp
					mmult_omp(c, a, n, n, b, n, n);
					// end timer
					clock_gettime(CLOCK_REALTIME , &end);
					// calculate and print the time it took to calculate a*b
					time = deltaTime(&start, &end);
					printf("%d %f\n", n, time);
					
					fprintf(fp, "%d %f\n", n, time);
					fclose(fp);
				}
				
				free(c);
			}
			else if(selection == -1)
			{
				run = 0;
				printf("Ending program.\n");
			}
			else
			{
				printf("Please select a valid choice!\n");
			}
		}
	}
}