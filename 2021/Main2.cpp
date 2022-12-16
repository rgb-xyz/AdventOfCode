#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using int8   = int8_t;
using int64  = int64_t;
using uint64 = uint64_t;

//************************************************************************************************************

int64 task22A(FILE* input) {
    static constexpr int GRID_SIZE   = 50;
    static constexpr int GRID_STRIDE = GRID_SIZE * 2 + 1;
    std::vector<bool> grid;
    grid.resize(GRID_STRIDE * GRID_STRIDE * GRID_STRIDE);
    const auto cube = [&](const int x, const int y, const int z) {
        assert(abs(x) <= GRID_SIZE && abs(y) <= GRID_SIZE && abs(z) <= GRID_SIZE);
        const int xx = x + GRID_SIZE;
        const int yy = y + GRID_SIZE;
        const int zz = z + GRID_SIZE;
        return grid[xx + GRID_STRIDE * (yy + GRID_STRIDE * zz)];
    };

    while (!feof(input)) {
        char state[10];
        int x1{}, x2{}, y1{}, y2{}, z1{}, z2{};
        if (fscanf_s(input, "%s x=%d..%d,y=%d..%d,z=%d..%d\n", state, unsigned(sizeof(state)), &x1, &x2, &y1, &y2, &z1, &z2) <= 0) {
            break;
        }
        const bool on = !strcmp(state, "on");
        if (x1 > GRID_SIZE || x2 < -GRID_SIZE || y1 > GRID_SIZE || y2 < -GRID_SIZE || z1 > GRID_SIZE || z2 < -GRID_SIZE) {
            printf("Skipping %d..%d %d..%d %d..%d\n", x1, x2, y1, y2, z1, z2);
            continue;
        }
        x1 = std::max(x1, -GRID_SIZE);
        x2 = std::min(x2, GRID_SIZE);
        y1 = std::max(y1, -GRID_SIZE);
        y2 = std::min(y2, GRID_SIZE);
        z1 = std::max(z1, -GRID_SIZE);
        z2 = std::min(z2, GRID_SIZE);
        for (int z = z1; z <= z2; ++z) {
            for (int y = y1; y <= y2; ++y) {
                for (int x = x1; x <= x2; ++x) {
                    cube(x, y, z) = on;
                }
            }
        }
    }

    int64 result = 0;
    for (int z = -GRID_SIZE; z <= GRID_SIZE; ++z) {
        for (int y = -GRID_SIZE; y <= GRID_SIZE; ++y) {
            for (int x = -GRID_SIZE; x <= GRID_SIZE; ++x) {
                if (cube(x, y, z)) ++result;
            }
        }
    }
    return result;
}

template <typename T>
class SegmentMap {
    std::vector<std::pair<int, T>> mRanges;

public:
    std::vector<std::reference_wrapper<T>> insert(const int from, const int to) {
        assert(from < to);
        mRanges.reserve(std::max(mRanges.capacity(), mRanges.size()+2));
        auto fit = std::lower_bound(mRanges.begin(), mRanges.end(), from, [](const auto& v, const int k) { return v.first < k; });
        if (fit == mRanges.end() || fit->first != from) {
            T value{};
            if (fit != mRanges.begin()) {
                value = (fit-1)->second;
            }
            fit = mRanges.insert(fit, { from, std::move(value) });
        }
        auto tit = std::lower_bound(mRanges.begin(), mRanges.end(), to, [](const auto& v, const int k) { return v.first < k; });
        if (tit == mRanges.begin() || (tit-1)->first != to) {
            T value{};
            if (tit != mRanges.begin()) {
                value = (tit-1)->second;
            }
            tit = mRanges.insert(tit, { to, std::move(value) });
        }
        std::vector<std::reference_wrapper<T>> result;
        for (; fit != tit; ++fit) {
            result.push_back(fit->second);
        }
        return result;
    }

    template <typename TVisitor>
    void visit(TVisitor&& visitor) const {
        for (auto it = mRanges.begin(); it != mRanges.end(); ++it) {
            if (it + 1 != mRanges.end()) {
                visitor(it->first, (it+1)->first, it->second);
            }
        }
    }
};

