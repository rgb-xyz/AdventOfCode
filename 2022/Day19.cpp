#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

static void printProgress(const int id, const int time) {
    static std::mutex  mutex;
    static std::string progress;

    const std::unique_lock lock{ mutex };
    if (progress.size() < id) {
        progress.resize(id, ' ');
    }
    if (time >= 0) {
        progress[id - 1] = 'P' - time;
    } else {
        progress[id - 1] = ' ';
    }
    std::cout << " :" << progress << '\r' << std::flush;
}

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

    int bestResult = 0;

    void solve(const int time, const int delta) {
        using State = uint64_t;
        std::vector<State> queue;
        queue.push_back(1);
        for (int t = 0; t < time; ++t) {
            printProgress(id, t);
            std::vector<State> next;
            State lastState = 0;
            for (const State state : queue) {
                if (state == lastState) continue;
                lastState = state;
                const uint8_t A = uint8_t(state >> 32);
                const uint8_t B = uint8_t(state >> 40);
                const uint8_t C = uint8_t(state >> 48);
                const uint8_t D = uint8_t(state >> 56);
                if (D + delta < bestResult) break;
                const State newState = state + (state << 32);
                if (A >= costD[0] && C >= costD[1]) {
                    next.push_back(newState + (1U << 24) - (State(costD[0]) << 32) - (State(costD[1]) << 48));
                    if (!delta) continue;
                }
                if (A >= costC[0] && B >= costC[1]) {
                    next.push_back(newState + (1U << 16) - (State(costC[0]) << 32) - (State(costC[1]) << 40));
                }
                if (A >= costB) {
                    next.push_back(newState + (1U << 8) - (State(costB) << 32));
                }
                if (A >= costA) {
                    next.push_back(newState + 1U - (State(costA) << 32));
                }
                next.push_back(newState);
            }
            queue = std::move(next);
            std::sort(std::execution::par_unseq, queue.begin(), queue.end(), std::greater{});
            bestResult = int(queue.front() >> 56);
        }
        printProgress(id, -1);
    }
};

// ********************************************************************************************************

// Note: MSVC's (parallelized) std::accumulate_reduce always executes at least 2 items on one thread.
// This is not particularly useful for few expensive calculations. Instead, let's do parallel std::for_each
// followed by sequential std::accumulate.

int solve1(std::ifstream input) {
    std::vector<Blueprint> blueprints;
    for (Blueprint bp; bp.parse(input);) {
        blueprints.push_back(std::move(bp));
    }
    std::for_each(
        std::execution::par, blueprints.begin(), blueprints.end(),
        [](Blueprint& bp) { bp.solve(24, 0); }
    );
    return std::accumulate(
        blueprints.cbegin(), blueprints.cend(), 0, 
        [](const int result, const Blueprint& bp) { return result + bp.bestResult * bp.id; }
    );
}

int solve2(std::ifstream input) {
    std::vector<Blueprint> blueprints;
    for (Blueprint bp; bp.parse(input);) {
        blueprints.push_back(std::move(bp));
        if (blueprints.size() >= 3) break;
    }
    std::for_each(
        std::execution::par, blueprints.begin(), blueprints.end(),
        [](Blueprint& bp) { bp.solve(32, 2); }
    );
    return std::accumulate(
        blueprints.cbegin(), blueprints.cend(), 1, 
        [](const int result, const Blueprint& bp) { return result * bp.bestResult; }
    );
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
