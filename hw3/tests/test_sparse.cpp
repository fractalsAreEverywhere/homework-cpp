#include "dense_matrix.h"
#include "sparse_matrix.h"
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}
void checkEqual(const Matrix& actual, const Matrix& expected) {
    require(actual.rows() == expected.rows() && actual.cols() == expected.cols(), "Different dimensions");
    for (int i = 0; i < expected.rows(); i++) {
        for (int j = 0; j < expected.cols(); j++) {
            double difference = std::abs(actual.get(i, j) - expected.get(i, j));
            require(difference < 1e-9, "Different elements");
        }
    }
}
// Обычная проверка исключения без шаблонов и лямбд
void checkBadMultiplication(const Matrix& a, const Matrix& b) {
    bool caught = false;
    try {
        a.mul(b);
    }
    catch (const std::invalid_argument&) {
        caught = true;
    }
    require(caught, "Expected dimension error");
}

// Один и тот же пример пропускаем через четыре виртуальных вызова
void checkCombinations(const DenseMatrix& a, const DenseMatrix& b, const Matrix& expected) {
    SparseMatrix sa = SparseMatrix::fromDense(a);
    SparseMatrix sb = SparseMatrix::fromDense(b);
    checkEqual(sa.toDense(), a);
    checkEqual(sb.toDense(), b);
    checkEqual(*a.mul(b), expected);
    checkEqual(*a.mul(sb), expected);
    checkEqual(*sa.mul(b), expected);
    auto sparseProduct = sa.mul(sb);
    checkEqual(*sparseProduct, expected);
    require(dynamic_cast<SparseMatrix*>(sparseProduct.get()) != nullptr, "Sparse x Sparse must return Sparse");
}

void testStorage() {
    // Чтение отсутствующего элемента не добавляет ноль в map
    SparseMatrix s(2, 3);
    require(s.rows() == 2 && s.cols() == 3, "Wrong shape");
    require(s.get(1, 2) == 0 && s.nonZeroCount() == 0, "Read inserted a zero");
    s.set(1, 2, 5);
    s.set(1, 2, -2.5);
    require(s.get(1, 2) == -2.5 && s.nonZeroCount() == 1, "Overwrite failed");
    checkEqual(SparseMatrix::fromDense(s.toDense()), s);
    s.set(1, 2, 0);
    s.set(0, 0, 0);
    require(s.nonZeroCount() == 0, "Zero was stored");
    checkEqual(s.toDense(), DenseMatrix(2, 3));
    std::cout << "Storage and conversions: passed\n";
}

void testManual() {
    // Независимо посчитанный ответ для прямоугольных матриц
    DenseMatrix a(2, 3);
    DenseMatrix b(3, 2);
    DenseMatrix expected(2, 2);
    a.set(0, 0, 2);
    a.set(0, 2, 3);
    a.set(1, 1, -1);
    b.set(0, 1, 4);
    b.set(1, 0, 5);
    b.set(2, 1, 2);
    expected.set(0, 1, 14);
    expected.set(1, 0, -5);
    checkCombinations(a, b, expected);
    // Вклад двух произведений в ячейку взаимно уничтожается
    DenseMatrix x(1, 2);
    DenseMatrix y(2, 1);
    x.set(0, 0, 1.5);
    x.set(0, 1, -1.5);
    y.set(0, 0, 2);
    y.set(1, 0, 2);
    checkCombinations(x, y, DenseMatrix(1, 1));
    auto product = SparseMatrix::fromDense(x).mulSparse(SparseMatrix::fromDense(y));
    require(product.nonZeroCount() == 0, "Cancellation left a stored zero");
    // Отдельно проверяем ненулевой дробный ответ
    DenseMatrix u(1, 1);
    DenseMatrix v(1, 1);
    DenseMatrix fractional(1, 1);
    u.set(0, 0, 1.5);
    v.set(0, 0, 2.5);
    fractional.set(0, 0, 3.75);
    checkCombinations(u, v, fractional);
    std::cout << "Manual, fractional and cancellation examples (all combinations): passed\n";
}

