#include "utils.h"
#include <fstream>
#include <iostream>
#include <random>
#include <cmath>
#include <limits>

using namespace std;

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

Config loadConfig(const string& filename) {
    Config cfg;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Blad: Nie mozna otworzyc pliku konfiguracyjnego: " << filename << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        size_t commentPos = line.find('#');
        if (commentPos != string::npos) line = line.substr(0, commentPos);
        line = trim(line);
        if (line.empty()) continue;
        size_t delimiterPos = line.find('=');
        if (delimiterPos != string::npos) {
            string key = trim(line.substr(0, delimiterPos));
            string value = trim(line.substr(delimiterPos + 1));

            if (key == "algorithm") cfg.algorithm = value;
            else if (key == "repetitions") cfg.repetitions = stoi(value);
            else if (key == "randIterations") cfg.randIterations = stoi(value);
            else if (key == "startInstanceSize") cfg.startInstanceSize = stoi(value);
            else if (key == "instancesCount") cfg.instancesCount = stoi(value);
            else if (key == "inputFile") cfg.inputFile = value;
            else if (key == "outputFile") cfg.outputFile = value;
            else if (key == "showProgress") cfg.showProgress = (value == "1" || value == "true");
            else if (key == "generateRandom") cfg.generateRandom = (value == "1" || value == "true");
            else if (key == "symmetric") cfg.symmetric = (value == "1" || value == "true");
        }
    }
    file.close();
    return cfg;
}

void printProgress(int current, int total, bool show) {
    if (!show) return;
    cout << "\rPostep: " << current << " / " << total << flush;
    if (current == total) cout << endl;
}

double calculateMean(const vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = 0.0;
    for (double v : values) sum += v;
    return sum / values.size();
}

double calculateStdDev(const vector<double>& values, double mean) {
    if (values.empty() || values.size() == 1) return 0.0;
    double sumSq = 0.0;
    for (double v : values) sumSq += (v - mean) * (v - mean);
    return sqrt(sumSq / values.size());
}


double getProcessMemoryKB() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<double>(pmc.WorkingSetSize) / 1024.0;
    }
    return 0.0;

#elif defined(__linux__)
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::string value_str;
            for (char c : line.substr(6)) {
                if (isdigit(c)) value_str += c;
            }
            return std::stod(value_str);
        }
    }
    return 0.0;

#else
    return 0.0;
#endif
}

vector<vector<int>> loadGraphFromFile(const string& filename, int& knownOptimum) {
    ifstream file(filename);
    if (!file.is_open()) { cerr << "Blad: Brak pliku " << filename << endl; exit(1); }

    int n;
    if (!(file >> n)) exit(1);

    vector<vector<int>> graph(n, vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int weight;
            file >> weight;
            graph[i][j] = (weight == -1) ? numeric_limits<int>::max() : weight;
        }
    }

    knownOptimum = 0;
    string token;
    while (file >> token) {
        if (token.find("sum_min=") != string::npos) {
            try {
                knownOptimum = stoi(token.substr(8));
            } catch (...) {
            }
            break;
        }
    }

    file.close();
    return graph;
}

vector<vector<int>> generateRandomGraph(int n, bool symmetric, int minWeight, int maxWeight) {
    vector<vector<int>> graph(n, vector<int>(n, 0));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(minWeight, maxWeight);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) graph[i][j] = numeric_limits<int>::max();
            else if (symmetric && i > j) graph[i][j] = graph[j][i];
            else graph[i][j] = dist(gen);
        }
    }
    return graph;
}