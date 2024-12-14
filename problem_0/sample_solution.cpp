#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <thread>
#include <mutex>

std::istream& operator>>(std::istream& in, __int128& value) {
    std::string s;
    in >> s;
    value = 0;
    bool negative = false;
    size_t i = 0;
    if (s[0] == '-') {
        negative = true;
        i = 1;
    }
    for (; i < s.size(); ++i) {
        value = value * 10 + (s[i] - '0');
    }
    if (negative) value = -value;
    return in;
}

std::ostream& operator<<(std::ostream& out, __int128 value) {
    if (value == 0) {
        out << '0';
        return out;
    }
    std::string s;
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }
    while (value > 0) {
        s += '0' + static_cast<int>(value % 10);
        value /= 10;
    }
    if (negative) s += '-';
    std::reverse(s.begin(), s.end());
    out << s;
    return out;
}

__int128 sqrtInt128(__int128 x) {
    if (x == 0 || x == 1) {
        return x;
    }

    __int128 low = 0, high = x;
    __int128 result = 0;
    while (low <= high) {
        __int128 mid = low + (high - low) / 2;
        __int128 sq = mid * mid;
        if (sq == x) {
            return mid;
        } else if (sq < x) {
            result = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    __int128 n;
    std::cin >> n;

    __int128 hwThreads = std::thread::hardware_concurrency();

    __int128 nSqrt = sqrtInt128(n);

    __int128 chunkSize = (nSqrt > 0 ? nSqrt / hwThreads : 1);

    std::vector<std::vector<__int128>> partialFactors(hwThreads);
    std::vector<std::thread> workers;
    workers.reserve((size_t)hwThreads);

    for (__int128 i = 0; i < hwThreads; ++i) {
        __int128 start = 2 + i * chunkSize;
        __int128 end   = start + chunkSize - 1;

        if (i == hwThreads - 1) {
            end = nSqrt;
        }
        
        workers.emplace_back([&, i, start, end] {
            __int128 localN = n;
            for (__int128 p = start; p <= end && p <= localN / p; ++p) {
                while (localN % p == 0) {
                    partialFactors[i].push_back(p);
                    localN /= p;
                }
            }
        });
    }

    for (auto &t : workers) {
        t.join();
    }

    std::vector<__int128> allFactors;
    for (auto &pf : partialFactors) {
        allFactors.insert(allFactors.end(), pf.begin(), pf.end());
    }
    std::sort(allFactors.begin(), allFactors.end());

    std::vector<__int128> distinctFactors;
    distinctFactors.reserve(allFactors.size());
    for (size_t i = 0; i < allFactors.size(); ++i) {
        bool isNewPrimeFactor = true;
        for (size_t j = 0; j < i; ++j) {
            if (allFactors[i] == allFactors[j]) {
                continue;
            }
            if (allFactors[i] % allFactors[j] == 0) {
                isNewPrimeFactor = false;
                break;
            }
        }
        if (isNewPrimeFactor) {
            distinctFactors.push_back(allFactors[i]);
        }
    }

    for (__int128 i : distinctFactors) {
        n /= i;
    }

    if (n > 1) {
        distinctFactors.push_back(n);
    }

    for (__int128 i : distinctFactors) {
        std::cout << i << ' ';
    }

    return 0;
}
