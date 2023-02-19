#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define SIZE 100

double *create_array(int size)
{
    double *arr = (double *)malloc(size * sizeof(double));
    int i;

#pragma omp parallel for
    for (i = 0; i < size; ++i)
    {
        arr[i] = i + 1;
    }

    return arr;
}

double sum_power_array(double *arr, int size, int power)
{
    double sum = 0;
    int i;
#pragma omp parallel for private(i) reduction(+ \
                                              : sum) shared(arr, size, power)
    for (i = 0; i < size; ++i)
    {
        sum += pow(arr[i], power);
    }

    return sum;
}

// primeste ca argumente un numar x si numarul de thread-uri
// numar x (in cod e variabila power)
// pe baza lui se calculeaza 1^x + 2^x + ... + n^x
// pentru n = 100 si x = 2 putem folosi formula sa ne
// verificam si anume 100 * (100 + 1) * 201 / 6 =
// 100 * 101 * 201 / 6 = 338350
// merge si pentru x negativ, la labul precedent era doar pentru
// pozitiv ca foloseam int-uri, am modificat in double
int main(int argc, char *argv[])
{
    double *v, sum;
    int i, power, number_of_threads;

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

    omp_set_num_threads(number_of_threads);
    v = create_array(SIZE);
    sum = sum_power_array(v, SIZE, power);

    printf("Full sum is %lf\n", sum);

    return 0;
}