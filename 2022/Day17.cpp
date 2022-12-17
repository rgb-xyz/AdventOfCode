#include <array>
#include <fstream>
#include <iostream>
#include <vector>

using Row   = unsigned;
using Brick = std::array<Row, 4>;

static constexpr Row                  WALL   = 0401;
static constexpr std::array<Brick, 5> BRICKS = {
    Brick{ 0074, 0000, 0000, 0000 },
    Brick{ 0020, 0070, 0020, 0000 },
    Brick{ 0070, 0010, 0010, 0000 },
    Brick{ 0040, 0040, 0040, 0040 },
    Brick{ 0060, 0060, 0000, 0000 }
};

struct Probe {
    int inputSequence{};
    int caveHeight{};
};

// ********************************************************************************************************

std::vector<Probe> solve(std::ifstream input, const int brickCount) {
    std::string data;
    input >> data;

    int brickIndex = 0;
    int dataIndex  = 0;
    const auto getBrick = [&]() { return BRICKS[brickIndex++ % BRICKS.size()]; };
    const auto getData  = [&]() { return data[dataIndex++ % data.size()]; };

    std::vector<Row> cave;
    const auto testRow = [&](const Row row, const int height) {
        return height >= 0 && !((height < cave.size() ? cave[height] : WALL) & row);
    };
    const auto testBrick = [&](const Brick& brick, const int height) {
        for (int i = 0; i < brick.size(); ++i) {
            if (!testRow(brick[i], height + i)) return false;
        }
        return true;
    };
    const auto placeBrick = [&](const Brick& brick, const int height) {
        for (int i = 0; i < brick.size(); ++i) {
            if (!brick[i]) break;
            const int j = height + i;
            if (j == cave.size()) {
                cave.push_back(WALL);
            }
            cave[j] |= brick[i];
        }
    };

    std::vector<Probe> probe;
    probe.emplace_back(int(dataIndex % data.size()), int(cave.size()));
    while (brickIndex < brickCount) {
        Brick brick = getBrick();
        for (int height = int(cave.size()) + 3;; --height) {
            const bool moveRight = getData() == '>';
            Brick moved = brick;
            for (auto& row : moved) {
                if (moveRight) {
                    row >>= 1;
                } else {
                    row <<= 1;
                }
            }
            if (testBrick(moved, height)) {
                brick = moved;
            }
            if (!testBrick(brick, height - 1)) {
                placeBrick(brick, height);
                break;
            }
        }
        probe.emplace_back(int(dataIndex % data.size()), int(cave.size()));
    }
    return probe;
}

int64_t solve1(std::ifstream input) {
    return solve(std::move(input), 2022).back().caveHeight;
}

int64_t solve2(std::ifstream input) {
    const std::vector<Probe> probe = solve(std::move(input), 10000);

    const auto detectCycle = [&]() -> std::pair<int, int> {
        for (int i = 1; i < probe.size(); ++i) {
            for (int j = 0; j < i; ++j) {
                if (probe[j].inputSequence == probe[i].inputSequence) {
                    bool hasCycle = true;
                    for (int k = 0; k < (i - j); ++k) {
                        if (probe[j+k].inputSequence != probe[i+k].inputSequence) {
                            hasCycle = false;
                            break;
                        }
                    }
                    if (hasCycle) {
                        return { j, i - j };
                    }
                }
            }
        }
        std::terminate();
    };

    const auto [cycleStart, cycleSize] = detectCycle();
    const int64_t cycleCount = (1'000'000'000'000 - cycleStart) / cycleSize;
    const int64_t cyclePart  = (1'000'000'000'000 - cycleStart) % cycleSize;
    const int64_t height1    = probe[cycleStart].caveHeight;
    const int64_t height2    = probe[cycleStart + cycleSize].caveHeight - probe[cycleStart].caveHeight;
    const int64_t height3    = probe[cycleStart + cyclePart].caveHeight - probe[cycleStart].caveHeight;
    return height1 + cycleCount * height2 + height3;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input17.txt";
static const std::string TEST_FILE     = "inputs/test17.txt";
static const int64_t     TEST_RESULT_1 = 3068;
static const int64_t     TEST_RESULT_2 = 1514285714288;

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
