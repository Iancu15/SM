#include "utils.h"
#include <omp.h>

int main(int argc, char *argv[]) {
    int result, number_of_threads;
    double time;

    if (argc != 2) {
        printf("There's only one argument and that is the number of threads!\n");
        exit(EXIT_FAILURE);
    }

    number_of_threads = atoi(argv[1]);
    if (!test_number_validity(number_of_threads, 1, INT_MAX, "Invalid number of threads"))
            exit(EXIT_FAILURE);

    omp_set_num_threads(number_of_threads);
    Input *input = read_input();
    time = get_time();
    result = quick_select(input->v, input->size, input->k);
    printf("time taken %.6lf\n", get_time() - time);
    printf("%d\n", result);
    free(input);
}

int quick_select(int *v, int size, int k) {
    int *lesser, *greater, pivot, was_result_found = 0, result;
    int lesser_i, greater_i, v_i, thread_lesser_i, thread_greater_i;

    #pragma omp parallel private(v_i, thread_lesser_i, thread_greater_i)
    {
        while (!was_result_found) {
            #pragma omp single
            {
                pivot = v[size/2];
                lesser_i = 0;
                greater_i = 0;
                lesser = malloc(sizeof(int) * size);
                greater = malloc(sizeof(int) * size);
            }

            #pragma omp for
            for (v_i = 0; v_i < size; v_i++) {
                if (v_i == size/2)
                    continue;

                if (v[v_i] < pivot) {
                    #pragma omp critical
                    {
                        thread_lesser_i = lesser_i++;
                    }

                    lesser[thread_lesser_i] = v[v_i];
                } else {
                    #pragma omp critical
                    {
                        thread_greater_i = greater_i++;
                    }

                    greater[thread_greater_i] = v[v_i];
                }
            }

            #pragma omp single
            {
                free(v);
                if (k <= lesser_i) {
                    free(greater);
                    v = lesser;
                    size = lesser_i;
                } else if (k > lesser_i + 1) {
                    free(lesser);
                    v = greater;
                    size = greater_i;
                    k = k - (lesser_i + 1);
                } else {
                    free(lesser);
                    free(greater);
                    result = pivot;
                    was_result_found = 1;
                }
            }
        }
    }

    return result;
}