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
#include "../include/file_interface.hpp"
#include "../include/decompression_interface.hpp"

void print_pq(CharPriorQ pq);
void print_cw_lens(CodeLenMap clm);

int main(int argc, char ** argv) {
    using namespace std::string_literals;

    if (argv[2] == "--compress"s) {
        FilePath input_file {argv[1]};
        FilePath output_file {argv[1] + ".guac"s};
        auto frequencies = FileInterface::get_frequencies(input_file);
        // Print them out for checking!
        for (auto n : frequencies) {
            std::cout << "Key:[" << static_cast<int>(n.first) << "] Value:["
                      << n.second << "]\n";
        }
        auto code_lens = Huffman::get_codeword_lengths(frequencies);
        print_cw_lens(code_lens);
        auto encoding_codebook = Codebooks::codebook_for_encoding(code_lens);
        

        FileInterface::compress_file(input_file, encoding_codebook);
    }
    else if (argv[2] == "--decompress"s) {
        FilePath input_file {argv[1]};
        auto code_lens = Decompress::codeword_lengths_from_file(input_file);
        auto decoding_codebook = Codebooks::codebook_for_decoding(code_lens);
        Decompress::decompress_file(input_file, decoding_codebook);
    }
}

void print_cw_lens(CodeLenMap clm) {
    for (auto& n : clm) {
        std::cout << "Of length " << n.first << ":\n";
        print_pq(n.second);
    }
}

void print_pq(CharPriorQ pq){
    while (!pq.empty()) {
        std::cout << static_cast<int>(pq.top()) << ", ";
        pq.pop();
    }
    std::cout << "\n";
}
