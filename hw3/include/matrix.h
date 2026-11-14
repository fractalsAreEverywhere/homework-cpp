#pragma once
#include <memory>

class Matrix {
public:
    virtual ~Matrix() = default;
    virtual int rows() const = 0;
    virtual int cols() const = 0;
    virtual double get(int i, int j) const = 0;
    virtual std::unique_ptr<Matrix> mul(const Matrix& other) const = 0;

    void print() const;
};
