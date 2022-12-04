#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

// ********************************************************************************************************

static int getScore(const char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 1;
    } else if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 27;
    } else {
        std::terminate();
    }
}

int solve1(std::ifstream input) {
    int result = 0;
    std::string line;
    while (std::getline(input, line)) {
        const auto middleIt = line.begin() + (line.size() / 2);
        for (char ch : line) {
            const auto it = std::find(middleIt, line.end(), ch);
            if (it != line.end()) {
                result += getScore(*it);
                break;
            }
        }
    }
    return result;
}

int solve2(std::ifstream input) {
    int result = 0;
    std::string line1, line2, line3;
    while (std::getline(input, line1) && std::getline(input, line2) && std::getline(input, line3)) {
        std::sort(line1.begin(), line1.end());
        std::sort(line2.begin(), line2.end());
        std::sort(line3.begin(), line3.end());
        auto it1 = line1.begin();
        auto it2 = line2.begin();
        auto it3 = line3.begin();
        for (;;) {
            if (*it1 < *it2) {
                ++it1;
            } else if (*it1 > *it2) {
                ++it2;
            } else if (*it1 < *it3) {
                ++it1;
            } else if (*it1 > *it3) {
                ++it3;
            } else {
                result += getScore(*it1);
                break;
            }
        }
    }
    return result;
}

// ********************************************************************************************************

int main() {
    const std::string inputFile = "inputs/input03.txt";
    std::cout << solve1(std::ifstream(inputFile)) << std::endl;
    std::cout << solve2(std::ifstream(inputFile)) << std::endl;
    return 0;
}
