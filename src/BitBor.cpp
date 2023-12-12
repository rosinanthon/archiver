#include "BitBor.h"
BitBor::Node::Node(uint64_t val1, std::shared_ptr<Node> l1, std::shared_ptr<Node> r1, bool is_leaf1) {
    val = val1;
    l = l1;
    r = r1;
    is_leaf = is_leaf1;
    if (l) {
        val = std::min(val, l->val);
    }
    if (r) {
        val = std::min(val, r->val);
    }
}
std::shared_ptr<BitBor::Node> BitBor::AddNode(uint64_t val1, std::shared_ptr<Node> l1, std::shared_ptr<Node> r1,
                                              bool is_leaf1) {
    return std::make_shared<Node>(val1, l1, r1, is_leaf1);
}
std::shared_ptr<BitBor::Node> BitBor::Go(std::shared_ptr<Node> curr, bool type) {
    if (curr == nullptr) {
        throw std::out_of_range("Cannot call function from nullptr object");
    }
    if (!type) {
        return curr->l;
    } else {
        return curr->r;
    }
}
void BitBor::GetInfo(std::shared_ptr<Node> curr_node, size_t curr_length, std::vector<std::pair<size_t, size_t>>& res) {
    if (curr_node == nullptr) {
        throw std::out_of_range("Cannot call function from nullptr object");
    }
    if (curr_node->is_leaf) {
        res.push_back({curr_length, curr_node->val});
    } else {
        GetInfo(curr_node->l, curr_length + 1, res);
        GetInfo(curr_node->r, curr_length + 1, res);
    }
}
