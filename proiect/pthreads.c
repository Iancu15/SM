#include "utils.h"
#include <pthread.h>
#include <math.h>

int min(int a, int b) {
    if (a < b)
        return a;
    
    return b;
}

int calculate_start(int thread_id, int dim, int num_of_threads) {
	return thread_id * ceil((double) dim / num_of_threads);
}

int calculate_end(int thread_id, int dim, int num_of_threads) {
	return min((thread_id + 1) * ceil((double) dim / num_of_threads), dim);
}

void calculate_inverse(float **matrix, int size, int number_of_threads, int id, pthread_barrier_t *barrier) {
    int i, j, k, start, end;
    float ratio;

    start = calculate_start(id, size, number_of_threads);
    end = calculate_end(id, size, number_of_threads);

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

        for (k = start; k < end; k++) {
            if (i != k) {
                ratio = matrix[k][i] / matrix[i][i];

                for (j = 0; j < size * 2; j++) {
                    matrix[k][j] -= ratio * matrix[i][j];
                }
            }
        }

        pthread_barrier_wait(barrier);
    }

    // Scale down solution
    for (i = start; i < end; i++) {
        for (j = size; j < 2 * size; j++) {
            matrix[i][j] /= matrix[i][i];
        }
    }
}

typedef struct arg {
    int id;
    float **matrix;
    int size;
    int number_of_threads;
    pthread_barrier_t *barrier;
} ThreadArg;

void *thread_function(void *arg) {
    ThreadArg *thread_arg;
    float **matrix;
    pthread_barrier_t *barrier;
    int id, number_of_threads, size;

    thread_arg = arg;
    id = thread_arg->id;
    matrix = thread_arg->matrix;
    size = thread_arg->size;
    number_of_threads = thread_arg->number_of_threads;
    barrier = thread_arg->barrier;

    calculate_inverse(matrix, size, number_of_threads, id, barrier);
}

void main(int argc, char *argv[]) {
    double time;
    float **matrix;
    int size, number_of_threads, r, id;
    pthread_barrier_t *barrier;
    ThreadArg *arguments;
    pthread_t *threads;
    void *status;

    if (argc != 2) {
        printf("There should be 1 argument! Format is: ./openmp number_of_threads\n");
        exit(EXIT_FAILURE);
    }

    number_of_threads = atoi(argv[1]);
    if (number_of_threads < 0) {
        exit(EXIT_FAILURE);
    }

    Input *input = read_input();
    matrix = input->matrix;
    size = input->size;

    time = get_time();

    barrier = malloc(sizeof(pthread_barrier_t));
    r = pthread_barrier_init(barrier, NULL, number_of_threads);
    if (r < 0) {
        printf("Eroare initializare bariera\n");
        exit(EXIT_FAILURE);
    }

    arguments = malloc(sizeof(ThreadArg) * number_of_threads);
    threads = malloc(sizeof(pthread_t) * number_of_threads);
    for (id = 0; id < number_of_threads; id++) {
        arguments[id].id = id;
        arguments[id].matrix = matrix;
        arguments[id].size = size;
        arguments[id].number_of_threads = number_of_threads;
        arguments[id].barrier = barrier;
        r = pthread_create(&threads[id], NULL, thread_function, (void *) &arguments[id]);

        if (r) {
            printf("Eroare la crearea thread-ului %ld\n", id);
            exit(-1);
        }
    }

    for (id = 0; id < number_of_threads; id++) {
        r = pthread_join(threads[id], &status);

        if (r) {
            printf("Eroare la asteptarea thread-ului %ld\n", id);
            exit(-1);
        }
    }

    r = pthread_barrier_destroy(barrier);
    if (r < 0) {
        printf("Eroare dezalocare bariera\n");
        exit(EXIT_FAILURE);
    }

    printf("time taken %.6lf\n", get_time() - time);
    //display_solution(matrix, size);
    free_matrix(matrix, size);
    free(input);
    free(arguments);
    free(threads);
}