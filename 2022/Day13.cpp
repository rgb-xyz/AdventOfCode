#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Packet {
    std::vector<Packet> items;
    int                 value = -1;

    Packet() = default;
    explicit Packet(const std::string_view& str) {
        parse(str.data(), str.data() + str.size());
    }

    bool isList() const { return !items.empty() || value < 0; }

    const char* parse(const char* from, const char* end) {
        if (*from == '[') {
            ++from;
            while (*from != ']') {
                items.emplace_back();
                from = items.back().parse(from, end);
                if (*from == ',') {
                    ++from;
                }
            }
            return from + 1;
        } else {
            return std::from_chars(from, end, value).ptr;
        }
    }

    std::weak_ordering operator<=>(const Packet& other) const {
        if (isList() && other.isList()) {
            return items <=> other.items;
        }
        if (!isList() && !other.isList()) {
            return value <=> other.value;
        }
        Packet listPacket;
        if (!isList()) {
            listPacket.items.push_back(*this);
            return listPacket <=> other;
        } else {
            listPacket.items.push_back(other);
            return *this <=> listPacket;
        }
    }
};

// ********************************************************************************************************

int solve1(std::ifstream input) {
    int result   = 0;
    int sequence = 0;
    for (std::string left, right; std::getline(input, left) && std::getline(input, right); input.get()) {
        ++sequence;
        if (Packet(left) < Packet(right)) {
            result += sequence;
        }
    }
    return result;
}

int solve2(std::ifstream input) {
    std::vector<Packet> packets;
    for (std::string left, right; std::getline(input, left) && std::getline(input, right); input.get()) {
        packets.emplace_back(left);
        packets.emplace_back(right);
    }
    const Packet packet1 = packets.emplace_back("[[2]]");
    const Packet packet2 = packets.emplace_back("[[6]]");
    std::sort(packets.begin(), packets.end());
    const auto packetIt1 = std::lower_bound(packets.begin(), packets.end(), packet1);
    const auto packetIt2 = std::lower_bound(packets.begin(), packets.end(), packet2);
    return int(packetIt1 - packets.begin() + 1) * int(packetIt2 - packets.begin() + 1);
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input13.txt";
static const std::string TEST_FILE     = "inputs/test13.txt";
static const int         TEST_RESULT_1 = 13;
static const int         TEST_RESULT_2 = 140;

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
