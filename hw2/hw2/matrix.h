#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>


class Matrix {
public:
	virtual ~Matrix() = default;
	virtual std::unique_ptr<Matrix> mul(Matrix& other) = 0; 
	virtual double get(int i, int j) = 0;
};


class DenseMatrix : public Matrix {
private:
	int rows_;
	int cols_;
	std::vector<double> data_;

public:
	DenseMatrix(int rows, int cols) {


		rows_ = rows;
		cols_ = cols;
		data_.resize(rows_ * cols_, 0.0);
	}
	
	int rows() { return rows_; }
	int cols() { return cols_; }
	
	DenseMatrix mulNaive(DenseMatrix& other) {
		
		if (cols_ != other.rows()) {
			throw std::invalid_argument("Incompatible matrix sizes");
		}

		DenseMatrix result(rows_, other.cols());

		for (int i = 0; i < rows_; i++) {
			for (int j = 0; j < other.cols(); j++) {
				double sum = 0.0;
				for (int k = 0; k < cols_; k++) {
					sum += get(i, k) * other.get(k, j);
				}
				result.set(i, j, sum);
			}
		}


		return result;

	}

	DenseMatrix mulOptimized(DenseMatrix& other) {
		if (cols_ != other.rows()) {
			throw std::invalid_argument("Incompatible matrix sizes");
		}

		DenseMatrix result(rows_, other.cols());

		for (int i = 0; i < rows_; i++) {
			for (int k = 0; k < cols_; k++) {
				double value = get(i, k);

				for (int j = 0; j < other.cols(); j++) {
					
					double sum = result.get(i, j) + value * other.get(k, j);

					result.set(i, j, sum);
				}

			}
		}

		return result;
	}

	std::unique_ptr<Matrix> mul(Matrix& other) override {
		DenseMatrix* dense = dynamic_cast<DenseMatrix*>(&other);

		if (dense == nullptr) {
			throw std::invalid_argument("Unsupported matrix type");
		}

		return std::make_unique<DenseMatrix>(mulOptimized(*dense));
	}

	void print() {
		for (int i = 0; i < rows_; i++) {
			for (int j = 0; j < cols_; j++) {
				int index = i * cols_ + j;
				std::cout << data_[index] << ' ';

			}
			std::cout << '\n';
		}
	}
	
	void set(int i, int j, double value) {
		int index = i * cols_ + j;
		data_[index] = value;

	}
	
	double get(int i, int j) override {
		int index = i * cols_ + j;
		return data_[index];
	}
};


