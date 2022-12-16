#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <memory>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>

using int64 = int64_t;

// ********************************************************************************************************

int task01A(FILE* file) {
    int result = 0;
    int lastDepth{};
    bool firstDepth = true;
    while (!feof(file)) {
        int depth{};
        fscanf_s(file, "%d", &depth);
        if (!firstDepth) {
            if (depth > lastDepth) {
                ++result;
            }
        } else {
            firstDepth = false;
        }
        lastDepth = depth;
    }
    return result;
}

int task01B(FILE* file) {
    int result = 0;
    int counter = 0;
    int depths[3] = {};
    int depthSum = 0;
    int lastSum = 0;
    while (!feof(file)) {
        const int indx2 = counter % 3;
        ++counter;
        int depth{};
        fscanf_s(file, "%d", &depth);
        depthSum += depth - depths[indx2];
        depths[indx2] = depth;
        if (counter > 3 && depthSum > lastSum) {
            ++result;
        }
        lastSum = depthSum;
    }
    return result;
}

// ********************************************************************************************************

int task02A(FILE* file) {
    int position = 0;
    int depth = 0;
    while (!feof(file)) {
        char command[32];
        int value{};
        fscanf_s(file, "%s %d", command, unsigned(sizeof(command)), &value);
        if (!strcmp(command, "forward")) {
            position += value;
        } else if (!strcmp(command, "down")) {
            depth += value;
        } else if (!strcmp(command, "up")) {
            depth -= value;
        } else {
            abort();
        }
        if (depth < 0) {
            printf("Careful!\n");
            depth = 0;
        }
    }
    printf("%d * %d = ", position, depth);
    return position * depth;
}

int task02B(FILE* file) {
    int position = 0;
    int depth = 0;
    int aim = 0;
    while (!feof(file)) {
        char command[32];
        int value{};
        fscanf_s(file, "%s %d", command, unsigned(sizeof(command)), &value);
        if (!strcmp(command, "forward")) {
            position += value;
            depth += aim * value;
        } else if (!strcmp(command, "down")) {
            aim += value;
        } else if (!strcmp(command, "up")) {
            aim -= value;
        } else {
            abort();
        }
        if (depth < 0) {
            printf("Careful!\n");
            depth = 0;
        }
    }
    printf("%d * %d = ", position, depth);
    return position * depth;
}

// ********************************************************************************************************

int task03A(FILE* file) {
    static constexpr int BITS = 12;
    int bits[BITS] = {};
    int lines = 0;
    while (!feof(file)) {
        char input[BITS+1];
        fscanf_s(file, "%s", input, unsigned(sizeof(input)));
        for (int i = 0; i < BITS; ++i) {
            if (input[i] == '1') {
                ++bits[i];
            }
        }
        ++lines;
    }
    int gamma = 0;
    for (int i = 0; i < BITS; ++i) {
        gamma <<= 1;
        const int threshold = 2 * bits[i];
        if (threshold > lines) {
            gamma |= 1;
        } else if (threshold == lines) {
            printf("Careful!\n");
        }
    }
    const int epsilon = gamma ^ ((1 << BITS) - 1);
    printf("%.3X * %.3X = ", unsigned(gamma), unsigned(epsilon));
    return gamma * epsilon;
}

int task03B(FILE* file) {
    static constexpr int BITS = 12;
    std::vector<int> data;
    while (!feof(file)) {
        char input[BITS+1];
        fscanf_s(file, "%s", input, unsigned(sizeof(input)));
        data.push_back(std::stoi(input, nullptr, 2));
    }
    std::sort(data.begin(), data.end());

    const auto findData = [&](auto begin, auto end, const auto pred) {
        int lookup = 0;
        for (int i = 0; i < BITS; ++i) {
            if (begin + 1 == end) {
                printf("ONE%c%d!\n", pred(0, 1) ? '+' : '-', i);
                return *begin;
            }
            if (begin == end) {
                printf("FCK%d!\n", i);
                abort();
            }
            const int  bit = (1 << (BITS - 1 - i));
            const auto it  = pred(0, 1) 
                ? std::lower_bound(begin, end, lookup | bit, pred)
                : std::upper_bound(begin, end, lookup | bit, pred);
            if (pred(int(end - it), int(it - begin))) {
                end = it;
                if (pred(1, 0)) {
                    lookup |= bit;
                }
            } else {
                begin = it;
                if (pred(0, 1)) {
                    lookup |= bit;
                }
            }
        }
        if (begin == end) {
            printf("FCK!\n");
            abort();
        }
        if (begin + 1 != end) {
            printf("Careful!-%d\n", int(end - begin));
        }
        return *begin;
    };

    const int rateO2  = findData(data.begin(), data.end(), std::less<int>{});
    const int rateCO2 = findData(data.rbegin(), data.rend(), std::greater<int>{});
    printf("%.3X * %.3X = ", unsigned(rateO2), unsigned(rateCO2));
    return rateO2 * rateCO2;
}


// ********************************************************************************************************

int task04(FILE* file, bool last) {
    static constexpr int NUMBERS    = 100;
    static constexpr int BOARD_SIZE = 5;
    int orders[NUMBERS] = {};
    int order = 0;
    for (int i = 0; i < NUMBERS; ++i) {
        int number{};
        fscanf_s(file, "%d,", &number);
        if (number < 0 || number >= NUMBERS || orders[number] > 0) {
            abort();
        }
        orders[number] = i + 1;
    };

    int bestTime = last ? 0 : NUMBERS;
    int bestScore{};
    while (!feof(file)) {
        int boardN[BOARD_SIZE * BOARD_SIZE];
        int boardO[BOARD_SIZE * BOARD_SIZE];
        for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
            int number{};
            fscanf_s(file, "%d", &number);
            if (number < 0 || number >= NUMBERS) {
                abort();
            }
            boardN[i] = number;
            boardO[i] = orders[number];
        }
        int completion[2 * BOARD_SIZE] = {};
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                completion[2 * i]     = std::max(completion[2 * i]    , boardO[j + i * BOARD_SIZE]);
                completion[2 * i + 1] = std::max(completion[2 * i + 1], boardO[i + j * BOARD_SIZE]);
            }
        }
        int time = completion[0];
        for (int i = 0; i < 2 * BOARD_SIZE; ++i) {
            time = std::min(time, completion[i]);
        }
        printf("Board completed in %d numbers.\n", time);
        if (last ? time > bestTime : time < bestTime) {
            int overNumbers = 0;
            int lastNumber  = 0;
            for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
                if (boardO[i] > time) {
                    overNumbers += boardN[i];
                } else if (boardO[i] == time) {
                    lastNumber = boardN[i];
                }
            }
            bestTime = time;
            bestScore = overNumbers * lastNumber;
        }
    }
    return bestScore;
}

