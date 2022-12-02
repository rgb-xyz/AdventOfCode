#include <algorithm>
#include <fstream>
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
    const int64_t result1 = solve1(std::ifstream(inputFile));
    printf("%lld\n", result1);
    const int64_t result2 = solve2(std::ifstream(inputFile));
    printf("%lld\n", result2);
    return 0;
}
