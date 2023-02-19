#include "utils.h"
#include "mpi.h"

#define ROOT 0

int main(int argc, char *argv[]) {
    int rank, proc, result;
    double time;
    Input *input;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    if (rank == ROOT) {
        input = read_input();
        time = get_time();
        result = quick_select_root(input->v, input->size, input->k);
    }

    if (rank != ROOT) {
        quick_select_worker();
    }

    if (rank == ROOT) {
        printf("time taken %.6lf\n", get_time() - time);
        printf("%d\n", result);
        free(input);
    }
}

void quick_select_worker() {
    
}

int quick_select_root(int *v, int size, int k) {
    int *lesser, *greater, pivot;
    int lesser_i, greater_i, v_i;

    while (1) {
        pivot = v[size/2];
        lesser_i = 0;
        greater_i = 0;
        lesser = malloc(sizeof(int) * size);
        greater = malloc(sizeof(int) * size);
        for (v_i = 0; v_i < size; v_i++) {
            if (v_i == size/2)
                continue;

            if (v[v_i] < pivot) {
                lesser[lesser_i++] = v[v_i];
            } else {
                greater[greater_i++] = v[v_i];
            }
        }

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
            return pivot;
        }
    }
}