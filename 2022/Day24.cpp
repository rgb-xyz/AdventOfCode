#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

// ********************************************************************************************************

std::vector<std::string> map;
std::set<int>            cache;
int                      width;
int                      height;
int                      loopSize;

bool canGoTo(const int x, const int y, const int time) {
    constexpr auto safeMod = [](const int a, const int b) { return ((a % b) + b) % b; };

    return map[safeMod(y + time, height)][x] != '^' && map[y][safeMod(x + time, width)] != '<' && 
           map[safeMod(y - time, height)][x] != 'v' && map[y][safeMod(x - time, width)] != '>';
}

void goTo(const int x, const int y, const int targetX, const int targetY, const int time, int& result) {
    if (!canGoTo(x, y, time)) return;
    if (!cache.insert(x + width * (y + height * (time % loopSize))).second) return;
    const int distance = std::abs(targetX - x) + std::abs(targetY - y);
    if (time + distance >= result) return;
    if (distance == 0) {
        result = time;
        return;
    }
    for (int j = 1; j >= -1; --j) {
        const int y2 = y + (targetY > y ? j : -j);
        if (y2 < 0 || y2 >= height) continue;
        for (int i = 1; i >= -1; --i) {
            const int x2 = x + (targetX > x ? i : -i);
            if (x2 < 0 || x2 >= width) continue;
            if (i == 0 || j == 0) {
                goTo(x2, y2, targetX, targetY, time+1, result);
            }
        }
    }
}

int walk(const int startX, const int startY, const int targetX, const int targetY, int time) {
    cache.clear();
    int result{};
    do {
        time++;
        result = INT_MAX;
        goTo(startX, startY, targetX, targetY, time, result);
    } while (result == INT_MAX);
    return result + 1;
}

int solve(std::ifstream input, const bool part2) {
    map.clear();
    for (std::string line; std::getline(input, line); ) {
        if (line[2] != '#') {
            map.push_back(line.substr(1, line.size() - 2));
        }
    }
    width    = int(map.front().size());
    height   = int(map.size());
    loopSize = std::lcm(width, height);
    int time = walk(0, 0, width-1, height-1, 0);
    if (part2) {
        time = walk(width-1, height-1, 0, 0, time);
        time = walk(0, 0, width-1, height-1, time);
    }
    return time;
}

int solve1(std::ifstream input) {
    return solve(std::move(input), false);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), true);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input24.txt";
static const std::string TEST_FILE     = "inputs/test24.txt";
static const int         TEST_RESULT_1 = 18;
static const int         TEST_RESULT_2 = 54;

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