// ********************************************************************************************************

int task05(FILE* file, bool allowDiagonal) {
    static constexpr int GRID_SIZE = 1000;
    std::unique_ptr<int[]> grid{ new int[GRID_SIZE * GRID_SIZE] };
    memset(grid.get(), 0, sizeof(int[GRID_SIZE * GRID_SIZE]));

    int result = 0;
    while (!feof(file)) {
        int x1, y1, x2, y2;
        if (fscanf_s(file, "%d,%d -> %d,%d", &x1, &y1, &x2, &y2) < 0) {
            break;
        }
        if (std::min({ x1, y1, x2, y2 }) < 0 || std::max({ x1, y1, x2, y2 }) >= GRID_SIZE) {
            abort();
        }
        if (x1 == x2) {
            if (y1 > y2) {
                std::swap(y1, y2);
            }
            const int x = x1;
            for (int y = y1; y <= y2; ++y) {
                const int count = ++grid[x + y * GRID_SIZE];
                if (count == 2) {
                    ++result;
                }
            }
        } else if (y1 == y2) {
            if (x1 > x2) {
                std::swap(x1, x2);
            }
            const int y = y1;
            for (int x = x1; x <= x2; ++x) {
                const int count = ++grid[x + y * GRID_SIZE];
                if (count == 2) {
                    ++result;
                }
            }
        } else if (allowDiagonal) {
            if (std::abs(x1 - x2) != std::abs(y1 - y2)) {
                abort();
            }
            if (x1 > x2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }
            int y = y1;
            const int pitch = y2 > y1 ? 1 : -1;
            for (int x = x1; x <= x2; ++x,(y+=pitch)) {
                const int count = ++grid[x + y * GRID_SIZE];
                if (count == 2) {
                    ++result;
                }
            }
        }
    }
    return result;
}

// ********************************************************************************************************

int64 task06(FILE* file, int days) {
    int64 counts[9] = {};
    while (!feof(file)) {
        int counter{};
        if (fscanf_s(file, "%d,", &counter) < 0) {
            break;
        }
        if (counter < 0 || counter > 8) {
            abort();
        }
        ++counts[counter];
    }
    for (int day = 0; day < days; ++day) {
        const int64 first = counts[0];
        for (int i = 0; i < 8; ++i) {
            counts[i] = counts[i+1];
        }
        counts[6] += first;
        counts[8] = first;
    }
    int64 totalCount = 0;
    for (int i = 0; i < 9; ++i) {
        totalCount += counts[i];
    }
    return totalCount;
}

// ********************************************************************************************************

int64 task07(FILE* file, bool quad) {
    int counts[2000] = {};
    while (!feof(file)) {
        int position{};
        if (fscanf_s(file, "%d,", &position) < 0) {
            break;
        }
        if (position < 0 || position >= 2000) {
            abort();
        }
        ++counts[position];
    }
    int best{};
    for (int i = 0; i < 2000; ++i) {
        int fuel = 0;
        for (int j = 0; j < 2000; ++j) {
            int cost = abs(i - j);
            if (quad) {
                cost = cost * (cost + 1) / 2;
            }
            fuel += counts[j] * cost;
        }
        if (i == 0 || fuel < best) {
            best = fuel;
        }
    }
    return best;
}

// ********************************************************************************************************

int64 task08A(FILE* file) {
    int result = 0;
    while (!feof(file)) {
        char input[8];
        for (int i = 0; i < 10; ++i) {
            if (fscanf_s(file, "%s", &input, unsigned(sizeof(input))) < 0) {
                break;
            }
        }
        if (fscanf_s(file, "%s", &input, unsigned(sizeof(input))) < 0) {
            break;
        }
        for (int i = 0; i < 4; ++i) {
            if (fscanf_s(file, "%s", &input, unsigned(sizeof(input))) < 0) {
                break;
            }
            switch (strlen(input)) {
            case 2:
            case 3:
            case 4:
            case 7:
                ++result;
            }
        }
    }
    return result;
}