int64 task22(FILE* input, const bool bound) {
    SegmentMap<SegmentMap<SegmentMap<bool>>> cubes;
    while (!feof(input)) {
        char state[10];
        int x1{}, x2{}, y1{}, y2{}, z1{}, z2{};
        if (fscanf_s(input, "%s x=%d..%d,y=%d..%d,z=%d..%d\n", state, unsigned(sizeof(state)), &x1, &x2, &y1, &y2, &z1, &z2) <= 0) {
            break;
        }
        const bool on = !strcmp(state, "on");
        if (bound) {
            if (x1 > 50 || x2 < -50 || y1 > 50 || y2 < -50 || z1 > 50 || z2 < -50) {
                printf("Skipping %d..%d %d..%d %d..%d\n", x1, x2, y1, y2, z1, z2);
                continue;
            }
            x1 = std::max(x1, -50);
            x2 = std::min(x2, 50);
            y1 = std::max(y1, -50);
            y2 = std::min(y2, 50);
            z1 = std::max(z1, -50);
            z2 = std::min(z2, 50);
        }
        for (SegmentMap<SegmentMap<bool>>& xx : cubes.insert(x1, x2+1)) {
            for (SegmentMap<bool>& yy : xx.insert(y1, y2+1)) {
                for (bool& zz : yy.insert(z1, z2+1)) {
                    zz = on;
                }
            }
        }
    }

    int64 result = 0;
    cubes.visit([&](const int64 fromX, const int64 toX, const SegmentMap<SegmentMap<bool>>& valX) {
        valX.visit([&](const int64 fromY, const int64 toY, const SegmentMap<bool>& valY) {
            valY.visit([&](const int64 fromZ, const int64 toZ, const bool valZ) {
                //printf("%s %d..%d %d..%d %d..%d\n", valZ ? "+++" : "---", fromX, toX, fromY, toY, fromZ, toZ);
                if (valZ) {
                    result += (toX - fromX) * (toY - fromY) * (toZ - fromZ);
                }
            });
        });
    });
    return result;
}

//************************************************************************************************************

// #############
// #01 2 3 4 56#
// ###3#7#B#F###
//   #2#6#A#E#
//   #1#5#9#D#
//   #0#4#8#C#
//   #########

std::unordered_map<uint64, std::unordered_map<int, int>> gScores;

static constexpr int HALL_INDEX[7] = { 0, 6, 1, 5, 2, 4, 3 };
static constexpr int HALL_MASKS[4][7] = {
    { 0x000000ff, 0x000000f0, 0x00000f00, 0x0000ff00, 0x000fff00, 0x00ffff00, 0x0fffff00 },
    { 0x00000fff, 0x00000ff0, 0x00000f00, 0x0000f000, 0x000ff000, 0x00fff000, 0x0ffff000 },
    { 0x0000ffff, 0x0000fff0, 0x0000ff00, 0x0000f000, 0x000f0000, 0x00ff0000, 0x0fff0000 },
    { 0x000fffff, 0x000ffff0, 0x000fff00, 0x000ff000, 0x000f0000, 0x00f00000, 0x0ff00000 }
};
static constexpr int LENGTHS[4][7] = {
    { 3, 2, 2, 4, 6, 8, 9 },
    { 5, 4, 2, 2, 4, 6, 7 },
    { 7, 6, 4, 2, 2, 4, 5 },
    { 9, 8, 6, 4, 2, 2, 3 }
};

static constexpr uint64 ROOM_MASKS[4] = { 0x1111, 0x2222, 0x3333, 0x4444 };
static constexpr int    ROOM_DEPTH = 4;

static constexpr int COSTS[4] = { 1, 10, 100, 1000 };

int eval23(const uint64 rooms, const int hall) {
    std::unordered_map<int, int>& roomScores = gScores[rooms];
    if (roomScores.contains(hall)) {
        return roomScores[hall];
    }

    int& bestScore = roomScores[hall];
    bestScore = INT_MAX;
    for (int ri = 0; ri < 4; ++ri) {
        const uint64 room = (rooms >> 16*ri) & 0xffff;
        for (int pi = ROOM_DEPTH-1; pi >= 0; --pi) {
            const int apod = int(room >> (pi*4));
            if (!apod) continue;
            const uint64 mask = (1UI64 << (4*pi)) - 1;
            if (apod == ri+1 && ((room & mask) == (ROOM_MASKS[ri] & mask))) break;

            const uint64 newRooms = rooms & ~(0xffffUI64 << (16*ri)) | ((room & mask) << (16*ri));
            for (int i = 0; i < 7; ++i) {
                const int hi = HALL_INDEX[i];
                if (hall & HALL_MASKS[ri][hi]) continue;
                const int newHall = hall | (apod << (4*hi));
                int newScore = eval23(newRooms, newHall);
                if (newScore < INT_MAX) {
                    newScore += COSTS[apod-1] * (LENGTHS[ri][hi] - 1 + ROOM_DEPTH - pi);
                }
                bestScore = std::min(bestScore, newScore);
            }
            break;
        }
    }

    for (int i = 6; i >= 0; --i) {
        const int hi = HALL_INDEX[i];
        const int apod = int(hall >> (hi*4)) & 0xf;
        if (!apod) continue;
        const int newHall = hall & ~(0xf << (4*hi));
        const int ri = apod-1;
        if (newHall & HALL_MASKS[ri][hi]) continue;
        const uint64 room = (rooms >> 16*ri) & 0xffff;
        for (int pi = 0; pi < ROOM_DEPTH; ++pi) {
            if (room >> (pi*4)) continue;
            const uint64 mask = (1UI64 << (4*pi)) - 1;
            if ((room & mask) != (ROOM_MASKS[ri] & mask)) break;
            const uint64 newRooms = rooms | (int64(apod) << (16*ri + 4*pi));
            int newScore = eval23(newRooms, newHall);
            if (newScore < INT_MAX) {
                newScore += COSTS[apod-1] * (LENGTHS[ri][hi] - 1 + ROOM_DEPTH - pi);
            }
            bestScore = std::min(bestScore, newScore);
            break;
        }
    }

    return bestScore;
}

