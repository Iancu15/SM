#ifndef _UTILS_HEADER
#define _UTILS_HEADER

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

double get_time()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec + now.tv_nsec*1e-9;
}

int quick_select(int *v, int size, int k);

int test_number_validity(int number, int lim_inf, int lim_sup, char* err_msg) {
    if (number < lim_inf || number > lim_sup) {
        printf("%s\n", err_msg);
        return 0;
    }

    return 1;
}

typedef struct input {
    int *v;
    int size;
    int k;
} Input;

Input* read_input() {
    FILE *fp;
    ssize_t read;
    char *line = NULL, *token;
    size_t len = 0;
    int size, i, k;
    int result;
    int *v;

    fp = fopen("input", "r");
    if (!fp)
        exit(EXIT_FAILURE);
    
    read = getline(&line, &len, fp);
    if (read < 0) {
        printf("getline error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " ");
    k = atoi(token);
    token = strtok(NULL, " ");
    size = atoi(token);
    if (!test_number_validity(size, 1, INT_MAX, "Invalid size value"))
        exit(EXIT_FAILURE);
    
    if (!test_number_validity(k, 1, size, "Invalid k value"))
        exit(EXIT_FAILURE);

    v = malloc(sizeof(int) * size);
    for (i = 0; i < size; i++) {
        token = strtok(NULL, " ");
        v[i] = atoi(token);
    }

    free(line);
    fclose(fp);

    Input *input = malloc(sizeof(Input));
    input->v = v;
    input->size = size;
    input->k = k;

    return input;
}

#endif