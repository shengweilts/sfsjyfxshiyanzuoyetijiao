/**
 * Sorting Experiment - Optimized output for subproblem sizes
 * For n <= 1000: output all sizes.
 * For larger n: output statistics (min, max, avg, distribution).
 * Compile: gcc -O2 sorting_experiment_optimized.c -o sort_exp
 * Run: ./sort_exp > sort_output.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

long long bubble_comp = 0;
long long merge_comp = 0;
long long quick_comp = 0;

int *merge_sizes = NULL;
int merge_count = 0;
int *quick_sizes = NULL;
int quick_count = 0;

void gen_random_array(int arr[], int n) {
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 100000;
}

void copy_array(int src[], int dst[], int n) {
    for (int i = 0; i < n; i++)
        dst[i] = src[i];
}

void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++) {
            bubble_comp++;
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
}

void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int i = 0; i < n2; i++) R[i] = arr[m + 1 + i];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        merge_comp++;
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}

void merge_sort(int arr[], int l, int r) {
    if (l < r) {
        int len = r - l + 1;
        merge_sizes[merge_count++] = len;
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        quick_comp++;
        if (arr[j] < pivot) {
            i++;
            int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }
    int tmp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = tmp;
    return i + 1;
}

void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int len = high - low + 1;
        quick_sizes[quick_count++] = len;
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

// Print all subproblem sizes (for small n)
void print_all_sizes(int sizes[], int count, const char *name, int n) {
    printf("\n%s subproblem sizes (n=%d, total %d calls):\n", name, n, count);
    for (int i = 0; i < count; i++) {
        printf("%d ", sizes[i]);
        if ((i + 1) % 20 == 0) printf("\n");
    }
    if (count % 20 != 0) printf("\n");
    printf("\n");
}

// Print statistics for large n
void print_size_statistics(int sizes[], int count, const char *name, int n) {
    if (count == 0) return;
    // Find min, max, sum
    int min = sizes[0], max = sizes[0];
    long long sum = 0;
    for (int i = 0; i < count; i++) {
        if (sizes[i] < min) min = sizes[i];
        if (sizes[i] > max) max = sizes[i];
        sum += sizes[i];
    }
    double avg = (double)sum / count;

    // Simple frequency buckets: 1-10, 11-100, 101-1000, >1000
    int bucket1 = 0, bucket2 = 0, bucket3 = 0, bucket4 = 0;
    for (int i = 0; i < count; i++) {
        if (sizes[i] <= 10) bucket1++;
        else if (sizes[i] <= 100) bucket2++;
        else if (sizes[i] <= 1000) bucket3++;
        else bucket4++;
    }

    printf("\n%s subproblem statistics (n=%d, total %d calls):\n", name, n, count);
    printf("  Min size: %d, Max size: %d, Average size: %.2f\n", min, max, avg);
    printf("  Distribution:\n");
    printf("    [1-10]   : %d (%.1f%%)\n", bucket1, bucket1*100.0/count);
    printf("    [11-100] : %d (%.1f%%)\n", bucket2, bucket2*100.0/count);
    printf("    [101-1000]: %d (%.1f%%)\n", bucket3, bucket3*100.0/count);
    printf("    >1000    : %d (%.1f%%)\n", bucket4, bucket4*100.0/count);
    printf("\n");
}

int main() {
    srand((unsigned int)time(NULL));
    printf("========== SORTING EXPERIMENT (OPTIMIZED OUTPUT) ==========\n");

    // Part 1: Two groups of 100 random numbers
    printf("\n--- Part 1: Two groups of 100 numbers ---\n");
    int arr1[100], arr2[100];
    gen_random_array(arr1, 100);
    gen_random_array(arr2, 100);
    int temp[100];

    for (int group = 1; group <= 2; group++) {
        int *arr = (group == 1) ? arr1 : arr2;
        printf("\nGroup %d:\n", group);

        copy_array(arr, temp, 100);
        bubble_comp = 0;
        bubble_sort(temp, 100);
        printf("  Bubble comparisons: %lld\n", bubble_comp);

        copy_array(arr, temp, 100);
        merge_comp = 0;
        merge_count = 0;
        merge_sizes = malloc(2000 * sizeof(int)); // 100's recursion depth < 2000
        merge_sort(temp, 0, 99);
        printf("  Merge comparisons: %lld\n", merge_comp);
        print_all_sizes(merge_sizes, merge_count, "Merge sort", 100);
        free(merge_sizes);

        copy_array(arr, temp, 100);
        quick_comp = 0;
        quick_count = 0;
        quick_sizes = malloc(2000 * sizeof(int));
        quick_sort(temp, 0, 99);
        printf("  Quick comparisons: %lld\n", quick_comp);
        print_all_sizes(quick_sizes, quick_count, "Quick sort", 100);
        free(quick_sizes);
    }

    // Part 2 & 3: Different sizes
    printf("\n--- Part 2 & 3: Comparisons and subproblem analysis ---\n");
    int sizes[] = {10, 100, 1000, 2000, 5000, 10000, 100000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("\nComparison counts table:\n");
    printf("%-8s %-15s %-15s %-15s\n", "Size", "Bubble", "Merge", "Quick");

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int *arr = malloc(n * sizeof(int));
        int *copy = malloc(n * sizeof(int));
        gen_random_array(arr, n);

        // Bubble
        copy_array(arr, copy, n);
        bubble_comp = 0;
        bubble_sort(copy, n);
        long long b_comp = bubble_comp;

        // Merge
        copy_array(arr, copy, n);
        merge_comp = 0;
        merge_count = 0;
        int merge_capacity = n * 2;  // recursion depth < 2*n
        merge_sizes = malloc(merge_capacity * sizeof(int));
        merge_sort(copy, 0, n - 1);
        long long m_comp = merge_comp;

        // Quick
        copy_array(arr, copy, n);
        quick_comp = 0;
        quick_count = 0;
        quick_sizes = malloc(merge_capacity * sizeof(int));
        quick_sort(copy, 0, n - 1);
        long long q_comp = quick_comp;

        // Output comparison row
        printf("%-8d %-15lld %-15lld %-15lld\n", n, b_comp, m_comp, q_comp);

        // Output subproblem analysis based on n
        if (n <= 1000) {
            print_all_sizes(merge_sizes, merge_count, "Merge sort", n);
            print_all_sizes(quick_sizes, quick_count, "Quick sort", n);
        } else {
            print_size_statistics(merge_sizes, merge_count, "Merge sort", n);
            print_size_statistics(quick_sizes, quick_count, "Quick sort", n);
        }

        free(merge_sizes);
        free(quick_sizes);
        free(arr); free(copy);
    }

    printf("\nExperiment finished.\n");
    return 0;
}