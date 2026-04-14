#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <deque>

int nearestNeighbour(const std::vector<std::vector<int>>& graph, int startNode);

int randomSearch(const std::vector<std::vector<int>>& graph, int iterations);

int repetitiveNearestNeighbour(const std::vector<std::vector<int>>& graph);

int repetitiveNearestNeighbourNoTies(const std::vector<std::vector<int>>& graph);

int bruteForceTSP(const std::vector<std::vector<int>>& graph);

// Optimized Node structure for Branch and Bound with reference semantics
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

// Branch and Bound implementation
int branchAndBoundBFS(const std::vector<std::vector<int>>& graph, int initialUB);
int branchAndBoundDFS(const std::vector<std::vector<int>>& graph, int initialUB);
int branchAndBoundBEST(const std::vector<std::vector<int>>& graph, int initialUB);

// Helper function for matrix reduction
int reduceMatrix(std::vector<std::vector<int>>& matrix);

#endif