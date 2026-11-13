#pragma once
#include "matrix.h"
#include <vector>

class DenseMatrix : public Matrix {
private:
	int rows_;
	int cols_;
	std::vector<double> data_;

public:
	DenseMatrix(int rows, int cols);

	int rows();
	int cols();

	DenseMatrix mulNaive(DenseMatrix& other);

	DenseMatrix mulOptimized(DenseMatrix& other);

	std::unique_ptr<Matrix> mul(Matrix& other) override;

	void print();

	void set(int i, int j, double value);

	double get(int i, int j) override;
};
