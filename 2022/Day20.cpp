#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

// ********************************************************************************************************

int64_t solve(std::ifstream input, const int64_t multiplier, const int rounds) {
    std::vector<std::pair<int64_t, size_t>> sequence;
    for (int64_t data{}; input >> data;) {
        sequence.emplace_back(data * multiplier, sequence.size());
    }
    // Exclude one because the front/back positions are equivalent in a circular buffer.
    // Must be signed (!) because "signed % unsigned" is evaluated as "unsigned % unsigned"
    // without any warning from MSVC. Thanks Microsoft!
    const int64_t ringSize = sequence.size() - 1;
    for (int round = 0; round < rounds; ++round) {
        for (size_t index = 0; index < sequence.size(); ++index) {
            const int64_t i1 = std::find_if(
                                   sequence.begin(), sequence.end(),
                                   [index](const auto& data) { return data.second == index; }
                               ) - sequence.begin();
            const auto data  = sequence[i1];
            const int64_t i2 = (i1 + (data.first % ringSize) + ringSize) % ringSize;
            sequence.erase(sequence.begin() + i1);
            sequence.insert(sequence.begin() + i2, data);
        }
    }
    const int64_t index = std::find_if(
                              sequence.begin(), sequence.end(),
                              [](const auto& data) { return data.first == 0; }
                          ) - sequence.begin();
    return sequence[(index + 1000) % sequence.size()].first +
           sequence[(index + 2000) % sequence.size()].first +
           sequence[(index + 3000) % sequence.size()].first;
}

int64_t solve1(std::ifstream input) {
    return solve(std::move(input), 1, 1);
}

int64_t solve2(std::ifstream input) {
    return solve(std::move(input), 811589153, 10);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input20.txt";
static const std::string TEST_FILE     = "inputs/test20.txt";
static const int64_t     TEST_RESULT_1 = 3;
static const int64_t     TEST_RESULT_2 = 1623178306;

int main() {
    if (solve1(std::ifstream(TEST_FILE)) == TEST_RESULT_1) {
        std::cout << solve1(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope (" << solve1(std::ifstream(TEST_FILE)) << ")" << std::endl;
    }
    if (solve2(std::ifstream(TEST_FILE)) == TEST_RESULT_2) {
        std::cout << solve2(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope (" << solve2(std::ifstream(TEST_FILE)) << ")" << std::endl;
    }
    return 0;
}