int64 task08B(FILE* file) {
    const auto decode = [](const char* input) {
        int result = 0;
        for (const char* c = input; *c; ++c) {
            result |= 1 << (*c - 'a');
        }
        return (result << 8) | int(strlen(input));
    };
    const auto hasBits = [](const int a, const int b) {
        return ((a >> 8) & (b >> 8)) == (b >> 8);
    };

    int result = 0;
    while (!feof(file)) {
        char input[8];
        int inputs[10];
        int digits[10] = {};
        for (int i = 0; i < 10; ++i) {
            if (fscanf_s(file, "%s", &input, unsigned(sizeof(input))) < 0) {
                break;
            }
            inputs[i] = decode(input);
            switch (inputs[i] & 0xff) {
            case 2:
                digits[1] = inputs[i];
                inputs[i] = 0;
                break;
            case 3:
                digits[7] = inputs[i];
                inputs[i] = 0;
                break;
            case 4:
                digits[4] = inputs[i];
                inputs[i] = 0;
                break;
            case 7:
                digits[8] = inputs[i];
                inputs[i] = 0;
                break;
            }
        }
        if (fscanf_s(file, "%s", &input, unsigned(sizeof(input))) < 0) {
            break;
        }
        for (int i = 0; i < 10; ++i) {
            const int length = inputs[i] & 0xff;
            if (length == 5) {
                if (hasBits(inputs[i], digits[1])) {
                    digits[3] = inputs[i];
                    inputs[i] = 0;
                }
            } else if (length == 6) {
                if (hasBits(inputs[i], digits[4])) {
                    digits[9] = inputs[i];
                    inputs[i] = 0;
                }
            }
        }
        for (int i = 0; i < 10; ++i) {
            const int length = inputs[i] & 0xff;
            if (length == 6) {
                if (hasBits(inputs[i], digits[7])) {
                    digits[0] = inputs[i];
                } else {
                    digits[6] = inputs[i];
                }
                inputs[i] = 0;
            }
        }
        for (int i = 0; i < 10; ++i) {
            const int length = inputs[i] & 0xff;
            if (length == 5) {
                if (hasBits(digits[6], inputs[i])) {
                    digits[5] = inputs[i];
                } else {
                    digits[2] = inputs[i];
                }
                inputs[i] = 0;
            }
        }
        printf("Digits:");
        for (int i = 0; i < 10; ++i) {
            printf(" %.4X", digits[i]);
        }
        int value = 0;
        for (int i = 0; i < 4; ++i) {
            value *= 10;
            if (fscanf_s(file, "%s", &input, unsigned(sizeof(input))) < 0) {
                break;
            }
            const int decoded = decode(input);
            for (int i = 0; i < 10; ++i) {
                if (digits[i] == decoded) {
                    value += i;
                    break;
                }
            }
        }
        printf(" = %.4d\n", value);
        result += value;
    }
    return result;
}

// ********************************************************************************************************

int64 task09A(FILE* file) {
    static constexpr int GRID_SIZE = 100;
    char data[GRID_SIZE * GRID_SIZE + 1];
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (fscanf_s(file, "%s", &data[i * GRID_SIZE], GRID_SIZE + 1) < 0) {
            abort();
        }
    }
    int result = 0;
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            const char d = data[x + y * GRID_SIZE];
            const char dl = x > 0 ? data[(x - 1) + y * GRID_SIZE] : d + 1;
            const char dr = x + 1 < GRID_SIZE ? data[(x + 1) + y * GRID_SIZE] : d + 1;
            const char du = y > 0 ? data[x + (y - 1) * GRID_SIZE] : d + 1;
            const char dd = y + 1 < GRID_SIZE ? data[x + (y + 1) * GRID_SIZE] : d + 1;
            if (d < dl && d < dr && d < du && d < dd) {
                result += 1 + int(d - '0');
            }
        }
    }
    return result;
}

int64 task09B(FILE* file) {
    static constexpr int GRID_SIZE = 100;
    char data_[GRID_SIZE * GRID_SIZE + 1];
    const auto data = [&](const int x, const int y) -> char& {
        return data_[x + y * GRID_SIZE];
    };


    for (int i = 0; i < GRID_SIZE; ++i) {
        if (fscanf_s(file, "%s", &data_[i * GRID_SIZE], GRID_SIZE + 1) < 0) {
            abort();
        }
    }
    std::vector<int> basins;
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (data(x, y) < '9') {
                std::vector<std::pair<int, int>> coords;
                coords.push_back({x, y});
                int size = 0;
                for (size_t i = 0; i < coords.size(); ++i) {
                    const int xx = coords[i].first;
                    const int yy = coords[i].second;
                    if (data(xx, yy) == '9') {
                        continue;
                    }
                    ++size;
                    data(xx, yy) = '9';
                    if (xx > 0 && data(xx - 1, yy) < '9') {
                        coords.push_back({xx - 1, yy});
                    }
                    if (xx + 1 < GRID_SIZE && data(xx + 1, yy) < '9') {
                        coords.push_back({xx + 1, yy});
                    }
                    if (yy > 0 && data(xx, yy - 1) < '9') {
                        coords.push_back({xx, yy - 1});
                    }
                    if (yy + 1 < GRID_SIZE && data(xx, yy + 1) < '9') {
                        coords.push_back({xx, yy + 1});
                    }
                }
                basins.push_back(size);
            }
        }
    }
    std::sort(basins.begin(), basins.end(), std::greater<int>{});
    return basins[0] * basins[1] * basins[2];
}

// ********************************************************************************************************

int64 task10(FILE* file, bool complete) {
    int score = 0;
    std::vector<int64> completeScores;
    while (!feof(file)) {
        char line[200];
        if (fscanf_s(file, "%s", line, unsigned(sizeof(line))) < 0) {
            break;
        }
        std::vector<char> chars;
        for (const char* c = line; *c; ++c) {
            if (*c == '(') {
                chars.push_back(')');
                continue;
            }
            if (*c == '{') {
                chars.push_back('}');
                continue;
            }
            if (*c == '[') {
                chars.push_back(']');
                continue;
            }
            if (*c == '<') {
                chars.push_back('>');
                continue;
            }
            if (!chars.empty() && chars.back() == *c) {
                chars.pop_back();
                continue;
            }
            switch (*c) {
            case ')': score += 3; break;
            case ']': score += 57; break;
            case '}': score += 1197; break;
            case '>': score += 25137; break;
            default: abort();
            }
            chars.clear();
            break;
        }
        if (complete && !chars.empty()) {
            int64 completeScore = 0;
            std::reverse(chars.begin(), chars.end());
            for (char c : chars) {
                completeScore *= 5;
                switch (c) {
                case ')': completeScore += 1; break;
                case ']': completeScore += 2; break;
                case '}': completeScore += 3; break;
                case '>': completeScore += 4; break;
                default: abort();
                }
            }
            completeScores.push_back(completeScore);
        }
    };
    if (complete) {
        if (!(completeScores.size() % 2)) {
            abort();
        }
        std::sort(completeScores.begin(), completeScores.end());
        return completeScores[completeScores.size() / 2];
    }
    return score;
}

// ********************************************************************************************************

