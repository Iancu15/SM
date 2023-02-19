#include <stdio.h>
#include <omp.h>

#define N 8
#define NUM_THREADS 4

void main() {
    omp_set_num_threads(NUM_THREADS);
    double a[N][N], b[N][N], c[N][N], v[N][N][N];
    int i, j, k, m;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i][j] = i * N + j;
            b[i][j] = i * N + j;
            c[i][j] = 0.0;
        }
    }

    #pragma omp parallel private(i, j, k)
    {
        int tid = omp_get_thread_num();
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                for (k = tid; k < N; k += NUM_THREADS) {
                    v[i][j][k] = a[i][k] * b[k][j];
                }
            }
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (m = 1; m < N; m *= 2) {
                #pragma omp parallel private(k)
                {
                    int tid = omp_get_thread_num();
                    for (k = tid; k < N; k += NUM_THREADS) {
                        if (k % (m * 2) == 0) {
                            v[i][j][k] += v[i][j][k + m];
                        }
                    }
                }
            }

            c[i][j] = v[i][j][0];
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%.1f ", c[i][j]);
        }

        printf("\n");
    }
}