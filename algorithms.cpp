#include "algorithms.h"
#include <limits>
#include <numeric>
#include <random>
#include <algorithm>
#include <queue>
#include <stack>
#include <map>

using namespace std;

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

int randomSearch(const vector<vector<int>>& graph, int iterations) {
    int n = graph.size();
    if (n <= 1) return 0;

    vector<int> path(n);
    for(int i = 0; i < n; i++) {
        path[i] = i;
    }
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

void rnnRecursive(const vector<vector<int>>& graph, vector<bool>& visited,
                  int currentNode, int startNode, int currentCost, int step, int& bestCost) {
    int n = graph.size();

    if (step == n - 1) {
        if (graph[currentNode][startNode] != numeric_limits<int>::max()) {
            int total = currentCost + graph[currentNode][startNode];
            if (total < bestCost) bestCost = total;
        }
        return;
    }

    int minCost = numeric_limits<int>::max();
    vector<int> nextNodes;

    for (int i = 0; i < n; ++i) {
        if (!visited[i] && graph[currentNode][i] < numeric_limits<int>::max()) {
            if (graph[currentNode][i] < minCost) {
                minCost = graph[currentNode][i];
                nextNodes.clear();
                nextNodes.push_back(i);
            } else if (graph[currentNode][i] == minCost) {
                nextNodes.push_back(i);
            }
        }
    }

    if (minCost == numeric_limits<int>::max()) return;

    for (int nextNode : nextNodes) {
        visited[nextNode] = true;
        rnnRecursive(graph, visited, nextNode, startNode, currentCost + minCost, step + 1, bestCost);
        visited[nextNode] = false;
    }
}

int repetitiveNearestNeighbour(const vector<vector<int>>& graph) {
    int n = graph.size();
    int globalBestCost = numeric_limits<int>::max();

    for (int startNode = 0; startNode < n; ++startNode) {
        vector<bool> visited(n, false);
        visited[startNode] = true;
        rnnRecursive(graph, visited, startNode, startNode, 0, 0, globalBestCost);
    }
    return globalBestCost;
}

int bruteForceTSP(const vector<vector<int>>& graph) {
     int n = graph.size();
     if (n <= 1) return 0;

     vector<int> path(n - 1);
     iota(path.begin(), path.end(), 1);

     int minCost = numeric_limits<int>::max();

     do {
         int currentCost = 0;
         int currentNode = 0;
         bool valid = true;

         for (int nextNode : path) {
             if (graph[currentNode][nextNode] == numeric_limits<int>::max()) {
                 valid = false; break;
             }
             currentCost += graph[currentNode][nextNode];
             currentNode = nextNode;
         }

         if (valid && graph[currentNode][0] != numeric_limits<int>::max()) {
             currentCost += graph[currentNode][0];
             if (currentCost < minCost) {
                 minCost = currentCost;
             }
         }
     } while (next_permutation(path.begin(), path.end()));

     return minCost;
 }

int repetitiveNearestNeighbourNoTies(const vector<vector<int>>& graph) {
     int n = graph.size();
     int globalBestCost = numeric_limits<int>::max();

     for (int startNode = 0; startNode < n; ++startNode) {
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
             if (totalCost < globalBestCost) {
                 globalBestCost = totalCost;
             }
         }
     }

     return globalBestCost;
 }

// Helper function for matrix reduction (Little's Algorithm)
int reduceMatrix(vector<vector<int>>& matrix) {
    int n = matrix.size();
    int reduction = 0;

    // Row reduction
    for (int i = 0; i < n; ++i) {
        int minRow = numeric_limits<int>::max();
        for (int j = 0; j < n; ++j) {
            if (matrix[i][j] < minRow && matrix[i][j] != numeric_limits<int>::max()) {
                minRow = matrix[i][j];
            }
        }
        if (minRow != numeric_limits<int>::max() && minRow > 0) {
            reduction += minRow;
            for (int j = 0; j < n; ++j) {
                if (matrix[i][j] != numeric_limits<int>::max()) {
                    matrix[i][j] -= minRow;
                }
            }
        }
    }

    // Column reduction
    for (int j = 0; j < n; ++j) {
        int minCol = numeric_limits<int>::max();
        for (int i = 0; i < n; ++i) {
            if (matrix[i][j] < minCol && matrix[i][j] != numeric_limits<int>::max()) {
                minCol = matrix[i][j];
            }
        }
        if (minCol != numeric_limits<int>::max() && minCol > 0) {
            reduction += minCol;
            for (int i = 0; i < n; ++i) {
                if (matrix[i][j] != numeric_limits<int>::max()) {
                    matrix[i][j] -= minCol;
                }
            }
        }
    }

    return reduction;
}

