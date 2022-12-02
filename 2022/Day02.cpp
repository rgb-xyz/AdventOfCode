#include <fstream>
#include <iostream>

// ********************************************************************************************************

int solve1(std::ifstream input) {
    int score = 0;
    char abc{}, xyz{};
    while (input >> abc >> xyz) {
        const int first  = abc - 'A';
        const int second = xyz - 'X';
        score += (second + 1);
        score += ((second - first + 4) % 3) * 3;
    }
    return score;
}

int solve2(std::ifstream input) {
    int score = 0;
    char abc{}, xyz{};
    while (input >> abc >> xyz) {
        const int first  = abc - 'A';
        const int result = xyz - 'X';
        const int second = (first + (result-1) + 3) % 3;
        score += (second + 1);
        score += ((second - first + 4) % 3) * 3;
    }
    return score;
}

// ********************************************************************************************************

int main() {
    const std::string inputFile = "inputs/input02.txt";
    std::cout << solve1(std::ifstream(inputFile)) << std::endl;
    std::cout << solve2(std::ifstream(inputFile)) << std::endl;
    return 0;
}
