#include <iostream>
#include <cstring>

class String;

String operator+ (const String& s1, const String& s2);

String operator+ (const String& s, const char& c);

String operator+ (const char& c, const String& s);

bool operator== (const String& s1, const String& s2);

size_t c_string_size(const char* s);

class String {

private:
    char* str;
    size_t sz;
    size_t capacity;

    void doubling() {
        capacity *= 2;
        char* new_str = new char[capacity];
        memcpy(new_str, str, sz);
        delete[] str;
        str = new_str;
    }

    void halve() {
        capacity /= 2;
        char* new_str = new char[capacity];
        memcpy(new_str, str, sz);
        delete[] str;
        str = new_str;
    }

    void swap(String &s) {
        std::swap(sz, s.sz);
        std::swap(str, s.str);
        std::swap(capacity, s.capacity);
    }
 
public:
    String(): str(new char[1]), sz(0), capacity(1) {}

    String(const char* s):
    str(new char[c_string_size(s) * 2]), sz(c_string_size(s)), capacity(c_string_size(s) * 2 + 1) {
        memcpy(str, s, sz);
    }

    String(const size_t& new_sz, const char& c):
        str(new char[new_sz * 2]), sz(new_sz), capacity(new_sz * 2 + 1) {
        memset(str, c, new_sz);
    }

    String(const String& s): String(s.sz, '\0') {
        memcpy(str, s.str, sz);
    }

    ~String() {
        delete[] str;
    }

    String& operator=(const String& s) {
        String copy = s;
        swap(copy);
        return *this;
    }

    void push_back(const char& c) {
        if (capacity == sz) {
            doubling();
        }
        str[sz] = c;
        ++sz;
    }

    void pop_back() {
        if (sz * 4 <= capacity) {
            halve();
        }
        --sz;
    }

    String& operator+= (const char& c) {
        push_back(c);
        return *this;
    }

    String& operator+= (const String &s) {
        size_t start_size = s.sz;
        for (size_t i = 0; i < start_size; ++i) {
            *this += s.str[i];
        }
        return *this;
    }

    char& operator[] (const size_t& i) {
        return str[i];
    }

    const char& operator[] (const size_t& i) const {
        return str[i];
    }

    char& front() {
        return str[0];
    }

    const char& front() const {
        return str[0];
    }

    char& back() {
        return str[sz - 1];
    }

    const char& back() const {
        return str[sz - 1];
    }

    size_t length() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void clear() {
        *this = String();
    }

    String substr(const size_t& start, const size_t& count) const {
        String substring(count, '0');
        memcpy(substring.str, str + start, count);
        return substring;
    }

    size_t find(const String& substring) const {
        for (int i = 0; i + substring.sz <= sz; ++i) {
            if (substr(i, substring.sz) == substring) {
                return i;
            }
        }
        return sz;
    }

    size_t rfind(const String& substring) const {
        int ret = sz;
        for (int i = 0; i + substring.sz <= sz; ++i) {
            if (substr(i, substring.sz) == substring) {
                ret = i;
            }
        }
        return ret;
    }

    friend std::istream& operator>> (std::istream& in, const String& point);
};


size_t c_string_size(const char* s) {
    size_t ret = 0;
    while (s[ret] != '\0') {
        ++ret;
    }
    return ret;
}


String operator+(const String& s1, const String& s2) {
    String sum(s1);
    sum += s2;
    return sum;
}


String operator+(const String& s, const char& c) {
    return s + String(1, c);
}


String operator+(const char& c, const String& s) {
    return String(1, c) + s;
}


bool operator==(const String& s1, const String& s2) {
    if (s1.length() != s2.length()) {
        return false;
    }
    bool equil = true;
    for (size_t i = 0; i < s1.length(); ++i) {
        if (s1[i] != s2[i]) {
            equil = false;
            break;
        }
    }
    return equil;
}


std::ostream& operator<< (std::ostream& out, const String& s) {
    for (size_t i = 0; i < s.length(); ++i) {
        out << s[i];
    }
    return out;
}


std::istream& operator>> (std::istream& in, String& s) {
    s.clear();
    char c = in.get();
    while (isspace(c)) {
        c = in.get();
    }
    while (!isspace(c) && c != EOF  ) {
        s.push_back(c);
        c = in.get();
    }
    return in;
}
