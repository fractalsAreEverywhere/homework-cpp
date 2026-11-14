#pragma once
#include "matrix.h"
#include <cstddef>
#include <vector>

class SparseMatrix;

class DenseMatrix : public Matrix {
    int rows_;
    int cols_;
    std::vector<double> data_;

    void checkIndex(int i, int j) const;

public:
    DenseMatrix(int rows, int cols);

    int rows() const override;

    int cols() const override;

    double get(int i, int j) const override;

    void set(int i, int j, double value);

    // Обычное умножение i-j-k: считаем одну ячейку целиком
    DenseMatrix mulNaive(const DenseMatrix& other) const;

    // i-k-j: внутренний цикл идёт по соседним элементам строк
    DenseMatrix mulOptimized(const DenseMatrix& other) const;

    DenseMatrix mulSparse(const SparseMatrix& other) const;
    std::unique_ptr<Matrix> mul(const Matrix& other) const override;
};
