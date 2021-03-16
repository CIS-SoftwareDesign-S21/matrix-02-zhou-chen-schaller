/** 
 * Program to multiply a matrix times a vector using
 * MPI to distribute the computation among nodes.
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mat.h"
#define min(x, y) ((x) < (y) ? (x) : (y))

int main(int argc, char *argv[])
{
    int nrows, ncols;

    double *aa, *bb, *c;
    double *buffer;
    double *ans;
    double *times;
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
        bb = (double *)malloc(sizeof(double) * nrows * ncols); // refactor to become full matrix
        c = (double *)malloc(sizeof(double) * nrows);
        buffer = (double *)malloc(sizeof(double) * ncols);
        ans = (double *)malloc(sizeof(double) * ncols); // allocate space for buffer row
        master = 0;
        if (myid == master)
        {
            // Master Code goes here
            aa = gen_matrix(nrows, ncols);
            bb = gen_matrix(nrows, ncols); // added this to randomly generate bb as matrix
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
                anstype = status.MPI_TAG; // number of row passed to slave code
                // putting buffer row into c
                for (int k = 0; k < ncols; k++) {
                    int m = (anstype - 1) * ncols + k;
                    c[m] = ans[k];
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
            printf("%f\n", (endtime - starttime));
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
                    // initalize result row ans
                    for (int i = 0; i < ncols; i++) {
                        ans[i] = 0.0;
                    }
                    // calculate row buffer * matrix bb here and put into row result
                    for (int k = 0; k < ncols; k++) {
                        for (j = 0; j < ncols; j++) {
                            ans[k] += buffer[j] * bb[j * ncols + k];
                        }
                    }
                    // send the row result back to master
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
