#include "../hw2/matrix.h"
#include <chrono>
#include <iomanip>
#include <iostream>

int main() {
    std::cout << "Dense matrix multiplication benchmark (seconds)\n";
    std::cout << "Average of 3 runs per algorithm\n";
    std::cout << std::left << std::setw(10) << "Size"
              << std::setw(16) << "Naive"
              << std::setw(16) << "Optimized"
              << "Speedup\n";

    for (int size : {100, 200, 500, 1000}) {
        DenseMatrix a(size, size);
        DenseMatrix b(size, size);

        // Заполнение не входит в замер времени умножения.
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                a.set(i, j, (i + j) % 10 + 1);
                b.set(i, j, (i * 2 + j) % 10 + 1);
            }
        }

        const int repetitions = 3;
        double naiveTotal = 0.0;
        double optimizedTotal = 0.0;
        double checksum = 0.0;
        for (int run = 0; run < repetitions; ++run) {
            auto start = std::chrono::steady_clock::now();
            DenseMatrix result = a.mulNaive(b);
            auto finish = std::chrono::steady_clock::now();
            naiveTotal += std::chrono::duration<double>(finish - start).count();

            auto optimizedStart = std::chrono::steady_clock::now();
            DenseMatrix optimizedResult = a.mulOptimized(b);
            auto optimizedFinish = std::chrono::steady_clock::now();
            optimizedTotal += std::chrono::duration<double>(optimizedFinish - optimizedStart).count();

            // Читаем результаты вне измеряемых участков.
            checksum += result.get(0, 0) + optimizedResult.get(0, 0);
        }
        double naiveAverage = naiveTotal / repetitions;
        double optimizedAverage = optimizedTotal / repetitions;

        std::cout << std::left << std::setw(10) << size
                  << std::setw(16) << naiveAverage
                  << std::setw(16) << optimizedAverage;
        if (optimizedAverage > 0.0) {
            std::cout << naiveAverage / optimizedAverage << "x";
        } else {
            std::cout << "n/a";
        }
        std::cout << "  (checksum: " << checksum << ")\n";
    }
}
