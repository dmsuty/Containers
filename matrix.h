#include <iostream>
#include <vector>
#include "biginteger.h"

const static char* BAD_DIVISION = "You can divide only prime fields";
const static char* BAD_ADDITION = "You can't stack matrices of different sizes.";

template<typename Type>
Type operator+ (const Type& first, const Type& second) {
    Type result(first);
    result += second;
    return result;
}

template<typename Type>
Type operator- (const Type& first, const Type& second) {
    Type result(first);
    result -= second;
    return result;
}

template<typename Type>
Type operator* (const Type& first, const Type& second) {
    Type result(first);
    result *= second;
    return result;
}

template<typename Type>
Type operator/ (const Type& first, const Type& second) {
    Type result(first);
    result /= second;
    return result;
}

template<size_t N, size_t K>
struct Is_prime_helper {
    static const bool value = N % K != 0 && Is_prime_helper<N, K - 1>::value;
};

template<size_t N>
struct Is_prime_helper<N, 1> {
    static const bool value = true;
};

template<size_t N>
struct Square_helper {
    static const size_t new_square = Square_helper<N - 1>::square + 1;
    static const size_t square = (new_square * new_square <= N) ? new_square : new_square - 1;
};

template<>
struct Square_helper<1> {
    static const size_t square = 1;
};

template<size_t N>
class Residue {
private:
    size_t value=0;
    static const bool is_prime = Is_prime_helper<N, Square_helper<N>::square>::value;

public:
    Residue() = default;

    explicit Residue(int number): value((N + number % N) % N) {}

    Residue& operator+= (const Residue& other) {
        value += other.value;
        value %= N;
        return *this;
    }

    Residue& operator-= (const Residue& other) {
        value += N - other.value;
        value %= N;
        return *this;
    }

    Residue& operator*= (const Residue& other) {
        value *= other.value;
        value %= N;
        return *this;
    }

    Residue power(size_t power_value) const {
        Residue result(*this);
        if (!power_value) {
            return *this;
        }
        if (power_value % 2 == 0) {
            return power(power_value / 2) * power(power_value / 2);
        }
        return *this * power(power_value - 1);
    }

    Residue& operator/= (const Residue& other) {
        static_assert(is_prime, "You can divide only prime fields");
        *this *= other.power(N - 2);
        return *this;
    }

    explicit operator int() const {
        return value;
    }
};


template<size_t Height, size_t Width, typename Field=Rational>
class Matrix {
private:
    static const size_t height = Height;
    static const size_t width = Width;
    std::vector<std::vector<Field> > matrix(height, width);
public:
    Matrix& operator+= (const Matrix& other) {
        static_assert(height == other.height && width == other.width, "You can't stack matrices of different sizes.");
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                matrix[i][j] += other[i][j];
            }
        }
        return this;
    }

    Matrix& operator-= (const Matrix& other) {
        static_assert(height == other.height && width == other.width, "You can't stack matrices of different sizes.");
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                matrix[i][j] -= other[i][j];
            }
        }
        return *this;
    }

    Matrix& operator*= (const Field& coefficient) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                matrix[i][j] *= coefficient;
            }
        }
        return *this;
    }

    Matrix& operator*= (const Matrix& other) {
        
        return *this;
    }

    std::vector<Field> getRow(size_t index) {
        std::vector<Field> row;
        for (int i = 0; i < width; ++i) {
            row.push_back(matrix[index][i]);
        }
        return row;
    }

    std::vector<Field> getColumn(size_t index) {
        std::vector<Field> column;
        for (int i = 0; i < height; ++i) {
            column.push_back(matrix[i][index]);
        }
        return column;
    }

    const Field* operator[] (size_t index) const {
        return matrix[index];
    }

    Field* operator[] (size_t index) {
        return matrix[index];
    }
};
