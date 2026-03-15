#include "algorithms.h"
#include <limits>
#include <numeric>
#include <random>
#include <algorithm>

using namespace std;

// --- 1. Metoda Najblizszego Sasiada (NN) ---
int nearestNeighbour(const vector<vector<int>>& graph, int startNode) {
    int n = graph.size();
    vector<bool> visited(n, false);
    int currentNode = startNode;
    int totalCost = 0;
    visited[currentNode] = true;

    for (int step = 0; step < n - 1; ++step) {
        int nextNode = -1;
        int minCost = numeric_limits<int>::max();

        for (int i = 0; i < n; ++i) {
            if (!visited[i] && graph[currentNode][i] < minCost && currentNode != i) {
                minCost = graph[currentNode][i];
                nextNode = i;
            }
        }

        if (nextNode != -1) {
            visited[nextNode] = true;
            totalCost += minCost;
            currentNode = nextNode;
        }
    }
    if (graph[currentNode][startNode] != numeric_limits<int>::max()) {
        totalCost += graph[currentNode][startNode];
    } else {
        totalCost = numeric_limits<int>::max();
    }
    return totalCost;
}

// --- 2. Algorytm Losowy (RAND) ---
int randomSearch(const vector<vector<int>>& graph, int iterations) {
    int n = graph.size();
    if (n <= 1) return 0;

    vector<int> path(n);
    iota(path.begin(), path.end(), 0);
    int bestCost = numeric_limits<int>::max();

    random_device rd;
    mt19937 g(rd());

    for (int i = 0; i < iterations; ++i) {
        shuffle(path.begin(), path.end(), g);
        int currentCost = 0;
        bool validPath = true;

        for (int j = 0; j < n - 1; ++j) {
            int u = path[j];
            int v = path[j + 1];
            if (graph[u][v] == numeric_limits<int>::max()) {
                validPath = false; break;
            }
            currentCost += graph[u][v];
        }

        if (validPath) {
            int lastNode = path[n - 1], firstNode = path[0];
            if (graph[lastNode][firstNode] == numeric_limits<int>::max()) validPath = false;
            else currentCost += graph[lastNode][firstNode];
        }

        if (validPath && currentCost < bestCost) bestCost = currentCost;
    }
    return bestCost;
}

// --- Funkcja pomocnicza dla RNN (Rekurencyjne rozgalezianie) ---
void rnnRecursive(const vector<vector<int>>& graph, vector<bool>& visited,
                  int currentNode, int startNode, int currentCost, int step, int& bestCost) {
    int n = graph.size();

    // Warunek zakonczenia: odwiedzono wszystkie miasta
    if (step == n - 1) {
        if (graph[currentNode][startNode] != numeric_limits<int>::max()) {
            int total = currentCost + graph[currentNode][startNode];
            if (total < bestCost) bestCost = total;
        }
        return;
    }

    int minCost = numeric_limits<int>::max();
    vector<int> nextNodes; // Przechowuje wierzcholki o najmniejszym koszcie

    for (int i = 0; i < n; ++i) {
        if (!visited[i] && graph[currentNode][i] < numeric_limits<int>::max()) {
            if (graph[currentNode][i] < minCost) {
                minCost = graph[currentNode][i];
                nextNodes.clear();
                nextNodes.push_back(i);
            } else if (graph[currentNode][i] == minCost) {
                // Znaleziono krawedz o takiej samej (minimalnej) wadze - dodajemy do rozgalezienia!
                nextNodes.push_back(i);
            }
        }
    }

    if (minCost == numeric_limits<int>::max()) return; // Slepa uliczka

    // Sprawdzenie wszystkich sciezek o minimalnym koszcie
    for (int nextNode : nextNodes) {
        visited[nextNode] = true;
        rnnRecursive(graph, visited, nextNode, startNode, currentCost + minCost, step + 1, bestCost);
        visited[nextNode] = false; // Backtracking: cofniecie odwiedzenia
    }
}

// --- 3. Repetitive Nearest Neighbour (RNN) ---
int repetitiveNearestNeighbour(const vector<vector<int>>& graph) {
    int n = graph.size();
    int globalBestCost = numeric_limits<int>::max();

    // Uruchomienie z kazdego wierzcholka poczatkowego
    for (int startNode = 0; startNode < n; ++startNode) {
        vector<bool> visited(n, false);
        visited[startNode] = true;
        rnnRecursive(graph, visited, startNode, startNode, 0, 0, globalBestCost);
    }
    return globalBestCost;
}

// --- 4. Przeglad Zupelny (Brute Force) ---
int bruteForceTSP(const vector<vector<int>>& graph) {
    int n = graph.size();
    if (n <= 1) return 0;

    // Permutujemy tylko wierzcholki od 1 do n-1. Wierzcholek 0 jest zablokowany jako start/koniec.
    vector<int> path(n - 1);
    iota(path.begin(), path.end(), 1);

    int minCost = numeric_limits<int>::max();

    do {
        int currentCost = 0;
        int currentNode = 0; // Zawsze startujemy z wierzcholka 0
        bool valid = true;

        for (int nextNode : path) {
            if (graph[currentNode][nextNode] == numeric_limits<int>::max()) {
                valid = false; break;
            }
            currentCost += graph[currentNode][nextNode];
            currentNode = nextNode;
        }

        // Dodanie kosztu powrotu do punktu startowego (0)
        if (valid && graph[currentNode][0] != numeric_limits<int>::max()) {
            currentCost += graph[currentNode][0];
            if (currentCost < minCost) {
                minCost = currentCost;
            }
        }
    } while (next_permutation(path.begin(), path.end()));

    return minCost;
}