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
        const auto getLine = [&input](const std::string_view& prefix) {
            std::string line;
            if (std::getline(input, line) && line.starts_with(prefix)) {
                return line.substr(prefix.size());
            }
            std::terminate();
        };

        std::istringstream iss(getLine("  Starting items: "));
        for (int item{}; iss >> item; iss.get()) {
            items.push_back(item);
        }
        const std::string op = getLine("  Operation: new = old ");
        argument = std::atoi(op.c_str() + 1);
        switch (op[0]) {
        case '+': operation = Operation::ADD;                                break;
        case '*': operation = argument ? Operation::MUL : Operation::SQUARE; break;
        default:  std::terminate();
        }
        divisor       = std::stoi(getLine("  Test: divisible by "));
        nextMonkey[0] = std::stoi(getLine("    If true: throw to monkey "));
        nextMonkey[1] = std::stoi(getLine("    If false: throw to monkey "));
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
              [](const Monkey& m1, const Monkey& m2) { return m1.counter > m2.counter; });
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
