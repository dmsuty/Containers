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

class BigInteger {

private:
    static const int power_of_radix=9;
    static const int radix=1'000'000'000; //must be a power of ten.
    std::vector<long long> rank;
    int sign = 1;

    static int number_of_digits(int n, const int& radix) {
        if (n == 0) {
            return 1;
        }
        n = abs(n);
        int cur_deg = 0;
        long long cur_pow = 1;
        while (cur_pow <= n) {
            cur_pow *= radix;
            ++cur_deg;
        }
        return cur_deg;
    }

    static int sign_of_number(const long long& n) {
        if (n >= 0) return 1;
        return -1;
    }

    void delete_nulls() {
        while (rank.size() && rank.back() == 0) {
            rank.pop_back();
        }
    }

    void carryover() {
        long long carry = 0;
        for (size_t i = 0; i < rank.size(); ++i) {
            long long new_carry = (rank[i] + carry) / radix;
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
        if (rank.size() && rank.back() < 0) {
            sign *= -1;
            for (size_t i = 0; i < rank.size(); ++i) {
                rank[i] *= -1;
            }
        }
        size_t position = 0;
        while (position < rank.size()) {
            if (rank[position] >= 0) {
                ++position;
                continue;
            }
            rank[position] += radix;
            ++position;
            while (rank[position] <= 0) {
                rank[position] += radix - 1;
                ++position;
            }
            rank[position] -= 1;
            ++position;
        }
        delete_nulls();
        if (rank.size() == 0) {
            sign = 1;
        }
    }

public:
    BigInteger() = default;

    BigInteger(int n): sign(sign_of_number(n)) {
        while (n) {
            rank.push_back(abs(n % radix));
            n /= radix;
        }
    }

    BigInteger(const BigInteger& other) = default;

    ~BigInteger() = default;

    int operator[] (const size_t& k) const {
        if (rank.size() <= k) {
            return 0;
        }
        return rank[k];
    }

    size_t size() const {
        return rank.size();
    }

    int get_sign() const {
        return sign;
    }

    BigInteger& operator+= (const BigInteger& other) {
        while (rank.size() < other.size()) {
            rank.push_back(0);
        }
        for (size_t i = 0; i < rank.size(); ++i) {
            rank[i] += other[i] * other.sign * sign;
        }
        reconstructor();
        return *this;
    }

    BigInteger& operator*= (const BigInteger& other) {
        if (other == 0) {
            return *this = 0;
        }
        size_t self_size = rank.size();
        while (rank.size() < self_size + other.size() - 1) {
            rank.push_back(0);
        }
        for (size_t i = rank.size() - 1; i + 1 != 0; --i) {
            int start_value = rank[i];
            for (size_t j1 = i; j1 + 1 != 0; --j1) {
                size_t j2 = i - j1;
                if (j2 == other.size()) {
                    break;
                }
                rank[i] += rank[j1] * other[j2];
                size_t j = i;
                while (rank[j] >= radix) {
                    int carry = rank[j] / radix;
                    rank[j] %= radix;
                    ++j;
                    if (j == rank.size()) {
                        rank.push_back(0);
                    }
                    rank[j] += carry;
                }
            }
            rank[i] -= start_value;
        }
        sign *= other.sign;
        reconstructor();
        return *this;
    }

    BigInteger operator-() const {
        return BigInteger(*this) * -1;
    }

    BigInteger& operator-= (const BigInteger& other) {
        while (rank.size() < other.size()) {
            rank.push_back(0);
        }
        for (size_t i = 0; i < rank.size(); ++i) {
            rank[i] -= other[i] * other.sign * sign;
        }
        reconstructor();
        return *this;
    }

    BigInteger& operator/= (const BigInteger& divider) {
        BigInteger quotient(0);
        BigInteger cur_dividend(0);
        for (size_t i = rank.size(); i + 1 != 0; --i) {
            long long high_digits = 0;
            for (int j = (int)cur_dividend.size() - 1; j >= (int)divider.size() - 1; --j) {
                high_digits = high_digits * radix + cur_dividend[j];
            }
            int left_board = high_digits / (divider.rank.back() + 1);
            int right_board = high_digits / divider.rank.back() + 1;
            while (left_board + 1 != right_board) {
                int middle = (left_board + right_board) / 2;
                if (divider * (divider.sign * middle) <= cur_dividend) {
                    left_board = middle;
                } else {
                    right_board = middle;
                }
            }
            quotient.rank.push_back(left_board);
            cur_dividend -= (left_board * divider.sign) * divider;
            if (i != 0) {
                cur_dividend = cur_dividend * radix + rank[i - 1];
            }
        }
        for (size_t i = 0; i * 2 < quotient.size(); ++i) {
            std::swap(quotient.rank[i], quotient.rank[quotient.size() - i - 1]);
        }
        *this = quotient * (divider.sign * sign);
        return *this;
    }

    BigInteger& operator%= (const BigInteger& divider) {
        BigInteger quotient(0);
        BigInteger cur_dividend(0);
        for (size_t i = rank.size(); i + 1 != 0; --i) {
            int left_board = 0;
            int right_board = radix;
            while (left_board + 1 != right_board) {
                int middle = (left_board + right_board) / 2;
                if (divider * (middle * divider.sign) <= cur_dividend) {
                    left_board = middle;
                } else {
                    right_board = middle;
                }
            }
            cur_dividend -= (left_board * divider.sign) * divider;
            if (i != 0) {
                cur_dividend = cur_dividend * radix + rank[i - 1];
            }
        }
        *this = cur_dividend * sign;
        return *this;
    }

    BigInteger& operator++() {
        size_t current_rank = 0;
        if (sign == 1) {
            while (current_rank < rank.size() && rank[current_rank] == radix - 1) {
                rank[current_rank] = 0;
                ++current_rank;
            }
        } else {
            while (current_rank < rank.size() && rank[current_rank] == 0) {
                rank[current_rank] = radix - 1;
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
        if (sign == 1) {
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
        if (sign == -1) {
            result += "-";
        }
        for (size_t i = rank.size() - 1; i + 1 != 0; --i) {
            if (i != rank.size() - 1) {
                result += std::string(power_of_radix - number_of_digits(rank[i], 10), '0');
            }
            result += std::to_string(rank[i]);
        }
        return result;
    }

    void show_ranks() {
        for (size_t i = rank.size() - 1; i + 1 != 0; --i) {
            std::cout << rank[i] << " ";
        }
        std::cout << '\n';
    }

    explicit operator bool() const {
         return *this != 0;
    }

    explicit operator int() const {
        int result = 0;
        for (size_t i = rank.size() - 1; i + 1 != 0; --i) {
            result *= radix;
            result += rank[i];
        }
        return result * sign;
    }

    friend std::istream& operator>> (std::istream& in, BigInteger& n);
};

std::ostream& operator<< (std::ostream& out, const BigInteger& n) {
    out << n.toString();
    return out;
}

std::istream& operator>> (std::istream& in, BigInteger& n) {
    n = 0;
    int input_sign = 1;
    char c = in.get();
    while (isspace(c)) {
        c = in.get();
    }
    if (c == '-') {
        input_sign = -1;
    } else {
        n += (c - '0');
    }
    c = in.get();
    while (!isspace(c) && c != EOF) {
        n *= 10;
        n += (c - '0') * input_sign;
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
    if (n1.get_sign() != n2.get_sign()) {
        return n1.get_sign() < n2.get_sign();
    }
    size_t  maxsize = std::max(n1.size(), n2.size());
    for (int i = maxsize - 1; i + 1 != 0; --i) {
        if (n1[i] == n2[i]) continue;
        return n1[i] < n2[i];
    }
    return false;
}

bool operator== (const BigInteger& n1, const BigInteger& n2) {
    if (n1.get_sign() != n2.get_sign()) {
        return false;
    }
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


class Rational;

bool operator< (const Rational& n1, const Rational& n2);

bool operator== (const Rational& n1, const Rational& n2);

bool operator> (const Rational& n1, const Rational& n2);

bool operator!= (const Rational& n1, const Rational& n2);

bool operator<= (const Rational& n1, const Rational& n2);

bool operator>= (const Rational& n1, const Rational& n2);

Rational operator+ (const Rational& n1, const Rational& n2);

Rational operator- (const Rational& n1, const Rational& n2);

Rational operator* (const Rational& n1, const Rational& n2);

Rational operator/ (const Rational& n1, const Rational& n2);

std::ostream& operator<< (std::ostream& out, const Rational& n);


class Rational {

private:
    BigInteger numerator=0;
    BigInteger denominator=1;

    static BigInteger gcd(BigInteger a, BigInteger b) {
        if (a == 0 || a == b) {
            return b;
        }
        if (b == 0) {
            return a;
        }
        if (a == 1 || b == 1) {
            return 1;
        }
        if (a % 2 == 0 && b % 2 == 0) {
            return gcd(a / 2, b / 2) * 2;
        }
        while (a % 2 == 0) {
            a /= 2;
        }
        while (b % 2 == 0) {
            b /= 2;
        }
        if (a < b) {
            return gcd((b - a) / 2, a);
        }
        return gcd((a - b) / 2, b);
    }

    void reconstructor() {
        numerator *= denominator.get_sign();
        denominator *= denominator.get_sign();
        BigInteger reducer = gcd(numerator * numerator.get_sign(), denominator);
        numerator /= reducer;
        denominator /= reducer;
    }

public:
    Rational() = default;

    Rational(const int& n): numerator(n) {};

    Rational(const BigInteger& n): numerator(n) {};

    Rational(const Rational&) = default;

    Rational& operator+= (const Rational& other) {
        numerator *= other.denominator;
        numerator += other.numerator * denominator;
        denominator *= other.denominator;
        reconstructor();
        return *this;
    }

    Rational& operator*= (const Rational& other) {
        denominator *= other.denominator;
        numerator *= other.numerator;
        reconstructor();
        return *this;
    }

    Rational& operator-= (const Rational& other) {
        numerator *= other.denominator;
        numerator -= other.numerator * denominator;
        denominator *= other.denominator;
        reconstructor();
        return *this;
    }

    Rational& operator/= (const Rational& other) {
        numerator *= other.denominator;
        denominator *= other.numerator;
        reconstructor();
        return *this;
    }

    Rational operator- () const {
        return Rational(*this) * -1;
    }

    std::string toString() const {
        std::string result = numerator.toString();
        if (denominator != 1) {
            result += "/" + denominator.toString();
        }
        return result;
    }

    std::string asDecimal(const size_t& precision) const {
        std::string result;
        if (numerator < 0) {
            result += "-";
        }
        result += (numerator / denominator * numerator.get_sign()).toString() + ".";
        BigInteger dividend = numerator % denominator * numerator.get_sign();
        for (size_t i = 0; i < precision; ++i) {
            result += (dividend * 10 / denominator).toString();
            dividend = dividend * 10 % denominator;
        }
        return result;
    }

    explicit operator double() const {
        double result = 0.;
        double degree_value = 1.;
        result += static_cast<int>(numerator / denominator) * degree_value;
        BigInteger dividend = numerator % denominator;
        degree_value *= 0.1;
        while (static_cast<int>(dividend * 10 / denominator) * degree_value != 0.) {
            result += static_cast<int>(dividend * 10 / denominator) * degree_value;
            degree_value *= 0.1;
            dividend = dividend * 10 % denominator;
        }
        return result;
    }

    friend bool operator< (const Rational& a, const Rational& b);

    friend bool operator== (const Rational& a, const Rational& b);

    friend bool operator> (const Rational& a, const Rational& b);

    friend bool operator!= (const Rational& a, const Rational& b);

    friend bool operator<= (const Rational& a, const Rational& b);

    friend bool operator>= (const Rational& a, const Rational& b);
};

bool operator< (const Rational& a, const Rational& b) {
    return a.numerator * b.denominator < b.numerator * a.denominator;
}

bool operator== (const Rational& a, const Rational& b) {
    return a.numerator * b.denominator == b.numerator * a.denominator;
}

bool operator> (const Rational& a, const Rational& b) {
    return !(a < b) && !(a == b);
}

bool operator!= (const Rational& a, const Rational& b) {
    return !(a == b);
}

bool operator<= (const Rational& a, const Rational& b) {
    return a < b || a == b;
}

bool operator>= (const Rational& a, const Rational& b) {
    return a == b || a > b;
}

Rational operator+ (const Rational& a, const Rational& b) {
    Rational result(a);
    result += b;
    return result;
}

Rational operator- (const Rational& a, const Rational& b) {
    Rational result(a);
    result -= b;
    return result;
}

Rational operator* (const Rational& a, const Rational& b) {
    Rational result(a);
    result *= b;
    return result;
}

Rational operator/ (const Rational& a, const Rational& b) {
    Rational result(a);
    result /= b;
    return result;
}

std::ostream& operator<< (std::ostream& out, const Rational& n) {
    out << n.toString();
    return out;
}
