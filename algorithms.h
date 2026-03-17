#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>

int nearestNeighbour(const std::vector<std::vector<int>>& graph, int startNode);

int randomSearch(const std::vector<std::vector<int>>& graph, int iterations);

int repetitiveNearestNeighbour(const std::vector<std::vector<int>>& graph);

int bruteForceTSP(const std::vector<std::vector<int>>& graph);

#endif