int64 task23() {
    //const uint64 ROOMS = 0x4131221333242441;
    const uint64 ROOMS = 0x4131221233213444;
    gScores[0x4444333322221111][0] = 0;
    eval23(ROOMS, 0);
    return gScores[ROOMS][0];
}

//************************************************************************************************************

int64 task24() {
    static constexpr int A[14] = {  1,  1,  1,  26,  1,  26, 26,  26,  1, 26,  1,  1,  26, 26 };
    static constexpr int B[14] = { 10, 13, 12, -12, 11, -13, -9, -12, 14, -9, 15, 11, -16, -2 };
    static constexpr int C[14] = {  5,  9,  4,   4, 10,  14, 14,  12, 14, 14,  5, 10,   8, 15 };

    const auto round = [](int64 seed, const int data, const int a, const int b, const int c) {
        const int x = seed % 26 + b;
        seed /= a;
        if (x != data) {
            seed *= 26;
            seed += data + c;
        }
        return seed;
    };

    std::vector<std::pair<int64, int64>> seeds;
    seeds.push_back({ 0, 0 });
    for (int pass = 0; pass < 14; ++pass) {
        int64 seedClip = 1;
        for (int i = pass+1; i < 14; ++i) seedClip *= A[i]; 
        std::unordered_set<int64> usedSeeds;
        std::vector<std::pair<int64, int64>> newSeeds;
        for (const auto [seed, data] : seeds) {
            //for (int digit = 9; digit > 0; --digit) {
            for (int digit = 1; digit < 10; ++digit) {
                const int64 newSeed = round(seed, digit, A[pass], B[pass], C[pass]);
                if (newSeed < seedClip && usedSeeds.insert(newSeed).second) {
                    newSeeds.push_back({ newSeed, (data * 10) + digit });
                }
            }
        }
        seeds = std::move(newSeeds);
        printf("%2d: %zd\n", pass+1, seeds.size());
    }
    assert(seeds.size() == 1);
    assert(seeds[0].first == 0);
    return seeds[0].second;
}

//************************************************************************************************************

int64 task25(FILE* input) {
    static constexpr int WIDTH  = 139;
    static constexpr int HEIGHT = 137;
    std::vector<char> grid;
    grid.resize(WIDTH * HEIGHT + 1);

    const auto data = [&](int x, int y) -> char& {
        x = (x + WIDTH) % WIDTH;
        y = (y + HEIGHT) % HEIGHT;
        return grid[x + y * WIDTH];
    };
    for (int y = 0; y < HEIGHT; ++y) {
        if (fscanf_s(input, "%s\n", &grid[y * WIDTH], WIDTH+1) <= 0) break;
    }

    int rounds = 0;
    for (;; ++rounds) {
        //printf("%d:\n", rounds);
        //for (int y = 0; y < HEIGHT; ++y) {
        //    for (int x = 0; x < WIDTH; ++x) {
        //        printf("%c", data(x, y));
        //    }
        //    printf("\n");
        //}
        bool hasMoved = false;
        std::vector<char> next;
        next.resize(WIDTH * HEIGHT, '.');
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                const char c = data(x, y);
                int xx = x;
                int yy = y;
                if (c == '>') {
                    if (data(x+1, y) == '.') ++xx;
                } else if (c == 'v') {
                    if ((data(x, y+1) == '.' && data(x-1, y+1) != '>') || (data(x, y+1) == '>' && data(x+1, y+1) == '.')) ++yy;
                } else {
                    assert(c == '.');
                    continue;
                }
                xx = xx % WIDTH;
                yy = yy % HEIGHT;
                if (xx != x || yy != y) hasMoved = true;
                char& out = next[xx + yy * WIDTH];
                assert(out == '.');
                out = c;
            }
        }
        grid = std::move(next);
        if (!hasMoved) break;
    }
    return rounds + 1;
}

//************************************************************************************************************

int main()
{
    using namespace std::literals;

    FILE* file = fopen("Z:/Download/input25.txt", "r");
    const auto before = std::chrono::high_resolution_clock::now();
    int64 result = task25(file);
    const auto after = std::chrono::high_resolution_clock::now();
    printf("%lld (took %.3f ms)\n", result, ((after-before) / 1us) * 1e-3);
    fclose(file);
}