// BFS implementation of Branch and Bound
int branchAndBoundBFS(const vector<vector<int>>& graph, int initialUB) {
    int n = graph.size();
    if (n <= 1) return 0;

    int globalUB = initialUB;
    queue<Node> q;
    queue<vector<vector<int>>> matrices;

    // Initial node setup
    Node root(n);
    vector<vector<int>> rootMatrix(n, vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            rootMatrix[i][j] = (graph[i][j] == numeric_limits<int>::max()) ? numeric_limits<int>::max() : graph[i][j];
        }
        rootMatrix[i][i] = numeric_limits<int>::max();
    }

    root.path.push_back(0);
    root.lowerBound = reduceMatrix(rootMatrix);
    root.level = 1;

    if (root.lowerBound < globalUB) {
        q.push(root);
        matrices.push(rootMatrix);
    }

    int bestCost = numeric_limits<int>::max();

    while (!q.empty()) {
        Node current = q.front();
        q.pop();
        vector<vector<int>> currentMatrix = matrices.front();
        matrices.pop();

        if (current.level == n) {
            int lastNode = current.path.back();
            int firstNode = current.path.front();
            if (graph[lastNode][firstNode] != numeric_limits<int>::max()) {
                int totalCost = current.lowerBound + graph[lastNode][firstNode];
                if (totalCost < bestCost) {
                    bestCost = totalCost;
                    globalUB = bestCost;
                }
            }
            continue;
        }

        int currentNode = current.path.back();
        vector<bool> visited(n, false);
        for (int v : current.path) visited[v] = true;

        for (int nextNode = 0; nextNode < n; ++nextNode) {
            if (visited[nextNode] || graph[currentNode][nextNode] == numeric_limits<int>::max()) {
                continue;
            }

            Node child = current;
            child.path.push_back(nextNode);
            child.level++;

            vector<vector<int>> childMatrix = currentMatrix;

            // Set infinity for used edges
            for (int j = 0; j < n; ++j) {
                childMatrix[currentNode][j] = numeric_limits<int>::max();
                childMatrix[j][nextNode] = numeric_limits<int>::max();
            }
            childMatrix[nextNode][0] = numeric_limits<int>::max();

            int edgeCost = graph[currentNode][nextNode];
            int reduction = reduceMatrix(childMatrix);
            child.lowerBound = current.lowerBound + edgeCost + reduction;

            if (child.lowerBound < globalUB) {
                q.push(child);
                matrices.push(childMatrix);
            }
        }
    }

    return bestCost;
}

