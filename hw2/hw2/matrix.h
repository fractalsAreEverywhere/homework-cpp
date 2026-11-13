#pragma once
#include <memory>

class Matrix {
public:
	virtual ~Matrix() = default;
	virtual std::unique_ptr<Matrix> mul(Matrix& other) = 0; 
	virtual double get(int i, int j) = 0;
};
