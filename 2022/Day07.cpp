#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// ********************************************************************************************************

struct Item;
using ItemPtr = std::shared_ptr<Item>;

struct Item {
    std::string          name;
    std::vector<ItemPtr> items;
    int64_t              size = 0;
};

ItemPtr parse(std::ifstream input) {
    std::string          line;
    ItemPtr              rootItem;
    std::vector<ItemPtr> currentItems;

    while (std::getline(input, line)) {
        if (line.starts_with("$ ")) {
            if (line.starts_with("$ cd ")) {
                const std::string dir = line.substr(5);
                if (dir == "/") {
                    rootItem = std::make_shared<Item>();
                    currentItems = { rootItem };
                } else if (dir == "..") {
                    currentItems.pop_back();
                } else {
                    for (const auto& item : currentItems.back()->items) {
                        if (item->name == dir) {
                            currentItems.push_back(item);
                            break;
                        }
                    }
                }
            }
        } else if (line.starts_with("dir ")) {
            ItemPtr item = std::make_shared<Item>();
            currentItems.back()->items.push_back(item);
            item->name = line.substr(4);
        } else {
            ItemPtr item = std::make_shared<Item>();
            currentItems.back()->items.push_back(item);
            std::istringstream iss(line);
            iss >> item->size >> item->name;
        }
    }
    return rootItem;
}

int64_t traverse(const ItemPtr& item, const auto& visitor) {
    if (item->items.empty()) {
        return item->size;
    }
    int64_t size = 0;
    for (const ItemPtr& i : item->items) {
        size += traverse(i, visitor);
    }
    item->size = size;
    visitor(item);
    return size;
}

int64_t solve1(std::ifstream input) {
    const ItemPtr root = parse(std::move(input));

    int64_t result = 0;
    traverse(root, [&](const auto& item) {
        if (item->size <= 100000) {
            result += item->size;
        }
        });
    return result;
}

int64_t solve2(std::ifstream input) {
    const ItemPtr root = parse(std::move(input));

    const int64_t totalSize = traverse(root, [&](const auto& i) {});
    const int64_t toFreeUp  = totalSize - 40000000;
    int64_t result = 0;
    traverse(root, [&](const auto& item) {
        if (item->size >= toFreeUp && (!result || item->size < result)) {
            result = item->size;
        }
    });
    return result;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input07.txt";
static const std::string TEST_FILE     = "inputs/test07.txt";
static const int64_t     TEST_RESULT_1 = 95437;
static const int64_t     TEST_RESULT_2 = 24933642;

int main() {
    if (solve1(std::ifstream(TEST_FILE)) == TEST_RESULT_1) {
        std::cout << solve1(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope." << std::endl;
    }
    if (solve2(std::ifstream(TEST_FILE)) == TEST_RESULT_2) {
        std::cout << solve2(std::ifstream(INPUT_FILE)) << std::endl;
    } else {
        std::cout << "Nope." << std::endl;
    }
    return 0;
}
