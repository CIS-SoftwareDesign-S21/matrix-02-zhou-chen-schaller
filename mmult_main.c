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
	
	int run1 = 1;
	int choice;
	
	while(run1)
	{
		printf("\nDo you want to clear all mmult text files (if they exist)?\n"
				"1 - yes\n"
				"2 - no\n"
				"> ");
		if(scanf("%d", &choice) != 1)
		{
			printf("Error: failed to read integer.\n");
			exit(0);
		}
		else
		{
			if(choice == 1)
			{
				fp = fopen("mmult.txt", "w");
				fclose(fp);
				
				fp = fopen("mmult_simd.txt", "w");
				fclose(fp);
				
				fp = fopen("mmult_simd_O3.txt", "w");
				fclose(fp);
				
				fp = fopen("mmult_openMP.txt", "w");
				fclose(fp);
				
				printf("Cleared!\n");
				run1 = 0;
			}
			else if(choice == 2)
			{
				run1 = 0;
				//break;
			}
			else
			{
				printf("Please select a valid choice!\n");
			}
		}
	}
	
	int run2 = 1;
	int selection;
	
	while(run2)
	{
		int n = 100;
		double time = 0;
		
		printf("\nPlease select a matrix multiplication option: \n"
				"1 - unoptimized (PLEASE ENSURE PROGRAM IS COMPILED W/O -O3 FLAG)\n"
				"2 - SIMD (PLEASE ENSURE PROGRAM IS COMPILED W/O -O3 FLAG)\n"
				"3 - SIMD w/ -O3 (PLEASE ENSURE PROGRAM IS COMPILED W/ -O3 FLAG)\n"
				"4 - openMP (PLEASE ENSURE PROGRAM IS COMPILED W/O -O3 FLAG)\n"
				"-1 - End the program\n"
				"> ");
		if(scanf("%d", &selection) != 1)
		{
			printf("Error: failed to read integer.\n");
			exit(0);
		}
		else
		{
			if(selection == 1)
			{
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
				run2 = 0;
				printf("Ending program.\n");
				//break;
			}
			else
			{
				printf("Please select a valid choice!\n");
			}
		}
	}
}