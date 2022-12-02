#include <fstream>

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
    const int64_t result1 = solve1(std::ifstream(inputFile));
    printf("%lld\n", result1);
    const int64_t result2 = solve2(std::ifstream(inputFile));
    printf("%lld\n", result2);
    return 0;
}
