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
    int nrows, ncols;

	double *aa;	 /* the A matrix */
	double *bb;	 /* the B matrix */
	double *cc;
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
        nrows = atoi(argv[1]);
        ncols = nrows;
		aa = (double *)malloc(sizeof(double) * nrows * ncols);
		bb = (double *)malloc(sizeof(double) * nrows * ncols);
        aa = gen_matrix(nrows, ncols); 
        bb = gen_matrix(nrows, ncols); 
        cc = (double *)malloc(sizeof(double) * nrows * ncols);
        buffer = (double *)malloc(sizeof(double) * ncols);
        ans = (double *)malloc(sizeof(double) * ncols);
        master = 0;

		if (myid == master)
		{
			/* Insert your master code here to store the product into cc */
			starttime = MPI_Wtime();
			numsent = 0;
			MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
			//MPI_Bcast(aa, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD); // don't need to bcast?
			for (i = 0; i < min(numprocs - 1, nrows); i++)
			{
				for (j = 0; j < ncols; j++)
				{
					buffer[j] = aa[i * ncols + j];
				}
				MPI_Send(buffer, ncols, MPI_DOUBLE, i + 1, i + 1, MPI_COMM_WORLD);
				numsent++;
			}
			for (i = 0; i < nrows; i++)
			{
				MPI_Recv(ans, nrows, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
						 MPI_COMM_WORLD, &status);
				sender = status.MPI_SOURCE;
				anstype = status.MPI_TAG;
				for (int k = 0; k < ncols; k++)
				{
					int m = (anstype - 1) * ncols + k;
					cc[m] = ans[k];
				}
				if (numsent < nrows)
				{
					for (j = 0; j < ncols; j++)
					{
						buffer[j] = aa[numsent * ncols + j];
					}
					MPI_Send(aa, ncols, MPI_DOUBLE, sender, numsent + 1,
							 MPI_COMM_WORLD);
					numsent++;
				}
				else
				{
					MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
				}
			}
			endtime = MPI_Wtime();
			/* Insert your master code here to store the product into cc */
            FILE* fp;
			fp = fopen("mmult_mpi_omp_rand.txt", "a+");
			fprintf(fp, "%d %f\n", atoi(argv[1]), (endtime - starttime));
			fclose(fp);
			printf("\nsize: %d x %d\n", nrows, ncols);
		}
		else
		{
			// Slave Code goes here
			MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
			//MPI_Bcast(aa, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD); // don't need to bcast aa?
			if (myid <= nrows)
			{
				while (1)
				{
					MPI_Recv(buffer, ncols, MPI_DOUBLE, master, MPI_ANY_TAG,
							 MPI_COMM_WORLD, &status);
					if (status.MPI_TAG == 0)
					{
						break;
					}
					row = status.MPI_TAG;
#pragma omp parallel
#pragma omp shared(ans) for reduction(+ : ans)
					// initalize result row ans
					for (int i = 0; i < ncols; i++)
					{
						ans[i] = 0.0;
					}
					int i = row - 1;
					// calculate row buffer * matrix bb here and put into row result
					for (int k = 0; k < ncols; k++)
					{
						for (j = 0; j < nrows; j++)
						{
							ans[k] += buffer[j] * bb[j * ncols + k];
						}
					}
					MPI_Send(ans, ncols, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "Usage mmult_mpi_omp_rand <size>\n");
	}
	MPI_Finalize();
	return 0;
}
