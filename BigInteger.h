#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

class BigInteger;

bool operator< (const BigInteger& n1, const BigInteger& n2);

bool operator== (const BigInteger& n1, const BigInteger& n2);

bool operator> (const BigInteger& n1, const BigInteger& n2);

bool operator!= (const BigInteger& n1, const BigInteger& n2);

bool operator<= (const BigInteger& n1, const BigInteger& n2);

bool operator>= (const BigInteger& n1, const BigInteger& n2);

BigInteger operator+ (const BigInteger& n1, const BigInteger& n2);

BigInteger operator- (const BigInteger& n1, const BigInteger& n2);

BigInteger operator* (const BigInteger& n1, const BigInteger& n2);

BigInteger operator/ (const BigInteger& n1, const BigInteger& n2);

BigInteger operator% (const BigInteger& n1, const BigInteger& n2);

std::ostream& operator<< (std::ostream& out, const BigInteger& n);

int number_of_digits(int n, const int& radix) {
    if (n == 0) {
        return 1;
    }
    n = abs(n);
    int cur_deg = 0;
    int cur_pow = 1; 
    while (cur_pow <= n) {
        cur_pow *= radix;
        ++cur_deg;
    }
    return cur_deg;
}

int sign_of_number(const int& n) {
    if (n >= 0) return 1;
    return -1;
}

class BigInteger {

private:    
    static const int power_of_radix=1;
    static const int radix=10; //must be a power of ten.
    std::vector<int> rank;

    void delete_nulls() {
        while (rank.size() && rank.back() == 0) {
            rank.pop_back();
        }
    }

    void carryover() {
        int carry = 0;
        for (size_t i = 0; i < rank.size(); ++i) {
            int new_carry = (rank[i] + carry) / radix;
            rank[i] = (rank[i] + carry) % radix;
            carry = new_carry;
        }
        while (carry) {
            rank.push_back(carry % radix);
            carry /= radix;
        }
    }

    void reconstructor() {
        carryover();
        delete_nulls();        
        int sign = 1;
        if (rank.size() && rank.back() < 0) {
            sign = -1;
        }
        size_t position = 0;
        while (position < rank.size()) {
            if (sign_of_number(rank[position]) == sign || rank[position] == 0) {
                ++position;
                continue;
            }
            rank[position] += 10 * sign;
            ++position;
            while (sign_of_number(rank[position]) != sign || rank[position] == 0) {
                rank[position] += 9 * sign;
                ++position;
            }
            rank[position] -= 1 * sign;
            ++position;
        }
        delete_nulls();
    }

public:
    BigInteger() = default;

    BigInteger(int n) {
        while (n) {
            rank.push_back(n % radix);
            n /= radix;
        }
    }

    BigInteger(const BigInteger& other) = default;

    ~BigInteger() = default;

    int sign() const {
        if (rank.size() == 0) {
            return 1;
        }
        return sign_of_number(rank.back());
    }
    
    int operator[] (const size_t& k) const {
        if (rank.size() <= k) {
            return 0;
        }
        return rank[k];
    }

    size_t size() const {
        return rank.size();
    }

    BigInteger& operator+= (const BigInteger& other) {
        while (rank.size() < other.size()) {
            rank.push_back(0);
        }
        for (size_t i = 0; i < rank.size(); ++i) {
            rank[i] += other[i];
        }
        reconstructor();
        return *this;
    }

    BigInteger& operator*= (const BigInteger& other) {
        int self_size = rank.size();
        while ((int)rank.size() < (int)self_size + (int)other.size() - 1) {
            rank.push_back(0);
        }
        for (int i = rank.size() - 1; i >= 0; --i) {
            int start_value = rank[i];
            for (int j1 = 0; j1 <= i; ++j1) {
                int j2 = i - j1;
                rank[i] += rank[j1] * other[j2];
            }
            rank[i] -= start_value;
        }
        reconstructor();

        return *this;
    }

    BigInteger operator-() const {
        return BigInteger(*this) * -1;
    }

    BigInteger& operator-= (const BigInteger& other) {
        *this *= -1;
        *this += other;
        *this *= -1;
        return *this;
    }

    BigInteger& operator/= (const BigInteger& divider) {
        int divider_sign = divider.sign();
        int self_sign = sign();
        *this *= self_sign;
        BigInteger quotient(0);
        BigInteger cur_dividend(0);
        for (size_t i = rank.size(); i + 1 != 0; --i) {         
            int cur_quotient = 0;
            BigInteger subtrahend = 0;
            while (subtrahend + divider * divider_sign <= cur_dividend) {
                ++cur_quotient;
                subtrahend += divider * divider_sign;
            }
            quotient = quotient * radix + cur_quotient;
            cur_dividend -= subtrahend;
            if (i != 0) {
                cur_dividend = cur_dividend * radix + rank[i - 1];
            }
        }
        *this = quotient * divider_sign * self_sign;
        return *this;
    }

