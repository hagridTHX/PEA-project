#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// --- Struktura Konfiguracyjna ---
struct Config {
    std::string algorithm = "NN";
    int repetitions = 10;
    int randIterations = 1000;
    bool generateRandom = true;
    bool symmetric = true;
    std::string inputFile = "";
    std::string outputFile = "results.csv";
    int startInstanceSize = 6;
    int instancesCount = 1;
    int optimalValue = 0;
    bool showProgress = true;
};

// --- Funkcje I/O oraz String ---
std::string trim(const std::string& str);
Config loadConfig(const std::string& filename);
void printProgress(int current, int total, bool show);

// --- Generowanie i wczytywanie grafow ---
std::vector<std::vector<int>> loadGraphFromFile(const std::string& filename);
std::vector<std::vector<int>> generateRandomGraph(int n, bool symmetric, int minWeight = 1, int maxWeight = 100);

// --- Statystyka i pamiec ---
double calculateMean(const std::vector<double>& values);
double calculateStdDev(const std::vector<double>& values, double mean);
double calculateMemoryKB(int n);

#endif // UTILS_H