#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>

#include "algorithms.h"
#include "utils.h"

using namespace std;
using namespace std::chrono;

int main() {
    Config cfg = loadConfig("config.txt");

    // --- USTAWIENIA PROGRAMU ---
    const int MAX_BF_SIZE = 14; // Zmienna sterująca bezpiecznikiem dla Brute Force

    ofstream outFile(cfg.outputFile);
    if (!outFile.is_open()) {
        cerr << "Blad zapisu do pliku wyjsciowego: " << cfg.outputFile << endl;
        return 1;
    }

    outFile << "Algorytm;Rozmiar;Koszt;Czas_us\n";

    cout << "=== SYMULACJA TSP ===" << endl;
    cout << "Wybrany algorytm testowy: " << cfg.algorithm << endl;

    int loopStart = cfg.generateRandom ? cfg.startInstanceSize : 0;
    int loopEnd = cfg.generateRandom ? (cfg.startInstanceSize + cfg.instancesCount - 1) : 0;

    for (int currentSize = loopStart; currentSize <= loopEnd; ++currentSize) {
        vector<vector<int>> graph;

        if (cfg.generateRandom) {
            graph = generateRandomGraph(currentSize, cfg.symmetric);
        } else {
            graph = loadGraphFromFile(cfg.inputFile);
            currentSize = graph.size();
        }

        cout << "\n-----------------------------------" << endl;
        cout << "Rozmiar instancji: " << currentSize << " x " << currentSize << endl;
        cout << "Zajeta pamiec: " << fixed << setprecision(2) << calculateMemoryKB(currentSize) << " [KB]" << endl;

        // --- Automatyczne szukanie optimum ---
        int dynamicOptimum = 0;

        if (cfg.algorithm == "BF") {
            cout << "Testujemy BF, wiec jego wynik bedzie automatycznie optimum." << endl;
        }
        else if (currentSize <= MAX_BF_SIZE) {
            cout << "Wyznaczanie optimum (Brute Force)... " << flush;
            dynamicOptimum = bruteForceTSP(graph);
            cout << "Gotowe! Koszt optymalny = " << dynamicOptimum << endl;
        } else {
            cout << "Uwaga: Rozmiar > " << MAX_BF_SIZE << ". Pomijam wyznaczanie optimum (zbyt dlugi czas dla BF)." << endl;
        }

        vector<double> times;
        int lastCost = 0;

        for (int i = 1; i <= cfg.repetitions; ++i) {
            auto start = high_resolution_clock::now();
            int cost = 0;

            if (cfg.algorithm == "NN") cost = nearestNeighbour(graph, 0);
            else if (cfg.algorithm == "RAND") cost = randomSearch(graph, cfg.randIterations);
            else if (cfg.algorithm == "RNN") cost = repetitiveNearestNeighbour(graph);
            else if (cfg.algorithm == "BF") cost = bruteForceTSP(graph);
            else {
                cerr << "Nieznany algorytm w pliku konfiguracyjnym!" << endl;
                return 1;
            }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start);

            times.push_back(duration.count());
            lastCost = cost;

            outFile << cfg.algorithm << ";" << currentSize << ";" << cost << ";" << duration.count() << "\n";
            printProgress(i, cfg.repetitions, cfg.showProgress);
        }

        // --- Logowanie wynikow na ekran ---
        double mean = calculateMean(times);
        double stdDev = calculateStdDev(times, mean);

        cout << "Wynik (" << cfg.algorithm << "): " << lastCost;

        if (cfg.algorithm == "BF") {
            dynamicOptimum = lastCost; // Jesli to BF, to optimum to po prostu jego wynik
        }

        if (dynamicOptimum > 0) {
            double error = static_cast<double>(lastCost - dynamicOptimum) / dynamicOptimum * 100.0;
            cout << " (Blad wzgledny: " << fixed << setprecision(2) << error << "%)";
        }
        cout << endl;

        cout << "Sredni czas: " << fixed << setprecision(2) << mean << " [us]" << endl;
        cout << "Odchylenie stand.: " << fixed << setprecision(2) << stdDev << " [us]" << endl;

        if (!cfg.generateRandom) break;
    }

    outFile.close();
    cout << "\nZakonczono pomyslnie. Wyniki zapisano w: " << cfg.outputFile << endl;
    return 0;
}