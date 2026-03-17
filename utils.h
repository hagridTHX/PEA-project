#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#endif

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
    bool showProgress = true;
    int minWeight = 1;
    int maxWeight = 100;
};

std::string trim(const std::string& str);
Config loadConfig(const std::string& filename);
void printProgress(int current, int total, bool show);

std::vector<std::vector<int>> loadGraphFromFile(const std::string& filename, int& knownOptimum);
std::vector<std::vector<int>> generateRandomGraph(int n, bool symmetric, int minWeight = 1, int maxWeight = 100);

double calculateMean(const std::vector<double>& values);
double calculateStdDev(const std::vector<double>& values, double mean);
double getProcessMemoryKB();

#endif