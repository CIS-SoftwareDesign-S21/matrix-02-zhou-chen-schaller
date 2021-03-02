/**
 * This file compares the time to multiply matrices
 * both unoptimized and with OMP. 
 */

#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>

#include "mat.h"

int main(int argc, char* argv[]) {
    struct timespec start;
    struct timespec end;
    struct timespec res;
    double *a, *b, *c1, *c2;
    int n;
    double times[2];
    if (argc > 1) {
        n = atoi(argv[1]);
        a = gen_matrix(n, n);
        b = gen_matrix(n, n);
        c1 = malloc(sizeof(double) * n * n);
        c2 = malloc(sizeof(double) * n * n);
        clock_gettime(CLOCK_REALTIME, &start);
        mmult(c1, a, n, n, b, n, n);
        clock_gettime(CLOCK_REALTIME, &end);
        times[0] = deltaTime(&start, &end);
        printf("%d %f", n, times[0]);
        clock_gettime(CLOCK_REALTIME, &start);
        mmult_omp(c2, a, n, n, b, n, n);
        clock_gettime(CLOCK_REALTIME, &end);
        times[1] = deltaTime(&start, &end);
        printf(" %f", times[1]);
        printf("\n");
        compare_matrices(c1, c2, n, n);
    } else {
        fprintf(stderr, "Usage %s <n>\n", argv[0]);
    }
}