// DFS implementation of Branch and Bound
int branchAndBoundDFS(const vector<vector<int>>& graph, int initialUB) {
    int n = graph.size();
    if (n <= 1) return 0;

    int globalUB = initialUB;
    stack<Node> s;
    stack<vector<vector<int>>> matrices;

    // Initial node setup
    Node root(n);
    vector<vector<int>> rootMatrix(n, vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            rootMatrix[i][j] = (graph[i][j] == numeric_limits<int>::max()) ? numeric_limits<int>::max() : graph[i][j];
        }
        rootMatrix[i][i] = numeric_limits<int>::max();
    }

    root.path.push_back(0);
    root.lowerBound = reduceMatrix(rootMatrix);
    root.level = 1;

    if (root.lowerBound < globalUB) {
        s.push(root);
        matrices.push(rootMatrix);
    }

    int bestCost = numeric_limits<int>::max();

    while (!s.empty()) {
        Node current = s.top();
        s.pop();
        vector<vector<int>> currentMatrix = matrices.top();
        matrices.pop();

        if (current.level == n) {
            int lastNode = current.path.back();
            int firstNode = current.path.front();
            if (graph[lastNode][firstNode] != numeric_limits<int>::max()) {
                int totalCost = current.lowerBound + graph[lastNode][firstNode];
                if (totalCost < bestCost) {
                    bestCost = totalCost;
                    globalUB = bestCost;
                }
            }
            continue;
        }

        int currentNode = current.path.back();
        vector<bool> visited(n, false);
        for (int v : current.path) visited[v] = true;

        for (int nextNode = n - 1; nextNode >= 0; --nextNode) {
            if (visited[nextNode] || graph[currentNode][nextNode] == numeric_limits<int>::max()) {
                continue;
            }

            Node child = current;
            child.path.push_back(nextNode);
            child.level++;

            vector<vector<int>> childMatrix = currentMatrix;

            // Set infinity for used edges
            for (int j = 0; j < n; ++j) {
                childMatrix[currentNode][j] = numeric_limits<int>::max();
                childMatrix[j][nextNode] = numeric_limits<int>::max();
            }
            childMatrix[nextNode][0] = numeric_limits<int>::max();

            int edgeCost = graph[currentNode][nextNode];
            int reduction = reduceMatrix(childMatrix);
            child.lowerBound = current.lowerBound + edgeCost + reduction;

            if (child.lowerBound < globalUB) {
                s.push(child);
                matrices.push(childMatrix);
            }
        }
    }

    return bestCost;
}

// Best-First (Priority Queue) implementation of Branch and Bound
struct CompareNode {
    bool operator()(const Node& a, const Node& b) const {
        return a.lowerBound > b.lowerBound;
    }
};

int branchAndBoundBEST(const vector<vector<int>>& graph, int initialUB) {
    int n = graph.size();
    if (n <= 1) return 0;

    int globalUB = initialUB;
    priority_queue<Node, vector<Node>, CompareNode> pq;
    map<vector<int>, vector<vector<int>>> matrixCache;

    // Initial node setup
    Node root(n);
    vector<vector<int>> rootMatrix(n, vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            rootMatrix[i][j] = (graph[i][j] == numeric_limits<int>::max()) ? numeric_limits<int>::max() : graph[i][j];
        }
        rootMatrix[i][i] = numeric_limits<int>::max();
    }

    root.path.push_back(0);
    root.lowerBound = reduceMatrix(rootMatrix);
    root.level = 1;

    if (root.lowerBound < globalUB) {
        pq.push(root);
        matrixCache[root.path] = rootMatrix;
    }

    int bestCost = numeric_limits<int>::max();

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        vector<vector<int>> currentMatrix = matrixCache[current.path];
        matrixCache.erase(current.path);

        if (current.level == n) {
            int lastNode = current.path.back();
            int firstNode = current.path.front();
            if (graph[lastNode][firstNode] != numeric_limits<int>::max()) {
                int totalCost = current.lowerBound + graph[lastNode][firstNode];
                if (totalCost < bestCost) {
                    bestCost = totalCost;
                    globalUB = bestCost;
                }
            }
            continue;
        }

        int currentNode = current.path.back();
        vector<bool> visited(n, false);
        for (int v : current.path) visited[v] = true;

        for (int nextNode = 0; nextNode < n; ++nextNode) {
            if (visited[nextNode] || graph[currentNode][nextNode] == numeric_limits<int>::max()) {
                continue;
            }

            Node child = current;
            child.path.push_back(nextNode);
            child.level++;

            vector<vector<int>> childMatrix = currentMatrix;

            // Set infinity for used edges
            for (int j = 0; j < n; ++j) {
                childMatrix[currentNode][j] = numeric_limits<int>::max();
                childMatrix[j][nextNode] = numeric_limits<int>::max();
            }
            childMatrix[nextNode][0] = numeric_limits<int>::max();

            int edgeCost = graph[currentNode][nextNode];
            int reduction = reduceMatrix(childMatrix);
            child.lowerBound = current.lowerBound + edgeCost + reduction;

            if (child.lowerBound < globalUB) {
                pq.push(child);
                matrixCache[child.path] = childMatrix;
            }
        }
    }

    return bestCost;
}