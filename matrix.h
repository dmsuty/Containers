/*
как делать так, чтобы деления для непростых давало CE
*/

#include <iostream>
#include <vector>
#include "biginteger.h"


template<int N, int K>
class Is_prime_helper {
    //static const bool value = N % K == 0 && 
};


template<size_t N>
class Residue {
private:
    size_t value=0;
 
public:
    Residue() = default; //почему нельзя без этого

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
        if (!power_value) {
            return *this;
        }
        if (power_value % 2 == 0) {
            return power(power_value / 2) * power(power_value / 2); 
        }
        return *this * power(power_value - 1);
    }
 
    Residue& operator/= (const Residue& other) {
        *this *= other.power(N - 2);
        return *this;
    }

    explicit operator int() {
        return value;
    }
};


template<size_t Height, size_t Width, typename Field=Rational>
class Matrix {
private:

public:
    
};
