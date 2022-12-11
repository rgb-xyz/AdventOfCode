#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>

struct Monkey {
    enum class Operation { ADD, MUL, SQUARE };

    std::list<int64_t> items;
    Operation          operation     = {};
    int                argument      = {};
    int                divisor       = {};
    int                nextMonkey[2] = {};
    int                counter       = 0;

    void parse(std::istream& input) {
        std::string line;
        if (!std::getline(input, line) || !line.starts_with("  Starting items: ")) {
            std::terminate();
        }
        std::istringstream iss(line.substr(18));
        int item{};
        while (iss >> item) {
            items.push_back(item);
            if (iss.get() != ',') {
                break;
            }
        }
        if (!std::getline(input, line) || !line.starts_with("  Operation: new = old ")) {
            std::terminate();
        }
        if (line[23] == '+') {
            operation = Operation::ADD;
            argument = atoi(line.c_str() + 25);
        } else if (line[23] == '*') {
            if (line.substr(25) == "old") {
                operation = Operation::SQUARE;
            } else {
                operation = Operation::MUL;
                argument = atoi(line.c_str() + 25);
            }
        } else {
            std::terminate();
        }
        if (!std::getline(input, line) || !line.starts_with("  Test: divisible by ")) {
            std::terminate();
        }
        divisor = atoi(line.c_str() + 21);
        if (!std::getline(input, line) || !line.starts_with("    If true: throw to monkey ")) {
            std::terminate();
        }
        nextMonkey[0] = atoi(line.c_str() + 29);
        if (!std::getline(input, line) || !line.starts_with("    If false: throw to monkey ")) {
            std::terminate();
        }
        nextMonkey[1] = atoi(line.c_str() + 30);
    }
};

// ********************************************************************************************************

int64_t solve(std::ifstream input, const int rounds, const bool reduce) {
    int64_t ringSize = 1;
    std::vector<Monkey> monkeys;
    std::string line;
    while (std::getline(input, line) && line.starts_with("Monkey ")) {
        Monkey monkey;
        monkey.parse(input);
        if (ringSize % monkey.divisor) {
            ringSize *= monkey.divisor;
        }
        monkeys.push_back(std::move(monkey));
        std::getline(input, line);
    }
    for (int round = 0; round < rounds; ++round) {
        for (Monkey& monkey : monkeys) {
            while (!monkey.items.empty()) {
                monkey.counter++;
                int64_t item = monkey.items.front();
                monkey.items.pop_front();
                switch (monkey.operation) {
                case Monkey::Operation::ADD:    item += monkey.argument; break;
                case Monkey::Operation::MUL:    item *= monkey.argument; break;
                case Monkey::Operation::SQUARE: item *= item;            break;
                default: std::terminate();
                }
                if (reduce) {
                    item /= 3;
                } else {
                    item %= ringSize;
                }
                const int nextMonkey = monkey.nextMonkey[bool(item % monkey.divisor)];
                monkeys[nextMonkey].items.push_back(item);
            }
        }
    }
    std::sort(monkeys.begin(), monkeys.end(),
              [](const auto& m1, const auto& m2) { return m1.counter > m2.counter; });
    return int64_t(monkeys[0].counter) * int64_t(monkeys[1].counter);
}

int64_t solve1(std::ifstream input) {
    return solve(std::move(input), 20, true);
}

int64_t solve2(std::ifstream input) {
    return solve(std::move(input), 10000, false);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input11.txt";
static const std::string TEST_FILE     = "inputs/test11.txt";
static const int64_t     TEST_RESULT_1 = 10605;
static const int64_t     TEST_RESULT_2 = 2713310158;

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
