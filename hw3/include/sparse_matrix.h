#pragma once
#include "matrix.h"
#include <cstddef>
#include <map>
#include <utility>

class DenseMatrix;

class SparseMatrix : public Matrix {
    int rows_;
    int cols_;
    std::map<std::pair<int, int>, double> data_;

    void checkIndex(int i, int j) const;

public:
    SparseMatrix(int rows, int cols);

    int rows() const override;

    int cols() const override;

    // Ноль не храним: удаляем запись с этими координатами
    void set(int i, int j, double value);

    double get(int i, int j) const override;

    std::size_t nonZeroCount() const;

    // Читаем ненулевые элементы без копирования map
    const std::map<std::pair<int, int>, double>& entries() const;

    DenseMatrix toDense() const;

    static SparseMatrix fromDense(const DenseMatrix& dense);

    // Ненулевой A[i,k] умножаем на строку k второй матрицы
    DenseMatrix mulDense(const DenseMatrix& other) const;

    // Для A[i,k] нужны только ненулевые элементы строки k из B
    SparseMatrix mulSparse(const SparseMatrix& other) const;

    std::unique_ptr<Matrix> mul(const Matrix& other) const override;
};
