#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>

#include "algorithms.h"
#include "utils.h"

using namespace std;
using namespace std::chrono;

int main() {
    Config cfg = loadConfig("config.txt");

    const int MAX_BF_SIZE = 14;

    bool isNewFile = false;
    ifstream testFile(cfg.outputFile);
    if (!testFile.good()) {
        isNewFile = true;
    }
    testFile.close();

    ofstream outFile(cfg.outputFile, ios::app);
    if (!outFile.is_open()) {
        cerr << "Blad zapisu do pliku wyjsciowego: " << cfg.outputFile << endl;
        return 1;
    }

    if (isNewFile) {
        outFile << "Algorytm;Rozmiar;Repetycje;Iteracje_RAND;Koszt;Optimum;Blad_wzgledny_%;Czas_us\n";
    }

    cout << "Wybrany algorytm: " << cfg.algorithm << endl;

    int loopStart = cfg.generateRandom ? cfg.startInstanceSize : 0;
    int loopEnd = cfg.generateRandom ? (cfg.startInstanceSize + cfg.instancesCount - 1) : 0;

    for (int currentSize = loopStart; currentSize <= loopEnd; ++currentSize) {
        vector<vector<int>> graph;
        int knownOptimumFromFile = 0;

        if (cfg.generateRandom) {
            graph = generateRandomGraph(currentSize, cfg.symmetric, cfg.minWeight, cfg.maxWeight);
        } else {
            graph = loadGraphFromFile(cfg.inputFile, knownOptimumFromFile);
            currentSize = graph.size();
        }

        cout << "\n" << endl;
        cout << "Rozmiar instancji: " << currentSize << " x " << currentSize << endl;

        int dynamicOptimum = knownOptimumFromFile;

        if (cfg.algorithm == "BF") {
            cout << "Testujemy BF, wiec jego wynik bedzie automatycznie optimum." << endl;
        }
        else if (dynamicOptimum > 0) {
            cout << "Wczytano optimum z pliku: " << dynamicOptimum << endl;
        }
        else if (currentSize <= MAX_BF_SIZE) {
            cout << "Brak optimum w pliku. Wyznaczanie optimum brute forcem. " << flush;
            dynamicOptimum = bruteForceTSP(graph);
            cout << "Koszt optymalny = " << dynamicOptimum << endl;
        } else {
            cout << "Rozmiar > " << MAX_BF_SIZE << " i brak 'sum_min'. Pomijam wyznaczanie optimum." << endl;
        }

         // Determine initial Upper Bound for Branch and Bound
         int initialUB = numeric_limits<int>::max();
         if (cfg.algorithm.find("BB") == 0) {
            cout << "Wyznaczanie poczatkowego Gornego Ograniczenia (UB) - strategia: " << cfg.ubStrategy << endl;

            if (cfg.ubStrategy == "RAND") {
                initialUB = randomSearch(graph, cfg.randIterations);
                cout << "UB z RAND: " << initialUB << endl;
            } else if (cfg.ubStrategy == "NN") {
                initialUB = nearestNeighbour(graph, 0);
                cout << "UB z NN: " << initialUB << endl;
            } else if (cfg.ubStrategy == "RNN_NO_TIES") {
                initialUB = repetitiveNearestNeighbourNoTies(graph);
                cout << "UB z RNN_NO_TIES: " << initialUB << endl;
            } else if (cfg.ubStrategy == "RNN_TIES") {
                initialUB = repetitiveNearestNeighbour(graph);
                cout << "UB z RNN_TIES: " << initialUB << endl;
            } else {
                // "INF" - domyslnie nieskonczonosc
                cout << "UB = INF" << endl;
            }
        }

        cout << "Calkowita zajeta pamiec RAM: " << fixed << setprecision(2) << getProcessMemoryKB() << " [KB]" << endl;

        vector<double> times;
        int lastCost = 0;

        for (int i = 1; i <= cfg.repetitions; ++i) {
            auto start = high_resolution_clock::now();
            int cost = 0;

            if (cfg.algorithm == "NN") cost = nearestNeighbour(graph, 0);
            else if (cfg.algorithm == "RAND") cost = randomSearch(graph, cfg.randIterations);
            else if (cfg.algorithm == "RNN") cost = repetitiveNearestNeighbour(graph);
            else if (cfg.algorithm == "RNN_NO_TIES") cost = repetitiveNearestNeighbourNoTies(graph);
            else if (cfg.algorithm == "BF") cost = bruteForceTSP(graph);
            else if (cfg.algorithm == "BB_BFS") cost = branchAndBoundBFS(graph, initialUB);
            else if (cfg.algorithm == "BB_DFS") cost = branchAndBoundDFS(graph, initialUB);
            else if (cfg.algorithm == "BB_BEST") cost = branchAndBoundBEST(graph, initialUB);
            else {
                cerr << "Nieznany algorytm w pliku konfiguracyjnym" << endl;
                return 1;
            }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);

            times.push_back(duration.count());
            lastCost = cost;

            string optStr = "Brak";
            string errStr = "Brak";

            if (cfg.algorithm == "BF" || cfg.algorithm.find("BB") == 0) {
                optStr = to_string(cost);
                errStr = "0.00";
            } else if (dynamicOptimum > 0) {
                optStr = to_string(dynamicOptimum);
                double error = static_cast<double>(cost - dynamicOptimum) / dynamicOptimum * 100.0;
                ostringstream errorStream;
                errorStream << fixed << setprecision(2) << error;
                errStr = errorStream.str();
            }

            outFile << cfg.algorithm << ";"
                    << currentSize << ";"
                    << cfg.repetitions << ";"
                    << cfg.randIterations << ";"
                    << cost << ";"
                    << optStr << ";"
                    << errStr << ";"
                    << duration.count() << "\n";

            printProgress(i, cfg.repetitions, cfg.showProgress);
        }

        double mean = calculateMean(times);

        cout << "Wynik (" << cfg.algorithm << "): " << lastCost;

        if (cfg.algorithm == "BF" || cfg.algorithm.find("BB") == 0) {
            dynamicOptimum = lastCost;
        }

        if (dynamicOptimum > 0) {
            double error = static_cast<double>(lastCost - dynamicOptimum) / dynamicOptimum * 100.0;
            cout << " (Blad wzgledny: " << fixed << setprecision(2) << error << "%)";
        }
        cout << endl;

        cout << "Sredni czas: " << fixed << setprecision(2) << mean << " [us]" << endl;

        if (!cfg.generateRandom) break;
    }

    outFile.close();
    cout << "\nWyniki zapisano w: " << cfg.outputFile << endl;
    return 0;
}