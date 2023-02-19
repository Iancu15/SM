#include <stdio.h>
#include <omp.h>

#define N 8
#define NUM_THREADS 4

void main() {
    omp_set_num_threads(NUM_THREADS);
    double a[N][N], b[N][N], c[N][N];
    int i, j, k;
    double sum;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i][j] = i * N + j;
            b[i][j] = i * N + j;
            c[i][j] = 0.0;
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            sum = 0.0;
            #pragma omp parallel private(k) reduction(+:sum)
            {
                int tid = omp_get_thread_num();
                for (k = tid; k < N; k += NUM_THREADS) {
                    sum += a[i][k] * b[k][j];
                }
            }

            c[i][j] = sum;
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%.1f ", c[i][j]);
        }

        printf("\n");
    }
}