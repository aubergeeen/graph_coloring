#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void generate_random_graph(int n, double p, int **graph) {
    // Initialize adjacency matrix
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = 0;
        }
    }
    
    // Generate random edges
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if ((double)rand() / RAND_MAX < p) {
                graph[i][j] = graph[j][i] = 1; // Undirected edge
            }
        }
    }
}

int greedy_coloring(int n, int **graph, int *ver_colors) {
    for (int i = 0; i < n; i++) {
        ver_colors[i] = -1; // Initialize colors
    }

    for (int vertex = 0; vertex < n; vertex++) {
        bool *used_colors = (bool *)calloc(n, sizeof(bool));

        // Check colors of adjacent vertices
        for (int neighbor = 0; neighbor < n; neighbor++) {
            if (graph[vertex][neighbor] == 1 && ver_colors[neighbor] != -1) {
                used_colors[ver_colors[neighbor]] = true;
            }
        }

        // Find first available color
        int color = 0;
        while (color < n && used_colors[color]) {
            color++;
        }

        ver_colors[vertex] = color;
        free(used_colors);
    }

    // Count unique colors
    bool *unique_colors = (bool *)calloc(n, sizeof(bool));
    int color_count = 0;
    for (int i = 0; i < n; i++) {
        if (!unique_colors[ver_colors[i]]) {
            unique_colors[ver_colors[i]] = true;
            color_count++;
        }
    }
    free(unique_colors);

    return color_count;
}

bool is_valid_coloring(int n, int **graph, int *colors) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (graph[i][j] == 1 && colors[i] == colors[j]) {
                return false; // Invalid coloring
            }
        }
    }
    return true;
}

void bruteforce_coloring(int n, int **graph, int *total_colors, int *coloring) {
    for (int k = 1; k <= n; k++) {
        // Initialize coloring
        for (int i = 0; i < n; i++) {
            coloring[i] = 0;
        }

        // Try all possible colorings with k colors
        while (true) {
            if (is_valid_coloring(n, graph, coloring)) {
                *total_colors = k;
                return;
            }

            // Generate next coloring (like incrementing a number)
            int index = n - 1;
            while (index >= 0) {
                if (coloring[index] < k - 1) {
                    coloring[index]++;
                    break;
                } else {
                    coloring[index] = 0;
                    index--;
                }
            }
            if (index < 0) break; // All colorings tried
        }
    }
    
    // Worst case - n colors needed
    *total_colors = n;
    for (int i = 0; i < n; i++) {
        coloring[i] = i;
    }
}

double measure_time_for_coloring(int n, int **graph, 
                               void (*func)(int, int **, int *, int *)) {
    clock_t start_time = clock();
    int total_colors;
    int *coloring = (int *)malloc(n * sizeof(int));
    func(n, graph, &total_colors, coloring);
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    free(coloring);
    return elapsed_time;
}

void test_performance(void (*func)(int, int **, int *, int *), 
                     int max_vertices, int step, int start) {
    for (int n = start; n <= max_vertices; n += step) {
        // Allocate graph
        int **graph = (int **)malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) {
            graph[i] = (int *)malloc(n * sizeof(int));
        }
        
        generate_random_graph(n, 0.5, graph);
        double time = measure_time_for_coloring(n, graph, func);

        printf("For graph with %d vertices: Time = %.6f sec\n", n, time);
        if (time > 180.0) {  // If time exceeds 3 minutes (180 seconds)
            printf("For %d vertices time exceeds limit, stopping tests.\n", n);
            break;
        }

        // Free graph memory
        for (int i = 0; i < n; i++) {
            free(graph[i]);
        }
        free(graph);
    }
}

int main() {
    srand(time(NULL)); // Initialize random number generator

    // Test with small fixed graph
    int n = 4;
    int **graph = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        graph[i] = (int *)malloc(n * sizeof(int));
    }
    
    // Example graph
    int example[4][4] = {
        {0, 1, 1, 1},
        {1, 0, 1, 0},
        {1, 1, 0, 1},
        {1, 0, 1, 0}
    };
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            graph[i][j] = example[i][j];
        }
    }

    // Brute force coloring
    int total_colors;
    int *coloring = (int *)malloc(n * sizeof(int));
    bruteforce_coloring(n, graph, &total_colors, coloring);
    printf("Brute Force Coloring: %d colors\nColors: ", total_colors);
    for (int i = 0; i < n; i++) {
        printf("%d ", coloring[i]);
    }
    printf("\n");
    free(coloring);

    // Greedy coloring
    int *ver_colors = (int *)malloc(n * sizeof(int));
    int color_count = greedy_coloring(n, graph, ver_colors);
    printf("Greedy Coloring: %d colors\nColors: ", color_count);
    for (int i = 0; i < n; i++) {
        printf("%d ", ver_colors[i]);
    }
    printf("\n");
    free(ver_colors);

    // Free graph memory
    for (int i = 0; i < n; i++) {
        free(graph[i]);
    }
    free(graph);

    // Performance testing
    printf("\nPerformance testing for brute force algorithm:\n");
    test_performance(bruteforce_coloring, 10, 1, 2);

    return 0;
}