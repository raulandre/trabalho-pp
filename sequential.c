#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef N
#define N 1000
#endif

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main() {
    int *arr = malloc(N * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    srand(time(0));
    for (int i = 0; i < N; i++) {
        arr[i] = rand() % 10000;
    }

    clock_t start, end;
    start = clock();
    quickSort(arr, 0, N - 1);
    end = clock();

    for(int i = 1; i < N; i++) {
	if(arr[i-1] > arr[i]) {
		printf("Validation FAILED\n");
	}	
    }
    printf("Total time = %f secs\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    free(arr);
    return 0;
}

