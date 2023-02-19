#include "utils.h"
#include <mpi.h>
#include <pthread.h>
#include <math.h>

#define ROOT 0
#define NUMBER_OF_THREADS_PER_PROC 2

int min(int a, int b) {
    if (a < b)
        return a;
    
    return b;
}

int max(int a, int b) {
    if (a < b)
        return b;
    
    return a;
}

int calculate_start(int thread_id, int dim, int num_of_threads) {
	return thread_id * ceil((double) dim / num_of_threads);
}

int calculate_end(int thread_id, int dim, int num_of_threads) {
	return min((thread_id + 1) * ceil((double) dim / num_of_threads), dim);
}

void send_row(float *row, int size, int destination, int tag) {
    MPI_Send(row, size, MPI_FLOAT, destination, tag, MPI_COMM_WORLD);
}

void recv_row(float *row, int size, int source, int tag) {
    MPI_Recv(row, size, MPI_FLOAT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void scatter_rows(float **matrix, int size, int start, int end, int proc, int tag, pthread_barrier_t *barrier) {
    int i, rank;

    for (i = start; i < end; i++) {
        rank = i % (proc - 1) + 1;
        send_row(matrix[i], size * 2, rank, tag * size + i);
    }
}

void gather_rows(float **matrix, int size, int start, int end, int proc, int tag, pthread_barrier_t *barrier) {
    int i, rank;

    for (i = start; i < end; i++) {
        rank = i % (proc - 1) + 1;
        recv_row(matrix[i], size * 2, rank, tag * size + i);
    }
}

typedef struct arg_root {
    int id;
    float **matrix;
    int size;
    int proc;
    pthread_barrier_t *barrier;
} ThreadArgRoot;

void *thread_function_root(void *arg) {
    ThreadArgRoot *thread_arg;
    float **matrix;
    pthread_barrier_t *barrier;
    int id, start, end, size, proc, i, j, k, rank, start_rank, end_rank;

    thread_arg = arg;
    id = thread_arg->id;
    matrix = thread_arg->matrix;
    size = thread_arg->size;
    proc = thread_arg->proc;
    barrier = thread_arg->barrier;

    start = calculate_start(id, size, NUMBER_OF_THREADS_PER_PROC);
    end = calculate_end(id, size, NUMBER_OF_THREADS_PER_PROC);

    start_rank = max(calculate_start(id, proc, NUMBER_OF_THREADS_PER_PROC), 1);
    end_rank = calculate_end(id, proc, NUMBER_OF_THREADS_PER_PROC);

    // Augment matrix
    for (i = start; i < end; i++) {
        matrix[i][i + size] = 1;
    }

    // Gauss Jordan
    for (i = 0; i < size; i++) {
        if (matrix[i][i] == 0.0) {
            printf("Mathematical Error!");
            exit(EXIT_FAILURE);
        }

        // Send current i row
        for (rank = start_rank; rank < end_rank; rank++) {
            send_row(matrix[i], size * 2, rank, 0);
        }

        pthread_barrier_wait(barrier);

        // Send the k rows to workers
        for (k = start; k < end; k++) {
            if (i != k) {
                rank = k % (proc - 1) + 1;
               send_row(matrix[k], size * 2, rank, k);
            }
        }

        pthread_barrier_wait(barrier);

        // Receive the modified k rows from workers
        for (k = start; k < end; k++) {
            if (i != k) {
                rank = k % (proc - 1) + 1;
                recv_row(matrix[k], size * 2, rank, k);
            }
        }

        pthread_barrier_wait(barrier);
    }

    // Get scaled down rows from workers
    scatter_rows(matrix, size, start, end, proc, 1, barrier);
    pthread_barrier_wait(barrier);
    gather_rows(matrix, size, start, end, proc, 1, barrier);
}

void calculate_inverse_root(float **matrix, int size, int proc, pthread_barrier_t *barrier) {
    int id, r;
    ThreadArgRoot *arguments;
    pthread_t *threads;
    void *status;

    MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    arguments = malloc(sizeof(ThreadArgRoot) * NUMBER_OF_THREADS_PER_PROC);
    threads = malloc(sizeof(pthread_t) * NUMBER_OF_THREADS_PER_PROC);
    for (id = 0; id < NUMBER_OF_THREADS_PER_PROC; id++) {
        arguments[id].id = id;
        arguments[id].matrix = matrix;
        arguments[id].size = size;
        arguments[id].proc = proc;
        arguments[id].barrier = barrier;
        r = pthread_create(&threads[id], NULL, thread_function_root, (void *) &arguments[id]);

        if (r) {
            printf("Eroare la crearea thread-ului %ld\n", id);
            exit(-1);
        }
    }

    for (id = 0; id < NUMBER_OF_THREADS_PER_PROC; id++) {
        r = pthread_join(threads[id], &status);

        if (r) {
            printf("Eroare la asteptarea thread-ului %ld\n", id);
            exit(-1);
        }
    }
}

typedef struct arg_worker {
    int id;
    int rank;
    int proc;
    int size;
    float *i_row;
    float *k_row;
    pthread_barrier_t *barrier;
} ThreadArgWorker;

void *thread_function_worker(void *arg) {
    ThreadArgWorker *thread_arg;
    pthread_barrier_t *barrier;
    float *i_row, *k_row, ratio;
    int id, start_gauss_jordan, end_gauss_jordan, start_scale_down, end_scale_down, size, proc, i, j, k, rank;

    thread_arg = arg;
    id = thread_arg->id;
    proc = thread_arg->proc;
    barrier = thread_arg->barrier;
    rank = thread_arg->rank;
    i_row = thread_arg->i_row;
    size = thread_arg->size;
    k_row = thread_arg->k_row;

    start_gauss_jordan = calculate_start(id, size * 2, NUMBER_OF_THREADS_PER_PROC);
    end_gauss_jordan = calculate_end(id, size * 2, NUMBER_OF_THREADS_PER_PROC);
    start_scale_down = calculate_start(id, size, NUMBER_OF_THREADS_PER_PROC) + size;
    end_scale_down = calculate_end(id, size, NUMBER_OF_THREADS_PER_PROC) + size;

    // Gauss Jordan
    for (i = 0; i < size; i++) {
        if (id == 0) {
            recv_row(i_row, size * 2, ROOT, 0);
        }

        for (k = rank - 1; k < size; k += proc - 1) {
            if (i != k) {
                if (id == 0) {
                    recv_row(k_row, size * 2, ROOT, k);
                }

                pthread_barrier_wait(barrier);
                ratio = k_row[i] / i_row[i];
                pthread_barrier_wait(barrier);
                for (j = start_gauss_jordan; j < end_gauss_jordan; j++) {
                    k_row[j] -= ratio * i_row[j];
                }

                pthread_barrier_wait(barrier);
                if (id == 0) {
                    send_row(k_row, size * 2, ROOT, k);
                }
            }
        }
    }

    // Scale down solution
    for (i = rank - 1; i < size; i += proc - 1) {
        if (id == 0) {
            recv_row(i_row, size * 2, ROOT, size + i);
        }

        pthread_barrier_wait(barrier);
        for (j = start_scale_down; j < end_scale_down; j++) {
            i_row[j] /= i_row[i];
        }

        pthread_barrier_wait(barrier);
        if (id == 0) {
            send_row(i_row, size * 2, ROOT, size + i);
        }
    }
}

void calculate_inverse_worker(int rank, int proc, pthread_barrier_t *barrier) {
    int size, id, r;
    float *i_row, *k_row;
    ThreadArgWorker *arguments;
    pthread_t *threads;
    void *status;

    MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    i_row = malloc(sizeof(float) * size * 2);
    k_row = malloc(sizeof(float) * size * 2);
    arguments = malloc(sizeof(ThreadArgWorker) * NUMBER_OF_THREADS_PER_PROC);
    threads = malloc(sizeof(pthread_t) * NUMBER_OF_THREADS_PER_PROC);
    for (id = 0; id < NUMBER_OF_THREADS_PER_PROC; id++) {
        arguments[id].id = id;
        arguments[id].size = size;
        arguments[id].proc = proc;
        arguments[id].barrier = barrier;
        arguments[id].i_row = i_row;
        arguments[id].rank = rank;
        arguments[id].k_row = k_row;
        r = pthread_create(&threads[id], NULL, thread_function_worker, (void *) &arguments[id]);

        if (r) {
            printf("Eroare la crearea thread-ului %ld\n", id);
            exit(-1);
        }
    }

    for (id = 0; id < NUMBER_OF_THREADS_PER_PROC; id++) {
        r = pthread_join(threads[id], &status);

        if (r) {
            printf("Eroare la asteptarea thread-ului %ld\n", id);
            exit(-1);
        }
    }

    free(i_row);
    free(k_row);
}

void main(int argc, char **argv) {
    double time;
    float **matrix;
    int size, rank, proc, provided, r;
    Input *input;
    pthread_barrier_t *barrier;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    barrier = malloc(sizeof(pthread_barrier_t));
    r = pthread_barrier_init(barrier, NULL, NUMBER_OF_THREADS_PER_PROC);
    if (r < 0) {
        printf("Eroare initializare bariera\n");
        exit(EXIT_FAILURE);
    }

    if (rank == ROOT) {
        input = read_input();
        matrix = input->matrix;
        size = input->size;
        time = get_time();
        calculate_inverse_root(matrix, size, proc, barrier);
        printf("time taken root: %.6lf\n", get_time() - time);
        //display_solution(matrix, size);
        free_matrix(matrix, size);
        free(input);
    } else {
        time = get_time();
        calculate_inverse_worker(rank, proc, barrier);
        printf("time taken worker %d: %.6lf\n", rank, get_time() - time);
    }

    r = pthread_barrier_destroy(barrier);
    if (r < 0) {
        printf("Eroare dezalocare bariera\n");
        exit(EXIT_FAILURE);
    }

    free(barrier);

    MPI_Finalize();
}