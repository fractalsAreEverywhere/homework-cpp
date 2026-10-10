#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include "my_sort.h"
#include "point.h"

int failedCount = 0;

// Печатает результат одного теста и считает проваленные.
void report(const std::string& name, bool ok) {
    if (ok) {
        std::cout << "[ OK ] " << name << "\n";
    } else {
        std::cout << "[FAIL] " << name << "\n";
        failedCount++;
    }
}

// Сортирует int через mySort и сравнивает с результатом std::sort.
bool checkInts(std::vector<int> data) {
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());

    mySort(data.begin(), data.end());

    return std::is_sorted(data.begin(), data.end()) && data == expected;
}

// Проверяет, что у двух точек совпадают обе координаты.
bool samePoint(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

// Сортирует точки через mySort и сравнивает с эталоном поточечно.
bool checkPoints(std::vector<Point> data) {
    // Эталон — stable_sort: как и mySort, он сохраняет порядок точек с равным расстоянием.
    std::vector<Point> expected = data;
    std::stable_sort(expected.begin(), expected.end(), closerToOrigin);

    mySort(data.begin(), data.end(), closerToOrigin);

    if (!std::is_sorted(data.begin(), data.end(), closerToOrigin)) {
        return false;
    }
    if (data.size() != expected.size()) {
        return false;
    }
    for (size_t i = 0; i < data.size(); i++) {
        if (!samePoint(data[i], expected[i])) {
            return false;
        }
    }
    return true;
}

std::mt19937 gen(42);

// Возвращает n случайных int из [minValue, maxValue].
std::vector<int> randomInts(int n, int minValue, int maxValue) {
    std::uniform_int_distribution<int> dist(minValue, maxValue);
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

// Тесты для vector<int> и обычного массива int[].
void testInts() {
    std::cout << "--- int ---\n";

    report("пустой вектор", checkInts({}));
    report("один элемент", checkInts({42}));

    std::vector<int> sorted = randomInts(1000, -1000, 1000);
    std::sort(sorted.begin(), sorted.end());
    report("уже отсортированный", checkInts(sorted));

    std::vector<int> reversed = sorted;
    std::reverse(reversed.begin(), reversed.end());
    report("отсортированный в обратном порядке", checkInts(reversed));

    report("все элементы равны", checkInts(std::vector<int>(1000, 7)));
    report("отрицательные числа", checkInts({-5, -1, -100, 0, -3, 2, -2}));
    report("много повторов", checkInts(randomInts(1000, 0, 9)));

    report("случайные, n = 10", checkInts(randomInts(10, -1000000, 1000000)));
    report("случайные, n = 1000", checkInts(randomInts(1000, -1000000, 1000000)));
    report("случайные, n = 100000", checkInts(randomInts(100000, -1000000, 1000000)));

    int arr[7] = {5, -2, 9, 0, 9, -7, 3};
    mySort(arr, arr + 7);
    report("обычный массив int[]", std::is_sorted(arr, arr + 7));
}

// Тесты для vector<Point>.
void testPoints() {
    std::cout << "--- Point ---\n";

    report("пустой вектор", checkPoints({}));
    report("одна точка", checkPoints({Point{1.0, 2.0}}));

    std::vector<Point> sorted = randomPoints(1000);
    std::stable_sort(sorted.begin(), sorted.end(), closerToOrigin);
    report("уже отсортированный", checkPoints(sorted));

    std::vector<Point> reversed = sorted;
    std::reverse(reversed.begin(), reversed.end());
    report("отсортированный в обратном порядке", checkPoints(reversed));

    report("все точки равны", checkPoints(std::vector<Point>(1000, Point{3.0, 4.0})));

    report("повторяющиеся расстояния",
           checkPoints({{3, 4}, {1, 1}, {4, 3}, {-3, 4}, {0, 0}, {5, 0}, {1, 1}, {0, -5}}));

    report("случайные, n = 10", checkPoints(randomPoints(10)));
    report("случайные, n = 1000", checkPoints(randomPoints(1000)));
    report("случайные, n = 100000", checkPoints(randomPoints(100000)));
}

// Запускает все тесты; код возврата 1, если хоть один не прошёл.
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    testInts();
    testPoints();

    if (failedCount == 0) {
        std::cout << "\nВсе тесты пройдены.\n";
        return 0;
    }
    std::cout << "\nНе пройдено тестов: " << failedCount << "\n";
    return 1;
}
