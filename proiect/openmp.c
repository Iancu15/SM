#include "utils.h"
#include <omp.h>

void calculate_inverse(float **matrix, int size) {
    int i, j, k;
    float ratio;

    #pragma omp parallel private(i, j, k, ratio) shared(matrix, size)
    {
        // Augment matrix
        #pragma omp for
        for (i = 0; i < size; i++) {
            matrix[i][i + size] = 1;
        }

        // Gauss Jordan
        for (i = 0; i < size; i++) {
            if (matrix[i][i] == 0.0) {
                printf("Can't divide by 0!\n");
                exit(EXIT_FAILURE);
            }

            #pragma omp for
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
        #pragma omp for
        for (i = 0; i < size; i++) {
            for (j = size; j < 2 * size; j++) {
                matrix[i][j] /= matrix[i][i];
            }
        }
    }
}

void main(int argc, char *argv[]) {
    double time;
    float **matrix;
    int size, number_of_threads;

    if (argc != 2) {
        printf("There should be 1 argument! Format is: ./openmp number_of_threads\n");
        exit(EXIT_FAILURE);
    }

    number_of_threads = atoi(argv[1]);
    if (number_of_threads < 0) {
        exit(EXIT_FAILURE);
    }

    omp_set_num_threads(number_of_threads);
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