int64 task11(FILE* file, bool sync) {
    static constexpr int GRID_SIZE = 10;
    static constexpr int STEPS     = 100;
    char grid[GRID_SIZE * GRID_SIZE + 1];
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (fscanf_s(file, "%s", &grid[i * GRID_SIZE], GRID_SIZE + 1) < 0) {
            break;
        }
    };
    int flashes = 0;
    for (int step = 0; step < STEPS || sync; ++step) {
        std::deque<std::pair<int, int>> steps;
        for (int y = 0; y < GRID_SIZE; ++y) {
            for (int x = 0; x < GRID_SIZE; ++x) {
                steps.push_back({ x, y });
            }
        }
        if (sync) {
            flashes = 0;
        }
        while (!steps.empty()) {
            const auto [x, y] = steps.front();
            steps.pop_front();
            char& data = grid[x + y * GRID_SIZE];
            if (data > '9') {
                continue;
            }
            ++data;
            if (data > '9') {
                ++flashes;
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        if (i || j) {
                            if (x + i >= 0 && x + i < GRID_SIZE && y + j >= 0 && y + j < GRID_SIZE) {
                                steps.push_back({ x + i, y + j });
                            }
                        }
                    }
                }
            }
        }
        if (sync && flashes >= 100) {
            return step + 1;
        }
        for (int y = 0; y < GRID_SIZE; ++y) {
            for (int x = 0; x < GRID_SIZE; ++x) {
                char& data = grid[x + y * GRID_SIZE];
                if (data > '9') {
                    data = '0';
                }
            }
        }
    }
    return flashes;
}

// ********************************************************************************************************

struct Node12 {
    enum Type {
        SMALL,
        BIG,
        START,
        END
    };

    std::string          name;
    std::vector<Node12*> nodes;
    Type                 type;
    bool                 inside = false;

    explicit Node12(std::string name_): name(std::move(name_)) {
        if (name == "start") {
            type = START;
        } else if (name == "end") {
            type = END;
        } else {
            type = std::islower(name[0]) ? SMALL : BIG;
        }
    }
};

void visit12(Node12* node, int& paths, bool allowDuplicates) {
    if (node->inside && (node->type != Node12::SMALL || !allowDuplicates)) {
        return;
    }
    if (node->type == Node12::END) {
        ++paths;
        return;
    }

    const bool inside = node->inside;
    if (node->type != Node12::BIG) {
        if (node->inside) {
            allowDuplicates = false;
        } else {
            node->inside = true;
        }
    }
    for (Node12* n : node->nodes) {
        visit12(n, paths, allowDuplicates);
    }
    node->inside = inside;
}

int64 task12(FILE* file, const bool allowDuplicates) {
    Node12*                                  start = nullptr;
    std::unordered_map<std::string, Node12*> lookup;
    const auto getNode = [&](const std::string& name) {
        auto it = lookup.find(name);
        if (it != lookup.end()) {
            return it->second;
        }
        auto* node = new Node12(name);
        lookup.insert({ name, node });
        return node;
    };

    while (!feof(file)) {
        char input[64];
        if (fscanf_s(file, "%s", input, unsigned(sizeof(input))) < 0) {
            break;
        }
        std::string str(input);
        auto pos = str.find('-');
        assert(pos != std::string::npos);
        Node12* node1 = getNode(str.substr(0, pos));
        Node12* node2 = getNode(str.substr(pos + 1));
        node1->nodes.push_back(node2);
        node2->nodes.push_back(node1);
        if (!start) {
            if (node1->type == Node12::START) {
                start = node1;
            } else if (node2->type == Node12::START) {
                start = node2;
            }
        }
    }
    int paths = 0;
    visit12(start, paths, allowDuplicates);
    return paths;
}

// ********************************************************************************************************

int64 task13(FILE* file, bool dump) {
    std::vector<std::pair<int, int>> dots;
    while (!feof(file)) {
        int x{}, y{};
        if (fscanf_s(file, "%d,%d", &x, &y) <= 0) {
            break;
        }
        dots.push_back({ x, y });
    }
    std::vector<std::pair<int, bool>> folds;
    while (!feof(file)) {
        char axis{};
        int line{};
        if (fscanf_s(file, "fold along %c=%d\n", &axis, 1, &line) < 0) {
            break;
        }
        folds.push_back({ line, axis == 'y' });
        if (!dump) {
            break;
        }
    }
    std::set<std::pair<int, int>> set;
    int sizeX = 0, sizeY = 0;
    for (auto [x, y] : dots) {
        for (const auto& [line, axisY] : folds) {
            int& axis = axisY ? y : x;
            (axisY ? sizeY : sizeX) = line;
            if (axis > line) {
                axis = 2 * line - axis;
            }
        }
        set.insert({ x, y });
    }
    if (dump) {
        for (int y = 0; y < sizeY; ++y) {
            for (int x = 0; x < sizeX; ++x) {
                printf(set.contains({x, y}) ? "#" : " ");
            }
            printf("\n");
        }
    }
    return set.size();
}

// ********************************************************************************************************

int64 task14(FILE* file, int steps) {
    char input[32];
    if (fscanf_s(file, "%s\n", input, unsigned(sizeof(input))) <= 0) {
        abort();
    }
    std::unordered_map<int, int> rules;
    while (!feof(file)) {
        char a, b, c;
        if (fscanf_s(file, "%c%c -> %c\n", &a, 1, &b, 1, &c, 1) <= 0) {
            break;
        }
        rules.insert({ (int(a - 'A') << 8)  | int(b - 'A'), int(c - 'A') });
    }
    int64 counts[26][26] = {};
    for (const char* c = input; *c; ++c) {
        if (!c[1]) {
            counts[c[0] - 'A'][0]++;
            break;
        }
        counts[c[0] - 'A'][c[1] - 'A']++;
    }
    for (int step = 0; step < steps; ++step) {
        int64 newCounts[26][26] = {};
        for (int y = 0; y < 26; ++y) {
            for (int x = 0; x < 26; ++x) {
                const int64 count = counts[x][y];
                if (!count) {
                    continue;
                }
                const auto it = rules.find((x << 8) | y);
                if (it == rules.end()) {
                    newCounts[x][y] += count;
                } else {
                    newCounts[x][it->second] += count;
                    newCounts[it->second][y] += count;
                }
            }
        }
        memcpy(counts, newCounts, sizeof(counts));
    }
    int64 min = INT64_MAX;
    int64 max = 0;
    for (int x = 0; x < 26; ++x) {
        int64 sum = 0;
        for (int y = 0; y < 26; ++y) {
            sum += counts[x][y];
        }
        if (sum) {
            min = std::min(min, sum);
            max = std::max(max, sum);
        }
    }
    return max - min;
}

