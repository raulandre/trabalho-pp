#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifndef N
#define N 1000
#endif

void swap(int* arr, int i, int j) {
    int t = arr[i];
    arr[i] = arr[j];
    arr[j] = t;
}

void quicksort(int* arr, int start, int end) {
    int pivot, index;
    if (end <= 1)
        return;

    pivot = arr[start + end / 2];
    swap(arr, start, start + end / 2);
    index = start;
    for (int i = start + 1; i < start + end; i++) {
        if (arr[i] < pivot) {
            index++;
            swap(arr, i, index);
        }
    }
    swap(arr, start, index);
    quicksort(arr, start, index - start);
    quicksort(arr, index + 1, start + end - index - 1);
}

int* merge(int* arr1, int n1, int* arr2, int n2) {
    int* result = (int*)malloc((n1 + n2) * sizeof(int));
    int i = 0, j = 0, k;
    for (k = 0; k < n1 + n2; k++) {
        if (i >= n1) {
            result[k] = arr2[j++];
        } else if (j >= n2) {
            result[k] = arr1[i++];
        } else if (arr1[i] < arr2[j]) {
            result[k] = arr1[i++];
        } else {
            result[k] = arr2[j++];
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    int number_of_elements = N;
    int* data = NULL;
    int chunk_size, own_chunk_size;
    int* chunk;
    double time_taken;
    MPI_Status status;

    int number_of_process, rank_of_process;
    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS) {
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);

    if (rank_of_process == 0) {
        data = (int*)malloc(N * sizeof(int));
        srand(1234);
        for (int i = 0; i < N; i++) {
            data[i] = rand() % 10000;
        }
        printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    time_taken = -MPI_Wtime();

    MPI_Bcast(&number_of_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);

    chunk_size = (number_of_elements + number_of_process - 1) / number_of_process;

    chunk = (int*)malloc(chunk_size * sizeof(int));

    MPI_Scatter(data, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank_of_process == 0) {
        free(data);
    }

    own_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 1)) ? chunk_size : (number_of_elements - chunk_size * rank_of_process);

    quicksort(chunk, 0, own_chunk_size);

    for (int step = 1; step < number_of_process; step *= 2) {
        if (rank_of_process % (2 * step) != 0) {
            MPI_Send(chunk, own_chunk_size, MPI_INT, rank_of_process - step, 0, MPI_COMM_WORLD);
            break;
        }

        if (rank_of_process + step < number_of_process) {
            int received_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 2 * step)) ? (chunk_size * step) : (number_of_elements - chunk_size * (rank_of_process + step));
            int* chunk_received = (int*)malloc(received_chunk_size * sizeof(int));
            MPI_Recv(chunk_received, received_chunk_size, MPI_INT, rank_of_process + step, 0, MPI_COMM_WORLD, &status);

            data = merge(chunk, own_chunk_size, chunk_received, received_chunk_size);

            free(chunk);
            free(chunk_received);
            chunk = data;
            own_chunk_size += received_chunk_size;
        }
    }

    time_taken += MPI_Wtime();

    if (rank_of_process == 0) {
        for (int i = 1; i < number_of_elements; i++) {
	    if (chunk[i - 1] > chunk[i]) {
		printf("Validation FAILED\n");
		MPI_Finalize();
                return 1;
	    }
        }
        printf("\nQuicksort %d ints on %d procs: %f secs\n", number_of_elements, number_of_process, time_taken);

        free(chunk);
    }

    MPI_Finalize();
    return 0;
}

