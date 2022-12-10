#include <fstream>
#include <iostream>
#include <set>
#include <vector>

struct Pos {
    int x = 0;
    int y = 0;

    auto operator<=>(const Pos&) const = default;
};

// ********************************************************************************************************

int solve(std::ifstream input, const int length) {
    const auto sign = [](const int x) {
        return x > 0 ? 1 : (x < 0 ? -1 : 0);
    };

    char dir{};
    int  steps{};
    std::vector<Pos> knots(length);
    std::set<Pos>    positions;
    positions.insert(knots.back());
    while (input >> dir >> steps) {
        for (; steps > 0; --steps) {
            switch (dir) {
            case 'L': knots.front().x--; break;
            case 'R': knots.front().x++; break;
            case 'U': knots.front().y--; break;
            case 'D': knots.front().y++; break;
            default: std::terminate();
            }
            for (size_t i = 1; i < knots.size(); ++i) {
                const auto& head = knots[i-1];
                auto&       tail = knots[i];
                if (tail.x < head.x - 1) {
                    tail.x++;
                    tail.y += sign(head.y - tail.y);
                } else if (tail.x > head.x + 1) {
                    tail.x--;
                    tail.y += sign(head.y - tail.y);
                } else if (tail.y < head.y - 1) {
                    tail.y++;
                    tail.x += sign(head.x - tail.x);
                } else if (tail.y > head.y + 1) {
                    tail.y--;
                    tail.x += sign(head.x - tail.x);
                }
            }
            positions.insert(knots.back());
        }
    }
    return int(positions.size());
}

int solve1(std::ifstream input) {
    return solve(std::move(input), 2);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), 10);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input09.txt";
static const std::string TEST_FILE     = "inputs/test09.txt";
static const int         TEST_RESULT_1 = 88;
static const int         TEST_RESULT_2 = 36;

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
