#ifndef _UTILS_HEADER
#define _UTILS_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double get_time() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec + now.tv_nsec*1e-9;
}

void print_matrix(float **matrix, int num_rows, int num_cols) {
    int i, j;
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            printf("%f ", matrix[i][j]);
        }

        printf("\n");
    }
}

void display_solution(float **matrix, int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = size; j < size * 2; j++) {
            printf("%f ", matrix[i][j]);
        }

        printf("\n");
    }
}

void free_matrix(float **matrix, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free(matrix[i]);
    }

    free(matrix);
}

typedef struct input {
    float **matrix;
    int size;
} Input;

Input *read_input() {
    FILE *fp;
    char *line = NULL, *token;
    size_t len = 0;
    int size, i, j;
    float **matrix;

    fp = fopen("input", "r");
    if (!fp)
        exit(EXIT_FAILURE);

    getline(&line, &len, fp);
    token = strtok(line, " ");
    size = atoi(token);
    matrix = malloc(size * sizeof(float *));
    for (i = 0; i < size; i++) {
        matrix[i] = calloc(size * 2, sizeof(float));
    }

    i = 0;
    while (getline(&line, &len, fp) != -1) {
        token = strtok(line, " ");
        for (j = 0; j < size; j++)
        {
            matrix[i][j] = atoi(token);
            token = strtok(NULL, " ");
        }

        i++;
    }

    fclose(fp);
    free(line);

    Input *input = malloc(sizeof(Input));
    input->matrix = matrix;
    input->size = size;

    return input;
}

#endif