#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <mutex>
#include <iomanip>

std::vector<std::vector<double>> read_matrix() {
    size_t rows, cols;
    std::cin >> rows >> cols;

    size_t a, b, x, y, z, p;
    std::cin >> a >> b >> x >> y >> z >> p;
    std::vector<std::vector<size_t>> intermediate(rows, std::vector<size_t>(cols, b % p));
    intermediate[0][0] = a % p;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (i > 0 && j > 0) {
                intermediate[i][j] = (intermediate[i][j] + intermediate[i - 1][j - 1] * x) % p;
            }
            if (i > 0) {
                intermediate[i][j] = (intermediate[i][j] + intermediate[i - 1][j] * y) % p;
            }
            if (j > 0) {
                intermediate[i][j] = (intermediate[i][j] + intermediate[i][j - 1] * z) % p;
            }
        }
    }
    size_t max_value = 0;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            max_value = std::max(max_value, intermediate[i][j]);
        }
    }

    std::vector<std::vector<double>> result(rows, std::vector<double>(cols));
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[i][j] = static_cast<double>(intermediate[i][j]) / static_cast<double>(max_value);
        }
    }

    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    auto left = read_matrix();
    auto right = read_matrix();
    auto left_rows = left.size();
    auto left_cols = left[0].size();
    auto right_cols = right[0].size();

    if (left.empty() || right.empty() || left[0].size() != right.size()) {
        std::cerr << "Wrong matrices";
        return 1;
    }

    std::vector<std::vector<long double>> result(left_rows, std::vector<long double>(right_cols, 0.0));

    unsigned hwThreads = std::thread::hardware_concurrency();

    size_t chunkSize = (left_rows + hwThreads - 1) / hwThreads;
    std::vector<std::thread> workers;
    workers.reserve(hwThreads);

    for (size_t t = 0; t < hwThreads; ++t) {
        size_t start = t * chunkSize;
        if (start >= left_rows) break;
        size_t end = std::min(start + chunkSize, left_rows);

        workers.emplace_back([&, start, end] {
            for (size_t i = start; i < end; ++i) {
                for (size_t j = 0; j < right_cols; ++j) {
                    double sum = 0.0;
                    for (size_t k = 0; k < left_cols; ++k) {
                        sum += static_cast<long double>(left[i][k]) * static_cast<long double>(right[k][j]);
                    }
                    result[i][j] = sum;
                }
            }
        });
    }

    for (auto &w : workers) {
        w.join();
    }

    std::cout << left_rows << ' ' << right_cols << "\n";
    for (size_t i = 0; i < left_rows; ++i) {
        for (size_t j = 0; j < right_cols; ++j) {
            std::cout << std::setprecision(20) << result[i][j] << ' ';
        }
        std::cout << "\n";
    }

    return 0;
}
