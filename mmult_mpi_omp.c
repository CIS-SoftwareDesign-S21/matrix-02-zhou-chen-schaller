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
#define min(x, y) ((x)<(y)?(x):(y))

#include "mat.h"

int main(int argc, char* argv[])
{
    int nrows_1, ncols_1, nrows_2, ncols_2;
    double *aa;	/* the A matrix */
    double *bb;	/* the B matrix */
    double *cc1;	/* A x B computed using the omp-mpi code you write */
    double *cc2;	/* A x B computed using the conventional algorithm */
    int myid, numprocs;
    double starttime, endtime;
    MPI_Status status;

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
		if(fp == NULL) {
			fprintf(stderr, "Unable to open file '%s'\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		// Read size of matrix 1
		if(!fgets(buf, buflen, fp)) {
			fprintf(stderr, "Unable to read file '%s'\n", argv[1]);
			exit(EXIT_FAILURE);
		} else if(sscanf(buf, "%d %d", &nrows_1, &ncols_1) != 2) {
			fprintf(stderr, "Unable to parse first line, expect '%%d %%d'.\n");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
		
		
		// 2nd matrix
		fp = fopen(argv[2], "r");
		if(fp == NULL) {
			fprintf(stderr, "Unable to open file '%s'\n", argv[2]);
			exit(EXIT_FAILURE);
		}
		// Read size of matrix 2
		if(!fgets(buf, buflen, fp)) {
			fprintf(stderr, "Unable to read file '%s'\n", argv[2]);
			exit(EXIT_FAILURE);
		} else if(sscanf(buf, "%d %d", &nrows_2, &ncols_2) != 2) {
			fprintf(stderr, "Unable to parse first line, expect '%%d %%d'.\n");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
		
        
        if (myid == 0) 
		{
            // Master Code goes here
			aa = read_matrix_from_file(argv[1]);
			bb = read_matrix_from_file(argv[2]);
            cc1 = malloc(sizeof(double) * nrows_1 * ncols_2);
            starttime = MPI_Wtime();
            /* Insert your master code here to store the product into cc1 */
            endtime = MPI_Wtime();
            printf("%f\n",(endtime - starttime));
            cc2  = malloc(sizeof(double) * nrows_1 * ncols_2);
            mmult(cc2, aa, nrows_1, ncols_1, bb, nrows_2, ncols_2);
            if(compare_matrices(cc2, cc1, nrows_1, ncols_2))
			{
				fp = fopen("")
			}
        }
		else 
		{
            // Slave Code goes here
        }
    } else {
        fprintf(stderr, "Usage mmult_mpi_omp <file> <file>\n");
    }
    MPI_Finalize();
    return 0;
}
