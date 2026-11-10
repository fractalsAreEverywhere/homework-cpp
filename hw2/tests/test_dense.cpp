#include "../hw2/matrix.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>

// Сравнивает размеры и все элементы матриц с допуском для double.
void checkEqual(DenseMatrix& actual, DenseMatrix& expected) {
    assert(actual.rows() == expected.rows());
    assert(actual.cols() == expected.cols());
    for (int i = 0; i < expected.rows(); ++i) {
        for (int j = 0; j < expected.cols(); ++j) {
            assert(std::abs(actual.get(i, j) - expected.get(i, j)) < 1e-9);
        }
    }
}

int main() {
    DenseMatrix a(2, 3);
    a.set(0, 0, 1); a.set(0, 1, 2); a.set(0, 2, 3);
    a.set(1, 0, 4); a.set(1, 1, 5); a.set(1, 2, 6);
    DenseMatrix b(3, 2);
    b.set(0, 0, 7); b.set(0, 1, 8);
    b.set(1, 0, 9); b.set(1, 1, 10);
    b.set(2, 0, 11); b.set(2, 1, 12);

    // Ручной пример: проверяем размеры и все элементы произведения 2x3 на 3x2.
    DenseMatrix expected(2, 2);
    expected.set(0, 0, 58); expected.set(0, 1, 64);
    expected.set(1, 0, 139); expected.set(1, 1, 154);
    DenseMatrix naive = a.mulNaive(b);
    DenseMatrix optimized = a.mulOptimized(b);
    checkEqual(naive, expected);
    checkEqual(optimized, expected);
    std::cout << "Manual rectangular multiplication: passed\n";

    // Умножение на единичную матрицу должно сохранять исходную матрицу.
    DenseMatrix identity(3, 3);
    for (int i = 0; i < 3; ++i) identity.set(i, i, 1.0);
    DenseMatrix identityNaive = a.mulNaive(identity);
    DenseMatrix identityOptimized = a.mulOptimized(identity);
    checkEqual(identityNaive, a);
    checkEqual(identityOptimized, a);
    std::cout << "Identity matrix: passed\n";

    // При умножении на нулевую матрицу каждый элемент результата равен нулю.
    DenseMatrix zero(3, 2);
    DenseMatrix expectedZero(2, 2);
    DenseMatrix zeroNaive = a.mulNaive(zero);
    DenseMatrix zeroOptimized = a.mulOptimized(zero);
    checkEqual(zeroNaive, expectedZero);
    checkEqual(zeroOptimized, expectedZero);
    std::cout << "Zero matrix: passed\n";

    // Оба алгоритма должны явно сообщить об ошибке несовместимых размеров.
    DenseMatrix incompatible(4, 2);
    bool naiveThrew = false;
    bool optimizedThrew = false;
    try { a.mulNaive(incompatible); }
    catch (const std::invalid_argument&) { naiveThrew = true; }
    try { a.mulOptimized(incompatible); }
    catch (const std::invalid_argument&) { optimizedThrew = true; }
    assert(naiveThrew && optimizedThrew);
    std::cout << "Incompatible dimensions: passed\n";

    // Виртуальный mul вызывается через Matrix и возвращает правильный результат.
    Matrix& base = a;
    std::unique_ptr<Matrix> product = base.mul(b);
    for (int i = 0; i < expected.rows(); ++i) {
        for (int j = 0; j < expected.cols(); ++j) {
            assert(std::abs(product->get(i, j) - expected.get(i, j)) < 1e-9);
        }
    }
    bool virtualThrew = false;
    try { base.mul(incompatible); }
    catch (const std::invalid_argument&) { virtualThrew = true; }
    assert(virtualThrew);
    std::cout << "Virtual multiplication: passed\n";

    // Сравниваем два алгоритма на воспроизводимых случайных матрицах разных размеров.
    std::mt19937 generator(42);
    std::uniform_real_distribution<double> distribution(-10.0, 10.0);
    for (int size : {1, 3, 10}) {
        DenseMatrix randomA(size, size + 1);
        DenseMatrix randomB(size + 1, size + 2);
        for (int i = 0; i < randomA.rows(); ++i) {
            for (int j = 0; j < randomA.cols(); ++j) {
                randomA.set(i, j, distribution(generator));
            }
        }
        for (int i = 0; i < randomB.rows(); ++i) {
            for (int j = 0; j < randomB.cols(); ++j) {
                randomB.set(i, j, distribution(generator));
            }
        }
        DenseMatrix randomNaive = randomA.mulNaive(randomB);
        DenseMatrix randomOptimized = randomA.mulOptimized(randomB);
        checkEqual(randomNaive, randomOptimized);
    }
    std::cout << "Random matrices, multiple sizes: passed\n";
    std::cout << "All correctness tests passed.\n";
}
