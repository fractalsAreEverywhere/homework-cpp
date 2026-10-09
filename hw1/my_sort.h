#pragma once  // защита от двойного подключения этого файла

#include <functional>  // std::less
#include <iterator>    // std::distance, std::next, std::iterator_traits
#include <vector>      // std::vector (временный буфер для слияния)

// Сортировка слиянием (merge sort).
// Сложность: O(n·log n) всегда, на любых данных (в том числе на уже отсортированных).
// Дополнительная память: один буфер на n элементов.
// Сортировка устойчивая: равные элементы сохраняют исходный порядок.

// Вспомогательная рекурсивная функция. Сортирует диапазон [first, last).
// buffer передаётся ПО ССЫЛКЕ (&), чтобы все рекурсивные вызовы
// пользовались одним и тем же вектором, а не создавали каждый свою копию.
template <typename It, typename Compare, typename T>
void mergeSortImpl(It first, It last, Compare comp, std::vector<T>& buffer) {
    // Сколько элементов в диапазоне.
    auto n = std::distance(first, last);

    // 0 или 1 элемент уже отсортированы. Это база рекурсии.
    if (n < 2) {
        return;
    }

    // Итератор на середину диапазона.
    It mid = std::next(first, n / 2);

    // 1. Сортируем левую половину [first, mid).
    mergeSortImpl(first, mid, comp, buffer);
    // 2. Сортируем правую половину [mid, last).
    mergeSortImpl(mid, last, comp, buffer);

    // 3. Сливаем две отсортированные половины в буфер.
    buffer.clear();
    It left = first;   // текущий элемент левой половины
    It right = mid;    // текущий элемент правой половины

    while (left != mid && right != last) {
        // Берём из правой половины, только если правый элемент СТРОГО меньше.
        // При равенстве берём левый: так равные элементы не меняются местами
        // (это и делает сортировку устойчивой).
        if (comp(*right, *left)) {
            buffer.push_back(*right);
            ++right;
        } else {
            buffer.push_back(*left);
            ++left;
        }
    }

    // Одна из половин закончилась. Дописываем остаток другой.
    while (left != mid) {
        buffer.push_back(*left);
        ++left;
    }
    while (right != last) {
        buffer.push_back(*right);
        ++right;
    }

    // 4. Копируем результат из буфера обратно в исходный диапазон.
    It out = first;
    for (const T& value : buffer) {
        *out = value;
        ++out;
    }
}

// Главная функция. Сигнатура взята из задания.
//   It      — тип итератора (например, std::vector<int>::iterator или int*)
//   Compare — компаратор: comp(a, b) == true, если a должен стоять раньше b.
//             По умолчанию std::less, то есть обычное «a < b».
template <typename It,
          typename Compare = std::less<typename std::iterator_traits<It>::value_type>>
void mySort(It first, It last, Compare comp = Compare{}) {
    // T — тип элементов, на которые указывает итератор (int, Point, ...).
    using T = typename std::iterator_traits<It>::value_type;

    // Один буфер на всю сортировку. reserve сразу выделяет память под n элементов,
    // чтобы push_back внутри слияния не перевыделял её каждый раз.
    std::vector<T> buffer;
    buffer.reserve(std::distance(first, last));

    mergeSortImpl(first, last, comp, buffer);
}
