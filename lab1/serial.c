#include <stdio.h>

#define N 8

void main() {
    double a[N][N], b[N][N], c[N][N];
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i][j] = i * N + j;
            b[i][j] = i * N + j;
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            c[i][j] = 0.0;
            for (k = 0; k < N; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%.1f ", c[i][j]);
        }

        printf("\n");
    }
}