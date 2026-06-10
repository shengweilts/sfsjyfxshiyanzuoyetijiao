/**
 * 0-1 Knapsack Experiment (with CSV export for n=1000)
 * - For n=1000: prints first 20 items on screen, and writes all 1000 items to items_1000.csv
 * - For n>1000: only performance table
 * Compile: gcc -O2 knapsack_experiment_final.c -o knap_exp
 * Run: ./knap_exp > knap_output.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    double weight;
    double value;
    double ratio;
} Item;

// Generate random items
void gen_items(Item items[], int n) {
    for (int i = 0; i < n; i++) {
        items[i].weight = (double)(rand() % 100 + 1);
        items[i].value = (double)(rand() % 9001 + 1000) / 10.0;
        items[i].ratio = items[i].value / items[i].weight;
    }
}

// Write all items to CSV file (for n=1000)
void write_items_to_csv(Item items[], int n, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: cannot create file %s\n", filename);
        return;
    }
    fprintf(fp, "ID,Weight,Value\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d,%.2f,%.2f\n", i+1, items[i].weight, items[i].value);
    }
    fclose(fp);
    printf("Full item list (n=%d) saved to %s\n", n, filename);
}

// Print first 20 items on screen (as table example)
void print_items_table_sample(Item items[], int n) {
    printf("\n========== Item Statistics (n=%d) ==========\n", n);
    printf("%-8s %-12s %-12s\n", "ID", "Weight", "Value");
    int limit = (n < 20) ? n : 20;
    for (int i = 0; i < limit; i++) {
        printf("%-8d %-12.2f %-12.2f\n", i+1, items[i].weight, items[i].value);
    }
    if (n > 20) printf("... (only first 20 shown, full list saved to CSV file)\n");
    printf("\n");
}

// Print selected items only for small n (<=1000), and only first 20
void print_selection_small(int selection[], int n, Item items[], double total_value, const char *method) {
    if (n > 1000) return;
    printf("\n[%s] Selected items (first 20):\n", method);
    printf("%-8s %-12s %-12s\n", "ID", "Weight", "Value");
    double total_w = 0;
    int printed = 0;
    for (int i = 0; i < n && printed < 20; i++) {
        if (selection[i]) {
            printf("%-8d %-12.2f %-12.2f\n", i+1, items[i].weight, items[i].value);
            total_w += items[i].weight;
            printed++;
        }
    }
    if (printed == 0) printf("(no items selected)\n");
    else if (printed < n) printf("... and %d more items not shown\n", n - printed);
    printf("Total weight: %.2f, Total value: %.2f\n", total_w, total_value);
}

// Brute force (n <= 20)
double brute_force(Item items[], int n, double capacity, int *selection) {
    double max_val = 0;
    int total = 1 << n;
    for (int mask = 0; mask < total; mask++) {
        double cur_w = 0, cur_v = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                cur_w += items[i].weight;
                cur_v += items[i].value;
            }
        }
        if (cur_w <= capacity && cur_v > max_val) {
            max_val = cur_v;
            for (int i = 0; i < n; i++)
                selection[i] = (mask & (1 << i)) ? 1 : 0;
        }
    }
    return max_val;
}

// DP (1D + keep matrix)
double dp_knapsack(Item items[], int n, double capacity, int *selection, long long *mem_used) {
    int cap = (int)capacity;
    if (n <= 0 || cap <= 0) return 0;
    long long required = (long long)(n + 1) * (cap + 1) * sizeof(char);
    if (required > 400 * 1024 * 1024) {
        printf("[DP] Memory required %lld MB > 400MB, skipped.\n", required / (1024 * 1024));
        return -1;
    }
    *mem_used = required / (1024 * 1024);
    char **keep = (char **)malloc((n + 1) * sizeof(char *));
    for (int i = 0; i <= n; i++)
        keep[i] = (char *)calloc(cap + 1, sizeof(char));
    double *dp = (double *)calloc(cap + 1, sizeof(double));
    for (int i = 1; i <= n; i++) {
        int w = (int)items[i-1].weight;
        double v = items[i-1].value;
        for (int c = cap; c >= w; c--) {
            if (dp[c - w] + v > dp[c]) {
                dp[c] = dp[c - w] + v;
                keep[i][c] = 1;
            }
        }
    }
    double result = dp[cap];
    int cur_cap = cap;
    for (int i = n; i >= 1; i--) {
        if (keep[i][cur_cap]) {
            selection[i-1] = 1;
            cur_cap -= (int)items[i-1].weight;
        } else {
            selection[i-1] = 0;
        }
    }
    for (int i = 0; i <= n; i++) free(keep[i]);
    free(keep); free(dp);
    return result;
}

// Greedy by ratio
int cmp_ratio(const void *a, const void *b) {
    Item *ia = (Item *)a, *ib = (Item *)b;
    return (ib->ratio > ia->ratio) ? 1 : -1;
}
double greedy_with_selection(Item items[], int n, double capacity, int *selection) {
    typedef struct { Item item; int idx; } Indexed;
    Indexed *idx_items = (Indexed *)malloc(n * sizeof(Indexed));
    for (int i = 0; i < n; i++) {
        idx_items[i].item = items[i];
        idx_items[i].idx = i;
    }
    qsort(idx_items, n, sizeof(Indexed),
          (int (*)(const void*, const void*))cmp_ratio);
    double cur_w = 0, cur_v = 0;
    for (int i = 0; i < n; i++) selection[i] = 0;
    for (int i = 0; i < n; i++) {
        if (cur_w + idx_items[i].item.weight <= capacity) {
            cur_w += idx_items[i].item.weight;
            cur_v += idx_items[i].item.value;
            selection[idx_items[i].idx] = 1;
        }
    }
    free(idx_items);
    return cur_v;
}

// Backtracking (n <= 20)
double max_back_val;
int *best_back_sel;
void backtrack(Item items[], int n, int idx, double cur_w, double cur_v, double cap, int *sel) {
    if (idx == n) {
        if (cur_v > max_back_val) {
            max_back_val = cur_v;
            for (int i = 0; i < n; i++) best_back_sel[i] = sel[i];
        }
        return;
    }
    sel[idx] = 0;
    backtrack(items, n, idx+1, cur_w, cur_v, cap, sel);
    if (cur_w + items[idx].weight <= cap) {
        sel[idx] = 1;
        backtrack(items, n, idx+1, cur_w + items[idx].weight, cur_v + items[idx].value, cap, sel);
    }
}
double backtrack_knapsack(Item items[], int n, double capacity, int *selection) {
    if (n > 20) return -1;
    max_back_val = 0;
    best_back_sel = (int *)malloc(n * sizeof(int));
    int *tmp = (int *)malloc(n * sizeof(int));
    backtrack(items, n, 0, 0, 0, capacity, tmp);
    for (int i = 0; i < n; i++) selection[i] = best_back_sel[i];
    double res = max_back_val;
    free(best_back_sel); free(tmp);
    return res;
}

double get_ms(clock_t start, clock_t end) {
    return ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
}

int main() {
    srand((unsigned int)time(NULL));
    printf("========== 0-1 KNAPSACK EXPERIMENT ==========\n");

    int item_counts[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
                         20000, 40000, 80000, 160000, 320000};
    int num_counts = sizeof(item_counts) / sizeof(item_counts[0]);
    double capacities[] = {10000, 100000, 1000000};
    int num_caps = sizeof(capacities) / sizeof(capacities[0]);

    printf("\n%-8s %-10s %-12s %-12s %-12s %-12s %-10s\n",
           "Items", "Capacity", "Brute(ms)", "DP(ms)", "Greedy(ms)", "Backtrack(ms)", "DP_Mem(MB)");

    for (int i = 0; i < num_counts; i++) {
        int n = item_counts[i];
        Item *items = (Item *)malloc(n * sizeof(Item));
        gen_items(items, n);

        if (n == 1000) {
            // Print sample (first 20) on screen
            print_items_table_sample(items, n);
            // Export full 1000 items to CSV for attachment
            write_items_to_csv(items, n, "knapsack_items_1000.csv");
        }

        for (int j = 0; j < num_caps; j++) {
            double cap = capacities[j];
            int *selection = (int *)calloc(n, sizeof(int));
            clock_t start, end;
            double t_bf = -1, t_dp = -1, t_greedy = -1, t_back = -1;
            long long mem_dp = 0;

            if (n <= 20) {
                start = clock();
                double val = brute_force(items, n, cap, selection);
                end = clock();
                t_bf = get_ms(start, end);
                if (val >= 0) print_selection_small(selection, n, items, val, "Brute Force");
            }

            if (n <= 10000 && cap <= 100000) {
                start = clock();
                double val = dp_knapsack(items, n, cap, selection, &mem_dp);
                end = clock();
                if (val != -1) {
                    t_dp = get_ms(start, end);
                    print_selection_small(selection, n, items, val, "DP");
                }
            } else {
                t_dp = -1;
            }

            start = clock();
            double val_g = greedy_with_selection(items, n, cap, selection);
            end = clock();
            t_greedy = get_ms(start, end);
            print_selection_small(selection, n, items, val_g, "Greedy");

            if (n <= 20) {
                start = clock();
                double val_b = backtrack_knapsack(items, n, cap, selection);
                end = clock();
                if (val_b != -1) {
                    t_back = get_ms(start, end);
                    print_selection_small(selection, n, items, val_b, "Backtracking");
                }
            }

            printf("%-8d %-10.0f %-12.2f %-12.2f %-12.2f %-12.2f %-10lld\n",
                   n, cap,
                   (t_bf < 0 ? 0 : t_bf),
                   (t_dp < 0 ? 0 : t_dp),
                   t_greedy,
                   (t_back < 0 ? 0 : t_back),
                   mem_dp);

            free(selection);
        }
        free(items);
    }

    printf("\nExperiment finished.\n");
    return 0;
}