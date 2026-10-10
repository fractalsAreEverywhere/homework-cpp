#include <algorithm>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "my_sort.h"
#include "point.h"

const int REPEATS = 5;

// Лучшее время mySort из REPEATS запусков на свежих копиях данных, в мс.
template <typename T, typename Compare>
double timeMySort(const std::vector<T>& original, Compare comp) {
    double best = 1e18;
    for (int r = 0; r < REPEATS; r++) {
        std::vector<T> data = original;

        auto start = std::chrono::steady_clock::now();
        mySort(data.begin(), data.end(), comp);
        auto end = std::chrono::steady_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        best = std::min(best, ms);
    }
    return best;
}

// Лучшее время std::sort из REPEATS запусков на свежих копиях данных, в мс.
template <typename T, typename Compare>
double timeStdSort(const std::vector<T>& original, Compare comp) {
    double best = 1e18;
    for (int r = 0; r < REPEATS; r++) {
        std::vector<T> data = original;

        auto start = std::chrono::steady_clock::now();
        std::sort(data.begin(), data.end(), comp);
        auto end = std::chrono::steady_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        best = std::min(best, ms);
    }
    return best;
}

std::mt19937 gen(42);

// Возвращает n случайных int.
std::vector<int> randomInts(int n) {
    std::uniform_int_distribution<int> dist(-1000000000, 1000000000);
    std::vector<int> v;
    for (int i = 0; i < n; i++) {
        v.push_back(dist(gen));
    }
    return v;
}

// Возвращает n случайных точек в квадрате [-1000, 1000].
std::vector<Point> randomPoints(int n) {
    std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
    std::vector<Point> v;
    for (int i = 0; i < n; i++) {
        v.push_back(Point{dist(gen), dist(gen)});
    }
    return v;
}

// Печатает заголовок таблицы.
void printHeader() {
    std::cout << std::setw(10) << "n"
              << std::setw(16) << "mySort, ms"
              << std::setw(16) << "std::sort, ms"
              << std::setw(20) << "mySort / std::sort" << "\n";
}

// Печатает строку таблицы: размер, оба времени и их отношение.
void printRow(int n, double mine, double standard) {
    std::cout << std::setw(10) << n
              << std::setw(16) << mine
              << std::setw(16) << standard
              << std::setw(20) << mine / standard << "\n";
}

// Замеры на случайных int и Point и на уже отсортированном массиве.
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    std::cout << std::fixed << std::setprecision(3);
    std::vector<int> sizes = {1000, 10000, 100000, 1000000};

    std::cout << "=== int, случайные данные ===\n";
    printHeader();
    for (int n : sizes) {
        std::vector<int> data = randomInts(n);
        printRow(n, timeMySort(data, std::less<int>()), timeStdSort(data, std::less<int>()));
    }

    std::cout << "\n=== Point, случайные данные ===\n";
    printHeader();
    for (int n : sizes) {
        std::vector<Point> data = randomPoints(n);
        printRow(n, timeMySort(data, closerToOrigin), timeStdSort(data, closerToOrigin));
    }

    const int n = 100000;
    std::vector<int> randomData = randomInts(n);
    std::vector<int> sortedData = randomData;
    std::sort(sortedData.begin(), sortedData.end());

    std::cout << "\n=== int, n = " << n << ": случайные против уже отсортированных ===\n";
    std::cout << std::setw(16) << "data"
              << std::setw(16) << "mySort, ms"
              << std::setw(16) << "std::sort, ms" << "\n";
    std::cout << std::setw(16) << "random"
              << std::setw(16) << timeMySort(randomData, std::less<int>())
              << std::setw(16) << timeStdSort(randomData, std::less<int>()) << "\n";
    std::cout << std::setw(16) << "sorted"
              << std::setw(16) << timeMySort(sortedData, std::less<int>())
              << std::setw(16) << timeStdSort(sortedData, std::less<int>()) << "\n";

    return 0;
}
