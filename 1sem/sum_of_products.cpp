#include <iostream>


int get_num(char* s) {
    int result = 0;
    for (int i = 0; s[i] != '\0'; ++i) {
        result = result * 10 + s[i] - '0';
    }
    return result;
}


bool check(int arrays_number, int* positions) {
    for (int i = 0; i < arrays_number; ++i) {
        for (int j = i + 1; j < arrays_number; ++j) {
            if (positions[i] == positions[j]) {
                return false;
            }
        }
    }
    return true;
}


long long sum_of_products(int arrays_number, int* positions, int** arrays) {
    long long result = 1;
    for (int i = 0; i < arrays_number; ++i) {
        result *= arrays[i][positions[i]];
    }
    return result;
}


long long solve(int arrays_number, int* sizes, int** arrays, int cur_pos, int* positions=nullptr) {
    if (!check(cur_pos, positions)) {
        return 0;
    } else if (cur_pos == arrays_number) {
        long long result = sum_of_products(arrays_number, positions, arrays);
        return result;
    }
    long long result = 0;
    int** next_round = new int*[sizes[cur_pos]];
    for (int i = 0; i < sizes[cur_pos]; ++i) {
        next_round[i] = new int[cur_pos + 1];
        for (int j = 0; j < cur_pos; ++j) {
            next_round[i][j] = positions[j];
        }
        next_round[i][cur_pos] = i;
    }
    for (int i = 0; i < sizes[cur_pos]; ++i) {
        result += solve(arrays_number, sizes, arrays, cur_pos + 1, next_round[i]);
        delete[] next_round[i];
    }
    delete[] next_round;
    return result;
}


int main(int arrays_number, char** argv) {
    --arrays_number;
    int* sizes = new int[arrays_number];
    int** arrays = new int*[arrays_number];
    for (int i = 0; i < arrays_number; ++i) {
        int cur_array_size = get_num(argv[i + 1]);
        sizes[i] = cur_array_size;
        arrays[i] = new int[cur_array_size];
        for (int j = 0; j < cur_array_size; ++j) {
            std::cin >> arrays[i][j];
        }
    }
    std::cout << solve(arrays_number, sizes, arrays, 0) << '\n';
    for (int i = 0; i < arrays_number; ++i) {
        delete[] arrays[i];
    }
    delete[] sizes;
    delete[] arrays;
}
