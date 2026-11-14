#include "dense_matrix.h"
#include "sparse_matrix.h"
#include <stdexcept>

void DenseMatrix::checkIndex(int i, int j) const {
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
        throw std::out_of_range("Matrix index out of range");
    }
}

DenseMatrix::DenseMatrix(int rows, int cols) {
    rows_ = rows;
    cols_ = cols;
    if (rows < 0 || cols < 0) {
        throw std::invalid_argument("Negative matrix size");
    }
    // Приводим тип до умножения, чтобы не переполнить int
    data_.resize(static_cast<std::size_t>(rows) * cols, 0.0);
}

int DenseMatrix::rows() const {
    return rows_;
}

int DenseMatrix::cols() const {
    return cols_;
}

double DenseMatrix::get(int i, int j) const {
    checkIndex(i, j);
    std::size_t index = static_cast<std::size_t>(i) * cols_ + j;
    return data_[index];
}

void DenseMatrix::set(int i, int j, double value) {
    checkIndex(i, j);
    std::size_t index = static_cast<std::size_t>(i) * cols_ + j;
    data_[index] = value;
}

DenseMatrix DenseMatrix::mulNaive(const DenseMatrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Incompatible matrix sizes");
    }
    DenseMatrix result(rows_, other.cols_);
    for (int i = 0; i < rows_; i++) {
        for (int j = 0; j < other.cols_; j++) {
            double sum = 0.0;
            for (int k = 0; k < cols_; k++) {
                sum += get(i, k) * other.get(k, j);
            }
            result.set(i, j, sum);
        }
    }
    return result;
}

DenseMatrix DenseMatrix::mulOptimized(const DenseMatrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Incompatible matrix sizes");
    }
    DenseMatrix result(rows_, other.cols_);
    for (int i = 0; i < rows_; i++) {
        for (int k = 0; k < cols_; k++) {
            std::size_t indexA = static_cast<std::size_t>(i) * cols_ + k;
            double value = data_[indexA];
            for (int j = 0; j < other.cols_; j++) {
                std::size_t indexB = static_cast<std::size_t>(k) * other.cols_ + j;
                std::size_t indexResult = static_cast<std::size_t>(i) * other.cols_ + j;
                result.data_[indexResult] += value * other.data_[indexB];
            }
        }
    }
    return result;
}

DenseMatrix DenseMatrix::mulSparse(const SparseMatrix& other) const {
    if (cols_ != other.rows()) {
        throw std::invalid_argument("Incompatible matrix sizes");
    }
    DenseMatrix result(rows_, other.cols());
    for (const auto& entry : other.entries()) {
        int k = entry.first.first;
        int j = entry.first.second;
        double value = entry.second;
        for (int i = 0; i < rows_; i++) {
            double sum = result.get(i, j) + get(i, k) * value;
            result.set(i, j, sum);
        }
    }
    return result;
}

std::unique_ptr<Matrix> DenseMatrix::mul(const Matrix& other) const {
    const DenseMatrix* dense = dynamic_cast<const DenseMatrix*>(&other);
    if (dense != nullptr) {
        return std::make_unique<DenseMatrix>(mulOptimized(*dense));
    }
    const SparseMatrix* sparse = dynamic_cast<const SparseMatrix*>(&other);
    if (sparse != nullptr) {
        return std::make_unique<DenseMatrix>(mulSparse(*sparse));
    }
    throw std::invalid_argument("Unsupported matrix type");
}
