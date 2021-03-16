#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mat.h"

int mmult_simd_O3(double *c,
               double *a, int aRows, int aCols,
               double *b, int bRows, int bCols)
{
    for (int i = 0; i < aRows; ++i)
    {
        for (int j = 0; j < bCols; ++j)
        {
            c[i * bCols + j] = 0;
        }
        for (int k = 0; k < aRows; ++k)
        {
            for (int l = 0; l < bCols; ++l)
            { 
                c[i * bCols + l] += a[i * aRows + k] * b[k * bCols + l];
            }
        }
    }

    return 0;
}