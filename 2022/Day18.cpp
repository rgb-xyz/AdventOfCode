#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

struct Pos {
    int x = 0;
    int y = 0;
    int z = 0;

    friend std::istream& operator>>(std::istream& stream, Pos& pos) {
        char sep{};
        return stream >> pos.x >> sep >> pos.y >> sep >> pos.z;
    }

    friend Pos min(const Pos& lhs, const Pos& rhs) {
        return Pos{ std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z) };
    }
    friend Pos max(const Pos& lhs, const Pos& rhs) {
        return Pos{ std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z) };
    }

    auto operator<=>(const Pos&) const = default;
};

// ********************************************************************************************************

int solve(std::ifstream input, const bool excludeInterior) {
    std::set<Pos> cubes;
    Pos lower{ INT_MAX, INT_MAX, INT_MAX };
    Pos upper{ INT_MIN, INT_MIN, INT_MIN };
    for (Pos p; input >> p;) {
        cubes.insert(p);
        lower = min(lower, p);
        upper = max(upper, p);
    }
    lower.x--; lower.y--; lower.z--;
    upper.x++; upper.y++; upper.z++;

    const auto process = [&](const Pos& cube, auto&& closure) {
        const auto test = [&](const int dx, const int dy, const int dz) {
            const Pos p{ cube.x + dx, cube.y + dy, cube.z + dz };
            if (max(p, lower) == p && min(p, upper) == p && !cubes.contains(p)) {
                closure(p);
            }
        };
        test(-1,  0,  0);
        test(+1,  0,  0);
        test( 0, -1,  0);
        test( 0, +1,  0);
        test( 0,  0, -1);
        test( 0,  0, +1);
    };
    const auto getSurface = [&]() {
        int surface = 0;
        for (const Pos& cube : cubes) {
            process(cube, [&](const Pos&) { ++surface; });
        }
        return surface;
    };

    int result = getSurface();
    if (excludeInterior) {
        std::set<Pos> queue;
        queue.insert(lower);
        while (!queue.empty()) {
            const Pos cube = *queue.begin();
            queue.erase(queue.begin());
            cubes.insert(cube);
            process(cube, [&](const Pos& p) { queue.insert(p); });
        }
        result -= getSurface();
    }
    return result;
}

int solve1(std::ifstream input) {
    return solve(std::move(input), false);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), true);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input18.txt";
static const std::string TEST_FILE     = "inputs/test18.txt";
static const int         TEST_RESULT_1 = 64;
static const int         TEST_RESULT_2 = 58;

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
