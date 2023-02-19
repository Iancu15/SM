#include "utils.h"
#include <mpi.h>
#include <omp.h>

#define ROOT 0
#define NUMBER_OF_THREADS_PER_PROC 2

void send_row(float *row, int size, int destination, int tag) {
    MPI_Send(row, size, MPI_FLOAT, destination, tag, MPI_COMM_WORLD);
}

void recv_row(float *row, int size, int source, int tag) {
    MPI_Recv(row, size, MPI_FLOAT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void calculate_inverse_root(float **matrix, int size, int proc) {
    int i, j, k, rank;

    MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    #pragma omp parallel private(i, j, k, rank) shared(matrix, size, proc)
    {

        // Augment matrix
        #pragma omp for
        for (i = 0; i < size; i++) {
            matrix[i][i + size] = 1;
        }

        // Gauss Jordan
        for (i = 0; i < size; i++) {
            if (matrix[i][i] == 0.0) {
                printf("Mathematical Error!");
                exit(EXIT_FAILURE);
            }

            // Send current i row
            #pragma omp for
            for (rank = 1; rank < proc; rank++) {
                send_row(matrix[i], size * 2, rank, 0);
            }

            // Send the k rows to workers
            #pragma omp for
            for (k = 0; k < size; k++) {
                if (i != k) {
                    rank = k % (proc - 1) + 1;
                    send_row(matrix[k], size * 2, rank, k);
                }
            }

            // Receive the modified k rows from workers
            #pragma omp for
            for (k = 0; k < size; k++) {
                if (i != k) {
                    rank = k % (proc - 1) + 1;
                    recv_row(matrix[k], size * 2, rank, k);
                }
            }
        }

        // Send rows to workers
        #pragma omp for
        for (i = 0; i < size; i++) {
            rank = i % (proc - 1) + 1;
            send_row(matrix[i], size * 2, rank, size + i);
        }

        // Get scaled down rows from workers
        #pragma omp for
        for (i = 0; i < size; i++) {
            rank = i % (proc - 1) + 1;
            recv_row(matrix[i], size * 2, rank, size + i);
        }
    }
}

void calculate_inverse_worker(int rank, int proc) {
    int i, j, k, size;
    float ratio;
    float *i_row, *k_row;

    MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    i_row = malloc(sizeof(float) * size * 2);

    #pragma omp parallel private(i, j, k, ratio, k_row) shared(size, rank, proc, i_row)
    {
        k_row = malloc(sizeof(float) * size * 2);
    
        // Gauss Jordan
        for (i = 0; i < size; i++) {
            #pragma omp single
            recv_row(i_row, size * 2, ROOT, 0);

            #pragma omp for
            for (k = rank - 1; k < size; k += proc - 1) {
                if (i != k) {
                    recv_row(k_row, size * 2, ROOT, k);
                    ratio = k_row[i] / i_row[i];

                    for (j = 0; j < size * 2; j++) {
                        k_row[j] -= ratio * i_row[j];
                    }

                    send_row(k_row, size * 2, ROOT, k);
                }
            }
        }

        // Scale down solution
        #pragma omp for
        for (k = rank - 1; k < size; k += proc - 1) {
            recv_row(k_row, size * 2, ROOT, size + k);
            for (j = size; j < 2 * size; j++) {
                k_row[j] /= k_row[k];
            }

            send_row(k_row, size * 2, ROOT, size + k);
        }

        free(k_row);
    }

    free(i_row);
}

void main(int argc, char **argv) {
    double time;
    float **matrix;
    int size, rank, proc, provided;
    Input *input;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    omp_set_num_threads(NUMBER_OF_THREADS_PER_PROC);
    if (rank == ROOT) {
        input = read_input();
        matrix = input->matrix;
        size = input->size;
        time = get_time();
        calculate_inverse_root(matrix, size, proc);
        printf("time taken root: %.6lf\n", get_time() - time);
        //display_solution(matrix, size);
        free_matrix(matrix, size);
        free(input);
    } else {
        time = get_time();
        calculate_inverse_worker(rank, proc);
        printf("time taken worker %d: %.6lf\n", rank, get_time() - time);
    }

    MPI_Finalize();
}