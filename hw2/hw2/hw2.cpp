#include "dense_matrix.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
	DenseMatrix e(2, 3);
	e.set(0, 0, 1);
	e.set(0, 1, 2);
	e.set(0, 2, 3);
	e.set(1, 0, 4);
	e.set(1, 1, 5);
	e.set(1, 2, 6);
	DenseMatrix f(3, 2);
	f.set(0, 0, 7);
	f.set(0, 1, 8);
	f.set(1, 0, 9);
	f.set(1, 1, 10);
	f.set(2, 0, 11);
	f.set(2, 1, 12);

	std::cout << "Naive:\n";
	DenseMatrix naive = e.mulNaive(f);
	assert(naive.get(0, 0) == 58);
	assert(naive.get(0, 1) == 64);
	assert(naive.get(1, 0) == 139);
	assert(naive.get(1, 1) == 154);
	naive.print();

	std::cout << "Optimized:\n";
	DenseMatrix optimized = e.mulOptimized(f);
	optimized.print();

	Matrix& first = e;
	std::unique_ptr<Matrix> product = first.mul(f);
	std::cout << "Virtual mul:\n";
	std::cout << product->get(0, 0) << ' ' << product->get(0, 1) << '\n';
	std::cout << product->get(1, 0) << ' ' << product->get(1, 1) << '\n';

	DenseMatrix d(2, 4);
	try {
		first.mul(d);
	}
	catch (const std::invalid_argument& error) {
		std::cout << "Expected error: " << error.what() << '\n';
	}

}
