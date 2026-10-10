#pragma once

#include <functional>
#include <iterator>
#include <vector>

// Рекурсивная сортировка слиянием диапазона [first, last) с общим буфером.
template <typename It, typename Compare, typename T>
void mergeSortImpl(It first, It last, Compare comp, std::vector<T>& buffer) {
    auto n = std::distance(first, last);

    // База рекурсии: 0 или 1 элемент уже отсортированы.
    if (n < 2) {
        return;
    }

    // Середина диапазона.
    It mid = std::next(first, n / 2);

    // Сортируем левую и правую половины.
    mergeSortImpl(first, mid, comp, buffer);
    mergeSortImpl(mid, last, comp, buffer);

    // Сливаем две отсортированные половины в буфер.
    buffer.clear();
    It left = first;
    It right = mid;

    while (left != mid && right != last) {
        // При равенстве берём левый элемент, чтобы сортировка была устойчивой.
        if (comp(*right, *left)) {
            buffer.push_back(*right);
            ++right;
        } else {
            buffer.push_back(*left);
            ++left;
        }
    }

    // Дописываем остаток незаконченной половины.
    while (left != mid) {
        buffer.push_back(*left);
        ++left;
    }
    while (right != last) {
        buffer.push_back(*right);
        ++right;
    }

    // Копируем результат из буфера обратно в диапазон.
    It out = first;
    for (const T& value : buffer) {
        *out = value;
        ++out;
    }
}

// Сортирует [first, last) слиянием за O(n·log n) с компаратором comp.
template <typename It,
          typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void mySort(It first, It last, Compare comp = Compare{}) {
    using T = typename std::iterator_traits<It>::value_type;

    std::vector<T> buffer;
    buffer.reserve(std::distance(first, last));

    mergeSortImpl(first, last, comp, buffer);
}
