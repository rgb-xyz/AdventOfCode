#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

struct Edge {
    int node{};
    int cost{};
};

struct Node {
    int               cost{};
    std::vector<Edge> edges;
};

// ********************************************************************************************************

std::vector<Node> process(std::ifstream input) {
    struct Room {
        std::string              name;
        int                      valveRate{};
        std::vector<std::string> tunnels;
    };

    std::vector<Room> rooms;
    for (std::string str; std::getline(input, str);) {
        std::istringstream iss(std::move(str));
        Room room;
        iss >> str >> room.name;
        while (iss.get() != '=') {}
        iss >> room.valveRate;
        while (iss.get() != 'v') {}
        iss >> str;
        while (iss >> str) {
            if (str.ends_with(',')) str.pop_back();
            room.tunnels.push_back(std::move(str));
        }
        rooms.push_back(std::move(room));
    }

    // Map the room names into indices. The rooms will be ordered as:
    //  - Room with functioning valve. \
    //  - Room with functioning valve.  }-- Only these will be returned.
    //  - ...                          /
    //  - Starting room ("AA")        /  -- Always last.
    //  - Room with broken valve.      \
    //  - Room with broken valve.       }-- Used only to calculate distances between rooms of interest.
    //  - ...                          /
    std::map<std::string, int> mapping;
    for (const Room& room : rooms) {
        if (room.valveRate > 0 && room.name != "AA") {
            mapping.insert({ room.name, int(mapping.size()) });
        }
    }
    const size_t validNodeCount = mapping.size();
    mapping.insert({ "AA", int(mapping.size()) });
    for (const Room& room : rooms) {
        if (room.valveRate == 0) {
            mapping.insert({ room.name, int(mapping.size()) });
        }
    }
    const size_t totalNodeCount = mapping.size();

    // Use Floyd-Warshall to find minimum distances between each and every room.
    std::vector<std::vector<int>> dists(totalNodeCount, std::vector<int>(totalNodeCount, INT_MAX / 2));
    std::vector<int>              costs(totalNodeCount, 0);
    for (const Room& room : rooms) {
        const int i = mapping.find(room.name)->second;
        dists[i][i] = 0;
        costs[i]    = room.valveRate;
        for (const std::string& tunnel : room.tunnels) {
            const int j = mapping.find(tunnel)->second;
            dists[i][j] = 1;
        }
    }
    for (int k = 0; k < totalNodeCount; ++k) {
        for (int i = 0; i < totalNodeCount; ++i) {
            for (int j = 0; j < totalNodeCount; ++j) {
                const int source = dists[i][k] + dists[k][j];
                int&      target = dists[i][j];
                target = std::min(source, target);
            }
        }
    }

    std::vector<Node> nodes;
    // Note: "<=" here to include paths from "AA" (but not to it).
    for (int i = 0; i <= validNodeCount; ++i) {
        Node& node = nodes.emplace_back();
        node.cost = costs[i];
        node.edges.reserve(validNodeCount);
        for (int j = 0; j < validNodeCount; ++j) {
            if (i != j) {
                node.edges.emplace_back(j, dists[i][j] + 1); // add 1 for opening the valve
            }
        }
        std::sort(node.edges.begin(), node.edges.end(),
                  [](const Edge& edge1, const Edge& edge2) { return edge1.cost < edge2.cost; });
    }
    return nodes;
}

int getPressure(const std::vector<Node>& nodes, const int index, const int time, const unsigned visitedNodes) {
    const Node& node = nodes[index];
    int result = 0;
    for (const Edge& edge : node.edges) {
        if (edge.cost >= time) break;
        const unsigned nodeBit = 1U << edge.node;
        if (!(visitedNodes & nodeBit)) {
            const int pressure = getPressure(nodes, edge.node, time - edge.cost, visitedNodes | nodeBit);
            result = std::max(result, pressure);
        }
    }
    return result + time * node.cost;
}

