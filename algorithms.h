#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>

// 1. Metoda Najblizszego Sasiada (NN)
int nearestNeighbour(const std::vector<std::vector<int>>& graph, int startNode);

// 2. Algorytm Losowy (RAND)
int randomSearch(const std::vector<std::vector<int>>& graph, int iterations);

// 3. Repetitive Nearest Neighbour (RNN) z rozgalezianiem przy rownych wagach
int repetitiveNearestNeighbour(const std::vector<std::vector<int>>& graph);

// 4. Przeglad Zupelny (Brute Force)
int bruteForceTSP(const std::vector<std::vector<int>>& graph);

#endif // ALGORITHMS_H