// ********************************************************************************************************

int64 task15(FILE* file, const int multiplier) {
    static constexpr int DATA_SIZE = 100;
    char data_[DATA_SIZE * DATA_SIZE + 1];
    const auto data = [&](const int x, const int y) -> int {
        const int multX = x / DATA_SIZE;
        const int multY = y / DATA_SIZE;
        const int value = data_[(x % DATA_SIZE) + (y % DATA_SIZE) * DATA_SIZE] - '1';
        return 1 + ((value + multX + multY) % 9);
    };

    for (int i = 0; i < DATA_SIZE; ++i) {
        if (fscanf_s(file, "%s", &data_[i * DATA_SIZE], DATA_SIZE + 1) < 0) {
            abort();
        }
    }
    const int MAP_SIZE = DATA_SIZE * multiplier;
    std::unique_ptr<int[]> paths{ new int[MAP_SIZE * MAP_SIZE] };
    memset(paths.get(), 0, sizeof(int) * MAP_SIZE * MAP_SIZE);
    const auto path = [&](const int x, const int y) -> int& {
        return paths[x + y * MAP_SIZE];
    };
    for (int size = 1; size < MAP_SIZE; ++size) {
        std::deque<std::pair<int, int>> opt;
        for (int i = 0; i < size; ++i) {
            path(i, size) = path(i, size-1) + data(i, size);
            path(size, i) = path(size-1, i) + data(size, i);
            opt.push_back({ i, size });
            opt.push_back({ size, i });
        }
        path(size, size) = std::min(path(size, size-1), path(size-1, size)) + data(size, size);
        opt.push_back({ size, size });
        while (!opt.empty()) {
            const auto [x, y] = opt.front();
            opt.pop_front();
            const int pathVal = path(x, y);
            if (x > 0 && path(x-1, y) > pathVal + data(x-1, y)) {
                path(x-1, y) = pathVal + data(x-1, y);
                opt.push_back({ x-1, y });
            }
            if (x < size && path(x+1, y) > pathVal + data(x+1, y)) {
                path(x+1, y) = pathVal + data(x+1, y);
                opt.push_back({ x+1, y });
            }
            if (y > 0 && path(x, y-1) > pathVal + data(x, y-1)) {
                path(x, y-1) = pathVal + data(x, y-1);
                opt.push_back({ x, y-1 });
            }
            if (y < size && path(x, y+1) > pathVal + data(x, y+1)) {
                path(x, y+1) = pathVal + data(x, y+1);
                opt.push_back({ x, y+1 });
            }
        }

    }
    return path(MAP_SIZE-1, MAP_SIZE-1);
}

// ********************************************************************************************************

int read16(std::vector<bool>::const_iterator& it, const int bits) {
    int result = 0;
    for (int i = 0; i < bits; ++i) {
        result <<= 1;
        if (it[i]) {
            result |= 1;
        }
    }
    it += bits;
    return result;
}

int handle16(std::vector<bool>::const_iterator& it, std::vector<bool>::const_iterator end, std::vector<int64>& vals) {
    if (end - it < 6) {
        it = end;
        return 0;
    }
    int version = read16(it, 3);
    const int id = read16(it, 3);
    if (id == 4) {
        int64 value = 0;
        for (;;) {
            const int hasMore = read16(it, 1);
            value <<= 4;
            value |= read16(it, 4);
            if (!hasMore) break;
        }
        vals.push_back(value);
        printf("%lld ", value);
        return version;
    }
    switch (id) {
    case 0: printf("sum("); break;
    case 1: printf("mul("); break;
    case 2: printf("min("); break;
    case 3: printf("max("); break;
    case 5: printf("gt("); break;
    case 6: printf("lt("); break;
    case 7: printf("eq("); break;
    default: abort();
    }
    std::vector<int64> ops;
    if (read16(it, 1)) {
        const int packets = read16(it, 11);
        for (int i = 0; i < packets; ++i) {
            version += handle16(it, end, ops);
        }
    } else {
        const int bytes = read16(it, 15);
        const auto end2 = it + bytes;
        while (it < end2) {
            version += handle16(it, end2, ops);
        }
    }
    printf(")");
    switch (id) {
    case 0: vals.push_back(std::accumulate(ops.begin(), ops.end(), int64(0), std::plus<int64>{})); break;
    case 1: vals.push_back(std::accumulate(ops.begin(), ops.end(), int64(1), std::multiplies<int64>{})); break;
    case 2: vals.push_back(*std::min_element(ops.begin(), ops.end())); break;
    case 3: vals.push_back(*std::max_element(ops.begin(), ops.end())); break;
    case 5: vals.push_back(ops[0] > ops[1]); break;
    case 6: vals.push_back(ops[0] < ops[1]); break;
    case 7: vals.push_back(ops[0] == ops[1]); break;
    default: abort();
    }
    return version;
}

int64 task16(FILE* file, const bool eval) {
    std::vector<bool> bits;
    while (!feof(file)) {
        char c = fgetc(file);
        int value{};
        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            value = 10 + (c - 'A');
        } else {
            break;
        }
        bits.push_back(value & 0x08);
        bits.push_back(value & 0x04);
        bits.push_back(value & 0x02);
        bits.push_back(value & 0x01);
    }
    std::vector<bool>::const_iterator it  = bits.begin();
    std::vector<bool>::const_iterator end = bits.end();
    std::vector<int64> vals;
    const int sum = handle16(it, end, vals);
    assert(vals.size() == 1);
    printf("\n");
    return eval ? vals.front() : sum;
}

