#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

// ********************************************************************************************************

std::vector<std::string> up, down, left, right;
std::set<int> cache;
int width;
int height;
int loopSize;

bool canGoTo(const int x, const int y, const int time) {
    return !up[(time + y) % height][x] && !down[(time - y) % height][x] &&
           !left[(time + x) % width][y] && !right[(time - x) % width][y];
}

void goTo(const int x, const int y, const int targetX, const int targetY, const int time, int& result) {
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
            if ((!i || !j) && canGoTo(x2, y2, time+1)) {
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
        while (!canGoTo(startX, startY, time)) time++;
        result = INT_MAX;
        goTo(startX, startY, targetX, targetY, time, result);
    } while (result == INT_MAX);
    return result + 1;
}

int solve(std::ifstream input, const bool part2) {
    up.clear();
    down.clear();
    left.clear();
    right.clear();
    for (std::string line; std::getline(input, line); ) {
        const std::string_view data = std::string_view(line).substr(1, line.size() - 2);
        if (data[2] == '#') {
            left.resize(data.size());
            right.resize(data.size());
            continue;
        }
        up.emplace_back();
        down.emplace_back();
        for (int i = 0; i < data.size(); ++i) {
            up  .back().push_back(data[i] == '^' ? '^' : '\0');
            down.back().push_back(data[i] == 'v' ? 'v' : '\0');
            left [i]   .push_back(data[i] == '<' ? '<' : '\0');
            right[i]   .push_back(data[i] == '>' ? '>' : '\0');
        }
    }
    width    = int(left.size());
    height   = int(up.size());
    loopSize = std::lcm(width, height);
    std::reverse(down.begin() + 1, down.end());
    std::reverse(right.begin() + 1, right.end());
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
