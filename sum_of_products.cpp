#include <iostream>


int get_num(char* s) {
    int ret = 0;
    for (int i = 0; s[i] != '\0'; ++i) {
        ret = ret * 10 + static_cast<int>(s[i]) - static_cast<int>('0');
    }
    return ret;
}


bool check(int argc, int* positions) {
    for (int i = 0; i < argc; ++i) {
        for (int j = i + 1; j < argc; ++j) {
            if (positions[i] == positions[j]) {
                return false;
            }
        }
    }
    return true;
}


long long sum_of_products(int argc, int* positions, int** arrays) {
    long long ret = 1;
    for (int i = 0; i < argc; ++i) {
        ret *= arrays[i][positions[i]];
    }
    return ret;
}


long long solve(int argc, int* sizes, int** arrays, int cur_pos, int* positions=nullptr) {
    if (!check(cur_pos, positions)) {
        delete[] positions;
        return 0;
    }
    else if (cur_pos == argc) {
        long long ret = sum_of_products(argc, positions, arrays);
        delete[] positions;
        return ret;
    }
    long long ret = 0;
    int** new_slay = new int*[sizes[cur_pos]];
    for (int i = 0; i < sizes[cur_pos]; ++i) {
        new_slay[i] = new int[cur_pos + 1];
        for (int j = 0; j < cur_pos; ++j) {
            new_slay[i][j] = positions[j];
        }
        new_slay[i][cur_pos] = i;
    }
    delete[] positions;
    for (int i = 0; i < sizes[cur_pos]; ++i) {
        ret += solve(argc, sizes, arrays, cur_pos + 1, new_slay[i]);
    }
    delete[] new_slay;
    return ret;
}


int main(int argc, char** argv) {
    --argc;
    int* sizes = new int[argc];
    int** arrays = new int*[argc];
    for (int i = 0; i < argc; ++i) {
        int cur_array_size = get_num(argv[i + 1]);
        sizes[i] = cur_array_size;
        arrays[i] = new int[cur_array_size];
        for (int j = 0; j < cur_array_size; ++j) {
            std::cin >> arrays[i][j];
        }
    }
    std::cout << solve(argc, sizes, arrays, 0) << '\n';
    for (int i = 0; i < argc; ++i) {
        delete[] arrays[i];
    }
    delete[] sizes;
    delete[] arrays;
}