    BigInteger& operator%= (const BigInteger& divider) {
        int divider_sign = divider.sign();
        int self_sign = sign();
        *this *= self_sign;
        BigInteger quotient(0);
        BigInteger cur_dividend(0);
        for (size_t i = rank.size(); i + 1 != 0; --i) {         
            int cur_quotient = 0;
            BigInteger subtrahend = 0;
            while (subtrahend + divider * divider_sign <= cur_dividend) {
                ++cur_quotient;
                subtrahend += divider * divider_sign;
            }
            cur_dividend -= subtrahend;
            if (i != 0) {
                cur_dividend = cur_dividend * radix + rank[i - 1];
            }
        }
        *this = cur_dividend * self_sign;
        return *this;
    }

    BigInteger& operator++() {
        size_t current_rank = 0;
        if (sign() == 1) {
            while (current_rank < rank.size() && rank[current_rank] == radix - 1) {
                rank[current_rank] = 0;
                ++current_rank;
            }
        } else {
            while (current_rank < rank.size() && rank[current_rank] == 0) {
                rank[current_rank] = -radix + 1;
                ++current_rank;
            }
        }
        if (current_rank == rank.size()) {
            rank.push_back(0);
        }
        ++rank[current_rank];
        return *this;
    }

    BigInteger operator ++(int) {
        BigInteger result(*this);
        ++(*this);
        return result;
    }

    BigInteger& operator--() {
        size_t current_rank = 0;
        if (sign() == 1) {
            while (current_rank < rank.size() && rank[current_rank] == 0) {
                rank[current_rank] = radix - 1;
                ++current_rank;
            }
        } else {
            while (current_rank < rank.size() && rank[current_rank] == -radix + 1) {
                rank[current_rank] = 0;
                ++current_rank;
            }
        }
        if (current_rank == rank.size()) {
            rank.push_back(0);
        }
        --rank[current_rank];
        return *this;
    }

    BigInteger operator --(int) {
        BigInteger result(*this);
        --(*this);
        return result;
    }

    std::string toString() const {
        std::string result;
        if (rank.size() == 0) {
            return "0";
        }
        if (rank.back() < 0) {
            result = "-";
        }
        for (size_t i = rank.size() - 1; i + 1 != 0; --i) {
            if (i != rank.size() - 1) {
                result += std::string(power_of_radix - number_of_digits(rank[i], 10), '0');
            }
            result += std::to_string(abs(rank[i]));
            if (i == 0) break;
        }
        return result;
    }

    explicit operator bool() const {
         return rank.size() == 0;
    }

    void show_each_rank() const {
        for (int i = rank.size() - 1; i >= 0; i--) {
            std::cout << rank[i] << " ";
        }
        std::cout << '\n';
    }

    //пробразования в int/bool

    friend std::istream& operator>> (std::istream& in, BigInteger& n);
};

std::ostream& operator<< (std::ostream& out, const BigInteger& n) {
    out << n.toString();
    return out;
}

std::istream& operator>> (std::istream& in, BigInteger& n) {
    n = 0;
    char c = in.get();
    while (isspace(c)) {
        c = in.get();
    }
    while (!isspace(c) && c != EOF) {
        n *= 10;
        n += c - '0';
        c = in.get();
    } 
    return in;
}

BigInteger operator+ (const BigInteger& n1, const BigInteger& n2) {
    BigInteger sum(n1);
    sum += n2;
    return sum;
}

BigInteger operator- (const BigInteger& n1, const BigInteger& n2) {
    BigInteger difference(n1);
    difference -= n2;
    return difference;
}

BigInteger operator* (const BigInteger& n1, const BigInteger& n2) {
    BigInteger product(n1);
    product *= n2;
    return product;
}

BigInteger operator/ (const BigInteger& n1, const BigInteger& n2) {
    BigInteger division_result = BigInteger(n1);
    division_result /= n2;
    return division_result;
}

BigInteger operator% (const BigInteger& n1, const BigInteger& n2) {
    BigInteger mod = BigInteger(n1);
    mod %= n2;
    return mod;
}

bool operator< (const BigInteger& n1, const BigInteger& n2) {
    if (n1.sign() != n2.sign()) {
        return n1.sign() < n2.sign();
    }
    size_t  maxsize = std::max(n1.size(), n2.size());
    for (int i = maxsize - 1; i + 1 != 0; --i) {
        if (n1[i] == n2[i]) continue;
        return n1[i] < n2[i];
    }
    return false;
}

bool operator== (const BigInteger& n1, const BigInteger& n2) {
    for (size_t rank = 0; rank < std::max(n1.size(), n2.size()); ++rank) {
        if (n1[rank] != n2[rank]) return false;
    } 
    return true;
}

bool operator> (const BigInteger& n1, const BigInteger& n2) {
    return !(n1 < n2) && !(n1 == n2);
}

bool operator!= (const BigInteger& n1, const BigInteger& n2) {
    return !(n1 == n2);
}

bool operator<= (const BigInteger& n1, const BigInteger& n2) {
    return n1 < n2 || n1 == n2;
}

bool operator>= (const BigInteger& n1, const BigInteger& n2) {
    return n1 > n2 || n1 == n2;
}
