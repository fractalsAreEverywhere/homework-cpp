# ДЗ 1. Сортировка `int[]` и `vector<Point>`

Алгоритм: сортировка слиянием (merge sort), `O(n·log n)` на любых данных, устойчивая,
дополнительная память O(n).

## Файлы

- `my_sort.h` — шаблонная функция `mySort(first, last, comp)`.
- `point.h` — структура `Point` и компаратор `closerToOrigin` (по расстоянию до начала координат).
- `tests/tests.cpp` — тесты корректности (Debug + `/fsanitize=address`).
- `bench/bench.cpp` — замеры времени против `std::sort` (Release, `/O2`, без санитайзера).

## Сборка и запуск

Из «Developer PowerShell for VS 2022», в корне репозитория:

```
cmake -B build
cmake --build build --config Debug
build\hw1\Debug\hw1_tests.exe

cmake --build build --config Release
build\hw1\Release\hw1_bench.exe
```

## Результаты замеров

```
=== int, случайные данные ===
         n      mySort, ms   std::sort, ms  mySort / std::sort
      1000           0.055           0.016               3.529
     10000           1.013           0.633               1.601
    100000          13.392           8.495               1.576
   1000000         149.899         101.668               1.474

=== Point, случайные данные ===
         n      mySort, ms   std::sort, ms  mySort / std::sort
      1000           0.079           0.101               0.783
     10000           1.469           1.310               1.122
    100000          19.550          17.872               1.094
   1000000         233.308         218.948               1.066

=== int, n = 100000: случайные против уже отсортированных ===
            data      mySort, ms   std::sort, ms
          random          12.494           8.438
          sorted           4.310           1.228
```

## Авторство
Код сортировки, тестов и бенчмарка сгенерирован ИИ (Claude).
Я разобрал весь код, заменил тернарный оператор в report на if-else
для читаемости, запустил тесты и бенчмарк на своём компьютере.
