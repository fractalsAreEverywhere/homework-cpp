// Тесты корректности для mySort.
// Собираются в Debug с санитайзером адресов (/fsanitize=address).

#include <algorithm>  // std::sort, std::stable_sort, std::is_sorted, std::reverse
#include <iostream>
#include <random>     // std::mt19937 и распределения
#include <string>
#include <vector>

#ifdef _WIN32
#define NOMINMAX      // иначе windows.h ломает std::min
#include <windows.h>  // SetConsoleOutputCP: чтобы русский текст в консоли Windows не превращался в кракозябры
#endif

#include "my_sort.h"
#include "point.h"

int failedCount = 0;  // сколько тестов не прошло

// Печатает результат одного теста.
void report(const std::string& name, bool ok) {
    if (ok) {
        std::cout << "[ OK ] " << name << "\n";
    } else {
        std::cout << "[FAIL] " << name << "\n";
        failedCount++;
    }
}

// ---------- Проверки ----------

// Проверка для int.
// data передаётся ПО ЗНАЧЕНИЮ: функция получает свою копию и спокойно её сортирует.
bool checkInts(std::vector<int> data) {
    // Эталон: та же копия, отсортированная стандартным std::sort.
    std::vector<int> expected = data;
    std::sort(expected.begin(), expected.end());

    mySort(data.begin(), data.end());

    // 1) результат упорядочен;
    // 2) результат совпадает с эталоном поэлементно, то есть это перестановка
    //    исходных данных (ничего не потеряно, не продублировано, не испорчено).
    return std::is_sorted(data.begin(), data.end()) && data == expected;
}

// Две точки совпадают, если совпадают обе координаты.
bool samePoint(const Point& a, const Point& b) {
    return a.x == b.x && a.y == b.y;
}

// Проверка для Point.
bool checkPoints(std::vector<Point> data) {
    // Для точек эталон считаем через std::stable_sort, а не std::sort.
    // У разных точек может быть одинаковое расстояние, например (3, 4) и (4, 3).
    // std::sort вправе расставить такие точки в любом порядке, а stable_sort,
    // как и наш merge sort, сохраняет исходный порядок равных.
    // Поэтому результаты должны совпасть точка в точку.
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

// ---------- Генераторы данных ----------

// Генератор случайных чисел с фиксированным начальным значением 42:
// при каждом запуске получаются одни и те же данные, тест воспроизводим.
std::mt19937 gen(42);

std::vector<int> randomInts(int n, int minValue, int maxValue) {
    std::uniform_int_distribution<int> dist(minValue, maxValue);
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

// ---------- Тесты ----------

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
    // Диапазон 0..9 на 1000 элементов: каждое значение повторяется ~100 раз.
    report("много повторов", checkInts(randomInts(1000, 0, 9)));

    report("случайные, n = 10", checkInts(randomInts(10, -1000000, 1000000)));
    report("случайные, n = 1000", checkInts(randomInts(1000, -1000000, 1000000)));
    report("случайные, n = 100000", checkInts(randomInts(100000, -1000000, 1000000)));

    // Обычный массив int[]: имя массива работает как итератор на первый элемент,
    // а arr + 7 — как итератор «за последним».
    int arr[7] = {5, -2, 9, 0, 9, -7, 3};
    mySort(arr, arr + 7);
    report("обычный массив int[]", std::is_sorted(arr, arr + 7));
}

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

    // Разные точки с одинаковым расстоянием 5 до начала координат, вперемешку с другими.
    report("повторяющиеся расстояния",
           checkPoints({{3, 4}, {1, 1}, {4, 3}, {-3, 4}, {0, 0}, {5, 0}, {1, 1}, {0, -5}}));

    report("случайные, n = 10", checkPoints(randomPoints(10)));
    report("случайные, n = 1000", checkPoints(randomPoints(1000)));
    report("случайные, n = 100000", checkPoints(randomPoints(100000)));
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);  // 65001 = кодировка UTF-8
#endif
    testInts();
    testPoints();

    if (failedCount == 0) {
        std::cout << "\nВсе тесты пройдены.\n";
        return 0;
    }
    std::cout << "\nНе пройдено тестов: " << failedCount << "\n";
    return 1;  // ненулевой код возврата означает ошибку
}
