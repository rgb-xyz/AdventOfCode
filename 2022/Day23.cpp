#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Pos {
    int x = 0;
    int y = 0;

    Pos operator+(const Pos& p) const { return Pos{ x + p.x, y + p.y }; }

    auto operator<=>(const Pos& p) const = default;
    bool operator==(const Pos& p) const = default;

    friend Pos min(const Pos& lhs, const Pos& rhs) {
        return Pos{ std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y) };
    }
    friend Pos max(const Pos& lhs, const Pos& rhs) {
        return Pos{ std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y) };
    }
};

// ********************************************************************************************************

int solve(std::ifstream input, const bool part2) {
    std::set<Pos> map;
    std::string line;
    for (int y = 0; std::getline(input, line); ++y) {
        for (int x = 0; x < line.size(); ++x) {
            if (line[x] == '#') map.insert(Pos{ x, y });
        }
    }

    int rounds = 10;
    for (int round = 0; round < rounds; ++round) {
        std::map<Pos, std::vector<Pos>> moves;
        for (const Pos& p : map) {
            bool has[3][3]{};
            bool hasAny = false;
            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    if ((i || j) && map.contains(p + Pos{ i, j })) {
                        has[i+1][j+1] = true;
                        hasAny        = true;
                    }
                }
            }
            if (!hasAny) continue;
            [&](){
                for (int d = 0; d < 4; ++d) {
                    switch ((d + round) % 4) {
                    case 0: if (!has[0][0] && !has[1][0] && !has[2][0]) { moves[p + Pos{ 0, -1 }].push_back(p); return; } break;
                    case 1: if (!has[0][2] && !has[1][2] && !has[2][2]) { moves[p + Pos{ 0, +1 }].push_back(p); return; } break;
                    case 2: if (!has[0][0] && !has[0][1] && !has[0][2]) { moves[p + Pos{ -1, 0 }].push_back(p); return; } break;
                    case 3: if (!has[2][0] && !has[2][1] && !has[2][2]) { moves[p + Pos{ +1, 0 }].push_back(p); return; } break;
                    }
                }
                moves[p].push_back(p);
            }();
        }
        bool anyMoved = false;
        for (const auto& [to, from] : moves) {
            if (from.size() == 1) {
                map.erase(from.front());
                map.insert(to);
                anyMoved = true;
            }
        }
        if (part2) {
            if (!anyMoved) {
                return round + 1;
            } else {
                rounds++;
            }
        }
    }

    Pos lower = *map.begin();
    Pos upper = *map.begin();
    for (const Pos& p : map) {
        lower = min(lower, p);
        upper = max(upper, p);
    }
    return (upper.x - lower.x + 1) * (upper.y - lower.y + 1) - int(map.size());
}

int solve1(std::ifstream input) {
    return solve(std::move(input), false);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), true);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input23.txt";
static const std::string TEST_FILE     = "inputs/test23.txt";
static const int         TEST_RESULT_1 = 110;
static const int         TEST_RESULT_2 = 20;

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
