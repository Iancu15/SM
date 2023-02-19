#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define SIZE 100

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

void create_array(double *arr, int size, int start, int end)
{
    int i;
    for (i = start; i < end; ++i)
    {
        arr[i] = i + 1;
    }
}

double sum_power_array(double *arr, int size, int power, int start, int end)
{
    int i;
    double sum = 0;
    for (i = start; i < end; ++i)
    {
        sum += pow(arr[i], power);
    }

    return sum;
}

typedef struct arg {
    int power;
    int id;
    double *arr;
    int number_of_threads;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;
    double *sum;
} ThreadArg;

void *thread_function(void *arg) {
    ThreadArg *thread_arg = arg;
    double *v, partial_sum, *arr, *sum;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;
    int power, id, number_of_threads, start, end;

    power = thread_arg->power;
    id = thread_arg->id;
    arr = thread_arg->arr;
    number_of_threads = thread_arg->number_of_threads;
    start = calculate_start(id, SIZE, number_of_threads);
    end = calculate_end(id, SIZE, number_of_threads);
    mutex = thread_arg->mutex;
    barrier = thread_arg->barrier;
    sum = thread_arg->sum;

    create_array(arr, SIZE, start, end);
    pthread_barrier_wait(barrier);
    partial_sum = sum_power_array(arr, SIZE, power, start, end);

    pthread_mutex_lock(mutex);
    *sum += partial_sum;
    pthread_mutex_unlock(mutex);
}

// primeste ca argumente un numar x si numarul de thread-uri
// numar x (in cod e variabila power)
// pe baza lui se calculeaza 1^x + 2^x + ... + n^x
// pentru n = 100 si x = 2 putem folosi formula sa ne
// verificam si anume 100 * (100 + 1) * 201 / 6 =
// 100 * 101 * 201 / 6 = 338350
// merge si pentru x negativ
// rulare: ./pthreads x number_of_threads
int main(int argc, char *argv[])
{
    double *v, *sum;
    int i, power, number_of_threads, id, r;
    ThreadArg *arguments;
    pthread_t *threads;
    void *status;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;

    if (argc != 3)
    {
        printf("There should be 2 arguments! Format is: ./openmp power number_of_threads\n");
        exit(EXIT_FAILURE);
    }

    power = atoi(argv[1]);
    number_of_threads = atoi(argv[2]);
    if (number_of_threads < 0)
    {
        exit(EXIT_FAILURE);
    }

    mutex = malloc(sizeof(pthread_mutex_t));
    r = pthread_mutex_init(mutex, NULL);
    if (r < 0) {
        printf("Eroare initializare mutex\n");
        exit(EXIT_FAILURE);
    }

    barrier = malloc(sizeof(pthread_barrier_t));
    r = pthread_barrier_init(barrier, NULL, number_of_threads);
    if (r < 0) {
        printf("Eroare initializare bariera\n");
        exit(EXIT_FAILURE);
    }

    arguments = malloc(sizeof(ThreadArg) * number_of_threads);
    threads = malloc(sizeof(pthread_t) * number_of_threads);
    v = (double *)malloc(SIZE * sizeof(double));
    sum = malloc(sizeof(double));
    *sum = 0;
    for (id = 0; id < number_of_threads; id++) {
        arguments[id].id = id;
        arguments[id].power = power;
        arguments[id].arr = v;
        arguments[id].number_of_threads = number_of_threads;
        arguments[id].mutex = mutex;
        arguments[id].barrier = barrier;
        arguments[id].sum = sum;
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

    r = pthread_mutex_destroy(mutex);
    if (r < 0) {
        printf("Eroare dezalocare mutex\n");
        exit(EXIT_FAILURE);
    }

    r = pthread_barrier_destroy(barrier);
    if (r < 0) {
        printf("Eroare dezalocare bariera\n");
        exit(EXIT_FAILURE);
    }

    printf("Full sum is %lf\n", *sum);

    free(sum);
    free(arguments);
    free(threads);
    free(v);

    return 0;
}