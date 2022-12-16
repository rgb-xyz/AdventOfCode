#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Pos {
    int x = 0;
    int y = 0;

    friend std::istream& operator>>(std::istream& stream, Pos& pos) {
        while (stream) {
            const int ch = stream.get();
            if (ch == 'x' && stream.get() == '=') {
                stream >> pos.x;
            } else if (ch == 'y' && stream.get() == '=') {
                stream >> pos.y;
                break;
            }
        }
        return stream;
    }
};

int l1norm(const int lhs, const int rhs) {
    return std::abs(lhs - rhs);
}

int l1norm(const Pos& lhs, const Pos& rhs) {
    return l1norm(lhs.x, rhs.x) + l1norm(lhs.y, rhs.y);
}

// ********************************************************************************************************

int64_t solve1(std::ifstream input, const int y) {
    std::vector<std::pair<Pos, int>> ranges;
    std::vector<int>                 beacons;
    std::pair<int, int>              scanRange{};
    for (Pos sensor, beacon; input >> sensor >> beacon;) {
        const int size = l1norm(sensor, beacon);
        ranges.emplace_back(sensor, size);
        if (beacon.y == y) {
            beacons.push_back(beacon.x);
        }
        scanRange.first  = std::min(scanRange.first , sensor.x - size);
        scanRange.second = std::max(scanRange.second, sensor.x + size);
    }

    // Note: string/vector<char> will have much better performance than vector<bool>.
    std::string scan(scanRange.second - scanRange.first + 1, '.');
    for (const auto& [sensor, dist] : ranges) {
        const int size = dist - l1norm(sensor.y, y);
        const int from = sensor.x - size;
        const int to   = sensor.x + size;
        if (to >= from) {
            std::fill(scan.begin() + from - scanRange.first,
                      scan.begin() + to - scanRange.first + 1,
                      '#');
        }
    }
    for (int beacon : beacons) {
        scan[beacon - scanRange.first] = 'B';
    }
    return std::count(scan.begin(), scan.end(), '#');
}

int64_t solve2(std::ifstream input, const int range) {
    std::vector<std::pair<Pos, int>> ranges;
    for (Pos sensor, beacon; input >> sensor >> beacon;) {
        ranges.emplace_back(sensor, l1norm(sensor, beacon));
    }

    // Sort the sensors in the X-direction.
    // This will make the sweeping faster because more sensors can be swept in a single loop.
    std::sort(ranges.begin(), ranges.end(),
              [](const auto& range1, const auto& range2) { return range1.first.x < range2.first.x; });

    for (int y = 0; y <= range; ++y) {
        for (int x = 0; x <= range;) {
            const int oldX = x;
            for (const auto& [sensor, dist] : ranges) {
                const int size = dist - l1norm(sensor.y, y);
                const int from = sensor.x - size;
                const int to   = sensor.x + size;
                if (x >= from && x <= to) {
                    x = to + 1;
                }
            }
            if (x == oldX) {
                return int64_t(x) * 4000000 + int64_t(y);
            }
        }
    }
    return -1;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input15.txt";
static const std::string TEST_FILE     = "inputs/test15.txt";
static const int64_t     INPUT_RANGE   = 4000000;
static const int64_t     TEST_RANGE    = 20;
static const int64_t     TEST_RESULT_1 = 26;
static const int64_t     TEST_RESULT_2 = 56000011;

int main() {
    if (solve1(std::ifstream(TEST_FILE), TEST_RANGE / 2) == TEST_RESULT_1) {
        std::cout << solve1(std::ifstream(INPUT_FILE), INPUT_RANGE / 2) << std::endl;
    } else {
        std::cout << "Nope (" << solve1(std::ifstream(TEST_FILE), TEST_RANGE / 2) << ")" << std::endl;
    }
    if (solve2(std::ifstream(TEST_FILE), TEST_RANGE) == TEST_RESULT_2) {
        std::cout << solve2(std::ifstream(INPUT_FILE), INPUT_RANGE) << std::endl;
    } else {
        std::cout << "Nope (" << solve2(std::ifstream(TEST_FILE), TEST_RANGE) << ")" << std::endl;
    }
    return 0;
}