int solve1(std::ifstream input) {
    const std::vector<Node> nodes = process(std::move(input));
    const int startIndex = int(nodes.size()) - 1;
    const int startTime  = 30;
    return getPressure(nodes, startIndex, startTime, 0);
}

// Brute-force solution with O(n!) complexity -- nicely readable by rather slow.
int solve2_old(std::ifstream input) {
    const std::vector<Node> nodes = process(std::move(input));
    const int      startIndex = int(nodes.size()) - 1;
    const int      startTime  = 26;
    const unsigned states     = 1U << startIndex;
    int result = 0;
    for (unsigned state = 0; state < states/2; ++state) {
        const int pressure = getPressure(nodes, startIndex, startTime, state) +
                             getPressure(nodes, startIndex, startTime, ~state);
        result = std::max(result, pressure);

    }
    return result;
}

// Dynamic programming solution with O(2^n*n^2) complexity -- much faster but very occluded.
int solve2(std::ifstream input) {
    constexpr int TIME      = 26;
    constexpr int MAX_NODES = 15;
    using Pressures = std::array<std::array<int, TIME>, MAX_NODES>;

    const std::vector<Node> nodes      = process(std::move(input));
    const int               nodeCount  = int(nodes.size()) - 1;
    if (nodeCount > MAX_NODES) {
        return -1;
    }

    // "pressures" and "flows" are indexed by a state bit-field specifying valves which *can be* opened.
    const unsigned         stateCount = 1U << nodeCount;
    std::vector<Pressures> pressures(stateCount);
    std::vector<int>       flows(stateCount);
    for (unsigned state = 0; state < stateCount; ++state) {
        int flow = 0;
        for (int i = 0; i < nodeCount; ++i) {
            if (!(state & (1U << i))) {
                flow += nodes[i].cost;
            }
        }
        flows[state] = flow;
    }

    for (unsigned state = 0; state < stateCount; ++state) {
        for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
            const unsigned nodeBit = 1U << nodeIndex;
            if (state & nodeBit) continue;
            auto& pressure = pressures[state][nodeIndex];
            const int flow = flows[state];
            for (int t = 0; t < TIME; ++t) {
                pressure[t] = flow * t;
            }
            const Node& node = nodes[nodeIndex];
            for (const Edge& edge : node.edges) {
                const unsigned subNodeBit = 1U << edge.node;
                if (!(state & subNodeBit)) continue;
                const unsigned subState = state & ~subNodeBit;
                const auto& subPressure = pressures[subState][edge.node];
                for (int t = edge.cost; t < TIME; ++t) {
                    const int p = subPressure[t - edge.cost] + flow * edge.cost;
                    pressure[t] = std::max(pressure[t], p);
                }
            }
        }
    }

    int result = 0;
    const unsigned stateMask = stateCount - 1;
    for (const Edge& player1 : nodes.back().edges) {
        const unsigned player1Bit = 1U << player1.node;
        for (unsigned state2 = 0; state2 < stateCount / 2; ++state2) {
            const unsigned state1 = state2 ^ stateMask;
            if (!(state1 & player1Bit)) {
                state2 += player1Bit - 1;
                continue;
            }
            const int time1     = TIME - player1.cost;
            const int pressure1 = pressures[state1 ^ player1Bit][player1.node][time1] - flows[state1] * time1;
            for (const Edge& player2 : nodes.back().edges) {
                const unsigned player2Bit = 1U << player2.node;
                if (!(state2 & player2Bit)) continue;
                const int time2     = TIME - player2.cost;
                const int pressure2 = pressures[state2 ^ player2Bit][player2.node][time2] - flows[state2] * time2;
                result = std::max(result, pressure1 + pressure2);
            }
        }
    }
    return result;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input16.txt";
static const std::string TEST_FILE     = "inputs/test16.txt";
static const int         TEST_RESULT_1 = 1651;
static const int         TEST_RESULT_2 = 1707;

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
