#include "utils.h"

void calculate_inverse(float **matrix, int size) {
    int i, j, k;
    float ratio;

    // Augment matrix
    for (i = 0; i < size; i++) {
        matrix[i][i + size] = 1;
    }

    // Gauss Jordan
    for (i = 0; i < size; i++) {
        if (matrix[i][i] == 0.0) {
            printf("Mathematical Error!");
            exit(EXIT_FAILURE);
        }


        for (k = 0; k < size; k++) {
            if (i != k) {
                ratio = matrix[k][i] / matrix[i][i];
                for (j = 0; j < size * 2; j++) {
                    matrix[k][j] -= ratio * matrix[i][j];
                }
            }
        }
    }

    // Scale down solution
    for (i = 0; i < size; i++) {
        for (j = size; j < 2 * size; j++) {
            matrix[i][j] /= matrix[i][i];
        }
    }
}

void main() {
    double time;
    float **matrix;
    int size;

    Input *input = read_input();
    matrix = input->matrix;
    size = input->size;

    time = get_time();
    calculate_inverse(matrix, size);
    printf("time taken %.6lf\n", get_time() - time);
    
    //display_solution(matrix, size);
    free_matrix(matrix, size);
    free(input);
}