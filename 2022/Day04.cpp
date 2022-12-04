#include <fstream>
#include <iostream>

// ********************************************************************************************************

int solve1(std::ifstream input) {
    int result = 0;
    std::pair<int, int> a, b;
    char sep{};
    while (input >> a.first >> sep >> a.second >> sep >> b.first >> sep >> b.second) {
        if ((a.first <= b.first && a.second >= b.second) ||
            (b.first <= a.first && b.second >= a.second)) {
            ++result;
        }
    }
    return result;
}

int solve2(std::ifstream input) {
    int result = 0;
    std::pair<int, int> a, b;
    char sep{};
    while (input >> a.first >> sep >> a.second >> sep >> b.first >> sep >> b.second) {
        if (a.first <= b.second && a.second >= b.first) {
            ++result;
        }
    }
    return result;
}

// ********************************************************************************************************

int main() {
    const std::string inputFile = "inputs/input04.txt";
    std::cout << solve1(std::ifstream(inputFile)) << std::endl;
    std::cout << solve2(std::ifstream(inputFile)) << std::endl;
    return 0;
}
