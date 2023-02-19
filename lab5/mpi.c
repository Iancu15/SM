#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROOT 0
#define CHUNKS_SIZE 4

int *create_array(int size)
{
    int *arr = (int *)malloc(size * sizeof(int));
    for (int i = 0; i < size; ++i)
    {
        arr[i] = i + 1;
    }

    return arr;
}

int sum_power_array(int *arr, int size, int power)
{
    int sum = 0;
    for (int i = 0; i < size; ++i)
    {
        sum += pow(arr[i], power);
    }

    return sum;
}

// citeste de la tastatura un numar x (in cod e variabila power)
// pe baza lui se calculeaza 1^x + 2^x + ... + n^x
// unde n este numarul de procese * dimensiunea unui chunk
int main(int argc, char **argv)
{
    int rank, proc, *v, i, sum, power;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);

    if (rank == ROOT)
    {
        v = create_array(CHUNKS_SIZE * (proc - 1));
        for (i = 1; i < proc; i++)
        {
            MPI_Send(v + (i - 1) * CHUNKS_SIZE, CHUNKS_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        scanf("%d", &power);
    }

    if (rank != ROOT)
    {
        v = malloc(sizeof(int) * CHUNKS_SIZE);
        MPI_Recv(v, CHUNKS_SIZE, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Bcast(&power, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT)
    {
        int partial_sum;
        sum = 0;
        for (i = 1; i < proc; i++)
        {
            MPI_Recv(&partial_sum, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += partial_sum;
        }
    }

    if (rank != ROOT)
    {
        sum = sum_power_array(v, CHUNKS_SIZE, power);
        MPI_Send(&sum, 1, MPI_INT, ROOT, 1, MPI_COMM_WORLD);
    }

    MPI_Bcast(&sum, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    printf("Thread %d got the full sum %d\n", rank, sum);

    MPI_Finalize();
    return 0;
}