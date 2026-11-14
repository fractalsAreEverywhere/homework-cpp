#include "sparse_matrix.h"
#include "dense_matrix.h"
#include <stdexcept>

void SparseMatrix::checkIndex(int i, int j) const {
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
        throw std::out_of_range("Matrix index out of range");
    }
}

SparseMatrix::SparseMatrix(int rows, int cols) {
    rows_ = rows;
    cols_ = cols;
    if (rows < 0 || cols < 0) {
        throw std::invalid_argument("Negative matrix size");
    }
}

int SparseMatrix::rows() const {
    return rows_;
}

int SparseMatrix::cols() const {
    return cols_;
}

void SparseMatrix::set(int i, int j, double value) {
    checkIndex(i, j);
    if (value == 0.0) {
        data_.erase({i, j});
    }
    else {
        data_[{i, j}] = value;
    }
}

double SparseMatrix::get(int i, int j) const {
    checkIndex(i, j);
    auto it = data_.find({i, j});
    if (it == data_.end()) {
        return 0.0;
    }
    return it->second;
}

std::size_t SparseMatrix::nonZeroCount() const {
    return data_.size();
}

const std::map<std::pair<int, int>, double>& SparseMatrix::entries() const {
    return data_;
}

SparseMatrix SparseMatrix::mulSparse(const SparseMatrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Incompatible matrix sizes");
    }
    SparseMatrix result(rows_, other.cols_);
    for (const auto& left : data_) {
        int i = left.first.first;
        int k = left.first.second;
        // map сортирует координаты по строке, затем по столбцу
        auto right = other.data_.lower_bound({k, 0});
        while (right != other.data_.end() && right->first.first == k) {
            int j = right->first.second;
            double sum = result.get(i, j) + left.second * right->second;
            result.set(i, j, sum);
            right++;
        }
    }
    return result;
}

DenseMatrix SparseMatrix::toDense() const {
    DenseMatrix result(rows_, cols_);
    for (const auto& entry : data_) {
        int i = entry.first.first;
        int j = entry.first.second;
        double value = entry.second;
        result.set(i, j, value);
    }
    return result;
}

SparseMatrix SparseMatrix::fromDense(const DenseMatrix& dense) {
    SparseMatrix result(dense.rows(), dense.cols());
    for (int i = 0; i < dense.rows(); i++) {
        for (int j = 0; j < dense.cols(); j++) {
            double value = dense.get(i, j);
            if (value != 0.0) {
                result.set(i, j, value);
            }
        }
    }
    return result;
}

DenseMatrix SparseMatrix::mulDense(const DenseMatrix& other) const {
    if (cols_ != other.rows()) {
        throw std::invalid_argument("Incompatible matrix sizes");
    }
    DenseMatrix result(rows_, other.cols());
    for (const auto& entry : data_) {
        int i = entry.first.first;
        int k = entry.first.second;
        double value = entry.second;
        for (int j = 0; j < other.cols(); j++) {
            double sum = result.get(i, j) + value * other.get(k, j);
            result.set(i, j, sum);
        }
    }
    return result;
}

std::unique_ptr<Matrix> SparseMatrix::mul(const Matrix& other) const {
    const DenseMatrix* dense = dynamic_cast<const DenseMatrix*>(&other);
    if (dense != nullptr) {
        return std::make_unique<DenseMatrix>(mulDense(*dense));
    }
    const SparseMatrix* sparse = dynamic_cast<const SparseMatrix*>(&other);
    if (sparse != nullptr) {
        return std::make_unique<SparseMatrix>(mulSparse(*sparse));
    }
    throw std::invalid_argument("Unsupported matrix type");
}
