#include "../include/huffmantree.hpp"
#include <iostream>

// Huffman tree node
struct Node {
    long long int weight;
    std::vector<Byte> bytes;
    struct Node * left_child;
    struct Node * right_child;
};

// Comparison overload, required for priority queue
struct NodeCompare {
    bool operator()(Node * n1, Node * n2) {
        return (n1->weight > n2->weight);
    }
};

using NodePriorQ = std::priority_queue<Node*, std::vector<Node*>, NodeCompare>;

namespace {
    // Builds a priority queue of Nodes, which is used in building the Huffman
    // tree
    NodePriorQ build_priority_queue (ByteFreqHashMap const & frequencies);
    // Builds the Huffman tree from the priority queue. Consumes the priority queue.
    Node * build_huffman_tree (NodePriorQ& pq);
    // Tests whether a node is a terminal/leaf node or one with children.
    bool is_leaf_node(Node * node);
    // Gets the lengths of each symbol's codeword
    CodeLenMap& get_code_lengths(Node * tree, int depth);
    // Print functions for debugging
    void print_pq(NodePriorQ);
    void print_ht(Node*);
}

// The public interface to this file.
// Given the frequencies of each character in the input file, returns a map
// where the keys are characters, and the value is the length of the character's
// Huffman codeword.
CodeLenMap Huffman::get_codeword_lengths(ByteFreqHashMap const & char_freqs) {
    auto pq = build_priority_queue(char_freqs);
    auto ht_root = build_huffman_tree(pq);
    auto cw_lens = get_code_lengths(ht_root, 0);
    return cw_lens;
}

namespace {    
    CodeLenMap& get_code_lengths(Node * tree, int depth) {
        // returns a reference since we have static lifetime
        static CodeLenMap  code_length_map;
        if (is_leaf_node(tree)) {
            code_length_map.try_emplace(depth, CharPriorQ {});
            code_length_map.at(depth).push(tree->bytes[0]);
        }
        else {
            if (tree->left_child != nullptr) {
                get_code_lengths(tree->left_child, depth + 1);
            }
            if (tree->right_child != nullptr) {
                get_code_lengths(tree->right_child, depth + 1);
            }
        }
        return code_length_map;
    }

    bool is_leaf_node(Node * node) {
        bool no_left_child = (node->left_child == nullptr);
        bool no_right_child = (node->right_child == nullptr);
        return (no_left_child && no_right_child);
    }

    NodePriorQ build_priority_queue(ByteFreqHashMap const & frequencies) {
        NodePriorQ pq;
        for (auto n : frequencies) {
            Node * temp = new Node;
            temp->left_child = nullptr;
            temp->right_child = nullptr;
            temp->weight = n.second;
            temp->bytes.push_back(n.first);
            pq.push(temp);
        }
        return pq;
    }

    Node * build_huffman_tree(NodePriorQ & pq) {
        while (pq.size() > 1) {
            Node * parent = new Node;
            Node * left_child = pq.top();
            pq.pop();
            Node * right_child = pq.top();
            pq.pop();
            parent->weight = left_child->weight + right_child->weight;
            parent->bytes = left_child->bytes;
            parent->bytes.insert (parent->bytes.cend(),
                                  right_child->bytes.cbegin(),
                                  right_child->bytes.cend());
            parent->left_child = left_child;
            parent->right_child = right_child;
            pq.push(parent);
        }
        return pq.top();
    }

    void print_ht(Node * ht) {
        if (is_leaf_node(ht)) {
            std::cout << "With weight " << ht->weight << " we have:\n";
            for (auto x : ht->bytes) {
                std::cout << static_cast<int>(x) << ", ";
            }
            std::cout << std::endl;
        }
        else {
            print_ht(ht->left_child);
            print_ht(ht->right_child);
        }
    }
    
    void print_pq(NodePriorQ pq) {
        while (!pq.empty()) {
            auto top = pq.top();
            std::cout << "With weight " << top->weight << " we have:\n";
            for (auto x : top->bytes) {
                std::cout << static_cast<int>(x) << ", ";
            }
            std::cout << std::endl;
            pq.pop();
        }
    }

    void print_node(Node * n) {
        std::cout << "Printing node with address " << n << "\n";
        std::cout << "With weight " << n->weight << " we have:\n";
        for (auto x : n->bytes) {
            std::cout << static_cast<int>(x) << ", ";
        }
        std::cout << std::endl;
    }
}
