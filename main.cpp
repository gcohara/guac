#include <algorithm>
#include <climits>
#include <deque>
#include <functional>
#include <iterator>
#include <ostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <map>
#include <vector>
#include <cstddef>
#include <iostream>

using Byte = unsigned char;
using CharFreqHashMap = std::unordered_map<Byte, long long int>;
using CharPriorQ = std::priority_queue<Byte, std::vector<Byte>, std::greater<Byte>>;
using CodeLenMap = std::map<int, CharPriorQ>;
using Codeword = std::deque<bool>;
using EncodingBook = std::unordered_map<Byte, Codeword>;
using DecodingBook = std::unordered_map<Codeword, Byte>;


// refactor to take references
// will need a constructor that takes two refs
// but might not be able to...
struct Node {
    long long int weight;
    std::vector<Byte> bytes;
    struct Node * left_child, * right_child;
};


struct NodeCompare {
    bool operator()(Node * n1, Node * n2) {
        return (n1->weight > n2->weight);
    }
};

using NodePriorQ = std::priority_queue<Node*, std::vector<Node*>, NodeCompare>;

void print_out_codebook(Node * parent_node, std::string codes[]);
CodeLenMap& get_code_lengths(Node * tree, int depth);
void print_node(Node * node);
bool is_leaf_node(Node * node);
CharFreqHashMap get_frequencies (std::string const & input);
NodePriorQ build_priority_queue (CharFreqHashMap const & frequencies);
Node * build_huffman_tree (NodePriorQ& pq);
EncodingBook get_codebook_for_encoding(CodeLenMap& clm);
std::vector<Byte> get_file_header(EncodingBook& ecb);
void print_encoding_codebook(EncodingBook ecb);
void increment_codeword(Codeword & cw);
std::ostream &operator<<(std::ostream &os, Codeword const &cw);



template<typename T> void print_pq(T pq);


int main(int argc, char ** argv) {
    using namespace std::string_literals;
    
    // open file
    // std::string const test = "oooh e ooh'?! ahaha ting tang walla walla bing bang"s;
    std::string const test = "aabbbbcd";
    std::cout << "Test string is " << test.length( )* 8 << " bits long.\n";

    auto frequencies = get_frequencies(test);
    
    // Print them out for checking!
    for (auto n : frequencies) {
        std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";
    }

    // Now build the priority queue up with the frequency data.
    auto pq = build_priority_queue(frequencies);
    print_pq(pq);
    
    // Build the Huffman tree from the priority queue.
    auto root = build_huffman_tree(pq);

    // Next steps:
    // Build a map indexed by code length. The values are collections of chars.
    // Then, using this map, construct the canonical Huffman tree.
    auto code_lens = get_code_lengths(root, 0);
    // for (auto n : code_lens) {
    //     std::cout << n.first << " - ";
    //     while(!empty(n.second)) {
    //         std::cout << n.second.top() << ", ";
    //         n.second.pop();
    //     }
    //     std::cout << std::endl;
    // }
    auto encoding_codebook = get_codebook_for_encoding(code_lens);
    print_encoding_codebook(encoding_codebook);
    auto fh = get_file_header(encoding_codebook);
    std::cout << fh.size() << "\n";
    for (auto itr = fh.rbegin(); itr != fh.rend(); itr++) {
        std::cout << static_cast<int>(*itr) << " ";
    }
    std::cout << std::endl;
    
}

std::ostream &operator<<(std::ostream &os, Codeword const &cw) {
    for ( auto& x : cw ) {
        os << x;
    }
    return os;
}

std::vector<Byte> get_file_header(EncodingBook& ecb) {
    std::vector<Byte> file_header {};
    for (Byte b = 0; b < UCHAR_MAX; b++) {
        if (ecb.contains(b)) {
            file_header.push_back(ecb.at(b).size());
        }
        else {
            file_header.push_back(0);
        }
    }
    return file_header;
}

EncodingBook get_codebook_for_encoding(CodeLenMap& clm) {
    EncodingBook ecb {};
    Codeword codeword {};
    for ( auto& n : clm ) {
        auto codeword_length = n.first;
        auto symbols = n.second;
        while ( codeword.size() < codeword_length ) {
                codeword.push_back(0);
            }
        while (!symbols.empty()) {
            auto current_symbol = symbols.top();
            symbols.pop();
            ecb.insert({current_symbol, codeword});
            increment_codeword(codeword);
        }
    }
    return ecb;
}


void print_encoding_codebook(EncodingBook ecb) {
    for (auto & n : ecb) {
        std::cout << n.first << " - ";
        for (auto x : n.second) {
            std::cout << x;
        }
        std::cout << std::endl;
    }
}


CodeLenMap& get_code_lengths(Node * tree, int depth) {
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

    
NodePriorQ build_priority_queue(CharFreqHashMap const & frequencies) {
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


CharFreqHashMap get_frequencies (std::string const & input) {
    CharFreqHashMap frequencies;
    for (Byte c : input) {
        frequencies.try_emplace(c, 0);
        frequencies.at(c)++;
    }
    return frequencies;
}


void increment_codeword(Codeword & cw) {
    if (cw.size() == 0) {
        cw.push_back(0);
        return;
    }
    for (auto itr = cw.rbegin(); itr != cw.rend(); ++itr) {
            if (*itr == 0) {
                *itr = 1;
                return;
            }
            else {
                *itr = 0;
            }
        }
    cw.push_front(1);
}


bool is_leaf_node(Node * node) {
    bool no_left_child = (node->left_child == nullptr);
    bool no_right_child = (node->right_child == nullptr);
    return (no_left_child && no_right_child);
}


template<typename T> void print_pq(T pq) {
    while (!pq.empty()) {
        print_node(pq.top());
        pq.pop();
    }
}


void print_node(Node * node) {
    std::cout << "Weight:[" << node->weight << "] Bytes:[";
    for (auto b : node->bytes) {
        std::cout << b << "]\n";
    }
}

