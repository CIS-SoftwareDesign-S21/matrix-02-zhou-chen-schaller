/** 
 * Program to multiply a matrix times a vector using
 * MPI to distribute the computation among nodes.
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mat.h"
#include <string.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

int main(int argc, char *argv[])
{
    int nrows, ncols;

    double *aa, *bb, *c;
    double *ans;
    double *times;
    double *buffer;
    double total_times;

    int run_index;
    int nruns;
    int myid, master, numprocs;

    double starttime, endtime;
    MPI_Status status;
    int i, j, numsent, sender;
    int anstype, row;

    srand(time(0));

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
        c = (double *)malloc(sizeof(double) * nrows * ncols);
        buffer = (double *)malloc(sizeof(double) * ncols);
        ans = (double *)malloc(sizeof(double) * ncols);
        master = 0;
        if (myid == master)
        {
            // Master Code goes here
            starttime = MPI_Wtime();
            numsent = 0;
            MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
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
                    int in = (anstype - 1) * ncols + k;
                    c[in] = ans[k];
                }
                if (numsent < nrows)
                {
                    for (j = 0; j < ncols; j++)
                    {
                        buffer[j] = aa[numsent * ncols + j];
                    }
                    MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent + 1,
                             MPI_COMM_WORLD);
                    numsent++;
                }
                else
                {
                    MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
                }
            }
            endtime = MPI_Wtime();
			double finaltime = endtime - starttime;
            FILE* fp;
			fp = fopen("mmult_mpi.txt", "a+");
			fprintf(fp, "%d %lf\n", atoi(argv[1]), finaltime);
			fclose(fp);
			printf("\nsize: %d x %d\n", nrows, ncols);
        }
        else
        {
            // Slave Code goes here
            MPI_Bcast(bb, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
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
                    for (int i = 0; i < ncols; i++)
                    {
                        ans[i] = 0.0;
                    }
                    for (int k = 0; k < ncols; k++)
                    {
                        for (j = 0; j < ncols; j++)
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
        fprintf(stderr, "Usage matrix_times_vector <size>\n");
    }
    MPI_Finalize();
    return 0;
}
