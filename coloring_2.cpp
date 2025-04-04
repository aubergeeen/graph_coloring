#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iomanip>
using namespace std;

// генерит тестовую матрицу смежности (p - порог вероятности ребра)
void generateRandomGraph(int n, double p, bool** graph) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            graph[i][j] = false;
    
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if ((rand() / (double)RAND_MAX) < p)
                graph[i][j] = graph[j][i] = true;
}

// жадный алг (+ принимает порядок вершин)
int greedyColoring(int n, bool** graph, int* colors, int* order = nullptr) {
    int temp_order[n];
    if (!order) {
        for (int i = 0; i < n; i++) temp_order[i] = i;
        order = temp_order;
    }

    fill_n(colors, n, -1);
    int max_color = 0;

    for (int idx = 0; idx < n; idx++) {
        int vertex = order[idx];
        bool used[n] = {false};

        for (int neighbor = 0; neighbor < n; neighbor++)
            if (graph[vertex][neighbor] && colors[neighbor] != -1)
                used[colors[neighbor]] = true;

        int color = 0;
        while (color <= max_color && used[color]) color++;
        if (color > max_color) max_color = color;
        colors[vertex] = color;
    }

    return max_color + 1;
}

// оптимизирвоанный жадный
void optimizedGreedy(int n, bool** graph, int* colors) {
    int degrees[n] = {0};
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            degrees[i] += graph[i][j];

    int order[n];
    for (int i = 0; i < n; i++) order[i] = i;
    sort(order, order + n, [&](int a, int b) { return degrees[a] > degrees[b]; }); // сортировка по степени (introsort ~ O(n log n) )

    greedyColoring(n, graph, colors, order);
}

// перебор 
void bruteForceColoring(int n, bool** graph, int* min_colors, int* best_colors) {
    int order[n];
    for (int i = 0; i < n; i++) order[i] = i;
    *min_colors = n;

    do {
        int colors_used = greedyColoring(n, graph, best_colors, order);
        if (colors_used < *min_colors)
            *min_colors = colors_used;
    } while (next_permutation(order, order + n));
}

void testGreedyPerformance(int max_vertices, int step) {
    cout << "\n=== Greedy Algorithm Performance ===\n";
    cout << "Vertices | Time (ms)\n";
    cout << "--------|----------\n";
    
    for (int n = 2; n <= max_vertices; n += step) {
        bool** graph = new bool*[n];
        for (int i = 0; i < n; i++) graph[i] = new bool[n];
        generateRandomGraph(n, 0.5, graph);

        int* colors = new int[n];
        int default_order[n];
        for (int i = 0; i < n; i++) default_order[i] = i;

        clock_t start = clock();
        greedyColoring(n, graph, colors, default_order);
        double elapsed = 1000.0 * (clock() - start) / CLOCKS_PER_SEC;

        cout << setw(7) << n << " | " << fixed << setprecision(2) << elapsed << endl;

        for (int i = 0; i < n; i++) delete[] graph[i];
        delete[] graph;
        delete[] colors;
    }
}

// Тест производительности для оптимизированного жадного алгоритма
void testOptimizedGreedyPerformance(int max_vertices, int step) {
    cout << "\n=== Optimized Greedy Algorithm Performance ===\n";
    cout << "Vertices | Time (ms)\n";
    cout << "--------|----------\n";
    
    for (int n = 2; n <= max_vertices; n += step) {
        bool** graph = new bool*[n];
        for (int i = 0; i < n; i++) graph[i] = new bool[n];
        generateRandomGraph(n, 0.5, graph);

        int* colors = new int[n];

        clock_t start = clock();
        optimizedGreedy(n, graph, colors);
        double elapsed = 1000.0 * (clock() - start) / CLOCKS_PER_SEC;

        cout << setw(7) << n << " | " << fixed << setprecision(2) << elapsed << endl;

        for (int i = 0; i < n; i++) delete[] graph[i];
        delete[] graph;
        delete[] colors;
    }
}

// тест полного перебора 
void testBruteForcePerformance(int max_vertices, int step) {
    cout << "\n=== Brute Force Algorithm Performance ===\n";
    cout << "Vertices | Time (ms) | Colors\n";
    cout << "--------|----------|-------\n";
    
    for (int n = 2; n <= max_vertices; n += step) {
        bool** graph = new bool*[n];
        for (int i = 0; i < n; i++) graph[i] = new bool[n];
        generateRandomGraph(n, 0.5, graph);

        int* colors = new int[n];
        int min_colors;

        clock_t start = clock();
        bruteForceColoring(n, graph, &min_colors, colors);
        double elapsed = 1000.0 * (clock() - start) / CLOCKS_PER_SEC;

        cout << setw(7) << n << " | " << fixed << setprecision(2) << elapsed 
             << " | " << min_colors << endl;

        for (int i = 0; i < n; i++) delete[] graph[i];
        delete[] graph;
        delete[] colors;
    }
}

int main() {
    srand(time(NULL));
    
    testGreedyPerformance(20, 2);         
    testOptimizedGreedyPerformance(20, 2); 
    testBruteForcePerformance(10, 1);      
    
    return 0;
}