#include <vector>
#include <stdexcept>
#include <memory>
#include <iostream>
class BitBor {

public:
    struct Node {
        size_t val;
        std::shared_ptr<Node> l = nullptr;
        std::shared_ptr<Node> r = nullptr;
        bool is_leaf;
        Node(uint64_t val, std::shared_ptr<Node> l, std::shared_ptr<Node> r, bool is_leaf);
    };
    std::shared_ptr<Node> AddNode(uint64_t, std::shared_ptr<Node> l, std::shared_ptr<Node> r, bool is_leaf);
    std::shared_ptr<Node> root = nullptr;
    std::shared_ptr<Node> Go(std::shared_ptr<Node> curr, bool type);
    void GetInfo(std::shared_ptr<Node> curr_node, size_t curr_length, std::vector<std::pair<size_t, size_t>>& res);
};
