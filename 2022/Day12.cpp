#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define PLOT_TRAIL

struct Pos {
    int x = 0;
    int y = 0;
};

struct Site {
    char height{};
    int  steps = INT_MAX;

#ifdef PLOT_TRAIL
    struct {
        char height{};
        bool isOnTrail = false;
        Pos  backtrace;
    } trailPlot;
#endif
};

// ********************************************************************************************************

int solve(std::ifstream input, const bool includeAll) {
    Pos                            targetPos;
    std::vector<std::vector<Site>> grid;
    std::deque<Pos>                queue;
    std::string                    line;
    for (int y = 0; std::getline(input, line); ++y) {
        std::vector<Site> row;
        row.reserve(line.size());
        for (int x = 0; x < int(line.size()); ++x) {
            Site site;
            site.height = line[x];
#ifdef PLOT_TRAIL
            site.trailPlot.height = line[x];
#endif
            if (site.height == 'S' || (includeAll && site.height == 'a')) {
                site.height = 'a';
                site.steps  = 0;
                queue.push_back(Pos{ x, y });
            } else if (site.height == 'E') {
                site.height = 'z';
                targetPos   = { x, y };
            }
            row.push_back(std::move(site));
        }
        grid.push_back(std::move(row));
    }
    while (!queue.empty()) {
        const Pos   pos  = queue.front();
        const Site& site = grid[pos.y][pos.x];
        queue.pop_front();

        const auto checkNextSite = [&](const int dx, const int dy) {
            const int nextX = pos.x + dx;
            const int nextY = pos.y + dy;
            if (nextX >= 0 && nextX < int(grid[pos.y].size()) && nextY >= 0 && nextY < int(grid.size())) {
                Site& nextSite = grid[nextY][nextX];
                if (nextSite.height <= site.height + 1 && nextSite.steps > site.steps + 1) {
                    nextSite.steps = site.steps + 1;
#ifdef PLOT_TRAIL
                    nextSite.trailPlot.backtrace = pos;
#endif
                    queue.push_back(Pos{ nextX, nextY });
                }
            }
        };
        checkNextSite(-1,  0);
        checkNextSite(+1,  0);
        checkNextSite( 0, -1);
        checkNextSite( 0, +1);
    }

#ifdef PLOT_TRAIL
    Pos pos = targetPos;
    for (int sequence = 1;; ++sequence) {
        Site& site = grid[pos.y][pos.x];
        site.trailPlot.isOnTrail = true;
        if (!site.steps) {
            break;
        }
        pos = site.trailPlot.backtrace;
    }
    for (const auto& row : grid) {
        for (const auto& site : row) {
            std::cout << (site.trailPlot.isOnTrail ? site.trailPlot.height : '.');
        }
        std::cout << std::endl;
    }
#endif

    return grid[targetPos.y][targetPos.x].steps;
}

int solve1(std::ifstream input) {
    return solve(std::move(input), false);
}

int solve2(std::ifstream input) {
    return solve(std::move(input), true);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input12.txt";
static const std::string TEST_FILE     = "inputs/test12.txt";
static const int         TEST_RESULT_1 = 31;
static const int         TEST_RESULT_2 = 29;

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
