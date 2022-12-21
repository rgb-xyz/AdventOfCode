#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>

struct Node {
    bool hasHuman = false;

    virtual ~Node() = default;
    virtual int64_t get() = 0;
};
using NodePtr = std::shared_ptr<Node>;

struct Number : Node {
    int64_t number{};

    virtual int64_t get() { return number; }
};

struct Operator : Node {
    NodePtr left;
    NodePtr right;
    char    op{};

    virtual int64_t get() {
        if (!mResult) {
            switch (op) {
            case '+': mResult = left->get() + right->get(); break;
            case '-': mResult = left->get() - right->get(); break;
            case '*': mResult = left->get() * right->get(); break;
            case '/': mResult = left->get() / right->get(); break;
            default: std::terminate();
            }
            hasHuman = left->hasHuman || right->hasHuman;
        }
        return *mResult;
    }

private:
    std::optional<int64_t> mResult;
};

// ********************************************************************************************************

NodePtr parse(std::ifstream input) {
    std::map<std::string, NodePtr>                                           nodes;
    std::map<std::shared_ptr<Operator>, std::pair<std::string, std::string>> nameLookup;
    for (std::string line; std::getline(input, line);) {
        const std::string name = line.substr(0, 4);
        if (line.size() == 17) {
            auto node        = std::make_shared<Operator>();
            node->op         = line[11];
            nodes[name]      = node;
            nameLookup[node] = { line.substr(6, 4), line.substr(13, 4) };
        } else {
            auto node      = std::make_shared<Number>();
            node->number   = std::stoi(line.substr(6));
            node->hasHuman = (name == "humn");
            nodes[name]    = node;
        }
    }
    for (const auto& [node, names] : nameLookup) {
        node->left  = nodes[names.first];
        node->right = nodes[names.second];
    }
    return nodes["root"];
}

int64_t solve1(std::ifstream input) {
    return parse(std::move(input))->get();
}

int64_t solve2(std::ifstream input) {
    const NodePtr root = parse(std::move(input));
    // Convert equality to requirement "left - right == 0".
    dynamic_cast<Operator&>(*root).op = '-';
    root->get(); // needed to evaluate the "hasHuman" flag.
    std::pair<NodePtr, int64_t> result{ root, 0 };
    while (auto* op = dynamic_cast<Operator*>(result.first.get())) {
        NodePtr left  = op->left;
        NodePtr right = op->right;
        if (left->hasHuman && !right->hasHuman) {
            result.first = left;
            switch (op->op) {
            case '+': result.second -= right->get(); break;
            case '-': result.second += right->get(); break;
            case '*': result.second /= right->get(); break;
            case '/': result.second *= right->get(); break;
            }
        } else if (!left->hasHuman && right->hasHuman) {
            result.first = right;
            switch (op->op) {
            case '+': result.second = result.second - left->get(); break;
            case '-': result.second = left->get() - result.second; break;
            case '*': result.second = result.second / left->get(); break;
            case '/': result.second = left->get() / result.second; break;
            }
        } else {
            std::terminate();
        }
    }
    return result.second;
}

// ********************************************************************************************************

static const std::string INPUT_FILE    = "inputs/input21.txt";
static const std::string TEST_FILE     = "inputs/test21.txt";
static const int64_t     TEST_RESULT_1 = 152;
static const int64_t     TEST_RESULT_2 = 301;

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
