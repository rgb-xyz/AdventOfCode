#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

struct Blueprint {
    int     id{};
    uint8_t costA{};
    uint8_t costB{};
    uint8_t costC[2]{};
    uint8_t costD[2]{};

    bool parse(std::istream& input) {
        std::string str;
        if (std::getline(input, str)) {
            std::vector<int>   numbers;
            std::istringstream iss(std::move(str));
            while (iss >> str) {
                try {
                    numbers.push_back(std::stoi(str));
                } catch (const std::invalid_argument&) {
                }
            }
            if (numbers.size() != 7) {
                return false;
            }
            id       = numbers[0];
            costA    = numbers[1];
            costB    = numbers[2];
            costC[0] = numbers[3];
            costC[1] = numbers[4];
            costD[0] = numbers[5];
            costD[1] = numbers[6];
        }
        return bool(input);
    }

    int getResult(const int time, const int delta) const {
        using State = uint64_t;
        using Queue = std::set<State, std::greater<State>>;
        Queue queue;
        queue.insert(1);
        std::cout << "Solving #" << (id / 10) << (id % 10);
        int bestResult = 0;
        for (int t = 0; t < time; ++t) {
            Queue next;
            for (const State state : queue) {
                const uint8_t A = uint8_t(state >> 32);
                const uint8_t B = uint8_t(state >> 40);
                const uint8_t C = uint8_t(state >> 48);
                const uint8_t D = uint8_t(state >> 56);
                if (D + delta < bestResult) break;
                const State newState = state + (state << 32);
                if (A >= costD[0] && C >= costD[1]) {
                    next.insert(newState + (1U << 24) - (State(costD[0]) << 32) - (State(costD[1]) << 48));
                    if (!delta) continue;
                }
                if (A >= costC[0] && B >= costC[1]) {
                    next.insert(newState + (1U << 16) - (State(costC[0]) << 32) - (State(costC[1]) << 40));
                }
                if (A >= costB) {
                    next.insert(newState + (1U << 8) - (State(costB) << 32));
                }
                if (A >= costA) {
                    next.insert(newState + 1U - (State(costA) << 32));
                }
                next.insert(newState);
            }
            queue = std::move(next);
            bestResult = int(*queue.begin() >> 56);
            std::cout << ".";
        }
        std::cout << " " << bestResult << std::endl;
        return bestResult;
    }
};

// ********************************************************************************************************

int solve1(std::ifstream input) {
    int result = 0;
    for (Blueprint bp; bp.parse(input);) {
        result += bp.getResult(24, 0) * bp.id;
    }
    return result;
}

int solve2(std::ifstream input) {
    int count  = 0;
    int result = 1;
    for (Blueprint bp; bp.parse(input);) {
        result *= bp.getResult(32, 2);
        if (++count >= 3) break;
    }
    return result;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input19.txt";
static const std::string TEST_FILE     = "inputs/test19.txt";
static const int         TEST_RESULT_1 = 33;
static const int         TEST_RESULT_2 = 56 * 62;

int main() {
#if 0
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
#else
    std::cout << solve1(std::ifstream(INPUT_FILE)) << std::endl;
    std::cout << solve2(std::ifstream(INPUT_FILE)) << std::endl;
#endif
    return 0;
}
