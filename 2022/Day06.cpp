#include <fstream>
#include <iostream>
#include <string>

// ********************************************************************************************************

int solve(std::ifstream input, const size_t length) {
    std::string line;
    std::getline(input, line);
    const std::string_view lineView = line;

    size_t candidate = length;
    for (size_t pos = 1; pos < line.size(); ++pos) {
        const size_t scanLength = std::min(pos, length);
        const size_t duplicate  = lineView.substr(pos - scanLength, scanLength).rfind(line[pos]);
        if (duplicate != std::string::npos) {
            candidate = std::max(candidate, pos + duplicate);
        }
        if (pos == candidate) {
            return static_cast<int>(candidate + 1);
        }
    }
    return -1;
}

int solve1(std::ifstream input) {
    return solve(std::move(input), 4);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), 14);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input06.txt";
static const std::string TEST_FILE     = "inputs/test06.txt";
static const int         TEST_RESULT_1 = 10;
static const int         TEST_RESULT_2 = 29;

int main() {
    if (solve1(std::ifstream(TEST_FILE)) == TEST_RESULT_1) {
        std::cout << solve1(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope." << std::endl;
    }
    if (solve2(std::ifstream(TEST_FILE)) == TEST_RESULT_2) {
        std::cout << solve2(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope." << std::endl;
    }
    return 0;
}
