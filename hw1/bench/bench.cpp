// Тесты производительности: mySort против std::sort.
// Собираются в Release (/O2) БЕЗ санитайзера: он сильно замедляет программу.

#include <algorithm>
#include <chrono>     // замер времени
#include <functional> // std::less
#include <iomanip>    // std::setw, std::fixed, std::setprecision
#include <iostream>
#include <random>
#include <vector>

#ifdef _WIN32
#define NOMINMAX      // иначе windows.h ломает std::min
#include <windows.h>  // SetConsoleOutputCP: чтобы русский текст в консоли Windows не превращался в кракозябры
#endif

#include "my_sort.h"
#include "point.h"

// Каждый замер повторяем несколько раз и берём лучшее время:
// так меньше влияют случайные помехи (другие программы, кэш и т. п.).
const int REPEATS = 5;

// Замер mySort. original передаётся по константной ссылке (без копии).
// Перед каждым замером делаем свежую копию данных: копирование происходит
// ДО запуска таймера и в замер не попадает.
template <typename T, typename Compare>
double timeMySort(const std::vector<T>& original, Compare comp) {
    double best = 1e18;
    for (int r = 0; r < REPEATS; r++) {
        std::vector<T> data = original;  // свежая неотсортированная копия

        auto start = std::chrono::steady_clock::now();
        mySort(data.begin(), data.end(), comp);
        auto end = std::chrono::steady_clock::now();

        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        best = std::min(best, ms);
    }
    return best;
}

// То же самое для std::sort.
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

std::vector<int> randomInts(int n) {
    std::uniform_int_distribution<int> dist(-1000000000, 1000000000);
    std::vector<int> v;
    for (int i = 0; i < n; i++) {
        v.push_back(dist(gen));
    }
    return v;
}

std::vector<Point> randomPoints(int n) {
    std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
    std::vector<Point> v;
    for (int i = 0; i < n; i++) {
        v.push_back(Point{dist(gen), dist(gen)});
    }
    return v;
}

void printHeader() {
    std::cout << std::setw(10) << "n"
              << std::setw(16) << "mySort, ms"
              << std::setw(16) << "std::sort, ms"
              << std::setw(20) << "mySort / std::sort" << "\n";
}

void printRow(int n, double mine, double standard) {
    std::cout << std::setw(10) << n
              << std::setw(16) << mine
              << std::setw(16) << standard
              << std::setw(20) << mine / standard << "\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);  // 65001 = кодировка UTF-8
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

    // Отдельный замер: уже отсортированный массив против случайного того же размера.
    // У quicksort с плохим выбором опорного элемента отсортированный массив
    // даёт O(n^2). У merge sort время должно быть не хуже, чем на случайных данных.
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
