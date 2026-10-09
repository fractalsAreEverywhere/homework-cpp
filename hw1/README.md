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
      1000           0.056           0.016               3.570
     10000           0.998           0.619               1.612
    100000          12.344           8.484               1.455
   1000000         146.380         102.059               1.434

=== Point, случайные данные ===
         n      mySort, ms   std::sort, ms  mySort / std::sort
      1000           0.081           0.049               1.651
     10000           1.447           1.302               1.112
    100000          19.759          17.479               1.130
   1000000         232.396         220.623               1.053

=== int, n = 100000: случайные против уже отсортированных ===
            data      mySort, ms   std::sort, ms
          random          12.399           8.367
          sorted           4.366           1.222
```
