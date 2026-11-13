# ДЗ 2 — умножение матриц

Плотная матрица хранится в одном `std::vector<double>` по строкам. Реализованы два варианта умножения: `i-j-k` и `i-k-j`. Виртуальный `Matrix::mul` сейчас поддерживает умножение двух плотных матриц. При несовместимых размерах выбрасывается `std::invalid_argument`.

## Состав

- `hw2/matrix.h` — абстрактный класс Matrix.
- `hw2/dense_matrix.h` — поля и объявления методов DenseMatrix.
- `hw2/dense_matrix.cpp` — тела методов DenseMatrix.
- `hw2/hw2.cpp` — демонстрационная программа.
- `tests/test_dense.cpp` — проверки корректности.
- `bench/bench_dense.cpp` — измерение производительности.
- `bench/Результаты ДЗ 2.md` — таблица измерений и конфигурация сборки.

## Сборка и тесты в Windows

Откройте терминал разработчика Visual Studio с компилятором MSVC и CMake, затем из папки проекта выполните:

```powershell
cmake -S . -B out/asan-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON
cmake --build out/asan-debug --target hw2_tests
ctest --test-dir out/asan-debug --output-on-failure -V
```

Опция `ENABLE_ASAN` включает AddressSanitizer для тестов при сборке MSVC. Тесты используют `assert`, поэтому запускайте их в Debug: при `NDEBUG` проверки отключаются.

Бенчмарк собирается отдельно в Release без санитайзера:

```powershell
cmake -S . -B out/bench-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=OFF
cmake --build out/bench-release --target hw2_bench
.\out\bench-release\hw2_bench.exe
```

В MSVC конфигурация Release в проверенной сборке использует `/O2`. Для демонстрационной программы можно собрать цель `hw2_demo`.

## Использование ИИ

Основную реализацию матрицы и алгоритмов писал студент с пошаговыми подсказками ИИ. ИИ помог с организацией файлов, CMake, тестами, бенчмарком и документацией. Факт и объём использования ИИ нужно указать в сообщении будущего коммита; каждую часть кода необходимо уметь объяснить на защите.
