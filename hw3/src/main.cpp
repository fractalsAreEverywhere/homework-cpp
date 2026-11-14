#include "dense_matrix.h"
#include "sparse_matrix.h"
#include <iostream>

int main() {
    DenseMatrix a(2, 3);
    a.set(0, 0, 2);
    a.set(0, 2, 3);
    a.set(1, 1, -1);
    DenseMatrix b(3, 2);
    b.set(0, 1, 4);
    b.set(1, 0, 5);
    b.set(2, 1, 2);
    SparseMatrix sa = SparseMatrix::fromDense(a);
    SparseMatrix sb = SparseMatrix::fromDense(b);
    std::cout << "Expected:\n0 14\n-5 0\n";
    std::cout << "Dense x Dense:\n";
    a.mul(b)->print();
    std::cout << "Dense x Sparse:\n";
    a.mul(sb)->print();
    std::cout << "Sparse x Dense:\n";
    sa.mul(b)->print();
    std::cout << "Sparse x Sparse:\n";
    sa.mul(sb)->print();
}
