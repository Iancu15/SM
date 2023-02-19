#include "utils.h"

int main(int argc, char *argv[]) {
    int result;
    double time;

    Input *input = read_input();
    time = get_time();
    result = quick_select(input->v, input->size, input->k);
    printf("time taken %.6lf\n", get_time() - time);
    printf("%d\n", result);
    free(input);
}

int quick_select(int *v, int size, int k) {
    int pivot = v[size/2];
    int *lesser, *greater;
    int lesser_i = 0, greater_i = 0, v_i;
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
        return quick_select(lesser, lesser_i, k);
    } else if (k > lesser_i + 1) {
        free(lesser);
        return quick_select(greater, greater_i, k - (lesser_i + 1));
    } else {
        free(lesser);
        free(greater);
        return pivot;
    }
}