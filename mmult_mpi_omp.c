/** 
 * Incomplete program to multiply a matrix times a matrix using both
 * MPI to distribute the computation among nodes and OMP
 * to distribute the computation among threads.
 */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

#include "mat.h"

int main(int argc, char *argv[])
{
	int nrows_1, ncols_1, nrows_2, ncols_2;
	double *aa;	 /* the A matrix */
	double *bb;	 /* the B matrix */
	double *cc1; /* A x B computed using the omp-mpi code you write */
	double *cc2; /* A x B computed using the conventional algorithm */
	double *buffer;
	double *ans;
	int myid, master, numprocs;
	double starttime, endtime;
	MPI_Status status;
	int i, j, numsent, sender;
	int anstype, row;
	

	/* insert other global variables here */
	size_t buflen = 255;
	char buf[buflen];
	FILE *fp;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if (argc > 1)
	{
		// 1st matrix
		fp = fopen(argv[1], "r");
		if (fp == NULL)
		{
			fprintf(stderr, "Unable to open file '%s'\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		// Read size of matrix 1
		if (!fgets(buf, buflen, fp))
		{
			fprintf(stderr, "Unable to read file '%s'\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		else if (sscanf(buf, "%d %d", &nrows_1, &ncols_1) != 2)
		{
			fprintf(stderr, "Unable to parse first line, expect '%%d %%d'.\n");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
		

		// 2nd matrix
		fp = fopen(argv[2], "r");
		if (fp == NULL)
		{
			fprintf(stderr, "Unable to open file '%s'\n", argv[2]);
			exit(EXIT_FAILURE);
		}
		// Read size of matrix 2
		if (!fgets(buf, buflen, fp))
		{
			fprintf(stderr, "Unable to read file '%s'\n", argv[2]);
			exit(EXIT_FAILURE);
		}
		else if (sscanf(buf, "%d %d", &nrows_2, &ncols_2) != 2)
		{
			fprintf(stderr, "Unable to parse first line, expect '%%d %%d'.\n");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
	
		aa = (double *)malloc(sizeof(double) * nrows_1 * ncols_1);
		bb = (double *)malloc(sizeof(double) * nrows_2 * ncols_2);
		aa = read_matrix_from_file(argv[1]);
		bb = read_matrix_from_file(argv[2]);
		cc1 = malloc(sizeof(double) * nrows_1 * ncols_2);
		cc2 = malloc(sizeof(double) * nrows_1 * ncols_2);
		buffer = (double *)malloc(sizeof(double) * ncols_2);
		ans = (double *)malloc(sizeof(double) * ncols_2);

		if (myid == master)
		{
			/* Insert your master code here to store the product into cc1 */
			starttime = MPI_Wtime();
			numsent = 0;
			MPI_Bcast(bb, ncols_2, MPI_DOUBLE, master, MPI_COMM_WORLD);
			for (i = 0; i < min(numprocs - 1, nrows_1); i++)
			{
				for (j = 0; j < ncols_2; j++)
				{
					buffer[j] = aa[i * ncols_2 + j];
				}
				MPI_Send(buffer, ncols_2, MPI_DOUBLE, i + 1, i + 1, MPI_COMM_WORLD);
				numsent++;
			}
			for (i = 0; i < nrows_1; i++)
			{
				MPI_Recv(ans, nrows_1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
						 MPI_COMM_WORLD, &status);
				sender = status.MPI_SOURCE;
				anstype = status.MPI_TAG;
				for (int k = 0; k < ncols_2; k++)
                {
                    int m = (anstype - 1) * ncols_2 + k;
                    cc1[m] = ans[k];
                }
				if (numsent < nrows_1)
				{
					for (j = 0; j < ncols_2; j++)
					{
						buffer[j] = aa[numsent * ncols_2 + j];
					}
					MPI_Send(buffer, ncols_2, MPI_DOUBLE, sender, numsent + 1,
							 MPI_COMM_WORLD);
					numsent++;
				}
				else
				{
					MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
				}
			}
			endtime = MPI_Wtime();
			/* Insert your master code here to store the product into cc1 */
			
			mmult(cc2, aa, nrows_1, ncols_1, bb, nrows_2, ncols_2);

			print_matrix(cc1, nrows_1, ncols_1);
			printf("\n");
			print_matrix(cc2, nrows_2, ncols_2);

			if (compare_matrices(cc2, cc1, nrows_1, ncols_2))
			{
				fp = fopen("resultant_matrix.txt", "w");
				for (int i = 0; i < ncols_2; i++)
				{
					for (int j = 0; j < nrows_1; j++)
					{
						fprintf(fp, "%5lf ", cc1[ncols_2 * i + j]);
					}
					puts("");
				}
				fclose(fp);
			}
		}
		else
		{
			// Slave Code goes here
			MPI_Bcast(bb, ncols_2, MPI_DOUBLE, master, MPI_COMM_WORLD);
			if (myid <= nrows_1)
			{
				while (1)
				{
					MPI_Recv(buffer, ncols_2, MPI_DOUBLE, master, MPI_ANY_TAG,
							 MPI_COMM_WORLD, &status);
					if (status.MPI_TAG == 0)
					{
						break;
					}
					row = status.MPI_TAG;
					// initalize result row ans
                    for (int i = 0; i < ncols_2; i++)
                    {
                        ans[i] = 0.0;
                    }
#pragma omp parallel
#pragma omp shared(ans) for reduction(+:ans)
					// calculate row buffer * matrix bb here and put into row result
                    for (int k = 0; k < ncols_2; k++)
                    {
                        for (j = 0; j < ncols_2; j++)
                        {
                            ans[k] += buffer[j] * bb[j * ncols_2 + k];
				printf("\nans[%d] = %lf * %lf", i, aa[i*ncols_2 + j], bb[j*ncols_2 + k]);
				printf("\nrow = %d", i);
				printf("\ni*ncols + j = %d, j*ncols + k = %d", i*ncols_2 +j, j*ncols_2 +k);
                        }
                    }
					MPI_Send(ans, ncols_2, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "Usage mmult_mpi_omp <file> <file>\n");
	}
	MPI_Finalize();
	return 0;
}
