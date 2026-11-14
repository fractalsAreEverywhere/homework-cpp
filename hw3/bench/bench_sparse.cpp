#include "dense_matrix.h"
#include "sparse_matrix.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

// Заполняем ровно заданное число ячеек случайными ненулевыми значениями
// Создание исходных данных и преобразования не входят в измерение
DenseMatrix generate(int size, double density, std::mt19937& generator) {
    DenseMatrix result(size, size);
    std::vector<int> positions(size * size);
    for (int i = 0; i < size * size; i++) {
        positions[i] = i;
    }
    std::shuffle(positions.begin(), positions.end(), generator);
    std::uniform_real_distribution<double> value(0.5, 1.5);
    int count = static_cast<int>(std::llround(size * size * density));
    for (int index = 0; index < count; ++index) {
        int position = positions[index];
        result.set(position / size, position % size, value(generator));
    }
    return result;
}

// Используем все элементы результата вне замера, а не только одну ячейку
double measure(const Matrix& a, const Matrix& b, double& checksum) {
    auto start = std::chrono::steady_clock::now();
    auto result = a.mul(b);
    auto finish = std::chrono::steady_clock::now();
    for (int i = 0; i < result->rows(); i++) {
        for (int j = 0; j < result->cols(); j++) {
            checksum += result->get(i, j);
        }
    }
    std::chrono::duration<double> elapsed = finish - start;
    return elapsed.count();
}

int main(int argc, char** argv) {
#ifndef NDEBUG
    std::cerr << "Build the benchmark in Release (without AddressSanitizer).\n";
    return 1;
#endif
    try {
        std::ofstream file;
        if (argc > 1) {
            file.open(argv[1]);
            if (!file) {
                throw std::runtime_error("Cannot open results file");
            }
        }
        const int size = 300;
        const int repeats = 5;
        std::mt19937 generator(42);
        double checksum = 0.0;
        const char* line = "+--------------+-----------+-----------+---------------+---------------+--------------+\n";
        std::cout << "\n  HW3 | DENSE VS SPARSE\n";
        std::cout << "  Matrix size: " << size << " x " << size
                  << " | Runs: " << repeats << " | Seed: 42\n";
        std::cout << "  One warm-up per method | Mean time: milliseconds\n\n";
        std::cout << line;
        std::cout << "| " << std::right << std::setw(12) << "Density (%)"
                  << " | " << std::setw(9) << "Nonzero A"
                  << " | " << std::setw(9) << "Nonzero B"
                  << " | " << std::setw(13) << "Dense (ms)"
                  << " | " << std::setw(13) << "Sparse (ms)"
                  << " | " << std::setw(12) << "Dense/Sparse"
                  << " |\n";
        std::cout << line;
        std::cout.flush();
        if (file.is_open()) {
            file << "density_percent,nnz_A,nnz_B,dense_seconds,sparse_seconds,speedup\n";
        }
        bool foundWin = false;
        double firstWinDensity = 0.0;
        for (double density : {0.5, 0.1, 0.01, 0.001}) {
            DenseMatrix a = generate(size, density, generator);
            DenseMatrix b = generate(size, density, generator);
            SparseMatrix sa = SparseMatrix::fromDense(a);
            SparseMatrix sb = SparseMatrix::fromDense(b);

            // Первый вызов прогревает код и память; заодно сверяем результат
            auto expected = a.mul(b);
            auto actual = sa.mul(sb);
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    double difference = std::abs(expected->get(i, j) - actual->get(i, j));
                    if (!(difference < 1e-9)) {
                        throw std::runtime_error("Benchmark result mismatch");
                    }
                }
            }

            double denseTotal = 0.0, sparseTotal = 0.0;
            for (int run = 0; run < repeats; ++run) {
                // Чередуем порядок, чтобы один метод не был всегда первым
                if (run % 2 == 0) {
                    denseTotal += measure(a, b, checksum);
                    sparseTotal += measure(sa, sb, checksum);
                } else {
                    sparseTotal += measure(sa, sb, checksum);
                    denseTotal += measure(a, b, checksum);
                }
            }
            double denseTime = denseTotal / repeats;
            double sparseTime = sparseTotal / repeats;
            if (sparseTime <= 0.0) {
                throw std::runtime_error("Timer resolution too low");
            }
            double speedup = denseTime / sparseTime;
            // В консоли показываем миллисекунды, а в CSV оставляем секунды
            std::cout << "| " << std::fixed << std::setprecision(1)
                      << std::setw(12) << density * 100
                      << " | " << std::setw(9) << sa.nonZeroCount()
                      << " | " << std::setw(9) << sb.nonZeroCount()
                      << " | " << std::setprecision(4) << std::setw(13) << denseTime * 1000
                      << " | " << std::setw(13) << sparseTime * 1000
                      << " | " << std::setprecision(3) << std::setw(12) << speedup
                      << " |\n";
            std::cout.flush();
            if (file.is_open()) {
                file << std::setprecision(10) << density * 100 << ','
                     << sa.nonZeroCount() << ',' << sb.nonZeroCount() << ','
                     << denseTime << ',' << sparseTime << ',' << speedup << '\n';
            }
            if (!foundWin && speedup > 1.0) {
                firstWinDensity = density * 100;
                foundWin = true;
            }
        }
        std::cout << line;
        std::cout << "\n  Dense/Sparse > 1: sparse is faster\n";
        if (foundWin) {
            std::cout << "  First measured Sparse win (descending densities): "
                      << std::fixed << std::setprecision(1) << firstWinDensity << "%\n";
        } else {
            std::cout << "  No Sparse win in the measured range\n";
        }
        std::cout << "  Correctness checks: passed for all densities\n";
        std::cout << "  Checksum: " << std::fixed << std::setprecision(6) << checksum << '\n';
        if (file.is_open()) {
            file.flush();
            if (!file) {
                throw std::runtime_error("Failed to write results");
            }
            std::cout << "  CSV saved: " << argv[1] << " (time in seconds)\n";
        }
        std::cout << "  Benchmark completed\n\n";
    } catch (const std::exception& error) {
        std::cerr << "BENCHMARK FAILED: " << error.what() << '\n';
        return 1;
    }
}
