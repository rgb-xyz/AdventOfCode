#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

struct Pos {
    int x = 0;
    int y = 0;

    Pos& operator|=(const Pos& other) {
        x = std::max(x, other.x);
        y = std::max(y, other.y);
        return *this;
    }
    friend std::istream& operator>>(std::istream& stream, Pos& pos) {
        char sep{};
        return stream >> pos.x >> sep >> pos.y;
    }
};

// ********************************************************************************************************

int solve(std::ifstream input, const bool withBottom) {
    std::vector<std::pair<Pos, Pos>> lines;
    Pos extent;
    for (std::string line; std::getline(input, line);) {
        std::istringstream iss(std::move(line));
        Pos from;
        iss >> from;
        extent |= from;
        for (Pos to; iss >> line >> to; from = to) {
            extent |= to;
            lines.emplace_back(from, to);
        }
    }
    if (withBottom) {
        extent |= Pos(500 + extent.y + 2, extent.y + 2);
        lines.emplace_back(Pos{ 0, extent.y }, Pos{ extent.x, extent.y });
    }

    std::vector<std::vector<bool>> map;
    map.resize(extent.x + 1, std::vector<bool>(extent.y + 1, false));
    for (auto& [from, to] : lines) {
        if (from.x > to.x || from.y > to.y) {
            std::swap(from, to);
        }
        if (from.x == to.x) {
            for (int y = from.y; y <= to.y; ++y) {
                map[from.x][y] = true;
            }
        } else {
            for (int x = from.x; x <= to.x; ++x) {
                map[x][from.y] = true;
            }
        }
    }

    try {
        for (int result = 0;; ++result) {
            const auto isFree = [&](const int x, const int y) {
                if (x < 0 || x > extent.x || y > extent.y) {
                    throw result;
                }
                return !map[x][y];
            };

            Pos sand = { 500, 0 };
            if (!isFree(sand.x, sand.y)) {
                return result;
            }
            for (;;) {
                while (isFree(sand.x, sand.y)) {
                    sand.y++;
                }
                if (isFree(sand.x-1, sand.y)) {
                    sand.x--;
                } else if (isFree(sand.x+1, sand.y)) {
                    sand.x++;
                } else {
                    map[sand.x][sand.y-1] = true;
                    break;
                }
            }
        }
    } catch (int result) {
        return result;
    }
}

int solve1(std::ifstream input) {
    return solve(std::move(input), false);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), true);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input14.txt";
static const std::string TEST_FILE     = "inputs/test14.txt";
static const int         TEST_RESULT_1 = 24;
static const int         TEST_RESULT_2 = 93;

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
