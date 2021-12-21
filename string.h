#include <iostream>
#include <cstring>

class String;

String operator+ (const String &first, const String &second);

String operator+ (const String &other, char symbol);

String operator+ (char symbol, const String &other);

bool operator== (const String &first, const String &second);

class String {

private:
    char* str;
    size_t size;
    size_t capacity;

    void make_new_capacity() {
        char* new_str = new char[capacity];
        memcpy(new_str, str, size);
        delete[] str;
        str = new_str;
    }

    void doubling() {
        capacity *= 2;
        make_new_capacity();
    }

    void halve() {
        capacity /= 2;
        make_new_capacity();
    }

    void swap(String &other) {
        std::swap(size, other.size);
        std::swap(str, other.str);
        std::swap(capacity, other.capacity);
    }

public:
    String(): str(new char[1]), size(0), capacity(1) {}

    String(size_t new_size, char symbol):
        str(new char[new_size * 2]), size(new_size), capacity(new_size * 2 + 1) {
        memset(str, symbol, new_size);
    }

    String(const char* c_string): String(strlen(c_string), '0') {
        memcpy(str, c_string, size);
    }

    String(const String &other): String(other.size, '\0') {
        memcpy(str, other.str, size);
    }

    ~String() {
        delete[] str;
    }

    String& operator=(const String &other) {
        String copy(other);
        swap(copy);
        return *this;
    }

    void push_back(char symbol) {
        if (capacity == size) {
            doubling();
        }
        str[size] = symbol;
        ++size;
    }

    void pop_back() {
        if (size * 4 <= capacity) {
            halve();
        }
        --size;
    }

    String& operator+= (char symbol) {
        push_back(symbol);
        return *this;
    }

    String& operator+= (const String &other) {
        size_t start_size = other.size;
        for (size_t i = 0; i < start_size; ++i) {
            *this += other.str[i];
        }
        return *this;
    }

    char& operator[] (size_t i) {
        return str[i];
    }

    const char& operator[] (size_t i) const {
        return str[i];
    }

    char& front() {
        return str[0];
    }

    const char& front() const {
        return str[0];
    }

    char& back() {
        return str[size - 1];
    }

    const char& back() const {
        return str[size - 1];
    }

    size_t length() const {
        return size;
    }

    bool empty() const {
        return size == 0;
    }

    void clear() {
        *this = String();
    }

    String substr(size_t start, size_t count) const {
        String substring(count, '0');
        memcpy(substring.str, str + start, count);
        return substring;
    }

    size_t find(const String &substring) const {
        for (int i = 0; i + substring.size <= size; ++i) {
            if (substr(i, substring.size) == substring) {
                return i;
            }
        }
        return size;
    }

    size_t rfind(const String &substring) const {
        int result = size;
        for (int i = 0; i + substring.size <= size; ++i) {
            if (substr(i, substring.size) == substring) {
                result = i;
            }
        }
        return result;
    }
};


String operator+(const String &first, const String &second) {
    String sum(first);
    sum += second;
    return sum;
}


String operator+(const String &other, char symbol) {
    return other + String(1, symbol);
}


String operator+(char symbol, const String &other) {
    return String(1, symbol) + other;
}


bool operator==(const String &first, const String &second) {
    if (first.length() != second.length()) {
        return false;
    }
    bool equil = true;
    for (size_t i = 0; i < first.length(); ++i) {
        if (first[i] != second[i]) {
            equil = false;
            break;
        }
    }
    return equil;
}


bool operator!=(const String &first, const String &second) {
    return !(first == second);
}


std::ostream& operator<< (std::ostream &out, const String &string) {
    for (size_t i = 0; i < string.length(); ++i) {
        out << string[i];
    }
    return out;
}


std::istream& operator>> (std::istream &in, String &string) {
    string.clear();
    char symbol = in.get();
    while (isspace(symbol)) {
        symbol = in.get();
    }
    while (!isspace(symbol) && symbol != EOF) {
        string.push_back(symbol);
        symbol = in.get();
    }
    return in;
}