// ********************************************************************************************************

int64 task17(FILE* file, const bool more) {
    const int minX =  217;
    const int maxX =  240;
    const int minY = -126;
    const int maxY =  -69;

    int result = 0;
    if (!more) {
        int steps = 21;
        for (int y = 1; y < 1000; ++y) {
            int depth = steps * y - steps * (steps - 1) / 2;
            if (depth < minY) continue;
            while (depth > maxY) {
                ++steps;
                depth = steps * y - steps * (steps - 1) / 2;
            }
            if (depth < minY) continue;
            for (int i = 1; i < steps; ++i) {
                const int d = i * y - i * (i - 1) / 2;
                if (d > result) {
                    result = d;
                }
            }
        }
    } else {
        for (int y = minY; y <= -2*minY; ++y) {
            for (int x = 1; x <= maxX; ++x) {
                for (int stepsY = 1;; ++stepsY) {
                    const int stepsX = std::min(stepsY, x);
                    const int xx = stepsX * x - stepsX * (stepsX - 1) / 2;
                    const int yy = stepsY * y - stepsY * (stepsY - 1) / 2;
                    if (xx > maxX || yy < minY) {
                        break;
                    }
                    if (xx >= minX && yy <= maxY) {
                        printf("%d,%d -(%d)-> %d,%d\n", x, y, stepsY, xx, yy);
                        ++result;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

// ********************************************************************************************************

class Number;
using NumberRef = std::unique_ptr<Number>;

class Number {
public:
    virtual ~Number() {}

    virtual NumberRef clone() const = 0;
    virtual void parse(FILE* file) = 0;
    virtual void print(FILE* file) const = 0;
    virtual int64 getMagnitude() const = 0;
    virtual bool reduce(std::vector<NumberRef*>& backtrace, int action) = 0;
    virtual bool scanValues(std::vector<int*>& values, const Number* split) = 0;

    static NumberRef create(FILE* file);
    static NumberRef sum(NumberRef left, NumberRef right);
};

class PairNumber : public Number {
    NumberRef mLeft;
    NumberRef mRight;

public:
    PairNumber() = default;
    PairNumber(NumberRef left, NumberRef right): mLeft(std::move(left)), mRight(std::move(right)) {
        assert(mLeft && mRight);
    }

    virtual NumberRef clone() const {
        return NumberRef(new PairNumber(mLeft->clone(), mRight->clone()));
    }
    virtual void parse(FILE* file) {
        if (getc(file) != '[') {
            abort();
        }
        mLeft = Number::create(file);
        if (getc(file) != ',') {
            abort();
        }
        mRight = Number::create(file);
        if (getc(file) != ']') {
            abort();
        }
    }
    virtual void print(FILE* file) const {
        fprintf(file, "[");
        mLeft->print(file);
        fprintf(file, ",");
        mRight->print(file);
        fprintf(file, "]");
    }
    virtual int64 getMagnitude() const {
        return 3 * mLeft->getMagnitude() + 2 * mRight->getMagnitude();
    }
    virtual bool reduce(std::vector<NumberRef*>& backtrace, int action);
    virtual bool scanValues(std::vector<int*>& values, const Number* split);
};

class SimpleNumber : public Number {
    int mValue{};

public:
    SimpleNumber() = default;
    explicit SimpleNumber(const int value): mValue(value) {}

    int getValue() const { return mValue; }

    virtual NumberRef clone() const {
        return NumberRef(new SimpleNumber(mValue));
    }
    virtual void parse(FILE* file) {
        if (fscanf_s(file, "%d", &mValue) <= 0) {
            abort();
        }
    }
    virtual void print(FILE* file) const {
        fprintf(file, "%d", mValue);
    }
    virtual int64 getMagnitude() const {
        return mValue;
    }
    virtual bool reduce(std::vector<NumberRef*>& backtrace, int action);
    virtual bool scanValues(std::vector<int*>& values, const Number* split);
};

bool PairNumber::reduce(std::vector<NumberRef*>& backtrace, const int action) {
    if (backtrace.size() > 4) {
        assert(action == 1);
        const int left  = dynamic_cast<SimpleNumber*>(mLeft.get())->getValue();
        const int right = dynamic_cast<SimpleNumber*>(mRight.get())->getValue();
        std::vector<int*> values;
        values.push_back(nullptr);
        (*backtrace.front())->scanValues(values, this);
        assert(values.size() == 2);
        if (values[0]) {
            *values[0] += left;
        }
        if (values[1]) {
            *values[1] += right;
        }
        backtrace.back()->reset(new SimpleNumber{});
        return true;
    }
    backtrace.push_back(&mLeft);
    bool reduced = mLeft->reduce(backtrace, action);
    assert(backtrace.back() == &mLeft);
    if (!reduced) {
        backtrace.back() = &mRight;
        reduced = mRight->reduce(backtrace, action);
        assert(backtrace.back() == &mRight);
    }
    backtrace.pop_back();
    return reduced;
}

bool PairNumber::scanValues(std::vector<int*>& values, const Number* split) {
    if (split == this) {
        values.push_back(nullptr);
        return false;
    }
    return mLeft->scanValues(values, split) || mRight->scanValues(values, split);
}

bool SimpleNumber::reduce(std::vector<NumberRef*>& backtrace, const int action) {
    if (mValue < 10 || action != 2) {
        return false;
    }
    const int left  = mValue / 2;
    const int right = mValue - left;
    backtrace.back()->reset(new PairNumber(NumberRef(new SimpleNumber(left)), NumberRef(new SimpleNumber(right))) );
    return true;
}

bool SimpleNumber::scanValues(std::vector<int*>& values, const Number* split) {
    values.back() = &mValue;
    return values.size() > 1;
}

NumberRef Number::create(FILE* file) {
    char scan = ' ';
    while (std::isspace(scan)) {
        scan = getc(file);
    }
    ungetc(scan, file);
    NumberRef result;
    if (scan == '[') {
        result.reset(new PairNumber{});
    } else if (std::isdigit(scan)) {
        result.reset(new SimpleNumber{});
    } else {
        abort();
    }
    result->parse(file);
    return result;
}

NumberRef Number::sum(NumberRef left, NumberRef right) {
    NumberRef result{ new PairNumber(std::move(left), std::move(right)) };
    //printf("   ");
    //result->print(stdout);
    std::vector<NumberRef*> backtrace;
    backtrace.push_back(&result);
    while (result->reduce(backtrace, 1) || result->reduce(backtrace, 2)) {
        assert(backtrace.size() == 1);
        //printf("\n-> ");
        //result->print(stdout);
    }
    //printf("\n");
    return result;
}

int64 task18A(FILE* file) {
    NumberRef sum = Number::create(file);
    while (!feof(file)) {
        sum = Number::sum(std::move(sum), Number::create(file));
        getc(file);
        ungetc(getc(file), file);
    }
    return sum->getMagnitude();
}

int64 task18B(FILE* file) {
    std::vector<NumberRef> numbers;
    while (!feof(file)) {
        numbers.push_back(Number::create(file));
        getc(file);
        ungetc(getc(file), file);
    }
    int64 result = 0;
    for (const auto& first : numbers) {
        for (const auto& second : numbers) {
            if (first.get() != second.get()) {
                result = std::max(result, Number::sum(first->clone(), second->clone())->getMagnitude());
            }
        }
    }
    return result;
}

// ********************************************************************************************************

struct Xyz {
    int x, y, z;

    Xyz() = default;
    Xyz(const int x, const int y, const int z): x(x), y(y), z(z) {}

    Xyz operator+(const Xyz& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Xyz operator-(const Xyz& other) const { return { x - other.x, y - other.y, z - other.z }; }
    bool operator==(const Xyz& other) const = default;

    Xyz shuffle(int i) const {
        int xx, yy, zz;
        switch (i & 7) {
        case 0: xx = +x; yy = +y; zz = +z; break;
        case 1: xx = -x; yy = -y; zz = +z; break;
        case 2: xx = +x; yy = -y; zz = -z; break;
        case 3: xx = -x; yy = +y; zz = -z; break;
        case 4: xx = -x; yy = +y; zz = +z; break;
        case 5: xx = +x; yy = -y; zz = +z; break;
        case 6: xx = +x; yy = +y; zz = -z; break;
        case 7: xx = -x; yy = -y; zz = -z; break;
        default: abort();
        }
        switch (i >> 2) {
        case 0: return { xx, yy, zz };
        case 1: return { xx, zz, yy };
        case 2: return { yy, zz, xx };
        case 3: return { yy, xx, zz };
        case 4: return { zz, xx, yy };
        case 5: return { zz, yy, xx };
        default: abort();
        }
    }
};

namespace std {
    template<>
    struct hash<Xyz> {
        size_t operator()(const Xyz& value) const { 
            size_t h = std::hash<int>{}(value.x);
            h ^= std::hash<int>{}(value.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(value.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
}

class Area {
    int                                               mId;
    std::vector<Xyz>                                  mPoints;
    std::unordered_multimap<Xyz, std::pair<int, int>> mDeltas;

public:
    Area(const int id, std::vector<Xyz> points)
        : mId(id)
        , mPoints(std::move(points)) {
        calcDeltas();
    }

    int getId() const { return mId; }
    const std::vector<Xyz>& getPoints() const { return mPoints; }

    int getMatch(const Area& other, Xyz& outTranslation) const {
        std::unordered_map<Xyz, int> translations[24];
        for (const auto& [delta, ij] : other.mDeltas) {
            for (int i = 0; i < 24; ++i) {
                const auto it = mDeltas.find(delta.shuffle(i));
                if (it != mDeltas.end()) {
                    ++translations[i][mPoints[it->second.first] - other.mPoints[ij.first].shuffle(i)];
                    ++translations[i][mPoints[it->second.second] - other.mPoints[ij.second].shuffle(i)];
                }
            }
        }
        for (int i = 0; i < 24; ++i) {
            for (const auto& [tx, count] : translations[i]) {
                if (count >= 2 * 12 * 11) {
                    outTranslation = tx;
                    return i;
                }
            }
        }
        return -1;
    }

    void transform(const int shuffle, const Xyz& translation) {
        for (Xyz& p : mPoints) {
            p = p.shuffle(shuffle) + translation;
        }
        calcDeltas();
    }

private:
    void calcDeltas() {
        mDeltas.clear();
        for (int i = 0; i < mPoints.size(); ++i) {
            for (int j = 0; j < mPoints.size(); ++j) {
                if (i != j) {
                    mDeltas.insert({ mPoints[j] - mPoints[i], { i, j } });
                }
            }
        }
    }
};

int64 task19(FILE* file, const bool distances) {
    std::vector<Area> areas;
    while (!feof(file)) {
        int id{};
        if (fscanf_s(file, "--- scanner %d ---\n", &id) <= 0) {
            break;
        }
        std::vector<Xyz> points;
        Xyz xyz{};
        while (fscanf_s(file, "%d,%d,%d", &xyz.x, &xyz.y, &xyz.z) > 0) {
            points.push_back(xyz);
        }
        ungetc('-', file);
        areas.emplace_back(id, std::move(points));
    }

    std::vector<Xyz> translations;
    int ui = 1;
    for (int ti = 0; ti < areas.size(); ++ti) {
        if (ti >= ui) abort();
        const Area& target = areas[ti];
        for (int si = ui; si < areas.size(); ++si) {
            Area& source = areas[si];
            Xyz translation{};
            const int shuffle = target.getMatch(source, translation);
            if (shuffle >= 0) {
                translations.push_back(translation);
                printf("Match %2d -> %2d [%2d | %5d, %5d, %5d]\n", source.getId(), target.getId(), shuffle, translation.x, translation.y, translation.z);
                source.transform(shuffle, translation);
                if (si != ui) {
                    Area area = std::move(source);
                    areas.erase(areas.begin() + si);
                    areas.insert(areas.begin() + ui, std::move(area));
                }
                ++ui;
            }
        }

    }

    if (!distances) {
        std::unordered_set<Xyz> points;
        for (const Area& a : areas) {
            for (const Xyz& p : a.getPoints()) {
                points.insert(p);
            }
        }
        return points.size();
    } else {
        translations.push_back(Xyz{});
        int maxDistance = 0;
        for (int i = 0; i < translations.size(); ++i) {
            for (int j = i + 1; j < translations.size(); ++j) {
                const int dist = abs(translations[i].x - translations[j].x) + abs(translations[i].y - translations[j].y) + abs(translations[i].z - translations[j].z);
                maxDistance = std::max(maxDistance, dist);
            }
        }
        return maxDistance;
    }
}

// ********************************************************************************************************

int64 task20(FILE* file, const int rounds) {
    std::vector<bool> lookup;
    for (int i = 0; i < 512; ++i) {
        const char c = getc(file);
        lookup.push_back(c == '#');
        assert(c == '#' || c == '.');
    }
    getc(file);
    getc(file);
    static constexpr int SIZE = 100;
    std::vector<bool> image;
    while (!feof(file)) {
        char line[SIZE+1];
        if (fscanf_s(file, "%s\n", line, unsigned(sizeof(line))) <= 0) {
            break;
        }
        for (int i = 0; i < SIZE; ++i) {
            image.push_back(line[i] == '#');
            assert(line[i] == '#' || line[i] == '.');
        }
    }
    int imageSize = SIZE;
    const auto getPixel = [&](const int x, const int y, const int pass) -> int {
        if (x >= 0 && x < imageSize && y >= 0 && y < imageSize) {
            return int(image[x + y * imageSize]);
        } else {
            return pass & 1;
        }
    };
    for (int pass = 0; pass < rounds; ++pass) {
        std::vector<bool> i2;
        i2.resize((imageSize + 2) * (imageSize + 2));
        for (int y = 0; y < imageSize + 2; ++y) {
            for (int x = 0; x < imageSize + 2; ++x) {
                const int index = (getPixel(x-2, y-2, pass) << 8) | (getPixel(x-1, y-2, pass) << 7) | (getPixel(x, y-2, pass) << 6) |
                                  (getPixel(x-2, y-1, pass) << 5) | (getPixel(x-1, y-1, pass) << 4) | (getPixel(x, y-1, pass) << 3) |
                                  (getPixel(x-2, y  , pass) << 2) | (getPixel(x-1, y  , pass) << 1) | (getPixel(x, y  , pass));
                i2[x + y * (imageSize + 2)] = lookup[index];
            }
        }
        imageSize += 2;
        image = std::move(i2);
    }

    int64 count = 0;
    for (const bool p : image) {
        if (p) ++count;
    }
    return count;
}

// ********************************************************************************************************

int64 task21A(FILE* file) {
    int position[2] = { 8-1, 3-1 };
    int score[2] = {};

    const auto roll = [](int turn) {
        turn *= 3;
        return (turn % 100) + ((turn+1) % 100) + ((turn+2) % 100) + 3;
    };
    for (int turn = 0;;++turn) {
        const int player = turn & 1;
        position[player] = (position[player] + roll(turn)) % 10;
        score[player] += position[player] + 1;
        if (score[player] >= 1000) {
            printf("%d * %d = ", score[1-player], 3*(turn + 1));
            return score[1-player] * 3*(turn + 1);
        }
    }
    return -1;
}

int64 task21B(FILE* file) {
    //static constexpr int START_POSITION[2] = { 4, 8 };
    static constexpr int START_POSITION[2] = { 8, 3 };

    static constexpr int POSITIONS = 10;
    static constexpr int MAX_SCORE = 21;
    static constexpr int ROLL[POSITIONS] = { 0, 0, 0, 1, 3, 6, 7, 6, 3, 1 };

    using State = int64[POSITIONS][POSITIONS][MAX_SCORE][MAX_SCORE];
    int64 wins[2] = {};
    State state   = {};
    state[START_POSITION[0]-1][START_POSITION[1]-1][0][0] = 1;

    for (int round = 0; round < 2*MAX_SCORE; ++round) {
        const int    player     = round & 1;
        State        newState   = {};
        for (int r = 0; r < POSITIONS; ++r) {
            if (!ROLL[r]) continue;
            for (int p = 0; p < POSITIONS; ++p) {
                const int newPos   = (p + r) % POSITIONS;
                const int addScore = newPos + 1;
                for (int s = 0; s < MAX_SCORE; ++s) {
                    for (int p2 = 0; p2 < POSITIONS; ++p2) {
                        for (int s2 = 0; s2 < MAX_SCORE; ++s2) {
                            const int64 states   = (player ? state[p2][p][s2][s] : state[p][p2][s][s2]) * ROLL[r];
                            const int   newScore = s + addScore;
                            if (newScore < MAX_SCORE) {
                                if (player) {
                                    newState[p2][newPos][s2][newScore] += states;
                                } else {
                                    newState[newPos][p2][newScore][s2] += states;
                                }
                            } else {
                                wins[player] += states;
                            }
                        }
                    }
                }
            }
        }
        memcpy(state, newState, sizeof(newState));
    }

    printf("Wins #1: %lld\n", wins[0]);
    printf("Wins #2: %lld\n", wins[1]);
    return std::max(wins[0], wins[1]);
}

// ********************************************************************************************************

int main(const int argc, const char* argv[]) {
    FILE* file = nullptr;
    if (fopen_s(&file, argv[1], "r")) {
        return -1;
    }
    const int64 result = task21B(file);
    fclose(file);
    printf("%lld\n", result);
    return 0;
}
