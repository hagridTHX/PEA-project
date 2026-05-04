#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <deque>

struct Config;

int nearestNeighbour(const std::vector<std::vector<int>>& graph, int startNode);

int randomSearch(const std::vector<std::vector<int>>& graph, int iterations);

int repetitiveNearestNeighbour(const std::vector<std::vector<int>>& graph);

std::vector<int> getRNNPath(const std::vector<std::vector<int>>& graph);

int repetitiveNearestNeighbourNoTies(const std::vector<std::vector<int>>& graph);

int bruteForceTSP(const std::vector<std::vector<int>>& graph);

struct Node {
    std::vector<int> path;
    int lowerBound;
    int level;

    Node(int n) : path(), lowerBound(0), level(0) {}

    Node(const Node& other) : path(other.path), lowerBound(other.lowerBound), level(other.level) {}

    Node& operator=(const Node& other) {
        if (this != &other) {
            path = other.path;
            lowerBound = other.lowerBound;
            level = other.level;
        }
        return *this;
    }
};

int branchAndBoundBFS(const std::vector<std::vector<int>>& graph, int initialUB);
int branchAndBoundDFS(const std::vector<std::vector<int>>& graph, int initialUB);
int branchAndBoundBEST(const std::vector<std::vector<int>>& graph, int initialUB);

int reduceMatrix(std::vector<std::vector<int>>& matrix);

int calculateMSTLowerBound(const std::vector<std::vector<int>>& graph);

int tabuSearch(const std::vector<std::vector<int>>& graph, const std::vector<int>& initialPath, int lowerBound, const Config& cfg);
int simulatedAnnealing(const std::vector<std::vector<int>>& graph, const std::vector<int>& initialPath, int lowerBound, const Config& cfg);

#endif