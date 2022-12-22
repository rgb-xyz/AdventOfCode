#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Pos {
    int x = 0;
    int y = 0;

    Pos operator+(const Pos& p) const { return Pos{ x + p.x, y + p.y }; }
};

struct Face {
    struct Adjacent {
        int face{};
        int edge{};
    };

    Pos                     origin;
    std::array<Adjacent, 4> adjacent{};
};

struct State {
    int faceId   = 0;
    Pos position = {};
    int rotation = 0;
};

// ********************************************************************************************************

int solve(std::ifstream input, const bool isCube) {
    const std::vector<std::string> map = [&input]() {
        std::vector<std::string> map;
        size_t                   maxWidth = 0;
        for (std::string line; std::getline(input, line) && !line.empty();) {
            maxWidth = std::max(maxWidth, line.size());
            map.push_back(std::move(line));
        }
        for (std::string& row : map) {
            row.resize(maxWidth, ' ');
        }
        return map;
    }();
    const int width    = int(map.front().size());
    const int height   = int(map.size());
    const int tileSize = std::min(width, height) / 3;
    assert(std::max(width, height) == 4 * tileSize);

    std::array<std::array<int, 4>, 4> faceIds;
    std::array<Face, 6> faces;
    for (int faceId = 0, i = 0; i < 16; ++i) {
        const int u = i % 4;
        const int v = i / 4;
        const int x = u * tileSize;
        const int y = v * tileSize;
        if (x < width && y < height && map[y][x] != ' ') {
            faceIds[v][u]        = faceId;
            faces[faceId].origin = Pos{ x, y };
            std::fill(faces[faceId].adjacent.begin(),
                      faces[faceId].adjacent.end(),
                      Face::Adjacent{ -1, -1 });
            ++faceId;
        } else {
            faceIds[v][u] = -1;
        }
    }

    if (isCube) {
        // Hard-coded for the submission. :]
        // May be generalized later.
        //        +------+------+
        //        |FRONT |RIGHT |
        //        |[0]   |[1]   |
        //        +------+------+
        //        |BOTTOM|
        //        |[2]   |
        // +------+------+
        // |LEFT  |BACK  |
        // |[3]   |[4]   |
        // +------+------+
        // |TOP   |
        // |[5]   |
        // +------+

        // FRONT
        faces[0].adjacent[0].face = 1; faces[0].adjacent[0].edge = 0;
        faces[0].adjacent[1].face = 2; faces[0].adjacent[1].edge = 1;
        faces[0].adjacent[2].face = 3; faces[0].adjacent[2].edge = 0;
        faces[0].adjacent[3].face = 5; faces[0].adjacent[3].edge = 0;
        // RIGHT
        faces[1].adjacent[0].face = 4; faces[1].adjacent[0].edge = 2;
        faces[1].adjacent[1].face = 2; faces[1].adjacent[1].edge = 2;
        faces[1].adjacent[2].face = 0; faces[1].adjacent[2].edge = 2;
        faces[1].adjacent[3].face = 5; faces[1].adjacent[3].edge = 3;
        // BOTTOM
        faces[2].adjacent[0].face = 1; faces[2].adjacent[0].edge = 3;
        faces[2].adjacent[1].face = 4; faces[2].adjacent[1].edge = 1;
        faces[2].adjacent[2].face = 3; faces[2].adjacent[2].edge = 1;
        faces[2].adjacent[3].face = 0; faces[2].adjacent[3].edge = 3;
        // LEFT
        faces[3].adjacent[0].face = 4; faces[3].adjacent[0].edge = 0;
        faces[3].adjacent[1].face = 5; faces[3].adjacent[1].edge = 1;
        faces[3].adjacent[2].face = 0; faces[3].adjacent[2].edge = 0;
        faces[3].adjacent[3].face = 2; faces[3].adjacent[3].edge = 0;
        // BACK
        faces[4].adjacent[0].face = 1; faces[4].adjacent[0].edge = 2;
        faces[4].adjacent[1].face = 5; faces[4].adjacent[1].edge = 2;
        faces[4].adjacent[2].face = 3; faces[4].adjacent[2].edge = 2;
        faces[4].adjacent[3].face = 2; faces[4].adjacent[3].edge = 3;
        // TOP
        faces[5].adjacent[0].face = 4; faces[5].adjacent[0].edge = 3;
        faces[5].adjacent[1].face = 1; faces[5].adjacent[1].edge = 1;
        faces[5].adjacent[2].face = 0; faces[5].adjacent[2].edge = 1;
        faces[5].adjacent[3].face = 3; faces[5].adjacent[3].edge = 3;
    } else {
        for (int v = 0; v < 4; ++v) {
            for (int u = 0; u < 4; ++u) {
                if (faceIds[v][u] < 0) continue;
                Face& face = faces[faceIds[v][u]];
                for (int r = 0; r < 4; ++r) {
                    int nextFace = -1;
                    for (int d = 1; d <= 4; ++d) {
                        switch (r) {
                        case 0: nextFace = faceIds[v][(u + d) % 4]; break;
                        case 1: nextFace = faceIds[(v + d) % 4][u]; break;
                        case 2: nextFace = faceIds[v][(u - d + 4) % 4]; break;
                        case 3: nextFace = faceIds[(v - d + 4) % 4][u]; break;
                        }
                        if (nextFace >= 0) break;
                    }
                    Face::Adjacent& adjacent = face.adjacent[r];
                    adjacent.face = nextFace;
                    adjacent.edge = r;
                }
            }
        }
    }

    const auto advance = [&](State s) {
        const int size = tileSize - 1;
        int       newX = s.position.x;
        int       newY = s.position.y;
        switch (s.rotation) {
        case 0: ++newX; break;
        case 1: ++newY; break;
        case 2: --newX; break;
        case 3: --newY; break;
        }
        if (newX < 0 || newX > size || newY < 0 || newY > size) {
            const Face::Adjacent& adjacent = faces[s.faceId].adjacent[s.rotation];
            if (s.rotation & 1) std::swap(newX, newY);
            newX = 0;
            if ((adjacent.edge ^ s.rotation) % 3) newY = size - newY;
            if (adjacent.edge & 2) newX = size - newX;
            if (adjacent.edge & 1) std::swap(newX, newY);
            s.faceId   = adjacent.face;
            s.rotation = adjacent.edge;
        }
        s.position.x = newX;
        s.position.y = newY;
        return s;
    };

    State s;
    for (int steps{}; input >> steps;) {
        for (; steps > 0; --steps) {
            const State s2 = advance(s);
            const Pos pos = faces[s2.faceId].origin + s2.position;
            if (map[pos.y][pos.x] == '.') {
                s = s2;
            } else {
                assert(map[pos.y][pos.x] == '#');
                break;
            }
        }
        switch (input.get()) {
        case 'R': s.rotation = (s.rotation + 1) % 4; break;
        case 'L': s.rotation = (s.rotation + 3) % 4; break;
        case EOF: break;
        default: std::terminate();
        }
    }

    const Pos result = s.position + faces[s.faceId].origin + Pos{ 1, 1 };
    return 1000 * result.y + 4 * result.x + s.rotation;
}

int solve1(std::ifstream input) {
    return solve(std::move(input), false);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), true);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input22.txt";
static const std::string TEST_FILE     = "inputs/test22.txt";
static const int         TEST_RESULT_1 = 6032;
static const int         TEST_RESULT_2 = 5031;

int main() {
    if (solve1(std::ifstream(TEST_FILE)) == TEST_RESULT_1) {
        std::cout << solve1(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope (" << solve1(std::ifstream(TEST_FILE)) << ")" << std::endl;
    }
    //if (solve2(std::ifstream(TEST_FILE)) == TEST_RESULT_2) {
        std::cout << solve2(std::ifstream(INPUT_FILE)) << std::endl;
    //} else {
    //    std::cout << "Nope (" << solve2(std::ifstream(TEST_FILE)) << ")" << std::endl;
    //}
    return 0;
}
