/**
 * Helper functions for matrix multiplication.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "mat.h"

/**
 * Creates an n x m matrix with random double values.
 * 
 * @param n : the number of rows.
 * @param m : the number of columns.
 * @return a dynamically-allocated array of doubles with size n * m representing the matrix.
 */
double *gen_matrix(int n, int m) {
    double *a = malloc(sizeof(double) * n * m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            a[i*m + j] = (double)rand() / RAND_MAX;
        }
    }
    return a;
}

/**
 * Compares two matrices to see if they are equal.
 * 
 * @param a : a matrix of size nRows * nCols
 * @param b : a matrix of size nRows * nCols
 * @param nRows : the number of rows of both matrices.
 * @param nCols : the number of columns of both matrices.
 * @return 1 if the matrices' values match or 0 otherwise.
 */
int compare_matrices(double *a, double *b, int nRows, int nCols) {
	int n = nRows * nCols;
	for (int k = 0; k < n; ++k) {
		if (fabs(a[k]-b[k]) / fabs(a[k]) > 1e-12) {
			int i = k / nCols;
			int j = k % nCols;
            printf("a[%d][%d] == %.12g\n", i, j, a[k]);
            printf("b[%d][%d] == %.12g\n", i, j, b[k]);
            printf("delta == %.12g\n", fabs(a[k]-b[k]));
            printf("relerr == %.12g\n", fabs(a[k]-b[k]) / fabs(a[k]));
			return 0;
		}
	}
	puts("Matrices are the same");
    return 1;
}

/**
 * Reads a matrix from a file and puts it into an array.
 * 
 * @param path : the path of the file.
 * @return a dynamically-allocated array of doubles representing the matrix.
 */
double *read_matrix_from_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        fprintf(stderr, "Unable to open file at path '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    size_t buflen = 255;
    char buf[buflen];
    int n, m;

    // Read size of matrix
    if(!fgets(buf, buflen, fp)) {
        fprintf(stderr, "Unable to read file at path '%s'\n", path);
        exit(EXIT_FAILURE);
    } else if(sscanf(buf, "%d %d", &n, &m) != 2) {
        fprintf(stderr, "Unable to parse first line, expect '%%d %%d'.\n");
        exit(EXIT_FAILURE);
    }

    double *matrix = malloc(n * m * sizeof(double));

    for(size_t i = 0; i < n; ++i) {
        if(!fgets(buf, buflen, fp)) {
            fprintf(stderr, "Unable to read file at line %lu.\n", i + 2);
            exit(EXIT_FAILURE);
        }

        char *nums = strtok(buf, " ");
        int j = 0;

        do {
            matrix[n * i + j] = atof(nums);
            j++;
        } while(nums = strtok(NULL, " "));
    }

    fclose(fp);

    return matrix;
}

/**
 * Prints the matrix for pretty viewing.
 * 
 * @param a : the matrix to be printed.
 * @param nrows : the number of rows in the matrix.
 * @param ncols : the number of columns in the matrix.
 */
void print_matrix(double *a, int nrows, int ncols) {
     for(int i = 0; i < ncols; i++) {
        for(int j = 0; j < nrows; j++) {
            printf("%5lf ", a[ncols * i + j]);
        }
        puts("");
    }
}

/**
 * Calculates the time between two times.
 * 
 * @param start : the beginning time.
 * @param end : the end time.
 * @return the time between them in seconds.
 */
double deltaTime(struct timespec *start, struct timespec *end) {
    double delta = (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
    return delta;
}