void testIdentityZero() {
    // Проверки с обеих сторон, включая полностью пустые строки и столбцы
    DenseMatrix a(2, 3);
    DenseMatrix leftIdentity(2, 2);
    DenseMatrix rightIdentity(3, 3);
    a.set(0, 1, -2.5);
    a.set(1, 2, 7);
    for (int i = 0; i < 2; i++) {
        leftIdentity.set(i, i, 1);
    }
    for (int i = 0; i < 3; i++) {
        rightIdentity.set(i, i, 1);
    }
    checkCombinations(a, rightIdentity, a);
    checkCombinations(leftIdentity, a, a);
    checkCombinations(a, DenseMatrix(3, 4), DenseMatrix(2, 4));
    checkCombinations(DenseMatrix(4, 2), a, DenseMatrix(4, 3));
    checkCombinations(DenseMatrix(2, 0), DenseMatrix(0, 3), DenseMatrix(2, 3));
    checkCombinations(DenseMatrix(0, 2), a, DenseMatrix(0, 3));
    std::cout << "Identity, zero and empty dimensions (all combinations): passed\n";
}

void testErrors() {
    // Ошибка размеров должна проходить через все четыре ветки диспетчеризации
    DenseMatrix a(2, 3);
    DenseMatrix b(4, 2);
    SparseMatrix sa(2, 3);
    SparseMatrix sb(4, 2);
    checkBadMultiplication(a, b);
    checkBadMultiplication(a, sb);
    checkBadMultiplication(sa, b);
    checkBadMultiplication(sa, sb);
    bool caught1 = false;
    try {
        SparseMatrix bad(-1, 3);
    }
    catch (const std::invalid_argument&) {
        caught1 = true;
    }
    require(caught1, "Expected exception was not thrown");

    bool caught2 = false;
    try {
        DenseMatrix bad(2, -1);
    }
    catch (const std::invalid_argument&) {
        caught2 = true;
    }
    require(caught2, "Expected exception was not thrown");

    bool caught3 = false;
    try {
        sa.get(-1, 0);
    }
    catch (const std::out_of_range&) {
        caught3 = true;
    }
    require(caught3, "Expected exception was not thrown");

    bool caught4 = false;
    try {
        sa.set(2, 0, 1);
    }
    catch (const std::out_of_range&) {
        caught4 = true;
    }
    require(caught4, "Expected exception was not thrown");

    bool caught5 = false;
    try {
        sa.set(0, 3, 1);
    }
    catch (const std::out_of_range&) {
        caught5 = true;
    }
    require(caught5, "Expected exception was not thrown");

    bool caught6 = false;
    try {
        a.get(0, 3);
    }
    catch (const std::out_of_range&) {
        caught6 = true;
    }
    require(caught6, "Expected exception was not thrown");

    bool caught7 = false;
    try {
        a.set(2, 0, 1);
    }
    catch (const std::out_of_range&) {
        caught7 = true;
    }
    require(caught7, "Expected exception was not thrown");

    std::cout << "Invalid dimensions and indices: passed\n";
}

void testRandom() {
    // Главная кросс-проверка: Sparse и смешанные умножения против
    // наивного плотного i-j-k, перенесённого из ДЗ 2
    std::mt19937 generator(42);
    std::uniform_real_distribution<double> value(-2.0, 2.0);
    for (int size : {1, 3, 10, 25}) {
        for (double density : {0.0, 0.01, 0.1, 0.5, 1.0}) {
            std::bernoulli_distribution keep(density);
            for (int repeat = 0; repeat < 3; ++repeat) {
                DenseMatrix a(size, size + 1);
                DenseMatrix b(size + 1, size + 2);
                for (int i = 0; i < a.rows(); i++) {
                    for (int j = 0; j < a.cols(); j++) {
                        if (keep(generator)) {
                            a.set(i, j, value(generator));
                        }
                    }
                }
                for (int i = 0; i < b.rows(); i++) {
                    for (int j = 0; j < b.cols(); j++) {
                        if (keep(generator)) {
                            b.set(i, j, value(generator));
                        }
                    }
                }
                DenseMatrix expected = a.mulNaive(b);
                checkCombinations(a, b, expected);
            }
        }
    }
    std::cout << "Random cross-checks: 60 matrix pairs, all four combinations passed\n";
}

int main() {
    try {
        testStorage();
        testManual();
        testIdentityZero();
        testErrors();
        testRandom();
        std::cout << "All HW3 correctness tests passed.\n";
    } catch (const std::exception& error) {
        std::cerr << "TEST FAILED: " << error.what() << '\n';
        return 1;
    }
}
