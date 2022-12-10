#include <fstream>
#include <iostream>

// ********************************************************************************************************

int solve(std::ifstream input) {
    int x      = 1;
    int cycle  = 1;
    int result = 0;

    const auto probe = [&]() {
        const int c = (cycle-1) % 40;
        std::cout << (std::abs(x - c) <= 1 ? "#" : ".");
        if (c == 39) {
            std::cout << std::endl;
        }
        if (c == 19) {
            result += x * cycle;
        }
    };

    std::string opcode;
    while (input >> opcode) {
        probe();
        cycle++;
        int arg{};
        if (opcode == "addx" && input >> arg) {
            probe();
            cycle++;
            x += arg;
        }
    }
    std::cout << std::endl;
    return result;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input10.txt";
static const std::string TEST_FILE     = "inputs/test10.txt";
static const int         TEST_RESULT_1 = 13140;

int main() {
    if (solve(std::ifstream(TEST_FILE)) == TEST_RESULT_1) {
        std::cout << solve(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope." << std::endl;
    }
    return 0;
}
