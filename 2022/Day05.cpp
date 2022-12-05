#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// ********************************************************************************************************

std::string solve(std::ifstream input, auto crane) {
    std::vector<std::string> piles;
    std::string line;
    while (std::getline(input, line)) {
        if (piles.empty()) {
            piles.resize((line.size() + 1) / 4);
        }
        if (line.find('[') == std::string::npos) {
            break;
        }
        for (size_t i = 0; i < piles.size(); ++i) {
            if (line[i*4] == '[') {
                piles[i].insert(piles[i].begin(), line[i*4 + 1]);
            }
        }
    }
    int count{}, from{}, to{};
    while (input >> line >> count >> line >> from >> line >> to) {
        crane(piles[from-1], piles[to-1], count);
    }
    std::string result;
    for (const std::string& pile : piles) {
        result.push_back(pile.back());
    }
    return result;
}

std::string solve1(std::ifstream input) {
    return solve(
        std::move(input),
        [](std::string& from, std::string& to, const int count) {
            for (int i = 0; i < count; ++i) {
                to.push_back(from.back());
                from.pop_back();
            }
        }
    );
}

std::string solve2(std::ifstream input) {
    return solve(
        std::move(input),
        [](std::string& from, std::string& to, const int count) {
            to += from.substr(from.size() - count);
            from.resize(from.size() - count);
        }
    );
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input05.txt";
static const std::string TEST_FILE     = "inputs/test05.txt";
static const std::string TEST_RESULT_1 = "CMZ";
static const std::string TEST_RESULT_2 = "MCD";

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
