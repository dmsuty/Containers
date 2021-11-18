#include <iostream>
#include <string>
#include <vector>

class BigInteger;

bool operator== (const BigInteger& n1, const BigInteger& n2);

bool operator!= (const BigInteger& n1, const BigInteger& n2);

bool operator< (const BigInteger& n1, const BigInteger& n2);

bool operator> (const BigInteger& n1, const BigInteger& n2);

bool operator<= (const BigInteger& n1, const BigInteger& n2);

bool operator>= (const BigInteger& n1, const BigInteger& n2);

int number_of_digits(int n, const int& radix) {
    if (n == 0) {
        return 1;
    }
    n = abs(n);
    int ret = 0;
    int cur_deg = 0;
    int cur_pow = 1; 
    while (cur_pow < n) {
        cur_pow *= radix;
        ++cur_deg;
    }
    return cur_deg;
}

class BigInteger {
private://make it private later!!!!!
    int radix=10;
    std::vector<int> rank;

    void delete_nulls() {
        while (rank.size() && rank.back() == 0) {
            rank.pop_back();
        }
    }

public:
    BigInteger(): rank({0}) {}

    BigInteger(int n): rank(number_of_digits(n, radix)) {
        while (n) {
            rank.push_back(n % radix);
            n /= radix;
        }
    }

    BigInteger& operator+= (const BigInteger& other) {
        while (rank.size() < other.rank.size()) {
            rank.push_back(0);
        }
        int carry = 0;
        for (int i = 0; i < rank.size(); ++i) {
            int other_val = 0;
            if (i < other.rank.size()) {
                other_val = other.rank[i];
            }
            rank[i] += other_val + carry;
            carry = rank[i] / radix;
            rank[i] %= radix; 
        }
        if (carry) {
            rank.push_back(carry);
        }
    }

    BigInteger& operator*= (const BigInteger& other) {
        //some code
    }

    BigInteger& operator-= (const BigInteger& other) {
        //some code
    }

    BigInteger& operator/= (const BigInteger& other) {
        //some code
    }

    BigInteger& operator%= (const BigInteger& other) {
        //some code
    }

    std::string to_String() {
        //some code
    }

    //пробразования в int/bool
    //инкремент/дикремент, постфикс/префикс

    friend std::istream& operator>> (std::istream& in, BigInteger& n);
};

std::ostream& operator<< (std::ostream& out, const BigInteger& n);

std::istream& operator>> (std::istream& in, BigInteger& n);