#include "matrix.h"
#include <iostream>

void Matrix::print() const {
    for (int i = 0; i < rows(); i++) {
        for (int j = 0; j < cols(); j++) {
            std::cout << get(i, j) << ' ';
        }
        std::cout << '\n';
    }
}
