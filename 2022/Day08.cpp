#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// ********************************************************************************************************

int solve1(std::ifstream input) {
    std::vector<std::string> grid;
    std::string line;
    while (std::getline(input, line)) {
        grid.push_back(std::move(line));
    }

    int result = 0;
    for (int y = 0; y < grid.size(); ++y) {
        for (int x = 0; x < grid[y].size(); ++x) {
            bool visible[4] = { true, true, true, true };

            for (int i = 0; i < grid[y].size(); ++i) {
                if (grid[y][i] >= grid[y][x]) {
                    if (i < x) {
                        visible[0] = false;
                    } else if (i > x) {
                        visible[1] = false;
                    }
                }
            }
            for (int i = 0; i < grid.size(); ++i) {
                if (grid[i][x] >= grid[y][x]) {
                    if (i < y) {
                        visible[2] = false;
                    } else if (i > y) {
                        visible[3] = false;
                    }
                }
            }

            if (visible[0] || visible[1] || visible[2] || visible[3]) {
                ++result;
            }
        }
    }
    return result;
}

int solve2(std::ifstream input) {
    std::vector<std::string> grid;
    std::string line;
    while (std::getline(input, line)) {
        grid.push_back(std::move(line));
    }

    int result = 0;
    for (int y = 0; y < grid.size(); ++y) {
        for (int x = 0; x < grid[y].size(); ++x) {
            int scores[4] = {};

            for (int i = x-1; i >= 0; --i) {
                scores[0]++;
                if (grid[y][i] >= grid[y][x]) {
                    break;
                }
            }
            for (int i = x+1; i < grid[y].size(); ++i) {
                scores[1]++;
                if (grid[y][i] >= grid[y][x]) {
                    break;
                }
            }
            for (int i = y-1; i >= 0; --i) {
                scores[2]++;
                if (grid[i][x] >= grid[y][x]) {
                    break;
                }
            }
            for (int i = y+1; i < grid.size(); ++i) {
                scores[3]++;
                if (grid[i][x] >= grid[y][x]) {
                    break;
                }
            }

            const int score = scores[0] * scores[1] * scores[2] * scores[3];
            result = std::max(result, score);
        }
    }
    return result;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input08.txt";
static const std::string TEST_FILE     = "inputs/test08.txt";
static const int         TEST_RESULT_1 = 21;
static const int         TEST_RESULT_2 = 8;

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
