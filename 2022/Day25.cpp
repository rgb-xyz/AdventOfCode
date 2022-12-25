#include <array>
#include <fstream>
#include <iostream>
#include <string>

// ********************************************************************************************************

constexpr int                    BASE   = 5;
constexpr int                    ZERO   = BASE / 2;
constexpr std::array<char, BASE> DIGITS = { '=', '-', '0', '1', '2' };

std::string solve(std::ifstream input) {
    int64_t sum = 0;
    for (std::string line; std::getline(input, line); ) {
        int64_t num = 0;
        for (const char ch : line) {
            num *= BASE;
            num += std::find(DIGITS.begin(), DIGITS.end(), ch) - (DIGITS.begin() + ZERO);
        }
        sum += num;
    }
    std::string result;
    for (; sum; sum /= BASE) {
        const int rem = sum % BASE + ZERO;
        result.push_back(DIGITS[rem % BASE]);
        sum += rem - rem % BASE;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// ********************************************************************************************************

static const std::string INPUT_FILE  = "inputs/input25.txt";
static const std::string TEST_FILE   = "inputs/test25.txt";
static const std::string TEST_RESULT = "2=-1=0";

int main() {
    if (solve(std::ifstream(TEST_FILE)) == TEST_RESULT) {
        std::cout << solve(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope (" << solve(std::ifstream(TEST_FILE)) << ")" << std::endl;
    }
    return 0;
}
