#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ********************************************************************************************************

int solve1(std::ifstream input) {
    int sum = 0;
    int maxSum = 0;
    while (input) {
        std::string line;
        getline(input, line);
        if (!line.empty()) {
            sum += atoi(line.c_str());
        } else {
            maxSum = std::max(maxSum, sum);
            sum = 0;
        }
    }
    maxSum = std::max(maxSum, sum);
    return maxSum;
}

int solve2(std::ifstream input) {
    std::vector<int> sums;
    sums.push_back(0);
    while (input) {
        std::string line;
        getline(input, line);
        if (!line.empty()) {
            sums.back() += atoi(line.c_str());
        } else {
            sums.push_back(0);
        }
    }
    std::sort(sums.begin(), sums.end(), std::greater{});
    return sums[0] + sums[1] + sums[2];
}

// ********************************************************************************************************

int main() {
    const std::string inputFile = "inputs/input01.txt";
    std::cout << solve1(std::ifstream(inputFile)) << std::endl;
    std::cout << solve2(std::ifstream(inputFile)) << std::endl;
    return 0;
}
