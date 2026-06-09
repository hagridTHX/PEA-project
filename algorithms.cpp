#include "algorithms.h"
#include <limits>
#include <numeric>
#include <random>
#include <algorithm>
#include <queue>
#include <stack>
#include <map>
#include <cmath>

#include "utils.h"

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

void rnnPathRecursive(const vector<vector<int>>& graph, vector<bool>& visited,
                      int currentNode, int startNode, int currentCost, int step,
                      vector<int>& currentPath, vector<int>& bestPath, int& bestCost) {
    int n = graph.size();

    if (step == n - 1) {
        if (graph[currentNode][startNode] != numeric_limits<int>::max()) {
            int total = currentCost + graph[currentNode][startNode];
            if (total < bestCost) {
                bestCost = total;
                bestPath = currentPath;
            }
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
        currentPath.push_back(nextNode);
        rnnPathRecursive(graph, visited, nextNode, startNode, currentCost + minCost, step + 1, currentPath, bestPath, bestCost);
        currentPath.pop_back();
        visited[nextNode] = false;
    }
}

vector<int> getRNNPath(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> bestPath;
    int bestCost = numeric_limits<int>::max();

    for (int startNode = 0; startNode < n; ++startNode) {
        vector<bool> visited(n, false);
        vector<int> currentPath;
        visited[startNode] = true;
        currentPath.push_back(startNode);
        rnnPathRecursive(graph, visited, startNode, startNode, 0, 0, currentPath, bestPath, bestCost);
    }

    if (bestPath.empty()) {
        bestPath.resize(n);
        iota(bestPath.begin(), bestPath.end(), 0);
    }
    return bestPath;
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

int reduceMatrix(vector<vector<int>>& matrix) {
    int n = matrix.size();
    int reduction = 0;

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

int branchAndBoundBFS(const vector<vector<int>>& graph, int initialUB) {
    int n = graph.size();
    if (n <= 1) return 0;

    int globalUB = initialUB;
    queue<Node> q;
    queue<vector<vector<int>>> matrices;

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

    int bestCost = initialUB;

    while (!q.empty()) {
        Node current = q.front();
        q.pop();
        vector<vector<int>> currentMatrix = matrices.front();
        matrices.pop();

        if (current.level == n) {
            int lastNode = current.path.back();
            int firstNode = current.path.front();
            if (graph[lastNode][firstNode] != numeric_limits<int>::max()) {
                int actualTotalCost = 0;
                for (size_t i = 0; i < current.path.size() - 1; ++i) {
                    actualTotalCost += graph[current.path[i]][current.path[i+1]];
                }
                actualTotalCost += graph[lastNode][firstNode];

                if (actualTotalCost < bestCost) {
                    bestCost = actualTotalCost;
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

            for (int j = 0; j < n; ++j) {
                childMatrix[currentNode][j] = numeric_limits<int>::max();
                childMatrix[j][nextNode] = numeric_limits<int>::max();
            }
            childMatrix[nextNode][0] = numeric_limits<int>::max();

            int edgeCost = currentMatrix[currentNode][nextNode];
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

int branchAndBoundDFS(const vector<vector<int>>& graph, int initialUB) {
    int n = graph.size();
    if (n <= 1) return 0;

    int globalUB = initialUB;
    stack<Node> s;
    stack<vector<vector<int>>> matrices;

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

    int bestCost = initialUB;

    while (!s.empty()) {
        Node current = s.top();
        s.pop();
        vector<vector<int>> currentMatrix = matrices.top();
        matrices.pop();

        if (current.level == n) {
            int lastNode = current.path.back();
            int firstNode = current.path.front();
            if (graph[lastNode][firstNode] != numeric_limits<int>::max()) {
                int actualTotalCost = 0;
                for (size_t i = 0; i < current.path.size() - 1; ++i) {
                    actualTotalCost += graph[current.path[i]][current.path[i+1]];
                }
                actualTotalCost += graph[lastNode][firstNode];

                if (actualTotalCost < bestCost) {
                    bestCost = actualTotalCost;
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

            for (int j = 0; j < n; ++j) {
                childMatrix[currentNode][j] = numeric_limits<int>::max();
                childMatrix[j][nextNode] = numeric_limits<int>::max();
            }
            childMatrix[nextNode][0] = numeric_limits<int>::max();

            int edgeCost = currentMatrix[currentNode][nextNode];
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

    int bestCost = initialUB;

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        vector<vector<int>> currentMatrix = matrixCache[current.path];
        matrixCache.erase(current.path);

        if (current.level == n) {
            int lastNode = current.path.back();
            int firstNode = current.path.front();
            if (graph[lastNode][firstNode] != numeric_limits<int>::max()) {
                int actualTotalCost = 0;
                for (size_t i = 0; i < current.path.size() - 1; ++i) {
                    actualTotalCost += graph[current.path[i]][current.path[i+1]];
                }
                actualTotalCost += graph[lastNode][firstNode];

                if (actualTotalCost < bestCost) {
                    bestCost = actualTotalCost;
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

            for (int j = 0; j < n; ++j) {
                childMatrix[currentNode][j] = numeric_limits<int>::max();
                childMatrix[j][nextNode] = numeric_limits<int>::max();
            }
            childMatrix[nextNode][0] = numeric_limits<int>::max();

            int edgeCost = currentMatrix[currentNode][nextNode];
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

int calculateMSTLowerBound(const vector<vector<int>>& graph) {
    int n = graph.size();
    if (n == 0) return 0;

    bool symmetric = true;
    for (int i = 0; i < n && symmetric; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (graph[i][j] != graph[j][i]) {
                symmetric = false;
                break;
            }
        }
    }

    if (!symmetric) {
        long long sum = 0;
        for (int v = 0; v < n; ++v) {
            int minIn = numeric_limits<int>::max();
            for (int u = 0; u < n; ++u) {
                if (u == v) continue;
                if (graph[u][v] < minIn) minIn = graph[u][v];
            }
            if (minIn == numeric_limits<int>::max()) return numeric_limits<int>::max();
            sum += minIn;
            if (sum > numeric_limits<int>::max()) return numeric_limits<int>::max();
        }
        return static_cast<int>(sum);
    }

    vector<int> minEdge(n, numeric_limits<int>::max());
    vector<bool> inMST(n, false);
    minEdge[0] = 0;
    long long total = 0;

    for (int i = 0; i < n; ++i) {
        int u = -1;
        for (int v = 0; v < n; ++v) {
            if (!inMST[v] && (u == -1 || minEdge[v] < minEdge[u])) u = v;
        }
        if (u == -1 || minEdge[u] == numeric_limits<int>::max()) return numeric_limits<int>::max();
        inMST[u] = true;
        total += minEdge[u];
        if (total > numeric_limits<int>::max()) return numeric_limits<int>::max();
        for (int v = 0; v < n; ++v) {
            if (!inMST[v]) {
                int w = graph[u][v];
                if (w < minEdge[v]) minEdge[v] = w;
            }
        }
    }

    return static_cast<int>(total);
}

int computePathCostInternal(const vector<vector<int>>& graph, const vector<int>& path) {
    int n = path.size();
    if (n == 0) return 0;
    long long total = 0;
    for (int i = 0; i < n - 1; ++i) {
        int w = graph[path[i]][path[i + 1]];
        if (w == numeric_limits<int>::max()) return numeric_limits<int>::max();
        total += w;
        if (total > numeric_limits<int>::max()) return numeric_limits<int>::max();
    }
    int w = graph[path[n - 1]][path[0]];
    if (w == numeric_limits<int>::max()) return numeric_limits<int>::max();
    total += w;
    if (total > numeric_limits<int>::max()) return numeric_limits<int>::max();
    return static_cast<int>(total);
}

void buildPrefixSums(const vector<vector<int>>& graph, const vector<int>& path,
                     vector<long long>& forwardPrefix, vector<int>& forwardInvalid,
                     vector<long long>& reversePrefix, vector<int>& reverseInvalid) {
    int n = path.size();
    forwardPrefix.assign(n, 0);
    reversePrefix.assign(n, 0);
    forwardInvalid.assign(n, 0);
    reverseInvalid.assign(n, 0);
    for (int i = 0; i < n - 1; ++i) {
        int wF = graph[path[i]][path[i + 1]];
        int wR = graph[path[i + 1]][path[i]];
        forwardPrefix[i + 1] = forwardPrefix[i] + (wF == numeric_limits<int>::max() ? 0 : wF);
        reversePrefix[i + 1] = reversePrefix[i] + (wR == numeric_limits<int>::max() ? 0 : wR);
        forwardInvalid[i + 1] = forwardInvalid[i] + (wF == numeric_limits<int>::max() ? 1 : 0);
        reverseInvalid[i + 1] = reverseInvalid[i] + (wR == numeric_limits<int>::max() ? 1 : 0);
    }
}

long long rangeSum(const vector<long long>& prefix, const vector<int>& invalid, int l, int r) {
    if (l >= r) return 0;
    if (invalid[r] - invalid[l] > 0) return static_cast<long long>(numeric_limits<int>::max());
    return prefix[r] - prefix[l];
}

long long swapDelta(const vector<vector<int>>& graph, const vector<int>& path, int i, int j) {
    int n = path.size();
    if (n < 2) return 0;
    if (i > j) swap(i, j);

    int a = (i - 1 + n) % n;
    int b = i;
    int c = (i + 1) % n;
    int d = (j - 1 + n) % n;
    int e = j;
    int f = (j + 1) % n;

    int ai = path[a], bi = path[b], ci = path[c], di = path[d], ei = path[e], fi = path[f];

    long long oldSum = 0;
    long long newSum = 0;

    if (i + 1 == j) {
        int o1 = graph[ai][bi];
        int o2 = graph[bi][ei];
        int o3 = graph[ei][fi];
        int n1 = graph[ai][ei];
        int n2 = graph[ei][bi];
        int n3 = graph[bi][fi];
        if (o1 == numeric_limits<int>::max() || o2 == numeric_limits<int>::max() || o3 == numeric_limits<int>::max()) return numeric_limits<int>::max();
        if (n1 == numeric_limits<int>::max() || n2 == numeric_limits<int>::max() || n3 == numeric_limits<int>::max()) return numeric_limits<int>::max();
        oldSum = static_cast<long long>(o1) + o2 + o3;
        newSum = static_cast<long long>(n1) + n2 + n3;
    } else if (i == 0 && j == n - 1) {
        int o1 = graph[path[n - 2]][path[n - 1]];
        int o2 = graph[path[n - 1]][path[0]];
        int o3 = graph[path[0]][path[1]];
        int n1 = graph[path[n - 2]][path[0]];
        int n2 = graph[path[0]][path[n - 1]];
        int n3 = graph[path[n - 1]][path[1]];
        if (o1 == numeric_limits<int>::max() || o2 == numeric_limits<int>::max() || o3 == numeric_limits<int>::max()) return numeric_limits<int>::max();
        if (n1 == numeric_limits<int>::max() || n2 == numeric_limits<int>::max() || n3 == numeric_limits<int>::max()) return numeric_limits<int>::max();
        oldSum = static_cast<long long>(o1) + o2 + o3;
        newSum = static_cast<long long>(n1) + n2 + n3;
    } else {
        int o1 = graph[ai][bi];
        int o2 = graph[bi][ci];
        int o3 = graph[di][ei];
        int o4 = graph[ei][fi];
        int n1 = graph[ai][ei];
        int n2 = graph[ei][ci];
        int n3 = graph[di][bi];
        int n4 = graph[bi][fi];
        if (o1 == numeric_limits<int>::max() || o2 == numeric_limits<int>::max() || o3 == numeric_limits<int>::max() || o4 == numeric_limits<int>::max()) return numeric_limits<int>::max();
        if (n1 == numeric_limits<int>::max() || n2 == numeric_limits<int>::max() || n3 == numeric_limits<int>::max() || n4 == numeric_limits<int>::max()) return numeric_limits<int>::max();
        oldSum = static_cast<long long>(o1) + o2 + o3 + o4;
        newSum = static_cast<long long>(n1) + n2 + n3 + n4;
    }

    return newSum - oldSum;
}

long long twoOptDelta(const vector<vector<int>>& graph, const vector<int>& path, int i, int k,
                      const vector<long long>& forwardPrefix, const vector<int>& forwardInvalid,
                      const vector<long long>& reversePrefix, const vector<int>& reverseInvalid) {
    int n = path.size();
    if (n < 2 || i >= k) return 0;
    if (i == 0 && k == n - 1) return numeric_limits<int>::max();

    int a = (i - 1 + n) % n;
    int b = i;
    int c = k;
    int d = (k + 1) % n;

    int old1 = graph[path[a]][path[b]];
    int old2 = graph[path[c]][path[d]];
    int new1 = graph[path[a]][path[c]];
    int new2 = graph[path[b]][path[d]];

    if (old1 == numeric_limits<int>::max() || old2 == numeric_limits<int>::max()) return numeric_limits<int>::max();
    if (new1 == numeric_limits<int>::max() || new2 == numeric_limits<int>::max()) return numeric_limits<int>::max();

    long long forwardSum = rangeSum(forwardPrefix, forwardInvalid, b, c);
    long long reverseSum = rangeSum(reversePrefix, reverseInvalid, b, c);
    if (forwardSum == numeric_limits<int>::max() || reverseSum == numeric_limits<int>::max()) return numeric_limits<int>::max();

    long long delta = -static_cast<long long>(old1) - old2 - forwardSum + new1 + new2 + reverseSum;
    return delta;
}

void applyTwoOpt(vector<int>& path, int i, int k) {
    reverse(path.begin() + i, path.begin() + k + 1);
}

int tabuSearch(const vector<vector<int>>& graph, const vector<int>& initialPath, int lowerBound, const Config& cfg) {
    int n = graph.size();
    if (n == 0) return 0;

    vector<int> currentPath = initialPath;
    if (currentPath.size() != static_cast<size_t>(n)) {
        currentPath.resize(n);
        iota(currentPath.begin(), currentPath.end(), 0);
    }

    int currentCost = computePathCostInternal(graph, currentPath);
    int bestCost = currentCost;
    vector<int> bestPath = currentPath;

    if (bestCost <= lowerBound) return bestCost;
    if (cfg.tsMaxIterations <= 0) return bestCost;

    vector<vector<int>> tabu(n, vector<int>(n, 0));

    for (int iter = 1; iter <= cfg.tsMaxIterations; ++iter) {
        int bestCandidateCost = numeric_limits<int>::max();
        int bestI = -1;
        int bestK = -1;
        bool bestIsSwap = true;

        if (cfg.metaNeighborhood == "TWO_OPT") {
            vector<long long> forwardPrefix, reversePrefix;
            vector<int> forwardInvalid, reverseInvalid;
            buildPrefixSums(graph, currentPath, forwardPrefix, forwardInvalid, reversePrefix, reverseInvalid);
            for (int i = 0; i < n - 1; ++i) {
                for (int k = i + 1; k < n; ++k) {
                    long long delta = twoOptDelta(graph, currentPath, i, k, forwardPrefix, forwardInvalid, reversePrefix, reverseInvalid);
                    if (delta == numeric_limits<int>::max() || currentCost == numeric_limits<int>::max()) continue;
                    long long candidate = static_cast<long long>(currentCost) + delta;
                    if (candidate > numeric_limits<int>::max()) continue;
                    int cityA = currentPath[i];
                    int cityB = currentPath[k];
                    bool isTabu = tabu[cityA][cityB] > iter;
                    if (isTabu && !(cfg.tsAspiration && candidate < bestCost)) continue;
                    if (candidate < bestCandidateCost) {
                        bestCandidateCost = static_cast<int>(candidate);
                        bestI = i;
                        bestK = k;
                        bestIsSwap = false;
                    }
                }
            }
        } else {
            for (int i = 0; i < n - 1; ++i) {
                for (int k = i + 1; k < n; ++k) {
                    long long delta = swapDelta(graph, currentPath, i, k);
                    if (delta == numeric_limits<int>::max() || currentCost == numeric_limits<int>::max()) continue;
                    long long candidate = static_cast<long long>(currentCost) + delta;
                    if (candidate > numeric_limits<int>::max()) continue;
                    int cityA = currentPath[i];
                    int cityB = currentPath[k];
                    bool isTabu = tabu[cityA][cityB] > iter;
                    if (isTabu && !(cfg.tsAspiration && candidate < bestCost)) continue;
                    if (candidate < bestCandidateCost) {
                        bestCandidateCost = static_cast<int>(candidate);
                        bestI = i;
                        bestK = k;
                        bestIsSwap = true;
                    }
                }
            }
        }

        if (bestI == -1 || bestK == -1 || bestCandidateCost == numeric_limits<int>::max()) break;

        int cityA = currentPath[bestI];
        int cityB = currentPath[bestK];

        if (bestIsSwap) {
            swap(currentPath[bestI], currentPath[bestK]);
        } else {
            applyTwoOpt(currentPath, bestI, bestK);
        }

        currentCost = bestCandidateCost;
        tabu[cityA][cityB] = iter + cfg.tsTenure;
        tabu[cityB][cityA] = iter + cfg.tsTenure;

        if (currentCost < bestCost) {
            bestCost = currentCost;
            bestPath = currentPath;
            if (bestCost <= lowerBound) return bestCost;
        }
    }

    return bestCost;
}

int antColonyOptimization(const vector<vector<int>>& graph, const Config& cfg) {
    int n = graph.size();
    if (n <= 1) return 0;

    size_t nn = static_cast<size_t>(n) * n;

    // 1. Inicjalizacja poziomu feromonu (heurystyka: tau0 = m / C_nn - zalecenie Dorigo)
    int nnCost = nearestNeighbour(graph, 0);
    if (nnCost <= 0 || nnCost == numeric_limits<int>::max()) nnCost = 10000;
    double tau0 = static_cast<double>(cfg.acoAnts) / nnCost;

    // Płaskie tablice 1D dla maksymalnego wykorzystania Cache procesora (Cache Locality)
    vector<double> tau(nn, tau0);
    vector<double> etaBeta(nn, 0.0);
    vector<double> choice(nn, 0.0);

    // Prekalkulacja widoczności (eta^beta), żeby nie liczyć potęgi w głównej pętli
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j && graph[i][j] < numeric_limits<int>::max() && graph[i][j] > 0) {
                etaBeta[i * n + j] = pow(1.0 / static_cast<double>(graph[i][j]), cfg.acoBeta);
            }
        }
    }

    int globalBestCost = numeric_limits<int>::max();
    vector<int> globalBestPath;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> startCityDis(0, n - 1);

    bool alphaIsOne = (cfg.acoAlpha == 1.0);
    bool alphaIsTwo = (cfg.acoAlpha == 2.0);

    for (int iter = 0; iter < cfg.acoIterations; ++iter) {
        // 1. Prekalkulacja tablicy decyzyjnej raz na całą iterację (Zabójca funkcji pow!)
        for (size_t idx = 0; idx < nn; ++idx) {
            double t = tau[idx];
            if (!alphaIsOne) {
                if (alphaIsTwo) t = t * t;
                else t = pow(t, cfg.acoAlpha);
            }
            choice[idx] = t * etaBeta[idx];
        }

        vector<vector<int>> antPaths(cfg.acoAnts, vector<int>(n));
        vector<int> antCosts(cfg.acoAnts, 0);

        // 2. Faza konstrukcji - mrówki budują ścieżki
        for (int k = 0; k < cfg.acoAnts; ++k) {
            vector<bool> visited(n, false);
            int startCity = startCityDis(gen);
            antPaths[k][0] = startCity;
            visited[startCity] = true;
            int currentCity = startCity;
            int currentCost = 0;
            bool validAnt = true;

            for (int step = 1; step < n; ++step) {
                const double* row = &choice[currentCity * n];
                double sumProbs = 0.0;

                for (int j = 0; j < n; ++j) {
                    if (!visited[j]) sumProbs += row[j];
                }

                int nextCity = -1;
                // Ruletka prawdopodobieństw
                if (sumProbs > 0.0) {
                    double r = dis(gen) * sumProbs;
                    double cumulative = 0.0;
                    for (int j = 0; j < n; ++j) {
                        if (!visited[j] && row[j] > 0.0) {
                            cumulative += row[j];
                            if (cumulative >= r) {
                                nextCity = j;
                                break;
                            }
                        }
                    }
                }

                // Fallback (jeśli zaokrąglenia zmiennoprzecinkowe zawiodły)
                if (nextCity == -1) {
                    for (int j = 0; j < n; ++j) {
                        if (!visited[j] && graph[currentCity][j] < numeric_limits<int>::max()) {
                            nextCity = j; break;
                        }
                    }
                }

                if (nextCity == -1) { validAnt = false; break; }

                antPaths[k][step] = nextCity;
                visited[nextCity] = true;
                currentCost += graph[currentCity][nextCity];
                currentCity = nextCity;
            }

            // Dodanie powrotu do startu (zamknięcie cyklu)
            if (validAnt && graph[currentCity][startCity] < numeric_limits<int>::max()) {
                currentCost += graph[currentCity][startCity];
                antCosts[k] = currentCost;

                if (currentCost < globalBestCost) {
                    globalBestCost = currentCost;
                    globalBestPath = antPaths[k];
                }
            } else {
                antCosts[k] = numeric_limits<int>::max();
            }
        }

        // 3. Parowanie feromonów (Evaporation)
        for (size_t idx = 0; idx < nn; ++idx) {
            tau[idx] *= (1.0 - cfg.acoRho);
            if (tau[idx] < 1e-10) tau[idx] = 1e-10; // Zabezpieczenie przed wyzerowaniem
        }

        // 4. Depozyt feromonów - CZYSTY CAS (Wszystkie mrówki zostawiają feromon proporcjonalny do jakości trasy)
        for (int k = 0; k < cfg.acoAnts; ++k) {
            if (antCosts[k] < numeric_limits<int>::max()) {
                double deposit = 1.0 / static_cast<double>(antCosts[k]); // Q=1.0, deposit = Q / L
                const vector<int>& p = antPaths[k];
                for (int i = 0; i < n; ++i) {
                    int u = p[i];
                    int v = p[(i + 1) % n];
                    tau[u * n + v] += deposit;
                    if (cfg.symmetric) tau[v * n + u] += deposit; // Dla TSP symetrycznego wzmacniamy w obie strony
                }
            }
        }
    }

    return globalBestCost;
}