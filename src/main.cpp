#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>
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
#include <fstream>

#include "../include/huffmantree.hpp"
#include "../include/codebooks.hpp"

// using Byte = std::uint8_t;
// using CharFreqHashMap = std::unordered_map<Byte, long long int>;
// using CharPriorQ = std::priority_queue<Byte, std::vector<Byte>, std::greater<Byte>>;
// using CodeLenMap = std::map<int, CharPriorQ>;
// using Codeword = std::deque<bool>;
// using EncodingBook = std::unordered_map<Byte, Codeword>;
// using DecodingBook = std::unordered_map<Codeword, Byte>;
using EncodedMessage = std::deque<bool>;
using ByteVec = std::vector<Byte>;
using ByteDeq = std::deque<Byte>;


// refactor to take references
// will need a constructor that takes two refs
// but might not be able to...
struct Node;

void print_node(Node * node);
CharFreqHashMap get_frequencies (std::string const & input);

std::deque<Byte> get_file_header(EncodingBook& ecb);


EncodedMessage encode_message(std::string const& message, EncodingBook& ecb);
std::ostream &operator<<(std::ostream &os, Codeword const &cw);
std::vector<Byte> convert_encoded_message_to_bytes(EncodedMessage em);
void write_out_to_file(ByteDeq file_header, ByteVec encoded_message);

template<typename T> void print_pq(T pq);

int main(int argc, char ** argv) {
    using namespace std::string_literals;
    
    // open file
    // std::string const test = "oooh e ooh'?! ahaha ting tang walla walla bing bang"s;
    std::string const test = "aabbbbcdaabbbbcdaabbbbcdaabbbbcdaabbbbcdaabbbbcdaabbbbcdaabbbbcd";
    auto frequencies = get_frequencies(test);
    // Print them out for checking!
    for (auto n : frequencies) {
        std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";
    }
    auto code_lens = Huffman::get_codeword_lengths(frequencies);

    auto encoding_codebook = Codebooks::get_codebook_for_encoding(code_lens);

    auto fh = get_file_header(encoding_codebook);
    auto encoded_message = encode_message(test, encoding_codebook);
    auto bm = convert_encoded_message_to_bytes(encoded_message);
    write_out_to_file(fh, bm);
}

void write_out_to_file(ByteDeq file_header, ByteVec encoded_message) {
    std::ofstream ofs {"test.guac"};
    if ( !ofs ) {
        std::cout << "Could not create output file.\n";
        exit(EXIT_FAILURE);
    }
    for ( auto b : file_header ) {
        ofs << b;
    }
    for ( auto b : encoded_message ) {
        ofs << b;
    }
    ofs.close();
}

// Important to note that the last byte is the number of unused trailing bits in
// the penultimate byte.
ByteVec convert_encoded_message_to_bytes(EncodedMessage em) {
    std::vector<Byte> byte_message {};
    while ( em.size() > 8 ) {
        Byte temp = 0;
        for (int i = 0; i < 8; i++) {
            temp <<= 1;
            temp +=em.front();
            em.pop_front();
        }
        byte_message.push_back(temp);
    }
    Byte loose_bits = 8;
    if ( em.size() == 0 ) {
        loose_bits = 0;
    }
    Byte temp = 0;
    while ( em.size() != 0 ) {
        temp<<=1;
        temp +=em.front();
        em.pop_front();
        loose_bits--;
    }
    temp <<= loose_bits;
    byte_message.push_back(temp);
    byte_message.push_back(loose_bits);
    return byte_message;
}

std::ostream &operator<<(std::ostream &os, Codeword const &cw) {
    for ( auto& x : cw ) {
        os << x;
    }
    return os;
}

EncodedMessage encode_message(std::string const& message, EncodingBook& ecb) {
    EncodedMessage enc_m {};
    for ( auto c : message ) {
        auto cw = ecb.at(c);
        enc_m.insert(enc_m.end(), cw.begin(), cw.end());
    }
    return enc_m;
}

std::deque<Byte> get_file_header(EncodingBook& ecb) {
    std::deque<Byte> file_header {};
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

CharFreqHashMap get_frequencies (std::string const & input) {
    CharFreqHashMap frequencies;
    for (Byte c : input) {
        frequencies.try_emplace(c, 0);
        frequencies.at(c)++;
    }
    return frequencies;
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

