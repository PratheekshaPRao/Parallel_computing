
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define MIN_SIZE 1000

int *temp;

void merge(int a[], int l, int m, int r) {
    int i = l, j = m + 1, k = l;

    while (i <= m && j <= r)
        temp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];

    while (i <= m) temp[k++] = a[i++];
    while (j <= r) temp[k++] = a[j++];

    for (i = l; i <= r; i++)
        a[i] = temp[i];
}

void mergeSortSequential(int a[], int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSortSequential(a, l, m);
        mergeSortSequential(a, m + 1, r);
        merge(a, l, m, r);
    }
}

void mergeSortParallel(int a[], int l, int r) {
    if (r - l + 1 <= MIN_SIZE) {
        mergeSortSequential(a, l, r);
        return;
    }

    if (l < r) {
        int m = (l + r) / 2;

        #pragma omp parallel sections
        {
            #pragma omp section
            mergeSortParallel(a, l, m);

            #pragma omp section
            mergeSortParallel(a, m + 1, r);
        }

        merge(a, l, m, r);
    }
}

int main() {
    int n = 100000;
    int *a = malloc(n * sizeof(int));
    int *b = malloc(n * sizeof(int));
    temp = malloc(n * sizeof(int));

    srand(time(NULL));

    for (int i = 0; i < n; i++)
        a[i] = b[i] = rand() % 10000;

    printf("Sorting %d elements...\n", n);

    double start = omp_get_wtime();
    mergeSortSequential(a, 0, n - 1);
    double seq = omp_get_wtime() - start;

    printf("Sequential Merge Sort Time: %f seconds\n", seq);

    start = omp_get_wtime();
    mergeSortParallel(b, 0, n - 1);
    double par = omp_get_wtime() - start;

    printf("Parallel Merge Sort Time: %f seconds\n", par);
    printf("Difference: %f seconds\n", seq - par);

    if (par > 0)
        printf("Speedup: %.2fx\n", seq / par);

    printf("\nFirst 20 sorted elements:\n");
    for (int i = 0; i < 20; i++)
        printf("%d ", b[i]);

    printf("\n");

    free(a);
    free(b);
    free(temp);

    return 0;
}
